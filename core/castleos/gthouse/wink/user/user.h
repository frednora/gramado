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
    struct kobject_d kobj;
    int used;
    int magic;

// SID - Session ID.
    int id;

// The user in this session.
    uid_t uid;

// Initial process for this session.
    pid_t initial_pid;
    //tid_t initial_tid;

// The session leader.
// The leader process for this process.
// The leader has some privileges in its session.
    pid_t leader_pid;
    //tid_t leader_tid;

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


//
// Account flags
//

#define UAF_ENABLED            0x00001000
#define UAF_DISABLED           0x00002000

#define UAF_INTERACTIVE        0x00004000
#define UAF_NONINTERACTIVE     0x00008000

#define UAF_SYSTEM             0x00010000
#define UAF_ADMINISTRATOR      0x00020000

#define UAF_GUEST              0x00040000
#define UAF_LOCKED             0x00080000

#define UAF_CAN_LOGIN          0x00100000
#define UAF_PASSWORD_REQUIRED  0x00200000

#define UAF_BUILTIN_ACCOUNT    0x00400000
#define UAF_SERVICE_ACCOUNT    0x00800000


//
// Account permissions
//

#define UPERM_CREATE_PROCESS   0x00001000
#define UPERM_KILL_PROCESS     0x00002000

#define UPERM_CREATE_THREAD    0x00004000
#define UPERM_DEBUG_PROCESS    0x00008000

#define UPERM_LOAD_MODULE      0x00010000
#define UPERM_UNLOAD_MODULE    0x00020000

#define UPERM_MOUNT_FS         0x00040000
#define UPERM_UNMOUNT_FS       0x00080000

#define UPERM_CREATE_SESSION   0x00100000
#define UPERM_MANAGE_USERS     0x00200000

#define UPERM_NETWORK_ADMIN    0x00400000
#define UPERM_REBOOT_SYSTEM    0x00800000

#define UPERM_SHUTDOWN_SYSTEM  0x01000000
#define UPERM_ALL_ACCESS       0xFFFFFFFFFFFFFFFF



// user_info_d:
// Estrutura para perfil de usuário do computador. 
// #test
// This is used in the terminal structure to handle user sessions.
// logon stuff.

struct user_info_d
{
    struct kobject_d kobj;
    int used;
    int magic;
    int initialized;  // This structure was initialized
    
    uid_t userId;
    user_type_t userType;  // user type

    uint64_t account_flags;
    uint64_t account_permissions;

//
// Security
//

    // user session
    struct usession_d *usession;
    int usessionId;

    // cgroup
    struct cgroup_d *cg;
    int cg_id;


//
// path for user directory
//

    char *path;             // '/root/user/(name)'

//
// Name
//

    char __username[64];    // HOSTNAME_BUFFER_SIZE
    size_t userName_len;    // len 

// Indica quais os tipos de objetos permitidos para esse usuário.
// see kobject.h
    // #deleted i dont like it.
    //int permissions[128];

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
// == prototypes ===========================
//

// Initialize the first cgroup.
int init_first_cgroup(void);

void set_current_cgroup(struct cgroup_d *cg);
struct cgroup_d *get_current_cgroup(void);

int get_current_cg_id(void);

int RegisterCG(struct cgroup_d *cg);
struct cgroup_d *CreateCG(void);


int GetCurrentGroupId (void);

uid_t GetCurrentUserId(void);
void SetCurrentUserId(uid_t user_id);

int is_superuser(void);

//
// syscall services.
//

int setreuid(uid_t ruid, uid_t euid);
int setregid(gid_t rgid, gid_t egid);

int sys_setuid(uid_t user_id);
int sys_getusername(char *ubuff);
int sys_setusername(const char *new_username);

struct user_info_d *CreateUser(char *name, int type);

int userCreateRootUser(void);

int user_initialize(void);

#endif    

