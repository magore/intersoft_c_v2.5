/*
 * The Intersoft C standard library #3
 * for Intersoft C v2.5.
 *
 * Copyright (c) 1981 by Intersoft.
 *
 * by Richard B. McMurray
 *
 * This library contains functions which require neither
 * I/O functions nor operating system functions.
 */



!STRCAT
/*           
 * Concatenates string "t" to the end of string "s".
 * REF. PAGE 44 OF K & R.
 * Note that we make use of an extension of our own,
 * findeos().
 */
strcat(s, t)
  char *s, *t;
  strcpy(findeos(s), t);



!STRLEN
/*           
 * Return the length of the string.
 * REF. PAGE 98-99 OF K & R.
 * Note that we make use of an extension of our own,
 * findeos().
 */
strlen(s)
  char *s;
  return findeos(s) - s;



!FINDEO
/*
 * Returns the address of the end of the string.
 * NOT IN K & R.
 */
findeos(s)
  char *s;
{
  while (*s++);
  return s-1;
}



!STRCPY
/*
 * Copies string "t" to string "s".
 * REF. PAGE 101 OF K & R.
 * Changed from the v2.0 release to conform with K & R;
 * old calling sequence was strcpy(t, s).
 */
strcpy(s, t)
  char *s, *t;
  while (*s++ = *t++);



!STRCMP
/*
 * Compare two strings character by character.  Returns the
 * difference between the first two non-equal characters.
 * REF. PAGE 102 OF K & R.
 * Zero is returned if the strings are equal.
 * ie. strcmp(s, t) == 0 iff s and t are identical.
 *                  <  0 iff s[i] < t[i] for the first i
 *                       such that s[i] != t[i].
 *                  >  0 otherwise.
 */
strcmp(s, t)
  char *s, *t;
{
  for (; *s == *t && *s; ++s, ++t);
  return (*s - *t);
}



!ISHEX
/*
 * Returns TRUE if the argument is a hexidecimal digit,
 * otherwise returns FALSE.
 * NOT IN K & R.
 */
ishex(c)
  int c;
  return bound(toupper(c), 'A', 'F') || isdigit(c);



!TOUPPE
/*
 * Returns its argument in uppercase.
 * REF. PAGE 156 OF K & R.
 */
toupper(c)
  int c;
  return islower(c) ? c & 0xDF : c;



!TOLOWE
/*
 * Returns its argument in lowercase.
 * REF. PAGE 156 OF K & R.
 */
tolower(c)
  int c;
  return isupper(c) ? c | 0x20 : c;



!ISALPH
/*
 * Returns TRUE if the argument is alphabetic,
 * otherwise returns FALSE.
 * REF. PAGE 156 OF K & R.
 */
isalpha(c)
  int c;
  return isupper(c) || islower(c);



!ISUPPE
/*
 * Returns TRUE if the argument is uppercase,
 * otherwise returns FALSE.
 * REF. PAGE 156 OF K & R.
 */
isupper(c)
  int c;
  return bound(c, 'A', 'Z');



!ISLOWE
/*
 * Returns TRUE if the argument is lowercase,
 * otherwise returns FALSE.
 * REF. PAGE 156 OF K & R.
 */
islower(c)
  int c;
  return bound(c, 'a', 'z');



!ISDIGI
/*
 * Returns TRUE if the argument is a decimal digit,
 * otherwise returns FALSE.
 * REF. PAGE 156 OF K & R.
 */
isdigit(c)
  int c;
  return bound(c, '0', '9');



!ISOCTA
/*
 * Returns TRUE if the argument is an octal digit,
 * otherwise returns FALSE.
 * NOT IN K & R.
 */
isoctal(c)
  int c;
  return bound(c, '0', '7');



!ISSPAC
#include <stdio/h>

/*
 * Returns TRUE if the argument is a whitespace character,
 * otherwise returns FALSE.
 * REF. PAGE 156 OF K & R.
 * CHANGED FOR V2.5 TO CONFORM WITH K & R.  FORMFEED
 * IS NO LONGER CONSIDERED A WHITESPACE CHARACTER.
 */
isspace(c)
  int c;
  return c == ' ' || c == 9 || c == EOL;



!BOUND
/*
 * Checks to see if a value is within a range.
 * Returns TRUE if the value is within the bounds given
 * (inclusively), otherwise returns FALSE.
 * NOT IN K & R.
 */
bound(v, l, u)
  int v, l, u;
  return v >= l && v <= u;



!ABS
/*
 * Returns the absolute value of its argument.
 * NOT FROM K & R.
 */
abs(n)
  int n;
  return (n < 0) ? -n : n;



!MIN
/*
 * Return the minimum of two signed integers.
 * NOT FROM K & R.
 */
min(a, b)
  int  a, b;
  return (a < b) ? a : b;



!MAX
/*          
 * Return the maximum of two signed integers.
 * NOT FROM K & R.
 */
max(a, b)
  int  a, b;
  return (a > b) ? a : b;



!UMIN
/*
 * Return the minimum of two unsigned integers.
 * NOT FROM K & R.
 * CREATED FOR INTERSOFT C V2.5
 */
umin(a, b)
  char *a, *b;
  return (a < b) ? a : b;



!UMAX
/*          
 * Return the maximum of two signed integers.
 * NOT FROM K & R.
 * CREATED FOR INTERSOFT C V2.5
 */
umax(a, b)
  char *a, *b;
  return (a > b) ? a : b;



!COPYBY
/* Copy a number of bytes. */
copybyte(from, to, number)
  char *from, *to;
  int number;
  while (number--)
    *to++ = *from++;



!INITBY
/* Initialize a number of bytes. */
initbyte(to, val, number)
  char *to, val;
  int number;
  while (number--)
    *to++ = val;



!INITWO
/* Initialize a number of words. */
initword(to, val, number)
  int *to, val, number;
  while (number--)
    *to++ = val;
