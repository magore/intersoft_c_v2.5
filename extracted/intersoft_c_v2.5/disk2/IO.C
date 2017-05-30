/*
 *   Intersoft C Support Package for the TRS-80
 *   Copyright 1982 by Intersoft
 *   By: Mike Gore, Richard McMurray, Bernie Roehl
 */


!CCSETU
#include "STDIO/H"
#include "IO/H"

/* Set up the C environment and call the program */
ccsetup()
{
  ccioin();
  main(argc, argv);
  exit(0);
}


!CCIOIN
#include "STDIO/H"
#include "IO/H"

/* Initialize the i/o package, parse the command line,
 * re-direct the standard i/o files.
 */
ccioin()
{
  char *ptr, *mode, *iptr, *optr;
  int c, i, tc;
  int unit;

/* Set the stack margin, initialize the dynamic
 * memory package
 */
  setstack();
  meminit();

  /* Initialize all file units to be closed */
  for (unit = STDIN; unit < MAXFILES; ++unit) {
  ccdcbs[unit] = ccmode[unit] = 0;
  ccbuff[unit] = ccerr[unit] = 0;
  ccugc[unit] = 0;
  ccugf[unit] = FALSE;
  ccdvc[unit] = 0;

/*  ccbad will set 0x0800 in ccerr[unit] and return EOF if 
 *  ccdvr[unit] is not set to a valid driver and i/o is 
 *  attempted. This should never happen!
 */
  ccdvr[unit] = ccbad;
}

  /* Save the command line, terminate it with a CR */
  ptr = DOSCMD;
  for (i = 0; i < 64; ++i)
    cccmd[i] = *ptr++;
  cccmd[i] = CR;

  /* Parse the command line */
  argc = 0;      /* No arguments */
  iptr = "*BI";  /* Default standard input */
  optr = "*CO";  /* Default standard output */
  mode = "w";    /* Default output (write, not append) */
  ptr = cccmd;   /* Start of command line */
  do {
    while ((c = *ptr) == ' ') ++ptr;  /* Skip blanks */
      if (c == CR)
        break; /* End of command line */
    tc = ' ';  /* Default termination character is blank */
    switch (c) {  /* What did we hit ? */
      case '<' :
        iptr = ptr + 1;  /* Re-direct standard input */
        break;
      case '>' :
        if (ptr[1] == '>') {    /*  '>>' ? */
          mode = "a";   /* If so, mode is append */
          ++ptr;        /* Point past second > */
        }
        optr = ptr + 1; /* Re-direct standard output */
        break;
      case QUOTE :
      case APOST :
        tc = c;
        ++ptr;
      default :
        argv[argc++] = ptr; /* Normal argument */
    }
    /* Skip until the termination character or CR */
    do
      c = *ptr++;
    while ((c != CR) & (c != tc));
    *(ptr-1) = 0;
  } while (c != CR);

  /* Open the standard input, output, and error files */
  if (!fopen(iptr, "r")) {
    sysmsg("Can't open stdin\n");
    exit(1);
  }
  if (!fopen(optr, mode)) {
    sysmsg("Can't open stdout\n");
    exit(1);
  }
  if (!fopen("*CO", "w")) {
    sysmsg("Can't open stderr\n");
    exit(1);
  }
}


!SETSTA
#include "STDIO/H"
#include "IO/H"

/* Initialize the stack margin to 1024 bytes */
setstack()
  ccstkmg = 1024;


!CKOPEN
#include "STDIO/H"
#include "IO/H"

/*
 * Open a file and return the unit.  If the file fails to 
 * open then a fatal error message is written to STDOUT and
 * execution is terminated.
 */
ckopen(fname, mode)
  char *fname, *mode;
{
  int unit;

  if (!(unit = fopen(fname, mode))) {
    fputs("Can't open ", STDERR);
    fputs(fname, STDERR);
    putc(EOL, STDERR);
    exit(1);
  } 
  return unit;
}



!EXIT
#include "STDIO/H"
#include "IO/H"

/* Exit the C program, closing all files.
 *
 * If n is zero the program will exit normally,
 * otherwise the program will abort. The abort
 * exit will terminate execution of any "Chain" or "DO"
 * file processing running under the DOS
 */
exit(n)
  int n;
{
  int unit, fn;
 
  for (unit = STDIN; unit < MAXFILES; ++unit)
    fclose(unit);
  ccexit(n ? ABORT : WARMS);
}



!FOPEN
#include "STDIO/H"
#include "IO/H"

/* Open a file or device.
 * File names are any valid file name under the DOS
 * being used.  The file name is turned to upper case
 * before the file is opened.
 * Valid modes are:
 *      "r" or "R" = Read
 *      "w" or "W" = Write
 *      "a" or "A" = Append
 * Returns the file descriptor (unit) or NULL upon error.
 */
