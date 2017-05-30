/*
 * scanf(), fscanf(), sscanf() and ccgetf() - Formatted input.
 *
 * Copyright (c) 1982 by Intersoft.
 *
 * Written by: Richard B. McMurray
 * Date:       July 31, 1982
 *
 * DOES NOT SUPPORT FLOAT ("F" FORMAT).
 * NOT RE-ENTRANT (due to globals)
 */



!SCANF
#include <stdio/h>
#include "scanf/h"

scanf() {
  /*
   * This assembly patch sets up the address of the control
   * string (ie. the first parameter.) into "ccscont", the
   * address of the first of the remaining parameters into
   * "ccccsargs" and the number of arguments less one into
   * "ccsccsnargs".
   */
#asm
 LD   L,A             ; Set up "ccsnargs".
 LD   H,0
 LD   (CCSNARGS),HL
 ADD  HL,HL
 ADD  HL,SP           ; Set up "ccsargs".
 PUSH HL
 DEC  HL
 DEC  HL
 LD   (CCSARGS),HL
 POP  HL              ; Set up "ccscont".
 LD   E,(HL)
 INC  HL
 LD   D,(HL)
 EX   DE,HL
 LD   (CCSCONT),HL
#endasm

  ccsunsc = STDIN; /* Set the input file unit */
  ccscptr = 0;     /* Set the string ptr for sscanf() */
  ccgetf();        /* Common formatted input function */
} /* end scanf() */



!FSCANF
#include <stdio/h>
#include "scanf/h"

fscanf() {
  /*
   * This assembly patch sets up the address of the control
   * string (ie. the first parameter.) into "ccscont", the
   * address of the first of the remaining parameters into
   * "ccsargs" and the number of arguments less one into
   * "ccsnargs".
   */
#asm
 LD   L,A             ; Set up "ccsnargs".
 LD   H,0
 DEC  HL              ; Subtract file unit arg.
 LD   (CCSNARGS),HL
 ADD  HL,HL
 ADD  HL,SP           ; Set up "ccsargs".
 PUSH HL
 DEC  HL
 DEC  HL
 LD   (CCSARGS),HL
 POP  HL              ; Set up "ccscont".
 LD   E,(HL)
 INC  HL
 LD   D,(HL)
 EX   DE,HL
 LD   (CCSCONT),HL
#endasm

  ccscptr = 0;
  ccsunsc = *(ccsargs+2); /* Set the file unit number */
  ccgetf();               /* Common formatted input function */
} /* end fscanf() */



!SSCANF
#include <stdio/h>
#include "scanf/h"

sscanf() {
  /*
   * This assembly patch sets up the address of the control
   * string (ie. the first parameter.) into "ccscont", the
   * address of the first of the remaining parameters into
   * "ccsargs" and the number of arguments less one into
   * "ccsnargs".
   */
#asm
 LD   L,A             ; Set up "ccsnargs".
 LD   H,0
 DEC  HL              ; Subtract string ptr arg.
 LD   (CCSNARGS),HL
 ADD  HL,HL
 ADD  HL,SP           ; Set up "ccsargs".
 PUSH HL
 DEC  HL
 DEC  HL
 LD   (CCSARGS),HL
 POP  HL              ; Set up "ccscont".
 LD   E,(HL)
 INC  HL
 LD   D,(HL)
 EX   DE,HL
 LD   (CCSCONT),HL
#endasm
  ccsunsc = 0;
  ccscptr = *(ccsargs+2); /* Set the string pointer */
  ccgetf();               /* Common formatted input routine */
} /* end sscanf() */



!CCGETF
#include <stdio/h>
#include "scanf/h"

ccgetf() {
  char c,
       cresult,
       *cptr;
  int  suppress,
       maxlen;

  /* Process the control string. */
  ccsnargs -= 2;
  ccsnmtch = 0;
  while (ccfmt()) {

    /* Handle all non-whitespace characters. */
    if (ccfmt() == ESCCHAR) {

      /* See if assignment is to be suppressed. */
      ccadv();
      if (ccsuppress = (ccfmt() == SUPCHAR))
        ccadv();

      /* Set (or get) the maximum length specifier. */
      if (isdigit(ccfmt()))
        for (maxlen = 0; isdigit(ccfmt()); ccadv())
          maxlen = maxlen * 10 + ccfmt() - '0';
      else
        maxlen = 65535;

      /* Process the conversion character. */
      if (!(c = ccfmt()))
        continue;
      switch (toupper(c)) {

        /* Get a decimal integer. */
        case 'D' :
          maxlen = cciasgn(isdigit, 10, maxlen);
          break;

        /* Get an octal integer. */
        case 'O' :
          maxlen = cciasgn(isoctal, 8, maxlen);
          break;

        /* Get a hexidecimal integer. */
        case 'X' :
          maxlen = cciasgn(ishex, 16, maxlen);
          break;

        /* Get a short integer. */
        case 'H' :
          maxlen = cccasgn(isdigit, 10, maxlen);
          break;

        /* Get a character. */
        case 'C' :
          if (ccget(&cresult))
            return EOF;
          if (!ccsuppress) {
            if (ccsnmtch > ccsnargs)
              return ccsnmtch;
            cptr = ccsargs[ccsnargs - ccsnmtch++];
            *cptr = cresult;
          }
          maxlen = 0;
          break;

        /* Get a string. */
        case 'S' :
          ccsskpwhite();
          if (!ccsuppress)
            if (ccsnmtch > ccsnargs)
              return ccsnmtch;
            else
              cptr = ccsargs[ccsnargs - ccsnmtch++];
          else
            cptr = 0;
          for(;;) {
            if (ccget(&c))
              return EOF;
            if (isspace(c)) {
              ccsunget(c);
              break;
            }
            if (!maxlen--) {
              ccsunget(c);
              break;
            }
            if (cptr)
              *cptr++ = c;
          }
          if (cptr)
            *cptr = 0;
          maxlen = 0;
          break;

        /* Ignore illegal format specifications. */
        default :
      }
      ccadv();
      if (maxlen == EOF)
        return EOF;
      if (maxlen == 1)
        return ccsnmtch;
    } else {

      /*
       * Process a string of ordinary characters which are
       * expected to match the input exactly.  (not whitespace
       * or ESCCHAR).
       */
      for (cptr = ccscont; ccfmt() != ESCCHAR && ccfmt();
        ccadv());
      while (cptr != ccscont) {
        if (isspace(*cptr)) {
          ++cptr;
          continue;
        }
        if (!ccgetnw(&c)) {
          if (*cptr++ != c)
            return ccsnmtch;
        } else
          return EOF;
      }
    }
  }
  return ccsnmtch;
} /* end ccgetf() */



