/* 
 * SHELL00.BIN
 *     Shell/console/editor ?
 * 
 * #important:
 * This is a very old project.
 * I am trying to port it to the new Gramado OS infrastructure.
 * ... and polish the code, offcorse.
 * 
 * ==================================================
 *  >>>> #todo: 
 *  Retirar o suporte a janelas desse shell e transforma-lo 
 * em um shell para rodar no terminal.
 * #ps: Actually this application has a lot of routines
 * that fits also with a console and an editor.
 * 
 * File: main.c - gdeshell - bash 1.05 clone. (baseado no bash)
 * #todo Para esse aplicativo vou usar o prompt: [username@hostname] $
 * #todo: precisamos deletar a parte referente ao terminal e outros... deve ficar apenas 
 * a parte de shell.
 *
 * #todo: a inten��o � que isso seja apenas um shell e n�o um terminal virtual.
 * devendo essa aplica��o rodar no terminal virtual quando chamada por ele, ou
 * pegar informa��es do terminal virtual onde a aplica��o vai rodar.
 * #todo: Como essa aplica��o faz parte do gde, o desafio no momento 
 * criar ou clonar processos, pra que os aplicativos chamados possam rodar.
 *
 * Essa � uma vers�o do shell apenas par ao kernel norax,
 * deve ser full screens sem frames. S� o suficiente para digitar
 * linhas de comando.
 *
 * Gramado Core Shell.
 * A shell to run only on Gramado Core environment. 
 *
 * GWM - Gramado Window Manager.
 * deve se comunicar com o GWS, Gramado Window Server. /gramado
 * General purpose application.
 *
 * #importante 
 * Observar o conceito de fluxo padr�o e o fato de printf enviar conte�do 
 * para o buffer stdout. Depois � s� imprimir o que est� em stdout.
 * Talvez isso implique em midan�as na libc pra ficar parecido com a 
 * implementa��o UNIX de output. E o refresh do buffer de sa�da?
 * O lado bom � que o buffer de sa�da poder� ser usado no PIPE.
 *
 *     SHELL.BIN � um aplicativo de pr�posito geral. Desenvolvido como 
 * ferramenta do desenvolvedor para prover varios tipos de testes de recursos do sistema.
 *
 * Ok, isso � um program do tipo 'janela', o pequeno terminal 
 * que roda em uma janela filha ser� gerenciado pelo pr�prio aplicativo.
 * Isso � diferente de um programa feito para rodar em um terminal, onde o kernel 
 * gerenciar� a janela usada pelo programa. 
 *
 * Podemos usar esse terminal na janela filha para interpretar linguagem basic.
 *
 * Descri��o:
 * Shell do sistema. (SHELL.BIN)
 * Arquivo principal do Shell.
 * � um aplicativo de 32bit em user mode. 
 * P3 - Processos em ring3, User Mode.
 * O Programa recebe comandos do usu�rio e envia os comandos para o n�cleo 
 * atrav�s de chamadas ao sistema. 
 *
 * (@todo: envio de mensagens) A id�ia � que cada comando chame um processo 
 * diferente, mas existem servi�os internos que o pr�prio Shell ofere�e
 * fazendo uso dos recursos do Kernel ou pr�prios.
 *     	
 * � o console, um terminal virtual em user mode.    
 *
 * Obs: O Shell � o aplicativo apropriado para o desenvolvedor criar e testar 
 * as chamadas ao Kernel via interrup��o e para ver as mensagens do compilador.
 *      O programa que executa aplica��es em lote deve enviar os outputs para 
 * a tela do Shell. 
 *
 * Obs: O entry point est� em head.s
 *      @todo: N�o usar o arquivo head em assembly efeito de portabilidade.
 * Obs: O prompt e cursor est�o definidos em stdio.h
 *
 * * IMPORTANTE: O FOCO DO INTERPRETADOR DE COMANDOS DO SHELL APP DEVE SER
 * A GER�NCIA DE ARQUIVOS E DISPOSITIVOS DE ARMAZENAMENTO, EM SEGUNDO LUGAR
 * A GERENCIA DOS RECURSOS DO SISTEMA, POIS ISSO � ATRIBUI��O DO APP TASKMAN.
 *
 * History:
 *     2016 - Created by Fred Nora.
 *     2018 - More commands.
 */

/*
 Como o kernel envia mensagens para o terminal em user mode?
 The mechanism for delivering messages to the console is implemented by the 
 printk function, defined in kernel/printk.c. 
 The function uses vsprintf (defined in lib/vsprintf.c) to create a message 
 string, places the string in the circular buffer of kernel messages and 
 passes it to all active console devices if the priority of the message is 
 less than console_loglevel. 
*/ 

#include "shell.h"

//# usado para teste 
//divis�vel por 4 � mais lento.

#define WINDOW_WIDTH     800    //750 
#define WINDOW_HEIGHT    600    //400
#define WINDOW_LEFT      0      //10
#define WINDOW_TOP       0      //10


// Input flags.
#define SHELLFLAG_NULL 0
#define SHELLFLAG_COMMANDLINE 1
#define SHELLFLAG_SCRIPT 2
#define SHELLFLAG_HELP 3
#define SHELLFLAG_VERSION 4
#define SHELLFLAG_USAGE 5
#define SHELLFLAG_TOPBAR 6
#define SHELLFLAG_FEEDTERMINAL 7
#define SHELLFLAG_EXIT 8
//...

/*
//para ficar igual a do Nelson;
struct shell_command {
    char *name;
    void *fun;
    char *help;
};

//extern struct command cmd_table[];
struct shell_command cmd_table[];

struct command cmd_table[] = {
    {"?",           cmd_help,           "This help"                                     },
    {"cd",          cmd_cd,             "Change current directory"                      },
    {"cls",         cmd_cls,            "Clear screen"                                  },
    {"copy",        cmd_copy,           "Copy file or directory"                        },
    {"date",        cmd_date,           "Date"                                          },
    {"del",         cmd_del,            "Delete file or directory"                      },
    {"dir",         cmd_dir,            "List directory"                                },
    {"echo",        cmd_echo,           "This ---"                                      },
    {"exit",        cmd_exit,           "Exit shell"                                    },
    {"help",        cmd_help,           "This help"                                     },
    {"mov",         cmd_mov,            "Move file or directory"                        },
    {"new",         cmd_new,            "New file or directory"                         },
    {"reboot",      cmd_reboot,         "Reboot system"                                 },
    {"rename",      cmd_rename,         "Rename file or directory"                      },
    {"shutdown",    cmd_shutdown,       "Shutdown your computer locally or remotely"    },
    {"time",        cmd_time,           "Time"                                          },
    {"version",     cmd_version,        "Shell version"                                 },
};
*/

//#define MIN(x,y) ((x < y) ? x : y)

int ShellFlag = 0;

//O shell est� rodadndo.
int _running = 1;

/* Non-zero when we are executing a top-level command. */
//o shell est� executando um comando que 
//est� emprimeiro plando.
int executing = 0;

int login_status = 0;

//Sendo assim, o shell poderia abrir no ambiente de logon.

char username[11];
char password[11];
//char sUsername[11];
//char sPassword[11];
//char *username;
//char *password; 

//#define DEFAULT_WINDOW_TITLE "Shell"

/* Non-zero means that this shell has already been run; i.e. you should
   call shell_reinitialize () if you need to start afresh. */
int shell_initialized = 0;

/* The current maintainer of the shell.  You change this in the
   Makefile. */
#ifndef MAINTAINER
#define MAINTAINER "deliberately-anonymous"
#endif
char *the_current_maintainer = MAINTAINER;

#ifndef PPROMPT
#define PPROMPT "bash\\$ "
#endif
char *primary_prompt = PPROMPT;

#ifndef SPROMPT
#define SPROMPT "bash> "
#endif
char *secondary_prompt = SPROMPT;

COMMAND *global_command = (COMMAND *) NULL;

/* Non-zero after SIGINT. */
int interrupt_state = 0;

/* The current user's name. */
char *current_user_name = (char *) "username";
//char *current_user_name = (char *) NULL;

/* The current host's name. */
char *current_host_name = (char *) "hostname";
//char *current_host_name = (char *) NULL;

/* Non-zero means that this shell is a login shell.
   Specifically:
   0 = not login shell.
   1 = login shell from getty (or equivalent fake out)
  -1 = login shell from "-login" flag.
  -2 = both from getty, and from flag.
 */

//Se o shell vai ser usado para login.
//Obs: Uma vari�vel no kernel guardo o id do processo 
//que fez login. 

int login_shell = 0;


/* Non-zero means this shell is running interactively. */

//Se for diferente de zero ent�o esse shell � interativo.
//Se for zero ele pode apenas estar executando um script.

int interactive = 0;

/* Non-zero means to remember lines typed to the shell on the history
   list.  This is different than the user-controlled behaviour; this
   becomes zero when we read lines from a file, for example. */
int remember_on_history = 1;

/* Non-zero means this shell is restricted. */
int restricted = 0;

/* Special debugging helper. */
int debugging_login_shell = 0;

/* The environment that the shell passes to other commands. */
//O ambiente que o shell passa para 
//o comando que ele executou.
char **shell_environment;

/* The number of commands executed so far. */
int current_command_number = 1;

/* The environment at the top-level REP loop.  We use this in the case of
   error return. */
//jmp_buf top_level, catch;

/* Non-zero is the recursion depth for commands. */
int indirection_level = 0;


/* The number of times BASH has been executed.  This is set
   by initialize_variables () in variables.c. */
int shell_level = 0;

/* The name of this shell, as taken from argv[2]. */
char *shell_name;

char *dist_version;
char *build_version; /*revision*/


/* The name of the .(shell)rc file. */
char *bashrc_file = "~/.bashrc";

/*nome do arquivo de configura��o*/
char *shell_config_file = "./shellcnf.txt";

/* Non-zero means to act more like the Bourne shell on startup. */
int act_like_sh = 0;

/* Values for the long-winded argument names. */
int debugging = 0;		/* Do debugging things. */
int no_rc = 0;			/* Don't execute ~/.bashrc */
int no_profile = 0;		/* Don't execute .profile */
int do_version = 0;		/* Display interesting version info. */
/* Be quiet when starting up. */
//inicializa��o silenciosa. Suprime alguns verboses.
int quiet = 0;	
int make_login_shell = 0;	/* Make this shell be a `-bash' shell. */
int no_line_editing = 0;	/* Don't do fancy line editing. */
int no_brace_expansion = 0;	/* Non-zero means no foo{a,b} -> fooa fooa. */

//
// ## Arguments support ##
//


/* Some long-winded argument names.  These are obviously new. */
#define Int 1
#define Charp 2

struct {
    char *name;
    int *value;
    int type;
} long_args[] = {

    { "debug",             &debugging,           Int   },
    { "norc",              &no_rc,               Int   },
    { "noprofile",         &no_profile,          Int   },
    { "rcfile",            (int *) &bashrc_file, Charp },
    { "version",           &do_version,          Int   },
    { "quiet",             &quiet,               Int   },
    { "login",             &make_login_shell,    Int   },
    { "nolineediting",     &no_line_editing,     Int   },
    { "nobraceexpansion",  &no_brace_expansion,  Int   },
    { (char *) NULL,       (int *) 0x0,          0     }

};

int shellStatus=0;
int shellError=0;

//... 

/*
//argument buffer
char **argbuf;
int argbuf_length;
int argbuf_index;
*/

/*
struct {
	
  char *word;
  int token;
  
} token_word_alist[] = {
  {"if", IF},
  {"then", THEN},
  {"else", ELSE},
  {"elif", ELIF},
  {"fi", FI},
  {"case", CASE},
  {"esac", ESAC},
  {"for", FOR},
  {"while", WHILE},
  {"until", UNTIL},
  {"do", DO},
  {"done", DONE},
  {"in", IN},
  {"function", FUNCTION},
  {"{", '{'},
  {"}", '}'},
  { (char *) NULL, 0 }
};
*/

//
// ===============================================================
// Prototypes
//

static void shellShell(void);  // constructor

static void updateObject (void);

int
__SendMessageToProcess ( 
    int pid, 
    struct window_d *window, 
    int message,
    unsigned long long1,
    unsigned long long2 );

void die (char *str);
void error ( char *msg, char *arg1, char *arg2 );
void fatal ( char *msg, char *arg1, char *arg2 );

//isso foi para stdlib.c
//void *xmalloc( int size);

char *concat ( char *s1, char *s2, char *s3 );
char *save_string ( char *s, int len );

int shell_save_file (void);

void shellInitSystemMetrics (void);
void shellInitWindowLimits (void);
void shellInitWindowSizes (void);
void shellInitWindowPosition (void);

// testes de scroll.
void testScrollChar (int c);

// row support

void textSetTopRow ( int number );
void textSetBottomRow ( int number );
int textGetTopRow (void);
int textGetBottomRow (void);


void clearLine ( int line_number );
void testShowLines (void);

void testChangeVisibleArea (void);
void updateVisibleArea ( int direction );
void shellRefreshVisibleArea (void);

void shellSocketTest (void);

// Procedimento de janela principal do aplicativo.
unsigned long 
shellProcedure ( struct window_d *window, 
                 int msg, 
 			     unsigned long long1, 
				 unsigned long long2 );
				
		
// ??
// di�logo para alimentar o terminal usado pelos aplicativos.				
int 
feedterminalDialog ( struct window_d *window, 
                     int msg, 
				     unsigned long long1, 
				     unsigned long long2 );
							  

// Procedimento de janela da topbar.							  
unsigned long 
shellTopbarProcedure ( struct window_d *window, 
                       int msg, 
			           unsigned long long1, 
					   unsigned long long2 );
					  

//
// ===============================================================
// Functions
//

int
__SendMessageToProcess ( 
    int pid, 
    struct window_d *window, 
    int message,
    unsigned long long1,
    unsigned long long2 )
{
	unsigned long message_buffer[5];

    if (pid<0)
		return -1;
	
	message_buffer[0] = (unsigned long) window;
	message_buffer[1] = (unsigned long) message;
	message_buffer[2] = (unsigned long) long1;
	message_buffer[3] = (unsigned long) long2;
	//...
	return (int) system_call ( 
		            112 , 
					(unsigned long) &message_buffer[0], 
	                (unsigned long) pid, 
					(unsigned long) pid );
}

//
// ## timer ##
//

int objectX;
int objectY;
int deltaX;
int deltaY;
//int deltaValue = 4;
int deltaValue = 1;

// Usado para testar o timer.
static void updateObject (void)
{
   //RECT rc;
   //GetClientRect(hwnd, &rc);

   objectX += deltaX;
   objectY += deltaY;

   if ( objectX < 2 )
   {
      objectX = 2;
      deltaX = deltaValue;
   }
   else if ( objectX > 78 ) {
	   
      objectX = 78; 
      deltaX = -deltaValue;  //muda a dire��o.
   }
   
   
   if (objectY < 2){
      objectY = 2;
      deltaY = deltaValue;
   }
   else if ( objectY > 24 ){
      objectY = 24; 
      deltaY = -deltaValue;
   }

    
	//
	// ## test ##
	//
	
	//update.
	//textCurrentRow = objectX;
    //textCurrentCol = objectY;
   
    //putchar.
	//shellInsertNextChar ( (char) 'T' );  
	
	shellSetCursor ( objectX, objectY );	
	
	printf ("%c", (char) 'X');
}

static void pause00 (void)
{	
    asm volatile ("pause" ::: "memory"); 
} 

/* 
 REP NOP (PAUSE) 
 is a good thing to insert into busy-wait loops. 
 */
static inline void rep_nop (void)
{	
    __asm__ __volatile__ ("rep;nop": : :"memory");
}
#define cpu_relax()  rep_nop()


//
// Prototypes
//

void quit (int status)
{
	_running = 0;
}
 
 

/*
void reader_loop ();
void reader_loop ()
{
    extern int indirection_level;
    
	int our_indirection_level;
    
	COMMAND *current_command = (COMMAND *) NULL;

    our_indirection_level = ++indirection_level;

    while (!EOF_Reached)
    {
        sighandler sigint_sighandler ();
        
		int code = setjmp (top_level);

        signal (SIGINT, sigint_sighandler);

        if (code != NOT_JUMPED)
 	    {
	        indirection_level = our_indirection_level;

	        switch (code)
	        {
	            //Some kind of throw to top_level has occured. 
	            case FORCE_EOF:
	            case EXITPROG:
	                current_command = (COMMAND *) NULL;
	                EOF_Reached = EOF;
	                goto exec_done;

	            case DISCARD:
	                //Obstack free command elements, etc. 
	                break;
					
	            default:
	                programming_error ("Bad jump %d", code);
	        }
	   }

      executing = 0;
		
      dispose_used_env_vars ();

      if (read_command () == 0)
	  {
	      if (global_command) 
		  {
	          current_command = global_command;

	          current_command_number++;

	          //POSIX spec: "-n  The shell reads commands but does
	          //not execute them; this can be used to check for shell
	          //script syntax errors.  The shell ignores the -n option
	          //for interactive shells. " 

	          if (interactive || !read_but_dont_execute)
	          {
		          executing = 1;
		          execute_command (current_command);
	          }

	         exec_done:
	         
			  if (current_command)
	             dispose_command (current_command);
	             QUIT;
	      }
		  
	  }else{
		  
	      // Parse error, maybe discard rest of stream if not interactive. 
	      if (!interactive)
	          EOF_Reached = EOF;
	  };
		
      if (just_one_command)
	      EOF_Reached = EOF;
    
	}; //while
	
    indirection_level--;
}
*/

// shellProcedure:
unsigned long 
shellProcedure( 
    struct window_d *window, 
    int msg, 
	unsigned long long1, 
	unsigned long long2 )
{
	unsigned long input_ret;
    unsigned long compare_return;	
    int q;	
	
	//if( msg == COMMAND_INITIALIZE_SHELL ){
		//...
	//}
	
    switch (msg)
    {
		//Faz algumas inicializa��es de posicionamento e dimens�es.
        //case MSG_INITDIALOG:
        //    break;

		//Torna a janela vis�vel.
        //case MSG_SHOWWINDOW:
		//    break; 
		 
		case MSG_KEYDOWN:
            switch (long1)
            {
				//EOF
				//case -1:
				//    #todo
				//    break;
				
				// Null key.
				case 0:
				    pause();
					pause();
					//pause();
					//pause();
					cpu_relax();
				    return (unsigned long) 0;
				    break;
				
				// Enter.
				// Finaliza a string e compara.
				case VK_RETURN:
				    
					//#test
					printf("\r");
					printf("\n");
				
				    input('\0'); 
					
					//#obs: 
					//#importante 
					//Se essa janela for a janela do shell, 
					//ent�o a rotina de compara��o poder� fazer um refresh dessa janela. 
					//#obs: talvez esse refresh nem seja necess�rio.
					//cada rotina chamada que fa�a seu pr�prio refresh se conseguir.
					shellCompare ();
					goto done;
                    break; 

				//#test	
                case VK_TAB:					
					printf("\t");
					goto done;
				    break;	

				//#todo
				case VK_BACK:
				    
					//#test
					//o cursor do ldisc no kernel precisa ser atualizado tambem.
					//textCurrentCol--;
					//apiSetCursor (textCurrentCol,textCurrentRow);
					//shellSetCursor (textCurrentCol,textCurrentRow);
					//shellInsertNextChar ( (char) ' ' ); 
					goto done;
                    break;					
                              
                // Mensagens de digita��o.
				// Texto. Envia o caractere.
                // Imprime os caracteres normais na janela com o foco de 
				// entrada.
				// Enfilera os caracteres na string 'prompt[]' para depois 
				// ser comparada com outras strings.
				// prompt[] � o stdin.
                default:			   
				    
					// Coloca no stdin, prompt[].
					input ( (unsigned long) long1 );      
                    
					// Coloca na mem�ria de video virtual,
					// Que � semelhante a vga, contendo char e atributo.
					shellInsertNextChar ( (char) long1 );  
					
					// #importante:   
					// IMPRIMINDO.
					// Funcionando bem.
					// Ok, no caso de backspace n�o deve imprimir nada,
					// mas talvez avan�e.
					//obs: tem que olhar o que a rotina no kernel faz no caso do backspace.
					
					//#importante: A ROTINA QUE INSERE O CHAR TEM QUE EFETUAR O REFRESH 
					//ASSIM ELA PODE CHAMAR O SCROLL SE PRECISAR.
					
					//shellRefreshCurrentChar();
					//printf ("%c", (char) long1 ); 	//deletar.				
					
					goto done;
                    break;               
            };
        break;
		
		case MSG_KEYUP: 
		    // printf("%c", (char) 'u');
            // printf("%c", (char) long1);  			
		    break;
		
		//N�o interceptaremos mensagens do sistema por enquanto.
		//As mensagens do sistema s�o interceptadas primeiro pelo procedimento 
		//do sistema.
		
		case MSG_SYSKEYDOWN:
		    switch (long1)
			{
		        
				case VK_F1:
                    

					//shellTestLoadFile ();
					
					//inicializa a �rea vis�vel.
					//textTopRow = 0;
	                //textBottomRow = 0 + 24;
						
					break;
					
				case VK_F2:
				    //testChangeVisibleArea();
					break;
					
				case VK_F3:
				    //shellRefreshVisibleArea();
					break;
					
				//...
				
                //full screen
                //colocar em full screen somente a �rea de cliente. 				
		        case VK_F11:
				    
					break;
					
				//...

			};
			break;
		
		//Obs:
		//essas teclas s�o tratadas pelo procedimento do sistema.
		//mas alguma tecla personalizada pode ser  tratada pelo aplicativo,
		//como o context menu [Application Key]
		case MSG_SYSKEYUP:
            switch (long1)
			{
				
				//O MENU APPLICATION � O CONTEXT MENU.
				//
				case VK_APPS:
				    //MessageBox ( 1, "Gramado Core Shell:", "VK_APPS Context Menu" );
					break;
			}		
		    break;
		

        // Commands.		
		case MSG_COMMAND:
            switch (long1)
			{
				// Null.
				case 0:
				    //MessageBox( 1, "Shell test", "Testing MSG_COMMAND.NULL." );
				    break;
				
				// About.
				// Abre uma janela e oferece informa��es sobre o aplicativo.
				case CMD_ABOUT:
				    // Test.
				    //MessageBox( 1, "Shell test", "Testing MSG_COMMAND.CMD_ABOUT." );
				    break;

				//default:
				//break;
			}
		    break;

		//Essa mensagem pode ser acionada clidando um bot�o.
		case MSG_CLOSE:
		    //??
		    //isso deve fechar qualquer janela que esteja usando esse procedimento.
			//pode ser uma janela filha ou ainda uma janela de dialogo criada pelo sistema.
			//??
		    printf("SHELL.BIN: MSG_CLOSE\n");
			
			//@todo: Criar essa funçao na api.
			//apiExitProcess(0);
			break;
		
		//Essa mensagem pode ser acionada clidando um bot�o.
		case MSG_DESTROY:
		    printf("SHELL.BIN: MSG_DESTROY\n");
		    break;
			
		// MSG_MOUSEKEYDOWN	
		case 30:
		    switch (long1)
			{
				case 1:
				
				    //#obs: No keydown a gente s� abaixa o bot�o.
					
				    //#debug
					//printf("button 1\n");     
					
					/*
                    //cima
					if ( window == app1_button )
                    {
						updateVisibleArea( 0 );
						shellRefreshVisibleArea(); 
                        //shellScroll();
						break;
					}
					*/

                    /*
					//baixo
					if ( window == app2_button )
                    {
                        updateVisibleArea( 1 );
						shellRefreshVisibleArea();
					    break;
					}
					*/
					
					/*
					if ( window == editboxWindow )
					{
					    APISetFocus (editboxWindow);
					    //APIredraw_window (editboxWindow,1);
						break;
					}
					*/					
					
					//APISetActiveWindow (window);
					//APIredraw_window ( window, 1 );
					
					//bot�o de close
					//if ( window == close_button )
				    //{
					    //APIresize_window ( window, 200, 200 );
					    //APIredraw_window ( window, 1 );
					    //refresh_screen (); //n�o precisa isso	

					//	_running = 0;
                    //    ShellFlag = SHELLFLAG_EXIT;						
					//}    
					break;
					
				case 2:
				    //#debug
				    printf("button 2\n");
				    break;
					
				case 3:
				    //#debug
				    printf("button 3\n");
				    break;
			};			
            break;

		// MSG_MOUSEKEYUP	
		case 31:
		    switch (long1)
			{		
				case 1:
				    /*
					//printf("up button 1\n");
					if (window == menu_button)
					{
	                    shellTestButtons ();	
		                refresh_screen ();						
					}
					*/
					
					/*
					//bot�o de reboot;
					if ( window == reboot_button )
                    {
					    printf("Rebooting...\n");
		                system("reboot"); 	
					}
					*/
					
					/*
					//bot�o de close
					if ( window == close_button )
				    {
					    //APIresize_window ( window, 200, 200 );
					    //APIredraw_window ( window, 1 );
					    //refresh_screen (); //n�o precisa isso	

						_running = 0;
                        
						ShellFlag = SHELLFLAG_EXIT;						
					} 
					*/ 

					break;
					
				case 2:
				    printf("up button 2\n");
				    break;
					
				case 3:
				    printf("up button 3\n");
				    break;
			};		
            break;	

		// MSG_MOUSEMOVE	
		case 32:
            //APISetFocus(window);
			//printf("m");
            break;	

		// MSG_MOUSEOVER	
		case 33:
		    
			/*
			//se tiver passando em cima do bot�o de close.
			if ( window == close_button )
			{
				//printf(".");
			    //APIresize_window ( window, 200, 200 );
				//APIredraw_window ( window, 1 );	
			}
			*/
            
		    break;

		//entered	
        case 38:
		    /*
			if ( window == close_button )
			{
			    //printf("entered\n");	
			}
			*/
            break;		

		//exited	
        case 39:
		    /*
			if ( window == close_button )
		    {
			    //printf("exited\n");
		    }
			*/
            break;
			
		//Quando a aplicativo em user mode chama o kernel para 
		//que o kernel crie uma janela, depois que o kernel criar a janela,
		//ele faz uma chamada ao procedimento de janela do aplicativo com a mensagem 
        //MSG_CREATE, se o aplicativo retornar -1, ent�o a rotina em kernel mode que 
        //esta criando a janela, cancela a janela que est� criando e retorn NULL.		
		case MSG_CREATE:
		    printf("SHELL.BIN: MSG_CREATE\n");
		    break;
			
		//MSG_TIMER ;;#TODO INCLUIR ISS0 NA API.	
		case 53:
		    //printf("shell tick\n");
			updateObject(); //interna
            break; 		
		
		case MSG_SETFOCUS:
		    //APISetFocus(window);
			break;
			
		case MSG_KILLFOCUS:
            break;

		//isso pinta os elementos da �rea de cliente.
        //essa mensagem � enviada para o aplicativo quando 
        //a fun��o 'update window'	� chamada.	
        case MSG_PAINT:
            printf("SHELL.BIN: MSG_PAINT\n");
			break;
			

		//@todo: isso ainda n�o existe na biblioteca. criar.	
        //case MSG_CLS:
            //limparemos o ret�ngulo da �rea de cliente,
			//mesmo que estejamos em full screen. 
		//	break;		
		
		//mudaremos o curso usando long1 e long2.
		//case MSG_SETCURSOR:
		//    break;
		
		//case MSG_HSCROLL:
		//    break;
		//case MSG_VSCROLL:
		//    break;
		
		
		//case MSG_FULLSCREEN:
		//    break;
		
		
		//case COMMAND_SET_WINDOW_SIZE:
		//    break;
		
		//case COMMAND_HIDE_WINDOW:
        //    break; 

		
		//#importante
		// os caracteres de controle encontrados na tabela ascii.
		//@todo: Essa mensagem precis ser criada, tanto no kernel 
		//quanto na API.
		//case MSG_ASCII_CONTROL_KEYS:
		//   switch(long1)
		//	{
	
		        //^A STX
                //case 1:
		        //    printf("^A");
                //    break;  		
	
		        //^B SOT
                //case 2:
		        //    printf("^B");
                //   break;  		
			
		        //^C ETX
                //case 3:
		        //    printf("^C");
		        //	shellPrompt();
                //    break;

		        //^D EOT
                //case 4:
		        //   printf("^D");
                //   break;

		        //^E ENQ
                //case 5:
		        //    printf("^E");
                //    break;  								
			
		        //^F ACK
                //case 6:
		        //    printf("^F");
                //    break;  					


		        //^G BEL
                //case 7:
		        //   printf("^G");
                //    break;  					
			
			
		        //^H BS
                //case 8:
                //    break;  								
			
		        //^I HT horizontal tab
                //case 9:
                //    break;  	

		        //^J LF
                //case 10:
                //    break;  			
			
						
		        //^K VT vertical tab
                //case 11:
                //    break;  	

		        //^L FF form feed
                //case 12:
                //    break;  	
			
			
		        //^M CR
                //case 13:
                //    break;  	
			

		        //^N SO
                //case 14:
                //    break;  				
		

		        //^O SI
                //case 15:
                //    break;


		        //^P DLE DATA LINK ESCAPE
                //case 16:
                //    break;  			
		
		
		        //^Q DC1 DEVICE CONTROL 1
                //case 17:
                //    break;  	
  			
			
		        //^R DC2 DEVICE CONTROL 2
                //case 18:
                //    break;

		        //^S DC3 DEVICE CONTROL 3
                //case 19:
                //    break;  			

		        //^T DC3 DEVICE CONTROL 4
                //case 20:
                //    break;  			
		
		
		        //^U NAK NEGATIVE ACKNOLEDGE
                //case 21:
                //    break;  			

		        //^V SYN SYNCHRONOUS IDLE
                //case 22:
                //    break;  			
		
		        //^W ETB END OF TRANSMISSION BLOCK
                //case 23:
                //    break;  			
		
		        //^X CAN CANCEL
                //case 24:
                //    break;  			

		
		        //^Y EM END OF MEDIUM
                //case 25:
                //    break;  			
		
		        //^Z SUB SUBSTITUTE
                //case 26:
                //    break;  			
		
		
		        //^[ ESC ESCAPE
                //case 27:
                //    break;  			
		
		        //^\ FS FILE SEPARATOR
                //case 28:
                //    break;  			
		
		        //^] GS GROUP SEPARATOR
                //case 29:
                //    break;  			
		
		        //^ RS RECORD SEPARATOR
                //case 30:
                //    break;  			
		
		
		        //_ US UNIT SEPARATOR
                //case 31:
                //    break;  			
		
		
		        //DELETE
                //case 127:
                //    break; 

        //    };		
		//    break;
			
		
		
		//Mensagem desconhecida.
		default:
		    //printf("shell procedure: mensagem desconhecida\n");
		    goto done;
		    break;	  
    };

    // Nothing for now !
done:
	return 0;
	//return (unsigned long) apiDefDialog ( window, msg, long1, long2 );
}

