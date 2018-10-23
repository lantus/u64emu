#ifndef DYNA_NATIVE
#define DYNA_NATIVE


extern WORD PUSH_STACK_POINTER(BYTE *cp);
extern WORD LOAD_REG_DWORD(BYTE *cp,BYTE Reg,BYTE *Address);
extern WORD CALL_REGISTER(BYTE *cp,BYTE Reg);
extern WORD PUSH_DWORD(BYTE *cp,DWORD Value);
extern WORD POP_REGISTER(BYTE *cp,BYTE Reg);
extern WORD PUSH_REGISTER(BYTE *cp,BYTE Reg);
extern WORD STORE_REG(BYTE *cp, WORD MipsRegs,BYTE NativeReg);
extern WORD LOAD_REG(BYTE *cp,BYTE MipsRegs, WORD NativeReg);
extern WORD SUB_REG_IMM(BYTE *cp,BYTE NativeReg,DWORD Immediate);
extern WORD ADD_REG_IMM(BYTE *cp,BYTE NativeReg,DWORD Immediate);
extern WORD ADD_REG_IMM_BYTE(BYTE *cp,BYTE NativeReg,BYTE Immediate);
extern WORD NOP(BYTE *cp);
extern WORD AND_REG_IMM(BYTE *cp,BYTE NativeReg,DWORD Immediate);
extern WORD AND_REG_IMM_BYTE(BYTE *cp,BYTE NativeReg,BYTE Immediate);
extern WORD OR_REG_IMM(BYTE *cp,BYTE NativeReg,DWORD Immediate);
extern WORD XOR_REG_IMM(BYTE *cp,BYTE NativeReg,DWORD Immediate);
extern WORD NOT_REG(BYTE *cp,BYTE NativeReg);
extern WORD CMP_REG_IMM(BYTE *cp,BYTE NativeReg,DWORD Immediate);
extern WORD CMP_RBANK_WITH_IMM(BYTE *cp,DWORD Offset,DWORD Immediate);
extern WORD CMP_RBANK_WITH_IMM_BYTE(BYTE *cp,DWORD Offset,BYTE Immediate);
extern WORD STORE_MEM_REG(BYTE *cp,BYTE DstRegPtr,BYTE Reg);
extern WORD STORE_MEM_WORD_REG(BYTE *cp,BYTE DstRegPtr,BYTE Reg);
extern WORD STORE_MEM_BYTE_REG(BYTE *cp,BYTE DstRegPtr,BYTE Reg);
extern WORD LOAD_MEM_REG(BYTE *cp,BYTE DstRegPtr,BYTE Reg);
extern WORD JMP_SHORT(BYTE *cp,BYTE off);
extern WORD JMP_LONG(BYTE *cp,DWORD Address);
extern WORD NATIVE_REG_TO_REG(BYTE *cp,BYTE r1,BYTE r2);
extern WORD ADD_REG_TO_DWORD(BYTE *cp,BYTE r1,DWORD Address);
extern WORD ADD_DWORD_TO_REG(BYTE *cp,BYTE r1,DWORD Address);
extern WORD INC_RBANK(BYTE *cp,DWORD Address);
extern WORD ADD_IMM_TO_RBANK_BYTE(BYTE *cp,BYTE Immediate,DWORD Address);
extern WORD ADD_IMM_TO_RBANK(BYTE *cp,DWORD Immediate,DWORD Address);
extern WORD AND_RBANK_WITH_IMM(BYTE *cp,DWORD Immediate,DWORD Address);
extern WORD OR_RBANK_WITH_REG(BYTE *cp,BYTE Reg,DWORD Address);
extern WORD ZERO_EXTEND_REG(BYTE *cp,BYTE reg);
extern WORD SUB_REG_FROM_DWORD(BYTE *cp,BYTE r1,DWORD Address);
extern WORD SUB_DWORD_FROM_REG(BYTE *cp,BYTE r1,DWORD Address);
extern WORD AND_REG_WITH_DWORD(BYTE *cp,BYTE r1,DWORD Address);
extern WORD AND_DWORD_WITH_IMM(BYTE *cp,DWORD Immediate,DWORD Address);
extern WORD OR_IMM_WITH_DWORD(BYTE *cp,DWORD Immediate,DWORD Address);
extern WORD NOT_DWORD(BYTE *cp,DWORD Address);
extern WORD XOR_IMM_WITH_DWORD(BYTE *cp,DWORD Immediate,DWORD Address);
extern WORD CMP_DWORD_WITH_REG(BYTE *cp,BYTE r1,DWORD Address);
extern WORD AND_DWORD_WITH_REG(BYTE *cp,BYTE r1,DWORD Address);
extern WORD XOR_REG_WITH_DWORD(BYTE *cp,BYTE r1,DWORD Address);
extern WORD XOR_DWORD_WITH_REG(BYTE *cp,BYTE r1,DWORD Address);
extern WORD OR_REG_WITH_DWORD(BYTE *cp,BYTE r1,DWORD Address);
extern WORD OR_DWORD_WITH_REG(BYTE *cp,BYTE r1,DWORD Address);
extern WORD ADC_REG_TO_DWORD(BYTE *cp,BYTE r1,DWORD Address);
extern WORD ADC_DWORD_TO_REG(BYTE *cp,BYTE r1,DWORD Address);
extern WORD SBB_REG_FROM_DWORD(BYTE *cp,BYTE r1,DWORD Address);
extern WORD SBB_DWORD_FROM_REG(BYTE *cp,BYTE r1,DWORD Address);
extern WORD JZ_SHORT(BYTE *cp,BYTE off);
extern WORD JNZ_SHORT(BYTE *cp,BYTE off);
extern WORD JG_SHORT(BYTE *cp,BYTE off);
extern WORD JL_SHORT(BYTE *cp,BYTE off);
extern WORD JB_SHORT(BYTE *cp,BYTE off);
extern WORD JB_LONG(BYTE *cp,int Offset);
extern WORD JLE_SHORT(BYTE *cp,BYTE off);
extern WORD JGE_SHORT(BYTE *cp,BYTE off);
extern WORD JAE_SHORT(BYTE *cp,BYTE off);
extern WORD JE_SHORT(BYTE *cp,BYTE off);
extern WORD JNE_SHORT(BYTE *cp,BYTE off);
extern WORD JAE_LONG(BYTE *cp,int Offset);
extern WORD JBE_SHORT(BYTE *cp,BYTE off);
extern WORD PUSH_ALL(BYTE *cp);
extern WORD POP_ALL(BYTE *cp);
extern WORD ZERO_REG(BYTE *cp,BYTE NativeReg);
extern WORD REG_COMPARE_IMM(BYTE *cp,BYTE NativeReg,DWORD Immediate);
extern WORD PREDICT_REG_INC(BYTE *cp,BYTE NativeReg);
extern WORD JGE_RELATIVE(BYTE *cp,DWORD Offset);
extern WORD REG_INC(BYTE *cp,BYTE NativeReg);
extern WORD SRA_REG_IMM(BYTE *cp,BYTE Reg,DWORD Immediate);
extern WORD SRL_REG_IMM(BYTE *cp,BYTE Reg,DWORD Immediate);
extern WORD SLL_REG_IMM(BYTE *cp,BYTE Reg,DWORD Immediate);
extern WORD SHLD_REG_IMM(BYTE *cp,BYTE Immediate);
extern WORD SHLD_REG_REG(BYTE *cp);
extern WORD SHLD_REG_REG2(BYTE *cp);
extern WORD SHRD_REG_IMM(BYTE *cp,BYTE Immediate);
extern WORD SHRD_REG_REG(BYTE *cp);
extern WORD SHRD_REG_REG2(BYTE *cp);
extern WORD SRA_REG_REG(BYTE *cp,BYTE Reg,BYTE ShiftReg);
extern WORD SUB_NATIVE_REG_REG(BYTE *cp,BYTE r1,BYTE r2);
extern WORD SRL_REG_REG(BYTE *cp,BYTE Reg,DWORD Immediate);
extern WORD SLL_REG_REG(BYTE *cp,BYTE Reg,DWORD Immediate);
extern WORD REG_TO_REG(BYTE *cp,BYTE MipsDst,BYTE MipsSrc);
extern WORD LOAD_REG_FROM_RBANK(BYTE *cp,BYTE NativeReg,DWORD Offset);
extern WORD STORE_REG_TO_RBANK(BYTE *cp,BYTE NativeReg,DWORD Offset);
extern WORD STORE_DWORD_TO_RBANK(BYTE *cp,DWORD Value,DWORD Offset);
extern WORD MEM_TO_MEM_QWORD(BYTE *cp,DWORD DstOff,DWORD SrcOff);
extern WORD MEM_TO_MEM_DWORD(BYTE *cp,DWORD DstOff,DWORD SrcOff);
extern WORD IMUL(BYTE *cp,BYTE op0,BYTE op1);		// op0 is ignored
extern WORD MUL(BYTE *cp,BYTE op0,BYTE op1);		// op0 is ignored
extern WORD IDIV(BYTE *cp,BYTE op0,BYTE op1);		// op0 is ignored
extern WORD DIV(BYTE *cp,BYTE op0,BYTE op1);		// op0 is ignored
extern WORD CONVERT_TO_QWORD(BYTE *cp);
extern WORD CONVERT_TO_DWORD(BYTE *cp);
extern WORD CONVERT_TO_WORD(BYTE *cp);
extern WORD REG_TO_MEM_DWORD(BYTE *cp,DWORD DstOff,BYTE Reg);
extern WORD CLD(BYTE *cp);
extern WORD STD(BYTE *cp);
extern WORD REP_MOVE_STORE_BYTE(BYTE *cp);
extern WORD LOAD_REG_IMM(BYTE *cp,BYTE Reg,DWORD Immediate);
extern WORD LOAD_REG_FROM_MEMORY(BYTE *cp,BYTE Reg,DWORD Address);
extern WORD CALL_CHECKINTS(BYTE *cp);
extern WORD JMP_REGISTER(BYTE *cp,BYTE reg);
extern WORD JNE_LONG(BYTE *cp,int Offset);
extern WORD JE_LONG(BYTE *cp,int Offset);
extern WORD JG_LONG(BYTE *cp,int Offset);
extern WORD JGE_LONG(BYTE *cp,int Offset);
extern WORD JL_LONG(BYTE *cp,int Offset);
extern WORD JLE_LONG(BYTE *cp,int Offset);
extern WORD JZ_LONG(BYTE *cp,int Offset);		// same as JE
extern WORD JNZ_LONG(BYTE *cp,int Offset);		// same as JNE
extern WORD J_LONG(BYTE *cp,int Offset);
extern WORD RETURN(BYTE *cp);
extern WORD INC_PC_COUNTER(BYTE *cp);
extern WORD INC_PC_COUNTER_S(BYTE *cp);
extern WORD CALL_FUNCTION(BYTE *cp,DWORD address);
extern WORD MOVSD(BYTE *cp);
extern WORD LODSD(BYTE *cp);
extern WORD STOSD(BYTE *cp);
extern WORD NATIVE_OR_REG_WITH_EAX(BYTE *cp,BYTE r1);
extern WORD ADD_NATIVE_REG_REG(BYTE *cp,BYTE r1,BYTE r2);
extern WORD SHL_RBANK_IMM(BYTE *cp,DWORD addy,BYTE amount);
extern WORD SHR_RBANK_IMM(BYTE *cp,DWORD addy,BYTE amount);
extern WORD SAR_RBANK_IMM(BYTE *cp,DWORD addy,BYTE amount);
extern WORD STORE_D_GPR(BYTE *cp);
extern WORD LOAD_D_GPR(BYTE *cp,DWORD Offset1,DWORD Offset2);
extern WORD STORE_GPR(BYTE *cp);
extern WORD LOAD_GPR(BYTE *cp,DWORD Offset1,DWORD Offset2);
extern WORD STORE_SE_D_GPR(BYTE *cp);


