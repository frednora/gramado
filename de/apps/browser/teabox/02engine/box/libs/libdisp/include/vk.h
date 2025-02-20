// vk.h
// Virtual Keys in Gramado OS.
// Created by Fred Nora.

#ifndef GWS_VK_H
#define GWS_VK_H    1

// Virtual keys.
#define VK_LBUTTON   0x01
#define VK_RBUTTON   0x02
#define VK_CANCEL    0x03
#define VK_POWER     0x03  //??
#define VK_MBUTTON   0x04
#define VK_XBUTTON1  0x05
#define VK_XBUTTON2  0x06
#define VK_CLEAR     0x0C  //12
#define VK_BACK      0x0E  //14  //8
#define VK_TAB       0x0F  //15  //9
#define VK_KANA      0x15
#define VK_HANGEUL   0x15
#define VK_HANGUL    0x15
#define VK_JUNJA     0x17
#define VK_FINAL     0x18
#define VK_HANJA     0x19
#define VK_KANJI     0x19
#define VK_ACENTO_AGUDO  0x1A  //26 //0xb4 abnt	
#define VK_ACENTO_GRAVE  0x1A  //26  //0x60	abnt
#define VK_ESCAPE      0x1B  //27 
#define VK_RETURN      0x1C  //28  //13    //Return.
#define VK_CONVERT     0x1C  //??
#define VK_CONTROL     0x1D  //29
#define VK_CTRL        0x1D  //29   
#define VK_LCONTROL    0x1D  //0xA2
#define VK_NONCONVERT  0x1D  //??
#define VK_ACCEPT      0x1E
#define VK_MODECHANGE  0x1F
#define VK_ACENTO_TIL  0x28  //40 //0x7e	abnt
#define VK_LSHIFT      0x2A  //0xA0 
#define VK_PRINT       0x2A  //42 ??
#define VK_EXECUTE     0x2B  //43
#define VK_SNAPSHOT    0x2C  //44
#define VK_VIRGULA     0x33       //51 abnt
#define VK_PONTO       0x34       //52 abnt
#define VK_PONTOVIRGULA  0x35  //53 abnt
#define VK_RSHIFT      0x36  //54  //0xA1
#define VK_ALT         0x38  //56   
#define VK_LMENU       0x38  //56   //Não mudar.
#define VK_MENU        0x38  //56 
#define VK_RMENU       0x38  //56   //@todo:Mudar esse
#define VK_ALTGR       0x38  //56 abnt //100 //56
#define VK_SPACE       0x39  //57  //Space.
#define VK_CAPITAL     0x3A  //58  //20  
#define VK_F1    0x3B  //59    // No DOS é 0x170.  
#define VK_F2    0x3C  //60 
#define VK_F3    0x3D  //61 
#define VK_F4    0x3E  //62 
#define VK_F5    0x3F  //63 
#define VK_F6    0x40  //64 
#define VK_F7    0x41  //65 
#define VK_F8    0x42  //66 
#define VK_F9    0x43  //67 
#define VK_F10   0x44  //68
#define VK_NUMLOCK  0x45  //69 //0x90
#define VK_SCROLL   0x46  //70 //0x91
#define VK_HOME     0x47  //71          
#define VK_END      0x4F  //79
#define VK_DELETE_TECLADONUMERICO 0x53  //83  
#define VK_F11    0x57  //87 
#define VK_F12    0x58  //88    
#define VK_LWIN   0x5B
#define VK_WINKEY 0x5B  //91    
#define VK_RWIN   0x5C
#define VK_APPS   0x5D
#define VK_CONTROL_MENU 0x5D  //93
#define VK_ACENTO_CIRCUNFLEXO 0x5E  //abnt	
#define VK_SLEEP     0x5F
#define VK_NUMPAD0   0x60
#define VK_NUMPAD1   0x61
#define VK_NUMPAD2   0x62
#define VK_NUMPAD3   0x63
#define VK_NUMPAD4   0x64
#define VK_BREAK     0x65  //101
#define VK_NUMPAD5   0x65
#define VK_FIND      0x66  //102
#define VK_NUMPAD6   0x66
#define VK_UP        0x67  //103 //72 
#define VK_NUMPAD7   0x67  //??
#define VK_PRIOR     0x68  //104 //33 (prior).
#define VK_NUMPAD8   0x68  //??
#define VK_LEFT      0x69  //105 //75 
#define VK_NUMPAD9   0x69  //??
#define VK_RIGHT     0x6A  //106 //77	
#define VK_MULTIPLY  0x6A  //??
#define VK_SELECT    0x6B  //107 //41
#define VK_ADD       0x6B  //??
#define VK_DOWN	     0x6C  //108 //80
#define VK_SEPARATOR 0x6C  //??
#define VK_NEXT      0x6D  //109 //34
#define VK_SUBTRACT  0x6D  //??
#define VK_INSERT    0x6E  //110 //45
#define VK_DECIMAL   0x6E  //??
#define VK_REMOVE    0x6F  //111
#define VK_DIVIDE    0x6F  //??
#define VK_HELP      0x73  //115 //47
#define VK_PAUSE     0x77  //119 //19 
#define VK_F13    0x7C
#define VK_F14    0x7D
#define VK_F15    0x7E
#define VK_F16    0x7F
#define VK_DELETE    0x7F  //127 (delete)
#define VK_F17    0x80
#define VK_F18    0x81
#define VK_F19    0x82
#define VK_F20    0x83
#define VK_F21    0x84
#define VK_F22    0x85
#define VK_F23    0x86
#define VK_F24    0x87
#define VK_RCONTROL         0xA3  //0x1D  //0xA3
#define VK_BROWSER_BACK     0xA6
#define VK_BROWSER_FORWARD  0xA7
#define VK_ACENTO_TREMA     0xA8  //abnt
#define VK_BROWSER_REFRESH  0xA8  //??
#define VK_BROWSER_STOP     0xA9
#define VK_BROWSER_SEARCH   0xAA
#define VK_BROWSER_FAVORITES  0xAB
#define VK_BROWSER_HOME  0xAC
#define VK_VOLUME_MUTE   0xAD
#define VK_VOLUME_DOWN   0xAE
#define VK_VOLUME_UP     0xAF
#define VK_MEDIA_NEXT_TRACK  0xB0
#define VK_MEDIA_PREV_TRACK  0xB1
#define VK_MEDIA_STOP        0xB2
#define VK_MEDIA_PLAY_PAUSE  0xB3
#define VK_LAUNCH_MAIL       0xB4
#define VK_LAUNCH_MEDIA_SELECT  0xB5
#define VK_LAUNCH_APP1    0xB6
#define VK_LAUNCH_APP2    0xB7
#define VK_OEM_1      0xBA
#define VK_OEM_PLUS   0xBB
#define VK_OEM_COMMA  0xBC
#define VK_OEM_MINUS  0xBD
#define VK_OEM_PERIOD 0xBE
#define VK_OEM_2      0xBF
#define VK_OEM_3      0xC0
#define VK_OEM_4      0xDB
#define VK_OEM_5      0xDC
#define VK_OEM_6      0xDD
#define VK_OEM_7      0xDE
#define VK_OEM_8      0xDF
#define VK_OEM_102    0xE2
#define VK_PROCESSKEY 0xE5
#define VK_PACKET     0xE7
#define VK_ATTN       0xF6
#define VK_CRSEL      0xF7
#define VK_EXSEL      0xF8
#define VK_EREOF      0xF9
#define VK_PLAY       0xFA
#define VK_ZOOM       0xFB
#define VK_NONAME     0xFC
#define VK_PA1        0xFD
#define VK_OEM_CLEAR  0xFE
//#define VK_
//#define VK_
//#define VK_
//#define VK_



#endif    



//
// End.
//