/*
 * shellWaitCmd:
 *     Espera completar o comando 
 * com um [ENTER]. ##suspensa
 */
void shellWaitCmd (void)
{	
	// @todo: Cuidado com loop infinito.
	
	// Obs: Quem muda o status � o procedimento,
	//      quando recebe o retorno da fun��o input:
	
	// Uma interrup��o de teclado aciona o procedimento
	// do shell que atualiza o status.
	
	// BUG BUG :
    //
    //    Pra esse shell funcionar teria que habilita
    //    agora a interrup��o de teclado e somente ela
    //    para que a interrup��o de timer n�o bagun�e as
    //    coisas fazendo troca de contexto.

	//asm("sti");    //@todo; N�o habilitar!

// Loop
    do {	 
		if (prompt_status == 1){
			prompt_status = 0;     
			return;
	    }
	} while (1);

    prompt_status = 0;	
}


/*
 * shellCompare:
 *     Compara comandos digitados com palavras chave.
 *     Compara a palavra em 'prompt[]' com outras e chama o servi�o.
 * o enter foi o caractere digitado, vamos comparar palavras.
 * Credits:
 * + Stephen Brennan - https://brennan.io/2015/01/16/write-a-shell-in-c/
 * + Frederico Martins Nora (frednora)
 */
 
//#TEST 
//#define LSH_TOK_DELIM " \t\r\n\a+!:=/.<>;|&" 
#define LSH_TOK_DELIM " \t\r\n\a" 
#define SPACE " "
#define TOKENLIST_MAX_DEFAULT 80
 
