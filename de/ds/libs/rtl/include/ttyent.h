
// ttyent.h


/*
The fields are as follows:

     ty_name	 The name of the character-special file.

     ty_getty	 The name of the command invoked by init(8) to initialize tty
		 line characteristics.

     ty_type	 The name of the default terminal type connected to this tty
		 line.

     ty_status	 A mask of bit fields which indicate various actions allowed
		 on this tty line.  The possible flags are as follows:

		 TTY_ON       Enables logins (i.e., init(8) will start the
			      command referenced by ty_getty on this entry).

		 TTY_SECURE   Allow users with a uid of 0 to login on this
			      terminal.

		 TTY_DIALUP   Identifies a tty as a dialin line.  If this flag
			      is set, then isdialuptty() will return a non-
			      zero value.

		 TTY_NETWORK  Identifies a tty used for network connections.
			      If this flag is set, then isnettty() will return
			      a non-zero value.

     ty_window	 The command to execute for a window system associated with
		 the line.

     ty_group	 A group name to which the tty belongs.  If no group is speci-
		 fied in the ttys description file, then the tty is placed in
		 an anonymous group called "none".

     ty_comment  Any trailing comment field, with any leading hash marks
		 (``#'') or whitespace removed.
 */

/*
     #define TTY_ON	         0x01    // enable logins (start ty_getty program) 
     #define TTY_SECURE      0x02    // allow uid of 0 to login 
     #define TTY_DIALUP      0x04    // is a dialup tty 
     #define TTY_NETWORK     0x08    // is a network tty 
*/ 
     
/*     
struct ttyent {
	     char    *ty_name;	     // terminal device name 
	     char    *ty_getty;      // command to execute, usually getty 
	     char    *ty_type;	     // terminal type for termcap 

	     int     ty_status;      // status flags 
	     char    *ty_window;     // command to start up window manager 
	     char    *ty_comment;    // comment field 
	     char    *ty_group;      // tty group name 
     };
*/    
    
     
     
     
