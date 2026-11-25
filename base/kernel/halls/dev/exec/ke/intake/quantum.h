// quantum.h
// Created by Fred Nora.

#ifndef __INTAKE_QUANTUM_H
#define __INTAKE_QUANTUM_H    1

// ===============================================

// Credits in ms.
// See: config/config.h

// Class 1: Priority Normal
#define QUANTUM_Q1  (CONFIG_QUANTUM_MULTIPLIER * 1)
#define QUANTUM_Q2  (CONFIG_QUANTUM_MULTIPLIER * 2)
#define QUANTUM_Q3  (CONFIG_QUANTUM_MULTIPLIER * 3)
// Class 2: Priority System
#define QUANTUM_Q4  (CONFIG_QUANTUM_MULTIPLIER * 4)
#define QUANTUM_Q5  (CONFIG_QUANTUM_MULTIPLIER * 5)
#define QUANTUM_Q6  (CONFIG_QUANTUM_MULTIPLIER * 6)

// -----------------------------------------
// Class 1: 
#define QUANTUM_NORMAL_THRESHOLD      QUANTUM_Q1  // Normal, threashold
#define QUANTUM_NORMAL_BALANCED       QUANTUM_Q2  // Normal, balanced
#define QUANTUM_NORMAL_TIME_CRITICAL  QUANTUM_Q3  // Normal, time critical
// Class 2: 
#define QUANTUM_SYSTEM_THRESHOLD        QUANTUM_Q4  // Real time, threashold
#define QUANTUM_SYSTEM_BALANCED         QUANTUM_Q5  // Real time, balanced
#define QUANTUM_SYSTEM_TIME_CRITICAL    QUANTUM_Q6  // Real time, time critical
// -----------------------------------------

#define QUANTUM_MIN    QUANTUM_Q1
#define QUANTUM_MAX    QUANTUM_Q6
//----------------------------------------------

#endif    


