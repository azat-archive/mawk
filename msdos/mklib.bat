
masm /mx /z  rand48.asm ;
tcc -c -f -O -v-  rand48_0.c

lib  msdos.lib  -+rand48.obj -+rand48_0.obj  ;



