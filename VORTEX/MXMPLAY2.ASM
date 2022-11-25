;// MXM player, (c) '95/96 Niklas Beisert / pascal

.386
locals

_TEXT segment dword public use32 'CODE'
assume cs:_TEXT

;copyright db "cubic tiny gus xm player v1.4 (c) '95/96 Niklas Beisert / pascal"

USEGLISSANDO=1
USEOFFSET=1
USETREMOR=1
USEENVPOS=1
USEVIBRATO=1
USETREMOLO=1
USEGVOL=1
USEPAN=1
USESPAN=1
USEARPEGGIO=1
USEMRETRIG=1
USERETRIG=1
USEPORTANOTE=1
USENOTECUT=1
USEPORTA=1
USEGVOLSLIDE=1
USEPORTAVOL=1
USEVIBRATOVOL=1
USESPEED=1
USEBREAK=1
USEJUMP=1
USEPATLOOP=1
USEPATDELAY=1
USEFPORTA=1
USEXFPORTA=1
USEVIBTYPE=1
USETREMTYPE=1
USESYNC=1
USEVOLSLIDE=1
USEPANSLIDE=1
USEKEYOFFCMD=1
USEDELAY=1
USEFVOLSLIDE=1
USEVOL=1

USEVVOL=1
USEVVOLSLIDE=1
USEVFVOLSLIDE=1
USEVVIBRATE=1
USEVVIBRATO=1
USEVPAN=1
USEVPANSLIDE=1
USEVPORTANOTE=1

USEVOLENV=1
USEPANENV=1
USEAUTOVIBRATO=1
USEAUTOVIBRATOTYPE=1
USE16BIT=1
USEDELTASAMP=1

USEFREQTAB=2 ;// 0:amiga, 1:linear, 2:both

;//this is the right place to include the effects include file!
;include xxx.inc
;include chip-goy.inc

USEVOLCOL=(USEVVOL or USEVVOLSLIDE or USEVFVOLSLIDE or USEVVIBRATE or USEVVIBRATO or USEVPAN or USEVPANSLIDE or USEVPORTANOTE)
USEAMIGAFREQ=((USEFREQTAB eq 1) eq 0)
USELINEARFREQ=((USEFREQTAB eq 0) eq 0)
USEBOTHFREQ=(USEAMIGAFREQ and USELINEARFREQ)

mxmheader struc
  hdMXMSig dd ?
  hdNOrders dd ?
  hdOrdLoopStart dd ?
  hdNChannels dd ?
  hdNPatterns dd ?
  hdNInstruments dd ?
  hdIniTempo db ?
  hdIniBPM db ?
  hdOptions dw ?
  hdSampStart dd ?
  hdSampMem8 dd ?
  hdSampMem16 dd ?
  hdPitchMin dd ?
  hdPitchMax dd ?
  hdPanPos db 32 dup (?)
  hdOrderTable db 256 dup (?)
  hdInstrTable dd 128 dup (?)
  hdPatternTable dd 256 dup (?)
ends

instrument struc
  insNSamples dd ?
  insSamples db 96 dup (?)
  insVolFade dw ?
  insVibType db ?
  insVibSweep db ?
  insVibDepth db ?
  insVibRate db ?
  insVNum db ?
  insVSustain db ?
  insVLoopS db ?
  insVLoopE db ?
  insVEnv dw 24 dup (?)
  insPNum db ?
  insPSustain db ?
  insPLoopS db ?
  insPLoopE db ?
  insPEnv dw 24 dup (?)
  db 46 dup (?)
ends

sample struc
  smpGUSStartPos db ?,?,?
  smpGUSLoopPos db ?,?,?
  smpGUSEndPos db ?,?,?
  smpGUSMode db ?
  smpDefVol db ?
  smpDefPan db ?
  smpNormNote dw ?
  db ?,?
ends




channelsize = 256
channel struc
  chGUSInited db ?
  chGUSStartPos dd ?
  chGUSEndPos dd ?
  chGUSLoopPos dd ?
  chGUSMode db ?
  chGUSStopIt db ?
  chGUSChangeSamp db ?
  chGUSNextPos dd ?
  chGUSFrq dw ?
  chGUSVol dw ?
  chGUSPan db ?

  chVol db ?
  chFinalVol db ?
  chPan db ?
  chFinalPan db ?
  chPitch dd ?
  chFinalPitch dd ?

  chCurIns db ?
  chEnvIns dd ?
  chCurNormNote dw ?
  chSustain db ?
  chFadeVol dw ?
  chAVibPos db ?
  chAVibSwpPos db ?
  chVolEnvPos dd ?
  chVolEnvSegPos dw ?
  chPanEnvPos dd ?
  chPanEnvSegPos dw ?

  chDefVol db ?
  chDefPan db ?
  chCommand db ?
  chVCommand db ?
  chPortaToPitch dd ?
  chPortaToVal dd ?
  chVolSlideVal db ?
  chGVolSlideVal db ?
  chVVolPanSlideVal db ?
  chPanSlideVal db ?
  chFineVolSlideUVal db ?
  chFineVolSlideDVal db ?
  chPortaUVal dd ?
  chPortaDVal dd ?
  chFinePortaUVal db ?
  chFinePortaDVal db ?
  chXFinePortaUVal db ?
  chXFinePortaDVal db ?
  chVibRate db ?
  chVibPos db ?
  chVibType db ?
  chVibDep db ?
  chTremRate db ?
  chTremPos db ?
  chTremType db ?
  chTremDep db ?
  chPatLoopCount db ?
  chPatLoopStart db ?
  chArpPos db ?
  chArpNotes db ?,?,?
  chActionTick db ?
  chMRetrigPos db ?
  chMRetrigLen db ?
  chMRetrigAct db ?
  chDelayNote db ?
  chOffset db ?
  chGlissando db ?
  chTremorPos db ?
  chTremorLen db ?
  chTremorOff db ?
ends

globaldatastruct struc
  globalvol db ?
  uservol db ?
  syncval db ?

  curtick db ?
  curtempo db ?
  tick0 db ?

  currow dd ?
  patptr dd ?
  patlen dd ?

  curord dd ?

  jumptoord dd ?
  jumptorow dd ?
  patdelay db ?

  procnot db ?
  procins db ?
  procvol db ?
  proccmd db ?
  procdat db ?
  notedelayed db ?

  tmOldTimer df ?
  tmOldSSESP df ?
  tmIntCount dd ?
  tmTimerRate dd ?
  tmTicker dd ?
  tmInRoutine dd ?
  stimerlen dd ?
  stimerpos dd ?
  datasegsel dw ?
  maxtimerrate dd ?

  gusport dd ?
  guschannels dd ?

  portatmp db ?

  head mxmheader ?

  guslinvol dw 257 dup (?)

  chandata db channelsize*32 dup (?)

  tmStack db 1024 dup (?)

  vibtabs db 1024 dup (?)

  jarnodata db 256 dup (?)
ends

;//.data?
;//globdat globaldatastruct ?
;//.code

globdatptr dd 0

;//*************************************************************************
;// XM player

loadebp proc
;//  lea ebp,globdat  ;// would cause a relocation
  call @@getadr
@@getadr:
  pop ebp
  mov ebp,cs:[ebp+globdatptr-@@getadr]
  ret
endp

inittables proc
if USEVIBRATO or USEVVIBRATO or USEAUTOVIBRATO
  call @@getadr
@@getadr:
  pop esi
  add esi,sintab-@@getadr
  lea edi,[ebp].vibtabs
  mov ecx,16
  rep movsd
  mov al,64
  stosb
  mov cl,63
@@sintabloop1:
    dec esi
    mov al,[esi]
    stosb
  dec cl
  jnz @@sintabloop1
  lea esi,[ebp].vibtabs
  mov cl,128
@@sintabloop2:
    lodsb
    neg al
    stosb
  dec cl
  jnz @@sintabloop2

if USEVIBTYPE or USETREMTYPE or USEAUTOVIBRATOTYPE
@@dwntabloop:
    mov al,cl
    sar al,1
    neg al
    stosb
  dec cl
  jnz @@dwntabloop

@@rectabloop:
    mov al,cl
    and al,80h
    sub al,40h
    stosb
  dec cl
  jnz @@rectabloop

@@uptabloop:
    mov al,cl
    sar al,1
    stosb
  dec cl
  jnz @@uptabloop
endif
endif

  mov [ebp].guslinvol[0],0
  mov [ebp].guslinvol[512],0EFFFh

  mov edi,1
@@lintabloop:
    mov ebx,edi
    mov edx,7
  @@logloop:
    cmp ebx,0
    je @@logfound
      shr ebx,1
      inc edx
    jmp @@logloop
  @@logfound:
    mov eax,edi
    mov cl,20
    sub cl,dl
    shl eax,cl
    and eax,0fffh
    shl edx,12
    or eax,edx
    sub eax,1000h
    mov [ebp].guslinvol[2*edi],ax
  inc edi
  cmp edi,256
  jne @@lintabloop

  ret
