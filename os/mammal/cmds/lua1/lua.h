/*
** LUA - Linguagem para Usuarios de Aplicacao
** Grupo de Tecnologia em Computacao Grafica
** TeCGraf - PUC-Rio
** 19 May 93
*/

#ifndef lua_h
#define lua_h


#include "opcode.h"

// Function hook
typedef void (*lua_CFunction)(void);

// Struct for object
typedef struct Object *lua_Object;

// Register function:
// See: opcode.c
#define lua_register(n,f)  \
    (lua_pushcfunction(f), lua_storeglobal(n))


//
// Error
//

void lua_errorfunction(void (*fn) (char *s));
void lua_error(char *s);

//
// String
//

int lua_dostring(char *string);

//
// Call
//

int lua_call(char *functionname, int nparam);


lua_Object     lua_getparam 		(int number);
real           lua_getnumber 		(lua_Object object);
char          *lua_getstring 		(lua_Object object);
char 	      *lua_copystring 		(lua_Object object);
lua_CFunction  lua_getcfunction 	(lua_Object object);
void          *lua_getuserdata  	(lua_Object object);
lua_Object     lua_getfield        	(lua_Object object, char *field);
lua_Object     lua_getindexed      	(lua_Object object, double index);
lua_Object     lua_getglobal 		(char *name);

//
// Pop
//

lua_Object lua_pop(void);

//
// Push
//

int lua_pushnil(void);
int lua_pushnumber(real n);
int lua_pushstring(char *s);
int lua_pushcfunction(lua_CFunction fn);
int lua_pushuserdata(void *u);
int lua_pushobject(lua_Object object);

//
// Store
//

int lua_storeglobal(char *name);
int lua_storefield(lua_Object object, char *field);
int lua_storeindexed(lua_Object object, double index);

//
// is_ ?
//

int lua_isnil 		 (lua_Object object);
int lua_isnumber 	 (lua_Object object);
int lua_isstring 	 (lua_Object object);
int lua_istable      (lua_Object object);
int lua_iscfunction  (lua_Object object);
int lua_isuserdata   (lua_Object object);

//
// #
// INITIALIZATION
//

int lua_main(int argc, char *argv[]);

#endif
