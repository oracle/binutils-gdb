#source: pltgot-1.s
#ld: -shared -melf_i386
#readelf: -d --wide
#as: --32

#...
 +0x[0-9a-f]+ +\(PLTREL.*
#...
