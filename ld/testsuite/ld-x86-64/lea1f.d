#source: lea1.s
#as: --x32 -mrelax-relocations=yes
#ld: -melf32_x86_64
#objdump: -dw
#xfail: *-*-*
# XFAILed because generation of the R_X86_64_REX_GOTPCRELX and R_X86_64_GOTPCRELX relocs is currently suppressed.

.*: +file format .*


Disassembly of section .text:

#...
[a-f0-9]+ <_start>:
[ 	]*[a-f0-9]+:	c7 c0 ([0-9a-f]{2} ){4} *	mov    \$0x[a-f0-9]+,%eax
[ 	]*[a-f0-9]+:	41 c7 c3 ([0-9a-f]{2} ){4} *	mov    \$0x[a-f0-9]+,%r11d
[ 	]*[a-f0-9]+:	40 c7 c0 ([0-9a-f]{2} ){4} *	rex mov \$0x[a-f0-9]+,%eax
[ 	]*[a-f0-9]+:	41 c7 c3 ([0-9a-f]{2} ){4} *	mov    \$0x[a-f0-9]+,%r11d
[ 	]*[a-f0-9]+:	40 c7 c0 ([0-9a-f]{2} ){4} *	rex mov \$0x[a-f0-9]+,%eax
[ 	]*[a-f0-9]+:	41 c7 c3 ([0-9a-f]{2} ){4} *	mov    \$0x[a-f0-9]+,%r11d
#pass
