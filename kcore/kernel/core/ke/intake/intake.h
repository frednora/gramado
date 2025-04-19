// intake.h
// 'Intake' is the microkernel component for the kernel base.
// Created by Fred Nora.

// This is an enbedded module that represents
// the microkernel components
// This kernel actually is not a microkernel.

#ifndef __KE_MK_H
#define __KE_MK_H    1

/*
 * Segments @todo: Include this constants.
 * it's Intel's specific stuff.
 *
#define GDT_KCODE   0x08
#define GDT_KDATA   0x10
#define GDT_UCODE   (0x18 | 3)
#define GDT_UDATA   (0x20 | 3)
*/

//
// == prototypes =========================
//

int keInitializeIntake (void);

#endif   

