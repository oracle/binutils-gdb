#source: tlspie3.s
#as: --32 -mrelax-relocations=yes
#ld: -melf_i386 -pie
#readelf: -r
#xfail: *-*-*
# XFAILed because generation of the R_386_GOT32X relocs is currently suppressed.

There are no relocations in this file.
