## 实验二—BombLab
###  实验材料
1. 一个能够运行的Linux或者Unix系统
2. 官网的实习手册
3. cgdb
###  实验规则

实验共有6个关卡，分别是phase_1到phase_6，对于每一个关卡，你需要输入相应的数字或者字符串，你应该仔细研究它们的汇编语句，根据这些汇编语句来推测出你需要输入的数据，避免程序跳转到explode_bomb函数，这意味着炸弹拆除失败。
开始实验前，你应该确保你已经从官网下载了实习手册，然后，你的系统应该要安装好gdb调试器，因为我们接下来的调试都是基于gdb调试的。
### 2BombLab
#### 关卡一

我们首先键入以下语句，来将bomb的汇编语句输出到bomb.txt文本中：
```
   objdump -d bomb > bomb.txt
```

我们找到main函数语句的入口：
```

0000000000400da0 <main>:
  400da0:  53                     push   %rbx
  400da1:  83 ff 01               cmp    $0x1,%edi
  400da4:  75 10                  jne    400db6 <main+0x16>
  400da6:  48 8b 05 9b 29 20 00 mov    0x20299b(%rip),%rax        # 603748 <stdin@@GLIBC_2.2.5>
  400dad:  48 89 05 b4 29 20 00 mov    %rax,0x2029b4(%rip)        # 603768 <infile>
  400db4:  eb 63                  jmp    400e19 <main+0x79>
  400db6:  48 89 f3               mov    %rsi,%rbx
  400db9:  83 ff 02               cmp    $0x2,%edi
  400dbc:  75 3a                  jne    400df8 <main+0x58>
  400dbe:  48 8b 7e 08            mov    0x8(%rsi),%rdi
  400dc2:  be b4 22 40 00         mov    $0x4022b4,%esi
  400dc7:  e8 44 fe ff ff         callq  400c10 <fopen@plt>
  400dcc:  48 89 05 95 29 20 00 mov    %rax,0x202995(%rip)        # 603768 <infile>
  400dd3:  48 85 c0               test   %rax,%rax
  400dd6:  75 41                  jne    400e19 <main+0x79>
  400dd8:  48 8b 4b 08            mov    0x8(%rbx),%rcx
  400ddc:  48 8b 13               mov    (%rbx),%rdx
  400ddf:  be b6 22 40 00         mov    $0x4022b6,%esi
  400de4:  bf 01 00 00 00         mov    $0x1,%edi
  400de9:  e8 12 fe ff ff         callq  400c00 <__printf_chk@plt>
  400dee:  bf 08 00 00 00         mov    $0x8,%edi
  400df3:  e8 28 fe ff ff         callq  400c20 <exit@plt>
  400df8:  48 8b 16               mov    (%rsi),%rdx
  400dfb:  be d3 22 40 00         mov    $0x4022d3,%esi
  400e00:  bf 01 00 00 00         mov    $0x1,%edi
  400e05:  b8 00 00 00 00         mov    $0x0,%eax
  400e0a:  e8 f1 fd ff ff         callq  400c00 <__printf_chk@plt>
  400e0f:  bf 08 00 00 00         mov    $0x8,%edi
  400e14:  e8 07 fe ff ff         callq  400c20 <exit@plt>
  400e19:  e8 84 05 00 00         callq  4013a2 <initialize_bomb>
  400e1e:  bf 38 23 40 00         mov    $0x402338,%edi
  400e23:  e8 e8 fc ff ff         callq  400b10 <puts@plt>
  400e28:  bf 78 23 40 00         mov    $0x402378,%edi
  400e2d:  e8 de fc ff ff         callq  400b10 <puts@plt>
  400e32:  e8 67 06 00 00         callq  40149e <read_line>
  400e37:  48 89 c7               mov    %rax,%rdi
  400e3a:  e8 a1 00 00 00         callq  400ee0 <phase_1>
  400e3f:  e8 80 07 00 00         callq  4015c4 <phase_defused>
  400e44:  bf a8 23 40 00         mov    $0x4023a8,%edi
  400e49:  e8 c2 fc ff ff         callq  400b10 <puts@plt>
  400e4e:  e8 4b 06 00 00         callq  40149e <read_line>
  400e53:  48 89 c7               mov    %rax,%rdi
  400e56:  e8 a1 00 00 00         callq  400efc <phase_2>
  400e5b:  e8 64 07 00 00         callq  4015c4 <phase_defused>
  400e60:  bf ed 22 40 00         mov    $0x4022ed,%edi
  400e65:  e8 a6 fc ff ff         callq  400b10 <puts@plt>
  400e6a:  e8 2f 06 00 00         callq  40149e <read_line>
  400e6f:  48 89 c7               mov    %rax,%rdi
  400e72:  e8 cc 00 00 00         callq  400f43 <phase_3>
  400e77:  e8 48 07 00 00         callq  4015c4 <phase_defused>
  400e7c:  bf 0b 23 40 00         mov    $0x40230b,%edi
  400e81:  e8 8a fc ff ff         callq  400b10 <puts@plt>
  400e86:  e8 13 06 00 00         callq  40149e <read_line>
  400e8b:  48 89 c7               mov    %rax,%rdi
  400e8e:  e8 79 01 00 00         callq  40100c <phase_4>
  400e93:  e8 2c 07 00 00         callq  4015c4 <phase_defused>
  400e98:  bf d8 23 40 00         mov    $0x4023d8,%edi
  400e9d:  e8 6e fc ff ff         callq  400b10 <puts@plt>
  400ea2:  e8 f7 05 00 00         callq  40149e <read_line>
  400ea7:  48 89 c7               mov    %rax,%rdi
  400eaa:  e8 b3 01 00 00         callq  401062 <phase_5>
  400eaf:  e8 10 07 00 00         callq  4015c4 <phase_defused>
  400eb4:  bf 1a 23 40 00         mov    $0x40231a,%edi
  400eb9:  e8 52 fc ff ff         callq  400b10 <puts@plt>
  400ebe:  e8 db 05 00 00         callq  40149e <read_line>
  400ec3:  48 89 c7               mov    %rax,%rdi
  400ec6:  e8 29 02 00 00         callq  4010f4 <phase_6>
  400ecb:  e8 f4 06 00 00         callq  4015c4 <phase_defused>
  400ed0:  b8 00 00 00 00         mov    $0x0,%eax
  400ed5:  5b                     pop    %rbx
  400ed6:  c3                     retq   
  400ed7:  90                     nop
  400ed8:  90                     nop
  400ed9:  90                     nop
  400eda:  90                     nop
  400edb:  90                     nop
  400edc:  90                     nop
  400edd:  90                     nop
  400ede:  90                     nop
  400edf:  90                     nop
  400f41:  5d                     pop    %rbp
  400f42:  c3                     retq   
```


这看上去很吓人，但是别急，我们只需要找到和第一关相关的语句就可以了：

```

  400e32:  e8 67 06 00 00         callq  40149e <read_line>
  400e37:  48 89 c7               mov    %rax,%rdi
  400e3a:  e8 a1 00 00 00         callq  400ee0 <phase_1>

```
在这里我们看到了函数read_line，顾名思义，应该是让我们输入一行数据，然后将返回值作为第一个参数进入第一关，这个返回值应该就是我们输入的一行数据的值或者地址，现在它被保存在%rdi中进入第一关。

我们命令行中键入**``` gdb bomb ```**以进入gdb调试模式，随后我们设置断点在第一关函数的起始位置，我们需要键入命令**``` break phase_1```**来完成这个目的，然后输入run(或者r)以启动它。在进入第一关前，就像我们刚刚所说的，我们需要输入一行数据，这里我们随便输入一行数据：”abcdefg”。

