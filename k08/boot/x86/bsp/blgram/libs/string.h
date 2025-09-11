// string.h
// String support. (C99)
// Created by Fred Nora.

#ifndef __STRING_H
#define __STRING_H    1

int strcmp(char *s1, char *s2);
int strncmp(char *s1, char *s2, int len);
int str_cmp(unsigned char *str1, unsigned char *str2);
void *memcpy(void *v_dst, const void *v_src, unsigned long c);
size_t strlen(const char *s);
char *strcpy(char *to, const char *from);
char *strcat(char *to, const char *from);
void bcopy(char *from, char *to, int len);
void  bzero(char *cp, int len);

#endif    

//
// End
//