fopen(name, mode)
  char *name, *mode;
{
  int unit, acc, ret, modemsk;
  char *fcb, *buff;

  /* Find a free unit */
  for (unit = STDIN; unit < MAXFILES; ++unit)
    if (!ccdcbs[unit])
      break;
  if (unit == MAXFILES)
    return NULL;       /* Couldn't find free unit */

/*   Reset FCB, mode, buffer pointer, error number, 
 *   ungetc character, ungetc flag & device flag
 */
  ccdcbs[unit] = ccmode[unit] = 0;
  ccbuff[unit] = ccerr[unit] = 0;
  ccugc[unit] = 0;
  ccugf[unit] = FALSE;
  ccdvc[unit] = 0;
/*  ccbad will set 0x0800 in ccerr[unit] and return EOF if 
 *  ccdvr[unit] is not set to a valid driver and i/o is 
 *  attempted. This should never happen!
*/
  ccdvr[unit] = ccbad;

  modemsk = 0;         /* Reset mode mask */

  buff = name;         /* Uppercase the file name */
  for (; *buff = toupper(*buff); buff++);

  mode = toupper(*mode); /* Uppercase the file mode */
  if (*name == '*')
    return ccdvop(name, unit, mode); /* Open a device */

  fcb = alloc(FCBSIZE); /* Allocate an FCB */
  if (!fcb)
    return NULL;
  ret = ccfspec(fcb, name); /* Put the filename in the FCB */
  if (ret >= 256) {
    free(fcb);
    return NULL; /* Couldn't put the file name in the FCB */
  }

  buff = alloc(BUFFSIZE); /* Allocate a disk buffer */
  if (!buff)
    return NULL;

  switch (mode) {

    case APPEND :
      ret = ccopen(LRL, fcb, buff); /* Open file */
      if (ret >= 256)
        break;
      ret = ccpeof(fcb);  /* Move to end of file */
      modemsk = WRMSK;    /* Access mode is write */
      /* ccatput writes a byte to a file */
      ccdvr[unit] = ccatput;
      break;
 
   case READ :
      ret = ccopen(LRL, fcb, buff); /* Open file */
      modemsk = RDMSK;
      /* ccatget reads a byte from a file */
      ccdvr[unit] = ccatget;
      break;

    case WRITE :
      ret = ccinit(LRL, fcb, buff); /* Initialize file */
      modemsk = WRMSK;
      /* ccatput writes a byte to a file */
      ccdvr[unit] = ccatput;
      break;

    default :
      free(fcb);
      free(buff);
      return NULL;  /* Invalid file access mode */
  }

  if (ret < 256) {
    ccdcbs[unit] = fcb;
    ccmode[unit] = modemsk;
    ccbuff[unit] = buff;
    return unit;
  }

  free(fcb);
  free(buff);
/*  ccbad will set 0x0800 in ccerr[unit] and return EOF if 
 *  ccdvr[unit] is not set to a valid driver and i/o is 
 *  attempted. This should never happen!
 */
  ccdvr[unit] = ccbad;
  return NULL;
}



/* Open a device
 * This should only be called from fopen() !
 * Valid devices are:
 *     *BI = buffered keyboard input
 *     *CI = unbuffered keyboard input
 *     *CO = unbuffered console output
 *     *PR = printer output
 * Valid Modes are:
 *     "r" or "R" = Read
 *     "w" or "W" = Write
 *     "a" or "A" = Append
 * Returns the file descriptor (unit) or NULL upon error
 */
ccdvop(name, unit, mode)
  char *name;
  int unit, mode;
{
  int modemsk, dvc;
  char *driver, *fcb;

  dvc = ( name[1] << 8 | name[2]);
  switch (dvc) {

    case BI :  /* Buffered keyboard input */
      if (ccdvck(BI) || mode != READ)
        return NULL; /* already open or invalid mode */
      ccinbf = alloc(BUFFSIZE); /* Allocate a buffer */
      if (!ccinbf)
        return NULL;
      ccbuff[unit] = ccinbf;
      ccbfin = 0;      /* Empty the buffer */
      /* Address of our buffered console input routine */
      ccdvr[unit] = bufdvr;
      ccdcbs[unit] = KEYDCB;
      ccmode[unit] = RDMSK | DVMSK; /* Read only device */
      break;

    case CI :  /* Unbuffered keyboard input */
      if (mode != READ)
        return NULL;     /* Invalid mode */
      /* Address of our unbuffered console input routine */
      ccdvr[unit] = keydvr;
      ccdcbs[unit] = KEYDCB;
      ccmode[unit] = RDMSK | DVMSK; /* Read only device */
      break;

    case CO : /* Unbuffered console output */
      if (mode != WRITE && mode != APPEND)
        return NULL;         /* Invalid mode */
      /* Address of our console output routine */
      ccdvr[unit] = codvr; 
      ccdcbs[unit] = DSPDCB;
      ccmode[unit] = WRMSK | DVMSK; /* Write only device */
      break;

    case PR :      /* Printer output */
      if (mode != WRITE && mode != APPEND)
        return NULL;     /* Invalid mode */
      /* Address of our printer output routine */
      ccdvr[unit] = prdvr;
      ccdcbs[unit] = PRTDCB;
      ccmode[unit] = WRMSK | DVMSK; /* Write only device */
      break;

    default :
    return NULL;  /* Invalid device */
  }

  ccdvc[unit] = dvc;
  return unit;
}