endp


public xmpInit_
xmpInit_ proc ;// esi:mxmdata, ecx:maxtimerrate, eax:pspseg, ebx:globdat
  push ebp

;//  lea ebx,globdat
  call @@getadr
@@getadr:
  pop ebp
  mov [ebp+globdatptr-@@getadr],ebx
  mov ebp,ebx
  mov ebx,4000h
@@inilp:
    dec ebx
    mov byte ptr [ebp+ebx],0
  jnz @@inilp

;  mov [ebp].maxtimerrate,ecx
  mov [ebp].uservol,40h

  test eax,80000000h
  jnz @@portgiven
    call gusGetPort
@@portgiven:
  and eax,not 80000000h
  mov [ebp].gusport,eax

  cmp [esi].hdMXMSig,004D584Dh
  jne @@fail

  lea edi,[ebp].head.hdMXMSig
  mov ecx,750h/4
  rep movsd

  sub esi,750h
  sub edi,600h
  mov ecx,180h
@@relloop:
    add [edi],esi
    add edi,4
  dec ecx
  jnz @@relloop

if USEDELTASAMP
  test byte ptr [esi].hdOptions,4
  jz @@nodelta
    and byte ptr [esi].hdOptions,not 4

    mov ecx,[esi].hdSampMem8
    mov ebx,[esi].hdSampStart
    add ebx,esi
    xor eax,eax
    cmp ecx,0
    je @@8bitfini
  @@8bitdelta:
      add al,[ebx]
      mov [ebx],al
      inc ebx
    dec ecx
    jnz @@8bitdelta
  @@8bitfini:
if USE16BIT
    mov ecx,[esi].hdSampMem16
    xor eax,eax
    cmp ecx,0
    je @@16bitfini
  @@16bitdelta:
      add ax,[ebx]
      mov [ebx],ax
      add ebx,2
    dec ecx
    jnz @@16bitdelta
  @@16bitfini:
endif
@@nodelta:
endif

  mov ecx,[esi].hdSampMem16
  shl ecx,1
  add ecx,[esi].hdSampMem8
  mov eax,[esi].hdSampStart
  add esi,eax

  call gusUploadSamples
  call inittables

  mov eax,1
  jmp @@done

@@fail:
  xor eax,eax
@@done:

  pop ebp
  ret
endp






getfreq6848 proc
  push edx
  push ebx
  push ecx
  push esi
  add eax,8000h
  mov edx,eax
  mov ebx,eax
  mov ecx,eax
  shr eax,12
  shr edx,8
  shr ebx,4
  and eax,15
  and ebx,15
  and ecx,15
  and edx,15
  call @@getadr
@@getadr:
  pop esi
  add esi,logfreqtab-@@getadr
  mov eax,[esi+6*16+eax*4]
  movzx edx,word ptr [esi+4*16+edx*2]
  movzx ebx,word ptr [esi+2*16+ebx*2]
  movzx ecx,word ptr [esi+0*16+ecx*2]
  mul edx
  shrd eax,edx,15
  mul ebx
  shrd eax,edx,15
  mul ecx
  shrd eax,edx,15
  pop esi
  pop ecx
  pop ebx
  pop edx
  ret
endp




PlayNote proc
  mov [ebp].portatmp,0

  cmp [ebp].proccmd,3
  jne @@noportac
    mov [ebp].portatmp,1
@@noportac:
  cmp [ebp].proccmd,5
  jne @@noportacv
    mov [ebp].portatmp,1
@@noportacv:
  cmp [ebp].procvol,0f0h
  jb @@noportav
    mov [ebp].portatmp,1
@@noportav:

  cmp [ebp].procnot,97
  jne @@nokeyoff
    mov [edi].chSustain,0
    mov [ebp].procnot,0
@@nokeyoff:

  movzx eax,[ebp].procins
  cmp al,0
  je @@noins1
  cmp eax,[ebp].head.hdNInstruments
  ja @@noins1
    mov [edi].chCurIns,al
@@noins1:
  cmp [edi].chCurIns,0
  je @@done

  movzx eax,[ebp].procnot
  cmp al,0
  je @@nonote
    cmp [ebp].procins,0
    je @@nohit
      mov [edi].chSustain,1
  @@nohit:

    mov [edi].chDelayNote,al
    cmp [ebp].proccmd,49
    jne @@nodelay
      cmp [ebp].procdat,0
      jne @@done
  @@nodelay:

    dec al
    cmp [ebp].portatmp,1
    je @@portanote
      mov [edi].chGUSStopIt,1

      movzx edx,[edi].chCurIns
      mov [ebp].jarnodata[edx],1
      dec dl
      mov edx,[ebp].head.hdInstrTable[4*edx]
      movzx ebx,insSamples[edx][eax]
      cmp ebx,insNSamples[edx]
      jae @@done
      shl ebx,4
      lea ebx,[ebx+edx+256]

      mov [edi].chGUSInited,1
      mov [edi].chGUSChangeSamp,1

      push eax
      mov eax,dword ptr [ebx].smpGUSStartPos
      and eax,0ffffffh
      mov [edi].chGUSStartPos,eax
      mov eax,dword ptr [ebx].smpGUSLoopPos
      and eax,0ffffffh
      mov [edi].chGUSLoopPos,eax
      mov eax,dword ptr [ebx].smpGUSEndPos
      and eax,0ffffffh
      mov [edi].chGUSEndPos,eax
      mov al,[ebx].smpGUSMode
      mov [edi].chGUSMode,al
      pop eax

      cmp [ebp].procins,0
      je @@noins2
        mov [edi].chEnvIns,edx
        mov dl,[ebx].smpDefVol
        mov [edi].chDefVol,dl
        mov dl,[ebx].smpDefPan
        mov [edi].chDefPan,dl
    @@noins2:

      mov dx,[ebx].smpNormNote
      mov [edi].chCurNormNote,dx

;// process finetune here (proccmd==41)
;// overwrite top 4 bits of instrument finetune value
;// cannot do this correctly, since conversion reduced information... :(
;// this command sucks anyway!!!

      shl eax,8
      add ax,dx
      neg ax
      add ah,48
      movsx eax,ax
if USEBOTHFREQ
      test byte ptr [ebp].head.hdOptions,1
      jnz @@noamiga1
endif
if USEAMIGAFREQ
        neg eax
        call getfreq6848
    @@noamiga1:
endif
      mov [edi].chPitch,eax
      mov [edi].chFinalPitch,eax
      mov [edi].chPortaToPitch,eax

      xor eax,eax
if USEOFFSET
      cmp [ebp].proccmd,9
      jne @@nooffset
        mov al,[ebp].procdat
        cmp al,0
        je @@reuseoffset
          mov [edi].chOffset,al
      @@reuseoffset:
        movzx eax,[edi].chOffset
        shl eax,8
    @@nooffset:
endif

      mov [edi].chGUSNextPos,eax
      mov [edi].chVibPos,0
      mov [edi].chTremPos,0
      mov [edi].chArpPos,0
      mov [edi].chMRetrigPos,0
      mov [edi].chTremorPos,0
      jmp @@nonote

  @@portanote:
      shl eax,8
      add ax,[edi].chCurNormNote
      neg ax
      add ah,48
      movsx eax,ax
if USEBOTHFREQ
      test byte ptr [ebp].head.hdOptions,1
      jnz @@noamiga2
endif
if USEAMIGAFREQ
        neg eax
        call getfreq6848
    @@noamiga2:
endif
      mov [edi].chPortaToPitch,eax
@@nonote:

  cmp [edi].chSustain,0
  je @@done
  cmp [ebp].procins,0
  je @@done
    cmp [ebp].notedelayed,1
    je @@noinsvolpan
      mov al,[edi].chDefVol
      mov [edi].chVol,al
      mov [edi].chFinalVol,al
      test byte ptr [ebp].head.hdOptions,2
      jnz @@noinsvolpan
      mov al,[edi].chDefPan
      mov [edi].chPan,al
      mov [edi].chFinalPan,al
  @@noinsvolpan:
    xor eax,eax
    mov [edi].chFadeVol,8000h
    mov [edi].chAVibPos,al
    mov [edi].chAVibSwpPos,al
    mov [edi].chVolEnvPos,eax
    mov [edi].chVolEnvSegPos,ax
    mov [edi].chPanEnvPos,eax
    mov [edi].chPanEnvSegPos,ax

@@done:
  ret
endp

public xmpSetVolume_
xmpSetVolume_ proc
  push ebp
  call loadebp
  mov [ebp].uservol,al
  pop ebp
  ret
endp

public xmpGetSync_
xmpGetSync_ proc
  push ebp
  call loadebp
  mov al,[ebp].syncval
  pop ebp
  ret
endp

public xmpGetPos_
xmpGetPos_ proc
  push ebp
  call loadebp
  mov al,byte ptr [ebp].currow
  mov ah,byte ptr [ebp].curord
  pop ebp
  ret
endp




