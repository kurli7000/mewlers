.386
.model flat, prolog
ideal
SMART
JUMPS
LOCALS

   ; ----------------------------------------------------------------- ;
   ;                                                                   ;
   ;               SUBMiSSiVES Tiny Heap Replacement                   ;
   ;                                                                   ;
   ;  Original Code from a guy whos's name I forgot. The original      ;
   ;  was limited to 64k heap. I enhanced it and optimized it a        ;
   ;  little bit                                                       ;
   ;                                          1997 SUBMiSSiVE          ;
   ;                                                                   ;
   ; ----------------------------------------------------------------- ;


DATASEG

;  These two variables contains the base and current size of the
;  Heap. Theoretical you can use as many heaps as you want if you
;  save and restore them. Maybe a future version will have this.

hbase   dd 0
hsize   dd 0

CODESEG

public hinit
public halloc
public hfree

proc hinit
; -------------------------------------
; initializes a local heap
; eax    = size in bytes
; esi = base of heap
; -------------------------------------
    push eax esi
    and eax, -1
    mov [hsize], eax
    mov [hbase], esi
    sub eax,4
    mov [dword ptr esi], eax
hinit1:
    pop esi eax
    ret
endp hinit

proc halloc
; ----------------------------------
; allocates some bytes
;   eax = size of block
; returns:
;   esi = blockoffset
;   carry set on error
; ---------------------------------
    inc eax
    and eax,-2
    jmp near hfind
endp halloc

proc hfree
; ---------------------------------
; frees a memory-bloc
; esi = blockoffset
; ---------------------------------
    and [dword ptr esi-4], -2
    call near hcoal
    ret
endp hfree

proc hfind
; ----------------------------------
; finds a memory-bloc
; eax = blocksize
; on success:
;   esi = blockoffset
;   carry = clear
; on error:
;   carry = set
; -----------------------------------

    push eax ebx ecx edi

    mov ecx, eax
    mov ebx, [hbase]
    mov edi,ebx
    add edi, [hsize]

hfind1:
    mov eax,[ebx]
    add ebx, 4
    test eax, 1
    jnz hfind2
    cmp eax,ecx
    jae hfind4
 hfind2:
    and eax,-4
    add ebx, eax
    cmp ebx,edi
    jne hfind1

hfind3:
    stc
    jmp hfind7
hfind4:
    mov esi, ebx
    je hfind6
    sub eax,ecx
    cmp eax, 4
    jae hfind5
    add eax,ecx
    jmp hfind2
hfind5:
    add ebx, ecx
    sub eax, 4
    mov [ebx], eax
hfind6:
    or ecx, 1
    mov [esi-4], ecx
hfind7:
    pop edi ecx ebx eax
    ret
endp hfind

proc hcoal
; -------------------------------
; connects continous free blocks
; --------------------------------
    push eax ebx esi edi
    mov esi, [hbase]
    mov edi, esi
    add edi, [hsize]

hcoal1:
    mov ebx,esi
hcoal2:
    mov eax,[ebx]
    add ebx, 4
    mov esi, eax
    and esi, -2
    add esi,ebx
    cmp esi,edi
    je hcoal3
    test  eax, 1
    jnz hcoal1
    test [dword ptr esi], 1
    jnz hcoal1
    add eax, [esi]
    add eax,4
    sub ebx,4
    mov [ebx],eax
    jmp hcoal2
hcoal3:
    pop edi esi ebx eax
    ret
endp hcoal

end