输入完成之后，程序如愿的在phase_1的起始位置停了下来，我们键入命令 **```disas```** 来查看当前函数的汇编代码：
```

0000000000400ee0 <phase_1>:

1  400ee0:  48 83 ec 08            sub    $0x8,%rsp         /*栈指针减*/
2  400ee4:  be 00 24 40 00         mov    $0x402400,%esi    /*将0x402400移入%esi作为第二个参数*/
3  400ee9:  e8 4a 04 00 00         callq  401338 <strings_not_equal> /*调用函数*/
4  400eee:  85 c0                  test   %eax,%eax                 /*测试返回值是否为0*/
5  400ef0:  74 05                  je     400ef7 <phase_1+0x17>      /*如果不为0，爆炸*/
6  400ef2:  e8 43 05 00 00         callq  40143a <explode_bomb>
7  400ef7:  48 83 c4 08            add    $0x8,%rsp
8  400efb:  c3                     retq   
```

在这一段代码中我们看到程序调用了一个函数<strings_not_equal>，顾名思义，它用来测试字符串是否不相等，事实上这段函数的汇编代码告诉我们它确实是如此的，如果函数不相等返回1，相等则返回0。而这段代码要求我们返回值一定为0，否则就会爆炸，所以我们要保证这两个字符串相等，这就是拆除炸弹的秘诀了。

而在第二行我们看到程序将0x402400作为第二个参数，这很显然不是一个字符串，那么它应该是一个地址，保存着一个字符串。那么第一个参数%rdi呢？我们在之前已经分析过了，我们所输入的字符串的值或者地址被保存在%rdi中，它将作为第一个参数。那么答案已经很明显了，我们要保证我们输入的字符串与地址0x402400中的字符串相等，那么我们键入 **```x/s 0x402400```** 以字符串显示的形式来查看这个地址保存的内容:
```

“Border relations with Canada have never been better.”

```

所以第一关的答案就是字符串：

>Border relations with Canada have never been better.

#### 关卡二

我们重新启动gdb调试模式，这一次，我们将断点设置在第二关，即键入```beak phase_2```。期间我们需要输入第一关的答案，然后我们会来到第二关，先来看看与main函数中第二关有关的汇编代码：
```

  400e4e:  e8 4b 06 00 00         callq  40149e <read_line>
  400e53:  48 89 c7               mov    %rax,%rdi
  400e56:  e8 a1 00 00 00         callq  400efc <phase_2>

```

和第一关一样，同样是需要输入一行数据，程序相要到达断点phase_2，我们必须要输入这一行数据，我们随便输入：”abcdefg”，然后键入***```disas```**将第二关代码以汇编语句输出：
```

0   0000000000400efc <phase_2>:

1     400efc:  55                   push   %rbp  /*保存信息*/
2     400efd:  53                   push   %rbx
3     400efe:  48 83 ec 28            sub    $0x28,%rsp /*栈指针减40*/
4     400f02:  48 89 e6               mov    %rsp,%rsi  /*%rsi = %rsp作为第二个参数*/
5     400f05:  e8 52 05 00 00         callq  40145c <read_six_numbers>  /*读取六个数字*/
6     400f0a:  83 3c 24 01            cmpl   $0x1,(%rsp)  /*将内存(%rsp)与1比较*/
7     400f0e:  74 20                  je     400f30 <phase_2+0x34>  /*if == 1, jmp 400f30,否则,bomb!*/

/*进入关键循环*/

8     400f10:  e8 25 05 00 00         callq  40143a <explode_bomb>
9     400f15:  eb 19                  jmp    400f30 <phase_2+0x34>
10    400f17:  8b 43 fc               mov    -0x4(%rbx),%eax  /*%eax = (%rbx - 4) = n[x-1]*/
11    400f1a:  01 c0                  add    %eax,%eax     /*%eax = 2 * %eax*/
12    400f1c:  39 03                  cmp    %eax,(%rbx)  /*将%eax与(%rbp)比较*/
13    400f1e:  74 05                  je     400f25 <phase_2+0x29>  /*必须要2 * n[x-1] = n[x],否则,bomb!*/
14    400f20:  e8 15 05 00 00         callq  40143a <explode_bomb>
15    400f25:  48 83 c3 04            add    $0x4,%rbx  /*%rbx = %rbx + 4, 引用下一个数*/
16    400f29:  48 39 eb               cmp    %rbp,%rbx  /*与%rsp + 24对比*/
17    400f2c:  75 e9                  jne    400f17 <phase_2+0x1b>  /*if != ,jmp 400f17*/
18    400f2e:  eb 0c                  jmp    400f3c <phase_2+0x40>
19    400f30:  48 8d 5c 24 04         lea    0x4(%rsp),%rbx  /*%rbx = %rsp + 4*/
20    400f35:  48 8d 6c 24 18         lea    0x18(%rsp),%rbp  /*%rbp = %rsp + 24*/
21    400f3a:  eb db                  jmp    400f17 <phase_2+0x1b>  /*jmp 400f17*/
22    400f3c:  48 83 c4 28            add    $0x28,%rsp
23    400f40:  5b                       pop    %rbx
```

由第5行可以看出，我们应该输入6个数字。由6，7，8行得出，内存(%rsp)的值必须为1，否则就会爆炸。我们尝试将关键循环的部分翻译成熟悉的C语言风格：
```
for (auto x = %rsp + 4; x != %rsp + 24; x += 4){
    int val1 = *(x - 4), val2 = *(x);
    if (val1 * 2 != val2)//这说明*(x)必须要等有2倍的*(x-4)
        Bomb!
}
```

由于内存(%rsp)的值必须为1，这就意味着内存(%rsp + 4)必须为2，内存(%rsp + 8)必须为4，内存(%rsp + 12)必须为8，内存(%rsp + 16)必须为16，内存(%rsp + 20)必须为32。其实我们已经猜到了这就是我们需要的答案，但是答案顺序是怎么样的呢？来看看函数read_six_numbers内究竟发生了什么！我们键入**```disas read_six_numbers```**来看看它的汇编代码：
```

0   /*%rsi = %rrsp, %rdi = read_line*/
1   000000000040145c <read_six_numbers>:
2     40145c:  48 83 ec 18            sub    $0x18,%rsp  /*%rsp - 24*/
3     401460:  48 89 f2               mov    %rsi,%rdx  /*%rdx = %rrsp*/
4     401463:  48 8d 4e 04            lea    0x4(%rsi),%rcx  /*%rcx = %rrsp + 4*/
5     401467:  48 8d 46 14            lea    0x14(%rsi),%rax  /*%rax = %rrsp + 20*/
6     40146b:  48 89 44 24 08         mov    %rax,0x8(%rsp)  /*(%rsp + 8) = %rrsp + 20*/
7     401470:  48 8d 46 10            lea    0x10(%rsi),%rax  /*%rax = %rrsp + 16*/
8     401474:  48 89 04 24            mov    %rax,(%rsp)  /*(%rsp) = %rrsp + 16*/
9     401478:  4c 8d 4e 0c            lea    0xc(%rsi),%r9  /*%r9 = %rrsp + 12*/
10    40147c:  4c 8d 46 08            lea    0x8(%rsi),%r8  /*%r8 = %rrsp + 8*/
11    401480:  be c3 25 40 00         mov    $0x4025c3,%esi  /*%esi = 0x4025c3*/
12    401485:  b8 00 00 00 00         mov    $0x0,%eax  /*%eax = 0*/
13    40148a:  e8 61 f7 ff ff         callq  400bf0 <__isoc99_sscanf@plt>
14    40148f:  83 f8 05               cmp    $0x5,%eax  /*必须要正确读6个数或以上，否则爆炸*/
15    401492:  7f 05                  jg     401499 <read_six_numbers+0x3d>
16    401494:  e8 a1 ff ff ff         callq  40143a <explode_bomb>
17    401499:  48 83 c4 18            add    $0x18,%rsp
18    40149d:  c3                     retq   
```