freqrange proc
  cmp eax,[ebp].head.hdPitchMin
  jg @@lowlimok
    mov eax,[ebp].head.hdPitchMin
@@lowlimok:
  cmp eax,[ebp].head.hdPitchMax
  jl @@highlimok
    mov eax,[ebp].head.hdPitchMax
  jmp @@highlimok
@@highlimok:
  ret
endp


;//***************************************************************************
;//effects

procnothing proc
  ret
endp

if USEJUMP
procjump proc
  movzx eax,[ebp].procdat
  mov [ebp].jumptoord,eax
  mov [ebp].jumptorow,0
  ret
endp
else
  procjump=procnothing
endif

if USEBREAK
procbreak proc
  cmp [ebp].jumptoord,-1
  jne @@onlyrow
    mov eax,[ebp].curord
    inc eax
    mov [ebp].jumptoord,eax
@@onlyrow:
  movzx eax,[ebp].procdat
  mov ebx,eax
  shr al,4
  imul eax,10
  and bl,0fh
  add eax,ebx
  mov [ebp].jumptorow,eax
  ret
endp
else
  procbreak=procnothing
endif

if USEPATLOOP
procpatloop proc
  mov al,[ebp].procdat
  cmp al,0
  je @@set
    inc [edi].chPatLoopCount
    cmp [edi].chPatLoopCount,al
    ja @@nextrow
      movzx eax,[edi].chPatLoopStart
      mov [ebp].jumptorow,eax
      mov eax,[ebp].curord
      mov [ebp].jumptoord,eax
    jmp @@done
  @@nextrow:
      mov [edi].chPatLoopCount,0
      mov al,byte ptr [ebp].currow
      inc al
      mov [edi].chPatLoopStart,al
    jmp @@done
@@set:
  mov al,byte ptr [ebp].currow
  mov [edi].chPatLoopStart,al
@@done:
  ret
endp
else
  procpatloop=procnothing
endif

if USEPATDELAY
procpatdelay proc
  mov al,[ebp].procdat
  mov [ebp].patdelay,al
  ret
endp
else
  procpatdelay=procnothing
endif

if USESPEED
proctempo proc
  movzx ebx,[ebp].procdat
  cmp bl,20h
  jb @@speed
    mov eax,2983615
    xor edx,edx
    div ebx
    mov [ebp].stimerlen,eax
    ret
@@speed:
    cmp bl,0
    je @@ignore
    mov [ebp].curtempo,bl
@@ignore:
  ret
endp
else
  proctempo=procnothing
endif

if USEVOL
procnvol proc
  mov al,[ebp].procdat
  cmp al,40h
  jbe @@vok
    mov al,40h
@@vok:
  mov [edi].chVol,al
  mov [edi].chFinalVol,al
  ret
endp
else
  procnvol=procnothing
endif

if USEGVOL
procgvol proc
  mov al,[ebp].procdat
  cmp al,40h
  jbe @@vok
    mov al,40h
@@vok:
  mov [ebp].globalvol,al
  ret
endp
else
  procgvol=procnothing
endif

if USEPAN
procpan proc
  mov al,[ebp].procdat
  mov [edi].chPan,al
  mov [edi].chFinalPan,al
  ret
endp
else
  procpan=procnothing
endif

if USESPAN
procspan proc
  mov al,[ebp].procdat
  shl al,4
  or al,[ebp].procdat
  mov [edi].chPan,al
  mov [edi].chFinalPan,al
  ret
endp
else
  procspan=procnothing
endif

if USEVPAN
procvpan proc
  mov al,[ebp].procvol
  shl al,4
  or al,[ebp].procvol
  mov [edi].chPan,al
  mov [edi].chFinalPan,al
  ret
endp
else
  procvpan=procnothing
endif

if USEARPEGGIO
procarpeggio proc
  movzx eax,[ebp].procdat
  cmp al,0
  jne @@doit
    mov [edi].chCommand,0ffh
@@doit:
  shl eax,4
  shr al,4
  mov [edi].chArpNotes[0],0
  mov [edi].chArpNotes[1],ah
  mov [edi].chArpNotes[2],al
  ret
endp

doarpeggio proc
  movzx eax,[edi].chArpPos
  mov al,[edi].chArpNotes[eax]
if USEBOTHFREQ
  test byte ptr [ebp].head.hdOptions,1
  jz @@amiga
endif
if USELINEARFREQ
    shl eax,8
    neg eax
    add eax,[edi].chFinalPitch
    call freqrange
    mov [edi].chFinalPitch,eax
endif
if USEBOTHFREQ
  jmp @@noamiga
endif
if USEAMIGAFREQ
@@amiga:
    call @@getadr
  @@getadr:
    pop edx
    mov ax,[edx+logfreqtab[16*4+eax*2]-@@getadr]
    mul [edi].chFinalPitch
    shrd eax,edx,15
    call freqrange
    mov [edi].chFinalPitch,eax
endif

@@noamiga:
  inc [edi].chArpPos
  cmp [edi].chArpPos,3
  jne @@done
    mov [edi].chArpPos,0
@@done:
  ret
endp
else
  procarpeggio=procnothing
  doarpeggio=procnothing
endif



if USETREMTYPE and USETREMOLO
proctremtype proc
  mov al,[ebp].procdat
  and al,3
  mov [edi].chTremType,al
  ret
endp
else
  proctremtype=procnothing
endif

if USETREMOLO
proctremolo proc
  mov al,[ebp].procdat
  and al,0Fh
  jz @@reusel
    shl al,2
    mov [edi].chTremDep,al
@@reusel:
  mov al,[ebp].procdat
  and al,0F0h
  jz @@reuseh
    shr al,2
    mov [edi].chTremRate,al
@@reuseh:
  ret
endp

dotremolo proc
  movzx eax,word ptr [edi].chTremPos
  movsx eax,[ebp].vibtabs[eax]
  imul [edi].chTremDep
  sar eax,6
  add al,[edi].chFinalVol
  jns @@lok
    mov al,0
@@lok:
  cmp al,40h
  jbe @@tok
    mov al,40h
@@tok:
  mov [edi].chFinalVol,al

  cmp [ebp].tick0,0
  jne @@done
  mov al,[edi].chTremRate
  add [edi].chTremPos,al
@@done:
  ret
endp
else
  proctremolo=procnothing
  dotremolo=procnothing
endif





if USEFPORTA
procfportau proc
  mov al,[ebp].procdat
  cmp al,0
  je @@reuse
    mov [edi].chFinePortaUVal,al
@@reuse:
  movzx eax,[edi].chFinePortaUVal
  shl eax,4
  neg eax
  add eax,[edi].chPitch
  call freqrange
  mov [edi].chPitch,eax
  mov [edi].chFinalPitch,eax
  ret
endp

procfportad proc
  mov al,[ebp].procdat
  cmp al,0
  je @@reuse
    mov [edi].chFinePortaDVal,al
@@reuse:
  movzx eax,[edi].chFinePortaDVal
  shl eax,4
  add eax,[edi].chPitch
  call freqrange
  mov [edi].chPitch,eax
  mov [edi].chFinalPitch,eax
  ret
endp
else
  procfportau=procnothing
  procfportad=procnothing
endif


if USEXFPORTA
procxfporta proc
  movzx eax,[ebp].procdat
  shl eax,4
  shr al,4
  cmp ah,2
  je @@down
  cmp ah,1
  jne @@done

    cmp al,0
    je @@reuseu
      mov [edi].chXFinePortaUVal,al
  @@reuseu:
    movzx eax,[edi].chXFinePortaUVal
    shl eax,2
    neg eax
    add eax,[edi].chPitch
    call freqrange
    mov [edi].chPitch,eax
    mov [edi].chFinalPitch,eax
  jmp @@done

@@down:
    cmp al,0
    je @@reused
      mov [edi].chXFinePortaDVal,al
  @@reused:
    movzx eax,[edi].chXFinePortaDVal
    shl eax,2
    add eax,[edi].chPitch
    call freqrange
    mov [edi].chPitch,eax
    mov [edi].chFinalPitch,eax

@@done:
  ret
endp
else
  procxfporta=procnothing
endif


if USEFVOLSLIDE
procfvolup proc
  mov al,[ebp].procdat
  cmp al,0
  je @@reuse
    mov [edi].chFineVolSlideUVal,al
@@reuse:
  mov al,[edi].chVol
  add al,[edi].chFineVolSlideUVal
  cmp al,40h
  jbe @@vok
    mov al,40h
@@vok:
  mov [edi].chVol,al
  mov [edi].chFinalVol,al
  ret
endp

procfvoldn proc
  mov al,[ebp].procdat
  cmp al,0
  je @@reuse
    mov [edi].chFineVolSlideDVal,al
@@reuse:
  mov al,[edi].chVol
  sub al,[edi].chFineVolSlideDVal
  jnc @@vok
    mov al,0
@@vok:
  mov [edi].chVol,al
  mov [edi].chFinalVol,al
  ret
endp
else
  procfvolup=procnothing
  procfvoldn=procnothing
