This directory contains a tool to collect reuse distance
histograms. The tool counts data accesses in a unit of cache line
width (64bits).

To download and install Intel PIN tool:
   Download PIN:
   https://software.intel.com/en-us/articles/pin-a-binary-instrumentation-tool-downloads
   Install PIN:
   
   $ tar zxf pin-3.2-81205-gcc-linux.tar.gz
   
   $ cd pin-3.2-81205-gcc-linux
   
   Get PIN path:
   
   $ pwd 

To configure: 
   You need to modify the PIN path in the files makefile and
   run.sh to your corresponding PIN path.
   
To make:
   $ make
   or
   $ make obj-intel64/rdpersig.so

To run:
   Suppose you want to collect the reuse distance histogram of "ls"
   command. You just need type the following:
     sh /run.sh /bin/ls

   An output file named "rd_hist.txt" will be created in the current
   directory. It contains the reuse distance histogram of the "ls"
   execution. The file format is self-explained. A note is that "Total
   data" means the total number of distinct data in the whole
   execution, and "Total access" is the total number of memory
   accesses in the execution.


   You want to start with the file named "rdpersig.cpp", which contains
   the main function.
   The major file containing reuse distance computation is
   "ScaleTree/analyze.c". The algorithm is in Setion 2.1 of the
   following paper:
   http://portal.acm.org/ft_gateway.cfm?id=781159&type=pdf&coll=GUIDE&dl=GUIDE&CFID=71410679&CFTOKEN=82162176

   Essentially it organizes data accesses in a tree to accelerate
   reuse distance computation. 


   
  

