/*
 * This program creates a control file to process each of
 * the output modules of the split program according to 
 * a specified template.
 * in the library that was just split.
 * 
 * Copyright (c) 1982 by Intersoft
 *
 * by: Richard B. McMurray
 */

#include <stdio/h>

#define LINELEN 130
#define MERGECH '%'
#define MAXMODS 100

char line[LINELEN];    /* A source line */
int  infile,           /* The input file unit number */
     outfile,          /* The output file unit number */
     modules[MAXMODS], /* Addresses of the strings containing */
                       /* the module names */
     nmods;            /* The number of modules */

main(argc, argv)
  int  argc, *argv;
{
  int  i;
  char c;

  /* Verify the number of arguments, initialize data */
  if (argc > 1)
    usage();
  outfile = nmods = 0;

  /* Skip lines in the source library until a module marker
   * or end of file is encountered
   */
  while (notnewfile());

  /* Loop until a module marker is not encountered */
  for (;*line == '!';) {

    /* Open a new output module, closing the old one */
    openout();

    /* Copy a portion of the source library (to the next module
     * marker) into the output module
     */
    while (notnewfile()) {
      fputs(line, outfile);
      putc('\n', outfile);
    }
  }

  /* Write the list of module names, one per line */
  for (i = 0; i < nmods; ++i) {
    puts(modules[i]);
    putchar('\n');
  }
}



/* This function prints the USAGE message and exits. */
usage(){
  puts("USAGE: split <infile >namelist\n");
  exit(1);
}



/* This function performs an anchored string match. */
/* "s" is assumed to be as long as or longer than "t". */
amatch(s, t)
  char *s, *t;
{
  while (*t && *s++ == *t++);
  return !*t;
}



/*
 * This function reads a line from the input library,
 * returning zero if the line specifies a new file,
 * otherwise non-zero is returned.
 */
notnewfile()
  return (gets(line) != EOF || *line) && *line != '!';



/*
 * This function closes the current output file and opens
 * the new output file.  The name of the new output file
 * is stored in the array "modules" for use later when
 * the control file to process the modules is created.
 */
openout() {
  if (outfile)
    fclose(outfile);
  modules[nmods++] = strsave(&line[1]);
  outfile = ckopen(&line[1], "w");
}



/*
 * This function saves a string dynamically and returns
 * the address of the saved copy.
 */
strsave(s)
  char *s;
{
  char *p;

  if (p = alloc(strlen(s) + 1))
    strcpy(p, s);
  return p;
}


 