endif

if USEVVOL
procvvol proc
procvvol4:
  mov [ebp].procvol,10h
procvvol3:
  add [ebp].procvol,10h
procvvol2:
  add [ebp].procvol,10h
procvvol1:
  add [ebp].procvol,10h
procvvol0:
  mov al,[ebp].procvol
  mov [edi].chVol,al
  mov [edi].chFinalVol,al
  ret
endp
else
  procvvol0=procnothing
  procvvol1=procnothing
  procvvol2=procnothing
  procvvol3=procnothing
  procvvol4=procnothing
endif

if USEVPANSLIDE or USEVVOLSLIDE
procvvpsl proc
  mov al,[ebp].procvol
  mov [edi].chVVolPanSlideVal,al
  ret
endp
else
  procvvpsl=procnothing
endif

if USEVVOLSLIDE
dovvolsld proc
  mov al,[edi].chVol
  cmp [ebp].tick0,0
  jne @@done
  sub al,[edi].chVVolPanSlideVal
  jnc @@done
    mov al,0
@@done:
  mov [edi].chVol,al
  mov [edi].chFinalVol,al
  ret
endp

dovvolslu proc
  mov al,[edi].chVol
  cmp [ebp].tick0,0
  jne @@done
  add al,[edi].chVVolPanSlideVal
  cmp al,40h
  jbe @@done
    mov al,40h
@@done:
  mov [edi].chVol,al
  mov [edi].chFinalVol,al
  ret
endp
else
  dovvolsld=procnothing
  dovvolslu=procnothing
endif

if USEVPANSLIDE
dovpansll proc
  mov al,[edi].chPan
  cmp [ebp].tick0,0
  jne @@done
  sub al,[edi].chVVolPanSlideVal
  jnc @@done
    mov al,0
@@done:
  mov [edi].chPan,al
  mov [edi].chFinalPan,al
  ret
endp

dovpanslr proc
  mov al,[edi].chVol
  cmp [ebp].tick0,0
  jne @@done
  add al,[edi].chVVolPanSlideVal
  jnc @@done
    mov al,0ffh
@@done:
  mov [edi].chPan,al
  mov [edi].chFinalPan,al
  ret
endp
else
  dovpansll=procnothing
  dovpanslr=procnothing
endif

if USEVFVOLSLIDE
procvfvolup proc
  mov al,[edi].chVol
  add al,[ebp].procvol
  cmp al,40h
  jbe @@vok
    mov al,40h
@@vok:
  mov [edi].chVol,al
  mov [edi].chFinalVol,al
  ret
endp

procvfvoldn proc
  mov al,[edi].chVol
  sub al,[ebp].procvol
  jnc @@vok
    mov al,0
@@vok:
  mov [edi].chVol,al
  mov [edi].chFinalVol,al
  ret
endp
else
  procvfvolup=procnothing
  procvfvoldn=procnothing
endif

if USESYNC
procsync proc
  mov al,[ebp].procdat
  mov [ebp].syncval,al
  ret
endp
else
  procsync=procnothing
endif


if USETREMOR
proctremor proc
  movzx eax,[ebp].procdat
  cmp al,0
  je @@reuse
    shl eax,4
    shr al,4
    inc al
    inc ah
    add al,ah
    mov [edi].chTremorLen,al
    mov [edi].chTremorOff,ah
    mov [edi].chTremorPos,0
@@reuse:
  ret
endp

dotremor proc
  mov al,[edi].chTremorPos
  cmp al,[edi].chTremorOff
  jb @@on
    mov [edi].chFinalVol,0
@@on:
  cmp [ebp].tick0,0
  jne @@done
  mov al,[edi].chTremorPos
  inc al
  cmp al,[edi].chTremorLen
  jb @@noloop
    xor al,al
@@noloop:
  mov [edi].chTremorPos,al
@@done:
  ret
endp
else
  proctremor=procnothing
  dotremor=procnothing
endif



if USEENVPOS
procenvpos proc
  cmp [edi].chEnvIns,0
  je @@noenvins
  mov ebx,[edi].chEnvIns

  xor eax,eax
  movzx edx,[ebp].procdat
  jmp @@venvloops
@@venvloop:
    sub dx,[ebx].insVEnv[4*eax]
    jb @@venvok
    inc eax
@@venvloops:
  cmp al,[ebx].insVNum
  jne @@venvloop
  xor edx,edx
  sub dx,[ebx].insVEnv[4*eax]
@@venvok:
  add dx,[ebx].insVEnv[4*eax]
  mov [edi].chVolEnvPos,eax
  mov [edi].chVolEnvSegPos,dx

  xor eax,eax
  movzx edx,[ebp].procdat
  jmp @@penvloops
@@penvloop:
    sub dx,[ebx].insPEnv[4*eax]
    jb @@penvok
    inc eax
@@penvloops:
  cmp al,[ebx].insPNum
  jne @@venvloop
  xor edx,edx
  sub dx,[ebx].insPEnv[4*eax]
@@penvok:
  add dx,[ebx].insPEnv[4*eax]
  mov [edi].chPanEnvPos,eax
  mov [edi].chPanEnvSegPos,dx

@@noenvins:
  ret
endp
else
  procenvpos=procnothing
endif




if USEPORTA
procportau proc
  movzx eax,[ebp].procdat
  cmp al,0
  je @@reuse
    shl eax,4
    mov [edi].chPortaUVal,eax
@@reuse:
  ret
endp

procportad proc
  movzx eax,[ebp].procdat
  cmp al,0
  je @@reuse
    shl eax,4
    mov [edi].chPortaDVal,eax
@@reuse:
  ret
endp

doportau proc
  cmp [ebp].tick0,0
  jne @@done
  mov eax,[edi].chPitch
  sub eax,[edi].chPortaUVal
  call freqrange
  mov [edi].chPitch,eax
  mov [edi].chFinalPitch,eax
@@done:
  ret
endp

doportad proc
  cmp [ebp].tick0,0
  jne @@done
  mov eax,[edi].chPitch
  add eax,[edi].chPortaDVal
  call freqrange
  mov [edi].chPitch,eax
  mov [edi].chFinalPitch,eax
@@done:
  ret
endp
else
  procportau=procnothing
  procportad=procnothing
  doportau=procnothing
  doportad=procnothing
endif

if USEPORTANOTE
procportanote proc
  movzx eax,[ebp].procdat
  cmp al,0
  je @@reuse
    shl eax,4
    mov [edi].chPortaToVal,eax
@@reuse:
  ret
endp
else
  procportanote=procnothing
endif

if USEVPORTANOTE
procvportanote proc
  movzx eax,[ebp].procvol
  cmp al,0
  je @@reuse
    shl eax,8
    mov [edi].chPortaToVal,eax
@@reuse:
  ret
endp
else
  procvportanote=procnothing
endif


if USEGLISSANDO and (USEPORTANOTE or USEVPORTANOTE)
procgliss proc
  mov al,[ebp].procdat
  mov [edi].chGlissando,al
  ret
endp
else
  procgliss=procnothing
endif

if USEPORTANOTE or USEVPORTANOTE
doportanote proc
  mov eax,[edi].chPitch
  cmp [ebp].tick0,0
  jne @@set
  cmp eax,[edi].chPortaToPitch
  je @@set
  jg @@down
    add eax,[edi].chPortaToVal
    cmp eax,[edi].chPortaToPitch
    jle @@set
    mov eax,[edi].chPortaToPitch
    jmp @@set
@@down:
    sub eax,[edi].chPortaToVal
    cmp eax,[edi].chPortaToPitch
    jge @@set
    mov eax,[edi].chPortaToPitch
@@set:
  mov [edi].chPitch,eax

if USEGLISSANDO
  cmp [edi].chGlissando,0
  je @@setfinpitch
if USEBOTHFREQ
    test byte ptr [ebp].head.hdOptions,1
    jz @@amiga
endif
if USELINEARFREQ
      movzx ebx,[edi].chCurNormNote
      add eax,ebx
      add eax,80h
      xor al,al
      sub eax,ebx
endif
if USEBOTHFREQ
      jmp @@setfinpitch
endif
if USEAMIGAFREQ
  @@amiga:
      mov edx,eax ;// search for closest note
      mov ebx,eax ;// how should i do it??
      push ecx
      mov ecx,-1
      mov eax,-48*256
    @@aloop:
        push eax
        add ax,[edi].chCurNormNote
        movsx eax,ax
        call getfreq6848
        sub eax,edx
        jae @@apos
          neg eax
      @@apos:
        cmp eax,ecx
        jae @@aold
          mov ecx,eax
          mov eax,[esp]
          add ax,[edi].chCurNormNote
          movsx eax,ax
          call getfreq6848
          mov ebx,eax
      @@aold:
        pop eax
      inc ah
      cmp ah,48
      jne @@aloop
      pop ecx
      mov eax,ebx
endif
endif

@@setfinpitch:
  mov [edi].chFinalPitch,eax
