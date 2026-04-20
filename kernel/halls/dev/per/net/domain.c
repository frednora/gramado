// domain.c
// 2023 - Created by Fred Nora.


#include <kernel.h>


static struct domain_d *__domain_create_object(void);

//
// ===========================================================
//

// Create domain object
static struct domain_d *__domain_create_object(void)
{
    struct domain_d *d;
    d = (void*) kmalloc( sizeof(struct domain_d) );
    if ((void*) d == NULL){
        panic("domain_create_object: d\n");
    }
    memset (d, 0, sizeof(struct domain_d) );

    d->initialized = FALSE;
    d->is_dc = FALSE;
    d->used = TRUE;
    d->magic = 1234;
    return (struct domain_d *) d;
}

// Create new domain.
// #todo: More parameters.
struct domain_d *domain_create_domain(const char *name)
{
    struct domain_d *d;

    d = (struct domain_d *) __domain_create_object();
    if ((void*) d == NULL){
        goto fail;
    }

// Name
    if ((void*) name == NULL){
        printk("domain_create_domain: Invalid name\n");
        goto fail;
    }
    if (*name == 0){
        printk("domain_create_domain: *name\n");
        goto fail;
    }
    d->domain_name = (char *) name;

    // ...
    d->initialized = TRUE;
    return (struct domain_d *) d;

fail:
    return NULL;
}

// Set this domain as a Domain Controller.
int domain_set_as_dc(struct domain_d *domain)
{
    if ((void*) domain == NULL){
        goto fail;
    }
    if (domain->magic !=1234){
        goto fail;
    }

    domain->is_dc = TRUE;
    return TRUE;

fail:
    return FALSE;
}

