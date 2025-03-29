// user.h
// userinfo >> user session
// Created by Fred Nora.  

#ifndef ____USER_H
#define ____USER_H    1

/*
 * usession_d:
 *     The user section structure. 
 *     (u.session)
 */ 
struct usession_d
{
    object_type_t objectType;
    object_class_t objectClass;
    struct object_d  *object;
// SID - Session ID.
    int id;
    int used;
    int magic;
// The user in this session.
    uid_t uid;
// Initial process for this session.
    pid_t initial_pid;
// The session leader.
// The leader process for this process.
// The leader has some privileges in its session.
    pid_t leader_pid;
// #todo
// All the process in this group will be killed when the
// session ends.
    // gid_t gid;
    int initialized;
// ??
// What is the virtual terminal used by this session?
// What is the virtual console used by this session?
// This session has a terminal or console?
    //int tty;

// #test #todo
// This usersession belongs to an cgroup.
    //struct cgroup_d *cgroup;

// =========================================================
    unsigned long BeginTime;
    unsigned long EndTime;
// Navigation
    struct usession_d *next;
};
// #todo
// Use external reference here.
extern struct usession_d *usession0;
extern struct usession_d *CurrentUserSession;
extern int current_usersession;
// List
// Counters.
#define USER_SESSION_COUNT_MAX  16
extern unsigned long usessionList[USER_SESSION_COUNT_MAX];

//
// == prototypes ===========================
//

// Initialize the first cgroup.
int init_first_cgroup(void);

void set_current_cgroup(struct cgroup_d *cg);
struct cgroup_d *get_current_cgroup(void);

int get_current_cg_id(void);

int RegisterCG(struct cgroup_d *cg);
struct cgroup_d *CreateCG(void);

// --------------------------------------------

// Ring0 components for the display server.
int gramkInitialize (void);

// --------------------------------------------


/*
 Um usuário só pode rodar o servidor de recursos gráficos se ele estiver no modo terminal.
 No Gramado o modo terminal utiliza o servidor de recursos gráficos kgws, que é um servidor
 dentro do base kernel.
 No Gramado, o modo terminal terá o kgws como seu servidor de recursos gráficos para a exibição do 
 terminal em full screen.
 O segundo servidor de recursos gráficos é o gws que está dentro do programa shell.bin.
 Isso significa que para um usuário inicializar os recursos gráficos do servidor gws ele
 precisa estar em modo terminal usando o kgws.
 */


#define USER_COUNT_MAX     16    //User count.
#define GROUP_COUNT_MAX    4     //Group count.


//
// Standard user names.
//

// Interactive
// Used by special real users.
#define USER_NAME_ROOT    "root" 

// Non interactive
// Used by some system services.
#define USER_NAME_SYSTEM  "system" 



//User types.  
typedef enum {
    USER_TYPE_NULL,            // 0
    USER_TYPE_INTERACTIVE,     // 1 ex: 'root'
    USER_TYPE_NONINTERACTIVE,  // 2 ex: 'system'
    // ...
}user_type_t;

// user_info_d:
// Estrutura para perfil de usuário do computador. 
// #test
// This is used in the terminal structure to handle user sessions.
// logon stuff.

struct user_info_d
{
    object_type_t  objectType;
    object_class_t objectClass;
    
    uid_t userId;
    
    int used;
    int magic;

// This structure was initialized.
    int initialized;

// Security

    // user session
    struct usession_d *usession;
    int usessionId;

    // cgroup
    struct cgroup_d *cg;
    int cg_id;

    //??
    char *path;             // '/root/user/(name)'

    char __username[64];    // HOSTNAME_BUFFER_SIZE
    size_t userName_len;    // len 

// Indica quais os tipos de objetos permitidos para esse usuário.
// see kobject.h
    int permissions[128];

// Bancos de Dados 
	//struct bank_d *kdb;         //O banco de dados do processo kernel.	
	//struct bank_d *gdbListHead; //Lista de bancos de contas conjuntas.
	//struct bank_d *ldbListHead; //Lista de bancos de contas particulares.

	//Tempo da sessão de uso.
	//start time.
	//unsigned long StartTime;
	//end time.
    //unsigned long EndTime;

// Em qual console virtual iniciamos a sessão.
    int virtualconsoleId;

// Em qual console virtual iniciamos a sessão?
// F1 ~ F7 ...
    //struct virtual_console_d *virtual_console;
//=========================

// user type.
    user_type_t userType;

//#importante:
//Olhe as explicações logo acima.
    //user_display_mode_t display_mode;

//
// TOKENs de acesso à recursos do kernel. @todo: Rever.
//

	//kernel_token_t       k_token;    //Kernel.
	//executive_token_t    e_token;    //Executive.
	//microkernel_token_t  m_token;    //Microkenrel.
	//hal_token_t          h_token;    //Hal.

//
// todo: tipo, tamanho ... 
//

// Continua...

// navigation.
    struct user_info_d *next;
}; 

// see: userenv.c
extern struct user_info_d *RootUser;       // Super user
extern struct user_info_d *CurrentUser;    // Current user
// see: userenv.c
// user list:
// root user is the user '0'.
extern unsigned long userList[USER_COUNT_MAX];

//
// == prototypes ====================
//

int GetCurrentGroupId (void);

uid_t GetCurrentUserId(void);
void SetCurrentUserId(uid_t user_id);

int is_superuser(void);

int sys_getusername(char *ubuff);
int sys_setusername(const char *new_username);

struct user_info_d *CreateUser( char *name, int type );

int userCreateRootUser(void);

int User_initialize(void);

#endif    




