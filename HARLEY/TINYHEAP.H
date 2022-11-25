#ifndef __tinyheap_h
#define __tinyheap_h

 /*
  * Tiny Heap Replacement
  *
  * Usefull if you need a fast heap and want to eliminate the large overhead
  * of the Watcom Heap. Please note, that you can't simply use the heap,
  * you have to initialize it first. Give the function hinit a memory block
  * and it's size..
  *
  * There are two different ways to get this memoryblock. The easy one is
  * to declare a static structure like this:
  *
  *    char heapmem[0x10000]
  *    hinit (heapmem, 0x10000);
  *
  * The more advanced user might alloc the heap with a dpmi-alloc call.
  *
  */

#ifdef __cplusplus
extern "C" {
#endif

void hinit (void *base, unsigned long size);
#pragma aux hinit "*" parm[esi][eax];

void *halloc (unsigned long size);
#pragma aux halloc "*" parm[eax] value [esi];

void hfree (void *data);
#pragma aux hfree "*" parm[esi];

#ifdef __cplusplus
}
#endif

#endif
