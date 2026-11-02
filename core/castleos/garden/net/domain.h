// domain.h
// Network domain support.
// 2023 - Created by Fred Nora.

#ifndef __NET_DOMAIN_H
#define __NET_DOMAIN_H    1

/*
#define DOMAIN_TYPE_XXXX  1000
#define DOMAIN_TYPE_XXXX  2000
#define DOMAIN_TYPE_XXXX  3000
// ...
*/

// It describes the domain the node belongs to.
struct domain_d 
{
    int used;
    int magic;
    int initialized;
    //int type;
    char *domain_name;
    int is_dc;  // Is it a domain controller.

    // ...
    // #todo: Include here some machine information.
};

// ============================================

struct domain_d *domain_create_domain(const char *name);
int domain_set_as_dc(struct domain_d *domain);

#endif   