@@done:
  ret
endp
else
  doportanote=procnothing
endif


if USEVIBTYPE and (USEVIBRATO or USEVVIBRATO)
procvibtype proc
  mov al,[ebp].procdat
  and al,3
  mov [edi].chVibType,al
  ret
endp
else
  procvibtype=procnothing
endif

if USEVIBRATO
procvibrato proc
  mov al,[ebp].procdat
  and al,0Fh
  jz @@reusel
    shl al,2
    mov [edi].chVibDep,al
@@reusel:
  mov al,[ebp].procdat
  and al,0F0h
  jz @@reuseh
    shr al,2
    mov [edi].chVibRate,al
@@reuseh:
  ret
endp
else
  procvibrato=procnothing
endif

if USEVVIBRATE
procvvibrat proc
  mov al,[ebp].procvol
  shl al,2
  je @@reuse
    mov [edi].chVibRate,al
@@reuse:
  ret
endp
else
  procvvibrat=procnothing
endif

if USEVVIBRATO
procvvib proc
  mov al,[ebp].procvol
  shl al,2
  je @@reuse
    mov [edi].chVibDep,al
@@reuse:
  ret
endp
else
  procvvib=procnothing
endif

if USEVIBRATO or USEVVIBRATO
dovibrato proc
  movzx eax,word ptr [edi].chVibPos
  movsx eax,[ebp].vibtabs[eax]
  imul [edi].chVibDep
  sar eax,3
  add eax,[edi].chFinalPitch
  call freqrange
  mov [edi].chFinalPitch,eax

  cmp [ebp].tick0,0
  jne @@done
  mov al,[edi].chVibRate
  add [edi].chVibPos,al
@@done:
  ret
endp
else
  dovibrato=procnothing
endif

if USEVOLSLIDE or USEVIBRATOVOL or USEPORTAVOL
procvolsl proc
  mov al,[ebp].procdat
  cmp al,0
  je @@reuse
    mov [edi].chVolSlideVal,al
@@reuse:
  ret
endp

dovolsl proc
  mov bl,[edi].chVolSlideVal
  mov al,[edi].chVol
  cmp [ebp].tick0,0
  jne @@done
  test bl,0f0h
  jnz @@up
    sub al,bl
    jnc @@done
      mov al,0
    jmp @@done
@@up:
    shr bl,4
    add al,bl
    cmp al,40h
    jbe @@done
      mov al,40h
@@done:
  mov [edi].chVol,al
  mov [edi].chFinalVol,al
  ret
endp
else
  procvolsl=procnothing
  dovolsl=procnothing
endif


if USEVIBRATOVOL and USEVIBRATO
dovibvol proc
  call dovibrato
  jmp dovolsl
endp
else
  dovibvol=procnothing
endif

if USEPORTAVOL and USEPORTANOTE
doportavol proc
  call doportanote
  jmp dovolsl
endp
else
  doportavol=procnothing
endif

if USEGVOLSLIDE
procgvolsl proc
  mov al,[ebp].procdat
  cmp al,0
  je @@reuse
    mov [edi].chGVolSlideVal,al
@@reuse:
  ret
endp

dogvolsl proc
  mov bl,[edi].chGVolSlideVal
  mov al,[ebp].globalvol
  cmp [ebp].tick0,0
  jne @@done
  test bl,0f0h
  jnz @@up
    sub al,bl
    jnc @@done
      mov al,0
    jmp @@done
@@up:
    shr bl,4
    add al,bl
    cmp al,40h
    jbe @@done
      mov al,40h
@@done:
  mov [ebp].globalvol,al
  ret
endp
else
  procgvolsl=procnothing
  dogvolsl=procnothing
endif

if USEPANSLIDE
procpansl proc
  mov al,[ebp].procdat
  cmp al,0
  je @@reuse
    mov [edi].chPanSlideVal,al
@@reuse:
  ret
endp

dopansl proc
  mov bl,[edi].chPanSlideVal
  mov al,[edi].chPan
  cmp [ebp].tick0,0
  jne @@done
  test bl,0f0h
  jnz @@left
    add al,bl
    jnc @@done
      mov al,0ffh
    jmp @@done
@@left:
    shr bl,4
    sub al,bl
    jnc @@done
      mov al,0
@@done:
  mov [edi].chPan,al
  mov [edi].chFinalPan,al
  ret
endp
else
  procpansl=procnothing
  dopansl=procnothing
endif

if USEDELAY or USEKEYOFFCMD or USENOTECUT
proctick proc
  mov al,[ebp].procdat
  mov [edi].chActionTick,al
  ret
endp
else
  proctick=procnothing
endif

if USEDELAY
dodelay proc
  cmp [ebp].tick0,0
  jne @@done
  mov al,[ebp].curtick
  cmp al,[edi].chActionTick
  jne @@done
    mov [ebp].notedelayed,1
    mov al,[edi].chDelayNote
    mov [ebp].procnot,al
    mov al,[edi].chCurIns
    mov [ebp].procins,al
    mov [ebp].proccmd,0ffh
    mov [ebp].procvol,0
    call PlayNote
@@done:
  ret
endp
else
  dodelay=procnothing
endif

if USEKEYOFFCMD
dokeyoff proc
  cmp [ebp].tick0,0
  jne @@done
  mov al,[ebp].curtick
  cmp al,[edi].chActionTick
  jne @@done
    mov [edi].chSustain,0
@@done:
  ret
endp
else
  dokeyoff=procnothing
endif

if USENOTECUT
donotecut proc
  cmp [ebp].tick0,0
  jne @@done
  mov al,[ebp].curtick
  cmp al,[edi].chActionTick
  jne @@done
    mov [edi].chVol,0
    mov [edi].chFinalVol,0
@@done:
  ret
endp
else
  donotecut=procnothing
endif

if USERETRIG
doretrig proc
  cmp [edi].chActionTick,0
  je @@done
    movzx eax,[ebp].curtick
    div [edi].chActionTick
    cmp ah,0
    jne @@done
      mov [edi].chGUSNextPos,0
@@done:
  ret
endp
else
  doretrig=procnothing
endif

if USEMRETRIG
procmretrig proc
  movzx eax,[ebp].procdat
  cmp al,0
  je @@reuse
    shl eax,4
    shr al,6
    mov [edi].chMRetrigLen,al
    mov [edi].chMRetrigAct,ah
    mov [edi].chMRetrigPos,0
@@reuse:
  ret
endp

domretrig proc
  mov al,[edi].chMRetrigPos
  inc [edi].chMRetrigPos
  cmp al,[edi].chMRetrigLen
  jne @@done
    mov [edi].chMRetrigPos,0
    mov [edi].chGUSNextPos,0

    mov al,[edi].chVol
    mov bl,[edi].chMRetrigAct
    mov ah,128
    xchg bl,cl
    rol ah,cl
    xchg bl,cl
    test bl,7
    jz @@done
    test bl,8
    jnz @@up
    cmp bl,5
    ja @@nosub
      sub al,ah
  @@nosub:
    cmp bl,6
    jne @@not6
      mov ah,al
      shr al,2
      add al,ah
      shr al,1
  @@not6:
    cmp bl,7
    jne @@setvol
      shr al,1
    jmp @@setvol

  @@up:
    cmp bl,13
    ja @@noadd
      add al,ah
  @@noadd:
    cmp bl,14
    jne @@not14
      mov ah,al
      shr al,1
      add al,ah
  @@not14:
    cmp bl,15
    jne @@setvol
      shl al,1
      jns @@setvol
        dec al

  @@setvol:
    cmp al,0
    jge @@lok
      mov al,0
  @@lok:
    cmp al,40h
    jbe @@tok
      mov al,40h
  @@tok:

    mov [edi].chVol,al
    mov [edi].chFinalVol,al
@@done:
  ret
endp
else
  procmretrig=procnothing
  domretrig=procnothing
endif





;//***************************************************************************
;//effects end

callproccmdtab proc
  call @@getadr
@@getadr:
  pop ebx
  mov eax,[ebx+proccmdtab[4*eax]-@@getadr]
  lea eax,[eax+ebx+procnothing-@@getadr]
  jmp eax
endp



PlayTick proc
  mov [ebp].tick0,0
  lea edi,[ebp].chandata
  xor ecx,ecx
