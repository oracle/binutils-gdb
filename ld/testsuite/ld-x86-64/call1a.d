#source: call1.s
#as: --64 -mrelax-relocations=yes
#ld: -melf_x86_64
#objdump: -dw
#xfail: *-*-*
# XFAILed because generation of the R_X86_64_REX_GOTPCRELX and R_X86_64_GOTPCRELX relocs is currently suppressed.

.*: +file format .*


Disassembly of section .text:

#...
[ 	]*[a-f0-9]+:	67 e8 ([0-9a-f]{2} ){4} *	addr32 callq +[a-f0-9]+ <foo>
#pass
