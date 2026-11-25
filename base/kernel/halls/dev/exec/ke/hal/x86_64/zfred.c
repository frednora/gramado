
// fred.c
// #todo:
// FRED for Intel processors.

//
// FRED
//

/*
FRED:

== Enumeration:
CPUID.(EAX=7,ECX=1):EAX[bit 17] is a new feature flag 
that enumerates support for the new FRED transitions. 
It also enumerates support for the new architectural state
(MSRs) defined by FRED.
CPUID.(EAX=7,ECX=1):EAX[bit 18] is another new CPUID feature flag 
that enumerates support for the LKGS instruction.

== Enabling in CR4:
When FRED transitions are enabled (CR4.FRED = IA32_EFER.LMA = 1), 
IDT event delivery of exceptions and interrupts is replaced 
with FRED event delivery.
Setting CR4.FRED enables FRED event delivery. CR4[32].

== New MSRs for Configuration of FRED Transitions:
+IA32_FRED_CONFIG (MSR index 1D4H).
+IA32_FRED_RSP0, IA32_FRED_RSP1, IA32_FRED_RSP2, and IA32_FRED_RSP3
(MSR indices 1CCH–1CFH).
+IA32_FRED_SSP1, IA32_FRED_SSP2, and IA32_FRED_SSP3 (MSR indices 1D1H–
1D3H).

-------------
far CALL, far JMP, far RET, and IRET. Enabling FRED
transitions modifies the operation of these instructions. 
A FRED-enabled operating system cannot use them for ring transitions.

*/