@@resetvalloop:
    mov al,[edi].chVol
    mov [edi].chFinalVol,al
    mov al,[edi].chPan
    mov [edi].chFinalPan,al
    mov eax,[edi].chPitch
    mov [edi].chFinalPitch,eax
  add edi,channelsize
  inc ecx
  cmp ecx,[ebp].head.hdNChannels
  jne @@resetvalloop

  inc [ebp].curtick
  mov al,[ebp].curtick
  cmp al,[ebp].curtempo
  jne @@notnextrow

    mov [ebp].curtick,0
    cmp [ebp].patdelay,0
    jz @@nextrow
      dec [ebp].patdelay
      jmp @@notnextrow
  @@nextrow:
    mov [ebp].tick0,1

    inc [ebp].currow
    cmp [ebp].jumptoord,-1
    jne @@dojump
    mov eax,[ebp].currow
    cmp eax,[ebp].patlen
    jb @@donotjump
      mov eax,[ebp].curord
      inc eax
      mov [ebp].jumptoord,eax
      mov [ebp].jumptorow,0
  @@dojump:

      mov eax,[ebp].jumptoord
      cmp [ebp].curord,eax
      je @@noresetploop
        lea edi,[ebp].chandata
        xor ecx,ecx
      @@resetplloop:
          mov [edi].chPatLoopCount,0
          mov [edi].chPatLoopStart,0
          add edi,channelsize
        inc ecx
        cmp ecx,[ebp].head.hdNChannels
        jne @@resetplloop
    @@noresetploop:

      mov eax,[ebp].jumptoord
      cmp eax,[ebp].head.hdNOrders
      jb @@dontloop
        mov eax,[ebp].head.hdOrdLoopStart
    @@dontloop:
      mov [ebp].curord,eax
      mov eax,[ebp].jumptorow
      mov [ebp].currow,eax
      mov [ebp].jumptoord,-1
      mov eax,[ebp].curord
      movzx eax,[ebp].head.hdOrderTable[eax]
      mov esi,[ebp].head.hdPatternTable[4*eax]
      lodsd
      mov [ebp].patlen,eax
      cmp [ebp].jumptorow,0
      je @@rowfound
    @@rowfind:
      @@chanskip:
          lodsb
          cmp al,0
          je @@rowend

          test al,20h
          jz @@not20
            add esi,2
        @@not20:
          test al,40h
          jz @@not40
            inc esi
        @@not40:
          test al,80h
          jz @@chanskip
            add esi,2
          jmp @@chanskip
      @@rowend:

      dec [ebp].jumptorow
      jnz @@rowfind
    @@rowfound:
      mov [ebp].patptr,esi
  @@donotjump:

    mov esi,[ebp].patptr
    lea edi,[ebp].chandata
    xor ecx,ecx
  @@processrow:
      mov [ebp].procnot,0
      mov dword ptr [ebp].procins,0
      mov [edi].chCommand,0ffh

      mov al,[esi]
      cmp al,0
      je @@procnextchan

      and al,1fh
      cmp al,cl
      jne @@procnextchan

      lodsb
      mov ah,al
      test ah,20h
      jz @@nonot
        lodsb
        mov [ebp].procnot,al
        lodsb
        mov [ebp].procins,al
    @@nonot:
      test ah,40h
      jz @@novol
        lodsb
        mov [ebp].procvol,al
    @@novol:
      test ah,80h
      jz @@nocmd
        lodsb
        mov [ebp].proccmd,al
        lodsb
        mov [ebp].procdat,al
    @@nocmd:
    @@procnote:
      mov [ebp].notedelayed,0
      call PlayNote

if USEVOLCOL
      movzx eax,[ebp].procvol
      and [ebp].procvol,0fh
      shr eax,4
      mov [edi].chVCommand,al
      add eax,(procvoltab-proccmdtab)/4
      call callproccmdtab
endif

      movzx eax,[ebp].proccmd
      cmp al,52
      jae @@procnextchan
        mov [edi].chCommand,al
        call callproccmdtab

    @@procnextchan:
      add edi,channelsize
    inc ecx
    cmp ecx,[ebp].head.hdNChannels
    jne @@processrow
    inc esi
    mov [ebp].patptr,esi

@@notnextrow:

  lea edi,[ebp].chandata
  xor ecx,ecx
@@dotickloop:
if USEVOLCOL
;//process volume column
    movzx eax,[edi].chVCommand
    add eax,(dovoltab-proccmdtab)/4
    call callproccmdtab
endif

;//process command
    movzx eax,[edi].chCommand
    cmp al,52
    jae @@donocmd
      add eax,(docmdtab-proccmdtab)/4
      call callproccmdtab
  @@donocmd:

    mov ebx,[edi].chEnvIns
    cmp ebx,0
    je @@noenvins

;//process fadeout
    movzx eax,[edi].chFinalVol
    mul [ebp].uservol
    shr eax,6
    mul [ebp].globalvol
    mul [edi].chFadeVol
    shr edx,4
    mov [edi].chFinalVol,dl

    cmp [edi].chSustain,0
    jne @@sustain
      mov ax,[ebx].insVolFade
      sub [edi].chFadeVol,ax
      jnb @@sustain
        mov [edi].chFadeVol,0
  @@sustain:

if USEVOLENV
;//process volume envelope
    mov eax,[edi].chVolEnvPos
    cmp [edi].chVolEnvSegPos,0
    je @@vnoloop
    cmp al,[ebx].insVLoopE
    jne @@vnoloop
      mov al,[ebx].insVLoopS
      mov [edi].chVolEnvPos,eax
  @@vnoloop:
    lea esi,[ebx].insVEnv[4*eax]
    cmp al,[ebx].insVNum
    je @@venvlast
      mov ax,[esi][4][2]
      mov dx,[esi][0][2]
      sub eax,edx
      imul [edi].chVolEnvSegPos
      idiv word ptr [esi][0][0]
      add al,byte ptr [esi][0][2]
      mul [edi].chFinalVol
      shr eax,6
      mov [edi].chFinalVol,al

      mov ax,[edi].chVolEnvSegPos
      cmp ax,0
      jne @@vnosustain
      cmp [edi].chSustain,0
      je @@vnosustain
        mov edx,[edi].chVolEnvPos
        cmp dl,[ebx].insVSustain
        je @@venvnostep
    @@vnosustain:
      inc eax
      cmp ax,[esi][0][0]
      jb @@venvnostep
        xor eax,eax
        inc [edi].chVolEnvPos
    @@venvnostep:
      mov [edi].chVolEnvSegPos,ax
      jmp @@venvend

  @@venvlast:
      mov al,byte ptr [esi][0][2]
      mul [edi].chFinalVol
      shr eax,6
      mov [edi].chFinalVol,al
  @@venvend:
endif

if USEPANENV
;//process panning envelope
    mov eax,[edi].chPanEnvPos
    cmp [edi].chPanEnvSegPos,0
    je @@pnoloop
    cmp al,[ebx].insPLoopE
    jne @@pnoloop
      mov al,[ebx].insPLoopS
      mov [edi].chPanEnvPos,eax
  @@pnoloop:
    lea esi,[ebx].insPEnv[4*eax]
    cmp al,[ebx].insPNum
    je @@penvlast
      mov ax,[esi][4][2]
      mov dx,[esi][0][2]
      sub eax,edx
      imul [edi].chPanEnvSegPos
      idiv byte ptr [esi][0][0]
      add al,byte ptr [esi][0][2]
      sub al,32
      movsx edx,[edi].chFinalPan
      xor dl,dh
      imul dl
      shr eax,5
      add [edi].chFinalPan,al

      mov ax,[edi].chPanEnvSegPos
      cmp ax,0
      jne @@pnosustain
      cmp [edi].chSustain,0
      je @@pnosustain
        mov edx,[edi].chPanEnvPos
        cmp dl,[ebx].insPSustain
        je @@penvnostep
    @@pnosustain:
      inc eax
      cmp ax,[esi][0][0]
      jb @@penvnostep
        xor eax,eax
        inc [edi].chPanEnvPos
    @@penvnostep:
      mov [edi].chPanEnvSegPos,ax
      jmp @@penvend

  @@penvlast:
      mov al,byte ptr [esi][0][2]
      sub al,32
      movsx edx,[edi].chFinalPan
      xor dl,dh
      imul dl
      shr eax,5
      add [edi].chFinalPan,al
  @@penvend:
endif

if USEAUTOVIBRATO
;//process auto vibrato
    movzx eax,[edi].chAVibPos
    mov ah,[ebx].insVibType
    mov al,[ebp].vibtabs[eax]
    imul [ebx].insVibDepth
    shr eax,4

    mov dl,[edi].chAVibSwpPos
    cmp dl,[ebx].insVibSweep
    jae @@nosweep
      imul dl
      idiv [ebx].insVibSweep
      inc [edi].chAVibSwpPos
  @@nosweep:

    neg eax
    movsx eax,al
    add eax,[edi].chFinalPitch
    call freqrange
    mov [edi].chFinalPitch,eax

    mov al,[ebx].insVibRate
    add [edi].chAVibPos,al
endif

  @@noenvins:

;//conv vals for gus
    movzx eax,[edi].chFinalVol
    shl eax,1
    mov ax,[ebp].guslinvol[2*eax]
    mov [edi].chGUSVol,ax

    mov al,[edi].chFinalPan
    shr al,4
    mov [edi].chGUSPan,al

    mov eax,[edi].chFinalPitch
if USEBOTHFREQ
    test byte ptr [ebp].head.hdOptions,1
    jz @@amiga
endif
if USELINEARFREQ
      call getfreq6848
      mul [ebp].guschannels
      mov ebx,494
      div ebx