!FCLOSE
#include "STDIO/H"
#include "IO/H"

/* Close a file or a device, deallocate the FCB and buffer.
 * Returns the file descriptor upon success, or NULL
 * upon failure.
 */

fclose(unit)
  int unit;
{
  char *fcb;
  int ret;

  if (!ccuchk(unit))  /* Check unit number */
    return NULL;
  fcb = ccdcbs[unit];
  /* check to see if we are closing a file or a device */
  if (fcb != 0 && !(ccmode[unit] & DVMSK))
    ret = ccclose(fcb);
  /* Free the FCB and disk buffer.  free() will not free
   * memory that has not been allocated by alloc()!
   */
  free(fcb);
  free(ccbuff[unit]);
  ccbuff[unit] = ccdcbs[unit] = 0;
  ccmode[unit] = ccerr[unit] = 0;
  ccdvc[unit] = 0;

/*  ccbad will set 0x0800 in ccerr[unit] and return EOF if 
 *  ccdvr[unit] is not set to a valid driver and i/o is 
 *  attempted. This should never happen!
*/
  ccdvr[unit] = ccbad;

  return (ret >= 256) ? NULL : unit;
}



!CCDVCK
#include "STDIO/H"
#include "IO/H"

/* Check to see if a device is currently active.
 * If the device is active return TRUE otherwise
 * return FALSE to indicate the device is not active.
*/

ccdvck(dev)
int dev;
{
 int unit;
 for (unit = STDIN; unit < MAXFILES; ++unit)
    if (ccdvc[unit] == dev)
       return TRUE;
  return FALSE;
}


!CCBAD
#include "STDIO/H"
#include "IO/H"

/*  This function is called if for some reason an i/o
 *  function is called that doesn't have it's driver
 *  initialized. This should never happen since testing
 *  is done before i/o is attempted. It is intended for
 *  safety only.
 *  If the unit number is valid ccerr[unit] is set to
 *  0x0800 ( device not available error code)
 *  The return code is EOF.
 */

ccbad(unit)
int unit;
{
  if (ccuchk(unit))
    ccerr[unit] = 0x0800; /* device not available */
  return EOF;
}



!FERROR
#include "STDIO/H"
#include "IO/H"

/* Return the last error code on a given unit
 * Returns EOF if invalid unit number
 * The DOS error code is returned in the high byte
 */
ferror(unit)
  int unit;
  return ccuchk(unit) ? ccerr[unit] : EOF;



!CLEARE
#include "STDIO/H"
#include "IO/H"

/* Clear the error condition on a given unit
 * Returns 0 upon success
 * Returns EOF if invalid unit
 */

clearerr(unit)
  int unit;
  return ccuchk(unit) ? (ccerr[unit] = 0) : EOF;



!FGETS
#include "STDIO/H"
#include "IO/H"

/* Get a string (line) from any input unit */
fgets(s, unit)
  char *s;
  int  unit;
{
  char *cs;
  int  c;
  cs = s;
  while ((c = getc(unit)) != EOF && c != EOL)
    *cs++ = c;
  *cs = EOS;
  return (c == EOF && cs == s) ? EOF : s;
}



!FPUTS
#include "STDIO/H"
#include "IO/H"

/* Send a string to any output unit. */
fputs(s, unit)
  char *s;
  int  unit;
{
  char c;

  while (c = *s++)
    putc(c, unit);
  return feof(unit) ? EOF : s;
}



!GETC
#include "STDIO/H"
#include "IO/H"

/* Read a 8 bit ascii character
 * Translates CR to EOL
 * Returns EOF on error or End Of File
 *
 * C Version! The assembler version is faster
 * but more cryptic. We have included this for
 * those interested in customizing this routine.
 getc(unit)
  int unit;
{
  int c;

  if (!(ccuchk(unit) & ccmode[unit]) & RDMSK))
    return EOF;
  if (ccugf[unit])
    ccugf[unit] = FALSE;
    return ccugc[unit];
  }
  c = ccinput(unit);
  return ( c == CR ) EOL : c;
}
*/



