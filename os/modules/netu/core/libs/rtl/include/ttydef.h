
// ttydef.h  (ttydefaults.h)

/*-
 * Copyright (c) 1982, 1986 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from: @(#)ttydefaults.h	7.9 (Berkeley) 5/9/91
 *	ttydefaults.h,v 1.3 1993/05/20 16:23:32 cgd Exp
 */
 
 
// Environment: 
// termios.h is needed.
 

#ifndef _SYS_TTYDEFAULTS_H_
#define _SYS_TTYDEFAULTS_H_


// System wide defaults for terminal state.



// Defaults on "first" open.
// See: termios.h
#define  TTYDEF_IFLAG  (BRKINT | ISTRIP | ICRNL | IMAXBEL | IXON | IXANY)
#define  TTYDEF_OFLAG  (OPOST | ONLCR | OXTABS)
#define  TTYDEF_LFLAG  (ECHO | ICANON | ISIG | IEXTEN | ECHOE|ECHOKE|ECHOCTL)
#define  TTYDEF_CFLAG  (CREAD | CS7 | PARENB | HUPCL)
#define  TTYDEF_SPEED  (B9600)
//...



/*
 * Control Character Defaults
 */

// Credits:
// See:
// http://osr600doc.sco.com/en/man/html.M/termio.M.html
// ...


// Clear some bits.
// 32 control chars.

#define  CTRL(x) ((x) & 0x1f) 


// ^d
// (CEOF: <Ctrl>d or ASCII EOT)
// May be used to generate an end-of-file from a terminal. 
// When received, all the characters waiting to be read are immediately 
// passed to the program, without waiting for a newline, and the EOF 
// is discarded. Thus, if there are no characters waiting, which is 
// to say the EOF occurred at the beginning of a line, zero characters 
// will be passed back, which is the standard end-of-file indication.
#define  CEOF  CTRL('d')   


//#bugbug overflow??
// 0xff
// ?? Isso tá certo ??
//#define  CEOL  ( (unsigned) '\377' )
//#define  CEOL  0xff


// ^h
// 0x7f
// (CERASE: <Ctrl>h or ASCII BS)
// Erase the preceding character. 
// It will not erase beyond the start of a line, 
// as delimited by an NL, EOF, EOL, or EOL2
#define  CERASE  0x7f 
//#define  CERASE  0177 


// ^c
// (CINTR: rubout or ASCII DEL)
// Generate an interrupt signal which is sent to all processes 
// with the associated control terminal. Normally, each such process 
// is forced to terminate, but arrangements may be made either to 
// ignore the signal or to receive a trap to an agreed-upon location.
#define  CINTR  CTRL('c')


// 0xff
#define  CSTATUS  ((unsigned)'\377')


// ^u
// (CKILL: <Ctrl>u or ASCII NAK)
// Delete the entire line, as delimited by a NL, EOF, or EOL character. 
#define  CKILL  CTRL('u')   


#define  CMIN  1


// ^\
// (CQUIT: <Ctrl>\ or ASCII FS) 
// Generate a quit signal. Its treatment is identical to the interrupt 
// signal except that, unless a receiving process has made other 
// arrangements, it will not only be terminated, but a core image file 
// (core) will be created in the current working directory. 
#define  CQUIT  0x1C
//#define  CQUIT  034

 
// ^z 
// (CSUSP: <Ctrl>z or ASCII SUB) 
// If the ISIG flag is enabled, receipt of the SUSP character 
// causes a SIGTSTP signal to be sent to the current process group. 
// SUSP is discarded after being processed and is set to ASCII NUL 
// by default. If SUSP is set to <Ctrl>z in the Korn shell, 
// for example, it allows the currently executing process in the 
// foreground process group to be temporarily suspended. The SUSP 
// character can only be changed or escaped if the SVID3 line 
// discipline or the termios structure is used.
#define  CSUSP  CTRL('z')  



#define  CTIME  0


// ^y
// (CDSUSP: <Ctrl>y or ASCII EM) 
// Delayed suspend similar to SUSP. The SIGTSTP signal is sent when 
// the process in the foreground process group tries to read DSUSP 
// rather than when the character is typed. If the ISIG flag is enabled, 
// receipt of the SUSP character causes a SIGTSTP signal to be sent to 
// the current process group. (SVID3 line discipline only.) 
#define  CDSUSP  CTRL('y')   


// ^q
// (CSTART: <Ctrl>q or ASCII DC1) 
// Resume output which has been suspended by a STOP character. 
// While output is not suspended, START characters are ignored and 
// not read. The START character can only be changed or escaped if 
// the SVID3 line discipline or the termios structure is used.
#define  CSTART  CTRL('q')  


// ^s
// (CSTOP: <Ctrl>s or ASCII DC3)
// Temporarily suspend output. It is useful with CRT terminals 
// to prevent output from disappearing before it can be read. 
// While output is suspended, STOP characters are ignored and not read. 
// The STOP character can only be changed or escaped if the SVID3 
// line discipline or the termios structure is used.
#define  CSTOP   CTRL('s')    


// ^v
// (CLNEXT: <Ctrl>v or ASCII SYN)
// Ignore the special meaning of the following special character. 
// This stops characters such as QUIT being interpreted. 
// (SVID3 line discipline only.) 
#define  CLNEXT  CTRL('v')   


// ^o
// (CFLUSH: <Ctrl>o or ASCII SI) 
// Discard subsequent output until another DISCARD is entered, 
// more input is received, or the program clears the condition. 
// (SVID3 line discipline only.) 
#define  CDISCARD  CTRL('o')  


// ^w
// (CWERASE: <Ctrl>w or ASCII ETX)
// Erase the preceding word. It will not erase beyond the start of a 
// line, as delimited by an NL, EOF, EOL, or EOL2 character. 
// (SVID3 line discipline only.) 
#define  CWERASE  CTRL('w')   


// ^r
// (CRPRNT: <Ctrl>r or ASCII DC2)
// Reprint all characters that have not previously been read, 
// preceding them with a newline. (SVID3 line discipline only.) 
#define  CREPRINT  CTRL('r')   


#define  CEOT  CEOF


// Compat.
#define  CBRK  CEOL
#define  CRPRNT  CREPRINT
#define  CFLUSH  CDISCARD
// ...


#endif  /* !_SYS_TTYDEFAULTS_H_ */





