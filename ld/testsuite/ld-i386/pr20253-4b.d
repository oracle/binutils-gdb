#source: pr20253-4.s
#as: --32 -mrelax-relocations=yes
#ld: -pie -melf_i386
#readelf: -r --wide
#xfail: *-*-*
# XFAILed because generation of the R_386_GOT32X relocs is currently suppressed.

Relocation section '.rel.dyn' at offset 0x[0-9a-f]+ contains 1 entries:
 +Offset +Info +Type +Sym.* Value +Symbol's Name
[0-9a-f]+ +[0-9a-f]+ +R_386_IRELATIVE +
