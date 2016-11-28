## Phase 1

解题过程：

```
gdb bomb
layout split # 看汇编代码和部分源码
b 73         # 源代码 73 行位置加 breakpoint
r            # 运行 bomb
n            # 运行下一行源代码
             # 随便输入个字符串
stepi        # 运行下一行汇编代码，直到运行到 phase_1 的汇编代码
```

这边截取两段可以给出提示的汇编代码：

```
0x400e32 <main+146>     callq  0x40149e <read_line>
0x400e37 <main+151>     mov    %rax,%rdi
0x400e3a <main+154>     callq  0x400ee0 <phase_1>
```

这里我们可以看到 %rax 中存的是 read_line 方法的返回值，然后再被存到 %rdi 中，也就是方法的第一个参数，传递并 call phase_1 方法。

```
0x400ee0 <phase_1>      sub    $0x8,%rsp
0x400ee4 <phase_1+4>    mov    $0x402400,%esi
0x400ee9 <phase_1+9>    callq  0x401338 <strings_not_equal>
0x400eee <phase_1+14>   test   %eax,%eax
0x400ef0 <phase_1+16>   je     0x400ef7 <phase_1+23>
0x400ef2 <phase_1+18>   callq  0x40143a <explode_bomb>
```

这里关键的是 strings_not_equal 方法，返回值如果是非零（也就是真）的话，就会触发 explode_bomb。

call strings_not_equal 方法之前的一句是把立即数 0x402400 存入 %esi（也就是方法的第二个参数），这里可以推论 strings_not_equal 有两个输入参数，第一个是前面输入的字符串的指针，第二个是待比较的字符串的指针。所以我们尝试看一下指针 0x402400 中存的是什么：

```
(gdb) x/s 0x402400
0x402400:       "Border relations with Canada have never been better."
```

这样就得到了第一个字符串：Border relations with Canada have never been better.

## Phase 2

```
b 82         # 源代码 82 行位置加 breakpoint
r            # 运行 bomb
             # 过掉 phase 1，随便输入 phase 2 的字符串
stepi        # 运行下一行汇编代码，直到运行到 phase_2 的汇编代码
```

我们得到以下汇编代码：

```
0x400efc <phase_2>      push   %rbp         # 保存 %rbp
0x400efd <phase_2+1>    push   %rbx         # 保存 %rbx
0x400efe <phase_2+2>    sub    $0x28,%rsp   # 栈指针下移 40 字节
0x400f02 <phase_2+6>    mov    %rsp,%rsi    # 把栈指针作为第二个参数
0x400f05 <phase_2+9>    callq  0x40145c <read_six_numbers>
0x400f0a <phase_2+14>   cmpl   $0x1,(%rsp)  # 判断第一个数字是否等于 1，如果不等于就引爆炸弹
0x400f0e <phase_2+18>   je     0x400f30 <phase_2+52>
0x400f10 <phase_2+20>   callq  0x40143a <explode_bomb>
0x400f15 <phase_2+25>   jmp    0x400f30 <phase_2+52>
0x400f17 <phase_2+27>   mov    -0x4(%rbx),%eax
0x400f1a <phase_2+30>   add    %eax,%eax
0x400f1c <phase_2+32>   cmp    %eax,(%rbx)        # 从以上三句可以推断，当前数字应该是前一个数字的两倍
0x400f1e <phase_2+34>   je     0x400f25 <phase_2+41>
0x400f20 <phase_2+36>   callq  0x40143a <explode_bomb>
0x400f25 <phase_2+41>   add    $0x4,%rbx          # 指针指向下个数字
0x400f29 <phase_2+45>   cmp    %rbp,%rbx          # 检查边界
0x400f2c <phase_2+48>   jne    0x400f17 <phase_2+27>
0x400f2e <phase_2+50>   jmp    0x400f3c <phase_2+64>
0x400f30 <phase_2+52>   lea    0x4(%rsp),%rbx     # 把当前指针指向第二个数字
0x400f35 <phase_2+57>   lea    0x18(%rsp),%rbp    # 根据后面我们可以推测这里保存的是边界，也就是栈顶 + 24 的位置
0x400f3a <phase_2+62>   jmp    0x400f17 <phase_2+27>
0x400f3c <phase_2+64>   add    $0x28,%rsp         # 恢复栈指针
0x400f40 <phase_2+68>   pop    %rbx
0x400f41 <phase_2+69>   pop    %rbp
0x400f42 <phase_2+70>   retq
```

