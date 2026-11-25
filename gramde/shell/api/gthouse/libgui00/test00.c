// Here is an improved version of the `putpixel0` function with additional raster operations:
// Created by Copilot on github.com


// #
// NOT TESTED YET.
int putpixel0(
    unsigned int _color,
    unsigned long _x,
    unsigned long _y,
    unsigned long _rop_flags,
    unsigned long buffer_va)
{
    unsigned char *where = (unsigned char *)buffer_va;
    unsigned int Color = (unsigned int)(_color & 0xFFFFFFFF);
    char b = 0, g = 0, r = 0, a = 0;
    int Operation = (int)(_rop_flags & 0xFF);
    int bytes_count = 0;
    int bpp = (int)libgd_SavedBPP;
    int width = (int)(libgd_SavedX & 0xFFFF);
    int offset = 0;
    int x = (int)(_x & 0xFFFF);
    int y = (int)(_y & 0xFFFF);

    if (buffer_va == 0)
        return 0;

    b = (Color & 0xFF);
    g = (Color & 0xFF00) >> 8;
    r = (Color & 0xFF0000) >> 16;
    a = (Color >> 24) + 1;

    switch (bpp)
    {
    case 32:
        bytes_count = 4;
        break;
    case 24:
        bytes_count = 3;
        break;
    default:
        printf("putpixel0: bpp\n");
        exit(1);
        while (1)
        {
        }
        break;
    }

    width = (int)(width & 0xFFFF);

    if (bytes_count == 4)
    {
        offset = (int)(((width << 2) * y) + (x << 2));
    }
    if (bytes_count == 3)
    {
        offset = (int)((bytes_count * width * y) + (bytes_count * x));
    }

    unsigned char b2, g2, r2, a2;
    b2 = where[offset];
    g2 = where[offset + 1];
    r2 = where[offset + 2];
    if (bpp == 32)
    {
        a2 = where[offset + 3];
    }

    unsigned char b3, g3, r3, a3;

    switch (Operation)
    {
    case 0:
        r3 = r;
        g3 = g;
        b3 = b;
        a3 = a;
        break;
    case 1:
        r3 = (r2 | r);
        g3 = (g2 | g);
        b3 = (b2 | b);
        a3 = a2;
        break;
    case 2:
        r3 = (r2 & r);
        g3 = (g2 & g);
        b3 = (b2 & b);
        a3 = a2;
        break;
    case 3:
        r3 = (r2 ^ r);
        g3 = (g2 ^ g);
        b3 = (b2 ^ b);
        a3 = a2;
        break;
    case 10:
        r3 = (r2 & 0xFE);
        g3 = g2;
        b3 = b2;
        a3 = a2;
        break;
    case 11:
        r3 = r2;
        g3 = (g2 & 0xFE);
        b3 = b2;
        a3 = a2;
        break;
    case 12:
        r3 = r2;
        g3 = g2;
        b3 = (b2 & 0xFE);
        a3 = a2;
        break;
    case 20:
        r3 = (r2 & 0x80);
        g3 = (g2 & 0x80);
        b3 = (b2 & 0x80);
        a3 = a2;
        break;
    case 21:
        r3 = (r2 & 0x00);
        g3 = (g2 & 0xFF);
        b3 = (b2 & 0xFF);
        a3 = a2;
        break;
    case 22:
        r3 = ((r2 * 30) / 100);
        g3 = ((g2 * 59) / 100);
        b3 = ((b2 * 11) / 100);
        unsigned char common_gray = (unsigned char)(r3 + g3 + b3);
        r3 = (unsigned char)common_gray;
        g3 = (unsigned char)common_gray;
        b3 = (unsigned char)common_gray;
        a3 = a2;
        break;
    case 30: // invert colors
        r3 = ~r2;
        g3 = ~g2;
        b3 = ~b2;
        a3 = a2;
        break;
    case 31: // brighter
        r3 = (r2 + 50 > 255) ? 255 : r2 + 50;
        g3 = (g2 + 50 > 255) ? 255 : g2 + 50;
        b3 = (b2 + 50 > 255) ? 255 : b2 + 50;
        a3 = a2;
        break;
    case 32: // darker
        r3 = (r2 - 50 < 0) ? 0 : r2 - 50;
        g3 = (g2 - 50 < 0) ? 0 : g2 - 50;
        b3 = (b2 - 50 < 0) ? 0 : b2 - 50;
        a3 = a2;
        break;
    default:
        r3 = r;
        g3 = g;
        b3 = b;
        a3 = a;
        break;
    }

    where[offset] = b3;
    where[offset + 1] = g3;
    where[offset + 2] = r3;
    if (bpp == 32)
    {
        where[offset + 3] = a3;
    }

    return (int)1;
}

/*
New raster operations added:
- Case 22: Converts the pixel to grayscale.
- Case 30: Inverts the colors.
- Case 31: Makes the pixel brighter.
- Case 32: Makes the pixel darker.

You can replace the old `putpixel0` function with this improved version in `libdisp.c`.
*/