这段代码中，我们以%rrsp表示我们为进入函数时原先的栈指针，以区分当前减小了的栈指针。这段代码中我们看到了sscanf函数，这是一个我们熟悉的函数，11行的有个数字很唐突，我们键入**```x/s 0x4025c3```**来看看sscanf的第二个参数是什么：
  ![image.png](https://upload-images.jianshu.io/upload_images/26093099-59f3c8bd1ce1f771.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


竟然是六个格式化的输入形式，而sscanf的第一个参数是%rdi，它是我们输入的一行数据的地址或值，根据第二个参数的信息可以看出，sscanf后面应该还会有第3到第8个参数来将我们输入的六个数字格式化输出到这六个参数中。看来这六个参数就是关键了，参考第3.4节基本寄存器以及我们的汇编注释，第三个参数到第六个参数分别是%rrsp,%%rrsp+4,%rrsp+8,%rrsp+12，而第七个第八个参数会依次保存至调用者的栈中上(参考3.8节)，即第七个参数对于(%rsp) = %rrsp + 16，第八个参数对于(%rsp + 8) = %rrsp + 20。好了，让我们回到第二关，现在我们已经知道%rsp到%rsp+20依次对应我们输入数字的地址，那么答案已经很明显了，我们需要输入的六个数字分别为:

>1  2  4  8  16  32

#### 关卡三

我们按照之前的步骤，来到断点phase_3处，并输出第三关的汇编语句：
```

1   400f43: sub    $0x18,%rsp       # 栈指针减24
2   400f47: lea    0xc(%rsp),%rcx # %rcx = %rsp + 12，第4个参数
3   400f4c: lea    0x8(%rsp),%rdx # %rdx = %rsp + 8，第3个参数
4   400f51: mov    $0x4025cf,%esi # %esi作为第二个入参准备进行函数调用
5   400f56: mov    $0x0,%eax # 为%eax赋值0
6   400f5b: callq  400bf0 <__isoc99_sscanf@plt>
7   400f60: cmp    $0x1,%eax # 如果正确输入的值的数量大于1
8   400f63: jg     400f6a <phase_3+0x27> # jmp 400f6a
9   400f65: callq  40143a <explode_bomb> # 否则爆炸
10  400f6a: cmpl   $0x7,0x8(%rsp) # %rsp+8的内存值与7比较
11  400f6f: ja     400fad <phase_3+0x6a>  # 如果大于，爆炸
12  400f71: mov    0x8(%rsp),%eax # %eax = (%rsp + 0x8)
13  400f75: jmpq   *0x402470(,%rax,8) # 并跳转至%rax*8+0x402470的内存保存的地址上；
14  400f7c: mov    $0xcf,%eax
15  400f81: jmp    400fbe <phase_3+0x7b>
16  400f83: mov    $0x2c3,%eax
17  400f88: jmp    400fbe <phase_3+0x7b>
18  400f8a: mov    $0x100,%eax
19  400f8f: jmp    400fbe <phase_3+0x7b>
20  400f91: mov    $0x185,%eax
21  400f96: jmp    400fbe <phase_3+0x7b>
22  400f98: mov    $0xce,%eax
23  400f9d: jmp    400fbe <phase_3+0x7b>
24  400f9f: mov    $0x2aa,%eax
25  400fa4: jmp    400fbe <phase_3+0x7b>
26  400fa6: mov    $0x147,%eax
27  400fab: jmp    400fbe <phase_3+0x7b>
28  400fad: callq  40143a <explode_bomb>
29  400fb2: mov    $0x0,%eax
30  400fb7: jmp    400fbe <phase_3+0x7b>
31  400fb9: mov    $0x137,%eax
32  400fbe: cmp    0xc(%rsp),%eax
33  400fc2: je     400fc9 <phase_3+0x86>
34  400fc4: callq  40143a <explode_bomb>
35  400fc9: add    $0x18,%rsp
36  400fcd: retq

```

我们又看到了sscanf函数，同之前一样，我们打印地址0x4025cf的字符串，发现这次需要我们输入两个数字:
```

(gdb) x/s 0x4025cf

0x4025cf:       "%d %d"
```

同样的分析，我们发现sscanf第三个参数%rdx = %rsp + 8，第四个参数%rcx = %rsp + 12，这意味着我们输入的第一个数被保存在地址%rsp + 8中，第二个数被保存在地址%rsp + 12中，由第10，11行可知，第一个参数应该小于等于7。来看看第13行的代码：400f75: jmpq   *0x402470(,%rax,8)，它跳转到(0x402470 + 8 * %rax)的位置，而%rax此时等于我们的第一个数，这段代码的意思是以输入的第一个数为下标，跳转到以0x402470为起始地址，数组元素大小为8字节的数组的内存位置，我们试着打印0x402470的值，这一次，我们以16进制格式打印，我们键入**```x/8x 0x402470```**:
```
(gdb) x/8a 0x402470
0x402470:       0x400f7c <phase_3+57>   0x400fb9 <phase_3+118>
0x402480:       0x400f83 <phase_3+64>   0x400f8a <phase_3+71>
0x402490:       0x400f91 <phase_3+78>   0x400f98 <phase_3+85>
0x4024a0:       0x400f9f <phase_3+92>   0x400fa6 <phase_3+99>
```

 那么这个数组应该为array[8] = {0x400f7c,0x400fb9,0x400f83,...,0x400fa6}，假设我们的第一个数为0，那么我们应该会跳转到0x400f7c位置处，我们抽取相应的代码：
```
14  400f7c: mov    $0xcf,%eax   /*%eax = 0xcf = 207*/
15  400f81: jmp    400fbe <phase_3+0x7b>  /*jmp 400fbe*/

......

32  400fbe: cmp    0xc(%rsp),%eax  /*将0xcf与我们的第二个参数对比*/
33  400fc2: je     400fc9 <phase_3+0x86>  /*如果等于，拆除成功，否则爆炸！*/
34  400fc4: callq  40143a <explode_bomb>
```

对应的第二个数应该为207，事实上应该有8中不同的答案，这仅仅是其中一种答案：

>0    207

#### 关卡四

现在，我们来到了关卡四，来看看它的汇编代码：

```

0   000000000040100c <phase_4>:
1     40100c: sub    $0x18,%rsp
2     401010: lea    0xc(%rsp),%rcx  # %rcx = %rsp + 12
3     401015: lea    0x8(%rsp),%rdx  # %rdx = %rsp + 8
4     40101a: mov    $0x4025cf,%esi # 0x4025cf是"%d %d"
5     40101f: mov    $0x0,%eax
6     401024: callq  400bf0 <__isoc99_sscanf@plt>
7     401029: cmp    $0x2,%eax # 正确输入值的个数只能为2个
8     40102c: jne    401035 <phase_4+0x29> # 否则爆炸
9     40102e: cmpl   $0xe,0x8(%rsp) # 第一个数和14比较
10    401033: jbe    40103a <phase_4+0x2e> # 如果这个数 <= 14 jmp
11    401035: callq  40143a <explode_bomb> # 否则爆炸
12    40103a: mov    $0xe,%edx # %edx = 14 第三个入参
13    40103f: mov    $0x0,%esi # %esi = 0 第二个入参
14    401044: mov    0x8(%rsp),%edi # 第一个数作为第一个参数
15    401048: callq  400fce <func4> # 调用函数
16    40104d: test   %eax,%eax # 返回值结果
17    40104f: jne    401058 <phase_4+0x4c> # 不等于0时，爆炸
18    401051: cmpl   $0x0,0xc(%rsp) # 第二个数和0比较
19    401056: je     40105d <phase_4+0x51> # 相等，返回
20    401058: callq  40143a <explode_bomb> # 不相等 爆炸
21    40105d: add    $0x18,%rsp
22    401061: retq
 ```

我们依然看到了sscanf这个函数，我们同之前一样的分析，会发现这一关依然只需要我们输入两个整数，并且它们被放在地址%rsp + 8 和 %rsp + 12的内存位置中。通过第18、19行可以看出，第二个数一定得为0。而对于第一个数而言，由第10行可知，它必须小于等于14，分析第12行到第17行，我们发现第一个数被当作第一个参数调用了函数func4，而这个函数调用的结果必须为0，那么，我们键入**```disas func4```**来看看这个函数到底做了什么：

```

0   /*%rdi = n[0], %rsi = 0, %rdx = 14*/
1   0000000000400fce <func4>:
2     400fce:  48 83 ec 08            sub    $0x8,%rsp  # %rsp - 8
3     400fd2:  89 d0                  mov    %edx,%eax  # %eax = %edx = 14
4     400fd4:  29 f0                  sub    %esi,%eax  # %eax -= %esi(初始值为0)
5     400fd6:  89 c1                  mov    %eax,%ecx  # %ecx = %eax = 14
6     400fd8:  c1 e9 1f               shr    $0x1f,%ecx  # %ecx >> 31 = 0, 逻辑右移
7     400fdb:  01 c8                  add    %ecx,%eax  # %eax += %ecx = 14
8     400fdd:  d1 f8                  sar    %eax  # %eax >> 1 = 7, 算术右移
9     400fdf:  8d 0c 30               lea    (%rax,%rsi,1),%ecx  # %ecx = %rax + %rsi = 7
10    400fe2:  39 f9                  cmp    %edi,%ecx  # 将%ecx 与 n[0] 比较
11    400fe4:  7e 0c                  jle    400ff2 <func4+0x24>  # if 7 小于等于 n[0], jmp 400ff2
12    400fe6:  8d 51 ff               lea    -0x1(%rcx),%edx  # 否则 %edx = % rcx - 1
13    400fe9:  e8 e0 ff ff ff         callq  400fce <func4>  # 递归调用
14    400fee:  01 c0                  add    %eax,%eax  # return %rax + %rax
15    400ff0:  eb 15                  jmp    401007 <func4+0x39>
16    400ff2:  b8 00 00 00 00         mov    $0x0,%eax  # 设置返回值 = 0
17    400ff7:  39 f9                  cmp    %edi,%ecx  # 将%ecx 与 n[0] 比较
18    400ff9:  7d 0c                  jge    401007 <func4+0x39>  # if %ecx >= n[0], return
19    400ffb:  8d 71 01               lea    0x1(%rcx),%esi  # 否则, %esi = %rcx + 1
20    400ffe:  e8 cb ff ff ff         callq  400fce <func4>  # 递归调用
21    401003:  8d 44 00 01            lea    0x1(%rax,%rax,1),%eax  # %eax = %rax + %rax + 1
22    401007:  48 83 c4 08            add    $0x8,%rsp
23    40100b:  c3                     retq   
 ```

可以看出来这是一段递归的调用，它的等价C语言很好写出来：

```

int func4(int nums, int x, int y){
    int ret = y - x;
    int k = (unsigned)(ret) >> 31;
    ret = (k + ret) >> 1;
    k = ret + x;

    if (k > nums)
        return 2 * func4(nums, x, k - 1);
    ret = 0;

    if (k < nums)
        return 2 * func4(nums, k + 1, y) + 1;

    return ret;

 }

 ```

首先我们必须肯定我们要输入的第一个数nums必须大于等于0，否则这段代码将会陷入死循环，那么我们枚举包括0到14之间所有的值，判断func4的返回值是否为0，这个枚举循环很好写：

```

for(int i = 0; i <= 14; i++)
    if(func4(i, 0, 14) == 0)
        printf("%d\n", i);

 ```

我们可以得到输出0、1、3、7，那么我们任选其一作为第一个数，这里我们选择0，而第二个数必须为0，于是有答案：

>0  0

#### 关卡五

现在，我们来到了倒数第二关，输出第五关的汇编代码如下：

```
31  0000000000401062 <phase_5>:
32    401062: push   %rbx
33    401063: sub    $0x20,%rsp
34    401067: mov    %rdi,%rbx  # %rdi是我们输入子串的地址，现在它保存在%rbx中
35    40106a: mov    %fs:0x28,%rax
36    401071:
37    401073: mov    %rax,0x18(%rsp)  # (%rsp + 0x18) = %rax
38    401078: xor    %eax,%eax  # %eax = 0
39    40107a: callq  40131b <string_length>  # 顾名思义，计算字符串长度函数
40    40107f: cmp    $0x6,%eax # 要求输入的字符序列长度为6
41    401082: je     4010d2 <phase_5+0x70> # jmp 4010d2
42    401084: callq  40143a <explode_bomb> # 不然爆炸
43    401089: jmp    4010d2 <phase_5+0x70>  # jmp 4010d2

44    # 关键循环部分
45    40108b: movzbl (%rbx,%rax,1),%ecx # %ecx = (%rbx + 0)(我们输入的字串) = str[0] = str[%rax]
46    40108f: mov    %cl,(%rsp) # (%rsp) = str[%rax]
47    401092: mov    (%rsp),%rdx # %rdx = (%rsp) = str[%rax]
48    401096: and    $0xf,%edx # %rdx = str[%rax] & 0xf
49    401099: movzbl 0x4024b0(%rdx),%edx # %rdx = 访问(0x4024b0 + %rdx)地址处的值,设为arr[%rdx]
50    4010a0: mov    %dl,0x10(%rsp,%rax,1) # (%rsp + %rax + 0x10) = arr[%rdx]
51    4010a4: add    $0x1,%rax  # %rax += 1
52    4010a8: cmp    $0x6,%rax  # 运行6次之后跳出
53    4010ac: jne    40108b <phase_5+0x29>
54    #
55    4010ae: movb   $0x0,0x16(%rsp)  # (%rsp + 0x16) = 0
56    4010b3: mov    $0x40245e,%esi # 第二个入参：0x40245e字的符串"flyers"
57    4010b8: lea    0x10(%rsp),%rdi # 第一个入参：上述循环压栈的字符
58    4010bd: callq  401338 <strings_not_equal>
59    4010c2: test   %eax,%eax
60    4010c4: je     4010d9 <phase_5+0x77> # 字符串相等， return
61    4010c6: callq  40143a <explode_bomb>  # 否则, bomb!
62    4010cb: nopl   0x0(%rax,%rax,1)
63    4010d0: jmp    4010d9 <phase_5+0x77>
64    4010d2: mov    $0x0,%eax
65    4010d7: jmp    40108b <phase_5+0x29>
66    4010d9: mov    0x18(%rsp),%rax
67    4010de: xor    %fs:0x28,%rax
68    4010e5:
69    4010e7: je     4010ee <phase_5+0x8c>
70    4010e9: callq  400b30 <__stack_chk_fail@plt>
71    4010ee: add    $0x20,%rsp
72    4010f2: pop    %rbx
73    4010f3: retq

 ```

根据39到41行可以看出，这一关要求我们输入一个字符串，并且字符串的长度一定得等于6。然后让我们来着重分析这一关的关键循环部分，根据我们的注释，循环一共执行了6次，它以我们输入的每一个字符串与0xf得到的结果作为下标，访问0x4024b0处的字符，并将得到的字符依次放地址为%rsp + 0x10 到 %rsp + 5 + 0x10的内存中。

![image.png](https://upload-images.jianshu.io/upload_images/26093099-ea26d7004f7c4825.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


来看看0x4024b0处的字符串：”maduiersnfotvbylso you think you can stop the bomb with ctrl-c, do you?”，这段话的奥义很神妙，我们姑且放一放。55到61行将我们循环部分中压入的6个字符与”flyers”对比，当它们相等时，我们才能成功拆除炸弹。这说明我们循环压入栈中的6个字符应该依次为’f’ ‘l’ ‘y’ ‘e’ ‘r’ ‘s’(56行)，我们假设我们输入的6个字符串为str，0x4024b0位置的字符串为arr，这就意味着我们必须要满足:
```
arr[str[0] & 0xf] = ‘f’
arr[str[1] & 0xf] = ‘l’
arr[str[2] & 0xf] = ‘y’
arr[str[3] & 0xf] = ‘e’
arr[str[4] & 0xf] = ‘r’
arr[str[5] & 0xf] = ‘s’
```

这会有很多种答案，我们仅给出一种答案，我们挑选arr[9] = ‘f’、arr[15] = ‘l’、arr[14] = ‘y’、arr[5] = ‘e’、arr[6] = ‘r’和 arr[7] = ‘s’，然后我们给出输入的答案：”)/>5&7”，它们ASCII码对应十进制数与0xf的值分别为9、15、14、5、6、7，因此，这是一个合法的答案：

>)/>5&7

