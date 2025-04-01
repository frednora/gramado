
// product.h

#ifndef __PRODUCT_PRODUCT_H
#define __PRODUCT_PRODUCT_H    1

//
//  == Product name ==========================================
//

#define PRODUCT_NAME_SHORT  "Gramado"
#define PRODUCT_NAME_LONG   "Gramado OS"
#define PRODUCT_NAME  PRODUCT_NAME_LONG

//
//  == Product type ==========================================
//


// --------------------------------------------------------

// Baremetal hypervisor.
#define PT_GRAMADO_HYPERVISOR           1000
#define PT_GRAMADO_HYPERVISOR_CLI       1001
#define PT_GRAMADO_HYPERVISOR_HEADLESS  1002
#define PT_GRAMADO_HYPERVISOR_STRING  "Hypervisor"

// The business server.
#define PT_GRAMADO_SERVER           2000
#define PT_GRAMADO_SERVER_CLI       2001
#define PT_GRAMADO_SERVER_HEADLESS  2002
#define PT_GRAMADO_SERVER_STRING  "Server"

// Enterprise machines to access the business server.
#define PT_GRAMADO_WORKSTATION           3000
#define PT_GRAMADO_WORKSTATION_CLI       3001
#define PT_GRAMADO_WORKSTATION_HEADLESS  3002
#define PT_GRAMADO_WORKSTATION_STRING  "Workstation"

// Regular desktop machine.
#define PT_GRAMADO_DESKTOP           4000
#define PT_GRAMADO_DESKTOP_CLI       4001
#define PT_GRAMADO_DESKTOP_HEADLESS  4002
#define PT_GRAMADO_DESKTOP_STRING  "Desktop"

// IOT devices
#define PT_GRAMADO_IOT           5000
#define PT_GRAMADO_IOT_CLI       5001
#define PT_GRAMADO_IOT_HEADLESS  5002
#define PT_GRAMADO_IOT_STRING  "IOT"

// --------------------------------------------------------

// Current product type
// Updated by the builder.

#define PRODUCT_TYPE         PT_GRAMADO_HYPERVISOR
//#define PRODUCT_TYPE         PT_GRAMADO_HYPERVISOR_HEADLESS
#define PRODUCT_TYPE_STRING  PT_GRAMADO_HYPERVISOR_STRING

//#define PRODUCT_TYPE         PT_GRAMADO_DESKTOP
//#define PRODUCT_TYPE_STRING  PT_GRAMADO_DESKTOP_STRING

#define MACHINE_TYPE_STRING  "x86_64"


extern int g_product_type; 


#endif    

