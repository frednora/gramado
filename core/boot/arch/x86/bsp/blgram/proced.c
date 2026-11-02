// proced.c
// Created by Fred Nora.
 
#include "bl.h"  


unsigned long 
bl_procedure ( 
    unsigned long window, 
    int msg, 
    unsigned long long1, 
    unsigned long long2 ) 
{ 

    //printf("Procedure do bootloader!\n");  
  
    switch (msg)
    {             
        case MSG_KEYDOWN:
            switch (long1)
            {   
                /* 			
			     * Cancela a atividade atual que o Boot Loader 
				 * estiver realizando e reiniciar o computador.
                 */
				case VK_ESCAPE:
                    //@todo
				    break;
                              
                default:
				    //Nothing.
                    break; 
            };
        break;
          
        case MSG_SYSKEYDOWN:                 
            switch (long1)	       
            { 
  
                // 4 possibilidades apenas.
                case VK_F1:
                    printf ("F1");
                    //Reboot ?
                    break;

                case VK_F2:
                    printf ("F2");
                    break;

                //case VK_F3:
                    //printf ("F3");
                    //break;

                //case VK_F4:
                    //printf ("F4");
                    //break;

                default:
                    // Nothing.
                    break;
             };              
        break;

        default:
            // Nothing.
            break;
    };

    return (unsigned long) 0;
}

//
// End
//

