#source: lea1.s
#as: --32 -mrelax-relocations=yes
#ld: -pie -melf_i386
#readelf: -Sw
#xfail: *-*-*
# XFAILed because generation of the R_386_GOT32X relocs is currently suppressed.

#failif
#...
[ 	]*\[.*\][ 	]+.*\.got .*
#...
