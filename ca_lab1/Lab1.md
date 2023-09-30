## 实验1:指令级MIPS模拟器

#### 实现指令：

本次编写的c语言程序实现了如下53条指令：

`J JAL BEQ BNE BLEZ BGTZ ADDI ADDIU SLTI SLTIU ANDI ORI XORI LUI LB LH LW LBU LHU SB SH SW BLTZ BGEZ BLTZAL BGEZAL SLL SRL SRA SLLV SRLV  SRAV JR  JALR  ADD  ADDU SUB SUBU AND OR XOR NOR SLT SLTU MULT MFHI MFLO MTHI MTLO MULTU DIV DIVU SYSCALL`

#### 实验流程：

1.将汇编代码转化为mips指令（将.s文件汇编成.x文件）

2.编写sim.c文件，实现mips指令的读取和处理

3.通过makefile，将sim.c编译成可执行程序，

#### 指令处理过程：

1.在函数`process_instruction()` 开始执行前，对mips指令的格式进行划分，并定义为全局变量。

```
//全局变量
uint8_t opcode = 0x0;
uint8_t rs = 0x0;
uint8_t rt = 0x0;
uint8_t rd = 0x0;
uint8_t shamt = 0x0;
uint8_t funct = 0x0;
uint16_t immediate = 0x0;
uint32_t target = 0x0;

```



2.通过shell.h里的`mem_read_32`函数，读入一条指令，然后将指令分割，进行解析

```
//获取指令
    uint32_t inst = mem_read_32(CURRENT_STATE.PC);
    
    printf("Instruction: 0x%08x\n", inst);
	
    //解析指令,对应赋值
    opcode = (inst>> 26) & 0x3F;
    rs = (inst >> 21) & 0x1F;
    rt = (inst >> 16) & 0x1F;
    rd = (inst >> 11) & 0x1F;
    shamt = (inst >> 6) & 0x1F;
    funct = inst & 0x3F;
    immediate = inst & 0xFFFF;
    target = inst & 0x3FFFFFF;

```

3.首先通过 `switch` 语句，根据 `opcode` 是否为0，来识别是否为R型指令。

对于R型指令，我们再根据其`funct`字段，来识别具体指令，对于不能正确识别的指令，输出"Unkown Instruction "。

```
switch (opcode)
        {
        case 0x0: //R型指令
            {
                switch (funct)
                {
                case 0x0:
                    SLL();
                    break;
                
                case 0x2:
                    SRL();
                    break;

                case 0x3:
                    SRA();
                    break;
                
                ……
                
                case 0x2B:
                    SLTU();
                    break;
                
                default:
                    printf("Unkown Instruction \n");
                    break;
                }
            }
```

对于J型指令，主要有两个`J`和`JAL`，对应的`opcode`分别为0x2和0x3，如下：

```
   case 0x2: //j
            {
                J();
            }
            break;

   case 0x3: //jal
            {
                JAL();
            }
            break;
```

剩余大部分指令为I型指令，我们也通过`switch`进行识别和处理。

4.我们正确识别指令后，会通过调用对应函数进行处理，下面我们以R、I、J型指令各一条样例来简单解析。

R型指令 `ADDU`

对于无符号加法，我们通过`NEXT_STATE.REGS`对于模拟寄存器进行读取和赋值，并在处理完成后，将程序计数器PC加4，为读取下一条指令做准备。

```
void ADDU(){
    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

```

I型指令 `BLEZ`

对于该指令，我们先对立即数进行符号扩展。并对寄存器rs的内容是否已清除符号位并且不等于零进行判定，进而决定是否跳转到目标地址。

```
void BLEZ(){
    //分支目标地址是从延迟槽中的指令地址和16位偏移量的总和中计算出来的，左移两位并进行符号扩展。如果寄存器rs的内容已清除符号位并且不等于零，则程序将分支到目标地址，并延迟一条指令
    uint32_t offset = (uint32_t)(int32_t)(int16_t)(immediate << 2) ;
    if (rt == 0) 
    {
        if ((CURRENT_STATE.REGS[rs] & 0x80000000) != 0 &&CURRENT_STATE.REGS[rs] != 0)
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + offset + (uint32_t)4;
        } 
        else 
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        }
    } 
    else 
    {
        printf("Error in BLEZ.\n");
    }
}
```

J型指令 `J`

我们先将26位目标地址进行扩展，即将`target`左移2位，前面剩余的4位使用当前PC的前4位，然后进行无条件跳转。

```
void J(){
    //26位目标地址左移两位，并与延迟槽地址的高阶位结合。程序无条件跳转到这个计算地址，延迟一条指令
    NEXT_STATE.PC = (CURRENT_STATE.PC & 0xf0000000) | (target << 2);
}

```

其余各种指令同样按照 MIPSISA 手册进行编写，不再赘述。

#### 测试结果：

由于spim447的版本过早，使用的是C++11的标准，实验提供的py程序asm2hex无法正常运行，我们选择使用较新版本的qtspim将汇编代码转化成mips指令，并在可视化界面截取指令码，手动写入.x文件。我们还额外编写并测试了extra.s文件。

对比我们编写的模拟器输出的REG值和spim的结果，发现结果相同。

#### 重难点：

编写mips指令执行过程较为困难的点是符号扩展部分。

#### 代码地址：

https://github.com/shiliwen/ca_lab/tree/main/ca_lab1