#### 关卡六

这是我们的最后一弹，也是最难的一弹，来看看它的汇编代码，我将它分为四个部分：

```

00000000004010f4 <phase_6>:

##第一部分

1   4010f4:  41 56                  push   %r14      /*一些准备工作*/
2   4010f6:  41 55                  push   %r13
3   4010f8:  41 54                  push   %r12
4   4010fa:  55                     push   %rbp
5   4010fb:  53                     push   %rbx
6   4010fc:  48 83 ec 50            sub    $0x50,%rsp /*栈指针减80*/
7   401100:  49 89 e5               mov    %rsp,%r13  /*%r13 = %rsp*/
8   401103:  48 89 e6               mov    %rsp,%rsi  /*%rsi = %rsp*/
9   401106:  e8 51 03 00 00         callq  40145c <read_six_numbers>  /*调用函数，读六个数字*/
10  40110b:  49 89 e6               mov    %rsp,%r14  /*%r14 = %rsp*/
11  40110e:  41 bc 00 00 00 00      mov    $0x0,%r12d  /*%r12d = 0*/
12  401114:  4c 89 ed               mov    %r13,%rbp  /*%rbp = %r13 = %rsp*/
13  401117:  41 8b 45 00            mov    0x0(%r13),%eax  /*%eax = (%rsp) = n[0]*/
14  40111b:  83 e8 01               sub    $0x1,%eax  /*%eax = (%rsp) - 1= n[0] - 1*/
15  40111e:  83 f8 05               cmp    $0x5,%eax  /*n[0] - 1与 5比较*/
16  401121:  76 05                  jbe    401128 <phase_6+0x34>  /*if (n[0] <= 6), jmp 401128*/
17  401123:  e8 12 03 00 00         callq  40143a <explode_bomb>  /*bomb*/
18  401128:  41 83 c4 01            add    $0x1,%r12d  /*%r12d++*/
19  40112c:  41 83 fc 06            cmp    $0x6,%r12d  /*if (%12d == 6), jmp 401153*/
20  401130:  74 21                  je     401153 <phase_6+0x5f>
21  401132:  44 89 e3               mov    %r12d,%ebx  /*%ebx = %12d*/
22  401135:  48 63 c3               movslq %ebx,%rax  /*%rax = %ebx = %12d*/
23  401138:  8b 04 84               mov    (%rsp,%rax,4),%eax  /*%eax = n[%rax]*/
24  40113b:  39 45 00               cmp    %eax,0x0(%rbp)  /*比较当前栈内存与当前输入数字*/
25  40113e:  75 05                  jne    401145 <phase_6+0x51>  /*if (n[%rax] == n[i]), bomb*/
26  401140:  e8 f5 02 00 00         callq  40143a <explode_bomb>
27  401145:  83 c3 01               add    $0x1,%ebx  /*%ebx++*/
28  401148:  83 fb 05               cmp    $0x5,%ebx  /*比较%ebx与5*/
29  40114b:  7e e8                  jle    401135 <phase_6+0x41>  /*if (%ebx <= 5), jmp 401135*/
30  40114d:  49 83 c5 04            add    $0x4,%r13  /*%r13 += 4*/
31  401151:  eb c1                  jmp    401114 <phase_6+0x20>  /*无条件跳转到401114*/
##

##第二部分
32  401153:  48 8d 74 24 18         lea    0x18(%rsp),%rsi  /*%rsi = %rsp + 24*/
33  401158:  4c 89 f0               mov    %r14,%rax  /*%rax = %r14 = %rsp*/
34  40115b:  b9 07 00 00 00         mov    $0x7,%ecx  /*%ecx = 7*/
35  401160:  89 ca                  mov    %ecx,%edx  /*%edx = %ecx = 7*/
36  401162:  2b 10                  sub    (%rax),%edx  /*(%rax) = n[x], %edx = 7 - n[x]*/
37  401164:  89 10                  mov    %edx,(%rax)  /*(%rax) = %edx = 7 - n[x]*/
38  401166:  48 83 c0 04            add    $0x4,%rax  /*%rax = %rsp + 4*/
39  40116a:  48 39 f0               cmp    %rsi,%rax  /*比较%rsp + x 与 %rsp + 24*/
40  40116d:  75 f1                  jne    401160 <phase_6+0x6c>  /*if (x != 24), jmp 401160*/
##

##第三部分
%rax = %rsp + 24
41  40116f:  be 00 00 00 00         mov    $0x0,%esi  /*%esi = 0*/
42  401174:  eb 21                  jmp    401197 <phase_6+0xa3>  /*jmp 40117*/
43  401176:  48 8b 52 08            mov    0x8(%rdx),%rdx  /*%rdx = (%rdx + 8)*/
44  40117a:  83 c0 01               add    $0x1,%eax  /*%eax++*/
45  40117d:  39 c8                  cmp    %ecx,%eax  /*比较%eax 与 %ecx*/
46  40117f:  75 f5                  jne    401176 <phase_6+0x82>  /*if (%eax != %%ecx), jmp 401176*/
47  401181:  eb 05                  jmp    401188 <phase_6+0x94>  /*jmp 401188*/
48  401183:  ba d0 32 60 00         mov    $0x6032d0,%edx  /*%edx = 0x6032d0*/
49  401188:  48 89 54 74 20         mov    %rdx,0x20(%rsp,%rsi,2)  /*%(%rsp + 2*%rsi + 0x20) = %rdx*/
50  40118d:  48 83 c6 04            add    $0x4,%rsi  /*%rsi += 4*/
51  401191:  48 83 fe 18            cmp    $0x18,%rsi  /*比较%rsp + 4x 与 %rsp + 24*/
52  401195:  74 14                  je     4011ab <phase_6+0xb7>  /*if ==, jmp 4011ab*/
53  401197:  8b 0c 34               mov    (%rsp,%rsi,1),%ecx  /*%ecx = n[%rsp / 4]*/
54  40119a:  83 f9 01               cmp    $0x1,%ecx  /*%ecx ? 1*/
55  40119d:  7e e4                  jle    401183 <phase_6+0x8f>  /*if (n[x] <= 1), jmp 401183*/
56  40119f:  b8 01 00 00 00         mov    $0x1,%eax  /*%eax = 1*/
57  4011a4:  ba d0 32 60 00         mov    $0x6032d0,%edx  /*%edx = 0x6032d0*/
58  4011a9:  eb cb                  jmp    401176 <phase_6+0x82>  /*jmp 401176*/
#
59  4011ab:  48 8b 5c 24 20         mov    0x20(%rsp),%rbx    /*%rbx = (%rsp + 0x20)*/
60  4011b0:  48 8d 44 24 28         lea    0x28(%rsp),%rax  /*%rax = %rsp + 0x28*/
61  4011b5:  48 8d 74 24 50         lea    0x50(%rsp),%rsi  /*%rsi = %rsp + 0x50*/
62  4011ba:  48 89 d9               mov    %rbx,%rcx  /*%rcx = %rbx = (%rsp + 0x20)*/
63  4011bd:  48 8b 10               mov    (%rax),%rdx  /*%rdx = (%rax) = (%rsp + 0x28)*/
64  4011c0:  48 89 51 08            mov    %rdx,0x8(%rcx)  /*(%rsp + 0x28) = (%rsp + 0x28)*/
65  4011c4:  48 83 c0 08            add    $0x8,%rax  /*%rax= %rax + 8*/
66  4011c8:  48 39 f0               cmp    %rsi,%rax  /*%rax ? %rsp + 0x50*/
67  4011cb:  74 05                  je     4011d2 <phase_6+0xde>  /*if ==, jmp 4011d2*/
68  4011cd:  48 89 d1               mov    %rdx,%rcx  /*if !=, %rcx = %rdx*/
69  4011d0:  eb eb                  jmp    4011bd <phase_6+0xc9>  /*jmp 4011bd*/
70  4011d2:  48 c7 42 08 00 00 00 movq   $0x0,0x8(%rdx)  /*(rdx+8) = 0*/
##

##第四部分
%rdx = 0x6032e0
%rbx = *(%rsp + 0x20)
71  4011d9:  00
72  4011da:  bd 05 00 00 00         mov    $0x5,%ebp  /*%ebp = 5*/
73  4011df:  48 8b 43 08            mov    0x8(%rbx),%rax  /*%rax = (%rbx + 8) = node<2>->val*/
74  4011e3:  8b 00                  mov    (%rax),%eax  /*%eax = node<2>->val*/
75  4011e5:  39 03                  cmp    %eax,(%rbx)  /*node<1>->val ? node<2>->val*/
76  4011e7:  7d 05                  jge    4011ee <phase_6+0xfa>  /*if <, bomb!*/
77  4011e9:  e8 4c 02 00 00         callq  40143a <explode_bomb>
78  4011ee:  48 8b 5b 08            mov    0x8(%rbx),%rbx  /*%rbx = node<2>*/
79  4011f2:  83 ed 01               sub    $0x1,%ebp  /*%ebp--*/
80  4011f5:  75 e8                  jne    4011df <phase_6+0xeb>  /*if (%ebp != 0), jmp 4011df*/
81  4011f7:  48 83 c4 50            add    $0x50,%rsp
82  4011fb:  5b                     pop    %rbx
83  4011fc:  5d                     pop    %rbp
84  4011fd:  41 5c                  pop    %r12
85  4011ff:  41 5d                  pop    %r13
86  401201:  41 5e                  pop    %r14
86  401203:  c3                     retq   
##结束了

 ```

