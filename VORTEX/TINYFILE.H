#ifndef __tinyfile_h
#define __tinyfile_h

/*
    Little helpers to kick out runtime-code

    These functions do some file i/o stuff and let you get a character
    from the keyboard.

    Please note, that these routines are specific to PmodeW. I tested
    them under DOS4G, and they worked. However since I don't have the
    DOS4G documentation of the enhanced interrupt calls I can't warranty
    that these functions will work!
*/

/* Constants for the SEEKMODE */
#define TINYSEEK_SET 0
#define TINYSEEK_CUR 1
#define TINYSEEK_END 2

/* Constants for the OpenMode */
#define TINYOPEN_READMODE 0
#define TINYOPEN_WRITEMODE 1
#define TINYOPEN_READWRITE 2

/* Predefined System Handles */
#define HSTDOUT 1
#define HSTDIN  0

typedef long doshandle;

doshandle tiny_create (char *name);
#pragma aux tiny_create = "mov ah, 0x03c" \
                          "xor ecx, ecx " \
                          "int 0x21" \
                          parm[edx] modify[eax ecx] value[eax];

doshandle tiny_open (char *name, char openmode);
#pragma aux tiny_open = "mov ah, 0x03d" \
                        "int 0x21 " \
                        parm[edx][al] modify[eax] value[eax];

void tiny_close (doshandle handle);
#pragma aux tiny_close = "mov ax, 0x03e00" \
                         "int 0x21 " \
                        parm[ebx] modify[eax];

long tiny_seek (doshandle handle, long pos, char mode);
#pragma aux tiny_seek =  "mov ah, 0x042"    \
                         "mov ecx, edx"     \
                         "shr ecx, 16"      \
                         "int 0x21 "        \
                         "shl edx, 16"      \
                         "mov dx, ax"       \
                          parm[ebx][edx][al] modify[eax ecx] value[edx];

long tiny_read (doshandle handle, long numbytes, void *dest);
#pragma aux tiny_read = "mov ah, 0x3f" \
                        "int 0x21" \
                        parm[ebx][ecx][edx] modify[ebx ecx edx] value[eax]

long tiny_write (doshandle handle, long numbytes, void *dest);
#pragma aux tiny_write = "mov ah, 0x40" \
                         "int 0x21" \
                        parm[ebx][ecx][edx] modify[ebx ecx edx] value[eax]

char tiny_inkey (void);
#pragma aux tiny_inkey = "mov ax, 0x0c08" \
                         "int 0x21"       \
                         value[al] modify[eax ebx ecx edx];
#endif