/* Read a 8 bit ascii character
 * Translates CR to EOL
 * Returns EOF on error or end of file
 *
 * Assembler version
 */

getc(unit)
  int unit;
{
#asm
  POP  IX   ; return address
  POP  BC   ; unit number
  PUSH BC   ; restore 
  PUSH IX   ; the stack
  ;
  ; if (STDIN <= unit < MAXFILES)
EXT  CCUNCK
  CALL CCUNCK   ; 0 < unit < MAXFILES
  JR   C,CCGET1 ; bad unit, return EOF
  ;  if (RDMSK == (RDMSK & ccmode[unit]))
  LD   HL,CCMODE
  ADD  HL,BC
  ADD  HL,BC
  LD   A,(HL)   ; A = CCMODE[UNIT]
  AND  RDMSK
  CP   RDMSK
  JR   NZ,CCGET1 ; bad mode, return EOF
; if (ccugf[unit] == TRUE)
  LD   HL,CCUGF
  ADD  HL,BC  ; index
  ADD  HL,BC
  PUSH HL     ; save ptr to ccugf[unit]
  LD   E,(HL)
  INC  HL
  LD   D,(HL)
  LD   HL,TRUE
  OR   A
  SBC  HL,DE     ; TRUE ?
  POP  HL        ; restore ptr to ccugf[unit]
  JR   NZ,CCGET2 ; TRUE if zero
  ; ccugf[unit] = FALSE
  LD   DE,FALSE
  LD   (HL),E
  INC  HL
  LD   (HL),D
  ;
  ; return ccugc[unit]
  LD   HL,CCUGC
  ADD  HL,BC
  ADD  HL,BC
  LD   A,(HL)
  INC  HL
  LD   H,(HL)
  LD   L,A
  JR   CCGET3 ; return HL
  ;
CCGET2:
  PUSH BC    ; place index on the stack
EXT CCINPUT
  CALL CCINPUT
  POP  BC    ; restore index and stack
  OR   A
  EX   DE,HL       ; save character in DE
  LD   HL,EOF
  SBC  HL,DE
  JR   Z,CCGET1
  EX   DE,HL      ; restore character
  LD   A,L   ; HL has the character
  CP   CR
  JR   NZ,CCGET3
  LD   L,EOL   ; translate CR to EOL
  JR   CCGET3
CCGET1:
  LD  HL,EOF
CCGET3:  ; return hl
#endasm
}


!PUTC
#include "STDIO/H"
#include "IO/H"

/* Write a 8 bit ascii character
 * Translates EOL to CR
 * Returns the character or EOF upon an error
 *
putc(c, unit)
  char c;
  int unit;
{
  int ret;
  if(!(ccuchk(unit) & ccmode[unit] & WRMSK))
    return EOF;
  ret = ( c == EOL) ? CR : c;
  return (ccoutput(ret, unit) == EOF) ? EOF : c;
}
*/


/* Write a 8 bit ascii character
 * Translates EOL to CR
 * Returns the character or EOF upon an error
 *
 * Assembler version
 */
putc(c, unit)
  char c;
  int unit;
{
#asm
  POP  IX       ; return address
  POP  BC       ; unit
  POP  DE       ; c
  PUSH DE       ; restore
  PUSH BC       ; the
  PUSH IX       ; stack
  ;
EXT  CCUNCK
  CALL CCUNCK   ; STDIN <= unit < MAXFILES
  JR   C,CCPUT1 ; bad unit, return EOF
  ;  if (WRMSK == (WRMSK & ccmode[unit]))
  LD   HL,CCMODE
  ADD  HL,BC
  ADD  HL,BC
  LD   A,(HL)   ; A = CCMODE[UNIT]
  AND  WRMSK    ; check mode
  CP   WRMSK
  JR   NZ,CCPUT1 ; bad mode , return EOF
  ;
  PUSH DE       ; untranslated character on stack
  LD   L,E      ; bring character into HL
  LD   H,0
  ; character is in L,translate EOL to CR
  LD   A,EOL
  CP   L
  JR   NZ,CCPUT2
  LD   L,CR
CCPUT2:
  PUSH HL       ; translated character on stack
  PUSH BC       ; unit number on stack
EXT CCOUTPUT
  CALL CCOUTPUT
  POP  BC       ; restore unit number and stack
  POP  BC       ; restore translated character
  POP  DE       ; restore untranslated character
  OR   A        ; HL = EOF ?
  LD   BC,EOF
  SBC  HL,BC
  JR   Z,CCPUT1 ; we have EOF so return EOF
  EX   DE,HL   ; return character
  JR   CCPUT3
  ;
CCPUT1:
  LD  HL,EOF   ; invalid unit or mode
CCPUT3:        ; return hl
#endasm
}