##### 第一部分

代码很长，但是别怕，让我们逐个击破，先提取出第一部分：

``` 

##第一部分

1   4010f4:  41 56                  push   %r14      /*一些准备工作*/
2   4010f6:  41 55                  push   %r13
3   4010f8:  41 54                  push   %r12
4   4010fa:  55                     push   %rbp
5   4010fb:  53                     push   %rbx
6   4010fc:  48 83 ec 50            sub    $0x50,%rsp /*栈指针减80*/
7   401100:  49 89 e5               mov    %rsp,%r13  /*%r13 = %rsp*/
8   401103:  48 89 e6               mov    %rsp,%rsi  /*%rsi = %rsp*/
9   401106:  e8 51 03 00 00         callq  40145c <read_six_numbers>  /*调用函数，读六个数字*/
10  40110b:  49 89 e6               mov    %rsp,%r14  /*%r14 = %rsp*/
11  40110e:  41 bc 00 00 00 00      mov    $0x0,%r12d  /*%r12d = 0*/
12  401114:  4c 89 ed               mov    %r13,%rbp  /*%rbp = %r13 = %rsp*/
13  401117:  41 8b 45 00            mov    0x0(%r13),%eax  /*%eax = (%rsp) = n[0]*/
14  40111b:  83 e8 01               sub    $0x1,%eax  /*%eax = (%rsp) - 1= n[0] - 1*/
15  40111e:  83 f8 05               cmp    $0x5,%eax  /*n[0] - 1与 5比较*/
16  401121:  76 05                  jbe    401128 <phase_6+0x34>  /*if (n[0] <= 6), jmp 401128*/
17  401123:  e8 12 03 00 00         callq  40143a <explode_bomb>  /*bomb*/
18  401128:  41 83 c4 01            add    $0x1,%r12d  /*%r12d++*/
19  40112c:  41 83 fc 06            cmp    $0x6,%r12d  /*if (%12d == 6), jmp 401153*/
20  401130:  74 21                  je     401153 <phase_6+0x5f>
21  401132:  44 89 e3               mov    %r12d,%ebx  /*%ebx = %12d*/
22  401135:  48 63 c3               movslq %ebx,%rax  /*%rax = %ebx = %12d*/
23  401138:  8b 04 84               mov    (%rsp,%rax,4),%eax  /*%eax = n[%rax]*/
24  40113b:  39 45 00               cmp    %eax,0x0(%rbp)  /*比较当前栈内存与当前输入数字*/
25  40113e:  75 05                  jne    401145 <phase_6+0x51>  /*if (n[%rax] == n[i]), bomb*/
26  401140:  e8 f5 02 00 00         callq  40143a <explode_bomb>
27  401145:  83 c3 01               add    $0x1,%ebx  /*%ebx++*/
28  401148:  83 fb 05               cmp    $0x5,%ebx  /*比较%ebx与5*/
29  40114b:  7e e8                  jle    401135 <phase_6+0x41>  /*if (%ebx <= 5), jmp 401135*/
30  40114d:  49 83 c5 04            add    $0x4,%r13  /*%r13 += 4*/
31  401151:  eb c1                  jmp    401114 <phase_6+0x20>  /*无条件跳转到401114*/
##

 ```

