// pci.c
// PCI interface support.
// 2016 - Created by Fred Nora.

#include "../bl.h"  

/*
 * pciConfigReadWord:
 *    Envia o comando para a porta 0xCF8, e 
 *    retorna o status na porta 0xCFC.
 * Argumentos:
 *    bus    = N�mero do BUS.           total 256.
 *    slot   = N�mero do slot. (device) total 32.
 *    func   = N�mero da fun��o.        total 8. 
 *    offset = Offset.
 *  Ex: 0x80000000 | bus << 16 | device << 11 | function <<  8 | offset.
 */

unsigned short 
pciConfigReadWord ( 
    unsigned char bus, 
    unsigned char slot, 
    unsigned char func, 
    unsigned char offset )
{
    unsigned long lbus  = (unsigned long) bus;
    unsigned long lslot = (unsigned long) slot;
    unsigned long lfunc = (unsigned long) func;

    unsigned long address=0;
    unsigned short tmp=0;

// Create configuration address and
// Write out the address.
    address = (unsigned long) ( (lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xfc) | ((unsigned long) 0x80000000) );
    out32( PCI_ADDRESS_PORT, address );

// Read in the data port.
// (offset & 2) * 8) = 0 
// Will choose the first word of the 32 bits register.  
    tmp = (unsigned short) ( ( in32 (PCI_DATA_PORT) >> ((offset & 2) * 8)) & 0xffff );

    return (unsigned short) tmp; 
}
 
/*
 * pciCheckDevice:
 *     Check device, offset 2. 
 */
unsigned short 
pciCheckDevice ( 
    unsigned char bus, 
    unsigned char slot )
{
// Get device.

//Off 0.
//Offset 2 = device.

    //unsigned short Vendor=0;    
    unsigned short Device=0;    

/*
// Pega o vendor. (Not used)
// PCI_OFFSET_VENDORID
    Vendor = pciConfigReadWord ( bus, slot, 0, 0 );    
    if (Vendor == PCI_INVALID_VENDORID){
        return (unsigned short) 0;
    }
*/

// Pega o device
// PCI_OFFSET_DEVICEID
    Device = pciConfigReadWord( bus, slot, 0, 2 ); 

    return (unsigned short) Device;
}

/*
 * pciCheckVendor:
 *     Check vendor, offset 0. 
 */
unsigned short 
pciCheckVendor ( 
    unsigned char bus, 
    unsigned char slot )
{
// Get vendor.

//Offset 0.
    unsigned short Vendor=0;    

//Pega o vendor.
//PCI_OFFSET_VENDORID
    Vendor = pciConfigReadWord ( bus, slot, 0, 0 );    
    if (Vendor == PCI_INVALID_VENDORID){
        return (unsigned short) 0;
    }

    return (unsigned short) Vendor; 
}

unsigned char 
pciGetClassCode ( 
    unsigned char bus, 
    unsigned char slot )
{
// Get class code.

    unsigned char ClassCode=0;
    ClassCode = (unsigned char) pciConfigReadWord ( 
                                    bus, slot, 0, 
                                    PCI_OFFSET_CLASSCODE );

    return (unsigned char) ClassCode;
}

/*
 * pciInfo:
 *    Pega e mostra informa��es sobre PCI.
 */
