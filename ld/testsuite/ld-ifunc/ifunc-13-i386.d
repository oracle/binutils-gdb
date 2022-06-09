#source: ifunc-13a-i386.s
#source: ifunc-13b-i386.s
#ld: -shared -m elf_i386 -z nocombreloc
#as: --32 -mrelax-relocations=yes
#readelf: -r --wide
#target: x86_64-*-* i?86-*-*
#xfail: *-*-*
# XFAILed because generation of the R_386_GOT32X relocs is currently suppressed.

Relocation section '.rel.ifunc' at offset 0x[0-9a-f]+ contains 1 entries:
[ ]+Offset[ ]+Info[ ]+Type[ ]+.*
[0-9a-f]+[ ]+[0-9a-f]+[ ]+R_386_32[ ]+ifunc\(\)[ ]+ifunc
