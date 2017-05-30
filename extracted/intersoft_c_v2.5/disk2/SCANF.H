/*
 * Constants and variables for formatted input for
 * Intersoft C Version 2.5
 *
 * Copyright (c) 1982 by Intersoft.
 *
 * Written by: Richard B. McMurray
 * Date:       July 31, 1982
 *
 * NOT RE-ENTRANT (due to globals)
 */

#define SP      0x20
#define TAB     0x09

/* The escape character to define a format specification. */
#define ESCCHAR '%'

/* The assignment suppression character. */
#define SUPCHAR '*'

char *ccscont;   /* Address of current control */
                 /* string character */
int  *ccsargs,   /* Address of first arg. */ 
     ccsnargs,   /* Number of arguments */
     ccsnmtch,   /* Number of format items matched */
     ccsuppress, /* Assignment suppression flag */
     ccscptr,    /* Pointer to string for sscanf() */
     ccsunsc;    /* Input unit for scanf(), fscanf() */