这题同第二题一样，也是调用了read_six_numbers，那么说明我们应该输入六个数字，并且这六个数字会被保存在内存 (%rsp) 到 (%rsp + 20) 中。对于第一部分，首先通过31行判定这是一个循环体，它将%r13 + 4，考虑第13行到16行，这个操作会使得我们输入的六个数(n[0-5])都会被强制性要求小于6，而在第18行到29行之间，又有一个内循环，它使得每一个数字之间两两对比，这段代码的等价C语言为：
```
for (int i = 0; i < 6; i++){
    if (n[0] > 6)
        Bomb!
    for (int j = i + 1; j < 6; j++){
        if (n[j] == n[i])
            Bomb!
    }
}
```

**所以，第一部分使得我们输入的六个数均小于等于6，且互不相同。**

##### 第二部分

```

##第二部分
32  401153:  48 8d 74 24 18         lea    0x18(%rsp),%rsi  /*%rsi = %rsp + 24*/
33  401158:  4c 89 f0               mov    %r14,%rax  /*%rax = %r14 = %rsp*/
34  40115b:  b9 07 00 00 00         mov    $0x7,%ecx  /*%ecx = 7*/
35  401160:  89 ca                  mov    %ecx,%edx  /*%edx = %ecx = 7*/
36  401162:  2b 10                  sub    (%rax),%edx  /*(%rax) = n[x], %edx = 7 - n[x]*/
37  401164:  89 10                  mov    %edx,(%rax)  /*(%rax) = %edx = 7 - n[x]*/
38  401166:  48 83 c0 04            add    $0x4,%rax  /*%rax = %rsp + 4*/
39  40116a:  48 39 f0               cmp    %rsi,%rax  /*比较%rsp + x 与 %rsp + 24*/
40  40116d:  75 f1                  jne    401160 <phase_6+0x6c>  /*if (x != 24), jmp 401160*/
##

 ```

