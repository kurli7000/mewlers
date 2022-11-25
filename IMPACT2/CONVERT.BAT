@echo off
..\cubmxm\xm2mxm.exe scom2.xm scom2.mxm -p5
copy /b shit+scom2.mxm musa
binobj32 musa. musa.obj mjuzic

pcx2raw2 pieniufo.pcx pieniufo.raw 50 80
binobj32 pieniufo.raw pieniufo.obj sprite

pcx2raw2 mewllogo.pcx mewllogo.raw 320 200
binobj32 mewllogo.raw mewllogo.obj logo1

pcx2raw2 implogo.pcx implogo.raw 320 200
binobj32 implogo.raw implogo.obj logo2

pcx2raw2 cow2.pcx cow2.raw 160 120
binobj32 cow2.raw cow.obj lehma



