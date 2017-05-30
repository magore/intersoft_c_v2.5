/*
 * Operating System Interface Library
 * for Intersoft C v2.5
 *
 * Copyright (c) 1982 by Intersoft
 * 
 * By: R. B. McMurray
 */

!ALLOC
#include <stdio/h>
#include "os/h"

/* alloc(n) - Attempt to allocate a block of memory. */
alloc(n)
  char *n;
{
  int  *ptr, mcb[2];

  /* May allocate 1 <= n <= 65536 - ALLOCOVERHEAD bytes. */
  if (!n || n >= -ALLOCOVERHEAD)
    return NULL;

  /*
   * Increase the size of the block to allocate by the
   * overhead on allocated blocks.  The minimum
   * allocation size is the overhead on unallocated
   * blocks.
   */
  if ((n += ALLOCOVERHEAD) < FREEOVERHEAD)
    n = FREEOVERHEAD;

  /*
   * Find the first free block large enough to satisfy the
   * request.
   * Return NULL (zero) if the request cannot be satisfied
   * due to insufficient memory.
   */
  for (ptr = ccmcb; n > *ptr; ptr = ptr[1])
    if (!*ptr)
      return NULL;

  if (*ptr - FREEOVERHEAD >= n) {
    /* Return the tail of the free block. */
    *ptr -= n;
    ptr = ptr[1] + *ptr;
    *ptr = n;
    n = ptr;
    return n + ALLOCOVERHEAD;
  }

  /*
   * There will be insufficient unused space left to
   * keep the free block.  Return the entire block.
   */
  copybyte(ptr, mcb, FREEOVERHEAD);
  copybyte(ptr[1], ptr, FREEOVERHEAD);
  *(mcb[1]) = *mcb;
  return mcb[1] + ALLOCOVERHEAD;
}



!FREE
#include <stdio/h>
#include "os/h"
#include "io/h"

/* free(address) - Deallocate a block of memory. */
free(addr)
  char *addr;
{
  int  *ptr, *oldptr, *sizeptr, mcb[2];

  /*
   * Set the address to the actual beginning of the
   * allocated block.  Check that the block to be freed
   * is within the free list.
   */
  sizeptr = (addr -= ALLOCOVERHEAD);
  ptr = *sizeptr;  
  if (addr < ccmem || addr >= ccemem || ptr < FREEOVERHEAD ||
      addr + *sizeptr > ccemem || addr + *sizeptr < ccmem)
    return FALSE;

  /* Find the block's proper place within the free list. */
  for (oldptr = 0, ptr = ccmcb; *ptr && addr > ptr[1];
       oldptr = ptr, ptr = ptr[1]);

  /* Does the block t overlap adjacent free blocks? */
  if ((*ptr && addr + *sizeptr > ptr[1]) ||
      (oldptr && addr < oldptr[1] + *oldptr))
    return FALSE;

  if (oldptr && addr == *oldptr + oldptr[1]) {
    /*
     * Append the deallocated space to the preceeding free
     * block.  This block becomes the current block.
     */
    *oldptr += *sizeptr;
    ptr = oldptr;
  } else { 
    /*
     * Make the deallocated space into a new free block.
     * This new block becomes the current block.
     */
    copybyte(ptr, mcb, FREEOVERHEAD);
    *ptr = *sizeptr;
    ptr[1] = addr;
    copybyte(mcb, addr, FREEOVERHEAD);
  }

  if (*ptr + ptr[1] == *(ptr[1] + 2)) {
    /* Combine the current block with the next block. */
    *ptr += *(ptr[1]);
    copybyte(*(ptr[1] + 2), ptr[1], FREEOVERHEAD);
  }
  return TRUE;
}



!MEMINI
#include <stdio/h>
#include "os/h"
#include "io/h"

/* meminit() - Reinitialize the dynamic memory pool. */
meminit()
{
  int *ptr;

  ccemem = *(0x4049) - ccstkmg;
  *ccmcb = (ccmem > ccemem) ? 0 : ccemem - ccmem;
  ccmcb[1] = *ccmcb ? ccmem : 0;
  if (*ccmcb) {
    ptr = ccmem;
    *ptr = ptr[1] = NULL;
  }
}



!MEMLEF
#include <stdio/h>
#include "os/h"

/* memleft() - Return the size of the largest free block. */
memleft()
{
  int  *ptr;
  char *maxval;

  for (ptr = ccmcb, maxval = 0; *ptr; ptr = ptr[1])
    if (*ptr > maxval)
      maxval = *ptr;
  return maxval - ALLOCOVERHEAD;
}


!OSG
/* Globals for the OS support */

#include <stdio/h>
#include "os/h"
