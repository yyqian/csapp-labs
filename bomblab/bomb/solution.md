## Phase 1

解题过程：

```
gdb bomb
layout asm # 看汇编代码
b 73       # 源代码 73 行位置加 breakpoint
r          # 运行 bomb
n          # 运行下一行源代码
           # 随便输入个字符串
stepi      # 运行下一行汇编代码，直到运行到 phase_1 的汇编代码
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