**第二部分相对而言比较简单，它将我们输入的每一个数都用7减**，等价的C语言是：
```
for (int i = 0; i < 6; i++){
    int t = 7 - n[i];
    n[i] = t;
}
```

##### 第三部分

第三部分是最难的部分，我又将第三部分分为两个小部分，先来看看第一个小部分：
```
##第三部分
%rax = %rsp + 24
41  40116f:  be 00 00 00 00         mov    $0x0,%esi  /*%esi = 0*/
42  401174:  eb 21                  jmp    401197 <phase_6+0xa3>  /*jmp 40117*/
43  401176:  48 8b 52 08            mov    0x8(%rdx),%rdx  /*%rdx = (%rdx + 8)*/
44  40117a:  83 c0 01               add    $0x1,%eax  /*%eax++*/
45  40117d:  39 c8                  cmp    %ecx,%eax  /*比较%eax 与 %ecx*/
46  40117f:  75 f5                  jne    401176 <phase_6+0x82>  /*if (%eax != %%ecx), jmp 401176*/
47  401181:  eb 05                  jmp    401188 <phase_6+0x94>  /*jmp 401188*/
48  401183:  ba d0 32 60 00         mov    $0x6032d0,%edx  /*%edx = 0x6032d0*/
49  401188:  48 89 54 74 20         mov    %rdx,0x20(%rsp,%rsi,2)  /*%(%rsp + 2*%rsi + 0x20) = %rdx*/
50  40118d:  48 83 c6 04            add    $0x4,%rsi  /*%rsi += 4*/
51  401191:  48 83 fe 18            cmp    $0x18,%rsi  /*比较%rsp + 4x 与 %rsp + 24*/
52  401195:  74 14                  je     4011ab <phase_6+0xb7>  /*if ==, jmp 4011ab*/
53  401197:  8b 0c 34               mov    (%rsp,%rsi,1),%ecx  /*%ecx = n[%rsp / 4]*/
54  40119a:  83 f9 01               cmp    $0x1,%ecx  /*%ecx ? 1*/
55  40119d:  7e e4                  jle    401183 <phase_6+0x8f>  /*if (n[x] <= 1), jmp 401183*/
56  40119f:  b8 01 00 00 00         mov    $0x1,%eax  /*%eax = 1*/
57  4011a4:  ba d0 32 60 00         mov    $0x6032d0,%edx  /*%edx = 0x6032d0*/
58  4011a9:  eb cb                  jmp    401176 <phase_6+0x82>  /*jmp 401176*/
```

48行的一个地址很唐突，先来看看这里面放的是什么：
```

# (gdb) x/24w 0x6032d0

    # 0x6032d0 <node1>:       0x0000014c      0x00000001      0x006032e0      0x00000000
    # 0x6032e0 <node2>:       0x000000a8      0x00000002      0x006032f0      0x00000000
    # 0x6032f0 <node3>:       0x0000039c      0x00000003      0x00603300      0x00000000
    # 0x603300 <node4>:       0x000002b3      0x00000004      0x00603310      0x00000000
    # 0x603310 <node5>:       0x000001dd      0x00000005      0x00603320      0x00000000
    # 0x603320 <node6>:       0x000001bb      0x00000006      0x00000000      0x00000000
```

其实题目已经提示我们它是一个node了，它的第一个字节暂定，我们不知道是什么意思，它的第二个字节我们很敏感，这应该就是我们要输入的六个数了，第三个字节是下一个节点的地址，而下一字节为0，这让我们意识到这应该是一个大小为8字节的指针，保存这4字节大小的地址。那么我们可以看出它应该是一个这样的结构体：
```

struct node{
    int val;
    int data(要输入的六个数之一);
    struct node* next;
};
```

来看看它的汇编版本的C代码：
```

    goto 401197;
401176:
    do{
        %edx = *(%edx + 8);
        %eax++;
      }while (n[%rsi/4] != %eax)
    goto 401188;

401183:
    %edx = 0x6032d0;

401188:
    *(%rsp + 2 * %rsi + 0x20) = %edx;
    %rsi = %rsi + 4;
    if (%rsi == %rsp + 24)  gooto 4011ab;

401197:
    if (n[%rsi/4] <= 1)  goto 401183
    %eax = 1;
    %edx = 0x6032d0;
    goto 401176;
```

我们来将它翻译成等价的C代码：
```

for (int %rsi = 0; %rsi < 24; %rsi += 4){
    %ecx = nums[%rsi / 4];
    if (%ecx > 1){
        %eax = 1;
        %edx = 0x6032d0;
        do{
            %edx = *(%edx + 8);
            %eax++;
        }while (n[%rsi/4] != %eax)
    }else{
        %edx = 0x6032d0;
    }
    *(%rsp + 2 * %rsi + 0x20) = %edx;
}
```
此时我们可能还是不知道它做了什么，不妨来模拟一下，假设此时经过被7减之后，到达第三部分的六个数为6, 5, 4, 3, 2, 1。我们必须要知道C代码中的*(%edx + 8)的含义，由于0x6032d0是一个上面分析的结构体，那么初始时%edx = 0x6032d0为结构体的起始位置，而%edx + 8是结构体成员*next指针的地址，它保存着下一结构体的起始地址，那么*(%edx + 8)起始就是下一结构体的起始地址，那么经过这一段代码后，6使得%rdx循环五次变为node6，5使得%rdx循环4次变为node5...会有：
```

(%rsp + 0x20) = 0x603320 <node6>
(%rsp + 0x28) = 0x603310 <node5>
(%rsp + 0x30) = 0x603300 <node4>
(%rsp + 0x38) = 0x6032f0 <node3>
(%rsp + 0x40) = 0x6032e0 <node2>
(%rsp + 0x48) = 0x6032d0 <node1>

%rsi = %rsp + 0x50
```

