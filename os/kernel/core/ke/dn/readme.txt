
  ke/ represents the main folder for the kernel base.
  ke/hal/ is the HAL.
  ke/intake/ is the microkernel.



  Intake has two different development level,
  two different realms:

------------------------------------------
  HIGH LEVEL
  + Process and Thread management:
    Where we create/delete processes and threads.
    ...

------------------------------------------
  LOWLEVEL
  + Dispatch and Scheduler
    Where we launch, change the state and schedule the threads.