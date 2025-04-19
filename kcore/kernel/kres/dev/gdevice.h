/*
 *  File: gdevice.h
 *
 * Definição:
 * Informações globais sobre a gerência de dispositivos.
 * + Informações globais sobre o dispositivo onde o sistema operacional 
 * foi instalado.
 * + Informações sobre o host device.
 * + Não é a qui o lugar para se detalhar todos os dispositivos instalados na máquina.
 * Aqui é lugar de classificar a plataforma alvo para o sistema operacional. Para
 * que as aplicações possam se adaptar às características do dispositivo que suportam
 * o sistema operacional.
 *
 * 2016 - Created.   
 */

#ifndef ____GDEVICE_H
#define ____GDEVICE_H  1
 

/*
 * driver_type_t: 
 *     Tipos de drivers. 
 *     + O driver de sistema é do tipo persistente. Não poderá ser descarregado,
 * permanecerá na memória o tempo todo. Poderá ser desativado. Pode estar 
 * em qualquer ring.
 *     + O driver modular poderá ser descarregado. 
 */ 
 
typedef enum {
    
    DriverTypeSystem,     //Persistente.
    DriverTypeModular,    //Modular.
    //...

}driver_type_t; 


/*
 * host_device_type_t:
 *     Classes de dispositivos segundo o host que pertencem.
 *     Classes mais comuns de dispositivos.
 *     Outras poderão ser incluídas futuramente. 
 */ 
typedef enum {

    HostDeviceTypePC,        //PC.
    HostDeviceTypeMobile,    //Mobile.
    HostDeviceTypeIOT,       //Iot.
    //...

}host_device_type_t; 


struct host_device_info_d
{

//Type.
    host_device_type_t type;

// Screen size.
    unsigned long host_device_screen_width;
    unsigned long host_device_screen_height;

    //Continua ...

};

struct host_device_info_d *HostDeviceInfo;



/*
 * #bugbug essa estrutura ja deve ter 
 * sido definida em outro lugar , por isso 
 * esta dando problemas.
 * @todo: tirar o x.
 *
typedef struct xdevice_driver_d xdevice_driver_t;
struct xdevice_driver_d
{
	driver_type_t driver_type;  //do sistema ou modular.
	
	struct host_device_info_d *device_info;
	
	int status;  //habilidado ou desabilitado.
	
	//...
};
*/
 
// 
// Globais de fácil acesso.
// devem refletir os valores armazenados na estrutura.
//

// Screen sizes and bpp.
// see: bldisp.c
extern unsigned long g_device_screen_width;
extern unsigned long g_device_screen_height;
extern unsigned long g_device_screen_bpp; 

#endif   

 

//
// End.
//

