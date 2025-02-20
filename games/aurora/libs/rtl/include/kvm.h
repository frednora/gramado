
/*	$NetBSD: kvm.h,v 1.17 2016/01/22 21:55:57 dholland Exp $	*/

#ifndef _KVM_H_
#define	_KVM_H_

/* Default version symbol. */
#define	VRS_SYM		"_version"
#define	VRS_KEY		"VERSION"

/*
 * Flag for kvm_open*() to disable opening of kernel files - used
 * by programs that use only sysctl() function to access kernel
 * information.
 */
#define KVM_NO_FILES	((int)0x80000000)

//typedef struct __kvm kvm_t;

//...

#endif /* !_KVM_H_ */


