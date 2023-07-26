#source: ../ld-x86-64/pr24322c.s
#source: ../ld-x86-64/pr24322b.s
#as: --32 
#ld: -z shstk -m elf_i386
#readelf: -n

Displaying notes found in: .note.gnu.property
  Owner                 Data size	Description
  GNU                  0x000000..	NT_GNU_PROPERTY_TYPE_0
      Properties: x86 feature: SHSTK

