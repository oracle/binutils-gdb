# Check that CFI directives can accept all of the CSR names (including
# aliases).  The results for this test also ensures that the DWARF
# register numbers for the CSRs shouldn't change.

	.text
	.global _start
_start:
	.cfi_startproc
	nop
	# user counters/timers
	.cfi_offset cycle, 12288
	.cfi_offset time, 12292
	.cfi_offset instret, 12296
	.cfi_offset hpmcounter3, 12300
	.cfi_offset hpmcounter4, 12304
	.cfi_offset hpmcounter5, 12308
	.cfi_offset hpmcounter6, 12312
	.cfi_offset hpmcounter7, 12316
	.cfi_offset hpmcounter8, 12320
	.cfi_offset hpmcounter9, 12324
	.cfi_offset hpmcounter10, 12328
	.cfi_offset hpmcounter11, 12332
	.cfi_offset hpmcounter12, 12336
	.cfi_offset hpmcounter13, 12340
	.cfi_offset hpmcounter14, 12344
	.cfi_offset hpmcounter15, 12348
	.cfi_offset hpmcounter16, 12352
	.cfi_offset hpmcounter17, 12356
	.cfi_offset hpmcounter18, 12360
	.cfi_offset hpmcounter19, 12364
	.cfi_offset hpmcounter20, 12368
	.cfi_offset hpmcounter21, 12372
	.cfi_offset hpmcounter22, 12376
	.cfi_offset hpmcounter23, 12380
	.cfi_offset hpmcounter24, 12384
	.cfi_offset hpmcounter25, 12388
	.cfi_offset hpmcounter26, 12392
	.cfi_offset hpmcounter27, 12396
	.cfi_offset hpmcounter28, 12400
	.cfi_offset hpmcounter29, 12404
	.cfi_offset hpmcounter30, 12408
	.cfi_offset hpmcounter31, 12412
	.cfi_offset cycleh, 12800
	.cfi_offset timeh, 12804
	.cfi_offset instreth, 12808
	.cfi_offset hpmcounter3h, 12812
	.cfi_offset hpmcounter4h, 12816
	.cfi_offset hpmcounter5h, 12820
	.cfi_offset hpmcounter6h, 12824
	.cfi_offset hpmcounter7h, 12828
	.cfi_offset hpmcounter8h, 12832
	.cfi_offset hpmcounter9h, 12836
	.cfi_offset hpmcounter10h, 12840
	.cfi_offset hpmcounter11h, 12844
	.cfi_offset hpmcounter12h, 12848
	.cfi_offset hpmcounter13h, 12852
	.cfi_offset hpmcounter14h, 12856
	.cfi_offset hpmcounter15h, 12860
	.cfi_offset hpmcounter16h, 12864
	.cfi_offset hpmcounter17h, 12868
	.cfi_offset hpmcounter18h, 12872
	.cfi_offset hpmcounter19h, 12876
	.cfi_offset hpmcounter20h, 12880
	.cfi_offset hpmcounter21h, 12884
	.cfi_offset hpmcounter22h, 12888
	.cfi_offset hpmcounter23h, 12892
	.cfi_offset hpmcounter24h, 12896
	.cfi_offset hpmcounter25h, 12900
	.cfi_offset hpmcounter26h, 12904
	.cfi_offset hpmcounter27h, 12908
	.cfi_offset hpmcounter28h, 12912
	.cfi_offset hpmcounter29h, 12916
	.cfi_offset hpmcounter30h, 12920
	.cfi_offset hpmcounter31h, 12924
	# supervisor
	.cfi_offset sstatus, 1024
	.cfi_offset sie, 1040
	.cfi_offset stvec, 1044
	.cfi_offset scounteren, 1048
	.cfi_offset senvcfg, 1064
	.cfi_offset sscratch, 1280
	.cfi_offset sepc, 1284
	.cfi_offset scause, 1288
	.cfi_offset stval, 1292
	.cfi_offset sip, 1296
	.cfi_offset satp, 1536
	# machine
	.cfi_offset mvendorid, 15428
	.cfi_offset marchid, 15432
	.cfi_offset mimpid, 15436
	.cfi_offset mhartid, 15440
	.cfi_offset mconfigptr, 15444
	.cfi_offset mstatus, 3072
	.cfi_offset misa, 3076
	.cfi_offset medeleg, 3080
	.cfi_offset mideleg, 3084
	.cfi_offset mie, 3088
	.cfi_offset mtvec, 3092
	.cfi_offset mcounteren, 3096
	.cfi_offset mstatush, 3136
	.cfi_offset mscratch, 3328
	.cfi_offset mepc, 3332
	.cfi_offset mcause, 3336
	.cfi_offset mtval, 3340
	.cfi_offset mip, 3344
	.cfi_offset mtinst, 3368
	.cfi_offset mtval2, 3372
	.cfi_offset menvcfg, 3112
	.cfi_offset menvcfgh, 3176
	.cfi_offset mseccfg, 7452
	.cfi_offset mseccfgh, 7516
	.cfi_offset pmpcfg0, 3712
	.cfi_offset pmpcfg1, 3716
	.cfi_offset pmpcfg2, 3720
	.cfi_offset pmpcfg3, 3724
	.cfi_offset pmpcfg4, 3728
	.cfi_offset pmpcfg5, 3732
	.cfi_offset pmpcfg6, 3736
	.cfi_offset pmpcfg7, 3740
	.cfi_offset pmpcfg8, 3744
	.cfi_offset pmpcfg9, 3748
	.cfi_offset pmpcfg10, 3752
	.cfi_offset pmpcfg11, 3756
	.cfi_offset pmpcfg12, 3760
	.cfi_offset pmpcfg13, 3764
	.cfi_offset pmpcfg14, 3768
	.cfi_offset pmpcfg15, 3772
	.cfi_offset pmpaddr0, 3776
	.cfi_offset pmpaddr1, 3780
	.cfi_offset pmpaddr2, 3784
	.cfi_offset pmpaddr3, 3788
	.cfi_offset pmpaddr4, 3792
	.cfi_offset pmpaddr5, 3796
	.cfi_offset pmpaddr6, 3800
	.cfi_offset pmpaddr7, 3804
	.cfi_offset pmpaddr8, 3808
	.cfi_offset pmpaddr9, 3812
	.cfi_offset pmpaddr10, 3816
	.cfi_offset pmpaddr11, 3820
	.cfi_offset pmpaddr12, 3824
	.cfi_offset pmpaddr13, 3828
	.cfi_offset pmpaddr14, 3832
	.cfi_offset pmpaddr15, 3836
	.cfi_offset pmpaddr16, 3840
	.cfi_offset pmpaddr17, 3844
	.cfi_offset pmpaddr18, 3848
	.cfi_offset pmpaddr19, 3852
	.cfi_offset pmpaddr20, 3856
	.cfi_offset pmpaddr21, 3860
	.cfi_offset pmpaddr22, 3864
	.cfi_offset pmpaddr23, 3868
	.cfi_offset pmpaddr24, 3872
	.cfi_offset pmpaddr25, 3876
	.cfi_offset pmpaddr26, 3880
	.cfi_offset pmpaddr27, 3884
	.cfi_offset pmpaddr28, 3888
	.cfi_offset pmpaddr29, 3892
	.cfi_offset pmpaddr30, 3896
	.cfi_offset pmpaddr31, 3900
	.cfi_offset pmpaddr32, 3904
	.cfi_offset pmpaddr33, 3908
	.cfi_offset pmpaddr34, 3912
	.cfi_offset pmpaddr35, 3916
	.cfi_offset pmpaddr36, 3920
	.cfi_offset pmpaddr37, 3924
	.cfi_offset pmpaddr38, 3928
	.cfi_offset pmpaddr39, 3932
	.cfi_offset pmpaddr40, 3936
	.cfi_offset pmpaddr41, 3940
	.cfi_offset pmpaddr42, 3944
	.cfi_offset pmpaddr43, 3948
	.cfi_offset pmpaddr44, 3952
	.cfi_offset pmpaddr45, 3956
	.cfi_offset pmpaddr46, 3960
	.cfi_offset pmpaddr47, 3964
	.cfi_offset pmpaddr48, 3968
	.cfi_offset pmpaddr49, 3972
	.cfi_offset pmpaddr50, 3976
	.cfi_offset pmpaddr51, 3980
	.cfi_offset pmpaddr52, 3984
	.cfi_offset pmpaddr53, 3988
	.cfi_offset pmpaddr54, 3992
	.cfi_offset pmpaddr55, 3996
	.cfi_offset pmpaddr56, 4000
	.cfi_offset pmpaddr57, 4004
	.cfi_offset pmpaddr58, 4008
	.cfi_offset pmpaddr59, 4012
	.cfi_offset pmpaddr60, 4016
	.cfi_offset pmpaddr61, 4020
	.cfi_offset pmpaddr62, 4024
	.cfi_offset pmpaddr63, 4028
	.cfi_offset mcycle, 11264
	.cfi_offset minstret, 11272
	.cfi_offset mhpmcounter3, 11276
	.cfi_offset mhpmcounter4, 11280
	.cfi_offset mhpmcounter5, 11284
	.cfi_offset mhpmcounter6, 11288
	.cfi_offset mhpmcounter7, 11292
	.cfi_offset mhpmcounter8, 11296
	.cfi_offset mhpmcounter9, 11300
	.cfi_offset mhpmcounter10, 11304
	.cfi_offset mhpmcounter11, 11308
	.cfi_offset mhpmcounter12, 11312
	.cfi_offset mhpmcounter13, 11316
	.cfi_offset mhpmcounter14, 11320
	.cfi_offset mhpmcounter15, 11324
	.cfi_offset mhpmcounter16, 11328
	.cfi_offset mhpmcounter17, 11332
	.cfi_offset mhpmcounter18, 11336
	.cfi_offset mhpmcounter19, 11340
	.cfi_offset mhpmcounter20, 11344
	.cfi_offset mhpmcounter21, 11348
	.cfi_offset mhpmcounter22, 11352
	.cfi_offset mhpmcounter23, 11356
	.cfi_offset mhpmcounter24, 11360
	.cfi_offset mhpmcounter25, 11364
	.cfi_offset mhpmcounter26, 11368
	.cfi_offset mhpmcounter27, 11372
	.cfi_offset mhpmcounter28, 11376
	.cfi_offset mhpmcounter29, 11380
	.cfi_offset mhpmcounter30, 11384
	.cfi_offset mhpmcounter31, 11388
	.cfi_offset mcycleh, 11776
	.cfi_offset minstreth, 11784
	.cfi_offset mhpmcounter3h, 11788
	.cfi_offset mhpmcounter4h, 11792
	.cfi_offset mhpmcounter5h, 11796
	.cfi_offset mhpmcounter6h, 11800
	.cfi_offset mhpmcounter7h, 11804
	.cfi_offset mhpmcounter8h, 11808
	.cfi_offset mhpmcounter9h, 11812
	.cfi_offset mhpmcounter10h, 11816
	.cfi_offset mhpmcounter11h, 11820
	.cfi_offset mhpmcounter12h, 11824
	.cfi_offset mhpmcounter13h, 11828
	.cfi_offset mhpmcounter14h, 11832
	.cfi_offset mhpmcounter15h, 11836
	.cfi_offset mhpmcounter16h, 11840
	.cfi_offset mhpmcounter17h, 11844
	.cfi_offset mhpmcounter18h, 11848
	.cfi_offset mhpmcounter19h, 11852
	.cfi_offset mhpmcounter20h, 11856
	.cfi_offset mhpmcounter21h, 11860
	.cfi_offset mhpmcounter22h, 11864
	.cfi_offset mhpmcounter23h, 11868
	.cfi_offset mhpmcounter24h, 11872
	.cfi_offset mhpmcounter25h, 11876
	.cfi_offset mhpmcounter26h, 11880
	.cfi_offset mhpmcounter27h, 11884
	.cfi_offset mhpmcounter28h, 11888
	.cfi_offset mhpmcounter29h, 11892
	.cfi_offset mhpmcounter30h, 11896
	.cfi_offset mhpmcounter31h, 11900
	.cfi_offset mcountinhibit, 3200
	.cfi_offset mhpmevent3, 3212
	.cfi_offset mhpmevent4, 3216
	.cfi_offset mhpmevent5, 3220
	.cfi_offset mhpmevent6, 3224
	.cfi_offset mhpmevent7, 3228
	.cfi_offset mhpmevent8, 3232
	.cfi_offset mhpmevent9, 3236
	.cfi_offset mhpmevent10, 3240
	.cfi_offset mhpmevent11, 3244
	.cfi_offset mhpmevent12, 3248
	.cfi_offset mhpmevent13, 3252
	.cfi_offset mhpmevent14, 3256
	.cfi_offset mhpmevent15, 3260
	.cfi_offset mhpmevent16, 3264
	.cfi_offset mhpmevent17, 3268
	.cfi_offset mhpmevent18, 3272
	.cfi_offset mhpmevent19, 3276
	.cfi_offset mhpmevent20, 3280
	.cfi_offset mhpmevent21, 3284
	.cfi_offset mhpmevent22, 3288
	.cfi_offset mhpmevent23, 3292
	.cfi_offset mhpmevent24, 3296
	.cfi_offset mhpmevent25, 3300
	.cfi_offset mhpmevent26, 3304
	.cfi_offset mhpmevent27, 3308
	.cfi_offset mhpmevent28, 3312
	.cfi_offset mhpmevent29, 3316
	.cfi_offset mhpmevent30, 3320
	.cfi_offset mhpmevent31, 3324
	# hypervisor
	.cfi_offset hstatus, 6144
	.cfi_offset hedeleg, 6152
	.cfi_offset hideleg, 6156
	.cfi_offset hie, 6160
	.cfi_offset hcounteren, 6168
	.cfi_offset hgeie, 6172
	.cfi_offset htval, 6412
	.cfi_offset hip, 6416
	.cfi_offset hvip, 6420
	.cfi_offset htinst, 6440
	.cfi_offset hgeip, 14408
	.cfi_offset henvcfg, 6184
	.cfi_offset henvcfgh, 6248
	.cfi_offset hgatp, 6656
	.cfi_offset htimedelta, 6164
	.cfi_offset htimedeltah, 6228
	.cfi_offset vsstatus, 2048
	.cfi_offset vsie, 2064
	.cfi_offset vstvec, 2068
	.cfi_offset vsscratch, 2304
	.cfi_offset vsepc, 2308
	.cfi_offset vscause, 2312
	.cfi_offset vstval, 2316
	.cfi_offset vsip, 2320
	.cfi_offset vsatp, 2560
	# Smstateen extension
	.cfi_offset mstateen0, 3120
	.cfi_offset mstateen1, 3124
	.cfi_offset mstateen2, 3128
	.cfi_offset mstateen3, 3132
	.cfi_offset sstateen0, 1072
	.cfi_offset sstateen1, 1076
	.cfi_offset sstateen2, 1080
	.cfi_offset sstateen3, 1084
	.cfi_offset hstateen0, 6192
	.cfi_offset hstateen1, 6196
	.cfi_offset hstateen2, 6200
	.cfi_offset hstateen3, 6204
	.cfi_offset mstateen0h, 3184
	.cfi_offset mstateen1h, 3188
	.cfi_offset mstateen2h, 3192
	.cfi_offset mstateen3h, 3196
	.cfi_offset hstateen0h, 6256
	.cfi_offset hstateen1h, 6260
	.cfi_offset hstateen2h, 6264
	.cfi_offset hstateen3h, 6268
	# Sscofpmf extension
	.cfi_offset scountovf, 13952
	.cfi_offset mhpmevent3h, 7308
	.cfi_offset mhpmevent4h, 7312
	.cfi_offset mhpmevent5h, 7316
	.cfi_offset mhpmevent6h, 7320
	.cfi_offset mhpmevent7h, 7324
	.cfi_offset mhpmevent8h, 7328
	.cfi_offset mhpmevent9h, 7332
	.cfi_offset mhpmevent10h, 7336
	.cfi_offset mhpmevent11h, 7340
	.cfi_offset mhpmevent12h, 7344
	.cfi_offset mhpmevent13h, 7348
	.cfi_offset mhpmevent14h, 7352
	.cfi_offset mhpmevent15h, 7356
	.cfi_offset mhpmevent16h, 7360
	.cfi_offset mhpmevent17h, 7364
	.cfi_offset mhpmevent18h, 7368
	.cfi_offset mhpmevent19h, 7372
	.cfi_offset mhpmevent20h, 7376
	.cfi_offset mhpmevent21h, 7380
	.cfi_offset mhpmevent22h, 7384
	.cfi_offset mhpmevent23h, 7388
	.cfi_offset mhpmevent24h, 7392
	.cfi_offset mhpmevent25h, 7396
	.cfi_offset mhpmevent26h, 7400
	.cfi_offset mhpmevent27h, 7404
	.cfi_offset mhpmevent28h, 7408
	.cfi_offset mhpmevent29h, 7412
	.cfi_offset mhpmevent30h, 7416
	.cfi_offset mhpmevent31h, 7420
	# Sstc extension
	.cfi_offset stimecmp, 1332
	.cfi_offset stimecmph, 1396
	.cfi_offset vstimecmp, 2356
	.cfi_offset vstimecmph, 2420
	# dropped
	.cfi_offset ubadaddr, 268	# aliases
	.cfi_offset sbadaddr, 1292	# aliases
	.cfi_offset sptbr, 1536		# aliases
	.cfi_offset mbadaddr, 3340	# aliases
	.cfi_offset mucounteren, 3200	# aliases
	.cfi_offset mbase, 3584
	.cfi_offset mbound, 3588
	.cfi_offset mibase, 3592
	.cfi_offset mibound, 3596
	.cfi_offset mdbase, 3600
	.cfi_offset mdbound, 3604
	.cfi_offset mscounteren, 3204
	.cfi_offset mhcounteren, 3208
	.cfi_offset ustatus, 0
	.cfi_offset uie, 16
	.cfi_offset utvec, 20
	.cfi_offset uscratch, 256
	.cfi_offset uepc, 260
	.cfi_offset ucause, 264
	.cfi_offset utval, 268
	.cfi_offset uip, 272
	.cfi_offset sedeleg, 1032
	.cfi_offset sideleg, 1036
	# unprivileged
	.cfi_offset fflags, 4
	.cfi_offset frm, 8
	.cfi_offset fcsr, 12
	.cfi_offset dcsr, 7872
	.cfi_offset dpc, 7876
	.cfi_offset dscratch0, 7880
	.cfi_offset dscratch1, 7884
	.cfi_offset dscratch, 7880	# aliases
	.cfi_offset tselect, 7808
	.cfi_offset tdata1, 7812
	.cfi_offset tdata2, 7816
	.cfi_offset tdata3, 7820
	.cfi_offset tinfo, 7824
	.cfi_offset tcontrol, 7828
	.cfi_offset hcontext, 6816
	.cfi_offset scontext, 5792
	.cfi_offset mcontext, 7840
	.cfi_offset mscontext, 7848
	.cfi_offset mcontrol, 7812	# aliases
	.cfi_offset mcontrol6, 7812	# aliases
	.cfi_offset icount, 7812	# aliases
	.cfi_offset itrigger, 7812	# aliases
	.cfi_offset etrigger, 7812	# aliases
	.cfi_offset tmexttrigger, 7812	# aliases
	.cfi_offset textra32, 7820	# aliases
	.cfi_offset textra64, 7820	# aliases
	.cfi_offset seed, 84
	.cfi_offset vstart, 32
	.cfi_offset vxsat, 36
	.cfi_offset vxrm, 40
	.cfi_offset vcsr, 60
	.cfi_offset vl, 12416
	.cfi_offset vtype, 12420
	.cfi_offset vlenb, 12424
	nop
	.cfi_endproc
