#source: call3.s
#as: --32 -mrelax-relocations=yes
#ld: -melf_i386 -z call-nop=suffix-144
#objdump: -dw
#xfail: *-*-*
# XFAILed because generation of the R_386_GOT32X relocs is currently suppressed.

.*: +file format .*


Disassembly of section .text:

#...
[ 	]*[a-f0-9]+:	e8 ([0-9a-f]{2} ){4} *	call +[a-f0-9]+ <foo>
[ 	]*[a-f0-9]+:	90                   	nop
#pass
