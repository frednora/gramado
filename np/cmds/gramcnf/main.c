// main.c
// This is the main file for a C-like interpreter for Gramado OS.
// Target: GRAMCNF.BIN
// 2022 - Fred Nora


//
// =====================================================
// Command Line Usage: IR File Workflow
// =====================================================
//
// This interpreter/compiler supports two special flags
// for working with Intermediate Representation (IR) files:
//
// 1. Generate IR file (-ir)
// -------------------------
// Usage:
//     gramcnf -ir source.gcn
//
// Behavior:
//   - The source file (e.g., source.gcn) is compiled normally.
//   - Instead of only producing assembly, the compiler serializes
//     the internal stack of objects (IR) into a binary file
//     with extension .gir (e.g., source.gir).
//   - This .gir file contains all opcodes, operands, and token
//     strings needed for execution.
//   - No VM execution is performed in this mode.
//
// 2. Run IR file (-r or --run)
// ----------------------------
// Usage:
//     gramcnf -r program.gir
//     gramcnf --run program.gir
//
// Behavior:
//   - The lexer and parser are skipped.
//   - The VM loads the specified .gir file directly.
//   - The loader reconstructs the stack of objects from the file.
//   - vm_initialize() prepares the runtime environment.
//   - vm_loop() executes the opcodes sequentially.
//   - This mode allows fast startup and direct execution of
//     previously compiled programs.
//
// Notes:
//   - These two flags are mutually exclusive with the normal
//     compile path. If -r/--run is set, compiler() is not called.
//   - The .gir format is portable: it can be generated once
//     and executed multiple times without re-parsing the source.
//   - This design mirrors other systems such as Java (.class),
//     Python (.pyc), and LLVM (.bc).
//
// =====================================================

// #todo
// Minimal header for a .gir file.
// It should contain:
// - Magic string to identify the file type. ex: "GIR"
// ...


#include "gramcnf.h"

// See: parser.h
struct program_d  program;

const char *VersionString = "1.0";

//default name.
char program_name[] = "[Default program name]";
char *compiler_name;
//static int running = 1;
int running = 1;
//Para o caso de não precisarmos produzir 
//nenhum arquivo de output. 
int no_output=0;

/* While POSIX defines isblank(), it's not ANSI C. */
//#define IS_BLANK(c) ((c) == ' ' || (c) == '\t')
// #important
// Specification for gramc.
//char *standard_spec = "%{%{CC} -c %{I} -o %{O}}";


// =====================================================
static void doUsage(char **argv);
static void doVersion(char **argv);
static int gramcnf_initialize(void);
static void debugShowStat(void);
// =====================================================

static void doUsage(char **argv)
{
    printf ("\n");
    printf("#todo: %s doUsage\n",argv[0]);
}

static void doVersion(char **argv)
{
    printf ("\n");
    printf ("====================\n");
    printf ("%s version %s \n", argv[0], VersionString );
}


/*
int is_letter(char c);
int is_letter(char c) 
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
*/


// gramcnf_initialize:
// Initialize global variables.
static int gramcnf_initialize(void)
{
    int Status = 0;
    register int i=0;

    //printf ("gramcnf_initialize:\n");

// Clear buffers
    infile_size = 0;
    outfile_size = 0;
// Clear infile and outfile buffers.
    for ( i=0; i<INFILE_MAX_SIZE; i++ ){
        infile[i] = '\0';
    };
    sprintf (infile, "; ======================== \n");
    strcat (infile,  "; Initializing infile ...  \n\n");
    for ( i=0; i<OUTFILE_MAX_SIZE; i++ ){
        outfile[i] = '\0';
    };
    sprintf (outfile, "; ========================\n");
    strcat (outfile,  ";Initializing outfile ... \n\n");

// Clear text, data, bss buffers.
    sprintf (TEXT, "; ======================== \n");
    strcat  (TEXT, "; Initializing TEXT buffer \n");
    strcat  (TEXT, "segment .text              \n");
    sprintf (DATA, "; ======================== \n");
    strcat  (DATA, "; Initializing DATA buffer \n");
    strcat  (DATA, "segment .data              \n");
    sprintf (BSS,  "; ======================== \n");
    strcat  (BSS,  "; Initializing BSS buffer  \n");
    strcat  (BSS,  "segment .bss               \n");

// Table.

// Contador para não estourar a lista. 
    keyword_count = 0;  
    identifier_count = 0; 
    keyword_count = 0; 
    constant_count = 0; 
    string_count = 0; 
    separator_count = 0; 
    special_count = 0;
    // ...

// Usado pelo lexar pra saber 
// qual lugar na lista colocar o lexeme.

    current_keyword = 0; 
    current_identifier = 0; 
    current_keyword = 0; 
    current_constant = 0; 
    current_string = 0; 
    current_separator = 0; 
    current_special = 0;

// The 'program' structure.

    program.name = program_name;
    program.function_count;
    program.function_list = NULL;
    //...

// Initializing the metadata structure.
// See: globals.h and globals.c
    for (i=0; i<32; i++)
    {
        metadata[i].id = 0;
        metadata[i].initialized = FALSE;
        metadata[i].tag_size = 0;
        metadata[i].name_size = 0;
        metadata[i].content_size = 0;
    };

    return (int) Status;
}

