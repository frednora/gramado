// #test
// Trying to scan float values from a string.


int customSscanf(const char* str, const char* format, float* value);
int testScanFloat(void);


// =====================


int customSscanf(const char* str, const char* format, float* value) 
{
    if (str == NULL || format == NULL || value == NULL) {
        return 0;
    }

    // Check if the format matches "%f"
    if (format[0] != '%' || format[1] != 'f' || format[2] != '\0') {
        return 0; // Invalid format
    }

    // Try to parse the float value from the string
    const char* current = str;
    while (*current && isspace(*current)) {
        current++; // Skip leading whitespace
    }

    char* endPtr;
    float tempValue = strtof(current, &endPtr);
    if (current == endPtr) {
        return 0; // No valid float found
    }

    *value = tempValue;
    return 1; // Successfully parsed one float
}

int testScanFloat(void) 
{
    const char* inputString = "  42.0   3.14";
    const char* format = "%f";
    float value;

    const char* current = inputString;
    while ( customSscanf(current, format, &value) ) 
    {
        // #bugbug
        // Our printf is not able to print float yet.
        printf("Found float: %f\n", value);

        // Move past the parsed float and any surrounding whitespace
        while (*current && isspace(*current)) {
            current++;
        }
        while (*current && !isspace(*current)) {
            current++;
        }
    }

    return 0;
}




