	.text
	.globl	func
func:
	.long	0x7c642e98	/*   0: lxvd2x  vs3,r4,r5         */
	.long	0x7c642ed8	/*   4: lxvd2ux vs3,r4,r5         */
	.long	0x7d642e99	/*   8: lxvd2x  vs43,r4,r5        */
	.long	0x7d642ed9	/*   c: lxvd2ux vs43,r4,r5        */
	.long	0x7c642f98	/*  10: stxvd2x vs3,r4,r5         */
	.long	0x7c642fd8	/*  14: stxvd2ux vs3,r4,r5        */
	.long	0x7d642f99	/*  18: stxvd2x vs43,r4,r5        */
	.long	0x7d642fd9	/*  1c: stxvd2ux vs43,r4,r5       */
	.long	0xf0642850	/*  20: xxmrghd vs3,vs4,vs5       */
	.long	0xf16c6857	/*  24: xxmrghd vs43,vs44,vs45    */
	.long	0xf0642b50	/*  28: xxmrgld vs3,vs4,vs5       */
	.long	0xf16c6b57	/*  2c: xxmrgld vs43,vs44,vs45    */
	.long	0xf0642850	/*  30: xxmrghd vs3,vs4,vs5       */
	.long	0xf16c6857	/*  34: xxmrghd vs43,vs44,vs45    */
	.long	0xf0642b50	/*  38: xxmrgld vs3,vs4,vs5       */
	.long	0xf16c6b57	/*  3c: xxmrgld vs43,vs44,vs45    */
	.long	0xf0642950	/*  40: xxpermdi vs3,vs4,vs5,1    */
	.long	0xf16c6957	/*  44: xxpermdi vs43,vs44,vs45,1 */
	.long	0xf0642a50	/*  48: xxpermdi vs3,vs4,vs5,2    */
	.long	0xf16c6a57	/*  4c: xxpermdi vs43,vs44,vs45,2 */
	.long	0xf0642780	/*  50: xvmovdp vs3,vs4           */
	.long	0xf16c6787	/*  54: xvmovdp vs43,vs44         */
	.long	0xf0642780	/*  58: xvmovdp vs3,vs4           */
	.long	0xf16c6787	/*  5c: xvmovdp vs43,vs44         */
	.long	0xf0642f80	/*  60: xvcpsgndp vs3,vs4,vs5     */
	.long	0xf16c6f87	/*  64: xvcpsgndp vs43,vs44,vs45  */
	.long	0x7c00007c	/*  68: wait                      */
	.long	0x7c00007c	/*  6c: wait                      */
	.long	0x7c20007c	/*  70: waitrsv                   */
	.long	0x7c20007c	/*  74: waitrsv                   */
	.long	0x7c40007c	/*  78: waitimpl                  */
	.long	0x7c40007c	/*  7c: waitimpl                  */
	.long	0x4c000324	/*  80: doze                      */
	.long	0x4c000364	/*  84: nap                       */
	.long	0x4c0003a4	/*  88: sleep                     */
	.long	0x4c0003e4	/*  8c: rvwinkle                  */
	.long	0x7c830134	/*  90: prtyw   r3,r4             */
	.long	0x7dcd0174	/*  94: prtyd   r13,r14           */
	.long	0x7d5c02a6	/*  98: mfcfar  r10               */
	.long	0x7d7c03a6	/*  9c: mtcfar  r11               */
	.long	0x7c832bf8	/*  a0: cmpb    r3,r4,r5          */
	.long	0x7d4b662a	/*  a4: lwzcix  r10,r11,r12       */
	.long	0xee119004	/*  a8: dadd    f16,f17,f18       */
	.long	0xfe96c004	/*  ac: daddq   f20,f22,f24       */
	.long	0x7c60066c	/*  b0: dss     3                 */
	.long	0x7e00066c	/*  b4: dssall                    */
	.long	0x7c2522ac	/*  b8: dst     r5,r4,1           */
	.long	0x7e083aac	/*  bc: dstt    r8,r7,0           */
	.long	0x7c6532ec	/*  c0: dstst   r5,r6,3           */
	.long	0x7e442aec	/*  c4: dststt  r4,r5,2           */
	.long	0x7d4b6356	/*  c8: divwe   r10,r11,r12       */
	.long	0x7d6c6b57	/*  cc: divwe.  r11,r12,r13       */
	.long	0x7d8d7756	/*  d0: divweo  r12,r13,r14       */
	.long	0x7dae7f57	/*  d4: divweo. r13,r14,r15       */
	.long	0x7d4b6316	/*  d8: divweu  r10,r11,r12       */
	.long	0x7d6c6b17	/*  dc: divweu. r11,r12,r13       */
	.long	0x7d8d7716	/*  e0: divweuo r12,r13,r14       */
	.long	0x7dae7f17	/*  e4: divweuo. r13,r14,r15      */
	.long	0x7e27d9f8	/*  e8: bpermd  r7,r17,r27        */
	.long	0x7e8a02f4	/*  ec: popcntw r10,r20           */
	.long	0x7e8a03f4	/*  f0: popcntd r10,r20           */
	.long	0x7e95b428	/*  f4: ldbrx   r20,r21,r22       */
	.long	0x7e95b528	/*  f8: stdbrx  r20,r21,r22       */
	.long	0x7d4056ee	/*  fc: lfiwzx  f10,0,r10         */
	.long	0x7d4956ee	/* 100: lfiwzx  f10,r9,r10        */
	.long	0xec802e9c	/* 104: fcfids  f4,f5             */
	.long	0xec802e9d	/* 108: fcfids. f4,f5             */
	.long	0xec802f9c	/* 10c: fcfidus f4,f5             */
	.long	0xec802f9d	/* 110: fcfidus. f4,f5            */
	.long	0xfc80291c	/* 114: fctiwu  f4,f5             */
	.long	0xfc80291d	/* 118: fctiwu. f4,f5             */
	.long	0xfc80291e	/* 11c: fctiwuz f4,f5             */
	.long	0xfc80291f	/* 120: fctiwuz. f4,f5            */
	.long	0xfc802f5c	/* 124: fctidu  f4,f5             */
	.long	0xfc802f5d	/* 128: fctidu. f4,f5             */
	.long	0xfc802f5e	/* 12c: fctiduz f4,f5             */
	.long	0xfc802f5f	/* 130: fctiduz. f4,f5            */
	.long	0xfc802f9c	/* 134: fcfidu  f4,f5             */
	.long	0xfc802f9d	/* 138: fcfidu. f4,f5             */
	.long	0xfc0a5900	/* 13c: ftdiv   cr0,f10,f11       */
	.long	0xff8a5900	/* 140: ftdiv   cr7,f10,f11       */
	.long	0xfc005140	/* 144: ftsqrt  cr0,f10           */
	.long	0xff805140	/* 148: ftsqrt  cr7,f10           */
	.long	0x7e084a2c	/* 14c: dcbtt   r8,r9             */
	.long	0x7e0849ec	/* 150: dcbtstt r8,r9             */
	.long	0xed406644	/* 154: dcffix  f10,f12           */
	.long	0xee80b645	/* 158: dcffix. f20,f22           */
	.long	0x7d4b6068	/* 15c: lbarx   r10,r11,r12       */
	.long	0x7d4b6068	/* 160: lbarx   r10,r11,r12       */
	.long	0x7d4b6069	/* 164: lbarx   r10,r11,r12,1     */
	.long	0x7e95b0e8	/* 168: lharx   r20,r21,r22       */
	.long	0x7e95b0e8	/* 16c: lharx   r20,r21,r22       */
	.long	0x7e95b0e9	/* 170: lharx   r20,r21,r22,1     */
	.long	0x7d4b656d	/* 174: stbcx.  r10,r11,r12       */
	.long	0x7d4b65ad	/* 178: sthcx.  r10,r11,r12       */
	.long	0xfdc07830	/* 17c: fre     f14,f15           */
	.long	0xfdc07831	/* 180: fre.    f14,f15           */
	.long	0xedc07830	/* 184: fres    f14,f15           */
	.long	0xedc07831	/* 188: fres.   f14,f15           */
	.long	0xfdc07834	/* 18c: frsqrte f14,f15           */
	.long	0xfdc07835	/* 190: frsqrte. f14,f15          */
	.long	0xedc07834	/* 194: frsqrtes f14,f15          */
	.long	0xedc07835	/* 198: frsqrtes. f14,f15         */
	.long	0x7c43271e	/* 19c: isel    r2,r3,r4,28       */
