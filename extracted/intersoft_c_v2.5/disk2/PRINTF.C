/*
 * printf(), fprintf(), sprintf() and ccputf() - formatted output
 *
 * Copyright (c) 1982 by Intersoft
 *
 * By: R. McMurray
 *
 * Based on printf() by Mike Gore.
 */

!PRINTF
#include <stdio/h>
#include "printf/h"

printf() {
/*
 *   All of the arguments that are passed to printf
 * are pushed on the stack. The A register contains
 * the argument count. CCARG is the address of the
 * first argument on the stack.  CCARG is the address
 * of the format string.
 */
#asm
 LD  L,A
 LD  H,0
 LD  (CCNARG),HL ; Save # of args. in CCNARG.
 ADD HL,HL       ; Index by words not bytes.
 ADD HL,SP
 LD  (CCARG),HL  ; Save address of pointer to
                 ; first argument.
#endasm
  ccunout = STDOUT;  /* Set the output unit */
  ccsadr = 0;
  ccputf();   /* Common formatted output routine */
} /* end printf */



!FPRINT
#include <stdio/h>
#include "printf/h"


fprintf() {
/*
 *   All of the arguments that are passed to fprintf
 * are pushed on the stack. The A register contains
 * the argument count. CCARG is the address of the
 * first argument on the stack.
 * CCARG is the address of the file pointer.
 */
#asm
 LD L,A
 LD H,0
 DEC  HL  ; Subtract file unit arg.
 LD (CCNARG),HL  ; Save # of args in CCNARG.
 ADD  HL,HL   ; Index by words not bytes.
 ADD  HL,SP
 LD (CCARG),HL ; Save address of pointer to
    ; first argument.
#endasm
  ccunout = *(ccarg + 2);  /* File pointer is the first arg. */
  ccsadr = 0;
  ccputf();   /* Common formatted output routine */
} /* end fprintf */



!SPRINT
#include <stdio/h>
#include "printf/h"



sprintf() {
/*
 *   All of the arguments that are passed to sprintf
 * are pushed on the stack. The A register contains
 * the argument count. CCARG is the address of the
 * first argument on the stack after the string address.
 * CCARG is the address of the format string.
 */
#asm
 LD L,A
 LD H,0
 DEC  HL  ; Subtract string ptr arg.
 LD (CCNARG),HL  ; Save # of args. in CCNARG.
 ADD  HL,HL   ; Index by words not bytes.
 ADD  HL,SP
 LD (CCARG),HL ; Save address of pointer to
    ; first argument.
#endasm
  ccsadr = *(ccarg + 2);
  ccunout = 0;
  ccputf();   /* Common formatted output routine */
  ccpch(0);   /* Terminate the string */
} /* end sprintf */



!CCPUTF
#include <stdio/h>
#include "printf/h"