int pciInfo(void)
{
    unsigned char i=0;
    unsigned char j=0;

// Offset 0 e 2.
    unsigned short Vendor=0;
    unsigned short Device=0;

    printf ("PCI INFO: \n");
    printf ("========= \n");

// This allows up to 256 buses, 
// each with up to 32 devices, 
// each supporting 8 functions.

//Bus.
//Slots. (devices)

    i=0;
    j=0;

    // bus
    for ( i=0; i< 0xFF; i++ )    
    {
        // device
        for ( j=0; j<32; j++ )
        {
		    //Checks.
		    Vendor = pciCheckVendor (i, j);
		    Device = pciCheckDevice (i, j);
		   
		    //Show. (Se existe um dispositivo.)
		    
			if (Device != 0)
			{
	            printf ("/BUS_%d/SLOT_%d/VENDOR_%x/DEVICE_%x \n", 
				    i, j, Vendor, Device );
		    };

            //@todo: Registrar o que foi encontrado em estrutura.
            //usar malloc pra alocar memoria pra estrutura. 
        };
    };


	/*
	// Checa v�rios slots no bus 0. 
	i = 0;
	do
	{
		// Checando 32 dispositivos (slots) no BUS 0.
		
		//checks.
		Vendor = pciCheckVendor(0, i);
		Device = pciCheckDevice(0, i);
        
		//show.
		if(Device != 0){
	        printf("@PCI:/BUS_0/SLOT_%d/VENDOR_%x/DEVICE_%x \n",i ,Vendor ,Device);
		};
		i++;
		
	}while(i < PCI_MAX_DEVICES);
	
	
	//Checa v�rios slots no bus 1.
	i = 0;
	do
	{
		// Checando 32 dispositivos (slots) no BUS 1.
		
		//checks.
		Vendor = pciCheckVendor(1, i);
		Device = pciCheckDevice(1, i);
        
		//show.
		if(Device != 0){
	    printf("@PCI:/BUS_1/SLOT_%d/VENDOR_%x/DEVICE_%x \n",i ,Vendor ,Device);
		};
		i++;
		
	}while(i < PCI_MAX_DEVICES);


	//Checa v�rios slots no bus 2.
	i = 0;
	do
	{
		// Checando 32 dispositivos(slots) no BUS 2.
		
		//checks.
		Vendor = pciCheckVendor(2, i);
		Device = pciCheckDevice(2, i);
        
		//show.
		if(Device != 0){
	    printf("@PCI:/BUS_2/SLOT_%d/VENDOR_%x/DEVICE_%x \n",i ,Vendor ,Device);
		};
		i++;
		
	}while(i < PCI_MAX_DEVICES);

	
	//
	// Detect hardware.
	//
	
    //hal_hardware_detect();
	*/


// Done
    printf ("Done\n");
    return 0;
}


/*
 * pciInit:
 *     Initialize PCI.
 */

int pciInit(void)
{

	//int i;
	//int j;
    
	//Offset 0.
	//Offset 2.
	
	//unsigned short Vendor;    
	//unsigned short Device;    
    //...
	
   //
   // Bug Bug !!	
   //	
	
	/*
	 * @todo:
	 *    Alocar mem�ria para a estrutura PCI criada.
	 *    Inicializar algumas vari�veis da estrutura PCI.
	 *
	 */
    
	/*
	int Status;
	Status = pci_info();    //@todo: Talvez essa fun��o ja foi chamada outra hora. Checar. 
	
	if( Status == 0 ){
	    goto done;	
	};
    */
	
	
	//
	// Probe pci buses.
	//
	
	
	//
	// Probe PCI devices. (procura os controladores liagados ao barramento pci.)
	//
	// @todo: essa rotina deve ir para o modulo pci.
	//

	
/*	
	i=0;    //Bus.
	j=0;    //Slots. (devices)
	
	//bus
	for(i=0; i<0xff; i++)
	{
		//Device.
	    for(j=0; j<32; j++)
        {
		    //Checks.
		    Vendor = pciCheckVendor(i, j);
		    Device = pciCheckDevice(i, j);
		   
		    //Show. (Se existe um dispositivo.)
		    if(Device != 0)
			{
	            //printf("/BUS_%d/SLOT_%d/VENDOR_%x/DEVICE_%x \n" ,i ,j ,Vendor ,Device);
	
                //colocar os dispositivos encontrados em estruturas.
			    current_pci_device = (void*) malloc( sizeof( struct pci_device_d  ) );
	            if( (void*) current_pci_device != NULL )
				{
				    current_pci_device->Vendor = (unsigned short) Vendor;
				    current_pci_device->Device = (unsigned short) Vendor;
				    //@todo: colocar a estrutura na lista.
					//ex: pcideviceList[0];
					//...
				};
			    //Nothing.
		    };		
		};
	};



done:	
    //g_driver_pci_initialized = (int) 1; 
	printf("Done.\n");
	
	*/


    return 0;
}

//
// End
//

