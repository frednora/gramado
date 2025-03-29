
  In this folder you can find the documentation for the network infrastructure.

Subfolders:


------------------------------------------
kernel/ - The ring0 infrastructure for network support.

 + Device drivers for NIC devices. 
 + Implementation of sockets in ring0.
 + Protocol stack in ring0.
 + Network routines in ring0.
 + ...

------------------------------------------
userland/ - The ring3 infrastructure for network support.

 + The syscalls related to the network services offered by the kernel.
 + The main network server. (netd/)
   The libraries for client-side applications.
 + Servers. (webd/,ftpd,telnetd ...)
   The libraries for the client-side applications.

 + ...