unsigned long shellCompare (void)
{
    //char **stringarray1;
	char *tokenList[TOKENLIST_MAX_DEFAULT];
	char *token;
	int token_count;	
	int i = 0;

    unsigned long ret_value;
	int q;    //di�logo
	char *c;
	
    //?? � um pathname absoluto ou n�o. ??
	//Ok. isso funcionou.
    int absolute; 
	
	//#importante:
	//Transferir toda alinha de comando para a mem�ria 
	//compartilhada.
	//obs: Possivelmente isso n�o continua� assim, pas por enquanto t� bom.

// #bugbug
// DANGER DANGER !!!
// #todo: Do NOT use this anymore.
	unsigned char *shared_memory = (unsigned char *) (0xC0800000 -0x100);
	//linha de 80 chars no m�x.
	for ( i=0; i<80; i++ ){
		shared_memory[i] = prompt[i];
	}

    // Temos uma linha de comando em prompt[]
    // que � o stdin.  
NewCmdLine:	

    //#debug
	//printf("prompt:{%s}\n",prompt);
	
	c = prompt;
	
	//Se alguem pressiona [ENTER] com prompt vazio
	//d� page fault.
	//Isso cancela caso o buffer esteja vazio.
	
	//>(NULL)
	//[ENTER] finalizou a atring antes de ter digitado alguma coisa.
	if ( *c == '\0' ){
	    goto exit_cmp;	
	}
	
	//>' '
	//S� faz o tratamento com while se for igual a ' '.
    if ( *c == ' ' || *c == '\t' )
	{
        int j;		
        //#importante:
		//Aqui estamos tratando uma linha de comando inteira.
		
	    //enquanto c[] for diverente de ' ' c avan�a.
	    //isso elimina os espa�os para c[]
	    j=0;
		while ( *c == ' ' || *c == '\t' )
		{ 
	        
			//Limits
		    j++;
			if ( j > wlMaxColumns )
			{
			   //#debug
               //Isso significa que uma string 
			   //construida em prompt[] n�o foi 
			   //corretamente finalizada.
			   //ou algum problema no while anterior.
			   //printf("#debug \n");				
			   printf("shellCompare: *hang while1\n");	
			   while (1){
				   asm ("pause");
			   }
			}
			
			//desrepeitamos o limite imposto pelo fim da string.
			//por isso temos que fazer a marca��o exata do in�cio 
			//da linha de comando e do fim da linha de comando.
			// |            |.......|        |
			// start       left   right     end
			
			c++; 
		};
		
		//se depois de eliminarmos os espa�os encontrarmos
		//um '\0' de fim de string.
		//significa que o usu�rio digitou um monte de espa�os
		//depois apertou enter.
		//n�o h� mais o que fazer.
	    if ( *c == '\0' ){
	        goto exit_cmp;	
	    }		
	    
		int line_rest = (wlMaxColumns-j);
		
		//copia todo o resto da linha para o inpicio da linha.
        for ( j=0; j<line_rest; j++ )
		{
			prompt[j] = c[j];
			
			//se enquanto estamos copiando,
			//ja copiamos o finalizador de string,
			//n�o precisamos copiar mais nada.
			if ( *c == '\0' ){
				goto commandlineok;
			}				
		}
        
		//#bugbug
        //se estamos aqui � porque copiamos quase 80 chars.		
    };

commandlineok:	
    //#debug
	//printf("prompt:{%s}\n",prompt);	
	
    //>/
    //>.
    //>./
    //>..
    //>../
	
	// #importante:
	// Vamos checar se o pathname � absoluto ou relativo.
	// +Se tiver barra, significa que o pathname � absoluto e 
	// come�a no diret�rio raiz do volume do sistema. root:/volume1/
	// +Se come�ar por root:/ tamb�m � absoluto.
	// +Pathname relativo � somente aquele que � apenas um nome 
	// de arquivo dentro do diret�rio atual. Como: 'name' ou name1/name2,
	// sem barra ou ponto.
	// $/ � o diret�rio raiz do volume do sistema.
	// $v/ � o diret�rio raiz do vfs.	
 

    absolute = absolute_pathname ( (char *) prompt );
	
    switch (absolute)
	{
		//N�o � absoluto ou falhar
		case 0:
		    goto this_directory;
		    break;
			
		//� absoluto	
		case 1:
		    goto check_directory; 
			break;
		//falha	
		case 2:
		default:
		    goto this_directory;
			break;
	};

//precisamos checar em que diret�rio 
//o programa est�.
//o comando tem um pathname absoluto ...
//Precisamos invocar esse programa 
//que est� em um diret�rio apontado no pathname.
//ex: ../cmd 	

check_directory:
	
	
//O primeiro elemento da linha de comandos 
// � um comando simples	
// cd ..
this_directory:	
	
	//if( *prompt == (char) '\0' ){
	//    goto exit_cmp;	
	//}
	
	// O input pode ser copiado aqui, ent�o manipularemos essa vari�vel.
	//char *FileName;
	
	//
	// ?? E se tivermos v�rias palavras na linha de comando ??
	//
		
    // Returns first token 
    //char *token = strtok(prompt, " -");
	
	//#debug
    //printf("shellCompare: Testing ...\n");
    //refresh_screen();	

	// ?? what ?
    // first call returns pointer
	// to first part of user_input
	// separated by delim

    //isso pega a primeira palavra digitada
	tokenList[0] = strtok( prompt, LSH_TOK_DELIM);

	//para o argumento atual
	//@todo: isso precisa ser limpado sempre.

	//salva a primeira palavra digitada.
	token = (char *) tokenList[0];

	i=0;                                  
    while ( token != NULL )
	{
        // Coloca na lista.
        //salva a primeira palavra digitada.
		tokenList[i] = token;

		//#debug
		//Mostra
        //printf("shellCompare: %s \n", tokenList[i] );
        //refresh_screen();
		
		token = strtok( NULL, LSH_TOK_DELIM );
		
		// Incrementa o �ndice da lista
        i++;
		
		//salvando a contagem.
		token_count = i;
    }; 

	//Finalizando a lista.
    tokenList[i] = NULL;
	
	//#importante:
	//nesse momento todos os argumentos est�o certinhos no vetor.
	//j� conferimos.

	//#debug
    //printf("shellCompare: %s \n", tokenList[i] );
    //refresh_screen();	

	//#debug
    //printf("shellCompare: Test done!\n");
    //refresh_screen();	

	// Zerando o �ndice do tokenList
    i=0;	
   
    //printf("shellCompare: Testing ...\n");
    //refresh_screen();
   
    // Keep printing tokens while one of the
    // delimiters present in str[].
    //while(token != NULL)
    //{
    //    printf("%s\n", token);
	//	refresh_screen();
	//	token = strtok(NULL, " -");
    //}
	
	//do_command(argc, argv);
	goto do_command;	
	
do_command:	
    //nothing.	
do_compare:

    //
	// ## identificadores de inicia��o de linha ##
	//
	
	//Esses identificadores determinam o que 
	//se vai encontrar na linha. E n�o existe volta, 
	//o que o identificador determinou ser� obedecido.

	// ## Lista de identificadores ligado a execuss�o ##
	// (do)
	// dobin - Execute programa .bin
	// dotry - Tenta executar qualquer extens�o
	// Obs: Esses comandos efetuam goto 
	// dentro desse c�digo, saltando diretamente 
	// para as rotinas executadoras de programas ou scripts.
	// Obs: O nome do progama est� em tokenList[1] e os 
    // argumentos nos pr�ximos tokenList.	


    if ( strncmp( (char *) tokenList[0], "dobin", 5 ) == 0 ){
	    goto dobin;	
	}

    if ( strncmp( (char *) tokenList[0], "dotry", 5 ) == 0 ){
		goto dotry;
	} 
	
	//um comando no shell pede para executar um script
    if ( strncmp( (char *) tokenList[0], "dosh", 4 ) == 0 ){
		goto dosh;
	} 	

    //...

	// #Importante:
	// Devemos pegar os argumentos salvos na lista.
	// O primeiro argumento � o nome do aplicativo que deve ser executado
	// e os outros argumentos s�o argumentos que devem ser passados para 
	// o aplicativo a ser executado.
	
	//
	// Talvez aqui devamos usar tokenList[0] e n�o prompt.
	//
	
    //@todo Criar sof links no vfs.
	//o vfs ficar� no kernel base.
	//@todo: Criar chamadas ao vfs.
		
    // L1 RAM /objetcs   
	// (diret�rio raiz para os arquivos que s�o diret�rios de objetos)
	// os objetos ser�o listador em um arquivo que nunca ser� salvo no disco.
	//if( strncmp( prompt, "/objects", 6 ) == 0 )
	//{
	//    printf("info: Open object manager root dir ...\n");
    //    goto exit_cmp;
    //};

	// L2 disk  /diret�rio raiz do sistema de arquivos.
	//if( strncmp( prompt, "/", 5 ) == 0 )
	//{
	//    printf("info: Open file system root dir ...\n");
    //    goto exit_cmp;
    //};

	// L3 LAN  // acesso a arquivos da lan
	//os arquivos lan ser�o listador em um arquivo que nunca ser� salvo no disco.
	//if( strncmp( prompt, "/lan", 6 ) == 0 )
	//{
	//    printf("info: Open lan root dir ...\n");
    //    goto exit_cmp;
    //};
 
	// L4 WAN   //acesso a arquivos da wan
	// os arquivos lan ser�o listador em um arquivo que nunca ser� salvo no disco.
	//if( strncmp( prompt, "/wan", 6 ) == 0 )
	//{
	//    printf("info: Open wan root dir ...\n");
    //    goto exit_cmp;
    //};
	
	
	//coment�rio
	// a linha � um coment�rio, podemos ignorar.
    //if( strncmp( prompt, "//", 2 ) == 0 ){
	//	goto exit_cmp;
	//};		
	
    //
	// Ordem alfab�tica.
	//
	
	// token
	// testando tokenList
	// comando usado para testes de comando.
	
	if ( strncmp( prompt, "token", 5 ) == 0 )
    {
		printf ("\nTesting tokenList ...\n");
		printf ("\nTotal={%d}\n", token_count );
		printf ("\n Comand = %s \n", tokenList[i] );
		//refresh_screen ();
		
		i++;
		token = (char *) tokenList[i];
		
		if ( token == NULL ){
			goto exit_cmp;
		}else{
			
		    printf ("\n argument_number={%d} argument={%s}\n", i, tokenList[i] );	
            
			if ( strncmp( (char*) tokenList[i], "-a", 2 ) == 0 ){
			    printf ("[OK] argumento %s selecionado.\n", tokenList[i] );
		    }
			//...
		};
		
		
		i++;
		token = (char *) tokenList[i];
		
		if ( token == NULL ){
			goto exit_cmp;
		}else{
			
		    printf ("\n argument_number={%d} argument={%s}\n", i, tokenList[i] );	
            
			if ( strncmp( (char*) tokenList[i], "-b", 2 ) == 0 ){
			    printf ("[OK] argumento %s selecionado.\n", tokenList[i] );
		    }
			//...
		};		
		
		
		i++;
		token = (char *) tokenList[i];
		
		if ( token == NULL ){
			goto exit_cmp;
		}else{
			
		    printf ("\n argument_number={%d} argument={%s}\n", i, tokenList[i] );	
            
			if ( strncmp( (char *) tokenList[i], "-c", 2 ) == 0 ){
			    printf ("[OK] argumento %s selecionado.\n", tokenList[i] );
		    }
			//...
		};
		
		printf ("\n");
		
		goto exit_cmp;
	};
    

    //
    // In�cio dos comandos.
    //
	
	// about 
	// isso � um teste.
	// mostra informa��es sobre o aplicativo usando 
	// um message box ou uma janela.

    if ( strncmp( prompt, "about", 5 ) == 0 )
    {
		printf ("~about\n");
        //shellSendMessage ( MSG_COMMAND, CMD_ABOUT, 0);
        goto exit_cmp;
    }

	// Imprime a tabela ascii usando a fonte atual.
    // 128 chars.	
    //if( strncmp( prompt, "ascii", 5 ) == 0 )
    //{
		//shellASCII();
	//	goto exit_cmp;
	//}


	// bmp
	// bmp exemplo.bmp
    if ( strncmp( prompt, "bmp", 3 ) == 0 )
	{
        /*
		// #suspended
		i++;
		token = (char *) tokenList[i];
		if( token == NULL ){
			printf("Error: No name!\n");
		}else{
		   
		   // test. aloca 200 KB
		   printf ("test 200 KB\n");
		   shellDisplayBMPEx ( (char *) tokenList[i], (int) (200) );		
			
			//@todo: podemos checar se o pathname � absoluto,
			//e onde se encontra o arquivo que queremos.
			//shellDisplayBMP ( (char *) tokenList[i] );
		   //test 2MB
		   //o argumento tamanho � dado  em kb.
		   //printf("test 2mb\n");
		   //shellDisplayBMPEx ( (char *) tokenList[i], (int) (1024*2) );
		};
		*/
		goto exit_cmp;
    };	

	// boot
    // ??
	
	if ( strncmp( prompt, "boot", 4 ) == 0 ){
	    printf ("~boot\n");
        goto exit_cmp;
    }

	// cd - Change dir.
	if ( strncmp( prompt, "cd", 2 ) == 0 )
	{	
		i++;
		token = (char *) tokenList[i];
		
		if( token == NULL )
		{
			printf("cd error: no arg\n");
		}else{
			
			//#bugbug: n�o � possivel fazer isso por enquanto,
			//pois n�o estamos fazendo parse de char por char.
			//if( strncmp( (char*) tokenList[i], ".", 2 ) == 0 )
			//{}
				
			if( strncmp( (char*) tokenList[i], "..", 2 ) == 0 )
			{
				//Apaga o nome do �ltimo diret�rio.
			    shell_pathname_backup ( current_workingdiretory_string, 1 ); 
                goto exit_cmp;				
		    }			
			
	        // updating the current working directory string.
	        shellUpdateWorkingDiretoryString ( (char *) tokenList[i] );
			
			//@todo: podemos checar se o pathname � absoluto,
			//e onde se encontra o arquivo que queremos.
			//shellDisplayBMP( (char*) tokenList[i] );
		};		

		// o que segue o comando cd � um pathname.
		//@todo: podemos checar se o pathname � absoluto,
		//e onde se encontra o arquivo que queremos.
		//cd_buitins();
	    goto exit_cmp;
	}
	
	// clear-screen-buffer
	if ( strncmp( prompt, "clear-screen-buffer", 19 ) == 0 )
	{
		shellClearBuffer ();
		goto exit_cmp;
	}	

	//close
	if ( strncmp( prompt, "close", 5 ) == 0 )
	{
		//shellSendMessage ( MSG_CLOSE, 0, 0 );
	    goto exit_cmp;
	}		

    // cls - Clear the screen.
	if ( strncmp( prompt, "CLS", 3 ) == 0 || 
	     strncmp( prompt, "cls", 3 ) == 0 )
	{
        printf ("~cls\n");
		//cls_builtins ();
        goto exit_cmp;
	}

    /*
	// #deprecated
	// color
	// color-scheme
	// Seleciona um dos 2 modos de cores padr�o do sistema.	
    if ( strncmp( prompt, "color", 5 ) == 0 || 
	     strncmp( prompt, "color-scheme", 12 ) == 0 )
	{
		i++;
		token = (char *) tokenList[i];
		
		if( token == NULL )
		{
			printf ("No args\n");
			goto exit_cmp;
		}else{
            
			//humility
			if ( strncmp( (char *) tokenList[i], "--humility", 10 ) == 0 )
			{ 
		        printf("Selecting Humility color scheme\n"); 
                system_call(119, (unsigned long)1, (unsigned long)1, (unsigned long)1);
				printf("done\n");
                goto exit_cmp;				
			}
			
			//pride
			if ( strncmp( (char *) tokenList[i], "--pride", 7 ) == 0 )
			{
				printf("Selecting Pride color scheme\n");
                system_call(119, (unsigned long)2, (unsigned long)2, (unsigned long)2);
                printf("done\n");
				goto exit_cmp;				
			}
			
			// Talvez haver�o mais esquemas ...
			//...
		};
		goto exit_cmp;
    }
	*/

	// copy
	// o que segue o comando copy s�o dois pathname.
	//@todo: podemos checar se o pathname � absoluto,
    //e onde se encontra o arquivo que queremos.		
	if ( strncmp( prompt, "copy", 4 ) == 0 )
	{	
		copy_builtins ();
	    goto exit_cmp;
	}
		
	// date
	if ( strncmp( prompt, "date", 4 ) == 0 )
	{
		date_builtins();
        goto exit_cmp;
    }
	
	// del
	// o que segue o comando del � um pathname.
	//@todo: podemos checar se o pathname � absoluto,
	//e onde se encontra o arquivo que queremos.		
	if ( strncmp( prompt, "del", 3 ) == 0 )
	{	
		del_builtins();
	    goto exit_cmp;
	}	

    /*
    // desktop
	if ( strncmp( prompt, "desktop", 7 ) == 0 )
	{
        desktopInitialize ();
        goto exit_cmp;
	}
	*/

    /*
    // dialog-box
	//Testing dialog box.
	if ( strncmp( prompt, "dialog-box", 10 ) == 0 )
	{
		//@todo:testar os 4 tipos 
	    //MessageBox( 1, "Shell dialog box","Testing dialog box...");
        goto exit_cmp;
    }
	*/

	// dir - Lista os arquivos no estilo DOS.
	if ( strncmp ( prompt, "dir", 3 ) == 0 )
	{
		char dir_name[] = "volume1";
		
		i++;
		token = (char *) tokenList[i];
		
		if ( token == NULL )
		{
			
		    //listar os arquivos em um diret�rio dado o nome do diret�rio.
			//enterCriticalSection ();
		    system_call ( 177,
		             (unsigned long) dir_name,   //nome do diret�rio.
                     (unsigned long) dir_name,   
                     (unsigned long) dir_name ); 
		    //exitCriticalSection ();				
			
		}else{

		    //listar os arquivos em um diret�rio dado o nome do diret�rio.
			//enterCriticalSection();
		    system_call ( 177,
		             (unsigned long) tokenList[i],   //nome do diret�rio.
                     (unsigned long) tokenList[i],   
                     (unsigned long) tokenList[i] ); 
		    //exitCriticalSection();		
			
			//...
		};		
				
		// o que segue o comando dir � um pathname.
		//@todo: podemos checar se o pathname � absoluto,
		//e onde se encontra o arquivo que queremos.
        //se o pathname for null ent�o o comando atua sobre o pwd		
		//dir_builtins();
        goto exit_cmp;
    }
	
    // disk-info
	if ( strncmp( prompt, "disk-info", 9 ) == 0 )
	{	
	    shellShowDiskInfo ();
        goto exit_cmp;
    }

	// echo - Echo de terminal.
    if ( strncmp( prompt, "echo", 4 ) == 0 )
	{
		echo_builtins (tokenList);
		goto exit_cmp;
    }

    /*
	// editbox
	// Cria uma edibox.
    // #teste: deletar.
	if ( strncmp( prompt, "editbox", 7 ) == 0 )
	{
	    enterCriticalSection();    
	    shellCreateEditBox ();
	    exitCriticalSection();    
		goto exit_cmp;
    }
	*/
	
	
	// exec - Executa um programa fechando o shell.
    if ( strncmp( prompt, "exec", 4 ) == 0 )
	{
		// o que segue o comando exec � um pathname.
		//@todo: podemos checar se o pathname � absoluto,
		//e onde se encontra o arquivo que queremos.		
		exec_builtins();
		ShellFlag = SHELLFLAG_EXIT;
		goto exit_cmp;
    }

	// exit - Exit the application.
    if ( strncmp( prompt, "exit", 4 ) == 0 )
	{
		exit_builtins();
		ShellFlag = SHELLFLAG_EXIT;
		goto exit_cmp;
    }
	
	/*
	// #suspended
    // fork
	// Retorna no filho e chamamos execve.
	if ( strncmp ( prompt, "fork", 4 ) == 0 )
	{	
		printf ("gdeshell: Tentando clonar o processo atual ...\n");
		int pidFORK = (int) fork();
		// Erro.
		if ( pidFORK < 0 )
		{
		    printf ("gdeshell: falhou\n");
			printf ("getpid=%d ", getpid());
			printf ("pidFORK=%d \n", pidFORK);
            exit (1);
		}
		// Pai.
		if ( pidFORK > 0 )
		{
		    printf ("gdeshell: estamos no PAI\n");
			printf ("getpid=%d ", getpid());
			printf ("pidFORK=%d \n", pidFORK);
			printf ("gdeshell: saindo do PAI\n");
			exit (0);
			goto exit_cmp; 
		}
		// Filho.
		if ( pidFORK == 0 )
		{
		    printf ("gdeshell: estamos no FILHO\n");
			printf ("getpid=%d ", getpid());
			printf ("pidFORK=%d \n", pidFORK);
			
			//execve ("jackpot.bin", NULL, NULL );
			//execve ("gramcode.bin", NULL, NULL );
			//execve ("jackpot.bin", NULL, NULL );
			execve ("noraterm.bin", NULL, NULL );
			
			printf ("gdeshell: execve falhou.\n");
			exit (1);
		}		

		//#TODO
		//se aqui estivermos rodando o filho, devemos analizar
		//porque ela trava olhando o registrador.
		
		//execve ("jackpot.bin", NULL, NULL );
		//exit (0);
		//while (1){}
		
		//shellShowKernelInfo ();

		//printf ("gdeshell: retornamos do fork");
		//printf ("gdeshell: *hang");
		//while (1){}

 		//Mostra o ID do processo clone.
		//printf("Clone PID={%d}\n", pidFORK );
		
		//mostra o id do processo atual.
		//shellShowPID ();
		
		//printf("gdeshell: done\n");
        
		goto exit_cmp;
	}
	*/
	

    // getpid
	if ( strncmp( prompt, "getpid", 6 ) == 0 ){
	    getpid_builtins ();
        goto exit_cmp;
    }
    // getppid
	if ( strncmp( prompt, "getppid", 7 ) == 0 ){
	    getppid_builtins();
        goto exit_cmp;
    }
    // getuid - get user id
	if ( strncmp( prompt, "getuid", 6 ) == 0 ){
	    getuid_builtins();
        goto exit_cmp;
    }
    // getgid - get group id
	if ( strncmp( prompt, "getgid", 6 ) == 0 ){
	    getgid_builtins();
        goto exit_cmp;
    }

    /*
	//get-usersession
	if ( strncmp( prompt, "get-usersession", 15 ) == 0 ){
	    shellShowUserSessionID();
        goto exit_cmp;
    }
	*/

    /*
    //get-windowstation
	if ( strncmp( prompt, "get-windowstation", 17 ) == 0 )
	{
	    shellShowWindowStationID();
        goto exit_cmp;
    }
    */	

    /*
    //get-desktop
	if ( strncmp( prompt, "get-desktop", 11 ) == 0 )
	{
	    shellShowDesktopID();
        goto exit_cmp;
    }
	*/

	//unsigned char *hBuffer = (unsigned char *) 0xC1000000;
	void *hBuffer;
	
	//int thisprocess_id = (int) system_call ( 85, 0, 0, 0); 
	//unsigned char *heaptest = (unsigned char *) system_call ( 184, thisprocess_id, 0, 0 );		
	
	/*
    // heap
	// #testing malloc() in ring 3.
	if ( strncmp( prompt, "heap", 4 ) == 0 )
	{
		printf ("Testando heap\n");
				
		hBuffer = (void *) malloc ( 1024*100 ); //100kb
        //hBuffer = (void *) malloc ( 1024*1024*3 ); //3mb		
		
		if ( (void *) hBuffer == NULL )
		{
			printf("malloc fail 100kb\n");
		}else{
			printf("malloc ok 100kb\n");
		}
		
		printf("done\n");
        goto exit_cmp;
    }
	*/
	
	/* 
	// hd ??
	// hd info maybe.
    if ( strncmp( prompt, "hd", 2 ) == 0 )
	{
	    printf ("~hd\n");
        goto exit_cmp;
    }
	*/

	// help
	// ?
	// Mostra ajuda.
    if ( strncmp( prompt, "HELP", 4 ) == 0 ||  
	     strncmp( prompt, "help", 4 ) == 0 || 
	     strncmp( prompt, "?", 1 ) == 0 )
	{

		i++;
		token = (char *) tokenList[i];
		if (token == NULL){
			help_builtins(1);
		}else{
            
			if( strncmp( (char*) tokenList[i], "--all", 5 ) == 0 )
			{ 
                help_builtins(1); 		
			}
			
			if( strncmp( (char*) tokenList[i], "--min", 5 ) == 0 )
			{
                help_builtins(2);				
			}
	
			//...
		};
		goto exit_cmp;
    }	

    /*
	// install	
	if ( strncmp( prompt, "install", 7 ) == 0 )
	{
	    printf ("~install\n");
        goto exit_cmp;
    }
	*/

    // kernel-info
	if ( strncmp( prompt, "kernel-info", 11 ) == 0 )
	{
	    shellShowKernelInfo ();
        goto exit_cmp;
    }	

    // ls
	// lista arquivos no estilo unix.
	//isso ser� um programa.
	//if( strncmp( prompt, "ls", 2 ) == 0 )
	//{
		//@todo: Isso deve ser um aplicativo.
	//	printf("~ls\n");
    //    goto exit_cmp;
	//};


    // message-box
	// Testing message box.
	// #todo:testar os 4 tipos 
	
	/*
	// #deprecated
	if ( strncmp( prompt, "message-box", 11 ) == 0 )
	{
	    MessageBox ( 1, "gdeshell", "Testing message box..." );
        goto exit_cmp;
    }
	*/
	
    // mm-info
	if ( strncmp( prompt, "mm-info", 7 ) == 0 )
	{
	    shellShowMemoryInfo ();
        goto exit_cmp;
    }
	
    // mm-kernelheap
	if ( strncmp( prompt, "mm-kernelheap", 13 ) == 0 )
	{
	    shellShowKernelHeapPointer ();
        goto exit_cmp;
    }

    // mm-processheap
	if ( strncmp( prompt, "mm-processheap", 14 ) == 0 )
	{
	    shellShowProcessHeapPointer ();
        goto exit_cmp;
    }

    /*
	// mov
	if ( strncmp( prompt, "mov", 3 ) == 0 )
	{
		// o que segue o comando mov s�o dois pathnames.
		//@todo: podemos checar se o pathname � absoluto,
		//e onde se encontra o arquivo que queremos.		
	    goto exit_cmp;
	}		
    */

    // metrics
	// Mostra algumas informa��es de m�trica do sistema.
	if ( strncmp( prompt, "metrics", 7 ) == 0 )
	{
		shellShowMetrics();
        goto exit_cmp;
	}	

    /*
	// new
	if ( strncmp( prompt, "new", 3 ) == 0 )
	{		
	    printf("~new - New file or directory\n");		
	    goto exit_cmp;
	}
	*/
    
	/*
	// #suspended
	// mbr
	// ?? Talvez mostrar informa��es sobre o mbr ou realizar testes.
    if ( strncmp( prompt, "mbr", 3 ) == 0 )
	{
	    printf("~mbr\n");
		//#bugbug pagefault	
		shellTestMBR();
		printf("done\n");
		goto exit_cmp;
    }
	*/
	
    // pci-info
	if ( strncmp( prompt, "pci-info", 8 ) == 0 )
	{
	    shellShowPCIInfo();
        goto exit_cmp;
    }	
	
    // puts - testing puts, from libc.
	if ( strncmp( prompt, "puts", 4 ) == 0 )
	{
		puts("puts(): Hello from puts\n");
        goto exit_cmp;
	}

    /*
	// #suspended
	if ( strncmp ( prompt, "current-process", 15 ) == 0 )
	{
		system_call ( SYSTEMCALL_CURRENTPROCESSINFO, 0, 0, 0 );
		goto exit_cmp; 
	}
	*/
	
	// pwd - print working directory
	if ( strncmp( prompt, "pwd", 3 ) == 0 )
	{
		pwd_builtins ();
	    goto exit_cmp;
	}

    // reboot
    // @todo: Isso ser� um aplicativo. reboot.bin
	// obs: testando comando reboot.bin
	
	/*
    if ( strncmp( prompt, "REBOOT", 6 ) == 0 || 
	     strncmp( prompt, "reboot", 6 ) == 0 )
	{
	    //printf("~reboot\n");
		printf ("Tem certeza que deseja reiniciar o sistema?\n");
		q = (int) apiDialog ("Pressione 'y' para Yes ou 'n' para No.\n");
		if ( q == 1 )
		{ 
		    printf ("Rebooting...\n");
		    system ("reboot"); 
		};
		goto exit_cmp;
    };
	*/

    /*
	// rename - reneme directory or file.
	if ( strncmp( prompt, "rename", 6 ) == 0 )
	{
		// o que segue o comando rename s�o dois pathnames.
		//@todo: podemos checar se o pathname � absoluto,
		//e onde se encontra o arquivo que queremos.		
        goto exit_cmp;
    }
	*/	

    /*
    // root
    if ( strncmp( prompt, "root", 4 ) == 0 )
	{
	    printf("~/root\n");
		//testa_root();
		goto exit_cmp;
    }
	*/

	
    // save
	if ( strncmp( prompt, "save", 4 ) == 0 )
	{
		// o que segue o comando save � um pathname.
		//@todo: podemos checar se o pathname � absoluto,
		//e onde se encontra o arquivo que queremos.		
	    printf("~save root\n");
        goto exit_cmp;
    }
	
	/*
	// #suspended
	//scroll1
	if ( strncmp( prompt, "scroll1", 7 ) == 0 )
	{
	    testScrollChar((int) '1');
        goto exit_cmp;		
	}
	*/
	
	/*
	// #suspended
	//scroll2
	if ( strncmp( prompt, "scroll2", 7 ) == 0 )
	{
	     testScrollChar((int) '2');
        goto exit_cmp;		
	}	
	*/
	
	/*
	// #deprecated
	// service
	//podemos tertar servi�os do kernel 
	//enviando argumentos nulos.
	if ( strncmp( prompt, "service", 7 ) == 0 )
	{
	    printf("~service - testa servicos do kernel:\n");
		//test_services();
        goto exit_cmp;
    }
	*/
	
	/*
	// #suspended
	// show-screen-buffer
	if ( strncmp( prompt, "show-screen-buffer", 18 ) == 0 )
	{
		shellShowScreenBuffer ();
		goto exit_cmp;
	}
	*/


    // shellinfo
	// informa��es sobre o aplicativo.	
	if ( strncmp( prompt, "shell-info", 10 ) == 0 )
	{
	    printf ("#todo: shell info.\n");
		shellShowInfo();
        goto exit_cmp;
    }
	
	/*
	// #suspended
	//socket-test
	//rotina de teste de soquetes.
	if ( strncmp( prompt, "socket-test", 11 ) == 0 )
	{
		//printf("~socket \n");        
		shellSocketTest();
		goto exit_cmp;
    }
	*/


	if ( strncmp( prompt, "shutdown", 8 ) == 0 )
	{
	    printf("~shutdown \n");
        goto exit_cmp;
    }

    /*
	// slots
	if ( strncmp( prompt, "slots", 5 ) == 0 )
	{
	    printf("~slots - mostra slots \n");
		//mostra_slots();
        goto exit_cmp;
    };
    */	
	
	/*
	// start
	// Inicia uma nova janela(instancia ??) para executar 
	// um programa ou comando desejado.
	//??#bugbug: O que isso deveria fazer ?? 
	//??reiniciar o shell ou iniciar um programa ??
    if ( strncmp( prompt, "start", 5 ) == 0 )
	{
		// Isso deve setar o foco na janela do shell.
		// ao mesmo tempo que reinicia o input para digita��o 
		// e ajusta as margens do cursor. :)
		// Qualquer editbox precisa desse tipo de ajuste.
	    
		APISetFocus(window);
		//shellPrompt();
		printf("~start\n");
		
		goto exit_cmp;
    }
	*/

    // systeminfo
	// informa��es sobre o sistema.
	if ( strncmp( prompt, "system-info", 11 ) == 0 )
	{
	    printf("~@todo: system info.\n");
		shellShowSystemInfo();
        goto exit_cmp;
    }	
	
	
    // t1 - Test file
	// Carrega um arquivo no buffer de words 
	// depois exibe todo o buffer mantendo o posicionamento 
	//no cursor. Isso for�a um scroll.
	if ( strncmp( prompt, "t1", 2 ) == 0 )
	{	
		//esse testa com fopen
		shellTestLoadFile ();		
        goto exit_cmp;
    }
	
	/*
	// #deprecated
	// t2 - Test bmp
	if ( strncmp( prompt, "t2", 2 ) == 0 )
	{
		shellTestDisplayBMP();
        goto exit_cmp;
    }
	*/	
	
	/*
	// #suspended
	// t3 - Test thread
	if ( strncmp( prompt, "t3", 2 ) == 0 ||
        strncmp( prompt, "test-thread", 11 ) == 0	)
	{
	    shellTestThreads();
        goto exit_cmp;
    }
	*/
	
	//@todo: Colocar no in�cio dessa fun��o.
	FILE *f1;
	int ch_test;
	
	// t4 - testando fopen
	if ( strncmp( prompt, "t4", 2 ) == 0 )
	{
		printf ("\n t4: Open init.txt \n");
        
		f1 = fopen ("init.txt","rb");  
        if( f1 == NULL )
		{
			printf ("fopen fail\n");
			//goto exit_cmp;
		}else{
			printf ("fopen ok\n");
			//goto exit_cmp;
		}
		
		//Isso mostra que o arquivo foi carregado corretamente 
		//na base esperada.
		//printf("Show f1->_base: %s\n",f1->_base);
		
		//printf("stream info:\n");
		//printf("f1->_base: %x\n",f1->_base);
		//printf("f1->_ptr: %x\n",f1->_ptr);
		//printf("f1->_cnt: %d\n",f1->_cnt);
		
		//
		// #bugbug ... o fgetc n�o l� na estrutura esperada.
		//
		printf("Testing fgetc ... \n\n");
		while(1)
		{
			//ch_test = (int) fgetc(f1);
			ch_test = (int) getc(f1); 
			if( ch_test == EOF )
			{
				printf("\n\n");
				printf("EOF reached :)\n\n");
				//printf("Show f1->_base: %s\n",f1->_base);
				goto exit_cmp;
			}else{
			    printf("%c", ch_test);	
			};
		};
		
		//fail.
		goto exit_cmp;
    }

	// t5 - ( save file )
	// Ok isso funcionou.
	if ( strncmp( prompt, "t5", 2 ) == 0 )
	{
		printf("t5: save file\n");
		shell_save_file ();
		printf("t5: done\n");
        goto exit_cmp;
    }	

    /*
	// t6 - testando a api. rotina que salva arquivo. 
	// ok, isso funcionou.
	if ( strncmp( prompt, "t6", 2 ) == 0 )	
	{
        printf("t6: todo\n");
        goto exit_cmp;					 
	}
	*/

    /*
	// #suspended	
	//t7
	//#test
	//testando estado das teclas.
	if ( strncmp( prompt, "KEYS", 4 ) == 0 ||
         strncmp( prompt, "keys", 4 ) == 0 || 
         strncmp( prompt, "T7", 2 ) == 0 || 		 
	     strncmp( prompt, "t7", 2 ) == 0 )	
	{
		printf("VK_CAPITAL %d \n", system_call ( 138, VK_CAPITAL, VK_CAPITAL, VK_CAPITAL ) );				
	    printf("VK_LSHIFT %d \n", system_call ( 138, VK_LSHIFT, VK_LSHIFT, VK_LSHIFT ) );
		printf("VK_RSHIFT %d \n", system_call ( 138, VK_RSHIFT, VK_RSHIFT, VK_RSHIFT ) );
		printf("VK_CONTROL %d \n", system_call ( 138, VK_CONTROL, VK_CONTROL, VK_CONTROL ) );
		printf("VK_WINKEY %d \n", system_call ( 138, VK_WINKEY, VK_WINKEY, VK_WINKEY ) );
		printf("VK_LMENU %d \n", system_call ( 138, VK_LMENU, VK_LMENU, VK_LMENU ) );
		//...
		goto exit_cmp;
	}
	*/

    /*
	//testando a cria��o de bot�o e a intera��o com ele atrav�s do mouse.
	if ( strncmp( prompt, "t8", 2 ) == 0 )
    {
	    shellTestButtons ();	
		refresh_screen ();
		goto exit_cmp;
	}
	*/

    /*
	if ( strncmp( prompt, "t9", 2 ) == 0 )
    {    
        testShowLines();	
		goto exit_cmp;
	}
	*/

    /*
	if ( strncmp( prompt, "t10", 3 ) == 0 )
    {    
	    testChangeVisibleArea();
		goto exit_cmp;
	}
	*/

    /*
	//t11 - testando o envio de mensagens para o procedimento,
	//usando o kernel.
	if ( strncmp( prompt, "t11", 3 ) == 0 )
    {    
        //chama message box com mensagem about.
        apiSendMessage ( (struct window_d *) 0, 
		                 (int) MSG_COMMAND, 
						 (unsigned long) CMD_ABOUT, 
						 (unsigned long) 0 );
		
		goto exit_cmp;
	}
	*/
	
	//buffer de test;
	unsigned long message_buffer[11];
	
	/*
	// #deprecated
	// t12
	// testando mudar um ret�ngulo de lugar.
	// + salva o ret�ngulo num buffer 
	// + copia do buffer para o backbuffer 
	if ( strncmp( prompt, "t12", 3 ) == 0 )	
	{
        printf("t12: test rect \n");
		
		enterCriticalSection(); 
		message_buffer[0] = 100;
		message_buffer[1] = 100;
		message_buffer[2] = 100;
		message_buffer[3] = 100;
		system_call ( 132,
		    (unsigned long) &message_buffer[0],
			(unsigned long) &message_buffer[0],
			(unsigned long) &message_buffer[0] );
		message_buffer[0] = 50;
		message_buffer[1] = 50;
		message_buffer[2] = 100;
		message_buffer[3] = 100;			
		system_call ( 133,
		    (unsigned long) &message_buffer[0],
			(unsigned long) &message_buffer[0],
			(unsigned long) &message_buffer[0] );
		exitCriticalSection(); 
		
        goto exit_cmp;					 
	}
	*/


    /*
	if ( strncmp( prompt, "t13", 3 ) == 0 )
    {    
        //#obs 
        //#lembrando: podemos ter problemas com o buffer do arquivo.		
	    //coloca no stdout.
		stdout_printf("Escrevendo no stdout com stdout_printf \n");
		//mostra o arquivo desde o come�o.
		printf(stdout->_base);
		goto exit_cmp;
	}
	*/
	
	/*
	// #deprecated
	if ( strncmp( prompt, "t14", 3 ) == 0 )
	{
	    testCreateWindow ();
		printf("t14: debug *hang");
		while(1){}
		goto exit_cmp;
	}
	*/
		
	// t16
	// Testando execve da libc
	// isso funcionou usando o processo init.
	if ( strncmp( prompt, "t16", 3 ) == 0 )
	{
		printf ("Testing execve()\n" );
		execve ("shutdown.bin", NULL, NULL );
		while (1){}
		//exit (1);
		goto exit_cmp;
	}	

    /*
	// #suspended
	//t17 - create process
	void *P;
	if ( strncmp( prompt, "t17", 3 ) == 0 )	
	{
		printf ("Creating processes ...\n");	
	    P = (void *) apiCreateProcess ( 0x400000, PRIORITY_HIGH, "PROCESS-TEST-1" );
	    if ( (void *) P == NULL ){
			printf ("Fail creating process\n");
		};
		goto exit_cmp;
	}
	*/
	
	/*
	// #suspended
	// t18
	// OpenTTY.
	FILE *opentty_fp;
	FILE *terminal_opentty_fp;
	int x_ch;
	int terminal_PID;
	#define MSG_TERMINALCOMMAND 100 //provis�rio
	if ( strncmp ( prompt, "t18", 3 ) == 0 )	
	{
		//get tty stream
		//o shell pega um stream para escrever.
		//pega o stdout do kernel
		opentty_fp = (FILE *) system_call ( 1000, getpid(), 0, 0 );
		fprintf (opentty_fp, "test1 ...\n");
		fprintf (opentty_fp, "test2 ...");   //sem \n
		
		//get tty stream
		//o terminal pegar um stream para ler.
		//terminal_opentty_fp = (FILE *) system_call ( 1001, 0, 0, 0 );
		 
	    //x_ch = (int) fgetc (terminal_opentty_fp);	
		
		//while (1)
		//{
		    //pega um char, mas n�o � o �ltimo que foi colocado, � o que ainda n�o foi pego.
		//    x_ch = (int) system_call ( 1002, 0, 0, 0 );
		//    if (x_ch == '\n'){ break;};	
		//	printf (" CHAR:{%c} \n",x_ch);
	    //}
		
		//get terminal pid
		//avisa o terminal que ele pode imprimir as mesangens pendentes que estao na stream
		
		terminal_PID = (int) system_call ( 1004, 0, 0, 0 );		
		__SendMessageToProcess ( terminal_PID, NULL, MSG_TERMINALCOMMAND, 2000, 2000 );
		
		goto exit_cmp;
	}
	*/
	
    /*
	// #suspended
    // t19
    int xxx__PID;
    if ( strncmp ( prompt, "t19", 3 ) == 0 )
    {
        xxx__PID = (int) system_call ( 900, (unsigned long) "terminal.bin", 0, 0 );
		
		printf ("t19:  xxx__PID = %d \n", xxx__PID);
		//goto exit_cmp;
		
		//registra o terminal
		system_call ( 1003, xxx__PID, 0, 0 ); 
		
		//invalida a vari�vel.
		xxx__PID = -1;
		
		//pega o pid do terminal atual
		xxx__PID = (int) system_call ( 1004, 0, 0, 0 ); 
		
		if ( xxx__PID <= 0 )
		{
			printf ("PID fail. We can't send the message\n");
		    goto exit_cmp;
		}
		
		//manda uma mensagem pedindo para o terminal dizer hello.
		__SendMessageToProcess ( xxx__PID, NULL, MSG_TERMINALCOMMAND, 2001, 2001 );
		goto exit_cmp;
	}
	*/
	
	/*
	// #suspended
	//t20
	//isso executa o terminal e manda uma mensagem pra ele.
	int terminal___PID;
	FILE *_fp;
	if ( strncmp ( prompt, "t20", 3 ) == 0 )
	{
		terminal___PID = (int) apiStartTerminal ();
	    if ( terminal___PID <= 0 ){
		    printf ("PID fail. We can't send the message\n");
	        goto exit_cmp;
	    }else{

			printf ("t20: The terminal PID is %d \n", terminal___PID );		
			printf ("t20: writing in the stdout\n"); //ring3 ??
			
			// #importante
			// Aqui vamos pegar o stdout associado com o terminal atual,
			// mas dever�amos pegar o stdout associado com o tty
			// usado pelo aplicativo.
			// Mesma coisa para o input. A aplicativo deve pegar
			// os chars no stdin do tty associado ao aplicativo e n�o
			// diretamente no driver de terminal.
			// Esse tipo de coisa ajudar� a controlar a concorr�ncia
			// dos aplicativos por input.
			
			//pega o stdout do kernel
			_fp = (FILE *) system_call ( 1000, getpid(), 0, 0 );
			
			//fprintf (stdout,"gdeshell: This is a string ..."); //#bugbug
			fprintf (_fp,"gdeshell: This is a string ..."); //#bugbug
			fprintf (_fp,"gdeshell: We're writing in the terminal's stdout. ");
			//#importante
			//Aqui podemos configurar o terminal.
			//pegar as caracter�sticas do terminal para configurar o app cliente.
			
			//testando mensagens diferentes.
			//#importante: N�o t� dando pra enviar mensagens seguidas.
			//acho que tem que esperar.
			
			//__SendMessageToProcess ( terminal___PID, NULL, MSG_TERMINALCOMMAND, 2001, 2001 );	//ok	
			__SendMessageToProcess ( terminal___PID, NULL, MSG_TERMINALCOMMAND, 2008, 2008 );	//ok			
			//__SendMessageToProcess ( terminal___PID, NULL, MSG_TERMINALCOMMAND, 2009, 2009 );		
			//...
		}		
		goto exit_cmp;
	}
	*/
	
	
    // t900
    // clona e executa o filho dado o nome do filho.
    if ( strncmp ( prompt, "t900", 4 ) == 0 )
    {
	    system_call ( 900, (unsigned long) "shutdown.bin", 0, 0 );
		//system_call ( 900, (unsigned long) "noraterm.bin", 0, 0 );
		goto exit_cmp;
    }

    // flush stdout
    if ( strncmp( prompt, "flush-stdout", 12 ) == 0 ){
		fflush (stdout);
		goto exit_cmp;
	}
	
	/*
	// #suspended
	// tasklist - Lista informa��es sobre os processos.
	//isso ser� um programa tasklist.bin
	if ( strncmp( prompt, "tasklist", 8 ) == 0 ){
		shellTaskList();
        goto exit_cmp;
    }
	*/

    /*
    //test-taskman-server - testando servidor taskman
	if ( strncmp( prompt, "test-taskman-server", 19 ) == 0 )
	{
		//s� podemos enviar a pr�xima mensagem depois que a primeira for atendida.
		//pois ainda n�o temos fila de mensagens e o servidor demora para receber 
		//tempo de processamento.
        system_call ( 116, 123, 0, 0 );
		//system_call ( 116, 1000, 0, 0 );
		//system_call ( 116, 2000, 0, 0 );
		//system_call ( 116, 3000, 0, 0 );
		//system_call ( 116, 4000, 0, 0 );
        goto exit_cmp;
	}
	*/
	
	/*
	// time
	if ( strncmp( prompt, "time", 4 ) == 0 ){
        goto exit_cmp;
    }
	*/
	
	/*
	// timer-test
	// Essa rotina cria um objeto timer que gera um interrup��o 
	// de tempos em tempos e � tratado pelo procedimento de janelas.
	if ( strncmp( prompt, "timer-test", 10 ) == 0 ){

		printf("timer-test: Creating timer\n");
		
	    printf("%d Hz | sys time %d ms | ticks %d \n", 
		    apiGetSysTimeInfo(1), 
			apiGetSysTimeInfo(2), 
			apiGetSysTimeInfo(3) );		
					
		//janela, 100 ms, tipo 2= intermitente.
		//system_call ( 222, (unsigned long) window, 100, 2);	
			
        apiCreateTimer ( (struct window_d *) window, 
            (unsigned long) 50, (int) 2 );			
		
		//inicializando.
        objectX = 0;
        objectY = 0;
        deltaX = deltaValue;
        deltaY = deltaValue;
          
        printf("timer-test: done\n");		  
					
        goto exit_cmp;
    }
	*/

    /*
	// #deprecated
	// taskbar
	// Cria uma top bar.
    if ( strncmp( prompt, "taskbar", 7 ) == 0 )
	{
	    enterCriticalSection();    	    
		//Apenas inicialize. Continuaremos com o procedimento 
		//do shell e n�o o da barra,
		//shellCreateTaskBar (1);
		exitCriticalSection();    
		goto exit_cmp;
    }
	*/
	
	// tree
	// Desenha uma pequena �rvore.
    if ( strncmp( prompt, "tree", 4 ) == 0 ){
		shellTree();
		goto exit_cmp;
    }

	// version
	//?? isso pode ser um programa
    if ( strncmp( prompt, "version", 7 ) == 0 )
	{
		show_shell_version();
	    //printf("\n Gramado version %s \n", OS_VERSION );
        goto exit_cmp;
    }

    // volume-info
	if ( strncmp( prompt, "volume-info", 11 ) == 0 )
	{
	    shellShowVolumeInfo();
        goto exit_cmp;
    }
	
	/*
	// window
    if ( strncmp( prompt, "window", 6 ) == 0 )
	{
		shellShowWindowInfo();
        goto exit_cmp;
    }
	*/

    // Se apertamos o enter e n�o encontramos um comando v�lido
    // ent�o damos um aviso de comando inv�lido e reiniciamos o prompt 
    // na pr�xima linha.
	
	//fail.
	int Execve_Ret = 1;
	
    //unsigned long a1 = (unsigned long) tokenList[0];
	
	//char *a1 = tokenList[1];
	//char *a1 = tokenList[2];

doexec_first_command:

    //
	// ## TEST ##
	//

	// #importante:
	// Se estamos aqui � porque o comando n�o corresponde a nenhuma das 
	// palavras reservadas acima, ent�o executaremos, presumindo ser um nome 
	// de aplicativo no formato 'test.bin'. 
	
	// #importante:
	// Vamos checar se o pathname � absoluto ou relativo.
	// +Se tiver barra, significa que o pathname � absoluto e 
	// come�a no diret�rio raiz do volume do sistema.
	// root:/volume1/
	// +Se come�ar por root:/ tamb�m � absoluto.
	// +Pathname relativo � somente aquele que � apenas um nome 
	// de arquivo dentro do diret�rio atual. Como: 'name' ou name1/name2,
	// sem barra ou ponto.
	
    absolute = absolute_pathname ( (char *) tokenList[0] );
    switch (absolute)
	{
		//N�o � absoluto ou falhar
		case 0:
		    //printf("doexec_first_command: not absolute pathname\n");
		    break;
		//� absoluto	
		case 1:
		    printf("doexec_first_command: absolute pathname\n");
			break;
		//falha	
		case 2:
		default:
		    printf("doexec_first_command: pathname fail\n");
			break;
	};

    // Trata no caso de ser absoluto.
	// + Eliminar ./ pois se trata de arquivo no diret�rio atual.
	if (absolute == 1)
	{
		//#bugbug: Essa defini��o n�o deve ficar aqui.
		char *t = (char *) tokenList[0];
		if ( *t == '.' )
		{
			t++;
		    if ( *t == '/' ){
				t++;
				tokenList[0] = (char *) t;
			}
	    }	
	}
	
	//
	// ## Executando um programa no formato ">test.bin"
	//
	
	//char *tmp[] = { tokenList[1], tokenList[2], tokenList[3], NULL };
	//char *tmp2[] = { tokenList[1], tokenList[2], tokenList[3], NULL };
	
    //Presumindo ser um nome de aplicativo no formato 'test.bin'.
    //o token 0 � o nome do programa e o os pr�ximos s�o argumenteos ..
	//dessa forma podemos enviar um comando e dois argumentos ...
	//?? Talvez devessemos enviar um ponteiro para um array de strings 
	// no �ltimo argumento. (argv) ou ainda um env

    //Execve_Ret = (int) shell_gramado_core_init_execve( 
	//                       (const char*) tokenList[0], //nome
	//                       (const char*) tokenList[1], 
	//					   (const char*) tokenList[2] ); //env ...deve ser null
	
	//#bugbug: e sem argumento.
	/*
	char *p = ( char * ) &prompt[0];
	
	while( *p && *p != ' ' && *p != '\0' )
	{
		p++;
	}
	
	while( *p && *p == ' ' && *p != '\0' )
	{
		p++;
	}
	*/	
	
	//#todo: Colocar isso no in�cio dessa fun��o.
	unsigned long buffer[5];
	int z;
// Colocamos todos os ponteiros no array.
	for ( z=0; z<token_count; z++ ){
	    buffer[z] = (unsigned long) tokenList[z];	
	}						 
    Execve_Ret = 
	    execve ( 
		    (const char *) tokenList[0], //nome
	        (const char *) 0,            //NULL
			(const char *) 0);           //NULL

	// ## ISSO DEU CERTO ## 	
    // Passamos anteriormente a linha de comandos via mem�ria compartilhada,
    // agora ent�o precisamos passar somente o nome do arquivo.	
    //Execve_Ret = (int) shell_gramado_core_init_execve ( 
	//                       (const char *) tokenList[0], //nome
	//                       (const char *) 0,            //NULL
	//					   (const char *) 0);           //NULL
	
    //Execve_Ret = (int) shell_gramado_core_init_execve( 
	//                       (const char*) tokenList[0], //nome
	//                       (const char*) tokenList[1], 
	//					   (const char*) tokenList[2]); //env ...deve ser null
	
	// Ok, funcionou e o arquivo foi carregado,
	// mas demora para receber tempo de processamento.
	if( Execve_Ret == 0 )
	{
		//
		// ## WAIT ??
		//
		
		// Aqui temos uma decis�o a tomar.
		// Se o aplicativo executou corretamente e esta em primeiro 
		// plano ent�o devemos entrar em wait.
		// Se o aplicativo funcionou corretamente mas est� em segundo 
		// plano ent�o decemos continuar. 
		// Por enquanto estamos continuando e rodando concomitantemente.

		// # Stop running #
		//Isso sai do loop de mensagens e 
		//sai do shell elegantemente.
		
		_running = 0;
		
	    goto exit_cmp;	
	}else{
		// falhou. Significa que o servi�o na� conseguir encontrar 
		// o arquivo ou falhou o carregamento.
		printf("shell: execve fail\n");
		goto fail;
	};
	
	
	//
	// # dobin #
	//
	
	//Vamos executar um programa .bin.
	//.bin � a extens�o padr�o.
	//Executaremos o segundo comando, pois o primeiro � ~$dobin.
	//
dobin:

    if ( is_bin( (char *) tokenList[1] ) != 1 )
	{
		printf("dobin: it's not a .bin filename.\n");
		printf("dobin: fail\n");
		//@todo: back ...
	}

// Precisamos passar somente o nome pois a linah de comando 
// j� foi enviada atrav�s de mem�ria compartilhada.
    Execve_Ret = 
	    (int) shell_gramado_core_init_execve( 
	            (const char*) tokenList[1],   // Nome
	            (const char*) 0,              // Null
				(const char*) 0 );            // Null
		 
	// Ok, funcionou e o arquivo foi carregado,
	// mas demora para receber tempo de processamento.
	if ( Execve_Ret == 0 )
	{
		//
		// ## WAIT ??
		//
		
		// Aqui temos uma decis�o a tomar.
		// Se o aplicativo executou corretamente e esta em primeiro 
		// plano ent�o devemos entrar em wait.
		// Se o aplicativo funcionou corretamente mas est� em segundo 
		// plano ent�o decemos continuar. 
		// Por enquanto estamos continuando e rodando concomitantemente.

		// # Stop running #
		//Isso sai do loop de mensagens e 
		//sai do shell elegantemente.
		_running = 0;
	    goto exit_cmp;	
	}else{		
		// falhou. Significa que o servi�o na� conseguir encontrar 
		// o arquivo ou falhou o carregamento.
		printf ("shell: execve fail\n");
		goto fail;
	};	
	
// Tente executar um aplicativo com outra extens�o.
// Checaremos se � uma extens�o das suportadas.	
dotry:

//Precisamos passar somente o nome pois j� passamos 
//a linha de comandos atrav�s de mem�ria compartilhada.
    Execve_Ret = 
	    (int) shell_gramado_core_init_execve( 
	            (const char*) tokenList[1], //Name
	            (const char*) 0,            //Null
				(const char*) 0 );          //Null
		 
	// Ok, funcionou e o arquivo foi carregado,
	// mas demora para receber tempo de processamento.
	if ( Execve_Ret == 0 )
	{
		//
		// ## WAIT ??
		//
		
		// Aqui temos uma decis�o a tomar.
		// Se o aplicativo executou corretamente e esta em primeiro 
		// plano ent�o devemos entrar em wait.
		// Se o aplicativo funcionou corretamente mas est� em segundo 
		// plano ent�o decemos continuar. 
		// Por enquanto estamos continuando e rodando concomitantemente.

		// # Stop running #
		// Isso sai do loop de mensagens e 
		// sai do shell elegantemente.
		_running = 0;		
	    goto exit_cmp;			
	}else{
		// falhou. Significa que o servi�o na� conseguir encontrar 
		// o arquivo ou falhou o carregamento.
		printf ("gdeshell: execve fail\n");
		goto fail;
	};		
	

//
// # Script #
// Um comando no shell invoca a execuss�o de um script 
// dado o nome via tokenList.
dosh:

	// Vamos tentar colocar o arquivo de script no stdin 
	// que � onde fica o prompt. Ent�o retornaremos no 
	// in�cio dessa rotina mas agora com uma nova linha de comando.

    // nothing for now.
	// o comando [0] � o 'dosh' o [1] � o nome do script.
    if ( is_sh1( (char *) tokenList[1] ) != 1 )
	{
		printf("dosh: it's not a .sh1 filename.\n");
		printf("dosh: fail\n");
	}

    // #ok
	// Estamos passando o nome do arquivo.
	
	//#todo:
	//Essa fun��o deve chamar o interpretador de script
	shellExecuteThisScript ( tokenList[1] );	
	
	//#todo 
	//Podemos apenas sair j� a fun��o acima executou o script
	//goto exit_cmp;
	
	goto NewCmdLine;
	
// ---------------------
fail:	
    printf(" Unknown command!\n");
    ret_value=1;
	goto done;

// ---------------------
exit_cmp:
    ret_value = 0;	
done:
// Limpando a lista de argumentos.
// Um array de ponteiros.	
	for ( i=0; i<TOKENLIST_MAX_DEFAULT; i++ ){
		tokenList[i] = NULL;
	};
	shellPrompt ();
	//#bugbug:
	//queremos dar refresh apenas da janela.
	//Mostrando as strings da rotina de compara��o.	
	//refresh_screen();
    return (unsigned long) ret_value;
}