extern WORD MOVSX_NATIVE_BPTR_NATIVE(BYTE *cp,BYTE n1,BYTE n2);
extern WORD MOVZX_NATIVE_BPTR_NATIVE(BYTE *cp,BYTE n1,BYTE n2);
extern WORD MOVSX_NATIVE_WPTR_NATIVE(BYTE *cp,BYTE n1,BYTE n2);
extern WORD MOVZX_NATIVE_WPTR_NATIVE(BYTE *cp,BYTE n1,BYTE n2);
extern WORD MOV_NATIVE_DPTR_NATIVE(BYTE *cp,BYTE n1,BYTE n2);
extern WORD MOV_NATIVE_GPR(BYTE *cp,BYTE n1,char Offset);
extern WORD MOV_GPR_NATIVE(BYTE *cp,char Offset,BYTE n1);
extern WORD MOV_NATIVE_NATIVE(BYTE *cp,BYTE n1,BYTE n2);
extern WORD ADD_NATIVE_IMM(BYTE *cp,BYTE n1,DWORD Immediate);
extern WORD ADD_NATIVE_DPTR_EAX4(BYTE *cp,BYTE n1,DWORD Offset);
extern WORD SHR_NATIVE_IMM(BYTE *cp,BYTE n1,BYTE Imm);
extern WORD XOR_NATIVE_NATIVE(BYTE *cp,BYTE n1,BYTE n2);
extern WORD MOV_BPTR_NATIVE_NATIVE(BYTE *cp,BYTE n1,BYTE n2);
extern WORD MOV_WPTR_NATIVE_NATIVE(BYTE *cp,BYTE n1,BYTE n2);
extern WORD MOV_DPTR_NATIVE_NATIVE(BYTE *cp,BYTE n1,BYTE n2);
extern WORD MOV_NATIVE_FPR(BYTE *cp,BYTE n1,DWORD Offset);
extern WORD MOV_FPR_NATIVE(BYTE *cp,DWORD Offset,BYTE n1);
extern WORD SAR_NATIVE_IMM(BYTE *cp,BYTE n1,BYTE Imm);
extern WORD MOV_NATIVE_DPTR_NATIVE_OFFSET(BYTE *cp,BYTE n0,BYTE n1,DWORD Imm);
extern WORD MOV_DPTR_NATIVE_OFFSET_NATIVE(BYTE *cp,BYTE n1,BYTE n2,DWORD Offset);
extern WORD MOV_WPTR_NATIVE_OFFSET_NATIVE(BYTE *cp,BYTE n1,BYTE n2,DWORD Offset);
extern WORD MOV_BPTR_NATIVE_OFFSET_NATIVE(BYTE *cp,BYTE n1,BYTE n2,DWORD Offset);
extern WORD MOVSX_NATIVE_BPTR_NATIVE_OFFSET(BYTE *cp,BYTE n1,BYTE n2,DWORD Offset);
extern WORD MOVZX_NATIVE_BPTR_NATIVE_OFFSET(BYTE *cp,BYTE n1,BYTE n2,DWORD Offset);
extern WORD MOVSX_NATIVE_WPTR_NATIVE_OFFSET(BYTE *cp,BYTE n1,BYTE n2,DWORD Offset);
extern WORD MOVZX_NATIVE_WPTR_NATIVE_OFFSET(BYTE *cp,BYTE n1,BYTE n2,DWORD Offset);



#endif