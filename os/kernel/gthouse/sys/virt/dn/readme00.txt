# Note on Virtualization: AMD vs Intel

------------------------
AMD SVM (VMCB)
------------------------
- AMD uses the Virtual Machine Control Block (VMCB).
- VMCB is simpler and directly accessible by the hypervisor.
- Layout: Two areas (control area + state save area).
- Access: Direct memory access.
- Nested paging: NPT (Nested Page Tables).

------------------------
Intel VMX (VMCS)
------------------------
- Intel uses the Virtual Machine Control Structure (VMCS).
- VMCS is more rigid and requires special instructions for access.
- Layout: Multiple areas (guest state, host state, control fields).
- Access: VMREAD / VMWRITE instructions.
- Nested paging: EPT (Extended Page Tables).