void shellInitSystemMetrics (void)
{
	//pegaremos todas as metricas de uma vez s�,
	//se uma falhar, ent�o pegaremos tudo novamente.
	
	// Tamanho da tela.	
	smScreenWidth = rtl_get_system_metrics(1);
    smScreenHeight = rtl_get_system_metrics(2); 
	smCursorWidth = rtl_get_system_metrics(3);
	smCursorHeight = rtl_get_system_metrics(4);
	smMousePointerWidth = rtl_get_system_metrics(5);
	smMousePointerHeight = rtl_get_system_metrics(6);
	smCharWidth = rtl_get_system_metrics(7);
	smCharHeight = rtl_get_system_metrics(8);	
	//...
} 

void shellInitWindowLimits(void)
{

//
// ## Window limits ##
//

//full screen support
    wlFullScreenLeft = 0;
    wlFullScreenTop = 0;
    wlFullScreenWidth = smScreenWidth;
    wlFullScreenHeight = smScreenHeight;
	
    //limite de tamanho da janela.
    wlMinWindowWidth = smCharWidth * 80;
    wlMinWindowHeight = smCharWidth * 25;
    wlMaxWindowWidth = wlFullScreenWidth;
    wlMaxWindowHeight = wlFullScreenHeight;	
	
    //quantidade de linhas e colunas na �rea de cliente.
    wlMinColumns = 80;
    wlMinRows = 1;
    wlMaxColumns = (wlFullScreenWidth / 8);
    wlMaxRows = (wlFullScreenHeight / 8);
	
	//dado em quantidade de linhas.
    textMinWheelDelta = 1;  //m�nimo que se pode rolar o texto
    textMaxWheelDelta = 4;  //m�ximo que se pode rolar o texto	
	textWheelDelta = textMinWheelDelta;
	//...
}

void shellInitWindowSizes(void)
{
	
//
//  ## Window size ##
//

    //wsWindowWidth = wlMinWindowWidth;
    //wsWindowHeight = wlMinWindowHeight;	
	
	//Tamanho da janela do shell com base nos limites 
    //que ja foram configurados.	
	
	wsWindowWidth =  WINDOW_WIDTH;
	wsWindowHeight = WINDOW_HEIGHT;
	
	if ( wsWindowWidth < wlMinWindowWidth )
	{
		wsWindowWidth = wlMinWindowWidth;
	}
	if ( wsWindowHeight < wlMinWindowHeight )
	{
	    wsWindowHeight = wlMinWindowHeight;	
	}
}

void shellInitWindowPosition(void)
{	
	//window position
	wpWindowLeft = WINDOW_LEFT;
	wpWindowTop = WINDOW_TOP;	
	//wpWindowLeft = (unsigned long) ( (smScreenWidth - wsWindowWidth)/2 );
	//wpWindowTop = (unsigned long) ( (smScreenHeight - wsWindowHeight)/2 );  	
}

// shellShell:
// Constructor.
// N�o emite mensagens.
// #aten��o
// Isso deve ser chamado antes de criarmos a janela principal. 
static void shellShell(void)
{
	register int i=0;
	int j=0;

    shellStatus = 0;
    shellError = 0;
		
// ## Inicializando as estruturas de linha ##
// inicializamos com espa�os.
	for ( i=0; i<32; i++ )
	{
		for ( j=0; j<80; j++ )
		{
		    LINES[i].CHARS[j] = (char) ' ';
		    LINES[i].ATTRIBUTES[j] = (char) 7;
	    }	
		LINES[i].left = 0;
		LINES[i].right = 0;
		LINES[i].pos = 0;
	};

	// Deve ser pequena, clara e centralizada.
	// Para ficar mais r�pido.
	
	// #importante:
	// O aplicativo tem que confiar nas informa��es 
	// retornadas pelo sistema.
		
	// Usar o get system metrics para pegar o 
	// tamanho da tela.
	
	//inicializa as metricas do sistema.
	shellInitSystemMetrics ();
	
    //inicializa os limites da janela.
	shellInitWindowLimits ();
	
	//inicia o tamanho da janela.
	shellInitWindowSizes ();
	
	//inicializar a posi��o da janela.
	shellInitWindowPosition ();
  
	// initi visible area.
	// #todo: criar fun��o para isso
	
	textTopRow = 0;
	textBottomRow = 24;
	
	//limits.
	//quantidade de linhas de colunas da janela.
	//na verdade deve ser da �rea de cliente.
    wlMaxColumns = (wsWindowWidth/8);    
    wlMaxRows = (wsWindowHeight/8);         
 
    if ( wlMaxColumns < wlMinColumns )
	{
	    wlMaxColumns = wlMinColumns;	
	}
		
	if ( wlMaxRows < wlMinRows )
	{
		wlMaxRows = wlMinRows;
	}
	
	//...	

	//
	// Setup buffers.
	//
	
	// #inportante:
	// #bugbug: N�o podemos deixar o stdout como screenbuffer 
	// pois no screen buffer os caracteres tem atributos ...
	// e no stdout podemos ter um arquivo normal ...
	//ou sa�da normal que servir� de entrada para  
	//outro processo.
    // reiniciando as vari�veis na estrutura do output
	
	//obs: Ao cancelarmos isso o std volta a ser 
	//o antigo e j� configurado stdout.
	//stdout->_base = &screen_buffer[0];
	//stdout->_ptr = stdout->_base;
	//stdout->_cnt = PROMPT_MAX_DEFAULT;
	//stdout->_file = 1;
	//stdout->_tmpfname = "shell_stdout";
	
	//...	
	
	// Obs:
	// prompt[] - Aqui ficam as digita��es. 

	shellClearBuffer();

	
	//shellBufferMaxColumns = DEFAULT_BUFFER_MAX_COLUMNS;
	//shellBufferMaxRows    = DEFAULT_BUFFER_MAX_ROWS;
	
	//buffersize = (shellBufferMaxColumns * shellBufferMaxRows);
	
	//
	// @todo: E o fluxo padr�o. Quem configurou os arquivos ???
	//        o kernel configuroru???
	//
	
	// N�mero m�ximo de colunas e linhas.
	g_columns = wlMaxColumns;  // 80;
	g_rows = wlMaxRows;        // 25;
    //...
	
	
	for ( i=0; i<WORKINGDIRECTORY_STRING_MAX; i++ ){
		current_workingdiretory_string[i] = (char) '\0';
	};
	
    sprintf ( 
        current_workingdiretory_string, 
        SHELL_UNKNOWNWORKINGDIRECTORY_STRING );    
	
	//...

//done:
    ShellFlag = SHELLFLAG_COMMANDLINE;
	
    //#bugbug
	//Nossa refer�ncia � a moldura e n�o a �rea de cliente.
	//@todo:usar a �rea de cliente como refer�ncia
	//shellSetCursor(0,0);
    //shellSetCursor(0,4);
    
	//@todo
	//tentando posicionar o cursor dentro da janela
	//shellSetCursor( (shell_info.main_window->left/8) , (shell_info.main_window->top/8));	
	
	//shellPrompt();
}


/*
 * shellInit:
 *     Inicializa o Shell.  
 *     #bugbug: 
 *     Essa rotina come�a escrever na janela com o foco de entrada. 
 * Mas um outro aplicativo solicitou o foco de entrada e essa rotina 
 * esta terminando de escrever mas agora na janela do outro aplicativo.
 * @todo: Inicializar globais e estruturas.
 */

