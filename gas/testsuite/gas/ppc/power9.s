	.text
power9:
	cnttzd      3,13
	cnttzd.     4,14
	cnttzw      5,15
	cnttzw.     6,16
	modsd       10,20,21
	modsw       11,21,22
	modud       12,22,23
	moduw       13,23,24
	bcdcfn.     3,4,0
	bcdcfn.     3,4,1
	bcdcfsq.    4,5,0
	bcdcfsq.    4,5,1
	bcdcfz.     5,6,0
	bcdcfz.     5,6,1
	bcdcpsgn.   6,7,8
	bcdctn.     7,8
	bcdctsq.    8,9
	bcdctz.     9,10,0
	bcdctz.     9,10,1
	bcdsetsgn.  10,11,0
	bcdsetsgn.  10,11,1
	bcdsr.      11,12,13,0
	bcdsr.      11,12,13,1
	bcds.       12,13,14,0
	bcds.       12,13,14,1
	bcdtrunc.   13,14,15,0
	bcdtrunc.   13,14,15,1
	bcdus.      14,15,16
	bcdutrunc.  15,16,17
	lxvll       20,0,21
	lxvll       20,10,21
	stxvll      21,0,11
	stxvll      21,10,11
	vmul10cuq   22,23
	vmul10ecuq  23,24,25
	vmul10euq   24,25,26
	vmul10uq    25,26
	xsaddqp     10,11,12
	xsaddqpo    11,12,12
	xsrqpi      0,20,30,0
	xsrqpi      1,20,30,0
	xsrqpi      0,20,30,3
	xsrqpi      1,20,30,3
	xsrqpix     0,21,31,0
	xsrqpix     1,21,31,0
	xsrqpix     0,21,31,3
	xsrqpix     1,21,31,3
	xsmulqp     12,13,14
	xsmulqpo    13,14,15
	xsrqpxp     0,22,23,0
	xsrqpxp     1,22,23,0
	xsrqpxp     0,22,23,3
	xsrqpxp     1,22,23,3
	xscpsgnqp   14,15,16
	xscmpoqp    0,15,16
	xscmpoqp    7,15,16
	xscmpexpqp  0,16,17
	xscmpexpqp  7,16,17
	xsmaddqp    17,18,19
	xsmaddqpo   18,19,20
	xsmsubqp    19,20,21
	xsmsubqpo   20,21,22
	xsnmaddqp   21,22,23
	xsnmaddqpo  22,23,24
	xsnmsubqp   23,24,25
	xsnmsubqpo  24,25,26
	xssubqp     25,26,27
	xssubqpo    26,27,28
	xsdivqp     27,28,29
	xsdivqpo    28,29,30
	xscmpuqp    0,29,30
	xscmpuqp    7,29,30
	xststdcqp   0,30,0
	xststdcqp   7,30,0
	xststdcqp   0,31,0x7f
	xststdcqp   7,31,0x7f
	xsabsqp     10,11
	xsxexpqp    11,12
	xsnabsqp    12,13
	xsnegqp     13,14
	xsxsigqp    14,15
	xssqrtqp    15,16
	xssqrtqpo   16,17
	xscvqpuwz   17,18
	xscvudqp    18,19
	xscvqpswz   19,20
	xscvsdqp    20,21
	xscvqpudz   21,22
	xscvqpdp    22,23
	xscvqpdpo   23,24
	xscvdpqp    24,25
	xscvqpsdz   25,26
	xsiexpqp    26,27,28
	vpermr      4,5,6,7
	vextractub  5,6,0
	vextractub  5,6,0xf
	vextractuh  6,7,0
	vextractuh  6,7,0xf
	vextractuw  7,8,0
	vextractuw  7,8,0xf
	vextractd   8,9,0
	vextractd   8,9,0xf
	vinsertb    9,10,0
	vinsertb    9,10,0xf
	vinserth    10,11,0
	vinserth    10,11,0xf
	vinsertw    11,12,0
	vinsertw    11,12,0xf
	vinsertd    12,13,0
	vinsertd    12,13,0xf
	mfvsrld     20,45
	mtvsrws     46,21
	mtvsrdd     47,0,23
	mtvsrdd     47,22,23
	lxvx        50,0,11
	lxvx        0,10,11
	lxvwsx      51,0,12
	lxvwsx      1,10,12
	lxvh8x      52,0,13
	lxvh8x      2,10,13
	lxvb16x     53,0,14
	lxvb16x     3,10,14
	stxvx       54,0,15
	stxvx       4,20,15
	stxvh8x     55,0,16
	stxvh8x     5,20,16
	stxvb16x    56,0,17
	stxvb16x    6,20,17
	xxextractuw 4,5,0x0
	xxextractuw 40,50,0xf
	xxspltib    4,0x0
	xxspltib    4,-128
	xxspltib    41,255
	xxspltib    41,-1
	xxinsertw   5,6,0
	xxinsertw   50,60,0xf
	xxbrh       6,7
	xxbrh       56,57
	xxbrw       7,8
	xxbrw       57,58
	xxbrd       8,9
	xxbrd       58,59
	xxbrq       9,10
	xxbrq       59,60
	lxsd        20,0(0)
	lxsd        20,0(10)
	lxsd        20,8(0)
	lxsd        20,8(10)
	lxsd        20,-8(0)
	lxsd        20,-8(10)
	lxsd        20,32764(0)
	lxsd        20,32764(10)
	lxsd        20,-32768(0)
	lxsd        20,-32768(10)
	lxssp       30,0(0)
	lxssp       30,0(11)
	lxssp       30,8(0)
	lxssp       30,8(11)
	lxssp       30,-8(0)
	lxssp       30,-8(11)
	lxssp       30,32764(0)
	lxssp       30,32764(11)
	lxssp       30,-32768(0)
	lxssp       30,-32768(11)
	lxv         40,0(0)
	lxv         40,0(12)
	lxv         40,16(0)
	lxv         40,16(12)
	lxv         40,-16(0)
	lxv         10,-16(12)
	lxv         10,32752(0)
	lxv         10,32752(12)
	lxv         10,-32768(0)
	lxv         10,-32768(12)
	stxsd       21,0(0)
	stxsd       21,0(10)
	stxsd       21,8(0)
	stxsd       21,8(10)
	stxsd       21,-8(0)
	stxsd       21,-8(10)
	stxsd       21,32764(0)
	stxsd       21,32764(10)
	stxsd       21,-32768(0)
	stxsd       21,-32768(10)
	stxssp      31,0(0)
	stxssp      31,0(11)
	stxssp      31,8(0)
	stxssp      31,8(11)
	stxssp      31,-8(0)
	stxssp      31,-8(11)
	stxssp      31,32764(0)
	stxssp      31,32764(11)
	stxssp      31,-32768(0)
	stxssp      31,-32768(11)
	stxv        41,0(0)
	stxv        41,0(12)
	stxv        41,16(0)
	stxv        41,16(12)
	stxv        41,-16(0)
	stxv        11,-16(12)
	stxv        11,32752(0)
	stxv        11,32752(12)
	stxv        11,-32768(0)
	stxv        11,-32768(12)
	xxperm      20,22,24
	xxperm      40,42,44
	xxpermr     21,23,25
	xxpermr     41,43,45
	extswsli    12,20,0
	extswsli    12,20,1
	extswsli    12,20,63
	extswsli.   13,21,0
	extswsli.   13,21,1
	extswsli.   13,21,63
	vrlwmi      14,22,23
	vrldmi      15,23,24
	vrlwnm      16,24,25
	vrldnm      17,25,26
	vbpermd     18,26,27
	vnegw       19,20
	vnegd       20,21
	vprtybw     21,22
	vprtybd     22,23
	vprtybq     23,24
	vextsb2w    24,25
	vextsh2w    25,26
	vextsb2d    26,27
	vextsh2d    27,28
	vextsw2d    28,29
	vctzb       29,30
	vctzh       30,31
	vctzw       31,30
	vctzd       30,29
	lxsibzx     10,0,20
	lxsibzx     50,10,20
	lxsihzx     11,0,21
	lxsihzx     51,11,21
	stxsibx     12,0,22
	stxsibx     52,12,22
	stxsihx     13,0,23
	stxsihx     53,13,23
	maddhd      10,11,12,13
	maddhdu     20,21,22,23
	maddld      2,3,4,5
	xscmpexpdp  0,10,20
	xscmpexpdp  7,40,50
	xsiexpdp    41,11,21
	xststdcdp   0,11,0x7f
	xststdcdp   7,41,0x7f
	xststdcsp   0,11,0x7f
	xststdcsp   7,41,0x7f
	xsxexpdp    13,43
	xsxsigdp    14,44
	xviexpdp    45,46,47
	xviexpsp    46,47,48
	xvtstdcdp   54,55,0
	xvtstdcdp   54,55,0x7f
	xvtstdcsp   55,56,0
	xvtstdcsp   55,56,0x7f
	xvxexpdp    57,58
	xvxexpsp    58,59
	xvxsigdp    59,60
	xvxsigsp    60,61
	cmpeqb      0,6,7
	cmpeqb      7,6,7
	cmprb       0,0,8,9
	cmprb       7,0,8,9
	cmprb       0,1,8,9
	cmprb       7,1,8,9
	setb        15,0
	setb        15,7
	lxvl        26,0,10
	lxvl        56,20,10
	stxvl       27,0,11
	stxvl       57,21,11
	vclzlsbb    20,30
	vctzlsbb    21,31
	vcmpneb     10,11,12
	vcmpneb.    20,21,22
	vcmpneh     11,12,13
	vcmpneh.    21,22,23
	vcmpnew     12,13,14
	vcmpnew.    22,23,24
	vcmpnezb    13,14,15
	vcmpnezb.   23,24,25
	vcmpnezh    14,15,16
	vcmpnezh.   24,25,26
	vcmpnezw    15,16,17
	vcmpnezw.   25,26,27
	vextublx    16,17,10
	vextubrx    17,18,11
	vextuhlx    18,19,12
	vextuhrx    19,20,13
	vextuwlx    20,21,14
	vextuwrx    21,22,15
	dtstsfi     0,0,3
	dtstsfi     7,0x3f,3
	dtstsfiq    0,0,4
	dtstsfiq    7,0x3f,4
	xscvhpdp    40,50
	xscvdphp    41,51
	xvcvhpsp    42,52
	xvcvsphp    43,53
	addpcis     3,0
	subpcis     3,0
	addpcis     4,1
	subpcis     4,-1
	addpcis     5,-2
	subpcis     5,2
	addpcis     6,0x7fff
	subpcis     6,-0x7fff
	addpcis     7,-0x8000
	subpcis     7,0x8000
	slbsync
	slbiag      10
	slbieg      10,11
	slbmfee     3,4
	slbmfee     3,4,0
	slbmfee     3,4,1
	slbmfev     4,5
	slbmfev     4,5,0
	slbmfev     4,5,1
	tlbie       3,4
	tlbie       3,4,0,0,0
	tlbie       3,4,3,1,1
	tlbiel      3
	tlbiel      3,0,0,0,0
	tlbiel      3,4,3,1,1
	copy        12,13
	paste.      10,11
	cpabort
	hwsync
	sync
	sync        0
	lwsync
	sync        1
	ptesync
	sync        2
	ldat        20,0,0x0
	ldat        20,10,0x1c
	lwat        21,0,0x0
	lwat        21,11,0x1c
	stdat       22,0,0x0
	stdat       22,12,0x1c
	stwat       23,0,0x0
	stwat       23,13,0x1c
	urfid
	rmieg       30
	ldmx        10,0,15
	ldmx        10,3,15
	stop
	wait
	wait        0
	darn        3,0
	darn        3,1
	darn        3,2
	mcrxrx      0
	mcrxrx      7
	vslv        20,21,22
	vsrv        23,24,25
	msgsync
	xscmpeqdp   30,40,50
	xscmpgtdp   31,41,51
	xscmpgedp   32,42,52
	xsmincdp    34,44,54
	xsmaxcdp    35,45,55
	xsminjdp    36,46,56
	xsmaxjdp    37,47,57
	vmsumudm    20,21,22,23
	addex       11,12,13,0
	addex       11,12,13,1
	addex       11,12,13,2
	mffs        25
	mffs.       25
	mffsce      26
	mffscdrn    27,20
	mffscdrni   28,0
	mffscdrni   28,7
	mffscrn     29,21
	mffscrni    30,0
	mffscrni    30,3
	mffsl       31
