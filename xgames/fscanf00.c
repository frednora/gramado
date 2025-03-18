// fscanf implementation.
// This is a test. Created by Grok.

/*
Creating a simplified implementation of fscanf for your own custom libc is an ambitious task, 
as the real fscanf handles a wide variety of format specifiers, edge cases, and file I/O intricacies. 
However, I can provide a basic version that focuses on reading floating-point values (e.g., %f) 
from a text file. This implementation will be minimal, 
handling only basic decimal numbers (e.g., "123.45" or "-67.89"), and 
it won’t support the full range of fscanf features like multiple format specifiers, 
whitespace handling beyond simplicity, or error reporting as robustly as the standard library.
Here’s a simple implementation:
*/

#include <stdio.h> // For FILE type; in a real libc, you'd define this yourself

// Custom fscanf-like function to read a float
int my_fscanf(FILE *stream, const char *format, void *ptr) {
    if (stream == NULL || format == NULL || ptr == NULL) {
        return -1; // Error: invalid arguments
    }

    // For simplicity, we only handle "%f" format
    if (format[0] != '%' || format[1] != 'f' || format[2] != '\0') {
        return -1; // Unsupported format
    }

    float *result = (float *)ptr; // Cast the void pointer to float pointer
    float value = 0.0f;
    int sign = 1;
    int c;

    // Skip leading whitespace
    while ((c = fgetc(stream)) != EOF && (c == ' ' || c == '\t' || c == '\n'));

    if (c == EOF) {
        return 0; // No items read
    }

    // Handle sign
    if (c == '-') {
        sign = -1;
        c = fgetc(stream);
    } else if (c == '+') {
        c = fgetc(stream);
    }

    // Read integer part
    int has_digits = 0;
    while (c >= '0' && c <= '9') {
        value = value * 10.0f + (c - '0');
        has_digits = 1;
        c = fgetc(stream);
    }

    // Read decimal part if present
    if (c == '.') {
        float fraction = 0.1f;
        c = fgetc(stream);
        while (c >= '0' && c <= '9') {
            value += (c - '0') * fraction;
            fraction *= 0.1f;
            has_digits = 1;
            c = fgetc(stream);
        }
    }

    // If no valid digits were read, return failure
    if (!has_digits) {
        ungetc(c, stream); // Put back the last character
        return 0;
    }

    // Apply sign and store the result
    *result = value * sign;

    // Put back the last non-number character
    if (c != EOF) {
        ungetc(c, stream);
    }

    return 1; // Successfully read one item
}

// Example usage
int main() {
    FILE *file = fopen("test.txt", "r");
    if (!file) {
        printf("Error opening file!\n");
        return 1;
    }

    float value;
    int result = my_fscanf(file, "%f", &value);
    if (result == 1) {
        printf("Read float: %f\n", value);
    } else {
        printf("Failed to read float, result: %d\n", result);
    }

    fclose(file);
    return 0;
}

// =====================================================

/*
Explanation of the Implementation
Function Signature:
my_fscanf(FILE *stream, const char *format, void *ptr) mimics the basic structure of fscanf.

It uses a void * pointer to allow passing the address of a float, which is then cast internally.

Input Validation:
Checks for NULL pointers and ensures the format is exactly "%f". This keeps it simple.

Reading Logic:
Skips leading whitespace (space, tab, newline).

Handles an optional sign (+ or -).

Parses the integer part by accumulating digits.

If a decimal point (.) is found, processes the fractional part by scaling each digit appropriately.

Uses fgetc to read characters one by one and ungetc to push back the last non-numeric character.

Return Value:
Returns 1 if a float is successfully read (mimicking fscanf’s count of assigned items).

Returns 0 if no valid number is found.

Returns -1 for invalid inputs or unsupported formats.

Limitations:
Only supports %f (no %lf, %d, etc.).

No scientific notation (e.g., "1.23e-4").

Minimal error handling (e.g., doesn’t distinguish between EOF and invalid input well).

Assumes FILE and fgetc/ungetc are available (in a real libc, you’d implement these too).

Testing
Create a test.txt file with content like:

3.14

Run the program, and it should output:

Read float: 3.140000

Expanding for Your libc
To make this part of a full custom libc, you’d need to:
Replace FILE with your own stream structure.

Implement fgetc and ungetc equivalents if they don’t exist.

Add support for more format specifiers (%d, %lf, %s, etc.).

Improve error handling and edge cases (overflow, malformed input).

This is a starting point—let me know if you want to refine it further!
*/