int shellInit (void)
{
	//#bugbug 
	//o ponteiro do argumento pode ser inv�lido, pois 
	//� uma vari�ve� local.
	
	int PID;
	int PPID;
	int ActiveWindowId = 0;
	int WindowWithFocusId = 0;
	void *P;
	//int CurrentVolumeID = 0;
		
	char buffer[512];
	
	// #bugbug:
    //     Esse ponteiro de estrutura est� em kernel mode. 
	//     N�o podemos us�-lo.
	
	//??
	//stream status
	shell_info.stream_status = 0;
	
	//
	// ## buffer support ##
	//
		
    /* 
	Line buffer output for stderr. 
    If your machine doesn't have either of setlinebuf or setvbuf,
    you can just comment out the buffering commands, and the shell
    will still work.  It will take more cycles, though. 
	*/
	
/*++ HAVE_SETLINEBUF */	
/*
#if defined (HAVE_SETLINEBUF)
    setlinebuf(stderr);
    setlinebuf(stdout);
#else
# if defined (_IOLBF)
    setvbuf( stderr, (char *) NULL, _IOLBF, BUFSIZ );
    setvbuf( stdout, (char *) NULL, _IOLBF, BUFSIZ );
# endif
#endif
*/ 
/*-- HAVE_SETLINEBUF */
	
	
	// Antes dessa fun��o ser chamada, o foco foi setado 
	// na janela do aplicativo.
	// Test: N�o mechemos no cursor nesse momento, deicharemos 
	// a fun��o SetFocus configurar o curso.
	// N�o mostraremos o prompt, somente depois dos testes de inicializa��o.
	
    //bugbug	
	//cursor
	//shellSetCursor(0,4);
	
	//pointer
	//shellPrompt();
    
	// message.
    //printf("shellInit: Running tests ...\n");	


	//
	// Window support.
	//

	// Testando a validade do ponteiro da janela.
	// #importante: A estrutura est� em kernel mode 
	// e n�o podemos acessa-la.
	
	/*
	if ( (void *) window == NULL ){
	    printf("shellInit: window fail.\n"); 
	} else {
		// Nesse momento temos um ponteiro v�lido,
		// mas infelismente n�o podemos testar outros elementos 
		// da estrutura.
		APISetFocus ( window );
		// mensagens !!
        //#ifdef SHELL_VERBOSE		
		//printf ("shellInit: Starting gdeshell.bin ...\n");
		//printf ("shellInit: Running tests ...\n");
        //#endif
	};
    */

//
// Obtendo informa��es sobre a janela ativa.
//
	
	/*
	//Active window
	ActiveWindowId = (int) APIGetActiveWindow ();
	//valor de erro
	if ( ActiveWindowId == (-1) ){	
	    printf("shellInit: ERROR getting Active window ID\n");	
	}	
	*/
	
    //#ifdef SHELL_VERBOSE			
	//printf("ActiveWindowId={%d}\n", ActiveWindowId );
    //#endif
	
	// Obtendo informa��es sobre a janela com o foco de entrada.

    /*
	// Focus.
	WindowWithFocusId = (int) APIGetFocus();
	//valor de erro
	if ( WindowWithFocusId == (-1) ){
	    printf("shellInit: ERROR getting Window With Focus ID\n");	
	}
	*/

    //#ifdef SHELL_VERBOSE			
	//printf("WindowWithFocusId={%d}\n", WindowWithFocusId );	
    //#endif
	
	//
	// Obetendo informa��es sobre linhas e colunas do shell.
	//
	
    //#ifdef SHELL_VERBOSE		
	//columns and rows
	//printf("wlMaxColumns={%d} \n", wlMaxColumns );
	//printf("wlMaxRows={%d} \n", wlMaxRows );	
    //#endif
	
	//
	// Process support.
	//
	
	// @todo: 
	// Essa mensagem est� aparecendo fora da �rea de trabalho do shell
	// pois ainda n�o temos um ponteiro para a mem�ria que representa essa �rea.
	// Talvez as mensagens devessem ir para um buffer de linha antes de irem 
	// para a mem�ria de v�deo.
	// #Impotante:
	// Devemos utilizar as configura��es de terminal virtual, respeitar a estrutura 
	// de terminal, que indicar� qual � a janela de terminal onde os caracteres 
	// devem ser escritos. Na verdade � um ponteiro para um ret�ngulo e n�o para 
	// uma janela. Obs: Esse ret�ngulo do terminal deve esr configur�vel e uma rotina 
	// deve dar suporte a essa configura��o.
	
	// @todo: 
	// O que tevemos fazer aqui � pegar informa��es sobre o processo Shell
	// e coloca-las na tela.

	// @todo: Criar na API uma rotina de inteface que use essa chamada.
	// ex: APIGetPID().
	
	//PID = (int) APIGetPID();

    PID = (int) system_call( SYSTEMCALL_GETPID, 0, 0, 0 );
	if ( PID == (-1) ){
	    printf("ERROR getting PID\n");	
	}
  
    PPID = (int) system_call( SYSTEMCALL_GETPPID, 0, 0, 0 );
	if ( PPID == (-1) ){
	    printf("ERROR getting PPID\n");	
	}
  
    //Mensagem ...
	//printf("Starting SHELL.BIN ... PID={%d} PPID={%d} \n", PID, PPID );
	
	
	// @todo: Criar processos processos:
	//     E.BIN, F.BIN, G.BIN, A.BIN, B.BIN, C.BIN, D.BIN
 	//     (Mermaids) Usados para testes.

//#ifdef SHELL_VERBOSE			
//	printf("Creating processes ...\n");
//#endif

/*
    printf("Creating process ...\n");	
	//D.:)
	P = (void *) apiCreateProcess ( 0x400000, PRIORITY_HIGH, "NEW-PROCESS");
	if ( (void *) P==NULL )
	{
		printf ("Fail creating process D :)\n");
	};
*/	

/*	
#ifdef SHELL_VERBOSE
    
    printf("Creating processes ...\n");	
	//D.:)
	P=(void*)apiCreateProcess(0x400000,PRIORITY_HIGH,"D");
	if((void*)P==NULL){printf("Fail creating process D :)\n");};
	//C.
	P=(void*)apiCreateProcess(0x400000,PRIORITY_HIGH,"C");
	if((void*)P==NULL){printf("Fail creating process C.\n");};
	//B.
	P=(void*)apiCreateProcess(0x400000,PRIORITY_HIGH,"B");
	if((void*)P==NULL){printf("Fail creating process B.\n");};
	//A.(Balance).
	P=(void*)apiCreateProcess(0x400000,PRIORITY_NORMAL,"A");	
	if((void*)P==NULL){printf("Fail creating process A.\n");};
	//G.
	P=(void*)apiCreateProcess(0x400000,PRIORITY_LOW,"G");	
	if((void*)P==NULL){printf("Fail creating process G.\n");};
	//F.
	P=(void*)apiCreateProcess(0x400000,PRIORITY_LOW,"F");
	if((void*)P==NULL){printf("Fail creating process F.\n");};
	//E.
	P=(void*)apiCreateProcess(0x400000,PRIORITY_LOW,"E");	
	if((void*)P==NULL){printf("Fail creating process E :( \n");};
	
	printf("Created!\n");
	//...
#endif	
*/	
	
	//
	//@todo: 
	// Devemos implementar uma rotina de inicializa��o dessas threads 
	// criadas atrav�s de chamadas ao sistema.
	//
	
	//printf("Initializing a thread...\n");
	//...
	
	//Testando bibliotecas.
	//printf("Testing libraries:\n");	
	
	//stdio.h
	//printf("Testing stdio:\n");
	//app_clear(0);  //N�o fez nada.
    //...

	//stdlib.h
    //#ifdef SHELL_VERBOSE			
	//printf("Testing stdlib:\n");
    //#endif
	
	// Importante:
	// Isso inicializa o gerenciamento de mem�ria oferecido pela 
	// biblioteca C99 em user mode. Coisas como inicializar o 'heap'
	// oferecido pela biblioteca.
	// Agora poderemos alocar mem�ria dentro do heap oferecido pela biblioteca.
	
//initRT:	
	//libcInitRT(); 

    /*
    #ifdef SHELL_VERBOSE			
	//Obs: Sempre inicia com o mesmo n�mero.
	int rand_value;
	rand_value = (int) rand();
	printf("RandValue1={%d}\n", rand_value);
	//rand_value = (int) rand();
	//printf("RandValue2={%d}\n", rand_value);
    //rand_value = (int) rand();
	//printf("RandValue3={%d}\n", rand_value);
	//...
    #endif	
    */

	//stddef.h
	//printf("Testing stddef:\n");	
	//Testing COLUMNS
	//Testing ROWS
	
	//testando api.h
	//...
	
	//Funcionou...
	//sprintf( buffer,"Testando String dentro do buffer\n");
	//printf("%s",buffer);
	
	// Get process info.
	// Get thread info.

	//
	// Testing commands.
	//

    //#ifdef SHELL_VERBOSE		
	//Lib C.
	//libC. (stdlib.c)
	//system("test");       
	//system("ls");
	//system("start");
	//system("xxfailxx");
	//...
    //#endif
	
	
    //#ifdef SHELL_VERBOSE			
	//API.
	//apiSystem("test");    
    //apiSystem("ls");
	//apiSystem("start");
	//apiSystem("xxfailxx");
	//...
    //#endif
	
	//Ok funcionando ...
	//@todo: Testar outros comandos.
	//...
	
	//Testing message box.
	//MessageBox(3, "Shell message box","Testing message box...");
	
	// @todo: Clear client area.
	//        System call redraw client area.
		
	// ## prompt string support ##
	shellInitializeWorkingDiretoryString ();

// Done
done:
    
	/*
	   #test 
	   #bugbug    
	   Estamos supendendo a rotina de password porque 
	   estamos testando o inpot de mensagens.
	   Sendo que o input do password � diferente do input 
	   de mensagens.
       em seguida trablharemos nesse input  tamb�m
       Esse input � o input usado pela libc.
     */

	// #bugbug
	// suspendendo isso porque deu porblemas. #pf
	
	/*
	if ( shellCheckPassword() != 1 ){		
	    printf("shellCheckPassword FAIL \n");		
	}

	// @todo:
	// Gerenciamento do heap do processo. ??
	
	// @todo:
	// Chamar rotinas da biblioteca que ofere�am informa��es sobre 
	// o heap oferecido pela biblioteca.
	// Obs: A rt foi inicializada logo acima.
	
    //heapTest:
    /*	
	printf("\n...\n");
	printf("Testing C99 RT ...\n");
	
	unsigned long hStart, hEnd, hPointer, hAvail;
	
	hStart   = (unsigned long) rtGetHeapStart();
	hEnd     = (unsigned long) rtGetHeapEnd();
	hPointer = (unsigned long) rtGetHeapPointer();
	hAvail   = (unsigned long) rtGetAvailableHeap();
	
	printf("heapStart{%x} heapEnd{%x} heapPointer{%x} heapAvailable={%x}\n", 
	     hStart, hEnd, hPointer, hAvail);
	
	// resultados do teste:
	// os valores parecem satisfat�rios pois est�o realmente dentro da �rea 
	// de mem�ria do aplicativo.
	// @todo: Confirmar no memorymap gerado pelo compilador se essa �rea de mem�ria 
	// � apropriada. #bugbug
	// observando o mmmap pelo jeito o compilador colocou o buffer do heap 
	// no bss do arquivo e do tamanho certo.
	// tudo indica que � saud�vel aumentar o tamanho do buffer usado pelo heap.
	*/

// Show prompt
	if (interactive == 1)
	    shellPrompt ();

	// #bugbug 
	// Vamos retornar sem mostrarmos a janela 
	// ou podemos confiar no ponteiro passado via argumento 
	// e efetuarmos o refresh da janela.
	
    //refresh_screen ();
	
	//isso deve mostrar as string
	//Se � que isso � preciso, pois 
	//a rotina de printf mostra as strings.
	//>>vamos tentar sem isso e confiarmos na fun��o printf.
	//apiShowWindow(window);
	
    return 0;
}

int shellCheckPassword (void)
{	
    char buffer[512];	
	
	// Se o shell n�o for interativo n�o tem login.
	if (interactive == 1)
	{
		//hostname
        current_host_name = "??host??";				
		//file 
		
		FILE *user_stream;
		
		user_stream = (FILE *) fopen ("user.txt","w+");
		
		//#todo check.
		
		// Testing welcome message.
	    //printf("\n");
	    printf("\n Welcome to Norax Kernel! \n");
	    //printf("\n");
	
		//  ## username  ##
	    printf("\n username: ");
	    gets(username);
		current_user_name = username;
		
		//  ## password ##
		printf("\n password: ");
	    gets(password);
		
		printf("\n");
	
        //#ifdef SHELL_VERBOSE	
        //@todo colocar o ponteiro na vari�vel no in�cio do arquivo.	
	    printf("username={%s} password={%s} \n", username, password );
		//printf("\n");
        //#endif
		
		char *c = (char *) &user_stream->_base[0];
		int i;

		//procura user name.
		while ( *c && *c != 'U' ){			
			c++;
		};
		
		if ( c[0] == 'U' &&
             c[1] == 'S' &&
             c[2] == 'E' &&
             c[3] == 'R' &&
             c[4] == 'N' &&
             c[5] == 'A' &&
             c[6] == 'M' && 			
		     c[7] == 'E' )
		{
		    //USERNAME={fred}
            c = c+10; 
			
		    
			//Move apenas 'fred'
		    for ( i=0; i<4; i++ ){
			    buffer[i] = c[i];
		    }			
			
			//printf("\n");
			//printf("%c", c[0]);
			//printf("%c", c[1]);
			//printf("%c", c[2]);
			//printf("%c", c[3]);
			//printf("\n");
			
			//printf("\n");
			//printf("%c", buffer[0]);
			//printf("%c", buffer[1]);
			//printf("%c", buffer[2]);
			//printf("%c", buffer[3]);
			//printf("\n");
			
            //#ifdef SHELL_VERBOSE			
			printf(">>%s\n", username);
			printf(">>%s\n", buffer);
            //#endif
			
            if( strncmp( username, buffer, 4 ) == 0 )
            {
                //#ifdef SHELL_VERBOSE				
				printf("# USERNAME OK #\n");
                //#endif				
				login_status = 1;
			}else{
				printf("# USERNAME FAIL #\n");
				login_status = 0;
			};				

        }else{
			printf("# USERNAME FAIL #\n");
			login_status = 0;
		};

		while ( *c && *c != 'P' ){			
			c++;
		};
		
		if ( c[0] == 'P' &&
             c[1] == 'A' &&
             c[2] == 'S' &&
             c[3] == 'S' &&
             c[4] == 'W' &&
             c[5] == 'O' &&
             c[6] == 'R' && 			
		     c[7] == 'D' )
		{
		    //PASSWORD={1234}
            c = c+10; 
			
		    //Move apenas '1234'
		    for ( i=0; i<4; i++ ){
			    buffer[i] = c[i];
		    };

            //#ifdef SHELL_VERBOSE				
			printf(">>%s\n", password);
			printf(">>%s\n", buffer);
            //#endif			
			
            if ( strncmp( password, buffer, 4 ) == 0 )
            {
                //#ifdef SHELL_VERBOSE								
				printf("# PASSWORD OK #\n");
                //#endif
				login_status = 1;
			}else{
				printf("# PASSWORD FAIL #\n");
				login_status = 0;
			};					
			
		}else{
		    printf("# PASSWORD FAIL #\n");
            login_status = 0; 			
		};
		    
	}else{
		printf("shell not interactive\n");
		login_status = 0;
	};
	
    //#ifdef SHELL_VERBOSE
	printf("Login done!\n");
    //#endif
	
	return (int) login_status;
}

/*
 * shellSetCursor:
 *     Configurando o cursor. (stdio.h).
 *
 * @todo: Aqui quando definimos os valores o cursor no shell 
 * devemos considerar que a janela com o foco de entrada tambem tem um cursor...
 * Temos que atualizar o cursor da janela com foco de entrada se quizermos 
 * escrever corretamente dentro dela.
 * e isso se faz atrav�s de uma chamada ao kernel.
 */
void shellSetCursor ( unsigned long x, unsigned long y )
{	
    //
	// Coisas do kernel.
	//
	
	// #deprecated
	//setando o cursor usado pelo kernel base.	
    //apiSetCursor (x,y);
	
//Atualizando as vari�veis globais usadas somente aqui no shell.
//setGlobals:	
    g_cursor_x = (unsigned long) x;
    g_cursor_y = (unsigned long) y;	
		
// Coisas do screen buffer.
	move_to ( x, y);
}

// shellThread:
// Um thread dentro para testes.
void shellThread (void)
{
// Handler
	printf("\n");
	printf("$\n");
	printf("$$\n");
	//printf("$$$\n");
    printf("#### This is a thread ####\n");
	//printf("$$$\n");
	printf("$$\n");
	printf("$\n");
    printf("\n");
    while(1){	
		asm ( "pause" );
    }	
}

//help message
void shellHelp (void)
{	
    printf (help_banner);	
}

//drawing a tree
void shellTree (void)
{	
    printf (tree_banner);	
}

/*
 * shellPrompt:
 *     Inicializa o prompt.
 *     Na inicializa��o de stdio, 
 *    prompt foi definido como stdin->_base.
 */
void shellPrompt (void)
{	
    register int i=0;

// Linpando o buffer de entrada.	
	for ( i=0; i<PROMPT_MAX_DEFAULT; i++ ){
		prompt[i] = (char) '\0';
	}
    prompt[0] = (char) '\0';
	prompt_pos = 0;
    prompt_status = 0;
	prompt_max = PROMPT_MAX_DEFAULT;  

	//old
    //printf ("\n");
    //printf ("[%s]", current_workingdiretory_string );	
	//printf ("%s ", SHELL_PROMPT );
    printf ("\n");
    printf ("[%s", current_user_name );
	printf ("@%s]", current_host_name );
	printf ("%s ", SHELL_PROMPT );		
}

// shellClearBuffer:
// Limpa o buffer da tela.
void shellClearBuffer (void)
{
	int i = 0;
	int j = 0;
	
//inicializamos com espa�os.
	for ( i=0; i<32; i++ )
	{
		for ( j=0; j<80; j++ )
		{
		    LINES[i].CHARS[j] = (char) ' ';
		    LINES[i].ATTRIBUTES[j] = (char) 7;
	    }
		LINES[i].left = 0;
		LINES[i].right = 0;
		LINES[i].pos = 0;
	};
}

/*
 * shellShowScreenBuffer:
 *     Quando for efeturar o refresh da tela
 * � o conte�do desse buffer que deve ser pintado 
 * no ret�ngulo que � a �rea de cliente do shell.  
 */
//mostra o buffer screen buffer, onde ficam 
//armazenados os caracteres e atributos datela
//do terminal virtual.
//#importante: vamos mostrar todo o buffer de words, a partir 
//da posi��o atual do cursor, for�ando um scroll
//Isso � s� um teste.

void shellShowScreenBuffer (void)
{	
    shellRefreshVisibleArea ();
}

// shellTestLoadFile:
// Carrega um arquivo de texto no buffer e mostra na tela.
void shellTestLoadFile (void)
{
	FILE *f;	
	int Ret;
	register int i=0;
	int ch_test;
	int pos;

	f = fopen ("gramado.txt","rb");  		
	//f = fopen ("init.txt","rb");  	

    if (f == NULL){
		printf("fopen fail\n");
		return;
	}else{
		//printf("fopen ok\n");
	};	

//#test 
//testando com um arquivo com texto pequeno.
//enviando para o buffer de words, 
//obs: agora tem rotinas de refresh.

// Testing getc() on f.
	while (1){
		ch_test = (int) getc(f); 
		if ( ch_test == EOF ){
			//printf("\n");
			//printf("EOF reached :)\n");
			goto done;
		} else {   			
			shellInsertNextChar((char) ch_test); 		
	    };
	};	
 
done:
fail:
    return;	
}

/*
 * shellTestThreads:
 *     Cria um thread e executa.
 *     #bugbug ...j� funcionou uma vez, mas agora est� com problemas.
 *     @todo: na hora de criar a thread precisamos passar o PID desse processo.
 */
 
void shellTestThreads (void)
{
// #suspended

/*
    void *T;	
	
	// Obs: 
	// As threads criadas aqui s�o atribu�das ao processo PID=0.
	// @todo: 
	// No kernel, quando criar uma thread ela deve ser atribu�da
    // ao processo que chamou a rotina de cria��o.	
	
	printf ("shellTestThreads: Creating threads..\n");
	//apiCreateThread((unsigned long)&shellThread, 0x004FFFF0,"TestShellThread1");
	//apiCreateThread((unsigned long)&shellThread, 0x004FFFF0,"TestShellThread2");
	//apiCreateThread((unsigned long)&shellThread, 0x004FFFF0,"TestShellThread3");
	//apiCreateThread((unsigned long)&shellThread, 0x004FFFF0,"TestShellThread4");
	//...
	
	//
	// # Criar e executar #
	//
	
	// Tentando executar um thread.
    // OBS: 
	// ISSO J� FUNCIONOU. 
	// ESTAMOS SUSPENDENDO PORQUE PRECISAMOS AUMENTAR O 
	// TAMANHO DO HEAP USADO PELO PROCESSO PARA 
	// ALOCA��O DIN�MICA, ELE N�O T� DANDO CONTA 
    // DE TODA A DEMANDA POR MEM�RIA.		  
	
	//>>Dessa vez pegaremos o retorno, 
	// que deve ser o ponteiro para a estrutura da thread.
	// Obs: N�o podemos usar a estrutura porque ela est� 
	// em ring0.
	//>>Chamaremos a system_call que executa essa thread 
	// que temos o ponteiro da estrutura.
    
	void *ThreadTest1;	
	
	//#bugbug: 
	// N�o temos mais espa�o no heap do preocesso 
	// para alocar mem�ria pois gastamos o heap com 
	// a imagem bmp. (isso aconteceu kkk).

	unsigned long *threadstack1;
	
	
	enterCriticalSection ();
	
	// #importante:
	// Como a torina de thread � bem pequena e o 
	// alocador tem pouqu�ssimo heap, vamos alocar o m�nimo.
	// Isso � apenas um teste, vamos var se a thread funciona 
	// com um a pilha bem pequena. 2KB.
	
	threadstack1 = (unsigned long *) malloc (2*1024);
	
	//Ajuste para o in�cio da pilha.
	//threadstack1 = ( threadstack1 + (2*1024) - 4 ); 
	
	//
	// # Criando a thread #
	//
	
//creating:

    printf ("shellTestThreads: Tentando executar uma thread..\n");	
	
	ThreadTest1  = (void *) apiCreateThread ( (unsigned long) &shellThread, 
	                            (unsigned long) (&threadstack1[0] + (2*1024) - 4), 
							    "ThreadTest1" );
	
	if ( (void *) ThreadTest1 == NULL )
	{	
	    printf("shellTestThreads: apiCreateThread fail \n");	
	    die("ThreadTest1");
	}
	
	// # executando #
	
	// #importante:
	// L� no kernel, isso deve selecionar a thread para 
	// execuss�o colocando ela no estado standby.
	// Logo em seguida a rotinad e taskswitch efetua o spawn.
	
	apiStartThread(ThreadTest1);
	exitCriticalSection();
	
	
	printf ("gdeshell: Tentando executar um thread [ok]..\n");
	
	//permitir que o shell continue.
*/

}


/*
 * shellClearScreen:
 *     Limpar a tela do shell.
 *     usada pelo comando 'cls'.
 */ 
void shellClearScreen (void)
{
// #suspended
/*
	struct window_d *w;
	unsigned long left, top, right, bottom;
	
// Desabilita o cursor
	system_call ( 245, (unsigned long) 0, (unsigned long) 0, (unsigned long) 0);	
	
	shellClearBuffer ();
	
	
	w = (void *) shell_info.terminal_window;
	if ( (void *) w != NULL ){
		APIredraw_window ( w, 1 );
	}

	
    left = (terminal_rect.left/8);
    top = (terminal_rect.top/8);
	
    shellSetCursor ( left, top );

	// Copiamos o conte�do do screenbuffer para 
	// a �rea de cliente do shell.
	// obs: A outra op��o seria repintarmos a janela.

    //shellRefreshScreen ();	
	
	//shellRefreshVisibleArea();
	
	//reabilita o cursor
	system_call ( 244, (unsigned long) 0, (unsigned long) 0, (unsigned long) 0);	
*/
}

/*
 * shellRefreshScreen:
 *     #importante 
       #OBS
       NA VERDADE SE REFERE AO REFRESH DA �REA DE CLIENTE DO SHELL.
 *     Copia o conte�do do (screen_buffer) buffer de output 
 * para a tela. (dentro da janela). 
 * ## Acho que se trata de stdout.
 * � uma mem�ria VGA virtual com caractere e atributo.
 * na hora de efetuar refresh precisamos considerar o atributo 
 * para sabermos a cor do caractere e de seu background.
 */

void shellRefreshScreen (void)
{
	int i=0;
	int j=0;

	//desabilita o cursor
	//system_call ( 245, (unsigned long) 0, (unsigned long) 0, (unsigned long) 0);
	
	for ( i=textTopRow; i<textBottomRow; i++ )
	{
		for ( j=0; j<80; j++ )
		{
		    //LINES[i].CHARS[j] = (char) 'x';
		    //LINES[i].ATTRIBUTES[j] = (char) 7;
	        
			printf ("%c", LINES[i].CHARS[j] );
		}
		printf ("\n");
	};

	//reabilita o cursor
	//system_call ( 244, (unsigned long) 0, (unsigned long) 0, (unsigned long) 0);		
}


