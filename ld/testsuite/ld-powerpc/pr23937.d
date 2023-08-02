#as: -a64
#ld: -melf64ppc --defsym puts=0 --defsym _start=0
#readelf: -srW
# Check that the IRELATIVE addend is magic+0, not magic+8

#...
.* R_PPC64_IRELATIVE +10000180
#pass
