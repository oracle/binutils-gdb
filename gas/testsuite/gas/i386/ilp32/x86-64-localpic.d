#source: ../x86-64-localpic.s
#as: --x32 -mrelax-relocations=yes
#readelf: -rsW
#name: x86-64 (ILP32) local PIC
#skip: *-*-*
# SKIPed because generation of the R_X86_64_REX_GOTPCRELX reloc is currently suppressed.

Relocation section '.rela.text' at offset 0x[0-9a-f]+ contains 1 entries:
 +Offset +Info +Type +Sym. Value +Symbol's Name \+ Addend
[0-9a-f]+ +[0-9a-f]+ R_X86_64_REX_GOTPCRELX +[0-9a-f]+ +foo - 4
#...
 +[0-9]+: +[0-9a-f]+ +[0-9a-f]+ +NOTYPE +LOCAL +DEFAULT +[0-9]+ +foo
#pass
