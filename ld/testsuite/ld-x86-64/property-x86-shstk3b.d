#source: property-x86-shstk.s
#source: property-x86-3.s
#as: --64 -defsym __64_bit__=1 -mx86-used-note=yes
#ld: -r -melf_x86_64
#readelf: -n

Displaying notes found in: .note.gnu.property
  Owner                 Data size	Description
  GNU                  0x00000030	NT_GNU_PROPERTY_TYPE_0
      Properties: x86 ISA needed: i486, 586, SSE2, SSE3
	x86 ISA used: <None>
	x86 feature used: x86
