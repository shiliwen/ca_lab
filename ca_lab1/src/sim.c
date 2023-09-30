#include <stdio.h>
#include "shell.h"


//全局变量
uint8_t opcode = 0x0;
uint8_t rs = 0x0;
uint8_t rt = 0x0;
uint8_t rd = 0x0;
uint8_t shamt = 0x0;
uint8_t funct = 0x0;
uint16_t immediate = 0x0;
uint32_t target = 0x0;

//指令对应函数
void J(){
    //26位目标地址左移两位，并与延迟槽地址的高阶位结合。程序无条件跳转到这个计算地址，延迟一条指令
    NEXT_STATE.PC = (CURRENT_STATE.PC & 0xf0000000) | (target << 2);
}
void JAL(){
    //26位目标地址左移两位，并与延迟槽地址的高阶位结合。程序无条件跳转到这个计算地址，延迟一条指令。延迟槽后的指令的地址放在链接寄存器r31中。
    NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
    NEXT_STATE.PC = (CURRENT_STATE.PC & 0xf0000000) | (target << 2);
}
void BEQ(){
    //分支目标地址是从延迟槽中的指令地址和16位偏移量的总和中计算出来的，左移两位并进行符号扩展。对rs和rt进行比较。如果相等，则程序分支到目标地址，并延迟一条指令。
    uint32_t offset = (uint32_t)(immediate << 2) ;
    if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt]) 
    {
        NEXT_STATE.PC = CURRENT_STATE.PC + offset + 4;
    } 
    else  
    {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}