!CCUNCK
#include "STDIO/H"
#include "IO/H"

/* This is the assembler version of ccuchk() used by
 * the assembler versions of getc(unit) and putc(c, unit).
 */
#asm
  ENTRY CCUNCK
CCUNCK:     ; STDIN <= unit < MAXFILES else return carry
  LD  A,H   ; unit out of range?
  OR  A     ; assume MAXFILE < 256
  JR  NZ,CCUNC1
  LD  A,L
  OR  A
  BIT 7,A   ; sign bit
  JR  NZ,CCUNC1
  CP  STDIN ; unit < STDIN ?
  JR  C,CCUNC1
  LD  A,MAXFILES-1
  SUB L     ; unit >= MAXFILES ?
  RET
CCUNC1:
  SCF
  RET
#endasm



!UNGETC
#include "STDIO/H"
#include "IO/H"

/* Un-read a character
 * Returns c with no error, or EOF upon an error
 */
ungetc(c, unit)
  char c, unit;
{
  if (!(ccuchk(unit) & ccmode[unit] & RDMSK))
    return EOF;
  if (!ccugf[unit]) {
    ccugc[unit] = c;
    ccugf[unit] = TRUE;
    return c;
  }
  return EOF;
}



!READ
#include "STDIO/H"
#include "IO/H"

/* Read bytes
 * Return codes:
 *   0  : Implies End Of File while trying to read the
 *        the first byte into the buffer.
 *   0  : Implies invalid unit or file mode.
 *  -1  : Implies an error occured while trying to read
 *        the first byte into the buffer.
 *   N  : N is the number of bytes actually read.
 * 
 * fd = file descriptor, buf = buffer address
 * n = number of bytes to read
 */
read(fd, buf, n)
  int fd;
  char *buf;
  int n;
{
  char *i;
  int  ret;

  if (!(ccuchk(fd) & ccmode[fd] & RDMSK) | feof(fd))
    return 0;
  for (i = 0; i < n; ++i) {
    if ((ret = ccinput(fd)) != EOF)
      buf[i] = ret;
    else {
      if (!i)
        return feof(fd) ? 0 : -1;
      break;
    }
  }
  return i;
}



!WRITE
#include "STDIO/H"
#include "IO/H"

/* Write bytes
 * Return codes:
 *   0  : Implies invalid unit or file mode.
 *  <n  : Implies an error occured while trying to write
 *        from the buffer.
 *   n  : n is the number of bytes requested to write.
 *
 * fd = file descriptor, buf = buffer pointer
 * n = number of bytes to write
 */
write(fd, buf, n)
  int fd;
  char *buf;
  int n;
{
  char *i;
  int ret;

  if (!(ccuchk(fd) & ccmode[fd] & WRMSK)| feof(fd))
    return 0;
  for (i = 0; i < n; ++i) {
    if (ccoutput(buf[i], fd) == EOF)
       break;
  }
  return i;
}



!CCINPUT
#include "STDIO/H"
#include "IO/H"

/* Device independent input Driver.
 * Not a general user function !
 * No testing is done at this level for valid unit
 * number or file mode.
 * EOF is returned if any kind of error occurs.
 * ccerr[unit] contains the actual error code
 *
 * unit = unit number returned from fopen()
 */

ccinput(unit)
int unit;
{
#asm
  POP  IX  ; return address
  POP  BC  ; unit number
  PUSH BC  ; restore
  PUSH IX  ; the  stack
  ;
  PUSH BC  ; save unit number
  LD   HL,CCINRET  ; return address
  PUSH HL          ; push the return address
  LD   HL,CCDVR    ; get driver address
  ADD  HL,BC       ; get ccdvr[unit]
  ADD  HL,BC
  LD   A,(HL)
  INC  HL
  LD   H,(HL)
  LD   L,A
  JP   (HL)        ; call our driver , pass unit on stack
  ;
CCINRET:  ; OK : H = 0,   NOT OK : H = DOS error code
  ;
  POP  BC  ; restore unit number, and stack
  LD   A,H
  OR   A
  JR   Z,CCINP1 ; L = character, H = 0
  ; ccerr[unit] = HL, where H = DOS error code, L = 0
  EX   DE,HL   ; save error code in DE
  LD   HL,CCERR
  ADD  HL,BC  ; index
  ADD  HL,BC
  LD   (HL),E ; Low byte is 0
  INC  HL
  LD   (HL),D
  ;
  LD   HL,EOF ; Return EOF
CCINP1:
#endasm
}


