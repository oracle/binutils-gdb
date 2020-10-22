#as: --32 -mrelax-relocations=yes
#ld: -melf_i386
#objdump: -dw
#xfail: *-*-*
# XFAILed because generation of the R_386_GOT32X relocs is currently suppressed.

.*: +file format .*


Disassembly of section .text:

#...
[ 	]*[a-f0-9]+:	67 e8 ([0-9a-f]{2} ){4} *	addr16 call +[a-f0-9]+ <foo>
[ 	]*[a-f0-9]+:	67 e8 ([0-9a-f]{2} ){4} *	addr16 call +[a-f0-9]+ <bar>
[ 	]*[a-f0-9]+:	e9 ([0-9a-f]{2} ){4} *	jmp +[a-f0-9]+ <foo>
[ 	]*[a-f0-9]+:	90                   	nop
[ 	]*[a-f0-9]+:	e9 ([0-9a-f]{2} ){4} *	jmp +[a-f0-9]+ <bar>
[ 	]*[a-f0-9]+:	90                   	nop
#pass