```
0x40145c <read_six_numbers>     sub    $0x18,%rsp       # 栈指针下移 24 字节
0x401460 <read_six_numbers+4>   mov    %rsi,%rdx        # 把 phase 2 的栈顶移到 %rdx 中，作为第三个方法参数
0x401463 <read_six_numbers+7>   lea    0x4(%rsi),%rcx   # 把 phase 2 栈顶 + 4 的指针移到 %rcx 中，作为第四个方法参数
0x401467 <read_six_numbers+11>  lea    0x14(%rsi),%rax
0x40146b <read_six_numbers+15>  mov    %rax,0x8(%rsp)   # 把 phase 2 栈顶 + 20 这个指针移到当前栈顶 + 8 的位置
0x401470 <read_six_numbers+20>  lea    0x10(%rsi),%rax
0x401474 <read_six_numbers+24>  mov    %rax,(%rsp)      # 把 phase 2 栈顶 + 16 这个指针移到当前栈顶的位置
0x401478 <read_six_numbers+28>  lea    0xc(%rsi),%r9    # phase 2 栈顶 + 12 的指针作为第六个方法参数
0x40147c <read_six_numbers+32>  lea    0x8(%rsi),%r8    # phase 2 栈顶 + 8 的指针作为第五个方法参数
0x401480 <read_six_numbers+36>  mov    $0x4025c3,%esi   # 把立即数 0x4025c3 作为第二个方法参数
0x401485 <read_six_numbers+41>  mov    $0x0,%eax        # 返回值设为 0
0x40148a <read_six_numbers+46>  callq  0x400bf0 <__isoc99_sscanf@plt>
0x40148f <read_six_numbers+51>  cmp    $0x5,%eax        # 判断 sscanf 的返回值是否大于 5
0x401492 <read_six_numbers+54>  jg     0x401499 <read_six_numbers+61>
0x401494 <read_six_numbers+56>  callq  0x40143a <explode_bomb>
0x401499 <read_six_numbers+61>  add    $0x18,%rsp
0x40149d <read_six_numbers+65>  retq
```

查看立即数 0x4025c3 指向的内容，发现是个输入格式：

```
(gdb) x/s 0x4025c3
0x4025c3:       "%d %d %d %d %d %d"
```

再根据 sscanf 的方法签名，我们可以推测 read_six_numbers 是把输入的字符串按照 "%d %d %d %d %d %d" 的格式移到 phase 2 栈顶 +0, +4, +8, +12，+16 和 +20 的位置（最多六个方法参数，所以最后这两个是在当前的栈上分配的），并且每个数字占 4 个字节，是 int。

由于 sscanf 的返回值是填充的变量的个数，这里判断如果返回值小于或等于 5，就会引爆炸弹。所以到这里我们可以推断我们输入的字符串必定包含 6 个空格分隔的数字，并且我们知道它们被读取后存到了哪里。

然后再回到 phase_2，分析汇编码可以得知，程序检查了当前栈指针 +0 到 +24 之间的六个 int 数字，第一个数字是 1，之后每个数字是前一个数字的 2 倍。所以答案是: 1 2 4 8 16 32

## Phase 3

