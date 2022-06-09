#source: tlspie2.s
#as: --64 -mrelax-relocations=yes
#ld: -melf_x86_64 -pie
#readelf: -r
#xfail: *-*-*
# XFAILed because generation of the R_X86_64_REX_GOTPCRELX and R_X86_64_GOTPCRELX relocs is currently suppressed.

There are no relocations in this file.