*我们发现程序将(%rsp + 0x20)到(%rsp + 0x48)分别设置成了node6、node5、node4、node3、node2、node1，这恰好与我们当前数字6、5、4、3、2、1相吻合，这并不是偶然，这就是这段程序的作用，我们可以换个数字模拟，依然会得到同样的结果*。

继续看看另一个小部分：
```

#
59  4011ab:  48 8b 5c 24 20         mov    0x20(%rsp),%rbx    /*%rbx = (%rsp + 0x20)*/
60  4011b0:  48 8d 44 24 28         lea    0x28(%rsp),%rax  /*%rax = %rsp + 0x28*/
61  4011b5:  48 8d 74 24 50         lea    0x50(%rsp),%rsi  /*%rsi = %rsp + 0x50*/
62  4011ba:  48 89 d9               mov    %rbx,%rcx  /*%rcx = %rbx = (%rsp + 0x20)*/
63  4011bd:  48 8b 10               mov    (%rax),%rdx  /*%rdx = (%rax) = (%rsp + 0x28)*/
64  4011c0:  48 89 51 08            mov    %rdx,0x8(%rcx)  /*(%rsp + 0x28) = (%rsp + 0x28)*/
65  4011c4:  48 83 c0 08            add    $0x8,%rax  /*%rax= %rax + 8*/
66  4011c8:  48 39 f0               cmp    %rsi,%rax  /*%rax ? %rsp + 0x50*/
67  4011cb:  74 05                  je     4011d2 <phase_6+0xde>  /*if ==, jmp 4011d2*/
68  4011cd:  48 89 d1               mov    %rdx,%rcx  /*if !=, %rcx = %rdx*/
69  4011d0:  eb eb                  jmp    4011bd <phase_6+0xc9>  /*jmp 4011bd*/
70  4011d2:  48 c7 42 08 00 00 00 movq   $0x0,0x8(%rdx)  /*(rdx+8) = 0*/
##
```

不做任何优化的等价C语言为：
```
%rbx = *(%rsp + 0x20)
%rax = %rsp + 0x28
%rsi = %rsp + 0x50
%rcx = %rbx = *(%rsp + 0x20)

while (1){
    %rdx = *%rax;
    *(%rcx + 8) = %rdx;
    rax += 8;
    if (%rax == %rsp + 0x50)
    break;
    %rcx = %rdx;
}

*(rdx+8) = 0
```

让我们以刚刚得到的结果继续模拟一遍：

```

初始化：

%rbx = *(%rsp + 0x20) =  0x603320 <node6>;
%rax = %rsp + 0x28  # 计数器
%rsi = %rsp + 0x50  # 计数器 ， 共循环5次
%rcx = %rbx = 0x603320 <node6>

第一次循环：

%rdx =  *%rax = *(%rsp + 0x28) = 0x603310 <node5>;
*(%rcx + 8) = node6->next = 0x603310 <node5>;
%rax = %rsp + 0x30;
%rcx = 0x603310 <node5>;

#第一次循环使得<node6>的next指针指向了<node5>

第一次循环：

%rdx =  *%rax = *(%rsp + 0x30) = 0x603300 <node4>;
*(%rcx + 8) = node4->next = 0x603300 <node4>;
%rax = %rsp + 0x38;
%rcx = 0x603300 <node4>;

#第一次循环使得<node5>的next指针指向了<node4>

如此反复循环，共5次

 ```

很显然，这段代码在修正各节点的next指针的指向，一共执行五次，最后一个节点的next指针指向在程序的最后被修改为0，那么此时，地址(%rsp + 0x20)所存放的便是该链表的头指针，而后的每八个数据块中，存放的是下一个链表节点。如果我们将断点打到4011d2处，并让程序执行这条语句，我们再次审视0x6032d0的值：
```

# (gdb) x/24w 0x6032d0

  # 0x6032d0 <node1>:       0x0000014c      0x00000001      0x00000000      0x00000000
  # 0x6032e0 <node2>:       0x000000a8      0x00000002      0x006032d0      0x00000000
  # 0x6032f0 <node3>:       0x0000039c      0x00000003      0x006032e0      0x00000000
  # 0x603300 <node4>:       0x000002b3      0x00000004      0x006032f0      0x00000000
  # 0x603310 <node5>:       0x000001dd      0x00000005      0x00603300      0x00000000
  # 0x603320 <node6>:       0x000001bb      0x00000006      0x00603310      0x00000000

```
正如我们分析的，<node6>成为了头节点。

**总结一下第三部分代码，它将0x6032d0处的链表打乱，以当前达到的6位数字X<sub>1</sub>、X<sub>2</sub>、...、X<sub>6</sub>为基准，使得链表的顺序变为node<sub>X1</sub> -> node<sub>X2</sub> -> ... -> node<sub>X6</sub> -> 0x0，并将这些节点放在以%rsp + 0x20地址开始的连续8字节块的内存中。**

##### 第四部分
```
##第四部分
%rdx = 0x6032e0
%rbx = *(%rsp + 0x20)
71  4011d9:  00
72  4011da:  bd 05 00 00 00         mov    $0x5,%ebp  /*%ebp = 5*/
73  4011df:  48 8b 43 08            mov    0x8(%rbx),%rax  /*%rax = (%rbx + 8) = node<2>->val*/
74  4011e3:  8b 00                  mov    (%rax),%eax  /*%eax = node<2>->val*/
75  4011e5:  39 03                  cmp    %eax,(%rbx)  /*node<1>->val ? node<2>->val*/
76  4011e7:  7d 05                  jge    4011ee <phase_6+0xfa>  /*if <, bomb!*/
77  4011e9:  e8 4c 02 00 00         callq  40143a <explode_bomb>
78  4011ee:  48 8b 5b 08            mov    0x8(%rbx),%rbx  /*%rbx = node<2>*/
79  4011f2:  83 ed 01               sub    $0x1,%ebp  /*%ebp--*/
80  4011f5:  75 e8                  jne    4011df <phase_6+0xeb>  /*if (%ebp != 0), jmp 4011df*/
81  4011f7:  48 83 c4 50            add    $0x50,%rsp
82  4011fb:  5b                     pop    %rbx
83  4011fc:  5d                     pop    %rbp
84  4011fd:  41 5c                  pop    %r12
85  4011ff:  41 5d                  pop    %r13
86  401201:  41 5e                  pop    %r14
86  401203:  c3                     retq   
##结束了

```
观察74到80行，这是一个循环，它严格保证下一节点的val值小于当前节点的val值，这就意味着我们经过第三部分得到的新链表的val值必须是单调递减的，我们来看看最初的链表：
```

    # 0x6032d0 <node1>:       0x0000014c(332)      0x00000001      0x006032e0      0x00000000
    # 0x6032e0 <node2>:       0x000000a8(168)      0x00000002      0x006032f0      0x00000000
    # 0x6032f0 <node3>:       0x0000039c(924)      0x00000003      0x00603300      0x00000000
    # 0x603300 <node4>:       0x000002b3(691)      0x00000004      0x00603310      0x00000000
    # 0x603310 <node5>:       0x000001dd(477)      0x00000005      0x00603320      0x00000000
    # 0x603320 <node6>:       0x000001bb(443)      0x00000006      0x00000000      0x00000000
```

可以看出val值递减的顺序是node3 -> node4 -> node5 -> node6 -> node1 -> node2，根据我们第三部分的结论，到达第三部分的数据必须是 3 4 5 6 1 2，而它又是被7减过的，那么结束了，答案是：

>4 3 2 1 6 5
