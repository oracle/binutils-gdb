#source: pr24322a.s
#source: pr24322b.s
#as: --64 -defsym __64_bit__=1
#ld: -z shstk -melf_x86_64
#readelf: -n

Displaying notes found in: .note.gnu.property
  Owner                 Data size	Description
  GNU                  0x000000..	NT_GNU_PROPERTY_TYPE_0
      Properties: x86 feature: SHSTK
