	.text
	.globl	func
func:
	blr
	.long	0x7c284fec	/* dcbzl	r8,r9		*/
	.long	0xed405834	/* frsqrtes	f10,f11		*/
	.long	0xec220804	/* dadd		f1,f2,f1	*/
	.long	0xfc020004	/* daddq	f0,f2,f0	*/
	.long	0xec220c04	/* dsub		f1,f2,f1	*/
	.long	0xfc020404	/* dsubq	f0,f2,f0	*/
	.long	0xec220844	/* dmul		f1,f2,f1	*/
	.long	0xfc020044	/* dmulq	f0,f2,f0	*/
	.long	0xec220c44	/* ddiv		f1,f2,f1	*/
	.long	0xfc020444	/* ddivq	f0,f2,f0	*/
	.long	0xec820d04	/* dcmpu	cr1,f2,f1	*/
	.long	0xfc820504	/* dcmpuq	cr1,f2,f0	*/