```
0x400f43 <phase_3>      sub    $0x18,%rsp
0x400f47 <phase_3+4>    lea    0xc(%rsp),%rcx           # sscanf 的第二个数字存在栈指针 +12 的位置
0x400f4c <phase_3+9>    lea    0x8(%rsp),%rdx           # sscanf 的第一个数字存在栈指针 +8 的位置
0x400f51 <phase_3+14>   mov    $0x4025cf,%esi           # x/s 0x4025cf 的结果是 "%d %d"
0x400f56 <phase_3+19>   mov    $0x0,%eax
0x400f5b <phase_3+24>   callq  0x400bf0 <__isoc99_sscanf@plt>
0x400f60 <phase_3+29>   cmp    $0x1,%eax                # scan 得到的数字数目必须大于 1
0x400f63 <phase_3+32>   jg     0x400f6a <phase_3+39>
0x400f65 <phase_3+34>   callq  0x40143a <explode_bomb>
0x400f6a <phase_3+39>   cmpl   $0x7,0x8(%rsp)           # 比较栈指针 +8 位置存储的数字和立即数 7
0x400f6f <phase_3+44>   ja     0x400fad <phase_3+106>   # 如果大于就转移（无符号比较），继而引爆炸弹，所以这里可以推测这个数字小于等于 7
0x400f71 <phase_3+46>   mov    0x8(%rsp),%eax           # 以下我们假设第一个数字为 x
0x400f75 <phase_3+50>   jmpq   *0x402470(,%rax,8)       # 间接跳转到 0x402470 + 8x，这个地方我们要看下 0x402470 中存的是什么，见后面的列表
0x400f7c <phase_3+57>   mov    $0xcf,%eax               # 207
0x400f81 <phase_3+62>   jmp    0x400fbe <phase_3+123>
0x400f83 <phase_3+64>   mov    $0x2c3,%eax              # 707
0x400f88 <phase_3+69>   jmp    0x400fbe <phase_3+123>
0x400f8a <phase_3+71>   mov    $0x100,%eax              # 256
0x400f8f <phase_3+76>   jmp    0x400fbe <phase_3+123>
0x400f91 <phase_3+78>   mov    $0x185,%eax              # 389
0x400f96 <phase_3+83>   jmp    0x400fbe <phase_3+123>
0x400f98 <phase_3+85>   mov    $0xce,%eax               # 206
0x400f9d <phase_3+90>   jmp    0x400fbe <phase_3+123>
0x400f9f <phase_3+92>   mov    $0x2aa,%eax              # 682
0x400fa4 <phase_3+97>   jmp    0x400fbe <phase_3+123>
0x400fa6 <phase_3+99>   mov    $0x147,%eax              # 327
0x400fab <phase_3+104>  jmp    0x400fbe <phase_3+123>
0x400fad <phase_3+106>  callq  0x40143a <explode_bomb>
0x400fb2 <phase_3+111>  mov    $0x0,%eax
0x400fb7 <phase_3+116>  jmp    0x400fbe <phase_3+123>
0x400fb9 <phase_3+118>  mov    $0x137,%eax              # 311
0x400fbe <phase_3+123>  cmp    0xc(%rsp),%eax           # 比较第二个数字与 %eax
0x400fc2 <phase_3+127>  je     0x400fc9 <phase_3+134>   # 如果以上比较相等，则过关
0x400fc4 <phase_3+129>  callq  0x40143a <explode_bomb>
0x400fc9 <phase_3+134>  add    $0x18,%rsp
0x400fcd <phase_3+138>  retq
```

我们可以检查得出，0x402470 以及其偏移的若干地址，都存着跳转地址，每个跳转地址都对应一个待比较的立即数。

```
(gdb) x/x 0x402470
0x402470:       0x00400f7c
(gdb) x/x 0x402478
0x402478:       0x00400fb9
(gdb) x/x 0x402480
0x402480:       0x00400f83
(gdb) x/x 0x402488
0x402488:       0x00400f8a
(gdb) x/x 0x402490
0x402490:       0x00400f91
(gdb) x/x 0x402498
0x402498:       0x00400f98
(gdb) x/x 0x4024a0
0x4024a0:       0x00400f9f
(gdb) x/x 0x4024a8
0x4024a8:       0x00400fa6
```

由上面的内容我们可以得出 phase 3 有多组解：

```
0 207
1 311
2 707
3 256
4 389
5 206
6 682
7 327
```

## Phase 4

