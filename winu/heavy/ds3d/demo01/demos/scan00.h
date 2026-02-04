// scan00.h
// Created by Fred Nora.

#ifndef __DEMOS_SCAN00_H
#define __DEMOS_SCAN00_H    1


#define OBJ_ELEMENT_TYPE_VECTOR  1
#define OBJ_ELEMENT_TYPE_FACE    2

// obj_element_d
// Represents a parsed element from an OBJ file.
// It can be either a vertex or a face.
// For now, only these two types are supported.
struct obj_element_d
{
    //int used;
    //int magic;
    int initialized;

    int type;  // 0=none, 1=vertex, 2=face
    struct gr_vecF3D_d vertex;  // Parsed "v" line
    struct gr_face_d face;      // Parsed "f" line
};


int scan00_custom_read_int(const char **strPtr);
float scan00_custom_read_float(const char **strPtr);

// It scans a single line given the pointer
const char *scan00_read_element_from_line(
    const char *line_ptr, 
    struct obj_element_d *elem );

#endif   

