#source: ibt-plt-2.s
#as: --64 -defsym __64_bit__=1
#ld: -shared -m elf_x86_64 -z ibtplt --hash-style=sysv -z max-page-size=0x200000 -z noseparate-code
#objdump: -dw

.*: +file format .*


Disassembly of section .plt:

0+..0 <.plt>:
 +[a-f0-9]+:	ff 35 ca 01 20 00    	pushq  0x2001ca\(%rip\)        # 200... <_GLOBAL_OFFSET_TABLE_\+0x8>
 +[a-f0-9]+:	f2 ff 25 cb 01 20 00 	bnd jmpq \*0x2001cb\(%rip\)        # 200... <_GLOBAL_OFFSET_TABLE_\+0x10>
 +[a-f0-9]+:	0f 1f 00             	nopl   \(%rax\)
 +[a-f0-9]+:	f3 0f 1e fa          	endbr64 
 +[a-f0-9]+:	68 00 00 00 00       	pushq  \$0x0
 +[a-f0-9]+:	f2 e9 e1 ff ff ff    	bnd jmpq ..0 <.plt>
 +[a-f0-9]+:	90                   	nop
 +[a-f0-9]+:	f3 0f 1e fa          	endbr64 
 +[a-f0-9]+:	68 01 00 00 00       	pushq  \$0x1
 +[a-f0-9]+:	f2 e9 d1 ff ff ff    	bnd jmpq ..0 <.plt>
 +[a-f0-9]+:	90                   	nop

Disassembly of section .plt.sec:

0+..0 <bar1@plt>:
 +[a-f0-9]+:	f3 0f 1e fa          	endbr64 
 +[a-f0-9]+:	f2 ff 25 a5 01 20 00 	bnd jmpq \*0x2001a5\(%rip\)        # 200... <bar1>
 +[a-f0-9]+:	0f 1f 44 00 00       	nopl   0x0\(%rax,%rax,1\)

0+..0 <bar2@plt>:
 +[a-f0-9]+:	f3 0f 1e fa          	endbr64 
 +[a-f0-9]+:	f2 ff 25 9d 01 20 00 	bnd jmpq \*0x20019d\(%rip\)        # 200... <bar2>
 +[a-f0-9]+:	0f 1f 44 00 00       	nopl   0x0\(%rax,%rax,1\)

Disassembly of section .text:

0+..0 <foo>:
 +[a-f0-9]+:	48 83 ec 08          	sub    \$0x8,%rsp
 +[a-f0-9]+:	e8 e7 ff ff ff       	callq  ..0 <bar2@plt>
 +[a-f0-9]+:	48 83 c4 08          	add    \$0x8,%rsp
 +[a-f0-9]+:	e9 ce ff ff ff       	jmpq   ..0 <bar1@plt>
#pass
