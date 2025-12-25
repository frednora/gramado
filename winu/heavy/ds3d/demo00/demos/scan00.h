// scan00.h
// Created by Fred Nora.

#ifndef __DEMOS_SCAN00_H
#define __DEMOS_SCAN00_H    1

float scan00_custom_read_float(const char **strPtr);

// It scans a single line given the pointer
const char *scan00_scanline(const char *line_ptr, struct gr_vecF3D_d *return_v);

#endif   

