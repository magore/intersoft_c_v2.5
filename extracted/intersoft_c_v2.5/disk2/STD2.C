/*
 * The Intersoft C standard library #2
 * for Intersoft C v2.5.
 *
 * Copyright (c) 1981 by Intersoft.
 *
 * by Richard B. McMurray
 *
 * This library contains functions which require operating
 * system functions but not I/O functions.
 */



!ATOI
#include <stdio/h>

/*
 * Convert an ascii string to a number.
 * REF. PAGE 58 OF K & R.  We have rewritten it in a
 * more efficient form for the v2.5 compiler.
 *
 * Changes from v2.0:
 * 1) Leading whitespace is skipped.
 * 2) Plus signs as well as minus signs are recognized.
 */
atoi(s)
  char *s;
{
  int n, sign;

  while (isspace(*s))
    ++s;
  sign = TRUE;
  if (*s == '+' || *s == '-')
    sign = (*s++ == '+') ? TRUE : FALSE;
  for (n = 0; isdigit(*s); ++s)
    n = n * 10 + *s - '0'; 
  return sign ? n : -n;
}



!ITOA
#include <stdio/h>

/*             
 * Convert a number to an ascii decimal string.
 * REF. PAGE 60 OF K & R.
 * Ours is significantly different, using recursion
 * to generate the string in the correct order initially.
 * We have also removed a subtle bug from the K & R
 * version.  With 16-bit integers -(-32768) == -32768
 * so the statement "n = -n" in the K & R version will
 * not work if n is -32768.
 */
itoa(n, s)
  char *s;
  int  n;
{
  char *temp;

  if (n < 0) {         /* Handle negative numbers. */
    if (n == -32768) { /* Special case. */
      strcpy(s, "-32768");
      return;
    }
    n = -n;
    *s++ = '-';
  }
  *ccitoa(n, s) = EOS; /* Convert to ascii and add EOS. */
}



/*
 * Recursively convert a positive integer to ascii.
 * Called by itoa().  NOT FROM K & R.
 */
ccitoa(n, s)
  int n;
  char *s;
{
  int i;

  if (i = n / 10)     /* Recurse to the most sig. digit. */
    s = ccitoa(i, s);
  *s = n % 10 + '0';  /* Convert the digit to ascii. */
  return s+1;         /*  Return next available address. */
}



!STRING
#include <stdio/h>

/*
 * Allocate and initialize a vector of bytes.
 * NOT IN K & R.
 */
string(n, c)
  int  n;
  char c;
{
  char *s;

  if (!(s = alloc(n)))
    return NULL;
  initbyte(s, c, n);
  return s;
}
