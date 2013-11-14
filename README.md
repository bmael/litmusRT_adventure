litmusRT_adventure
==================

This project is the final project study of M2 ALMA master (Nantes university). 
We have to modify JetBench to works under a litmus^RT kernel.

Samples that we developed
-------------------------
First Step: 

	- We create a useless real time task to understand how it works. 
	  Take a look at src/samples/Aprinter.c based on base_task.c template source
	  code from liblitmus.
	
Second Step:

	- We create a useless real time multithreaded task to understand how it works.
	  Take a look at src/samples/Aprinter_mt.c based on base_task_mt.c template 
	  source code from liblitmus.
	  
Third Step:

	- We use Aprinter_mt.c to replace basic threads by STM TL2 threads.
	  Take a look at src/samples/Aprinter_mt_tl2.c.
	  
How to launch these samples
--------------------------- 
	- Install litmus^RT. You can find some help here: 
	  https://wiki.litmus-rt.org/litmus/InstallationInstructions
    - Go to src/samples/ and use make command to build the samples.
    - To execute a sample jsute type the name of the sample to launch without 
      extension
	

