// Estudando as estruturas de gerenciamento de símbolos 
// usadas no gcc 0.9










/* Different name spaces for symbols.  Looking up a symbol specifies
   a namespace and ignores symbol definitions in other name spaces.

   VAR_NAMESPACE is the usual namespace.
   In C, this contains variables, function names, typedef names
   and enum type values.

   STRUCT_NAMESPACE is used in C to hold struct, union and enum type names.
   Thus, if `struct foo' is used in a C program,
   it produces a symbol named `foo' in the STRUCT_NAMESPACE.

   LABEL_NAMESPACE may be used for names of labels (for gotos);
   currently it is not used and labels are not recorded at all.  */

/* For a non-global symbol allocated statically,
   the correct core address cannot be determined by the compiler.
   The compiler puts an index number into the symbol's value field.
   This index number can be matched with the "desc" field of
   an entry in the loader symbol table.  */

//namespace do símbolo.
typedef enum {
    UNDEF_NAMESPACE, 
	VAR_NAMESPACE, 
	STRUCT_NAMESPACE, 
	LABEL_NAMESPACE,	
	
}namespace_t;


// ?? onde encontrar o valor ??
/* An address-class says where to find the value of the symbol in core.  */
//classe do símbolo.
typedef enum {
  LOC_UNDEF,		/* Not used; catches errors */
  LOC_CONST,		/* Value is constant (enum type value name) */
  LOC_STATIC,		/* Value is at fixed address */
  LOC_REGISTER,		/* Value is in register */
  LOC_ARG,		/* Value is at spec'd position in arglist */
  LOC_LOCAL,		/* Value is at spec'd pos in stack frame */
  LOC_TYPEDEF,		/* Value not used; definition in SYMBOL_TYPE
			   Symbols in the namespace STRUCT_NAMESPACE
			   all have this class.  */
  LOC_LABEL,		/* Value is address in the code */
  LOC_BLOCK,		/* Value is address of a `struct block'.
			   Function names have this class.  */
  LOC_EXTERNAL		/* Value is at address not in this compilation.
			   This is used for .comm symbols
			   and for extern symbols within functions.
			   Inside GDB, this is changed to LOC_STATIC once the
			   real address is obtained from a loader symbol.  */	
	
}address_class_t;


//??tipo de símbolo
struct type_d
{
  TYPE_CODE_UNDEF,		/* Not used; catches errors */
  TYPE_CODE_PTR,		/* Pointer type */
  TYPE_CODE_ARRAY,		/* Array type, lower bound zero */
  TYPE_CODE_STRUCT,		/* C struct or Pascal record */
  TYPE_CODE_UNION,		/* C union or Pascal variant part */
  TYPE_CODE_ENUM,		/* Enumeration type */
  TYPE_CODE_FUNC,		/* Function type */
  TYPE_CODE_INT,		/* Integer type */
  TYPE_CODE_FLT,		/* Floating type */
  TYPE_CODE_VOID,		/* Void type (values zero length) */
  TYPE_CODE_SET,		/* Pascal sets */
  TYPE_CODE_RANGE,		/* Range (integers within spec'd bounds) */
  TYPE_CODE_PASCAL_ARRAY,	/* Array with explicit type of index */	
};

struct block_d
{
  /* Addresses in the executable code that are in this block.
     Note: in an unrelocated symbol segment in a file,
     these are always zero.  They can be filled in from the
     N_LBRAC and N_RBRAC symbols in the loader symbol table.  */
  int startaddr, endaddr;
  /* The symbol that names this block,
     if the block is the body of a function;
     otherwise, zero.
     Note: In an unrelocated symbol segment in an object file,
     this field may be zero even when the block has a name.
     That is because the block is output before the name
     (since the name resides in a higher block).
     Since the symbol does point to the block (as its value),
     it is possible to find the block and set its name properly.  */
  struct symbol_d *function;
  /* The `struct block' for the containing block, or 0 if none.  */
  /* Note that in an unrelocated symbol segment in an object file
     this pointer may be zero when the correct value should be
     the second special block (for symbols whose scope is one compilation).
     This is because the compiler ouptuts the special blocks at the
     very end, after the other blocks.   */
  struct block_d *superblock;
  /* Number of local symbols.  */
  int nsyms;
  /* The symbols.  */
  struct symbol_d *sym[1];
};


/* All of the name-scope contours of the program
   are represented by `struct block' objects.
   All of these objects are pointed to by the blockvector.

   Each block represents one name scope.
   Each lexical context has its own block.

   The first two blocks in the blockvector are special.
   The first one contains all the symbols defined in this compilation
   whose scope is the entire program linked together.
   The second one contains all the symbols whose scope is the
   entire compilation excluding other separate compilations.
   In C, these correspond to global symbols and static symbols.

   Each block records a range of core addresses for the code that
   is in the scope of the block.  The first two special blocks
   give, for the range of code, the entire range of code produced
   by the compilation that the symbol segment belongs to.

   The blocks appear in the blockvector
   in order of increasing starting-address,
   and, within that, in order of decreasing ending-address.

   This implies that within the body of one function
   the blocks appear in the order of a depth-first tree walk.  */

struct blockvector_d
{
  /* Number of blocks in the list.  */
  int nblocks;
  /* The blocks themselves.  */
  struct block_d *block[1];
};

// Inspirada no gcc 0.9.
struct symbol_d
{
    /* Symbol name */
	//Nome. ?? provavelmente é a string sendo tratada.
    char *name;
    
	/* Name space code.  */
	// A qual namespace o símbolo pertence.
	// aaaa.bbbb.cccc = 0;
    namespace_t namespace;
  
    /* Address class */
    address_class_t class;
    
	/* Data type of value */
	//?? tipo de símbolo ??
    struct type_d *type;
    
	/* 
	 Referente ao 'onde?'
	 constant value, or 
	 address if static, or 
	 register number,
     or offset in arguments, or 
	 offset in stack frame.  
	 */

    long value;
    struct block_d *block;
};


