@echo off
..\cubmxm\xm2mxm.exe glx-vrtx.xm glx-vrtx.mxm -p5
copy /b shit+glx-vrtx.mxm musa
binobj32 musa. musa.obj mjuzic

pcx2raw2 clif.pcx clif.raw 64 64
pak16 clif.raw temp.$$$
binobj32 temp.$$$ clif.obj tmap
del temp.$$$

rem pcx2raw2 acidmap1.pcx acidmap1.raw 128 128
rem binobj32 acidmap1.raw acidmap1.obj acidmap


