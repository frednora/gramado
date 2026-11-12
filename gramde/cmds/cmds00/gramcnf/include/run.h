// compiler.h
// Created by Fred Nora.

#ifndef __COMPILER_H
#define __COMPILER_H    1

// Run from binay. (Intermediary Representation)
// Its called intermediary representation because at the
// last instance we can translate it into native machine code.
// .gir = executable program
int gramcnf_run_program(FILE *fp);

// Run from script
// .cnf = source script
FILE *gramcnf_run_script(int dump_output);


#endif    

