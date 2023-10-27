
hello.o：     文件格式 elf64-x86-64

Contents of section .text:
 0000 f30f1efa 554889e5 4883ec20 897dec48  ....UH..H.. .}.H
 0010 8975e083 7dec0474 19488d05 00000000  .u..}..t.H......
 0020 4889c7e8 00000000 bf010000 00e80000  H...............
 0030 0000c745 fc000000 00eb4b48 8b45e048  ...E......KH.E.H
 0040 83c01048 8b10488b 45e04883 c008488b  ...H..H.E.H...H.
 0050 004889c6 488d0500 00000048 89c7b800  .H..H......H....
 0060 000000e8 00000000 488b45e0 4883c018  ........H.E.H...
 0070 488b0048 89c7e800 00000089 c7e80000  H..H............
 0080 00008345 fc01837d fc077eaf e8000000  ...E...}..~.....
 0090 00b80000 0000c9c3                    ........        
Contents of section .rodata:
 0000 e794a8e6 b3953a20 48656c6c 6f20e5ad  ......: Hello ..
 0010 a6e58fb7 20e5a793 e5908d20 e7a792e6  .... ...... ....
 0020 95b0efbc 81004865 6c6c6f20 25732025  ......Hello %s %
 0030 730a00                               s..             
Contents of section .comment:
 0000 00474343 3a202855 62756e74 75203132  .GCC: (Ubuntu 12
 0010 2e322e30 2d337562 756e7475 31292031  .2.0-3ubuntu1) 1
 0020 322e322e 3000                        2.2.0.          
Contents of section .note.gnu.property:
 0000 04000000 10000000 05000000 474e5500  ............GNU.
 0010 020000c0 04000000 03000000 00000000  ................
Contents of section .eh_frame:
 0000 14000000 00000000 017a5200 01781001  .........zR..x..
 0010 1b0c0708 90010000 1c000000 1c000000  ................
 0020 00000000 98000000 00450e10 8602430d  .........E....C.
 0030 06028f0c 07080000                    ........        

Disassembly of section .text:

0000000000000000 <main>:
   0:	f3 0f 1e fa          	endbr64
   4:	55                   	push   %rbp
   5:	48 89 e5             	mov    %rsp,%rbp
   8:	48 83 ec 20          	sub    $0x20,%rsp
   c:	89 7d ec             	mov    %edi,-0x14(%rbp)
   f:	48 89 75 e0          	mov    %rsi,-0x20(%rbp)
  13:	83 7d ec 04          	cmpl   $0x4,-0x14(%rbp)
  17:	74 19                	je     32 <main+0x32>
  19:	48 8d 05 00 00 00 00 	lea    0x0(%rip),%rax        # 20 <main+0x20>
  20:	48 89 c7             	mov    %rax,%rdi
  23:	e8 00 00 00 00       	call   28 <main+0x28>
  28:	bf 01 00 00 00       	mov    $0x1,%edi
  2d:	e8 00 00 00 00       	call   32 <main+0x32>
  32:	c7 45 fc 00 00 00 00 	movl   $0x0,-0x4(%rbp)
  39:	eb 4b                	jmp    86 <main+0x86>
  3b:	48 8b 45 e0          	mov    -0x20(%rbp),%rax
  3f:	48 83 c0 10          	add    $0x10,%rax
  43:	48 8b 10             	mov    (%rax),%rdx
  46:	48 8b 45 e0          	mov    -0x20(%rbp),%rax
  4a:	48 83 c0 08          	add    $0x8,%rax
  4e:	48 8b 00             	mov    (%rax),%rax
  51:	48 89 c6             	mov    %rax,%rsi
  54:	48 8d 05 00 00 00 00 	lea    0x0(%rip),%rax        # 5b <main+0x5b>
  5b:	48 89 c7             	mov    %rax,%rdi
  5e:	b8 00 00 00 00       	mov    $0x0,%eax
  63:	e8 00 00 00 00       	call   68 <main+0x68>
  68:	48 8b 45 e0          	mov    -0x20(%rbp),%rax
  6c:	48 83 c0 18          	add    $0x18,%rax
  70:	48 8b 00             	mov    (%rax),%rax
  73:	48 89 c7             	mov    %rax,%rdi
  76:	e8 00 00 00 00       	call   7b <main+0x7b>
  7b:	89 c7                	mov    %eax,%edi
  7d:	e8 00 00 00 00       	call   82 <main+0x82>
  82:	83 45 fc 01          	addl   $0x1,-0x4(%rbp)
  86:	83 7d fc 07          	cmpl   $0x7,-0x4(%rbp)
  8a:	7e af                	jle    3b <main+0x3b>
  8c:	e8 00 00 00 00       	call   91 <main+0x91>
  91:	b8 00 00 00 00       	mov    $0x0,%eax
  96:	c9                   	leave
  97:	c3                   	ret
