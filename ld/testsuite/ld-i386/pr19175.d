#source: pr19175.s
#as: --32 -mrelax-relocations=yes
#ld: -Bsymbolic -shared -melf_i386 -T pr19175.t
#objdump: -dw
#xfail: *-*-*
# XFAILed because generation of the R_386_GOT32X relocs is currently suppressed.

.*: +file format .*


Disassembly of section .text:

#...
[ 	]*[a-f0-9]+:	8d 81 ([0-9a-f]{2} ){4} *	lea    -0x[a-f0-9]+\(%ecx\),%eax
#pass
