# kernel - The kernel.

This is code for the base kernel. It builds the image KERNEL.BIN.


## Folders

```
core/:
This is the hot spot, the singularity.
  core/apps  - Putting the whole universe into here.
  core/arch  - Entry point and initialization.
  core/ke    - Task manager.
  core/kmain - The main initialization routine and some wrappers. 

gramk/:
The interface for the graphics device and the user interaction manager.

include/:
Main header files for the kernel.

libk/:
The kernel library.

res/:
Kernel resources. For containers.

```

## About core/ folder:

```
  It is basically for CPU and process stuff.
```

## About kres/ folder:

```
  Based on the idea that we need to control the kernel resourses used 
by the process groups, we put all these resources into a single folder called kres/. 

  So the core/ folder needs to control the access to theses resources 
inside the kres/ folder.
  mm/ is also considered a resource, so the right place for this compiment is inside kres/ 
not as a core/ component.
```


      