// Mostra as estatísticas para o desenvolvedor.
static void debugShowStat(void)
{
    register int i=0;

    printf("\n");
    printf("debugShowStat\n");

// -------------------------
// lexer
    printf("==========================================\n");
    printf("Lexer info:\n");
    printf("number of lines: {%d}\n", LexerInfo.lexer_number_of_lines );
    printf("first line:      {%d}\n", LexerInfo.lexer_firstline );
    printf("last line:       {%d}\n", LexerInfo.lexer_lastline );
    printf("token count:     {%d}\n", LexerInfo.lexer_token_count );

// -------------------------
// parser
    printf("==========================================\n");
    printf("parser info:\n");
    printf("infile_size:     {%d bytes}\n",infile_size);
    printf("outfile_size:    {%d bytes}\n",outfile_size);

    printf("\n");
    printf("==========================================\n");
    printf("Printing metadata structure. :)\n");

// metadata
    printf("\n");
    for (i=0; i<32; i++)
    {
        if (metadata[i].initialized == TRUE)
        {
            printf("id{%d}: tag{%s} name{%s} content{%s} return{%d}\n",
                metadata[i].id,
                metadata[i].meta_tag,
                metadata[i].name,
                metadata[i].content,
                metadata[i].return_value ); // unsigned int
        }
    };

    printf("\n");
    printf("==========================================\n");
}

