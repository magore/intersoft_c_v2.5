DISK 1
This disk contains:
c/cmd        - The V2.5 C compiler.
clib/rel     - The searchable compiled runtime support
               library module.
stdio/h      - The standard #include file.
runlib/mac   - The 16 bit runtime support source.
cpy/c        - copy program.
readme/txt   - This file.

DISK 2
Contains the sources for all runtime support functions
except runlib/mac ( on DISK 1 ).
Also the split, merge and extract C source programs.        
DISK 3 & 4 ( if ordered )
Contains the V2.5 compiler source.
Also merge/cmd, split/cmd & extract/cmd.
See readme/txt on disk 4.

   If you have ordered V2.5 C without source you will
have to compile merge/c, extract/c & split/c on disk 2.
That is of course if you need the programs at all. For
those of you with source the files merge/cmd, split/cmd
& extract/cmd are supplied on disk 4.


To compile merge/c, split/c & extract/c use the following
commands.

Note:
In the following instructions "d" refers to the drive 
number that you want.

To compile merge/c type:

c merge/c o=merge/mac:d
m80 merge:d=merge:d
l80 merge,clib-s,merge:d-n-e:ccmain


To compile split/c type:

c split/c o=split/mac:d
m80 split:d=split:d
l80 split,clib-s,split:d-n-e:ccmain


To compile extract/c type:

c extract/c o=extract/mac:d
m80 extract:d=extract:d
l80 extract,clib-s,extract:d-n-e:ccmain
 