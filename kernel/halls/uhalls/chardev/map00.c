/*
The `kbdmap.c` file contains mappings for the ABNT2 keyboard standard. 
Specifically, it defines three character maps:

1. **`map_abnt2[]`**: For lowercase (normal typing without modifiers like Shift or Ctrl).
2. **`shift_abnt2[]`**: For uppercase (typing with the Shift key or Caps Lock).
3. **`ctl_abnt2[]`**: For control mappings (typing with the Ctrl key).

I will analyze and provide corrections for each table to ensure strict adherence to the ABNT2 standard. 
*/

/*
---
### Observations and Fixes for `map_abnt2[]` (Lowercase)

- **Observation**: The `map_abnt2[]` table generally looks correct for lowercase mappings, but there are some inconsistencies or missing characters:
  - `239` (commented as `$`) for scancode 41 should be replaced with `'´'` (acute accent).
  - `135` (commented as `ç`) for scancode 54 is correct but should be documented as such.
  - `~` for scancode 40 is correct as the tilde key.
  - Ensure all special characters like brackets, accents, and punctuations match the ABNT2 layout.
*/

/*
---
### Observations and Fixes for `shift_abnt2[]` (Uppercase)

- **Observation**: This table represents the mappings when Shift or Caps Lock is active. Issues noted:
  - `186` (grave accent) for scancode 26 might need adjustment; this should be `'`'` (grave accent).
  - `128` (commented as `Ç`) for scancode 39 is correct but should be documented as uppercase "Ç".
  - Ensure bracket mappings (`{` and `}`) and other symbols like `;`, `:`, `<`, `>`, and `?` align with ABNT2 standards.
*/

/*
---
### Observations and Fixes for `ctl_abnt2[]` (Control)

- **Observation**: This table maps control key combinations. Issues noted:
  - ASCII control characters (e.g., `^A`, `^W`, etc.) are generally correct.
  - Many positions in the table are marked as `0` and may not have mappings specified. This is fine unless required by your OS.
*/

//---
//### Suggested Changes for Proper ABNT2 Compliance

/*
#### Corrected `map_abnt2[]`:
```c
unsigned char map_abnt2[ABNT2_CHARMAP_SIZE] = {
    0,  033,  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',  '-',  '=',  VK_BACK, VK_TAB,  // 0-15
    'q', 'w',  'e',  'r',  't',  'y',  'u',  'i',  'o',  'p',  '´',  '[',  VK_RETURN, VK_LCONTROL,  // 16-29
    'a', 's',  'd',  'f',  'g',  'h',  'j',  'k',  'l',  'ç',  '~',  '\'', VK_LSHIFT, ']',  // 30-43
    'z', 'x',  'c',  'v',  'b',  'n',  'm',  ',',  '.',  ';',  VK_RSHIFT, '*',  VK_ALTGR, ' ',  // 44-57
    // Remaining keys...
};
```
*/

/*
#### Corrected `shift_abnt2[]`:
```c
unsigned char shift_abnt2[ABNT2_CHARMAP_SIZE] = {
    0,  033,  '!',  '@',  '#',  '$',  '%',  '¨',  '&',  '*',  '(',  ')',  '_',  '+',  VK_BACK, VK_TAB,  // 0-15
    'Q', 'W',  'E',  'R',  'T',  'Y',  'U',  'I',  'O',  'P',  '`',  '{',  VK_RETURN, VK_LCONTROL,  // 16-29
    'A', 'S',  'D',  'F',  'G',  'H',  'J',  'K',  'L',  'Ç',  '^',  '"', VK_LSHIFT, '}',  // 30-43
    'Z', 'X',  'C',  'V',  'B',  'N',  'M',  '<',  '>',  ':',  VK_RSHIFT, '*',  VK_ALTGR, ' ',  // 44-57
    // Remaining keys...
};
```
*/

/*
#### Corrected `ctl_abnt2[]`:
```c
unsigned char ctl_abnt2[ABNT2_CHARMAP_SIZE] = {
    0,  033,  '!',  0,    '#',  '$',  '%',  036,  '&',  '*',  '(',  ')',  ASCII_US, '+',  034, '\177',  // 0-15
    ASCII_DC1, ASCII_ETB, ASCII_ENQ, ASCII_SO, ASCII_DC4, ASCII_EM, ASCII_NAK, ASCII_HT, ASCII_SI, ASCII_DLE,  // 16-25
    033, ASCII_ESC, '\r', CTL, ASCII_SOH, ASCII_DC3, ASCII_EOT, ASCII_ACK,  // 26-33
    ASCII_BEL, ASCII_BS, ASCII_LF, ASCII_VT, ASCII_FF, ';', ASCII_RS, '`',  // 34-41
    SHF, ASCII_GS, ASCII_SUB, ASCII_CAN, ASCII_ETX, ASCII_SYN, ASCII_STX, ASCII_SO,  // 42-49
    ASCII_CR, '<', '>', '?', SHF, '*', ALT, ' ',  // 50-57
    // Remaining keys...
};
```
*/

/*
---
### Next Steps
1. Replace the mappings in your kernel's `kbdmap.c` file with the corrected tables above.
2. Test the mappings in your OS to verify that the keys produce the desired characters in normal, Shift, and Ctrl states.
3. If specific keys still behave unexpectedly, double-check the scancodes being passed to the mapping functions.
*/

===========================================


