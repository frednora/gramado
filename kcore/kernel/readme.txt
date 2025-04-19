
arch    - Entry point and initialization.
core    - Core components and Control resources layer.
include - Main header files for the kernel.
kres    - Kernel resources layer.
your    - Stuff


======================
About core/ folder:
  It is basically for CPU and process stuff.

======================
About kres/ folder:

  Based on the idea that we need to control the kernel resourses used 
by the process groups, we put all these resources into a single folder called kres/. 

  So the core/ folder needs to control the access to theses resources 
inside the kres/ folder.
  mm/ is also considered a resource, so the right place for this compiment is inside kres/ 
not as a core/ component.


      
