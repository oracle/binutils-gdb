#name: GNU Property (single input, combine section)
#source: property-bti-pac1.s
#as: -defsym __mult__=0
#ld: -shared
#readelf: -n
#target: *linux*

Displaying notes found in: .note.gnu.property
  Owner                 Data size	Description
  GNU                  0x00000010	NT_GNU_PROPERTY_TYPE_0
      Properties: AArch64 feature: BTI, PAC
