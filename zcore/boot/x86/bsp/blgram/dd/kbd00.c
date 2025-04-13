
/*
The provided keyboard.c file contains three tables: map[], shift[], and ctl[], which define the key mappings. These mappings appear to be based on a US keyboard layout, but you want them to comply with the ABNT2 standard for Brazilian keyboards.

To ensure compliance with ABNT2, I need to adjust the mappings in the map[], shift[], and ctl[] arrays. Key differences in ABNT2 include:

Presence of the "ç" key.
Special characters like "´" (acute), "~" (tilde), "`" (grave), "^" (circumflex), etc., in specific positions.
Adjustments in alphanumeric keys to align with the layout.
I'll analyze and fix the mappings in these tables for ABNT2 compliance.

Adjustments for ABNT2 Compliance
Below are the suggested fixes for the map[], shift[], and ctl[] tables. These corrections are based on the standard layout of the ABNT2 keyboard.

Corrected map[] Table (No Shift)
The following adjustments correct the basic key mappings:
*/

/*
static unsigned char map[128] = {
    // ...
    'ç', // scancode for 'ç'
    '\'', // scancode for "'"
    // Replace '[' with '´' and ']' with '`' for ABNT2
    '´', // acute accent
    '`', // grave accent
    // ...
};
*/

/*
Corrected shift[] Table (Shift Pressed)
With Shift pressed, additional symbols like uppercase letters and special characters are included:
*/

/*
static unsigned char shift[128] = {
    // ...
    'Ç', // scancode for 'Ç'
    '\"', // scancode for '"'
    // Replace '[' with '^' and ']' with '~' for ABNT2
    '^', // circumflex accent
    '~', // tilde
    // ...
};
*/

/*
Corrected ctl[] Table (Control Pressed)
For Control, the table should map to control characters or other functionalities:
*/

/*
static unsigned char ctl[128] = {
    // ...
    0, // Control combinations for 'ç' (not standard, likely unused)
    0, // Control combinations for '´' (normally unused)
    // Replace '[' and ']' control mappings with ABNT2 alternatives
    0, // Control combinations for '^'
    0, // Control combinations for '~'
    // ...
};
*/

/*
Implementation Steps
Replace map[], shift[], and ctl[] tables with the corrected mappings above.
Test the updated keyboard handler in the bootloader to ensure all keys match the expected output for the ABNT2 layout.
Validate special keys like "ç", "`", and "´" in various combinations (with/without Shift and Control).
*/

===================================================================
// abnt2

Here are the ABNT2 (Brazilian) keyboard mappings in table format for reference. 
These tables provide the mappings for regular (no shift), Shift, and 
Ctrl key states. They are based on the ABNT2 standard keyboard layout.


ABNT2 Keyboard - Regular (No Shift) Key Mappings (map[])
static unsigned char map[128] = {
    0,    27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\'',  // 0-12
    '`',  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',  '[',  // 13-25
    ']',  '\n',  0,   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',  'ç',  // 26-38
    '~',  '^',  0,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',',  '.',  // 39-51
    ';',  0,    0,   ' ',  0    // 52-56
};


ABNT2 Keyboard - Shift Key Mappings (shift[])
static unsigned char shift[128] = {
    0,    27,  '!', '@', '#', '$', '%', '¨', '&', '*', '(', ')', '"',   // 0-12
    '~',  '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{',  // 13-25
    '}',  '\n',  0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',  'Ç',  // 26-38
    '`',  '^',  0,   '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<',  '>',  // 39-51
    ':',  0,    0,   ' ',  0    // 52-56
};


ABNT2 Keyboard - Ctrl Key Mappings (ctl[])
static unsigned char ctl[128] = {
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    // 0-12
    0,    0,    0x11, 0x17, 0x05, 0x12, 0x14, 0x19, 0x15, 0x09, 0x0F, 0x10, 0x1B, // 13-25
    0x1D, 0x0A, 0,    0x01, 0x13, 0x04, 0x06, 0x07, 0x08, 0x0A, 0x0B, 0x0C, 0,    // 26-38
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    // 39-51
    0,    0,    0,    0,    0    // 52-56
};

============================================================

/*
Key Changes for ABNT2
Regular (No Shift):

'ç' replaces ''` (tilde/accent key).
'[' and ']' become '´' (acute accent) and ''` (grave accent), respectively.
'~' (tilde) and '^' (circumflex) are introduced.
Shift:

'Ç' (uppercase Ç).
'[' and ']' become '{' and '}', respectively.
'~' and '^' are swapped with their shifted versions.
Ctrl:

Control mappings are generally less used but can be adjusted if specific control sequences are required.
*/

====================================================

/*
The ABNT2 keyboard standard (short for Associação Brasileira de Normas Técnicas, model 2) is the Brazilian keyboard layout specifically designed to accommodate the Portuguese language. It includes unique features and key placements tailored to Brazilian Portuguese, such as diacritical marks and other symbols used in the language.

Key Features of the ABNT2 Keyboard Layout:
"Ç" Key:

A dedicated key for the letter "ç" is located to the right of the "L" key.
This character is essential in Portuguese and one of the defining features of the ABNT2 layout.
Dead Keys for Accents:

The ABNT2 layout includes "dead keys" for adding diacritical marks to vowels, such as acute (´), grave (`), tilde (~), circumflex (^), and diaeresis (¨).
These keys allow users to type accented characters like "á", "é", "í", "ó", "ú", "ã", "õ", and "ü".
"AltGr" Key:

The "AltGr" key (to the right of the spacebar) allows access to additional symbols and characters, such as:
"²" (superscript 2)
"³" (superscript 3)
"§" (section symbol)
"ª" (feminine ordinal indicator)
"º" (masculine ordinal indicator)
Special Symbol Positions:

The "´" (acute accent) key replaces the US layout's "`" (grave accent) key.
The "~" (tilde) key is present on the same key as "`" (grave accent) but requires the "Shift" modifier.
The "" (backslash) key is located next to the "Enter" key, instead of being on the top-left corner like in the US layout.
Additional Keys:

The ABNT2 keyboard includes an extra key between the left "Shift" and "Z" key for "<" and ">" symbols.
This is often referred to as the "ISO" key and is absent on US keyboards.
Numeric Keypad Adjustments:

The numeric keypad features a comma (,) instead of a period (.) as the decimal separator, which aligns with Brazilian conventions.
*/

/*
Visual Layout Comparison:
Below is a comparison of key differences between ABNT2 and US layouts:

Key	ABNT2 Location	US Location
Ç	Right of "L"	Not present
´ (acute)	Left of "Backspace"	Replaced by grave
~ (tilde)	Same key as acute; requires "Shift"	Dedicated key
\ (backslash)	Left of "Enter"	Top-left corner
< > (less/greater)	Between "Shift" and "Z"	Not present
, (decimal)	Numeric keypad	. (period)

*/

/*
Benefits of the ABNT2 Layout:
Optimized for Brazilian Portuguese, making it easier to type common characters and symbols.
Dedicated keys for frequently used accents and punctuation in the Portuguese language.
Preserves layout compatibility for Latin-based alphabets while adding support for unique Portuguese characters.
*/
