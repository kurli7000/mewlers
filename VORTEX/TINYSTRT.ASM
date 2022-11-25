.386
.387
.model flat,prolog
locals
smart
jumps
IDEAL


STACK 10000h

DATASEG

CODESEG

extrn main_: proc

public __do_exit_with_msg__
public _cstart_
public __CHP
public ___wcpp_4_data_module_dtor_ref_
public ___wcpp_4_data_init_fs_root_
public __wcpp_4_undefed_cdtor__
;public __wcpp_4_lcl_register__
public __init_387_emulator
public __ppfltused_
public _fltused_
public __8087
public exit_
public __GETDS
public __psp
public __argc
___wcpp_4_data_module_dtor_ref_:
___wcpp_4_data_init_fs_root_:
__wcpp_4_undefed_cdtor__:
;__wcpp_4_lcl_register__:
__init_387_emulator:
__ppfltused_:
_fltused_:
__8087:
_cstart_:
__argc:

__CHP:
  ret
__GETDS:
  mov ds,[ds_save]
  ret

start:
  jmp cmain
  db 'WATCOM'
cmain:
;  mov     ax,0024H
;  mov [__psp,ax
;  mov [__psp],es
  sti
  cld
  push ds
  pop es
  mov [ds_save],ds

  finit
  push 1F7Fh
  fldcw [word ptr esp]
  add esp,4

  call main_

__do_exit_with_msg__:
exit_:
  mov ax,4c00h
  int 21h

  ds_save dw 0
  __psp dw 24h

end start