!CCOUTPUT
#include "STDIO/H"
#include "IO/H"

/* Device independent output Driver.
 * Not a general user function !
 * No testing is done at this level for valid unit
 * number or file mode.
 * EOF is returned if any kind of error occurs.
 * ccerr[unit] contains the actual error code
 *
 * c = character, unit = unit number returned from fopen()
 */

ccoutput(c, unit)
int c, unit;
{
#asm
  POP  IX  ; return address
  POP  BC  ; unit number
  POP  DE  ; c
  PUSH DE  ; restore
  PUSH BC  ; the
  PUSH IX  ; stack
  ;
  PUSH DE  ; character on stack
  PUSH BC  ; push unit number on stack
  LD   HL,CCOUT1   ; return address is CCOUT1
  PUSH HL          ; push return address on stack
  LD   HL,CCDVR    ; get driver address
  ADD  HL,BC       ; get ccdvr[unit]
  ADD  HL,BC
  LD   A,(HL)
  INC  HL
  LD   H,(HL)
  LD   L,A
  JP   (HL)        ; call our driver , pass unit on stack
  ;
CCOUT1:  ; OK -> H = 0,   NOT OK -> H = DOS error code
  ;
  POP  BC     ; restore unit number
  POP  DE     ; restore character, restore the stack!
  LD   A,H
  OR   A
  JR   Z,CCOUT2 ; L = character, H = 0
  ; ccerr[unit] = HL, where H = DOS error code, L = 0
  EX   DE,HL   ; save error code in DE
  LD   HL,CCERR
  ADD  HL,BC  ; index
  ADD  HL,BC
  LD   (HL),E ; Low byte is 0
  INC  HL
  LD   (HL),D
  ;
  LD   HL,EOF ; Return EOF
CCOUT2:
#endasm
}



!FEOF
#include "STDIO/H"
#include "IO/H"

/* Check for end of file
 * Returns 0 if not end of file
 * Returns EOF if End Of File or invalid unit
 */
feof(unit)
  int unit;
{
  if (!(ccuchk(unit) & ccmode[unit]))
    return EOF;
  if (ccerr[unit] == SYSEOF || ccerr[unit] == SYSNRF)
    return EOF;
  return 0;
}



!CCUCHK
#include "STDIO/H"
#include "IO/H"

/* Check for valid unit number
 * Return 0xffff on valid unit
 * Return 0 on invalid unit
 */
ccuchk(unit)
  int unit;
  return (STDIN <= unit && unit < MAXFILES) ? 0xffff : 0;


!CODVR
#include "STDIO/H"
#include "IO/H"

/* Lowest level Console output driver (*CO).
 * Not a general user function !
 * Returns character
 */

codvr(c, unit)
int c, unit;
{
#asm
     POP  IX       ; return address
     POP  BC       ; unit
     POP  HL       ; c 
     PUSH HL       ; restore
     PUSH BC       ; the
     PUSH IX       ; stack
     ;
     PUSH HL       ; save the character passed in L
     LD   HL,CCDCBS   ; get the DCB
     ADD  HL,BC
     ADD  HL,BC
     LD   E,(HL)
     INC  HL
     LD   D,(HL)
     POP  HL
     PUSH HL
     LD   A,L      ; pass character in A
     CALL ATPUT    ; dispaly a byte
     POP  HL       ; restore character
#endasm
}



!PRDVR
#include "STDIO/H"
#include "IO/H"

/* Lowest level printer driver (*PR).
 * Not a general user function !
 * Returns character.
 */

prdvr(c, unit)
int c, unit;
{
#asm
     POP  IX       ; return address
     POP  BC       ; unit
     POP  HL       ; c 
     PUSH HL       ; restore
     PUSH BC       ; the
     PUSH IX       ; stack
     ;
     PUSH HL       ; save the character passed in L
     LD   HL,CCDCBS
     ADD  HL,BC
     ADD  HL,BC
     LD   E,(HL)
     INC  HL
     LD   D,(HL)
     POP  HL
     PUSH HL
     LD   A,L      ; pass character in A
     CALL ATPUT    ; display a byte
     POP  HL       ; restore character
#endasm
}




!KEYDVR
#include "STDIO/H"
#include "IO/H"

/* Unbuffered keyboard input (*CI)
 * Not a general user function !
 * Ctrl-z or Break returns EOF
 */