ccputf() {
  int  sign;  /* Sign flag for %d */
  int  mask;  /* Mask for octal & hex numbers */
  int  nbits;   /* The number of bits in mask */
  int  num;   /* Unconverted # for %u %d %o %h */
  int  big;   /* Assigned the value MAXINT */
  int  carry;   /* Used in %u for carry */
  int  temp;  /* Used in %u */
  int  flag;  /* Number / string flag */
  int  flr;   /* Left or right justify flag */
  int  fmin;  /* Minimum field width */
  int  fmax;  /* Maximum field width */
  int  alen;  /* Length    */
  int  n;   /* The length of a string */
  int  npad;  /* Number of pad chr chrs */
  char fcode;   /* Holds current fmt chr */
  char fill;  /* Fill character  */
  int  counter;   /* Current argument number */
  char *arg;  /* Pointer to current argument */
  char *fmt;  /* Pointer to the fmt control arg. */
  char buf[BUFSIZE]; /* Buffer for %c and numbers */
    
  counter = 1;
  fmt = ccargs(counter);
  if (!fmt)   /* Check for missing control arg. */
    return;

  for(;;) {
    ccbptr = fmt;
    cceptr = 0xffff;

    /* Output all non-format characters */
    while ((fcode = *ccbptr) != '%') {
      if (fcode == EOS)
        return;
      ccpch(*ccbptr++);
    }
    fmt = ++ccbptr;  /* Move to next format character */

    flr = RIGHT; /* Right justify by default */
    if (*fmt == '-') {
      flr = LEFT;  /* Left justify if specified */
      ++fmt;
    }

    fill = BLANK;  /* Fill with blanks by default */
    if (*fmt == '0') {
      fill = ZERO; /* Fill with zeroes if specified */
      ++fmt;
    }

    fmin = 0;  /* Get min. field width, default is 0 */
    while (isdigit(*fmt))
      fmin = fmin * 10 + *fmt++ - '0';
    if (*fmt == '.') 
      ++fmt;

    fmax = 0;  /* Get max. field width, default is WIDTH */
    while (isdigit(*fmt))
      fmax = fmax * 10 + *fmt++ - '0';
    if (fmax == 0 || fmax > WIDTH)
      fmax = WIDTH;

    arg = ccargs(++counter); /* Get arg. to print */

    /* Select the appropriate format */
    switch (fcode = toupper(*fmt++)) {

      /* Single character */
      case 'C':
 flag = NUMBER;
 ccbptr = cceptr = buf + BUFSIZE;
 *--ccbptr = arg & 0xFF;
 break;

      /* String */
      case 'S':
 flag = STRING;
 if (!arg)
   arg = "";
 ccbptr = cceptr = arg;
 n = 0;
 while (*cceptr != EOS)
   ++cceptr;
 break;

      /* Octal or Hex number */
      case 'O':
      case 'X':
 flag = NUMBER;
 num = arg;
 if (fcode == 'O') {
   mask = 0x7;
   nbits = 3;
 } else {
   mask = 0xf;
   nbits = 4;
 }
 ccbptr = cceptr = buf + BUFSIZE;
 do {

   *--ccbptr = (num & mask) + ((num & mask) < 10 ?
        060 : 0127);
 } while ((num >>= nbits) > 0);
 break;

      /* Signed Decimal number */
      case 'D' :
 flag = NUMBER;
 num = arg;
 ccbptr = cceptr = buf + BUFSIZE;
 sign = (num < 0);
 do {
   temp = num % 10;
   if (temp < 0)
     temp = -temp;
   *--ccbptr = '0' + temp;
   num /= 10;
 } while (num);
 if (sign)
   *--ccbptr = '-';
 break;

      /* Unsigned Decimal number */
      case 'U' :
 flag = NUMBER;
 num = arg;
 ccbptr = cceptr = buf + BUFSIZE;
 if (num >= 0)
     do {
     *--ccbptr = '0' + (num % 10);
   } while ((num /= 10) > 0);
 else {
   big = MAXINT;
   num = (num & big);
   carry = 1;
   do {
     temp = (num % 10) + (big % 10) + carry;
     carry = temp / 10;
     temp %= 10;
     *--ccbptr = '0' + temp;
     num /= 10;
   } while ((big /= 10) > 0);
 }
 break;

      /* Unrecognized format */
      default :
 flag = STRING;
 cceptr = fmt;
 fmax = WIDTH;
 fmin = 0;
 break;
    } /* end switch */

    /* Enforce the maximum field width. */
    if (ccbptr + fmax < cceptr)
      cceptr = ccbptr + fmax;

    /* Propogate minus sign to the front of a number padded */
    /* with preceeding zeroes. */
    if (*ccbptr == '-' && fill == ZERO && flag == NUMBER)
      ccpch(*ccbptr++);

    alen = cceptr - ccbptr;  /* Compute the min. # of */
    if (alen > fmin)  /* characters to print */
      fmin = alen;
    npad = fmin - alen;

    if (flr == RIGHT)   /* Right justify */
      while (npad-- > 0)
        ccpch(fill);

    while (cceptr > ccbptr)  /* Output the item */
      ccpch(*ccbptr++);

    if (flr == LEFT)  /* Left justify */
      while (npad-- > 0)
        ccpch(fill);
  } /* end for(;;) */
} /* end ccputf */



/* Returns argument number z */
ccargs(z)
  int z;
  return (!ccnarg || z > ccnarg) ? 0 : *(ccarg - z - z + 2);



/* Output a character */
ccpch(c)
  int  c;
{
  if (ccunout)
    putc(c, ccunout);
  else
    *ccsadr++ = c;
}


!PRINTG
/* Globals for printf(), fprintf(), sprintf() and ccputf() */

#include <stdio/h>
#include "printf/h"