```
0x40100c <phase_4>      sub    $0x18,%rsp
0x401010 <phase_4+4>    lea    0xc(%rsp),%rcx            # 存第二个数字
0x401015 <phase_4+9>    lea    0x8(%rsp),%rdx            # 存第一个数字
0x40101a <phase_4+14>   mov    $0x4025cf,%esi            # "%d %d"
0x40101f <phase_4+19>   mov    $0x0,%eax
0x401024 <phase_4+24>   callq  0x400bf0 <__isoc99_sscanf@plt>
0x401029 <phase_4+29>   cmp    $0x2,%eax
0x40102c <phase_4+32>   jne    0x401035 <phase_4+41>     # 必须是两个数字
0x40102e <phase_4+34>   cmpl   $0xe,0x8(%rsp)
0x401033 <phase_4+39>   jbe    0x40103a <phase_4+46>     # 第一个数字必须 <= 14, >= 0（无符号）
0x401035 <phase_4+41>   callq  0x40143a <explode_bomb>
0x40103a <phase_4+46>   mov    $0xe,%edx                 # func4 第三个参数为 14
0x40103f <phase_4+51>   mov    $0x0,%esi                 # 第二个参数为 0
0x401044 <phase_4+56>   mov    0x8(%rsp),%edi            # 第一个参数为我们输入的第一个数字
0x401048 <phase_4+60>   callq  0x400fce <func4>
0x40104d <phase_4+65>   test   %eax,%eax
0x40104f <phase_4+67>   jne    0x401058 <phase_4+76>     # 测试 func4 的返回值，如果不为零，则引爆炸弹
0x401051 <phase_4+69>   cmpl   $0x0,0xc(%rsp)
0x401056 <phase_4+74>   je     0x40105d <phase_4+81>     # 测试我们输入的第二个数字，如果为 0，则过关
0x401058 <phase_4+76>   callq  0x40143a <explode_bomb>
0x40105d <phase_4+81>   add    $0x18,%rsp
0x401061 <phase_4+85>   retq
```

从上面汇编码可以看出，输入为两个数字，第二个数字必须为 0，第一个数字要使得 func4 返回值为 0，因此我们接下来分析 func4。

```
0000000000400fce <func4>:
  400fce:   48 83 ec 08             sub    $0x8,%rsp
  400fd2:   89 d0                   mov    %edx,%eax
  400fd4:   29 f0                   sub    %esi,%eax              # eax = j - i
  400fd6:   89 c1                   mov    %eax,%ecx
  400fd8:   c1 e9 1f                shr    $0x1f,%ecx
  400fdb:   01 c8                   add    %ecx,%eax              # 以上几句是在判断 eax 的符号
  400fdd:   d1 f8                   sar    %eax                   # eax /= 2
  400fdf:   8d 0c 30                lea    (%rax,%rsi,1),%ecx     # val = eax + i = (j + i) / 2
  400fe2:   39 f9                   cmp    %edi,%ecx              # 比较输入和 val
  400fe4:   7e 0c                   jle    400ff2 <func4+0x24>    # 如果输入小于或等于 val，则跳转
  400fe6:   8d 51 ff                lea    -0x1(%rcx),%edx
  400fe9:   e8 e0 ff ff ff          callq  400fce <func4>
  400fee:   01 c0                   add    %eax,%eax
  400ff0:   eb 15                   jmp    401007 <func4+0x39>
  400ff2:   b8 00 00 00 00          mov    $0x0,%eax              # eax 为零
  400ff7:   39 f9                   cmp    %edi,%ecx
  400ff9:   7d 0c                   jge    401007 <func4+0x39>    # 如果输入大于等于 val，则 return
  400ffb:   8d 71 01                lea    0x1(%rcx),%esi         # 否则递归 call，第二个参数变为 val + 1
  400ffe:   e8 cb ff ff ff          callq  400fce <func4>
  401003:   8d 44 00 01             lea    0x1(%rax,%rax,1),%eax
  401007:   48 83 c4 08             add    $0x8,%rsp
  40100b:   c3                      retq
```

func4 开头几句看似很绕的汇编实际是在计算 (j - i) / 2，因为用右移来实现除法的话，要注意负数无法被整除时的舍入问题，所以这段代码中的计算与符号有关。func4 逆向工程为 C 代码为：

```
int func4(int n, int i, int j) {
    int val = (j + i) / 2;
    if (val <= n) {
        if (val >= n) return 0;
        else return 2 * func4(n, val+1, j) + 1;
    } else {
        return 2 * func4(n, i, val-1);
    }
}
```

这段代码的作用类似二分搜索，但是目标值只能向小的半段搜索，否则 `return 2 * func4(n, val+1, j) + 1;` 会返回非零的值。我们分析这个函数的取值可以得知满足条件的数有 0, 1, 3, 7

所以 phase 4 有四组可能的答案：

```
0 0
1 0
3 0
7 0
```