void BNE(){
    //分支目标地址是从延迟槽中的指令地址和16位偏移量的总和中计算出来的，左移两位并进行符号扩展。对rs和rt进行比较。如果不等，则程序分支到目标地址，并延迟一条指令。
    uint32_t offset = (uint32_t)(int32_t)(int16_t)(immediate << 2) ;
    if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt]) 
    {
        NEXT_STATE.PC = CURRENT_STATE.PC + offset + 4;
    } 
    else  
    {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}
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
void BGTZ(){
    //分支目标地址是从延迟槽中的指令地址和16位偏移量的总和中计算出来的，左移两位并进行符号扩展。如果寄存器rs的内容已清除符号位并且不等于零，则程序将分支到目标地址，并延迟一条指令
    uint32_t offset = (uint32_t)(int32_t)(int16_t)(immediate << 2) ;
    if (rt == 0) 
    {
        if ((CURRENT_STATE.REGS[rs] & 0x80000000) == 0 &&CURRENT_STATE.REGS[rs] != 0)
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
        printf("Error in BGTZ \n");
    }
}
void ADDI(){
    //立即数加法,有符号
    NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + (uint32_t)(int32_t)(int16_t)immediate;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void ADDIU(){
    //立即数加法,无符号
    NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + (uint32_t)(int32_t)(int16_t)immediate;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void SLTI(){
    //有符号比较
    NEXT_STATE.REGS[rt] = (int32_t)CURRENT_STATE.REGS[rs] < (int32_t)immediate;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void SLTIU(){
    // 无符号比较
    NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] < (uint32_t)(int32_t)(int16_t)immediate;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void ANDI(){
    //逻辑与，高位填充0
    NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & (immediate & 0x0000FFFF);
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void ORI(){
    //逻辑或
    NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | (immediate & 0x0000FFFF);
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void XORI(){
    //逻辑异或
    NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] ^ (immediate & 0x0000FFFF);
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void LUI(){
    //高位加载
    NEXT_STATE.REGS[rt] = (uint32_t)immediate << 16;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void LB(){
    uint32_t addr = (uint32_t)(int32_t)(int16_t)immediate + CURRENT_STATE.REGS[rs];
    uint32_t byte = mem_read_32(addr) & 0xFF;
    NEXT_STATE.REGS[rt] = byte > 0x7F? byte | 0xFFFFFF00: byte;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void LH(){
    uint32_t addr = (uint32_t)(int32_t)(int16_t)immediate + CURRENT_STATE.REGS[rs];
    uint32_t byte = mem_read_32(addr) & 0xFFFF;
    NEXT_STATE.REGS[rt] = byte > 0x7FFF? byte | 0xFFFF0000: byte;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void LW(){
    uint32_t addr = (uint32_t)(int32_t)(int16_t)(immediate) + CURRENT_STATE.REGS[rs];
    NEXT_STATE.REGS[rt] = mem_read_32(addr);
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void LBU(){
    uint32_t addr = CURRENT_STATE.REGS[rs] + (int16_t)immediate;
    NEXT_STATE.REGS[rt] = (uint32_t) (mem_read_32(addr) & 0x000000FF);
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void LHU(){
    uint32_t addr = CURRENT_STATE.REGS[rs] + (int16_t)immediate;
    NEXT_STATE.REGS[rt] = (uint32_t) (mem_read_32(addr) & 0x0000FFFF);
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void SB(){
    uint32_t addr = (uint32_t)(int32_t)(int16_t)immediate + CURRENT_STATE.REGS[rs];
    uint32_t val = (mem_read_32(addr) & 0xFFFFFF00) |(CURRENT_STATE.REGS[rt] & 0xFF);
    mem_write_32(addr, val);
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void SH(){
    uint32_t addr = (uint32_t)(int32_t)(int16_t)immediate + CURRENT_STATE.REGS[rs];
    uint32_t val = (mem_read_32(addr) & 0xFFFF0000) |(CURRENT_STATE.REGS[rt] & 0xFFFF);
    mem_write_32(addr, val);
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void SW(){
    uint32_t addr = (uint32_t)(int32_t)(int16_t)immediate + CURRENT_STATE.REGS[rs];
    mem_write_32(addr, CURRENT_STATE.REGS[rt]);
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void BLTZ(){
    if ((CURRENT_STATE.REGS[rs] & 0x80000000) != 0) 
    {
        NEXT_STATE.PC = CURRENT_STATE.PC + ((uint32_t)(int32_t)(int16_t)immediate << 2) + 4;
    } 
    else 
    {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}
void BGEZ(){
    if ((CURRENT_STATE.REGS[rs] & 0x80000000) == 0) 
    {
        NEXT_STATE.PC = CURRENT_STATE.PC + ((uint32_t)(int32_t)(int16_t)immediate << 2) + 4;
    } 
    else 
    {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}
void BLTZAL(){
    NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
    if ((CURRENT_STATE.REGS[rs] & 0x80000000) != 0) 
    {
        NEXT_STATE.PC = CURRENT_STATE.PC + ((uint32_t)(int32_t)(int16_t)immediate << 2) + 4;
    } 
    else 
    {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}
void BGEZAL(){
    NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
    if ((CURRENT_STATE.REGS[rs] & 0x80000000) == 0) 
    {
        NEXT_STATE.PC = CURRENT_STATE.PC + ((uint32_t)(int32_t)(int16_t)immediate << 2) + 4;
    } 
    else 
    {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}
void SLL(){
    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << shamt;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void SRL(){
    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shamt;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void SRA(){
    NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rt] >> shamt;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void SLLV(){
    uint32_t shamt = CURRENT_STATE.REGS[rs] & 0x1F;
    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << shamt;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void SRLV(){
    uint32_t shamt = CURRENT_STATE.REGS[rs] & 0x1F;
    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shamt;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void SRAV(){
    NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F);
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void JR(){
    NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
}
void JALR(){
    uint8_t d = rd == 0? 31: rd;
    NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
    NEXT_STATE.REGS[d] = CURRENT_STATE.PC + 4;
}
void ADD(){
    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void ADDU(){
    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void SUB(){
    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void SUBU(){
    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void AND(){
    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void OR(){
    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void XOR(){
    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void NOR(){
    NEXT_STATE.REGS[rd] = ~( CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt] );
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void SLT(){
    NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rs] < (int32_t)CURRENT_STATE.REGS[rt];
}
void SLTU(){
    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt];
}
void MULT(){
    int64_t product = (int32_t)CURRENT_STATE.REGS[rs] * (int32_t)CURRENT_STATE.REGS[rt];
    uint64_t uint_product = *((uint32_t*)&product);
    NEXT_STATE.HI =  (uint32_t)((uint_product >> 32) & 0xFFFFFFFF);
    NEXT_STATE.LO = (uint32_t)(uint_product & 0xFFFFFFFF);
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void MULTU(){
    int64_t product = CURRENT_STATE.REGS[rs] * CURRENT_STATE.REGS[rt];
    uint64_t uint_product = *((uint32_t*)&product);
    NEXT_STATE.HI =  (uint32_t)((uint_product >> 32) & 0xFFFFFFFF);
    NEXT_STATE.LO = (uint32_t)(uint_product & 0xFFFFFFFF);
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void DIV(){
    NEXT_STATE.LO = (int32_t)CURRENT_STATE.REGS[rs] / (int32_t)CURRENT_STATE.REGS[rt];
    NEXT_STATE.HI = (int32_t)CURRENT_STATE.REGS[rs] % (int32_t)CURRENT_STATE.REGS[rt];
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void DIVU(){
    NEXT_STATE.LO = (uint32_t)CURRENT_STATE.REGS[rs] / (uint32_t)CURRENT_STATE.REGS[rt];
    NEXT_STATE.HI = (uint32_t)CURRENT_STATE.REGS[rs] % (uint32_t)CURRENT_STATE.REGS[rt];
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void MFHI(){
    NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void MFLO(){
    NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void MTHI(){
    NEXT_STATE.HI = CURRENT_STATE.REGS[rs];
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void MTLO(){
    NEXT_STATE.LO = CURRENT_STATE.REGS[rs];
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
void SYSCALL(){
    if(CURRENT_STATE.REGS[2] == 0x0A)
    {
        RUN_BIT = FALSE;  // 退出程序 
    }      
    else 
    {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}


void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */

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

    if(inst == 0)
        return ;
    else
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
                    
                case 0x4:
                    SLLV();
                    break;
                    
                case 0x6:
                    SRLV();
                    break;
                
                case 0x7:
                    SRAV();
                    break;
                
                case 0x8:
                    JR();
                    break;
                
                case 0x9:
                    JALR();
                    break;
                
                case 0xC:
                    SYSCALL();
                    break;
                
                case 0x10:
                    MFHI();
                    break;
                
                case 0x11:
                    MTHI();
                    break;
                
                case 0x12:
                    MFLO();
                    break;
                
                case 0x13:
                    MTLO();
                    break;
                
                case 0x18:
                    MULT();
                    break;
                
                case 0x19:
                    MULTU();
                    break;
                
                case 0x1A:
                    DIV();
                    break;
                
                case 0x1B:
                    DIVU();
                    break;
                
                case 0x20:
                    ADD();
                    break;
                
                case 0x21:
                    ADDU();
                    break;
                
                case 0x22:
                    SUB();
                    break;
                
                case 0x23:
                    SUBU();
                    break;
                
                case 0x24:
                    AND();
                    break;
                
                case 0x25:
                    OR();
                    break;
                
                case 0x26:
                    XOR();
                    break;
                
                case 0x27:
                    NOR();
                    break;
                
                case 0x2A:
                    SLT();
                    break;
                
                case 0x2B:
                    SLTU();
                    break;
                
                default:
                    printf("Unkown Instruction \n");
                    break;
                }
            }
            break;
        
   
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

        //I型指令
        case 0x1: 
            {
                switch (rt)
                {
                case 0x0:
                    BLTZ();
                    break;
                
                case 0x1:
                    BGEZ();
                    break;
                
                case 0x10:
                    BLTZAL();
                    break;
                
                case 0x11:
                    BGEZAL();
                    break;
                
                default:
                    printf("Unkown Instruction \n");
                    break;
                }
            }
            break;
        
        case 0x4: 
            {
                BEQ();
            }
            break;
        
        case 0x5:
            {
                BNE();
            }
            break;
        
        case 0x6:
            {
                BLEZ();
            }
            break;
        
        case 0x7:
            {
                BGTZ();
            }
            break;

        case 0x8: 
            {
                ADDI();
            }
            break;
        
        case 0x9: 
            {
                ADDIU();
            }
            break;
        
        case 0xA: 
            {
                SLTI();
            }
            break;

        case 0xB: 
            {
                SLTIU();
            }
            break;

        case 0xC: 
            {
                ANDI();
            }
            break;
        
        case 0xD: 
            {
                ORI();
            }
            break;
        
        case 0xE: 
            {
                XORI();
            }
            break;
        
        case 0xF: 
            {
                LUI();
            }
            break;
        
        case 0x20: 
            {
                LB();
            }
            break;
        
        case 0x21: 
            {
                LH();
            }
            break;
        
        case 0x23: 
            {
                LW();
            }
            break;
        
        case 0x24: 
            {
                LBU();
            }
            break;
        
        case 0x25: 
            {
                LHU();
            }
            break;
        
        case 0x28: 
            {
                SB();
            }
            break;
        
        case 0x29: 
            {
                SH();
            }
            break;
        
        case 0x2B: 
            {
                SW();
            }
            break;
        
        default:
            printf("Unkown Instruction \n");
            break;
        }
 for(int i = 0 ; i < 32 ; i++)
 {
 	printf("REG%d 0x%08x\n",i, NEXT_STATE.REGS[i]);
 }
}
