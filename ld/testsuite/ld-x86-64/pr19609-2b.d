#source: pr19609-2.s
#as: --x32 -mrelax-relocations=yes
#ld: -melf32_x86_64 -Ttext=0x70000000 -Tdata=0xa0000000
#error: .*relocation truncated to fit: R_X86_64_32S .*
#xfail: *-*-*
# XFAILed because generation of the R_X86_64_REX_GOTPCRELX and R_X86_64_GOTPCRELX relocs is currently suppressed.