int main(int argc, char *argv[])
{
// Input
    FILE *fp;
// Output file for compiler.
    FILE *____O;
    register int i=0;
    char *filename;
// Output string.
    char *o;
// Switches 
    int flagA = 0;
    int flagB = 0;
    int flagC = 0;
    int flagD = 0;
    int flagString1 = 0;
    int flagString2 = 0;
    int flagString3 = 0;
    int flagString4 = 0;
    int flagX = 0;
    int flagY = 0;
    int flagZ = 0;
    int flagR = 0;
    int flagS = 0;
    int flagT = 0;

    int fShowStats = FALSE;  //#bugbug
    int fDumpOutput = FALSE;  // Dump output file?

    int fDumpIR = FALSE;      // Generate Intermediate Representation (IR) file.
    int fRunProgram = FALSE;  // Run program from a target binary (.gir).
    char *inputFile = NULL;

// Carregamos o arquivo num buffer em ring0.
// getc() precisa ler os dados em stdin
// #bugbug: 
// Se o buffer for maior que isso, read() falha.
    char __buf[1024];
    int nreads=0;

// Initializing
    //debug_print ("gramcnf: Initializing ...\n");  
    //printf ("\n");
    //printf ("main: Initializing ..\n");

// Inicializa variáveis globais.
    gramcnf_initialize();

    //printf ("*breakpoint");
    //while (1){}

//
// ## Args ##
//

// #todo
// O nome do programa é o primeiro comando da linha.
// compiler_name = argv[0];
 
// #debug 
// Mostrando os argumentos. 

    //printf ("argc=%d \n", argc );
    //for ( i=0; i < argc; i++ ){
    //    printf("arg %d = %s \n", i, argv[i] );
    //};

// flags.
// Comparando os argumentos para acionar as flags.

    for (i=0; i<argc; i++)
    {
        // #todo
        // Create symmetric symbols for these flags. 

        if ( strncmp( argv[i], "-a", 2) == 0 ){
        }
        if ( strncmp( argv[i], "-b", 2) == 0 ){
        }
        if ( strncmp( argv[i], "-s", 2) == 0 ){
            asm_flag = 1;
        }
        // Generate Intermediate Representation (IR) file.
        if (strncmp(argv[i], "-ir", 3) == 0) {
            fDumpIR = TRUE;
        }
        // Run program from a target binary (.gir).
        if ( strncmp(argv[i], "-r", 2) == 0 || 
             strncmp(argv[i], "--run", 5) == 0 ) 
        {
            fRunProgram = TRUE;
        }
        // Show stats
        if ( strncmp( argv[i], "--stats", 7) == 0 ){
            fShowStats = TRUE;
        }
        // Dump (Show) output file.
        // Show assembly code.
        if ( strncmp( argv[i], "--dumpo", 7) == 0 ){
            fDumpOutput = TRUE;
        }
        // help
        if ( strncmp( argv[i], "--help", 6) == 0 ){
            doUsage(argv);
            return EXIT_SUCCESS;
        }
        // version
        if ( strncmp( argv[i], "--version", 9) == 0 ){
            doVersion(argv);
            return EXIT_SUCCESS;
        }
        //...
    };

// # Arquivo de entrada #
// #bugbug
// lembrando que não podemos mais usar os elementos
// da estrutura em user mode.
// Então o buffer é gerenciado pelo kernel.
// podemos copiar o conteúdo do arquivo para um buffer aqui no programa
// através de fread, mas fread está disponível apenas na libc03.

// Open
    //printf ("\n");
    //printf("Calling fopen()    :)\n");
    //while(1){}

    //if ((void*) argv[2] == NULL) 
        //goto fail;

    fp = fopen((char *) argv[2], "rb");
    if (fp == NULL){
        printf("gramcnf: Couldn't open the input file\n");
        doUsage(argv);
        goto fail;
    }

// Input file
// Para que getc leia desse arquivo que carregamos.
    stdin = fp;
    finput = fp;
 
//#debug
// Esse while está aqui para visualizarmos o arquivo carregado.

    //int c;
    //while(1)
    //{
        //c=getc(stdin);
        //if(c == EOF)
            //break;
        //printf("%c",c);
    //}
    //fflush(stdout);
    //while(1){}
//=====================================

    if (fRunProgram == TRUE) 
    {
        int Status = -1;

        // Step 1: Load the target binary (.gir) into memory and 
        // build the stack of objects needed for execution.
        // ex: load_ir_binary(argv[2]);
        
        // Step 2: Run the program.
        Status = (int) vm_initialize();
        if (Status < 0){ 
            printf("main: on vm_initialize()\n");
            goto fail; 
        }
        Status = (int) vm_loop();
        if (Status < 0){ 
            printf("main: on vm_loop()\n");
            goto fail; 
        }
    }

//=====================================


// Compiler
// Routine:
// + Initialize the lexer.
// + Parse the tokens.
// + Return a pointer to the output file.
// IN: dump output file?

// #bugbug
// Do not write things in stdout, because its gonna show it 
// into the screen.
// We gotta use another output file to simply save it into the disk.

    if (fRunProgram != TRUE) 
    {
        ____O = (FILE *) compiler(fDumpOutput);
        //if ( (void*) ____O == NULL )
        //{
        //
        //}
    }


// Show the content of the output file.
// This is an assembly code.
// 'outfile' is a buffer for the assembly code 
// generate by the compiler.
    if (fDumpOutput == TRUE){
        printf ("\n");
        printf ("--------------------\n");    
        printf ("OUTPUT FILE:\n");
        printf ("%s\n", outfile);
        printf ("\n");
        printf ("--------------------\n");
        printf ("number of lines: %d\n", 
            LexerInfo.current_line );
    }

// Show stats
    if (fShowStats){
        debugShowStat();
    }

    printf("Done :)\n");
    return EXIT_SUCCESS;

fail:
    printf("Failed :(\n");
    return EXIT_FAILURE;
}

//
// End
//

