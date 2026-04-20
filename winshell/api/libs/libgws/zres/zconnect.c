

// Create socke and connect to the window server.


// Return the client fd.

int gramado_connect_ws (void);
int gramado_connect_ws (void)
{
    int client_fd = -1;
    
    // Porta para o Window Server 'ws' em gramado_ports[]
    struct sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    
    // Connecting to the window server in this machine.
    addr_in.sin_port   = PORTS_WS;   
    addr_in.sin_addr.s_addr = IP(127,0,0,1); 


    debug_print ("---------------------------\n");    
    debug_print ("gramado_connect_ws: Connecting to ws ...\n");


    // #debug
    printf ("gramado_connect_ws: Creating socket\n");


    // cria o soquete.
    // AF_GRAMADO
    //client_fd = socket ( 8000, SOCK_STREAM, 0 );
    //client_fd = socket ( AF_INET, SOCK_STREAM, 0 );
    client_fd = socket ( AF_INET, SOCK_RAW, 0 );
    
    if ( client_fd < 0 ){
       printf ("gramado_connect_ws: Couldn't create socket\n");
       exit(1);
    }

    //
    // connect
    // 

    // Nessa hora colocamos no accept um fd.
    // então o servidor escreverá em nosso arquivo.

    while(1){

        // #debug
        //printf ("gnst: Connecting to the address 'ws' ...\n");      
        printf ("gramado_connect_ws: Connecting to ws via inet  ...\n");   

        if (connect (client_fd, (void *) &addr_in, sizeof(addr_in)) < 0){ 
            gws_debug_print ("gramado_connect_ws: Connection Failed \n");
            printf          ("gramado_connect_ws: Connection Failed \n"); 
            //return -1;
            //try again 
        }else{ break; }; 
    };


    gws_debug_print ("gramado_connect_ws: done \n");

    return (int) client_id;
}
