#source: pr19609-6.s
#as: --64 -mrelax-relocations=yes
#ld: -melf_x86_64 --defsym foobar=0x70000000
#objdump: -dw
#xfail: *-*-*
# XFAILed because generation of the R_X86_64_REX_GOTPCRELX and R_X86_64_GOTPCRELX relocs is currently suppressed.

.*: +file format .*


Disassembly of section .text:

[a-f0-9]+ <_start>:
[ 	]*[a-f0-9]+:	48 c7 c0 00 00 00 70 	mov    \$0x70000000,%rax
#pass
