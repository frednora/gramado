
// Let's print a float value using printf();
// Using Copilot to understand what we need to do.


To add support for printing floating-point values (%f) to the kinguio_printf() function, you will need to modify the worker function kinguio_vsprintf() to handle the %f format specifier. This involves converting floating-point numbers into strings and formatting them appropriately.

Here’s how you can proceed step by step:

------------------------------
Step 1: Modify kinguio_vsprintf()
Add logic to handle %f in the switch statement for format specifiers.

case 'f': {
    double d = va_arg(ap, double); // Fetch the floating-point value
    char buffer[64]; // Buffer to store the formatted float
    int precision = 6; // Default precision for floating-point values is 6

    // Format the float into the buffer
    snprintf(buffer, sizeof(buffer), "%.*f", precision, d);

    // Append the formatted float to the output
    str_tmp = _vsputs_r(str_tmp, buffer);
    break;
}

-----------------------
Step 2: Update the Buffer Printing Helper _vsputs_r()
The _vsputs_r() helper function already appends a string to the output. Ensure that it has no limitations on appending formatted strings.

static char *_vsputs_r(char *dest, char *src) {
    unsigned char *usrc  = (unsigned char *) src;
    unsigned char *udest = (unsigned char *) dest;
    while (*usrc) {
        *udest++ = *usrc++;
    }
    return (char *) udest;
}


------------------------------
//Step 3: Update kinguio_printf()
// The kinguio_printf() function doesn't need much change since it already uses kinguio_vsprintf() for the heavy lifting. It should automatically support the %f specifier after the changes.

------------------------------
//Step 4: Test the Changes
//Write test cases to verify the updated functionality. For example:

void test_float_support() {
    char buffer[128];
    kinguio_printf("Testing floats: %.2f, %.4f, %.0f\n", 3.14159, 2.71828, 42.0);
    kinguio_vsprintf(buffer, "Buffer test: %f %f\n", 1.23, 4.56);
    kinguio_puts(buffer);
}

------------------------------
Step 5: Further Considerations
Precision Handling: Add support for specifying precision dynamically (e.g., "%.2f") by parsing the format string in kinguio_vsprintf().
Edge Cases: Handle special values like NaN, +Inf, and -Inf.
Buffer Size: Ensure the buffer size is sufficient for large floating-point values.
Localization: Consider handling the decimal separator based on locale settings (e.g., . vs ,).

------------------------------
Final Updated kinguio_vsprintf() Code
Here’s the complete updated switch block for your kinguio_vsprintf():

switch (fmt[index]) {
    case 'c':
        c = (char) va_arg(ap, int);
        *_c_r = c;
        str_tmp = _vsputs_r(str_tmp, _c_r);
        break;

    case 's':
    case 'S':
        s = va_arg(ap, char*);
        if ((void*) s != NULL) {
            str_tmp = _vsputs_r(str_tmp, s);
        } else {
            str_tmp = _vsputs_r(str_tmp, "(null)");
        }
        break;

    case 'd':
    case 'i':
        d = va_arg(ap, int);
        kinguio_itoa(d, buffer);
        str_tmp = _vsputs_r(str_tmp, buffer);
        break;

    case 'u':
        u = va_arg(ap, unsigned int);
        kinguio_utoa(u, buffer, 10);
        str_tmp = _vsputs_r(str_tmp, buffer);
        break;

    case 'x':
    case 'X':
        d = va_arg(ap, int);
        kinguio_i2hex(d, buffer, 8);
        str_tmp = _vsputs_r(str_tmp, buffer);
        break;

    case 'f': {
        double d = va_arg(ap, double);
        char buffer[64];
        int precision = 6; // Default precision
        snprintf(buffer, sizeof(buffer), "%.*f", precision, d);
        str_tmp = _vsputs_r(str_tmp, buffer);
        break;
    }

    default:
        *_c_r = fmt[index];
        str_tmp = _vsputs_r(str_tmp, _c_r);
        break;
}

=====================================================

After implementing these changes, the kinguio_printf() function will be able to handle %f format specifiers and print floating-point values.


=======================================================

Here is the updated version of the kinguio_vsprintf() function, now including support for the "%f" format specifier to handle floating-point values:
Updated kinguio_vsprintf()

int kinguio_vsprintf(char *str, const char *fmt, va_list ap)
{
    register int index = 0;
    char c = 0;  // Character
    char *s;     // String
    char *str_tmp = str;
    char _c_r[] = "\0\0";
    char buffer[256];

    // Integer values
    int d = 0;
    unsigned int u = 0;

    // Floating-point
    double f = 0.0;

    // 64-bit support
    unsigned long lu = 0;
    int type64bit = FALSE;

    // Loop through the format string
    while (fmt[index])
    {
        switch (fmt[index])
        {
        case '%':
            ++index;

            // Check for 64-bit specifier
            if (fmt[index] == 'l')
            {
                ++index;
                type64bit = TRUE;
            }

            switch (fmt[index])
            {
            // Character
            case 'c':
                c = (char)va_arg(ap, int);
                *_c_r = c;
                str_tmp = _vsputs_r(str_tmp, _c_r);
                break;

            // String
            case 's':
            case 'S':
                s = va_arg(ap, char *);
                if ((void *)s != NULL)
                {
                    str_tmp = _vsputs_r(str_tmp, s);
                }
                else
                {
                    str_tmp = _vsputs_r(str_tmp, "(null)");
                }
                break;

            // Integer
            case 'd':
            case 'i':
                d = va_arg(ap, int);
                kinguio_itoa(d, buffer);
                str_tmp = _vsputs_r(str_tmp, buffer);
                break;

            // Unsigned Integer
            case 'u':
                if (type64bit == TRUE)
                {
                    lu = va_arg(ap, unsigned long);
                    kinguio_utoa(lu, buffer, 10);
                    str_tmp = _vsputs_r(str_tmp, buffer);
                    type64bit = FALSE;
                }
                else
                {
                    u = va_arg(ap, unsigned int);
                    kinguio_itoa(u, buffer);
                    str_tmp = _vsputs_r(str_tmp, buffer);
                }
                break;

            // Hexadecimal
            case 'x':
            case 'X':
                if (type64bit == TRUE)
                {
                    lu = va_arg(ap, unsigned long);
                    kinguio_utoa(lu, buffer, 16);
                    str_tmp = _vsputs_r(str_tmp, buffer);
                    type64bit = FALSE;
                }
                else
                {
                    d = va_arg(ap, int);
                    kinguio_i2hex(d, buffer, 8);
                    str_tmp = _vsputs_r(str_tmp, buffer);
                }
                break;

            // Floating-point
            case 'f': {
                f = va_arg(ap, double); // Fetch the floating-point value
                int precision = 6;     // Default precision for floating-point values
                // #bugbug: This strinfg is not good.
                snprintf(buffer, sizeof(buffer), "%.*f", precision, f);
                str_tmp = _vsputs_r(str_tmp, buffer);
                break;
            }

            default:
                *_c_r = fmt[index];
                str_tmp = _vsputs_r(str_tmp, _c_r);
                break;
            }
            break;

        default:
            *_c_r = fmt[index];
            str_tmp = _vsputs_r(str_tmp, _c_r);
            break;
        }

        ++index;
    }

    // Null-terminate the result string
    *str_tmp = '\0';

    // Return the length of the formatted string
    return (int)((long)str_tmp - (long)str);
}