keydvr(unit)
int unit;
{
#asm
     POP  IX  ; return address
     POP  BC  ; unit number
     PUSH BC  ; restore
     PUSH IX  ; the stack
CCKEY1:
     LD   HL,CCDCBS  ; get the DCB
     ADD  HL,BC
     ADD  HL,BC
     LD   E,(HL)
     INC  HL
     LD   D,(HL)
     CALL ATGET    ; get a character from the keyboard
     OR   A
     JR   Z,CCKEY1 ; wait for a char
     CP   01       ; break ?
     JR   Z,CCKEY2
     CP   1AH      ; ctrl-Z
     JR   Z,CCKEY2
     LD   L,A     ; return character
     LD   H,0H
     JR   CCKEY3
CCKEY2:
     LD   HL,EOF
CCKEY3:
     POP  DE
     POP  BC
#endasm
}

!BUFDVR
#include "STDIO/H"
#include "IO/H"

/* Buffered keyboard input with console echo (*BI)
 * Not a general user function !
 * Break returns EOF
 */
bufdvr(unit)
int unit;
{
#asm
     PUSH BC
     PUSH DE
     LD   HL,(CCBFIN) ; index into buffer
     LD   A,H
     OR   L
     JR   NZ,CCBUF1
     LD   HL,(CCINBF) ; buffer address
     LD   BC,0FF00H   ; get 255 chars.
     CALL ATBUF       ; system keyboard buffered input
     JR   NC,CCBUF1   ; carry = BREAK
     LD   HL,0000H    ; reset index
     LD   (CCBFIN),HL
     LD   HL,1C00FH    ; EOF error code
     JR   CCBUF3
CCBUF1:
     LD   HL,(CCBFIN)
     PUSH HL          ; save ccbfin
     LD   DE,(CCINBF) ; buffer pointer
     ADD  HL,DE
     LD   A,(HL)      ; get char from buffer
     POP  HL          ; restore ccbfin
     INC  HL          ; ccbfin +=1
     LD   (CCBFIN),HL
     CP   0DH         ; ENTER key was pressed
     JR   NZ,CCBUF2
     LD   HL,0000H    ; reset ccbfin
     LD   (CCBFIN),HL
CCBUF2:
     LD   H,0
     LD   L,A      ; return character in HL
CCBUF3:
     POP  DE
     POP  BC
#endasm
}


!CCCLOS
#include "STDIO/H"
#include "IO/H"

/* DOS call to close a file
 * Not a general user function !
 * Returns 0 if no errors
 * H = DOS error code, L = 0 in an error occurs.
 * 
 * fd = fcb
 */
ccclos(fd)
  int fd;
{
#asm
  POP  IX      ; return address
  POP  DE      ; fcb
  PUSH DE      ; restore
  PUSH IX      ; stack
  CALL ATCLOSE ; close file
  JR   Z,CCCL1 ; all is ok
  LD   H,A
  LD   L,0
  JR   CCCL2   ; finish up
CCCL1:
  LD   HL,0000H
CCCL2: 
#endasm
}



!CCOPEN
#include "STDIO/H"
#include "IO/H"

/* Call DOS to open a file
 * Not a general user function !
 * Returns 0 if no errors
 * H = DOS error code, L = 0 in an error occurs.
 *
 * lrl = Logical record length, fd = fcb
 * buf = buffer pointer
 */
ccopen(lrl, fd, buf)
  int lrl, fd, buf;
{
#asm
  POP  IX      ; return address
  POP  HL      ; buf
  POP  DE      ; fcb
  POP  BC      ; LRL
  PUSH BC      ; restore the stack
  PUSH DE
  PUSH HL
  PUSH IX
  LD   B,C     ; b = LRL
  LD   C,0
  CALL ATOPEN  ; open a file
  JR   Z,CCOP1 ; all is ok
  LD   H,A
  LD   L,0
  JR   CCOP2   ; finish up
CCOP1:
  LD   HL,0000H
CCOP2: 
#endasm
}



!CCINIT
#include "STDIO/H"
#include "IO/H"

/* DOS call to intialize a file
 * Not a general user function !
 * Returns 0 if no errors
 * H = DOS error code, L = 0 in an error occurs.
 *
 * lrl = Logical record length, fd = fcb
 * buf = buffer pointer
 */
ccinit(lrl, fd, buf)
  int lrl, fd, buf;
{
#asm
  POP  IX      ; return address
  POP  HL      ; buf
  POP  DE      ; fcb
  POP  BC      ; LRL
  PUSH BC      ; restore the stack
  PUSH DE
  PUSH HL
  PUSH IX
  LD   B,C     ; b = LRL
  LD   C,0
  CALL ATINIT  ; initialize a file
  JR   Z,CCIN1 ; all is ok
  LD   H,A
  LD   L,0
  JR   CCIN2   ; finish up
CCIN1:
  LD   HL,0000H
CCIN2: 
#endasm
}



