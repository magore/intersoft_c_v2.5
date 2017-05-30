/*
 * This program splits a source library up into individual
 * modules.  It is capable of simultaneously creating a
 * control file to process each module.
 * 
 * Copyright (c) 1982 by Intersoft
 *
 * by: Richard B. McMurray
 */

#include <stdio/h>

#define LINELEN 130
#define MERGECH '%'
#define TABCH 9

char line[LINELEN],  /* A source line */
     repchar;        /* The replacement character */
int  mergefile;      /* The template file unit number */

main(argc, argv)
  int  argc, *argv;
{
  char *startbuf, *endbuf, *ptr, *bsize;
  int c;


  /* Parse the command line */
  if (argc < 2 || argc > 3 || !(mergefile = fopen(argv[1], "r")))
    usage();
  repchar = MERGECH;
  if (argc == 3)
    if (!amatch(argv[2], "c="))
      usage();
    else
      repchar = fetchch(argv[2], 2);


  /* Read the template file into a memory buffer
   * Trailing whitespace is stripped from all lines of
   * the template file.  The template file is terminated
   * by either EOF or by the first non-printable,
   * non-whitespace character.
   */
  bsize = memleft();
  if (!(startbuf = alloc(bsize))) {
    puts("Insufficient memory!\n");
    exit(1);
  }
  endbuf = startbuf;
  ptr = startbuf - 1;
  while ((c = getc(mergefile)) != EOF) {
    if ((c >= ' ' && c <= '~') || c == TABCH) {
      if (endbuf >= startbuf + bsize) {
        puts("Insufficient memory to save the template file!\n");
        exit(1);
      }
      if (c != ' ' && c != TABCH)
        ptr = endbuf;
      *endbuf++ = c;
    } else
      if (c == '\n') {
        *++ptr = '\n';
        endbuf = ptr + 1;
      } else
        break;
  }
  endbuf = ptr + 1;
  fclose(mergefile);


  /* Read module names from the standard input file */
  while (gets(line) != EOF) {

    /* Module names must begin with a printable non-space
     * character.
     */
    if (*line <= ' ' || *line > '~')
      break;

    /* Copy the template file to the standard output file,
     * replacing the "repchar" with the module name
     */
    for (ptr = startbuf; ptr < endbuf; ptr++) {
      if (*ptr == repchar)
        puts(line);
      else
        putchar(*ptr);
    }
  }
}



/* This function prints the USAGE message and exits. */
usage(){
  puts("USAGE: merge template [c=x] <infile >outfile\n");
  puts("Makes n copies of template where n = # of lines in infile\n");
  puts("Uses successive lines of infile to replace all instances\n");
  puts("of the merge character from the template file.\n");
  exit(1);
}



/* This function returns the n+1th character from a string. */
fetchch(s, n)
  char *s;
  int  n;
  return s[n];



/* This function performs an anchored string match. */
/* "s" is assumed to be as long as or longer than "t". */
amatch(s, t)
  char *s, *t;
{
  while (*t && *s++ == *t++);
  return !*t;
}