// a inten��o aqui � fazer o refresh de apenas uma linha do arquivo.
//#todo podemos fazer o mesmo para um char apenas.
void shellRefreshLine ( int line_number )
{	
    
	if ( line_number > wlMaxRows )
		return;	
	
	int lin = (int) line_number; 
	int col = 0;  
	
	
    //#ifdef SHELL_VERBOSE		
	//#debug
	printf("shellRefreshScreen:\n");
    //#endif 

	//cursor apontando par ao in�cio da janela.
	//usado pelo printf.
	//@todo: podemos colocar o cursor no 
	//in�cio da �rea de cliente.
	//left ser� a coluna.
	
	shellSetCursor ( col, lin );
		
	//colunas.
	for ( col=0; col < wlMaxColumns; col++ )
	{
	    //Mostra um char do screen buffer.
		printf( "%c", LINES[lin].CHARS[col] );
	};	
}

// a inten��o aqui � fazer o refresh de apenas uma linha do arquivo.
//#todo podemos fazer o mesmo para um char apenas.
// #importante
// Rever essa rotina, pois vamos usar
// a nova vers�o da libc que envia os caracteres para o arquivo
// ficando por conta do terminal imprimi-los em sua �rea de cliente.
// #bugbug: Aqui, por enquanto, esse printf envia ele mesmo
// os chars para a tela.

void shellRefreshChar ( int line_number, int col_number )
{	
	if ( col_number > wlMaxColumns || line_number > wlMaxRows )
		return;
	
	shellSetCursor ( col_number, line_number );

	//Mostra um char do screen buffer.
	printf ("%c", LINES[line_number].CHARS[col_number] );	
}

//refresh do char que est� na posi��o usada pelo input.
// #importante
// Rever essa rotina, pois vamos usar
// a nova vers�o da libc que envia os caracteres para o arquivo
// ficando por conta do terminal imprimi-los em sua �rea de cliente.
// #bugbug: Aqui, por enquanto, esse printf envia ele mesmo
// os chars para a tela.
void shellRefreshCurrentChar (void)
{	
	printf ("%c", LINES[textCurrentRow].CHARS[textCurrentCol] );
}

/*
 * shellScroll:
 *     @todo: Efetuar um scroll somente dentro 
 * da VGA virtual.
 * #importante: isso n�o deveria estar aqui,
 * deve ser uma rotina de automa��o, presente 
 * em alguma biblioteca, servidor ou kernel.
 */
void shellScroll (void)
{	
	//reajustando a �rea vis�vel do buffer 
 
	//desliga o cursor
	//system_call ( 245, (unsigned long) 0, (unsigned long) 0, (unsigned long) 0);	

    testChangeVisibleArea();	//desloca a �rea vis�vel usando delta.
	shellRefreshVisibleArea();
	
	//#todo:posicionar cursor
	
	//reabilita o cursor
	//system_call ( 244, (unsigned long) 0, (unsigned long) 0, (unsigned long) 0);	
}

static void save_cur (void)
{	
	textSavedCol = textCurrentCol;
	textSavedRow = textCurrentRow;
}

static void restore_cur (void)
{	
	textCurrentCol = textSavedCol;
	textCurrentRow = textSavedRow;
}

//line feed
static void lf (void)
{	
	//enquanto for menor que o limite de linhas, avan�a.
	if ( textCurrentRow+1 < wlMaxRows )
	{
		textCurrentRow++; 
		return;
	}
	
	//#todo: Scroll up;
	//scrup();
}

// ??
//voltando uma linha.
static void ri (void)
{	
	//if ( screen_buffer_y > top ){
		
		// Volta uma linha.
	//	screen_buffer_y--;
	//	screen_buffer_pos = (screen_buffer_pos - columns); 
	//	return;
	//}
	
	//@todo:
	//scrdown();
}

//carriege return
static void cr (void)
{	
    textCurrentCol = 0;
}

static void del (void)
{	
	LINES[textCurrentRow].CHARS[textCurrentCol] = (char) '\0';
	LINES[textCurrentRow].ATTRIBUTES[textCurrentCol] = 7;
}


// Insere um caractere sentro do buffer.
void 
shellInsertCharXY ( 
    unsigned long x, 
    unsigned long y, 
	char c )
{
	if ( x >= wlMaxColumns || y >= wlMaxRows )
	{	
		return;
	}
	LINES[y].CHARS[x] = (char) c;
	LINES[y].ATTRIBUTES[x] = 7;
}


 // Insere um caractere sentro do buffer.
char 
shellGetCharXY ( 
	unsigned long x, 
    unsigned long y )
{
	if ( x >= wlMaxColumns || y >= wlMaxRows )
	{	
		return 0;
	}
	return (char) LINES[y].CHARS[x];
}


/*
//inserindo uma string em uma posi��o do buffer de sa�da.
void shellInsertStringPos( unsigned long offset, char *string );
void shellInsertStringPos( unsigned long offset, char *string )
{
    //@todo
};
*/

/*
 preenche todo o buffer de sa�da com char ou atributo
void shellFillOutputBuffer( char element, int element_type )
{
	
}
*/

//usado para teste de scroll.
//imprime varias vezes o char indicado.
void testScrollChar (int c)
{	
    register int i=0;	
    for ( i=0; i < (wlMaxColumns*26); i++ )
	{
	    //se chegamos no limite do screen_buffer
		//...
		shellInsertNextChar ((char) c);	
	}		
}

/*
 * shellInsertNextChar:
 *     Coloca um char na pr�xima posi��o do buffer.
 *     Mem�ria de v�deo virtual, semelhante a vga.
 */
void shellInsertNextChar (char c)
{		
	//cursor da linha
	LINES[textCurrentRow].CHARS[textCurrentCol] = (char) c;
	
	//refresh
	shellRefreshCurrentChar();
	
	//update
	textCurrentCol++;
	if (textCurrentCol >= 80 )
	{
		textCurrentCol = 0;
		textCurrentRow++;
		if ( textCurrentRow >= 25 )
		{
			shellScroll ();
			while(1){}
		}
	}
	LINES[textCurrentRow].pos = textCurrentCol;
	LINES[textCurrentRow].right = textCurrentCol;
}

void shellInsertCR (void)
{    
	shellInsertNextChar ( (char) '\r' );		
}

void shellInsertLF (void)
{	
	shellInsertNextChar ( (char) '\n' );
}

void shellInsertNullTerminator (void)
{
	shellInsertNextChar ( (char) '\0' );	
}

/*
 * shellTestMBR:
 *     Testar a leitura de um setor do disco.
 *     Testaremos o setor do mbr.
 */
// #suspended
void shellTestMBR (void)
{
	
/*
	unsigned char buffer[512];
	
	enterCriticalSection(); 
	
	//message 
	printf("shellTestMBR: Initializing MBR test ...\n");
	
	//read sector
	system_call ( SYSTEMCALL_READ_LBA, 
	              (unsigned long) &buffer[0],  //address 
				  (unsigned long) 0,           //lba
				  (unsigned long) 0);
				 
	
	exitCriticalSection();   
	
	//
	// exibe o conte�do carregado.
	//
	
	//?? address #bugbug
	printf("Signature: [ %x %x ] \n" , buffer[0x1FE], buffer[0x1FF] );

	// @todo: Sondar cada elemento do MBR para 
	// confirmar a presen�a.
*/
}

/*
 * move_to:
 *    Move o cursor de posi��o.
 *    Assim o pr�ximo char ser� em outro lugar da janela.
 */

void move_to ( unsigned long x, unsigned long y )
{	
	if ( x > wlMaxColumns || y > wlMaxRows )
		return;
	
	//screen_buffer_x = x;
	//screen_buffer_y = y;
	textCurrentCol = x;
	textCurrentRow = y;
	
	//screen_buffer_pos = ( screen_buffer_y * wlMaxColumns + screen_buffer_x ) ;
}

// show shell info
void shellShowInfo (void)
{
	int PID, PPID;
	
    printf (" # shellShowInfo: #\n");
	
	/*
    PID = (int) system_call( SYSTEMCALL_GETPID, 0, 0, 0);
	if ( PID == (-1))
	{
	    printf("ERROR getting PID\n");	
	}
  
    PPID = (int) system_call( SYSTEMCALL_GETPPID, 0, 0, 0);
	if ( PPID == (-1))
	{
	    printf("ERROR getting PPID\n");	
	}
	*/
  
	PID = getpid();
	PPID = getppid();
	printf ("Process info: PID={%d} PPID={%d} \n", PID, PPID );
	//printf ("wlMaxColumns={%d} \n", wlMaxColumns );
	//printf ("wlMaxRows={%d} \n", wlMaxRows );	
	//...
}

// #suspended
void shellShowMetrics (void)
{
/*
    //reinicializa as metricas do sistema.
	//isso pega os valores e coloca nas vari�veis globais.
	
	shellInitSystemMetrics ();
	
	printf ("\n");  
	printf (" # shellShowMetrics: # \n");
	
	printf ("screenWidth={%d} screenHeight={%d}\n",smScreenWidth, smScreenHeight );
	
	printf ("cursorWidth={%d} cursorHeight={%d}\n", smCursorWidth, smCursorHeight );
	
	printf ("mousepointerWidth={%d} mousepointerHeight={%d}\n", 
	    smMousePointerWidth, smMousePointerHeight );
	
	printf ("charWidth={%d} charHeight={%d}\n", smCharWidth, smCharHeight );	
		
    printf ("Done\n");	
*/
}

// #suspended
// show system info
void shellShowSystemInfo (void)
{

/*
	int ActiveWindowId;
	int WindowWithFocusId;
	
	printf (" # shellShowSystemInfo: #\n");
	
	//Active
	ActiveWindowId = (int) APIGetActiveWindow ();
	
	//valor de erro
	if ( ActiveWindowId == (-1))
	{
	    printf("ERROR getting Active window ID\n");	
	}	
	printf ("ActiveWindowId={%d}\n", ActiveWindowId );


	// Focus.
	WindowWithFocusId = (int) APIGetFocus ();
	
	//valor de erro
	if ( WindowWithFocusId == (-1))
	{
	    printf("ERROR getting Window With Focus ID\n");	
	}	
	printf ("WindowWithFocusId={%d}\n", WindowWithFocusId );	
*/
}

// #deprecated
void shellShowWindowInfo (void)
{

/*	
    int wID;	
	
	// #bugbug.
	// Testando a estrutura de janela.
	// A janela foi criada pelo kernel e copiamos o ponteiro 
	// da estrutura para um ponteiro em user mode.
	// Podemos ter erros de mem�ria com essas opera��es.
		
	printf ("\n");	
	printf (" # shellShowWindowInfo #\n");
	
	printf ("mainWindow={%x}", shell_info.main_window );
		
	//#bugbug 
	//temos um problema aqui.
	// provavelmente o erro � por acessar um endere�o que est� 
	// em kernel mode.
	//if( shell_info.main_window->left > 0 && shell_info.main_window->top > 0  )
	//{
	//    shellSetCursor( (shell_info.main_window->left/8), (shell_info.main_window->top/8) );
	//}
		
		
	//obs: Isso � uma estrutura interna, n�o reflete 
    //a informa��o usada pelo kernel.	
	
	printf ("\n");		
	printf ("Window info: \n");	
    printf ("l={%d} t={%d} w={%d} h={%d}\n", 
	    wpWindowLeft, wpWindowTop, wsWindowWidth, wsWindowHeight );

													  
	//Obs: isso funcionou. setando o cursor.
	//if( terminal_rect.left > 0 && terminal_rect.top > 0 )
	//{											  
    //    shellSetCursor( (terminal_rect.left/8), (terminal_rect.top/8) );													  
	//};
		
	// #importante:
	// Getting terminal window.
		
	wID = (int) system_call ( SYSTEMCALL_GETTERMINALWINDOW, 0, 0, 0 ); 
	
	// Check?
	
	printf ("current terminal: ");
	printf ("Windows ID for current terminal = {%d} \n", wID);
*/
}

//void shellSetScreenColors( ... ){}
//void *shellGetTerminalWindow(){}
//void shellSetTerminalRectangle(....){}
//void *shellOpenTerminal(.){}
		  
unsigned long 
shellSendMessage ( 
    int msg, 
	unsigned long long1, 
	unsigned long long2 )
{
    return 0;
	//return (unsigned long) shellProcedure ( window, msg, long1, long2 );
}

void 
shell_memcpy_bytes( 
    unsigned char *Dest, 
    unsigned char *Source,
    unsigned long Length )
{
    while (Length--)
        *Dest++ = *Source++;
}


/*
 * shell_write_to_screen:
 *     refresh de um ret�ngulo ??    
 */
/* 
void shell_write_to_screen( struct shell_screen_d *screen, 
                            struct shell_rect_d *region )
{
    //@todo	
}
*/


void shellExit (int code)
{
	exit(code);
}
 

/*
 * shellUpdateWorkingDiretoryString:
 *     Atualiza a string do diret�rio de trabalho.
 * Essa � a string que ser� mostrada antes do prompt.
 * 'pwd'> 
 * ?? isso deve sser todo o pathname do pwd ?? 
 * ex: root:/volume0>
 */

void shellUpdateWorkingDiretoryString (char *string)
{
	if (pwd_initialized == 0){
		goto fail;
	}else{
		
        if( (void *) string == NULL )
	    {
		    goto fail;
	    }else{
	    
	        // ## separador ##
		    //strcat( current_workingdiretory_string, 
			//    SHELL_PATHNAME_SEPARATOR );		
		
	        // ## separador ##		
		    strcat ( current_workingdiretory_string, string );	

		    strcat( current_workingdiretory_string, 
			    SHELL_PATHNAME_SEPARATOR );				
		
            //Atualizar no gerenciamento feito pelo kernel.
	        system_call( 175, (unsigned long) string,
		        (unsigned long) string, (unsigned long) string );		
		};
	};
	//...
fail:	
done:
    return;
}

/*
 * shellInitializeWorkingDiretoryString:
 *     Atualiza a string do diret�rio de trabalho.
 * Essa � a string que ser� mostrada antes do prompt.
 * 'pwd'> 
 * ?? isso deve sser todo o pathname do pwd ?? 
 * ex: root:/volume0>
 */
void shellInitializeWorkingDiretoryString (void)
{
	//get info
	
	//test: get current volume id.
	current_volume_id = (int) system_call ( 171, 0, 0, 0 );
	
	//global usada para string do nome do volume.
	current_volume_string = (char *) SHELL_VOLUME1_STRING;
	
	
    //  ## volume list ##
	
	//root:
    //primeiro colocamos a string que indica 
	//a lista de volumes.
    sprintf ( current_workingdiretory_string, SHELL_VOLUMELIST_STRING ); 
	
	
	// ## separador ##

	//root:/
	strcat ( current_workingdiretory_string, SHELL_PATHNAME_SEPARATOR );
	
	//  ## volume root dir ##
	
	//root:/volumex
	strcat ( current_workingdiretory_string, current_volume_string );

	// ## separador ##

	//root:/volumex/
	strcat ( current_workingdiretory_string, SHELL_PATHNAME_SEPARATOR );
	
    pwd_initialized = 1;
}

// atualiza a vari�vel global para id de diret�rio atual de trabalho.
void shellUpdateCurrentDirectoryID (int id)
{
    if (id<0)
	    return;
	g_current_workingdirectory_id = (id);
}

// Lista informa��es sobre os processos.
// op��es:
// +podemos pedir para o kernel listar as informa��es.
// +podemos solicitar as informa��es uma a uma.
// Obs: A segunda op��o � a mais trabalhosa, mas 
// da oportunidade para testarmos as chamadas ao kernel e 
// explorarmos as possibilidades que cada informa��o traz 
// individualmente.

// #todo
// Devemos considerar aqui o que � trabalho do shell e o que �
// trabalho do terminal virtual.

void shellTaskList (void)
{	

/*
	// testando posicionamento de strings
	unsigned long X, Y;
	int PID;

	//Pega o PID do processo atual.
    PID = (int) system_call( SYSTEMCALL_GETPID, 0, 0, 0 );
	
    //X = apiGetCursorX();
	Y = apiGetCursorY();
	
	Y++;
	X=0;
	shellSetCursor(X,Y);	
    printf("PID ");
	X=8;
	shellSetCursor(X,Y);
	printf("XXXXXXXX");
	
	Y++;
	X=0;
	shellSetCursor(X,Y);
    printf("====");
	X=8;
	shellSetCursor(X,Y);
	printf("========");

	Y++;
	X=0;
	shellSetCursor(X,Y);	
    printf("%d",PID);
	X=8;
	shellSetCursor(X,Y);
	printf("...");
*/
}

void shellShowPID (void)
{
	printf ("~ Current PID %d\n", (int) getpid() );	
}

void shellShowPPID (void)
{
	printf ("~ Current PPID %d\n", (int) getppid() );
}

void shellShowGID(void)
{
	printf ("~ Current GID %d\n", (int) getgid() );
}

void shellShowUID (void)
{
	printf ("~ Current UID %d\n", (int) getuid() );
}

void shellShowUserSessionID (void)
{
/*
	printf ("Current user session %d\n", 
	    (int) system_call( SYSTEMCALL_GETCURRENTUSERSESSION, 0, 0, 0) );
*/
}

void shellShowWindowStationID (void)
{
/*	
	printf ("Current room %d\n", 
	(int) system_call( SYSTEMCALL_GETCURRENTWINDOWSTATION, 0, 0, 0) );
*/
}

void shellShowDesktopID (void)
{
/*	
	printf ("Current desktop %d\n", 
	    (int) system_call( SYSTEMCALL_GETCURRENTDESKTOP, 0, 0, 0) );
*/
}

void shellShowProcessHeapPointer (void)
{
/*	
	int id = (int) getpid(); 	
	unsigned long heap_pointer = 
	    (unsigned long) system_call( 
			SYSTEMCALL_GETPROCESSHEAPPOINTER, id, 0, 0 );
	printf ("Current Process heap pointer address %x\n", 
	    (unsigned long) heap_pointer );
*/
}

void shellShowKernelHeapPointer (void)
{
/*
    //??
	//Id do processo kernel.
	int id = 0;   
	
	unsigned long heap_pointer = 
	    (unsigned long) system_call( 
		    SYSTEMCALL_GETPROCESSHEAPPOINTER, id, 0, 0 );

	printf ("Current Process heap pointer address %x\n", 
	    (unsigned long) heap_pointer );
*/
}

//mostra informa��es sobre o disco atual.
void shellShowDiskInfo (void)
{	
	//@todo: atualizar api.h
	system_call ( 251, 0, 0, 0 );
}

//mostra informa��es sobre o volume atual.
void shellShowVolumeInfo (void)
{	
	//@todo: atualizar api.h
	system_call ( 252, 0, 0, 0 );
}

//mostrar informa��es gerais sobre a mem�ria.
void shellShowMemoryInfo (void)
{	
	system_call ( SYSTEMCALL_MEMORYINFO, 0, 0, 0 );
}

//mostrar informa��es gerais sobre a mem�ria.
void shellShowPCIInfo (void)
{	
    system_call ( SYSTEMCALL_SHOWPCIINFO, 0, 0, 0 );	
}

//mostrar informa��es gerais sobre a mem�ria.
void shellShowKernelInfo (void)
{	
	system_call ( SYSTEMCALL_SHOWKERNELINFO, 0, 0, 0 );
}

/*
 * shell_fntos:
 *     Rotina interna de support.
 * isso deve ir para bibliotecas depois.
 * n�o tem prot�tipo ainda.
 * Credits: Luiz Felipe
 */
void shell_fntos (char *name)
{	
    int  i, ns = 0;
    char ext[4];
    //const char ext[4];
	
    //transforma em mai�scula
	while ( *name && *name != '.' )
	{
        if(*name >= 'a' && *name <= 'z')
            *name -= 0x20;

        name++;
        ns++;
    }

    // aqui name[0] � o ponto.
	// ent�o constroi a extens�o.
	for ( i=0; i < 3 && name[i+1]; i++ )
	{
        if(name[i+1] >= 'a' && name[i+1] <= 'z')
            name[i+1] -= 0x20;

        ext[i] = name[i+1];
    };

    while (ns < 8)
	{
        *name++ = ' ';
        ns++;
    };

    for ( i=0; i < 3; i++ )
        *name++ = ext[i];

    *name = '\0';
}

/*
 * shell_gramado_core_init_execve:
 *     Essa � uma rotina alternativa que roda um processo usando os recursos 
 * do processo init.
 */									 
int 
shell_gramado_core_init_execve ( 
    const char *arg1,     // nome
    const char *arg2,     // arg (endere�o da linha de comando)
    const char *arg3 )    // env
{
	//erro.

    int Status = 1;

	//unsigned long arg_address = (unsigned long) &argv[0];

	// suprimindo dot-slash
	// The dot is the current directory and the 
	// slash is a path delimiter.
	//if( filename[0] == '.' && filename[1] == '/' )
	//{ 
	//    filename++;
	//    filename++; 
	//    goto translate;	
	//};

	//suprimindo a barra.
	//if( *arg1 == '/' || 
	//    *arg1 == '\\' )
	//{ 
	//    arg1++; 
	//};

translate:

	// ## BUG BUG

	// Talvez nesse momento, ao transformar a string ele 
	// corrompa o espa�o reservado para o argumento seguinte.
	// vamos fazer um teste no quan a rotina n�o precise 
	// acrescentar zeros.

	// correto � isso mesmo,
	// para n�o corromper o espa�o dos argumentos no vetor,
	// teremos que transformar somente l� no kernel, pouco antes 
	// de carregarmos o arquivo.

	//Isso faz uma convers�o de 'test.bin' em 'TEST    BIN'.
	//Ok funcionou.
	//shell_fntos( (char *) arg1);
	
	//const char* isso n�o foi testado.
	//shell_fntos(filename);

	// #importante:
	// Isso deve chamar gramado_core_init_execve() na api.

	// #obs:
	// isso chamar� uma rotina especial de execve, somente  
	// usada no ambiente gramado core. 
	// Essa � uma rotina alternativa que roda um processo usando os recursos 
	// do processo init.
	
execve:

	// Obs: 
	// Se retornar o n�mero do processo ent�o podemos esperar por ele 
	// chamando wait (ret);

    Status = 
	    (int) system_call ( 
			    167, 
                (unsigned long) arg1,    // Nome
                (unsigned long) arg2,    // arg (endere�o da linha de comando)
                (unsigned long) arg3 );  // env

    if ( Status == 0 )
    {
		//N�o houve erro. O aplicativo ir� executar.

		// Nesse momento devemos usar um novo procedimento de janela.
		// Que vai enviar as mensagens de caractere para um terminal 
		// espec�fico, para que aplicativos que user aquele terminal 
		// possam pegar essas mensgens de caractere.

        //#ifdef SHELL_VERBOSE
        printf ("gdeshell: aplicativo inicializado.\n");
        //#endif

		// ## teste ##
		// saindo do shell.

		// getpid...
		// waitforpid(?);
		
		//die("Exiting shell.bin\n");
		
		//Saindo sem erro.
		//exit(0);
		
		//Sa�da elegante, retornando para o crt0.
		ShellFlag = SHELLFLAG_EXIT;
		
		//ShellFlag = SHELLFLAG_FEEDTERMINAL;		
		goto done;
	}else{
		
		// Se estamos aqui � porque ouve erro 
		// ainda n�o sabemos o tipo de erro. 
		// Status indica o tipo de erro.
		// Se falhou significa que o aplicativo n�o vai executar,
		// ent�o n�o mais o que fazer.
		
		//#importante: Error message.
		printf("shell: aplicativo nao foi inicializado.\n");
		
		ShellFlag = SHELLFLAG_COMMANDLINE;
		goto fail;
	};

//fail.
// Retornaremos. 
// Quem chamou essa rotina que tome a decis�o 
// se entra em wait ou n�o.
fail:
    //#importante: Error message.
    //status = 1.
    printf ("shell_gramado_core_init_execve: [F] Retornando ao interpretador\n");
done:
    return (int) Status;
}