endif
if USEBOTHFREQ
      jmp @@noamiga
endif
if USEAMIGAFREQ
  @@amiga:
      cmp eax,100
      jb @@noamiga
      mov ebx,eax
      mov eax,94929
      mul [ebp].guschannels
      div ebx
  @@noamiga:
endif

    and al,not 1
    mov [edi].chGUSFrq,ax

  add edi,channelsize
  inc ecx
  cmp ecx,[ebp].head.hdNChannels
  jne @@dotickloop

  ret
endp





public xmpPlay_
xmpPlay_ proc
  push ebp
  call loadebp

  mov [ebp].jumptoord,eax
  mov [ebp].curord,eax
  xor eax,eax
  mov [ebp].currow,eax

  call gusOpen

  lea edi,[ebp].chandata
  mov ecx,channelsize*8
  xor eax,eax
  rep stosd

  lea edi,[ebp].chandata
  xor ecx,ecx
@@panloop:
    mov al,[ebp].head.hdPanPos[ecx]
    mov [edi].chPan,al
    add edi,channelsize
  inc ecx
  cmp cl,32
  jne @@panloop

  xor eax,eax
  mov [ebp].globalvol,40h
  mov [ebp].jumptorow,eax
  mov [ebp].syncval,al

  mov al,[ebp].head.hdIniTempo
  mov [ebp].curtempo,al
  dec al
  mov [ebp].curtick,al

  mov eax,2983615
  xor edx,edx
  movzx ebx,[ebp].head.hdIniBPM
  div ebx
  mov [ebp].stimerlen,eax
  call tmInit
  pop ebp
  ret
endp

public xmpStop_
xmpStop_ proc
  push ebp
  call loadebp
  call tmClose
  call gusClose
  pop ebp
  ret
endp



;//*************************************************************************
;// system timer



tmTimerHandler proc
  pushad
  push ds
  push es
  push fs
  push gs

  call loadebp
  mov esi,ebp

  cld
  mov ds,cs:[esi].datasegsel
  mov es,[esi].datasegsel

  mov eax,[esi].tmTimerRate
  add [esi].tmTicker,eax

  add [esi].tmIntCount,eax
  cmp byte ptr [esi].tmIntCount+2,0
  je @@noorgcall
    mov byte ptr [esi].tmIntCount+2,0
    pushfd
    call [esi].tmOldTimer
@@noorgcall:

  mov al,20h
  out 20h,al

  mov ebx,65536;[esi].maxtimerrate
  mov ecx,[esi].stimerlen
  mov edx,[esi].stimerpos

  sub edx,ebx
  ja @@another
    mov edx,ecx
@@another:

  mov [esi].stimerpos,edx

  cmp edx,ebx
  jb @@intervalok
    mov edx,ebx
@@intervalok:
  mov [esi].tmTimerRate,edx
  mov al,34h
  out 43h,al
  mov al,dl
  out 40h,al
  mov al,dh
  out 40h,al

  cmp [esi].stimerpos,ecx
  jne @@notick
  cmp [esi].tmInRoutine,0
  jnz @@notick
    mov [esi].tmInRoutine,1

    mov eax,[esi].tmTicker
    mov [esi].maxtimerrate,eax

    mov dword ptr [esi].tmOldSSESP,esp
    mov word ptr [esi].tmOldSSESP+4,ss
    push ds
    pop ss
    lea esp,[esi].tmStack+1024
    sti
    call gusPlayTick
    call PlayTick
    cli
    lss esp,[ebp].tmOldSSESP
    mov ds:[ebp].tmInRoutine,0
@@notick:

  pop gs
  pop fs
  pop es
  pop ds
  popad
  iretd
endp




tmInit proc
  mov [ebp].datasegsel,ds

  xor eax,eax
  mov [ebp].stimerpos,eax
  mov [ebp].tmIntCount,eax
  mov [ebp].tmInRoutine,0
  mov [ebp].tmTimerRate,65536
  mov eax,-65536
  sub eax,[ebp].stimerlen
  mov [ebp].tmTicker,eax

  push es
  mov ax,3508h
  int 21h
  mov dword ptr [ebp].tmOldTimer+0,ebx
  mov word ptr [ebp].tmOldTimer+4,es
  pop es

  push ds
  mov ax,2508h
  push cs
  pop ds
  call @@getadr
@@getadr:
  pop edx
  add edx,tmTimerHandler-@@getadr
  int 21h
  pop ds

  mov al,34h
  out 43h,al
  mov eax,[ebp].tmTimerRate
  out 40h,al
  mov al,ah
  out 40h,al
  ret
endp


tmClose proc
  push ds
  mov ax,2508h
  lds edx,[ebp].tmOldTimer
  int 21h
  pop ds

  mov al,34h
  out 43h,al
  xor al,al
  out 40h,al
  out 40h,al
  ret
endp

public xmpGetTimer_
xmpGetTimer_ proc
  pushf
  xor eax,eax
  cli
  out 43h,al
  in al,40h
  mov ah,al
  in al,40h
  popf
  xchg ah,al
  neg eax
  push ebp
  call loadebp
  add eax,[ebp].tmTimerRate
  add eax,[ebp].tmTicker
  pop ebp
  ret
endp





;//*************************************************************************
;// GUS

gusoutp proc
  add edx,[ebp].gusport
  out dx,al
  sub edx,[ebp].gusport
  ret
endp

gusout proc
  push edx
  mov dx,103h
  xchg al,bl
  call gusoutp
  add dl,2
  xchg al,bl
  call gusoutp
  pop edx
  ret
endp

gusoutd proc
  push edx
  mov dx,103h
  xchg al,bl
  call gusoutp
  add dl,2
  xchg al,bl
  call gusoutp
  call gusdelay
  call gusoutp
  pop edx
  ret
endp

gusoutw proc
  push edx
  mov dx,103h
  xchg al,bl
  call gusoutp
  inc edx
  xchg al,bl
  add edx,[ebp].gusport
  out dx,ax
  pop edx
  ret
endp

gusin proc
  push edx
  mov dx,103h
  xchg al,bl
  call gusoutp
  add dl,2
  xchg al,bl
  add edx,[ebp].gusport
  in al,dx
  pop edx
  ret
endp

gusinw proc
  push edx
  mov dx,103h
  xchg al,bl
  call gusoutp
  inc edx
  xchg al,bl
  add edx,[ebp].gusport
  in ax,dx
  pop edx
  ret
endp

gusdelay proc
  push edx
  push eax
  mov dx,107h
  add edx,[ebp].gusport
  in al,dx
  in al,dx
  in al,dx
  in al,dx
  in al,dx
  in al,dx
  in al,dx
  pop eax
  pop edx
  ret
endp

gusselchn proc
  push edx
  mov dx,102h
  xchg al,cl
  call gusoutp
  xchg al,cl
  pop edx
  ret
endp


gussetpoint proc
  push eax
  shr eax,7
  and eax,1FFFh
  call gusoutw
  mov eax,[esp]
  inc bl
  shl eax,9
  call gusoutw
  dec bl
  pop eax
  ret
endp

gusopenchan proc
  cmp al,14
  jae @@chanok
    mov al,14
@@chanok:
  mov [ebp].guschannels,eax

  mov bl,04ch
  mov al,0
  call gusout
  call gusdelay
  call gusdelay
  inc al
  call gusout
  call gusdelay
  call gusdelay

  mov bl,0eh
  mov al,byte ptr [ebp].guschannels
  dec al
  or al,0c0h
  call gusout

  xor ecx,ecx
@@initloop:
    call gusselchn
    xor ax,ax
    mov bl,9
    call gusoutw
    mov al,3
    mov bl,0
    call gusoutd
    mov al,3
    mov bl,0dh
    call gusoutd
    mov al,3fh
    mov bl,6
    call gusout
    call gusdelay
  inc ecx
  cmp ecx,32
  jne @@initloop

  mov bl,4ch
  mov al,7
  call gusout

  mov cl,0
  call gusselchn
  xor edx,edx
  mov al,8
  call gusoutp
  ret
endp






gusfadevol proc
  mov bl,89h
  call gusinw
  movzx eax,ax
  mov bl,0
  cmp eax,edx
  jbe @@up
    xchg eax,edx
    mov bl,40h
@@up:

  sub edx,eax
  jz @@done
  cmp edx,4096
  jae @@normfade
    cmp bl,40h
    je @@swapped
      add eax,edx
  @@swapped:
    mov bl,9
    call gusoutw
    jmp @@done

@@normfade:
  add edx,eax

  cmp eax,4096
  jae @@stok
    mov eax,4096
@@stok:
  cmp edx,64512
  jbe @@eok
    mov edx,64512
@@eok:

  push ebx
  mov al,ah
  mov bl,7
  call gusout
  mov al,dh
  mov bl,8
  call gusout
  pop eax
  mov bl,0dh
  call gusoutd

@@done:
  ret
endp

