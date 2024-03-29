/* This program allows the operator to extract
 * modules from an Intersoft format source library.
 * Copyright (c) 1982 by Intersoft
 * By: Richard McMurray
 */


#include <stdio/h>


/* Define the maximum line length */
#define LINELEN 256



/* File descriptors and names of the source library and
 * module being extracted.
 */
FILE library,
     module;
char *libname,
     *modname;



/* Address and size of the memory buffer */
char *buffer;
int  bsize;



/* Extract the specified modules from the specified
 * libraries
 */
main() {
  libname = modname = 0;
  bsize = memleft() - 0x400;
  if (!(buffer = alloc(bsize))) {
    printf("Insufficient memory!\n");
    exit(1);
  }
  while (getlib())
    while (getmod());
}



/* Get a new library from which modules are to be
 * extracted.
 */
getlib() {
  char line[LINELEN];

  do {
    puts("Library name (Press ENTER to exit) ? ");
    gets(line);
    if (!strlen(line))  /* A null response exits. */
      return 0;
    if (!(library = fopen(line, "r")))
      printf("Cannot open %s!\n", line);
  } while (!library);

  /* Close the library.  We will re-open it later. */
  fclose(library);

  /* Save the name of the library. */
  free(libname);
  libname = strsave(line);
  return 1;
}



/* Get the names of modules to be extracted.  As each
 * name is provided the library is searched and the
 * appropriate block of text is extracted.
 */
getmod() {
  char line[LINELEN], *ptr, *ptr2;
  int  i, lasti;

  for (;;) {
    puts("Module name (Press ENTER to exit) ? ");
    gets(line);
    if (!strlen(line))
      return 0;
    free(modname);
    modname = strsave(line);
    if (!(library = fopen(libname, "r"))) {
      printf("Cannot open the source library!\n");
      continue;
    }

    /* Find the module within the source library */
    while (fgets(line, library) != EOF) {
      if (line[0] == '!' && match(line+1, modname))
        break;
    }
    if (feof(library)) {
      printf("Module %s not found!\n", modname);
      fclose(library);
      continue;
    }

    /* Read the module into memory and close the library.
     * The module is terminated either by End of File,
     * module header (line beginning with '!').  We also
     * check to make sure that the module doesn't overflow
     * the memory buffer.
     */
    ptr = buffer;
    lasti = 0;
    while ((i = getc(library)) != EOF && (lasti != '\n' ||
            i != '!') && buffer + bsize > ptr) {
      *ptr++ = i;
      lasti = i;
    }
    fclose(library);
    if (buffer + bsize <= ptr) {
      printf("Cannot fit the module into memory!\n");
      continue;
    }

    /* Get the output file name, copy the module to the
     * output file and close the output file.
     */
    puts("The module is in memory.\nName of output file ? ");
    gets(line);
    if (!(module = fopen(line, "w"))) {
      printf("Cannot open %s for output!\n");
      continue;
    }
    for (ptr2 = buffer; ptr2 < ptr; putc(*ptr2++, module));
    fclose(module);
  }
}



/* Save a string dynamically, returning its new address */
strsave(s)
  char *s;
{
  char *d;

  d = alloc(strlen(s) + 1);
  strcpy(d, s);
  return d;
}



/* Match a module name to a module header in a source
 * library.
 */
match(s, t)
  char *s, *t;
{
  while (*s && toupper(*s) == toupper(*t)) {
    s++;
    t++;
  }
  return !*s && !*t;
}
 