// feedterminalDialog:
// Para alimentar um terminal com caracteres. 
int 
feedterminalDialog ( 
    struct window_d *window, 
    int msg, 
    unsigned long long1, 
    unsigned long long2 )
{
	//int q;

	switch (msg)
	{
	
	    //para alimentar o terminal
		case MSG_KEYDOWN:
			//para todas as teclas.
			//feed terminal system call.
			system_call( 135,  // n�mero do servi�o 
			             0,    // n�mero do terminal
						 long1,    // o caractere
						 long1 );  // o caractere

			break;
			
		//para sair do di�logo.	
		case MSG_SYSKEYDOWN:
		    switch(long1)
			{

                //
                // ?? #importante:
				// ?? Quando parar de alimentar o terminal ??
                // ## [CONTROL + C]  ou [ESC]  ## 
				// >>> F12 POR ENQUANTO PARA TESTES.
				//
				//help
				case VK_F1:
				    //APISetFocus(i1Window);
					//APIredraw_window(i1Window);
					//MessageBox( 1, "feedterminalDialog","F1: HELP");
					break;
				
                //full screen
                //colocar em full screen somente a �rea de cliente. 				
		        case VK_F2:
				    //APISetFocus(i2Window);
					//APIredraw_window(i2Window);				
				    //MessageBox( 1, "feedterminalDialog","F2: ");
					//ShellFlag = SHELLFLAG_COMMANDLINE;
					break;
					
				case VK_F3:
				    printf("F3: Saindo do aplicativo e voltando ao shell...\n");
				    ShellFlag = SHELLFLAG_COMMANDLINE;
				    break;
					
					
			};
		    break;
			
		default:  
		    break;
	
    };
	
    return 0;
}

// #aten��o
// A fun��o fprintf est� emd esenvolvimento. Pode falhar.
void die (char *str)
{	
    if ((void*) str == NULL)
	    exit(1);
	printf ("die: %s",str);	
	fprintf ( stderr, "%s\n", str );
	exit(1);
}

/*
//isso foi para stdlib.c
void *xmalloc( int size){
	
    register int value = (int) malloc(size);
    if(value == 0)
        die ("xmalloc fail.\n");
//done:  
    return (void *) value;
};
*/

// concat:
// Concatena strings.
char *concat ( char *s1, char *s2, char *s3 )
{	
    int len1 = (int) strlen (s1);
    int len2 = (int) strlen (s2);
    int len3 = (int) strlen (s3);
  
    char *result = (char *) xmalloc ( len1 +len2 +len3 +1 );

    strcpy ( result, s1);
    strcpy ( result +len1, s2 );
    strcpy ( result +len1 +len2, s3 );
  
    *( result +len1 +len2 +len3 ) = 0;

    return (void *) result;
}

void error ( char *msg, char *arg1, char *arg2 )
{	
    fprintf ( stderr, "shell: ");
    fprintf ( stderr, "%s %s %s", msg, arg1, arg2 );
    fprintf ( stderr, "\n");
}

void fatal ( char *msg, char *arg1, char *arg2 )
{	
    error ( msg, arg1, arg2 );	
    //delete_temp_files ();
    exit (1);
}

char *save_string ( char *s, int len )
{	
    register char *result = (char *) xmalloc (len + 1);
    bcopy (s, result, len);
	result[len] = 0;

    return result;
}

/*
 * shellExecuteThisScript:
 *     Aqui temos que carregar o arquivo de script indicado 
 * nos argumentos.
 * #importante:
 * Apenas colocaremos o arquivo em stdin, que � o pormpt[]
 * e retornaremos ao incio da fun��o que compara, para 
 * comparar agora o que estava escrito no arquivo de script.
 * 	#todo:
 *	Essa fun��o deve chamar o interpretador de script
 * IN: Nome do arquivo.
 */

int shellExecuteThisScript (char *script_name)
{
 	//#todo:
	//Essa fun��o deve chamar o interpretador de script	
	
	FILE *script_file;	
	register int i=0;
	
	// Aqui temos que carregar o arquivo de script indicado 
	// nos argumentos.

	printf("shellExecuteThisScript:\n");	
	//printf("Initializing script ...\n");
    printf("CurrentFile={%s}\n",script_name);	

// #ok 
// Carregaremos o arquivo com o nome passado por argumento.		
    script_file = fopen (script_name,"rw");
	if ( (void *) script_file == NULL ){	
		printf ("shellExecuteThisScript: Can't open script file!\n");
		die ("*");
	}

//#Ok
//atualizando a linha de comandos no prompt[], stdin.
	
	//#obs 
	//Talvez n�o precise copiar o conte�do, e sim apenas 
    //mudar os ponteiros.
    
    //#todo 	
	//Fazer isso ao inv�s de copiar.
	//stdin = script_file;
	
	for ( i=0; i< 128; i++ ){	
		stdin->_base[i] = script_file->_base[i];
	};

	// ## parser ##
	//#todo
	//Chamaremos o parser, que num loop chamar� o lexer yylex()	
	//parser();
	
	//EOF_Reached = EOF;

    return 0;		
}

/*
char **
array_of_strings()
{
	char **a;
	
	a[0] = "test0";
	a[1] = "test1";
	a[2] = NULL;
	
	return (a);
};
*/

/*
//retorna o pool de strings contendo os nomes dos arquivos
//em um diret�rio que est� no buffer.
char **
get_dir_files( char *buffer )
{
    char **poll;	
	// ...
	
    return (poll);	
}
*/


/*
//coloca no pool de strings passado por argumento 
//as strings que correspondem  aos nomes 
// dos arquivos encontrados no diret�rio 
//indicado pelo buffer.
int set_dir_files( char **poll, char *buffer )
{
	//...	
    return (0);
};
*/

/*
 * absolute_pathname:
 * Retorna 1 nos seguintes casos:
 * >/
 * >.
 * >./
 * >..
 * >../
 * Credits: bash 1.05
 */

// #importante:
// Vamos checar se o pathname � absoluto ou relativo.
// +Se tiver barra, significa que o pathname � absoluto e 
// come�a no diret�rio raiz do volume do sistema. root:/volume1/
// +Se come�ar por root:/ tamb�m � absoluto.
// +Pathname relativo � somente aquele que � apenas um nome 
// de arquivo dentro do diret�rio atual. Como: 'name' ou name1/name2,
// sem barra ou ponto.
// $/ � o diret�rio raiz do volume do sistema.
// $v/ � o diret�rio raiz do vfs.
// #todo
// rever isso.
// absoluto � o que come�a no diret�rio raiz '/' ou 'root:/'	

int absolute_pathname (char *string)
{

// Parameter
    if ( !string || !strlen(string) )
        return (0);

	//Come�a por barra.
	//Ent�o essa barra significa o diret�rio raiz do 
	//volume do sistema. Ex: root:/volume1/
    if ( *string == '/' )
        return (1);

	// .
    // � absoluto, pois estamos determinando o diret�rio.
	// Se for ponto, avan�a em seguida.
	if( *string++ == '.' )
    {
        // ./
		// Significa que se trata do diret�rio atual
		if( (!*string) || *string == '/' )
	        return (1);

        // ..
		// ../
		// Significa que se trata do diret�rio pai.
		// Se for ponto, avan�a em seguida.
		// se for barra ou nada � absoluto.
		//obs: o nada nesse caso significa que foi digitado apenas '..'
		if( *string++ == '.' )
	        if( !*string || *string == '/' )  
	            return (1);
		
		// ?? Continua ...
		// Se estamos aqui, significa que � um ponto que n�o � seguido 
		// de outro ponto ou barra.
		//?? dever�amos falhar com a op��o 2. 
		return (2);
    }
	
	// root:/
	if ( *string == 'r' )
	{
	    if ( string[0] == 'r' &&
             string[1] == 'o' &&
             string[2] == 'o' &&
             string[3] == 't' &&
             string[4] == ':' &&
             string[5] == '/' )
	    {
		    return (1);		 
		}

        return (2);		
	}
	
	// #teste:
	// invendando esquema.
	// Volume do sistema.
	// O n�mero do volume do sistema � um padr�o.
	if ( *string == '$' )
	{
		
	    // root:/volume1/ = $/ (volume do sistema)	    
		if ( string[0] == '$' && 		
		     string[1] == '/' )
	    {
		    return (1);		 
		}
		
	    // root:/volume1/ = $v/ (vfs)
		if ( string[0] == '$' &&
             string[1] == 'v' && 		
		     string[2] == '/' )
	    {
		    return (1);		 
		}

        return (2);		
	}

// fail: NOT absolute.
    return 0;
}

// inicializaremos o supporte a pathname
int shellInitPathname (void)
{
	register int i=0;

	if (pathname_initilized == 1)
	{
		return 0;
		//goto done;
	}
	
	for ( i=0; i<PATHNAME_LENGHT; i++ ){
		pathname_buffer[i] = (char) '\0';
	};
	
	pathname_lenght = 0;
	//...
//done:	
    pathname_initilized = 1;
	return 0;
}
 
// inicializaremos o supporte a filename
int shellInitFilename (void)
{
	register int i=0;

	if (filename_initilized == 1)
	{	
		return 0;
		//goto done;
	}
	
	for ( i=0; i<FILENAME_LENGHT; i++ ){
		filename_buffer[i] = (char) '\0';
	};
	filename_lenght = 0;	
	//...

//done:	
    filename_initilized = 1;
	return 0;
}

/* 
 Remove the last N directories from PATH.  
 Do not leave a blank path.
 PATH must contain enough space for MAXPATHLEN characters. 
 Credits: bash 1.05
 */
//#obs: This is a cool thing!

void shell_pathname_backup ( char *path, int n )
{	
    register char *p = path + strlen(path);	
	unsigned long saveN = (unsigned long) n;

    //#debug 
	//printf("%s", path);
	
	if (*path)
        p--;

    while (n--)
    {
        while(*p == '/')
	        p--;

        while(*p != '/')
	        p--;

        *++p = '\0';
    };

//@todo: Criar em kernelmode uma roptina que 
//que fa�a o mesmo que a shell_pathname_backup 	
//Atualizar no gerenciamento feito pelo kernel.

// ??	
	system_call ( 
		176, 
		(unsigned long) saveN, 
		(unsigned long) saveN, 
        (unsigned long) saveN );			   	
}

// Imprime todas as strings de um vetor de ponteiros.
// Isso vai ser usado pelo echo.
void shell_print_tokenList ( char *token_list[], char *separator )
{	
	register int i=0;
	char *token;
	
	token = (char *) token_list[0];	
	if ( token == NULL )
	    return;
	    //goto fail;

	//token = (char *) tokenList[i];
		
	//	if( token == NULL ){
		
    //#todo: Limits.		
    
	for ( i=0; i,128; i++ )
    {
		token = (char *) token_list[i];
	    if ( token == NULL )
	        return;
		if ( strncmp ( (char *) token_list[i], "echo", 4 ) == 0 )
		    continue;	
		printf ("%s", token_list[i] );
		printf ("%s", separator );
    };
}

/* 
This way I don't have to know whether fclose is a function or a macro. 
Credits: bash 1.05.
 */
/*
int
stream_close( FILE *file );
int
stream_close( FILE *file )
{
     return ( fclose(file) );
};
*/

/* 
 Return the octal number parsed from STRING, or -1 to indicate
 that the string contained a bad number. 
 Credits: bash 1.05.
 */
/* 
int
read_octal( char *string );
int
read_octal( char *string )
{
    int result = 0;
    int digits = 0;
    
	while( *string && 
	       *string >= '0' && 
		   *string < '8' )
    {
        digits++;
        result = (result * 8) + *string++ - '0';
    };

    if(!digits || result > 0777 || *string)
        result = -1;

    
done:	
	return (result);
};
*/

/*  Credits: bash 1.05. */
/*
void 
decrement_variable( int *var );
void 
decrement_variable( int *var )
{
    *var = *var - 1;
};
*/


/* Check if it's a .bin file */
int is_bin (char *cmd)
{	
    char *p;	
	p = cmd;
	
	int len = strlen(p);

	if( len <= 4 ) 
		return 0;
	
    p += len - 4;
    
	if ( *p++ != '.' ) 
		return 0;
	
    if ( strncmp ( (char *) p, "bin", 3 ) == 0 ){
	    return 1;	
	}

    return 0;
}

/* Check if it's a .sh1 file */
int is_sh1 ( char *cmd )
{	
    char *p;	
	p = cmd;
	
	int len = strlen (p);
    
	if ( len <= 4 ) 
		return 0;
	
    p += len - 4;
    
	if ( *p++ != '.' ) 
		return 0;
	
    if ( strncmp ( (char *) p, "sh1", 3 ) == 0 ){
	    return 1;	
	}
	
    return 0;
}

// Give version information about this shell. 
void show_shell_version (void)
{	
    printf ("%s, version %s.%s \n", 
	    shell_name, dist_version, build_version );
}

//testando a rotina de salvar um arquivo.
//estamos usando a API.
	//
	// #importante:
	// N�o podemos chamar a API sem que todos os argumentos estejam corretos.
	//
	
	// #obs:
	// Vamos impor o limite de 4 setores por enquanto. 
	// 512*4 = 2048  (4 setores) 2KB
	// Se a quantidade de bytes for '0'. ???

int shell_save_file (void)
{
// #suspended
/*
	int Ret;	
	char file_1[] = "t5: Arquivo \n escrito \n em \n user mode. \n";
	char file_1_name[] = "FILE1UM TXT";
	unsigned long number_of_sectors = 0;
    size_t len = 0;
	
	printf ("shell_save_file: Salvando um arquivo ...\n");	

// Lenght in bytes.
	len = (size_t) strlen (file_1);

	if (len <= 0)
	{
	    printf ("shell_save_file:  Fail. Empty file.\n");
        return (int) 1;		
	}
	
	if (len > 2048)
	{
	    printf ("shell_save_file:  Limit Fail. The  file is too long.\n");
        return (int) 1;		
	}
	
// Number os sectors.
	number_of_sectors = (unsigned long) ( len / 512 );
	
	if ( len > 0 && len < 512 ){
	    number_of_sectors = 1; 
    }		
	
	if ( number_of_sectors == 0 )
	{
	    printf ("shell_save_file:  Limit Fail. (0) sectors so save.\n");
        return (int) 1;				
	}
	
	//limite de teste.
	//Se tivermos que salvar mais que 4 setores.
	if ( number_of_sectors > 4 )
	{
	    printf ("shell_save_file:  Limit Fail. (%d) sectors so save.\n",
		    number_of_sectors );
        return (int) 1;				
	}

    Ret = (int) apiSaveFile ( file_1_name,  // name 
                    number_of_sectors,      // number of sectors.
                    len,                    // size in bytes			
                    file_1,                 // address
                    0x20 );                 // flag		
				
	//if (Ret == 0)
	
	printf ("shell_save_file: done\n");	
	
	return (int) Ret;
*/
    return 0;
}

/*
 Credits: gcc 0.9 
 Read chars from INFILE until a non-whitespace char
 and return that.  
 
 Comments, both Lisp style and C style, are treated 
 as whitespace.
 
 Tools such as genflags use this function.  
 */
/* 
int
read_skip_spaces (infile)
    FILE *infile;
{
    register int c;
    while (c = getc (infile))
    {
        if (c == ' ' || c == '\n' || c == '\t' || c == '\f')
	    ;
            else if (c == ';')
	             {
	                 while ((c = getc (infile)) && c != '\n') ;
	             }
                 else if (c == '/')
	                  {
	                      register int prevc;
	                      c = getc (infile);
	                      //if (c != '*')
	                          //dump_and_abort ('*', c, infile);
	  
	                      prevc = 0;
	                      while (c = getc (infile))
	                      {
	                          if (prevc == '*' && c == '/')
		                      break;
	                          prevc = c;
	                      }
	                  }
                      else break;
    }
    return c;
};
*/


/* 
 Credits: gcc 0.9
 Read an rtx code name into the buffer STR[].
 It is terminated by any of the punctuation chars 
 of rtx printed syntax.  
 */
/*
static void
read_name (str, infile)
     char *str;
     FILE *infile;
{
  register char *p;
  register int c;

  c = read_skip_spaces(infile);

  p = str;
  while (1)
    {
      if (c == ' ' || c == '\n' || c == '\t' || c == '\f')
	break;
      if (c == ':' || c == ')' || c == ']' || c == '"' || c == '/'
	  || c == '(' || c == '[')
	{
	  ungetc (c, infile);
	  break;
	}
      *p++ = c;
      c = getc (infile);
    }
  *p = NULL;
}
*/

//Qual ser� a linha que estar� no topo da janela.
void textSetTopRow ( int number )
{
    textTopRow = (int) number; 	
}

int textGetTopRow (void)
{
    return (int) textTopRow; 	
}

//Qual ser� a linha que estar� na parte de baixo da janela.
void textSetBottomRow ( int number )
{
    textBottomRow = (int) number; 	
}

int textGetBottomRow (void)
{
    return (int) textBottomRow; 	
}

void clearLine ( int line_number )
{	
    int lin = (int) line_number; 
	int col; 	
	int Offset = 0; //Deslocamento dentro do screen buffer.

	//cursor apontando par ao in�cio da janela.
	//usado pelo printf.
	//@todo: podemos colocar o cursor no 
	//in�cio da �rea de cliente.
	//left ser� a coluna.
	shellSetCursor ( col, lin );
		
//colunas.
	for ( col=0; col < wlMaxColumns; col++ )
	{
	    //Mostra um char do screen buffer.
		printf( "%c", screen_buffer[Offset] );
		    
		Offset++; //ignora o atributo.
	    Offset++;
	};
	
    //shell_buffer_pos = 0;  //?? posi��o dentro do buffer do shell.	
}

//um teste mostrando todas as linhas do boffer de linhas.
void testShowLines (void)
{	
	//desabilita o cursor
	//system_call ( 245, (unsigned long) 0, (unsigned long) 0, (unsigned long) 0);
	
	int i=0;
	int j=0;
	
	for ( i=0; i<32; i++ )
	{
		for ( j=0; j<80; j++ )
		{
		    //LINES[i].CHARS[j] = (char) 'x';
		    //LINES[i].ATTRIBUTES[j] = (char) 7;	        
			printf ("%c", LINES[i].CHARS[j] );
		}
		printf ("\n");
	};

	//reabilita o cursor
	//system_call ( 244, (unsigned long) 0, (unsigned long) 0, (unsigned long) 0);	
}

//mostra a �rea vis�vel dentro do buffer de linhas.
void shellRefreshVisibleArea (void)
{
	//desabilita o cursor
	//system_call ( 245, (unsigned long) 0, (unsigned long) 0, (unsigned long) 0);
	
	//seta o cursor no in�cio da janela.
	
	unsigned long left, top, right, bottom;
 
    left = (terminal_rect.left/8);
    top = (terminal_rect.top/8);
	
    shellSetCursor ( left, top );

	// efetua o refresh do char atual, que agora � o primeiro 
	// depois os outros consecutivos.
	
	int i=0;
	int j=0;
	
	//textTopRow = 3;
	//textBottomRow = 3 + 25;
	
	if ( textTopRow > textBottomRow ){
		printf("shellRefreshVisibleArea: textTopRow fail");
	}
	
	//toda a �rea vis�vel.
	//for ( i=0; i<25; i++ )	
	for ( i=textTopRow; i<textBottomRow; i++ )
	{
		for ( j=0; j<80; j++ )
		{	
	        //refresh
            printf ("%c", LINES[i].CHARS[j] );						
		}
	};

	//reabilita o cursor
	//system_call ( 244, (unsigned long) 0, (unsigned long) 0, (unsigned long) 0);	
}

void testChangeVisibleArea(void)
{
	textTopRow += textWheelDelta;
	textBottomRow += textWheelDelta;	
}

void updateVisibleArea(int direction)
{
    switch (direction)
    {
	    case 0:
	        textTopRow += textWheelDelta;
	        textBottomRow += textWheelDelta;			
            break; 		
			
	    case 1:
	        textTopRow -= textWheelDelta;
	        textBottomRow -= textWheelDelta;					
            break; 		
	}	
}

//rotina de testes de socket
void shellSocketTest(void)
{
/*
	//#todo: isso precisa ser um porteiro de estrutura.
	void *socketHandle;
	
	unsigned long iplong;
	unsigned long port; //short
	
	unsigned char ip[4];
	
    printf("\n");
    printf("shellSocketTest: Testing socket stuff ...\n");
	
	//
	// Creating socket
	//
	
	printf("Creating socket ...\n");
	socketHandle = (void *) system_call ( 160, (unsigned long) 0xC0A80164, (unsigned long) 0, (unsigned long) 0x22C3 );
	
	printf("Updating socket ...\n");
	system_call ( 163, (unsigned long) socketHandle, (unsigned long) 0xC0A80165, (unsigned long) 0x22C2 );
	
	printf("Getting ip from socket ...\n");
	iplong = (unsigned long) system_call ( 161, (unsigned long) socketHandle, (unsigned long) socketHandle, (unsigned long) socketHandle);
	
	printf("Getting port from socket ...\n");
	port = (unsigned long) system_call ( 162, (unsigned long) socketHandle, (unsigned long) socketHandle, (unsigned long) socketHandle);
	
	//
	// output
	//
	unsigned long tmp;
	
	tmp = iplong;
	ip[3] = (char) ( tmp & 0x000000FF ); 
	
	tmp = iplong;
	tmp = (tmp >> 8);
	ip[2] = (char) ( tmp & 0x000000FF );
	
	tmp = iplong;
	tmp = (tmp >> 16);
	ip[1] = (char) ( tmp & 0x000000FF );

	tmp = iplong;
	tmp = (tmp >> 24);
	ip[0] = (char) ( tmp & 0x000000FF );

	printf("\n");
	//printf("iplong=%x\n",iplong);
	printf ("Socket: ( %d.%d.%d.%d:%d )\n", 
		ip[0], ip[1], ip[2], ip[3], port );

	printf ("done\n");
*/
}


/*
 * main: 
 * @todo:
 *    +Checar argumentos.
 *    +Criar a janela do TERMINAL. (Ok).
 *    +Enviar mensagem de texto para ele. (Ok).
 *    +Configurar procedimento de janela.
 *    +Definir a janela que � �rea do cliente.
 *    +Carregar um arquivo na �rea de cliente.
 *    +Testar outros recursos do Kernel.
 *    +Testar as chamadas para pegar inform��es sobre o processo.
 *    +...
 * Obs: Esses argumentos podem ser um padr�o.
 */
// #obs:
// O shell � um alicativo e deve rodar no terminal virtual.
// Receberemos os argumentos do crt0.
// #bugbug: No momento estamos usando uma vers�o persnalizada do crt0,
// onde os argumentos s�o criados � m�o. Mas precisamos usar o crt0 padr�o
// que pega os argumentos enviados pelo processo pai.