gusfadevoldown proc
  push ebx
  push eax
  mov bl,07h
  mov al,04h
  call gusout
  mov bl,08h
  mov al,0fch
  call gusout
  mov bl,0dh
  mov al,40h
  call gusoutd
  pop eax
  pop ebx
  ret
endp


gusPlayTick proc
  cmp [ebp].gusport,0
  je @@nogus

  lea edi,[ebp].chandata
  xor ecx,ecx
@@clearloop:
    cmp [edi].chGUSInited,0
    je @@nostop
    cmp [edi].chGUSStopIt,0
    je @@nostop
      call gusselchn
      mov bl,0
      mov al,[edi].chGUSMode
      or al,3
      call gusoutd
      call gusfadevoldown
      mov [edi].chGUSStopIt,0
  @@nostop:
  add edi,channelsize
  inc ecx
  cmp ecx,[ebp].head.hdNChannels
  jne @@clearloop

  xor ecx,ecx
@@waitloop:
    call gusselchn
    mov bl,8dh
  @@dowait:
      call gusin
      test al,1
    jz @@dowait
  inc ecx
  cmp ecx,[ebp].head.hdNChannels
  jne @@waitloop

  lea edi,[ebp].chandata
  xor ecx,ecx
@@playloop:
    call gusselchn
    cmp [edi].chGUSInited,0
    je @@quiet
      cmp [edi].chGUSChangeSamp,0
      je @@samesample
        mov bl,2
        mov eax,[edi].chGUSLoopPos
        call gussetpoint
        mov bl,4
        mov eax,[edi].chGUSEndPos
        call gussetpoint
    @@samesample:

      cmp [edi].chGUSNextPos,-1
      je @@noposchange
        mov eax,[edi].chGUSNextPos
        add eax,[edi].chGUSStartPos

        cmp eax,[edi].chGUSEndPos
        jb @@posok
          mov eax,[edi].chGUSEndPos
          dec eax
      @@posok:
        mov bl,10
        call gussetpoint
        mov bl,80h
        call gusin
        mov bl,0
        and al,40h
        or al,[edi].chGUSMode
        call gusout

    @@noposchange:
      mov bl,80h
      call gusin
      test bl,1
      jnz @@quiet
        mov bl,0ch
        mov al,[edi].chGUSPan
        call gusout
        mov bl,1
        mov ax,[edi].chGUSFrq
        call gusoutw
        movzx edx,[edi].chGUSVol
        call gusfadevol
      jmp @@nextchan

  @@quiet:
      call gusfadevoldown

  @@nextchan:
    mov [edi].chGUSChangeSamp,0
    mov [edi].chGUSNextPos,-1
  add edi,channelsize
  inc ecx
  cmp ecx,[ebp].head.hdNChannels
  jne @@playloop

@@nogus:
  ret
endp


gusOpen proc
  cmp [ebp].gusport,0
  je @@nogus
  mov eax,[ebp].head.hdNChannels
  call gusopenchan
  mov cl,0
  call gusselchn
  xor edx,edx
  mov al,9
  call gusoutp
@@nogus:
  ret
endp

gusClose proc
  cmp [ebp].gusport,0
  je @@nogus
  mov eax,14
  call gusopenchan
@@nogus:
  ret
endp


gusUploadSamples proc
  cmp [ebp].gusport,0
  je @@done
  shr ecx,4
  jz @@done
  xor ebx,ebx

@@bigloop:
    pushf
    cli
    mov edx,[ebp].gusport
    add dx,103h
    mov al,44h
    out dx,al
    add dl,2
    mov eax,ebx
    shr eax,16
    out dx,al
    sub dl,2
    mov al,43h
    out dx,al
    inc dl
    cli
  @@loop:
      mov eax,ebx
      out dx,ax
      lodsb
      add dl,3
      out dx,al
      sub dl,3
      inc ebx
    test bl,0fh
    jnz @@loop
    popf
  dec ecx
  jnz @@bigloop
@@done:
  ret
endp

gusGetPort proc
  push es
  mov es,ax
  mov es,es:[2Ch]
  xor edi,edi
  xor eax,eax
  xor ecx,ecx
  dec ecx
  jmp @@scanentry
@@envscan:
    repne scasb
  @@scanentry:
    cmp byte ptr es:[edi],0
    je @@done
    cmp dword ptr es:[edi],52544C55h
    jne @@envscan
    cmp dword ptr es:[edi+4],444E5341h
    jne @@envscan
    cmp word ptr es:[edi+8],323Dh
    jne @@envscan
    cmp word ptr es:[edi+11],2C30h
    jne @@envscan
    mov al,byte ptr es:[edi+10]
    sub al,30h
    shl al,4
    mov ah,2
@@done:
  pop es
  ret
endp

sintab db 0,2,3,5,6,8,9,11,12,14,16,17,19,20,22,23,24,26,27,29,30,32,33
       db 34,36,37,38,39,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56
       db 56,57,58,59,59,60,60,61,61,62,62,62,63,63,63,64,64,64,64,64
logfreqtab dw 32768,32761,32753,32746,32738,32731,32724,32716,32709,32702,32694,32687,32679,32672,32665,32657
           dw 32768,32650,32532,32415,32298,32182,32066,31950,31835,31720,31606,31492,31379,31266,31153,31041
           dw 32768,30929,29193,27554,26008,24548,23170,21870,20643,19484,18390,17358,16384,15464,14596,13777
           dd 11131415,4417505,1753088,695713,276094,109568,43482,17256,6848,2718,1078,428,170,67,27,11

proccmdtab label dword
  dd procarpeggio-procnothing
  dd procportau-procnothing
  dd procportad-procnothing
  dd procportanote-procnothing
  dd procvibrato-procnothing
  dd procvolsl-procnothing
  dd procvolsl-procnothing
  dd proctremolo-procnothing
  dd procpan-procnothing
  dd 0
  dd procvolsl-procnothing
  dd procjump-procnothing
  dd procnvol-procnothing
  dd procbreak-procnothing
  dd 0
  dd proctempo-procnothing
  dd procgvol-procnothing
  dd procgvolsl-procnothing
  dd 0
  dd 0
  dd proctick-procnothing
  dd procenvpos-procnothing
  dd 0
  dd 0
  dd 0
  dd procpansl-procnothing
  dd 0
  dd procmretrig-procnothing
  dd procsync-procnothing
  dd proctremor-procnothing
  dd 0
  dd 0
  dd 0
  dd procxfporta-procnothing
  dd 0
  dd 0

  dd 0
  dd procfportau-procnothing
  dd procfportad-procnothing
  dd procgliss-procnothing
  dd procvibtype-procnothing
  dd 0
  dd procpatloop-procnothing
  dd proctremtype-procnothing
  dd procspan-procnothing
  dd proctick-procnothing
  dd procfvolup-procnothing
  dd procfvoldn-procnothing
  dd proctick-procnothing
  dd proctick-procnothing
  dd procpatdelay-procnothing
  dd procsync-procnothing

docmdtab label dword
  dd doarpeggio-procnothing
  dd doportau-procnothing
  dd doportad-procnothing
  dd doportanote-procnothing
  dd dovibrato-procnothing
  dd doportavol-procnothing
  dd dovibvol-procnothing
  dd dotremolo-procnothing
  dd 0
  dd 0
  dd dovolsl-procnothing
  dd 0
  dd 0
  dd 0
  dd 0
  dd 0
  dd 0
  dd dogvolsl-procnothing
  dd 0
  dd 0
  dd dokeyoff-procnothing
  dd 0
  dd 0
  dd 0
  dd 0
  dd dopansl-procnothing
  dd 0
  dd domretrig-procnothing
  dd 0
  dd dotremor-procnothing
  dd 0
  dd 0
  dd 0
  dd 0
  dd 0
  dd 0

  dd 0
  dd 0
  dd 0
  dd 0
  dd 0
  dd 0
  dd 0
  dd 0
  dd 0
  dd doretrig-procnothing
  dd 0
  dd 0
  dd donotecut-procnothing
  dd dodelay-procnothing
  dd 0
  dd 0

if USEVOLCOL
procvoltab label dword
  dd 0
  dd procvvol0-procnothing
  dd procvvol1-procnothing
  dd procvvol2-procnothing
  dd procvvol3-procnothing
  dd procvvol4-procnothing
  dd procvvpsl-procnothing
  dd procvvpsl-procnothing
  dd procvfvoldn-procnothing
  dd procvfvolup-procnothing
  dd procvvibrat-procnothing
  dd procvvib-procnothing
  dd procvpan-procnothing
  dd procvvpsl-procnothing
  dd procvvpsl-procnothing
  dd procvportanote-procnothing

dovoltab label dword
  dd 0
  dd 0
  dd 0
  dd 0
  dd 0
  dd 0
  dd dovvolsld-procnothing
  dd dovvolslu-procnothing
  dd 0
  dd 0
  dd 0
  dd dovibrato-procnothing
  dd 0
  dd dovpansll-procnothing
  dd dovpanslr-procnothing
  dd doportanote-procnothing
endif

ends

end
