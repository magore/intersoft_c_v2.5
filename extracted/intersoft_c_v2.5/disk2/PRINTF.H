/*
 * Definitions for printf, fprintf() and sprintf() for
 * Intersoft C Version 2.5
 *
 * Copyright (c) 1982 by Intersoft
 *
 * By: R. McMurray
 *
 * Based on printf() by Mike Gore.
 */

#define STRING 0
#define NUMBER 1
#define ZERO '0'
#define BLANK ' '
#define LEFT 0
#define RIGHT 1
#define PLUS 0
#define MINUS 1
#define MAXINT 32767
#define BUFSIZE 16
#define WIDTH 132

int ccnarg;   /* Hold the number of arguments to printf */
int ccarg;    /* Points to the first argument */
char *ccbptr; /* Typically, beginning of string pointer  */
char *cceptr; /* Typically, end of string pointer */
int  ccunout; /* The output unit number, 0 for sprintf() */
char *ccsadr; /* The string address for sprintf() */
