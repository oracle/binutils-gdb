#source: variant_pcs-1.s
#source: variant_pcs-2.s
#ld: -shared --hash-style=sysv -T variant_pcs.ld -z now
#readelf: -rsW

Relocation section '\.rela\.plt' at offset 0x11000 contains 12 entries:
    Offset             Info             Type               Symbol's Value  Symbol's Name \+ Addend
0000000000009020  0000000.00000402 R_AARCH64_JUMP_SLOT    0000000000000000 f_base_global_default_undef \+ 0
0000000000009028  0000000.00000402 R_AARCH64_JUMP_SLOT    0000000000000000 f_spec_global_default_undef \+ 0
0000000000009030  0000000.00000402 R_AARCH64_JUMP_SLOT    0000000000008000 f_base_global_default_def \+ 0
0000000000009038  0000000.00000402 R_AARCH64_JUMP_SLOT    0000000000008000 f_spec_global_default_def \+ 0
0000000000009040  0000000000000408 R_AARCH64_IRELATIVE                       8000
0000000000009048  0000000.00000402 R_AARCH64_JUMP_SLOT    f_spec_global_default_ifunc\(\) f_spec_global_default_ifunc \+ 0
0000000000009050  0000000000000408 R_AARCH64_IRELATIVE                       8000
0000000000009058  0000000.00000402 R_AARCH64_JUMP_SLOT    f_base_global_default_ifunc\(\) f_base_global_default_ifunc \+ 0
0000000000009060  0000000000000408 R_AARCH64_IRELATIVE                       8038
0000000000009068  0000000000000408 R_AARCH64_IRELATIVE                       8000
0000000000009070  0000000000000408 R_AARCH64_IRELATIVE                       8000
0000000000009078  0000000000000408 R_AARCH64_IRELATIVE                       8038

Symbol table '\.dynsym' contains . entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
#...
     .: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT  UND f_base_global_default_undef
     .: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT  UND f_spec_global_default_undef 	\[VARIANT_PCS\] 
     .: 0000000000008000     0 IFUNC   GLOBAL DEFAULT    1 f_spec_global_default_ifunc 	\[VARIANT_PCS\] 
     .: 0000000000008000     0 NOTYPE  GLOBAL DEFAULT    1 f_base_global_default_def
     .: 0000000000008000     0 NOTYPE  GLOBAL DEFAULT    1 f_spec_global_default_def 	\[VARIANT_PCS\] 
     .: 0000000000008000     0 IFUNC   GLOBAL DEFAULT    1 f_base_global_default_ifunc

Symbol table '\.symtab' contains 35 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000008000     0 SECTION LOCAL  DEFAULT    1 
     2: 0000000000008070     0 SECTION LOCAL  DEFAULT    2 
     3: 0000000000009000     0 SECTION LOCAL  DEFAULT    3 
     4: 0000000000009080     0 SECTION LOCAL  DEFAULT    4 
     5: 0000000000011000     0 SECTION LOCAL  DEFAULT    5 
     6: 0000000000011120     0 SECTION LOCAL  DEFAULT    6 
     7: 00000000000111..     0 SECTION LOCAL  DEFAULT    7 
     8: 00000000000112..     0 SECTION LOCAL  DEFAULT    8 
     9: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS .*variant_pcs-1\.o
    10: 0000000000008000     0 NOTYPE  LOCAL  DEFAULT    1 f_spec_local 	\[VARIANT_PCS\] 
    11: 0000000000008000     0 IFUNC   LOCAL  DEFAULT    1 f_spec_local_ifunc 	\[VARIANT_PCS\] 
    12: 0000000000008000     0 IFUNC   LOCAL  DEFAULT    1 f_base_local_ifunc
    13: 0000000000008000     0 NOTYPE  LOCAL  DEFAULT    1 f_base_local
    14: 0000000000008000     0 NOTYPE  LOCAL  DEFAULT    1 \$x
    15: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS .*variant_pcs-2\.o
    16: 0000000000008038     0 NOTYPE  LOCAL  DEFAULT    1 f_spec_local2 	\[VARIANT_PCS\] 
    17: 0000000000008038     0 IFUNC   LOCAL  DEFAULT    1 f_spec_local2_ifunc 	\[VARIANT_PCS\] 
    18: 0000000000008038     0 IFUNC   LOCAL  DEFAULT    1 f_base_local2_ifunc
    19: 0000000000008038     0 NOTYPE  LOCAL  DEFAULT    1 f_base_local2
    20: 0000000000008038     0 NOTYPE  LOCAL  DEFAULT    1 \$x
    21: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS 
    22: 0000000000009080     0 OBJECT  LOCAL  DEFAULT  ABS _DYNAMIC
    23: 0000000000008000     0 NOTYPE  LOCAL  DEFAULT    1 f_spec_global_hidden_def 	\[VARIANT_PCS\] 
    24: 0000000000008000     0 IFUNC   LOCAL  DEFAULT    1 f_base_global_hidden_ifunc
    25: 0000000000008000     0 NOTYPE  LOCAL  DEFAULT    1 f_base_global_hidden_def
    26: 0000000000009000     0 OBJECT  LOCAL  DEFAULT  ABS _GLOBAL_OFFSET_TABLE_
    27: 0000000000008000     0 IFUNC   LOCAL  DEFAULT    1 f_spec_global_hidden_ifunc 	\[VARIANT_PCS\] 
    28: 0000000000008070     0 NOTYPE  LOCAL  DEFAULT    2 \$x
    29: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT  UND f_base_global_default_undef
    30: 0000000000000000     0 NOTYPE  GLOBAL DEFAULT  UND f_spec_global_default_undef 	\[VARIANT_PCS\] 
    31: 0000000000008000     0 IFUNC   GLOBAL DEFAULT    1 f_spec_global_default_ifunc 	\[VARIANT_PCS\] 
    32: 0000000000008000     0 NOTYPE  GLOBAL DEFAULT    1 f_base_global_default_def
    33: 0000000000008000     0 NOTYPE  GLOBAL DEFAULT    1 f_spec_global_default_def 	\[VARIANT_PCS\] 
    34: 0000000000008000     0 IFUNC   GLOBAL DEFAULT    1 f_base_global_default_ifunc
