/*
 * Copy from standard input to standard output.
 * Copyright April 1981 by Intersoft Unlimited
 * by Bernie Roehl
 */
#include "stdio/h"
main(argc, argv)
  int argc, argv[];
{
  int in, c;
  if (argc == 1)
     in = STDIN;
  else
     in = ckopen(argv[1], "r");
  while ((c = getc(in)) != EOF)
     putc(c, STDOUT);
}
