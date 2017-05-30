/*
 * Intersoft C Support Package for the TRS-80
 * Copyright 1982 by Intersoft
 * By: Mike Gore, Richard McMurray, Bernie Roehl
 */

/* System parameters */
#define MAXFILES 15
#define BUFFSIZE 256
#define LRL      256

/* Ascii characters */
#define CR    0x0D
#define QUOTE 0x22
#define APOST 0x27

/* File modes */
#define READ   'R'
#define WRITE  'W'
#define APPEND 'A'

/* Flag bits */
#define ZEROMASK 0x0040
#define SYSEOF   0x1C00
#define SYSNRF   0x1D00

/* Device name abbreviations */
#define BI 0x4249
#define CI 0x4349
#define CO 0x434F
#define PR 0x5052

/* DOS entry points */
#define ABORT   0x4030
#define WARMS   0x402D

/* System DCB Addresses */
#define DSPDCB 0x401D
#define PRTDCB 0x4025
#define KEYDCB 0x4015

/* Address of the command line */
#define DOSCMD  0x4318

/* Size of a disk file control block */
#define FCBSIZE 32

/* File access modes:
 * RDMSK - read
 * WRMSK - write 
 * APMSK - Append
 * DVMSK - Device, not disk file.
 */
#define RDMSK 0x01
#define WRMSK 0x02
#define APMSK 0x06
#define DVMSK 0X08

#asm
;
; DUPLICATES OF C DEFINITIONS, FOR USE BY THE ASSEMBLER
; PORTION OF THE I/O PACKAGE.
;
; *** MUST DUPLICATE THE CORRESPONDING C DEFINITIONS ***
;
TRUE     EQU 1
FALSE    EQU 0
RDMSK    EQU 01H
WRMSK    EQU 02H
APMSK    EQU 06H
DVMSK    EQU 08H
STDIN    EQU 1
MAXFILES EQU 15
EOF      EQU -1
CR       EQU 13
EOL      EQU 30
;
; Addresses of DOS routines.
;
ATFSPEC EQU 441CH ; Parse a file specifier
ATPEOF  EQU 4448H ; Go to end of a file
ATINIT  EQU 4420H ; Create a file
ATOPEN  EQU 4424H ; Open a file
ATCLOSE EQU 4428H ; Close a file
;
; Address of Rom routines.
;
ATPUT   EQU 001BH ; Write a byte to a file
ATGET   EQU 0013H ; Read a byte from a file
ATBUF   EQU 0040H ; Buffered keyboard input
ATKEY   EQU 002BH ; Unbuffered keyboard input
ATDISP  EQU 0033H ; Unbuffered console output
;
; Address of system top of memory pointer.
;
MEMEND  EQU 4049H ; Top of memory pointer
#endasm


int argc,             /* Number of arguments */
    argv[20],         /* Vector of command-line arguments */
    ccbfin,           /* Buffered keyboard input buffer index */
    ccdcbs[MAXFILES], /* Device Control Blocks */
    ccbuff[MAXFILES], /* Device buffers */
    ccmode[MAXFILES], /* Modes of files */
    ccugf[MAXFILES],  /* Flags for ungetc(c,unit) */
    ccerr[MAXFILES],  /* Error number for ferror(unit) */
    ccugc[MAXFILES],  /* Characters for ungetc(c, unit) */
    ccdvc[MAXFILES],  /* Device names */
    ccdvr[MAXFILES];      /* Driver addresses */


char *ccmem,     /* Free-space pointer */
     *ccstkmg,   /* Stack margin for alloc() and free() */
     *ccinbf,    /* Buffer address for *BI */
     cccmd[80];  /* Command line buffer */