/*
 * Return the next non-whitespace character from
 * the format string. */
ccadv() {
  ++ccscont;
}



/*
 * Return the curreent (or next) non-whitespace
 * character from the format string.
 */
ccfmt() {
  while (isspace(*ccscont))
    ++ccscont;
  return *ccscont;
}




/* Get a character.  A non-zero value is returned upon EOF. */
ccget(c)
  char *c;
{
  int  rval;

  if (!ccscptr) {
    *c = rval = getc(ccsunsc);
    rval = (rval == EOF);
  } else {
    *c = *ccscptr++;
    rval = !*c;
  }
  return rval;
}



/*
 * Get the next non-whitespace character.  A non-zero
 * value is returned upon EOF.
 */
ccgetnw(c)
  char *c;
{
  int  rval;

  while (rval = ccget(c), isspace(*c));
  return rval;
}



/* Push a character back. */
ccsunget(c)
  char c;
{
  if (ccscptr)
    --ccscptr;
  else
    ungetc(c, ccsunsc);
}



/*
 * Get a (possibly signed) numerical integer from the input
 * stream and put it in the appropriate memory address.
 * EOF is returned upon end-of-file, 0 means success, 
 * 1 means the number wasn't the next thing in the input
 * stream or that the value couldn't be assigned because
 * ccgetf() was passed too few arguments.
 */
cciasgn(func, base, maxlen)
  int  *func, base, maxlen;
{
  int result, rcode;

  if (ccsnmtch > ccsnargs)
    return 1;
  if ((rcode = ccgnum(func, base, maxlen, &result)))
    return rcode;
  if (!ccsuppress)
    *ccsargs[ccsnargs - ccsnmtch++] = result;
  return 0;
}



/*
 * Get a (possibly signed) numerical character from the input
 * stream and put it in the appropriate memory address.
 * EOF is returned upon end-of-file, 0 means success, 
 * 1 means the number wasn't the next thing in the input
 * stream or that the value couldn't be assigned because
 * ccgetf() was passed too few arguments.
 */
cccasgn(func, base, maxlen)
  int  *func, base, maxlen;
{
  int  result, rcode;
  char *cptr;

  if (ccsnmtch > ccsnargs)
    return 1;
  if ((rcode = ccgnum(func, base, maxlen, &result)))
    return rcode;
  if (!ccsuppress) {
    cptr = ccsargs[ccsnargs - ccsnmtch++];
    *cptr = result;
  }
  return 0;
}



/*
 * Get a number from the input stream in octal, decimal or hex.
 * The number has a maximum length "maxlen".  "func" is the
 * address of the function which checks to see if a character
 * is a digit of the appropriate base.  The value is returned
 * in "*ptr".  EOF is returned if end-of-file is encountered,
 * 1 is returned if the next item in the input stream is not
 * the number we were after, 0 is returned upon success.
 */
ccgnum(func, base, maxlen, ptr)
  int  *func, base, maxlen, *ptr;
{
  char c;
  int  iresult, sign;

  *ptr = 0;
  if (!maxlen)
    return 0;
  sign = 1;
  if (ccgetnw(&c))
    return EOF;
  if (c == '-') {    /* Remove the sign (+ or -). */
    if (!--maxlen)
      return 0;
    sign = -1;
    if (ccget(&c))
      return EOF;
  } else 
    if (c == '+') {
      if (ccget(&c))
        return EOF;
      if (!--maxlen)
        return 0;
    }
  if (base == 16 && c == '0') { /* Remove leading "0x" from */
    if (!--maxlen)                   /* hex numbers. */
      return 0;
    if (ccget(&c))
      return EOF;
    if (toupper(c) == 'X') {
      if (!--maxlen)
        return 0;
      if (ccget(&c))
        return EOF;
    }
  }
  if (!func(c)) { /* Make sure that we have a number! */
    ccsunget(c);
    return 1;
  }

  /* Get the number from the input stream. */
  for (iresult = 0; maxlen-- && func(c);) {
    c = toupper(c);
    iresult = iresult * base + c - ((c >= 'A') ? '7' : '0');
    if (ccget(&c))
      return EOF;
  }

  /*
   * Give back the last character, set the result,
   * return SUCCESS.
   */
  if (!func(c))
    ccsunget(c);
  *ptr = iresult * sign;
  return 0;
} /* end ccgnum() */



/* Skip whitespace characters in the input stream. */
ccsskpwhite() {
  char c;

  for (;;) {
    if (ccget(&c))
      return;
    if (!isspace(c))
      break;
  }
  ccsunget(c);
}


!SCANG
/* Globals for scanf(), fscanf(), sscanf() and ccgetf() */

#include <stdio/h>
#include "scanf/h"