!CCPEOF
#include "STDIO/H"
#include "IO/H"

/* DOS call to postion to end of file
 * Not a general user function !
 * Returns 0 if no errors
 * H = DOS error code, L = 0 in an error occurs.
 *
 * fd = fcb
 */
ccpeof(fd)
  int fd;
{
#asm
  POP  IX      ; return address
  POP  DE      ; fcb
  PUSH DE      ; restore
  PUSH IX      ; stack
  CALL ATPEOF  ; position to end of file
  JR   Z,CCPE1 ; all is ok
  LD   H,A
  LD   L,0
  JR   CCPE2   ; finish up
CCPE1:
  LD   HL,0000H
CCPE2: 
#endasm
}



!CCFSPE
#include "STDIO/H"
#include "IO/H"

/* DOS call to format a file spec
 * Not a general user function !
 * Returns 0 if no errors
 * H = DOS error code, L = 0 in an error occurs.
 *
 * fd = fcb, name = file name
 */
ccfspec(fd, name)
  int fd, name;
{
#asm
  POP  IX      ; return address
  POP  HL      ; name
  POP  DE      ; fd
  PUSH DE      ; restore
  PUSH HL      ; the
  PUSH IX      ; stack
  CALL ATFSPEC ; format a file spec
  JR   Z,CCFS1 ; all is ok
  LD   H,A
  LD   L,0
  JR   CCFS2   ; finish up
CCFS1:
  LD   HL,0000H
CCFS2: 
#endasm
}



!CCATGET
#include "STDIO/H"
#include "IO/H"

/* Lowest level driver to read a byte from a file.
 * Not a general user function !
 * L = character, H = 0 if no error occurs.
 * H = DOS error code, L = 0 if an error occurs.
 *
 * unit = unit number returned from fopen()
 */
ccatget(unit)
  int unit;
{
#asm
  POP  IX      ; return address
  POP  BC      ; n 
  PUSH BC      ; restore
  PUSH IX      ; stack
  LD   HL,CCDCBS   ; DE = ccdcbs[unit]
  ADD  HL,BC
  ADD  HL,BC
  LD   E,(HL)
  INC  HL
  LD   D,(HL)
  CALL ATGET       ; read character
  JR   Z,CCATG1    ; all is ok
  LD   H,A         ; DOS error code in H, L=0
  LD   L,0
  JR   CCATG2  ; finish up
CCATG1:
  LD   L,A         ; Character in L, H=0
  LD   H,0
CCATG2:  
#endasm
}


!CCATPUT
#include "STDIO/H"
#include "IO/H"

/* Lowest level driver to write a byte to a file
 * Not a general user function !
 * L = chacarter, H = 0 if no error occurs.
 * H = DOS error code, L = 0 if an error occurs.
 * c = character, unit = unit returned from fopen()
 */
ccatput(c, unit)
  int c, unit;
{
#asm
  POP  IX      ; return address
  POP  BC      ; unit
  POP  HL      ; c 
  PUSH HL      ; restore
  PUSH BC      ;  the
  PUSH IX      ;   stack
  ;
  LD   H,0     ; mask off high byyte
  LD   A,L     ; character in A
  PUSH HL      ; save character on stack
  LD   HL,CCDCBS   ; DE = ccdcbs[unit]
  ADD  HL,BC
  ADD  HL,BC
  LD   E,(HL)
  INC  HL
  LD   D,(HL)
  CALL ATPUT   ; write character
  JR   Z,CCATP1    ; all is ok
  POP  BC          ; restore stack
  LD   H,A         ; DOS error code in H, L=0
  LD   L,0
  JR   CCATP2   ; finish up
CCATP1:
  POP  HL       ; restore character
CCATP2: 
#endasm
}



!SYSMSG
#include "STDIO/H"
#include "IO/H"

/* DOS call to put a message to the console, used typically
 * when some error prevents use of STERR
 * message must be terminated by an ASCII NULL (0x00)
 *
 * ptr = pointer to string
 */
sysmsg(ptr)
char *ptr;
{
#asm
  POP  IX      ; return address
  POP  HL      ; pointer to string
  PUSH HL      ; restore
  PUSH IX      ; stack
SYSM1:
  LD   A,(HL)
  CP   00H     ; extra translation can go here
  JR   Z,SYSM2
  PUSH HL
  CALL ATDISP  ; display byte
  POP  HL
  INC  HL
  JR   SYSM1
SYSM2:
#endasm
}



!IOG
#include "STDIO/H"
#include "IO/H"
/* Compile this module without the "-g" option
 * This module defines the i/o globals
 */
