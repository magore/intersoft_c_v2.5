    This file contains the options that must be selected
to compile and link you V2.5 C compiler.

Notes:
   The drive number for input and output files depends
on your disk organization. Here they are set to drive 1.


The files option/scx, vars/scx & consts/scx must be present
on the system when the C compiler is compiled.


These are the options to compile V2.5 C source:

c main/scx:1 o=main/mac:1 -n f=1 
c io/scx:1 o=io/mac:1 -n -g f=1 
c utils/scx:1 o=utils/mac:1 -n -g f=1 
c symtab/scx:1 o=symtab/mac:1 -n -g DP=1575 #D=110 f=1
c err/scx:1 o=err/mac:1 -n -g f=1 
c prep/scx:1 o=prep/mac:1 -n -g f=1 
c parse1/scx:1 o=parse1/mac:1 -n -g f=1 
c parse2/scx:1 o=parse2/mac:1 -n -g #g=170 f=1
c parse3/scx:1 o=parse3/mac:1 -n -g f=1
c tables/scx:1 o=tables/mac:1 f=1
c os/scx:1 o=os/mac:1 -n -g lp=280 dp=1300 f=1
c codegen/scx:1 o=codegen/mac:1 -n -g #g=180 f=1
c stack/scx:1 o=stack/mac:1 -n -g f=1


At this stage you must assemble the /mac files to create
/rel files.

This is the link order to creat a compiler from the 
compiled and assembled source files. The clib library
in this example resides on disk 0. The compiler name
is set to "newc:1" which is newc/cmd:1 by the default
convension of L80.


L80 main:1,io:1,utils:1,symtab:1,err:1,prep:1,parse1:1
parse2,parse3:1,os:1,tables:1,codegen:1,stack:1
clib:0-s
-u,newc:1-n,-e:ccmain
 