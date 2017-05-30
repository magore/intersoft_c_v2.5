/*
 * The Intersoft C standard library #1
 * for Intersoft C v2.5.
 *
 * Copyright (c) 1981 by Intersoft.
 *
 * by Richard B. McMurray
 *
 * This library contains functions which require I/O functions.
 *
 */



!GETDEC
#include <stdio/h>

/*
 * Get a decimal number from the standard input unit.
 * NOT FROM K & R.
 */
getdec()
  return getnum(STDIN);



!GETNUM
#include <stdio/h>

/*
 * Get a decimal number from a unit.
 * This function skips the leading whitespace, then attempts to
 * convert the first non-blank string to a decimal integer.
 * NOT FROM K & R.
 */
getnum(unit)
  int unit;
{
  char str[10], *ptr;
  int  i;

  ptr = str;
  while (isspace(*ptr = getc(unit)));
  for (i = 2; (i < 7) & !isspace(*++ptr = getc(unit)); ++i);
  *ptr = EOS;
  return atoi(str);
}



!PUTDEC
#include <stdio/h>

/*
 * Write a decimal integer to the standard output unit.
 * NOT FROM K & R.
 */
putdec(n)
  int n;
  return putnum(n, STDOUT);



!PUTNUM
#include <stdio/h>

/*
 * Write a decimal number to a unit.
 * NOT FROM K & R.
 */
putnum(n, unit)
  int n, unit;
{
  char s[10];

  itoa(n, s);
  return (fputs(s, unit) != EOF);
}



!SYSTEM
#include <stdio/h>

/*
 * Run a program from a C program.
 * REF. PAGE 157 OF K & R.
 * IMPLEMENTED AS AN ERROR TRAP.
 * CREATED FOR V2.5.
 */
system(s)
  char *s;
  error("Attempted system call to: ", s);



!ERROR
#include <stdio/h>

/*
 * Send a fatal error message to STDERR and exit.
 * The error message has two parts.
 * NOT IN K & R.
 */
error(s1, s2)
  char *s1, *s2;
{
  fputs(s1, STDERR);
  fputs(s2, STDERR);
  putc(EOL, STDERR);
  exit(1);
}



!GETS
#include <stdio/h>

/* Return a string (line) from STDIN. */
gets(s)
  char *s;
  return fgets(s, STDIN);



!PUTS
#include <stdio/h>

/* Send a string to STDOUT. */
puts(s)
  char *s;
  return fputs(s, STDOUT);



!GETCHA
#include <stdio/h>

/* Get the next character from STDIN. */
getchar()
  return getc(STDIN);



!PUTCHA
#include <stdio/h>

/* Send the next character to STDOUT. */
putchar(c)
  char c;
  return putc(c, STDOUT);



!CALLOC
#include <stdio/h>

/*
 * Allocate storage for a number of objects.
 * REF. PAGE 157 OF K & R.
 * CREATED FOR V2.5
 */
calloc(n, size)
  int  n, size;
  return alloc(n * size);



!CFREE
#include <stdio/h>

/*
 * Free space obtained by calloc()
 * REF. PAGE 157 OF K & R.
 * CREATED FOR V2.5
 */
cfree(p)
  char *p;
  return free(p);