int main( int argc, char *argv[] )
{
    int i, arg_index = 1;
    //extern int yydebug;
    FILE *default_input = stdin;
    char *local_pending_command = (char *) NULL;
    //extern int last_command_exit_value;
    int locally_skip_execution = 0, top_level_arg_index;
    //extern char *base_pathname ();

    //#ifdef JOB_CONTROL
    //extern int job_control;
    //#endif

    printf("SHELL00.BIN: Initializing ...\n");

    /* Wait forever if we are debugging a login shell. */
    //  while (debugging_login_shell);	

 /* If this shell has already been run, then reinitialize it to a vanilla state. */

    if (shell_initialized){
        //shell_reinitialize ();
        //if ( setjmp (top_level) )
            //exit (2);
    }

    /* Here's a hack.  If the name of this shell is "sh", then don't do
    any startup files; just try to be more like /bin/sh. */
   //{
       //char *tshell_name = base_pathname (argv[0]);

       //if (*tshell_name == '-')
           //tshell_name++;

       //if (strcmp (tshell_name, "sh") == 0)
           //act_like_sh++;
   //}


    //yydebug = 0;

    //shell_environment = env;
    //shell_name = argv[0];

    //if (*shell_name == '-')
    //{
    //  shell_name++;
    //  login_shell++;
    //}

#ifdef JOB_CONTROL
    if (act_like_sh)
        job_control = 0;
#endif

    //dollar_vars[0] = savestring (argv[0]);

    /* Parse argument flags from the input line. */

    // Find full word arguments first. 

    while ( (arg_index != argc) && *(argv[arg_index]) == '-' )
    {
        for ( i=0; long_args[i].name; i++ )
	    {
	        if ( strcmp( &(argv[arg_index][1]), long_args[i].name ) == 0 )
	        {
	            if ( long_args[i].type == Int )
		            *(long_args[i].value) = 1;
	                else
		            {
		                if ( !argv[++arg_index] )
		                {
		                    //report_error ("%s: Flag `%s' expected an argument",
				            //    shell_name, long_args[i].name);
							
		                    printf ("%s: Flag `%s' expected an argument",
				                shell_name, long_args[i].name );
		                    
							exit (1);
		                }
		                else
		                    *long_args[i].value = (int) argv[arg_index];
		           }
				
	               goto next_arg;
	        }
			//Nothing.
	    }

        break;  /* No such argument.  Maybe flag arg. */
        
		next_arg:
            arg_index++;
    };

	//??
    /* If user supplied the "-login" flag, then set and invert LOGIN_SHELL. */
    if (make_login_shell)
        login_shell = -++login_shell;	
	
    /* All done with full word args; do standard shell arg parsing.*/
    while ( arg_index != argc  && 
		    argv[arg_index]    && 
		    (*(argv[arg_index]) == '-' || (*argv[arg_index] == '+')) )
    {

      /* There are flag arguments, so parse them. */

	  int arg_character;
      int on_or_off = ( *argv[arg_index] );
      int  i = 1;

        while ( arg_character = (argv[arg_index])[i++] )
	    {
	        switch (arg_character)
	        {
	            case 'c':
	                /* The next arg is a command to execute, and the following args
		            are $1 .. $n respectively. */
	                local_pending_command = argv[++arg_index];
	                if (!local_pending_command)
		            {
		                //report_error ("`%cc' requires an argument", on_or_off);
		                printf ("`%cc' requires an argument", on_or_off);
		                exit (1);
		            }

	                arg_index++;
	                goto after_flags;
					break;

	            default:
					printf ("Nothing ...\n");
	                //if (change_flag_char (arg_character, on_or_off) == FLAG_ERROR)
		            //{
		                //report_error ("%c%c: bad option", on_or_off, arg_character);
		                //printf ("%c%c: bad option", on_or_off, arg_character);
		                //exit (1);
		            //}

	        }
	    }
		
        arg_index++;
    };	

after_flags:			
	printf ("Nothing ...\n");

    /* First, let the outside world know about our interactive status. */  
	/*
	if ( forced_interactive ||
         ( !local_pending_command && arg_index == argc && isatty (fileno (stdin)) && isatty (fileno (stdout)))  )
        interactive = 1;
        else
        {
            interactive = 0;
#ifdef JOB_CONTROL
            job_control = 0;
#endif
        };	
	*/
	
	  // #importante
	  // Essa � a inicializa��o do bash 1.05.
	  // No futuro usaremos essa, mas por enquanto temos nossa pr�pria.
	  /* From here on in, the shell must be a normal functioning shell.
         Variables from the environment are expected to be set, etc. */
     //shell_initialize ();
	
	
	//
	// emac stuff - line editing.
	//
	
	/*
    if (interactive)
    {
        char *emacs = (char *) getenv ("EMACS");
        
		if ( emacs && ( strcmp( emacs, "t") == 0) )
	        no_line_editing = 1;
    }	
	*/
	
    top_level_arg_index = arg_index;
	
    if ( !quiet && do_version )
        show_shell_version();	
	
	//
	// setjmp - (abort support)
	//
	
    /* Give this shell a place to longjmp to before executing the
       startup files.  This allows users to press C-c to abort the
       lengthy startup. */
	
	/*
    if ( setjmp (top_level) )
    {
        if (!interactive)
	        exit (2);
        else
	        locally_skip_execution++;
    }
    */
	
    arg_index = top_level_arg_index;	

	
    /* Execute the start-up scripts. */

	/*
    if (!interactive)
    {
        makunbound ("PS1");
        makunbound ("PS2");
    }
	*/

	/*
    if (!locally_skip_execution)
    {
        if (login_shell)
	        maybe_execute_file ("/etc/profile");

        if (login_shell && !no_profile)
	    {
	        // If we are doing the .bash_profile, then don't do the .bashrc. 
	        no_rc++;

	        if (act_like_sh)
	            maybe_execute_file ("~/.profile");
	        else
	        {
	            if (maybe_execute_file ("~/.bash_profile") == 0)
		            if (maybe_execute_file ("~/.bash_login") == 0)
		                maybe_execute_file ("~/.profile");
	        }

	        // I turn on the restrictions afterwards because it is explictly
	        // stated in the POSIX spec that PATH cannot be set in a restricted
	        // shell, except in .profile. 
			
	        if (*++(argv[0]) == 'r')
	        {
	            set_var_read_only ("PATH");
	            restricted++;
	        }
	    }	
	
        // Execute ~/.bashrc for all shells except direct script shells,
	    //and shells that are doing -c "command". 

        if ( arg_index == argc && !no_rc && !act_like_sh &&
	         (!local_pending_command || shell_level < 2) )
	      maybe_execute_file (bashrc_file);

        ///Try a TMB suggestion.  If running a script, then execute the
	    //file mentioned in the ENV variable. 
		
        if (!interactive)
	    {
	      char *env_file = (char *) getenv ("ENV");
	      
		  if (env_file && *env_file)
	          maybe_execute_file (env_file);
	    }

        if (local_pending_command)
	    {
	      with_input_from_string (local_pending_command, "-c");
	      goto read_and_execute;
	    }
    }	
	*/
	
    /* Do the things that should be done only for interactive shells. */
    /*
	if (interactive)
    {
        // Set up for checking for presence of mail. 
#ifdef SYSV
        // Under SYSV, we can only tell if you have mail if the
	    //modification date has changed.  So remember the current
	    //modification dates. 
        remember_mail_dates ();
#else
        // Under 4.x, you have mail if there is something in your inbox.
	    //I set the remembered mail dates to 1900.  
        reset_mail_files ();
#endif // SYSV 

        // If this was a login shell, then assume that /bin/login has already
	    //taken care of informing the user that they have new mail.  Otherwise,
	    //we want to check right away. 
	  
        if (login_shell == 1)
	    {
#ifndef SYSV
	        remember_mail_dates ();
#endif  // SYSV
	    }

        reset_mail_timer ();

        // Initialize the interactive history stuff.
        if (!shell_initialized)
	    {
	        char *hf = get_string_value ("HISTFILE");
	        
			if (hf)
	            read_history (hf);
	    }
    };
	*/
	
  /* Get possible input filename. */
	/*
get_input_filename:
    
	if (arg_index != argc)
    {
        int fd;

        free (dollar_vars[0]);
        
		dollar_vars[0] = savestring (argv[arg_index]);

        fd = open (argv[arg_index++], O_RDONLY);
        
		if (fd < 0)
	    {
	        file_error (dollar_vars[0]);
	        exit (1);
	    }else{
			
	       default_input = fdopen (fd, "r");
	       
			if (!default_input)
	        {
	            file_error (dollar_vars[0]);
	            exit (1);
	        }
	   }

       if (!interactive || (!isatty (fd)))
	   {
	      extern int history_expansion;
	      
		  history_expansion = interactive = 0;
#ifdef JOB_CONTROL
	      set_job_control (0);
#endif
	   }else{
	       dup2 (fd, 0);
	       close (fd);
	       fclose (default_input);
	   }
    }
	
    //Bind remaining args to $1 ... $n 
    {
        WORD_LIST *args = (WORD_LIST *) NULL;
        while (arg_index != argc)
            args = make_word_list (make_word (argv[arg_index++]), args);
            args = (WORD_LIST *)reverse_list (args);
            remember_args (args, 1);
            dispose_words (args);
    }

    if (interactive && !no_line_editing)
        with_input_from_stdin ();
        else
            with_input_from_stream (default_input, dollar_vars[0]);
	*/
	
	
	//char **internal;
	char *filename;

	// Obs: Esse n�o � um programa que roda em modo terminal,
	// ele na verdade cria um terminal dentro de uma janela filha.
	// isso pode servir para esse programa interpretar linguagem 
	// basic por exemplo.
	// os programas em modo terminal n�o criar�o janelas e rodar�o nas 
	// janelas de terminal cridas para eles pelo kernel.

	//#debug
	//deixe o kernel usar essa janela para teste.
	//Obs: N�o criaremos a janela principal desse programa 
	//para evitarmos erros com printf.
 
	/*
	 * Uma boa ordem para os passos dessa rotina nos aplicativos  
	 * �: Step1=WindowClass, Step2=Window e Step3=Message.
	 */
	
	//@todo: Criar esse tipo de classe.
	//       Provavelmente deve coincidir com a estrutura presente
    //nas rotinas de gerenciamento de janela que est�o em kernel mode.	
	//struct window_class_d *wc; 
	
	
	// A janela principal do aplicativo.
	struct window_d *hWindow;    

	//JANELA CRIADA NA �REA DE CLIENTE DA JANELA PRINCIPAL.
    //struct window_d *hWindow2;       
	
	//struct message_d *m;

	
	int Status = 0;
	//char *s;    //String	
	

    /* Wait forever if we are debugging a login shell. */
    //while(debugging_login_shell);	

	
    //if(shell_initialized)
    //{
        //reinitialize();
    //};
	
    //shell_environment = env;
    //shell_name = argv[0];
	
	//
	// ## ARGS ##
	//
	
	//
	// ## Command Switch ##
	//
	
	//Ok isso funcionou.
	//Argumentos passados com sucesso do crt0 para o main.
	
	//printf("argc={%d}\n",argc);
	
	//if ( argc >=2 )
	//{
	//    printf("arg[0]={%s}\n",argv[0]);
	//    printf("arg[1]={%s}\n",argv[1]);	
	//}

	
	//
	// Filtra a quantidade de argumentos.
	//
	
	//goto noArgs; 
	
	//N�o usar verbose nessa fase de tratar os argumentos
	//pois a janela ainda n�o foi inicializada.
	
	// Se n�o h� argumentos.
	if (argc < 1){
		//printf("No args !\n");
		//#Test.
        //fprintf( stderr,"Starting Shell with no arguments...\n");	 	
		die("No args");
		goto noArgs; 
	}else{
		
        if (argc < 2){
		    printf ("main: argc=%d We need 2 args or more\n", argc );
		 return 1;
		}

		//printf("Testing args ...\n");
		
		//#todo: (possibilidades)
		//As flags poderia come�ar com f. Ex: fInteractive, fLoginShell,
		
	    if ( strncmp ( (char *) argv[0], "-interactive", 12 ) == 0 )
		{	
			interactive = 1;    
            //printf("Initializing an interactive shell ...\n");
            //printf("arg[0]={%s}\n",argv[0]);			
        }

        //Se o shell foi iniciado com um arquivo de script para ser 
        //executado.
		//a Flag -f indica que o que segue � um arquivo de script.
        //if( strncmp( (char *) argv[0], "-f", 2 ) == 0 )
        //{
		//	goto dosh2;
		//}			
		
	    if ( strncmp ( (char *) argv[1], "-login", 6 ) == 0 )
		{	
			login_shell = 1;	
			//printf("Initializing login ...\n");
            //printf("arg[1]={%s}\n",argv[1]);    
        }

		//Nome passado viar argumento.
		//shell_name = (char*) argv[2];

        //...		
	};
	
	// Nothing.
noArgs:		
	
	//...
	
	//@todo:
	//Podemos come�ar pegando o id do processo atual e 
	//melhorando a prioridade.
	
	// get current dir
	//pegamos o diret�rio atual.
	
	//get user dir
	//pegamos o diret�rio do usu�rio /root/user/(name)
	
	//set current dir
	//setamos para que o diret�rio do usu�rio seja o diret�rio atual.
	
	// Isso configura alguns padr�es do aplicativo.
	// Os argumentos tratados abaixo podem modificar esses padr�es
	// Ex: Um argumento de entrada pode solicitar a troca de cor de fonte.
	
	// #todo
	// #importante
	// Temos que usar as configura��es do terminal que o gdeshell est� rodando.
	// Ent�o n�o podemos simplesmente criar uma janela do tamanho que quisermos.
	
	shellShell(); 	
	
	// Apenas inicialize. 
	// Continuaremos com o procedimento do shell e n�o o da barra,	

	// #Aten��o
	// Criaremos a janela tranquilamente usando os valores obtidos
	// na inicializa��o.
	
	/*
	// #deprecated: Delete this.
	enterCriticalSection ();    
    hWindow = shellCreateMainWindow(1);
	if ( (void *) hWindow == NULL ){
		printf ("shellCreateMainWindow FAIL!");
		while (1){}
	}	
	exitCriticalSection ();	
	*/
	
    // Registrar.
	// Configurar como ativa.
    // Setar foco.
	// *IMPORTANTE: 
	// � FUNDAMENTAL SETAR O FOCO, POIS O KERNEL DEPENDE DELE
	// PARA RETORNAR A MENSAGEM DA JANELA COM O FOCO DE ENTRADA.
	// Nesse momento o kernel configura as margens para o cursor 
	// dentro da janela.
	// @todo: O kernel deve reiniciar as vari�veis de cursor 
	// dentro da janela tamb�m, pois cada janela tem uma configura��o 
	// diferente de cursor.
	//
	// ?? Show Window !!
	// Precisamos mostrar a janela e n�o repintar 
	// a tela toda.
	
    //APIRegisterWindow (hWindow);
    //APISetActiveWindow (hWindow);	
    //APISetFocus (hWindow);
	
	//#test
	//vamos mostrar a janela do shell antes de criarmos a janela 
	//da �rea de cliente
	//apiShowWindow (hWindow);
	
	//#test 
	//Criando um timer.
	
	//printf("shmain: Creating timer\n");
					
		//janela, 100 ms, tipo 2= intermitente.
	//system_call ( 222, (unsigned long) hWindow, 100, 2);		

	
	//printf("SHELL\n");	
    //printf("#debug breakpoint");
    //while(1){} 	
	
	//#importante
	//VAMOS EFETUAR ESSE REFRESH DEPOIS DE CRIARMOS OUTRA JANELA.
	//refresh_screen ();
	
	
	//
	// #importante:
	// +pegamos o ret�ngulo referente � area de cliente da janela registrada. 
	// +atualizamos as vari�veis que precisam dessa informa��o. 
	// reposicionamos o cursor.
	// reabilitamos a piscagem de cursor.
	
	//3bugbug
	//vamos suspender isso porque estamos usando janela WT_SIMPLE,
	//e sanela simples n�o tem �rea de cliente 
	
	// +pegamos o ret�ngulo referente � area de cliente da janela registrada. 
	//unsigned long xbuffer[8];	
	//system_call ( 134, (unsigned long) hWindow, 
	//    (unsigned long) &xbuffer[0], (unsigned long) &xbuffer[0] );	
	//terminal_rect.left = xbuffer[0];
	//terminal_rect.top = xbuffer[1];
	//terminal_rect.width = xbuffer[2];
	//terminal_rect.height = xbuffer[3];	
	
	//...
	
	terminal_rect.left = wpWindowLeft;
	terminal_rect.top = wpWindowTop;
	terminal_rect.width = wsWindowWidth;
	terminal_rect.height = wsWindowHeight;

	//
	// ## Se der problema no tamanho da �rea de cliente ##
	//
	
	if ( terminal_rect.left < wpWindowLeft ||
         terminal_rect.top < wpWindowTop ||	
	     terminal_rect.width > wsWindowWidth ||
		 terminal_rect.height > wsWindowHeight )
	{
        //#debug
		printf("## fail ## \n");
	    printf("terminal_rect: 2\n");	
        printf("l={%d} t={%d} w={%d} h={%d}\n", 
	        terminal_rect.left, 
			terminal_rect.top,
		    terminal_rect.width, 
			terminal_rect.height );
		
        while (1){ asm ("pause"); }			
	}
	
	
        //#debug
		/*
	    printf("## debug ## \n");
	    printf("terminal_rect: 2\n");	
        printf("l={%d} t={%d} w={%d} h={%d}\n", 
	        terminal_rect.left, 
			terminal_rect.top,
		    terminal_rect.width, 
			terminal_rect.height );	
	   */
	
	//printf("SHELL\n");	
    //printf("#debug breakpoint");
    //while(1){} 	
	
	//===============================
	
    //
	// ## Janela para texto ##
	//
	
    /*
    apiBeginPaint();
	
	//mudando as dimens�es a janela dentro da �rea de cliente.
	
	terminal_rect.left = terminal_rect.left +2;
	terminal_rect.top = terminal_rect.top +2;
	
	terminal_rect.width = terminal_rect.width -4 -40; //8 * 80;
	terminal_rect.height = terminal_rect.height -4; //8 * 32;
	
	//terminal_rect.width = 8 * 80;
	//terminal_rect.height = 8 * 32;
	
	
	hWindow2 = (void *) APICreateWindow ( WT_SIMPLE, 1, 1, "SHELL-CLIENT",
	                        terminal_rect.left, terminal_rect.top, 
					        terminal_rect.width, terminal_rect.height,    
                            0, 0, SHELL_TERMINAL_COLOR2, SHELL_TERMINAL_COLOR2 );	   

						
	if ( (void *) hWindow2 == NULL )
	{	
		die ("shell.bin: hWindow2 fail");
	}	
	
	apiEndPaint();
	
    APIRegisterWindow (hWindow2);
    //APISetActiveWindow (hWindow2);	
    APISetFocus (hWindow2);	 
	
	//#importante
	//refresh_screen ();	
	
	//#test 
	//substituindo refresh screen por show window.
	//vamos mostrar a janela da �rea do cliente, depois de 
	//termos mostrado a janela m�e.
	apiShowWindow (hWindow2);
	*/
	
//===========================================================

	
	//
	// Habilitando o cursor piscante de textos.
	//
	
	shellSetCursor ( (terminal_rect.left / 8) , ( terminal_rect.top/8) );	
	
	//system_call ( 244, (unsigned long) 0, (unsigned long) 0, (unsigned long) 0 );
	
    //Mensagem ...
	//printf ("\n");
	//printf ("Starting GDESHELL.BIN ... \n\n");	
	
    //printf("#debug breakpoint");
    //while(1){} 	
	
	//#bugbug
	//janela usada para input de textos ...
	//o input de texto pode vir de v�rias fontes.
	//api_set_window_with_text_input(hWindow);
	
	//
	// ++ terminal ++
	//
	
	// #importante
	// Definindo a janela como sendo uma janela de terminal.
	// Isso faz com que as digita��es tenham acesso ao procedimento de janela de terminal 
	// para essa janela e n�o apenas ao procedimento de janela do sistema.
	// # provavelmente isso marca os limites para a impress�o de caractere em modo terminal 
	
	//#importante
	//nesse momento estamos configurando os limites do terminal gerenciado pelo kernel.
    // >> logo abaixo vamos chamar uma inst�ncia do aplicativo terminal e configurar
	//o aplicativo terminal com os mesmos limites.
	
	//system_call ( SYSTEMCALL_SETTERMINALWINDOW, (unsigned long) hWindow, 
		//(unsigned long) hWindow, (unsigned long) hWindow );
			 
	//salva ponteiro da janela principal e da janela do terminal. 
	//shell_info.main_window = ( struct window_d * ) hWindow;			 
	//shell_info.terminal_window = ( struct window_d * ) hWindow;		

	/*
	// ===========================

	// #test
	// Vamos executar uma inst�ncoa do noraterm e tentar configur�-lo.
	// Esse teste funcionou.
	
	int __terminal___PID;
	
	__terminal___PID = (int) apiStartTerminal ();
	
	if ( __terminal___PID <= 0 )
	{
	    printf ("PID fail. We can't send the message\n");

    }else{			
			//#importante
			//Aqui podemos configurar o terminal.
			//pegar as caracter�sticas do terminal para configurar o app cliente.
	    printf ("The terminal PID is %d \n", __terminal___PID );		
		__SendMessageToProcess ( __terminal___PID, NULL, MSG_TERMINALCOMMAND, 2001, 2001 );		
	};		
	
	printf ("gdeshell: *breakpoint");
	while (1){}

	// ===========================
	*/

	//===========================

	// Init Shell:
	//     Inicializa vari�veis, buffers e estruturas. Atualiza a tela.
	// #BUGBUG
	// Estamos passando um ponteiro que � uma vari�vel local.

    //enterCriticalSection();
    Status = (int) shellInit(); 
    if (Status != 0){
        die ("shell00: main: shellInit fail");
    }
    //exitCriticalSection(); 

	// #importante:
	// Agora � a hora de pegar mensagens de input de teclado.
	// Mas se o shell n�o for interativo, ent�o n�o pegaremos 
	// mensagens de input de teclado.

    if ( interactive != 1 )
	{
		//#debug
        printf("shell is not interactive\n");
        goto skip_input;
    }

	//@todo: Isso � um teste.
	//system("reboot");
	
	//
	// Podemos tentar criar um processo.
	//

    //
	// Get message.
	//

	/*
	 * @todo: 
	 *     Entrar num loop, solicitando ao kernel possiveis mensagens
	 * deixadas no buffer de mensagens do PCB do processo shell.
	 * Obs: O kernel coloca na fila de mensagens do processo, mensagens
	 * provenientes n�o apenas do teclado, mas tamb�m de outras fontes.
	 * Ent�o dessa forma, pode-se enviar mensagens de um processo para outro.
	 * *IMPORTANTE:Cabe a cada processo pegar as mensagens deixadas no buffer de mensagens 
	 * em seu PCB, atravez de uma chamada ao kernel.
	 */
    
   
	//
	// Por fim: Testar cursor e terminar.
	//

	//@todo: 0,0 n�o est� na �rea de cliente.
		
	//
	// **** Mensagens  ****
	//
	
	//printf("Tentando pegar mensagem enviada para o procedimento de janela.");
	//refresh_screen();
	
	//isso � um teste pegar um valor por vez n�o � a melhor op��o.
	
	//struct window_d *msg_Window;
	int msg_Message;
	void *msg_Long1;
	void *msg_Long2;
	
	//struct shell_message_d *msg;
	

	// Get Message: 
	// Systemcall get message
	// Enviamos um ponteiro de estrutura de janela para que o Kernel possa 
	// pegar a mensagem que esta dentro da estrutura. Essa estrtura fica 
	// protegida no Kernel.
		
	// #bugbug: ??
	// Na verdade essa rotina est� pegando a mensagem na janela 
	// com o foco de entrada. Esse argumento foi passado mas n�o foi usado.
		
	unsigned long message_buffer[5];	
		
read_and_execute:
    // Nothing.
	shell_initialized = 1;
	
	// #importante 
	// #todo
	/* Read commands until exit condition. */
    //reader_loop ();
	
	// #importante
	// A fun��o acima � um loop
	// Esse aqui � outro loop.
	// #todo: No futuro s� teremos o loop no estilo bash, que � a fun��o acima.
	
	/* Nesse teste vamos enviar um ponteiro de array, pegarmos os quatro 
	   elementos da mensagem e depois zerar o buffer */
	
Mainloop:
	
	while (_running)
	{
		// #obs: 
		// O retorno ser� 1 se tiver mensagem e 0 se n�o tiver.
		
		//enterCriticalSection(); 
		system_call ( 
            111,
		    (unsigned long) &message_buffer[0],
			(unsigned long) &message_buffer[0],
			(unsigned long) &message_buffer[0] );
		//exitCriticalSection(); 
			
		//if ( message_buffer[1] != 0 ){
            //printf(".");
		//}	

		if ( message_buffer[1] != 0 )
		{
	        shellProcedure ( 
				(struct window_d *) message_buffer[0], 
		        (int) message_buffer[1], 
		        (unsigned long) message_buffer[2], 
		        (unsigned long) message_buffer[3] );
			
			message_buffer[0] = 0;
            message_buffer[1] = 0;
            message_buffer[3] = 0;
            message_buffer[4] = 0;
        }
	};
	
// Entramos aqui se running for igual a 0.
	switch (ShellFlag)
	{
	    // Sai do shell.
		case SHELLFLAG_EXIT:
		    goto end;
			break;
		// Sai do shell.
        default:
            goto end;
			break;
	};

// Pulamos a parte que pega mensgens de input de teclado 
// porque esse shell n�o est� configurado como interativo.

// # RunScript #	
skip_input:	
    shellExecuteThisScript( argv[3] );
// Exit 
end:

// #?
// Desabilitando o cursor de texto.
// Quando outro aplicativo precisar ele clica em uma janela, 
// ela ganha o foco e habilita o cursor piscante.	
    //system_call( 
        //245, (unsigned long) 0, (unsigned long) 0, (unsigned long) 0);

    /* Do trap[0] if defined. */
    //run_exit_trap ();	
	
	//
	//    ## History ##
	//
	
    /* Save the history of executed commands. */
    /*
	if (interactive){
        char *hf = get_string_value ("HISTFILE");
		if (hf)
	        write_history (hf);
    }	
	*/

    //#ifdef SHELL_VERBOSE		
    //printf ("gdeshell: Exiting code '0' ...\n");
    //#endif 

    return EXIT_SUCCESS;
}

//
// End
//

