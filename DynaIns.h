typedef WORD (*ins_ptr)(BYTE *dst);

ins_ptr SpecialInstruction[64] =
{
    dynaCompile_sll,
    dynaCompile_reserved,
    dynaCompile_srl,
    dynaCompile_sra,
    dynaCompile_sllv,
    dynaCompile_reserved,
    dynaCompile_srlv,
    dynaCompile_srav,
    dynaCompile_jr,
    dynaCompile_jalr,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_syscall,
    dynaCompile_Break,
    dynaCompile_reserved,
    dynaCompile_sync,
    dynaCompile_mfhi,
    dynaCompile_mthi,
    dynaCompile_mflo,
    dynaCompile_mtlo,
    dynaCompile_dsllv,
    dynaCompile_reserved,
    dynaCompile_dsrlv,
    dynaCompile_dsrav,
    dynaCompile_mult,
    dynaCompile_multu,
    dynaCompile_div,
    dynaCompile_divu,
    dynaCompile_dmult,
    dynaCompile_dmultu,
    dynaCompile_ddiv,
    dynaCompile_ddivu,
    dynaCompile_add,
    dynaCompile_addu,
    dynaCompile_sub,
    dynaCompile_subu,
    dynaCompile_and,
    dynaCompile_or,
    dynaCompile_xor,
    dynaCompile_nor,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_slt,
    dynaCompile_sltu,
    dynaCompile_dadd,
    dynaCompile_daddu,
    dynaCompile_dsub,
    dynaCompile_dsubu,
    dynaCompile_tge,
    dynaCompile_tgeu,
    dynaCompile_tlt,
    dynaCompile_tltu,
    dynaCompile_teq,
    dynaCompile_reserved,
    dynaCompile_tne,
    dynaCompile_reserved,
    dynaCompile_dsll,
    dynaCompile_reserved,
    dynaCompile_dsrl,
    dynaCompile_dsra,
    dynaCompile_dsll32,
    dynaCompile_reserved,
    dynaCompile_dsrl32,
    dynaCompile_dsra32
};


ins_ptr MainInstruction[64] =
{
	dynaCompile_special,
    dynaCompile_regimm,
    dynaCompile_j,
    dynaCompile_jal,
    dynaCompile_beq,
    dynaCompile_bne,
    dynaCompile_blez,
    dynaCompile_bgtz,
    dynaCompile_addi,
    dynaCompile_addiu,
    dynaCompile_slti,
    dynaCompile_sltiu,
    dynaCompile_andi,
    dynaCompile_ori,
    dynaCompile_xori,
    dynaCompile_lui,
    dynaCompile_cop0,
    dynaCompile_cop1,
    dynaCompile_cop2,
    dynaCompile_lui,		//old ldi
    dynaCompile_beql,
    dynaCompile_bnel,
    dynaCompile_blezl,
    dynaCompile_bgtzl,
    dynaCompile_daddi,
    dynaCompile_daddiu,
    dynaCompile_ldl,
    dynaCompile_ldr,
    dynaCompile_bgei,
    dynaCompile_bgeiu,
    dynaCompile_bli,
    dynaCompile_bliu,
    dynaCompile_lb,
    dynaCompile_lh,
    dynaCompile_lwl,
    dynaCompile_lw,
    dynaCompile_lbu,
    dynaCompile_lhu,
    dynaCompile_lwr,
    dynaCompile_lwu,
    dynaCompile_sb,
    dynaCompile_sh,
    dynaCompile_swl,
    dynaCompile_sw,
    dynaCompile_sdl,
    dynaCompile_sdr,
    dynaCompile_swr,
    dynaCompile_cache,
    dynaCompile_ll,
    dynaCompile_lwc1,
    dynaCompile_lwc2,
    dynaCompile_reserved,
    dynaCompile_lld,
    dynaCompile_ldc1,
    dynaCompile_ldc2,
    dynaCompile_ld,
    dynaCompile_sc,
    dynaCompile_swc1,
    dynaCompile_swc2,
    dynaCompile_reserved,
    dynaCompile_scd,
    dynaCompile_sdc1,
    dynaCompile_sdc2,
    dynaCompile_sd
};




ins_ptr RegimmInstruction[32] =
{
    dynaCompile_bltz,
    dynaCompile_bgez,
    dynaCompile_bltzl,
    dynaCompile_bgezl,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_tgei,
    dynaCompile_tgeiu,
    dynaCompile_tlti,
    dynaCompile_tltiu,
    dynaCompile_teqi,
    dynaCompile_reserved,
    dynaCompile_tnei,
    dynaCompile_reserved,
    dynaCompile_bltzal,
    dynaCompile_bgezal,
    dynaCompile_bltzall,
    dynaCompile_bgezall,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved
};





ins_ptr Cop0RSInstruction[32] =
{
    dynaCompile_cop0_rs_mf,
    dynaCompile_cop0_rs_dmf,
    dynaCompile_cop0_rs_cf,
    dynaCompile_reserved,
    dynaCompile_cop0_rs_mt,
    dynaCompile_cop0_rs_dmt,
    dynaCompile_cop0_rs_ct,
    dynaCompile_reserved,
    dynaCompile_cop0_rs_bc,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_cop0_rs_co,
    dynaCompile_cop0_rs_co,
    dynaCompile_cop0_rs_co,
    dynaCompile_cop0_rs_co,
    dynaCompile_cop0_rs_co,
    dynaCompile_cop0_rs_co,
    dynaCompile_cop0_rs_co,
    dynaCompile_cop0_rs_co,
    dynaCompile_cop0_rs_co,
    dynaCompile_cop0_rs_co,
    dynaCompile_cop0_rs_co,
    dynaCompile_cop0_rs_co,
    dynaCompile_cop0_rs_co,
    dynaCompile_cop0_rs_co,
    dynaCompile_cop0_rs_co,
    dynaCompile_cop0_rs_co
};





ins_ptr Cop0RTInstruction[32] =
{
    dynaCompile_cop0_rt_bcf,
    dynaCompile_cop0_rt_bct,
    dynaCompile_cop0_rt_bcfl,
    dynaCompile_cop0_rt_bctl,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved
};


ins_ptr Cop0Instruction[64]=
{
	dynaCompile_invalid,
	dynaCompile_cop0_tlbr,
    dynaCompile_cop0_tlbwi,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_cop0_tlbwr,
    dynaCompile_invalid,
    dynaCompile_cop0_tlbp,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_reserved,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_cop0_eret,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid
};



ins_ptr Cop1RSInstruction[32] =
{
    dynaCompile_cop1_rs_mf,
    dynaCompile_cop1_rs_dmf,
    dynaCompile_cop1_rs_cf,
    dynaCompile_reserved,
    dynaCompile_cop1_rs_mt,
    dynaCompile_cop1_rs_dmt,
    dynaCompile_cop1_rs_ct,
    dynaCompile_reserved,
    dynaCompile_cop1_rs_bc,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_cop1_rs_co,
    dynaCompile_cop1_rs_co,
    dynaCompile_cop1_rs_co,
    dynaCompile_cop1_rs_co,
    dynaCompile_cop1_rs_co,
    dynaCompile_cop1_rs_co,
    dynaCompile_cop1_rs_co,
    dynaCompile_cop1_rs_co,
    dynaCompile_cop1_rs_co,
    dynaCompile_cop1_rs_co,
    dynaCompile_cop1_rs_co,
    dynaCompile_cop1_rs_co,
    dynaCompile_cop1_rs_co,
    dynaCompile_cop1_rs_co,
    dynaCompile_cop1_rs_co,
    dynaCompile_cop1_rs_co
};

ins_ptr Cop1RTInstruction[32] =
{
    dynaCompile_cop1_rt_bcf,
    dynaCompile_cop1_rt_bct,
    dynaCompile_cop1_rt_bcfl,
    dynaCompile_cop1_rt_bctl,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved,
    dynaCompile_reserved
};

ins_ptr Cop1Instruction[64] =
{
    dynaCompile_cop1_add,
    dynaCompile_cop1_sub,
    dynaCompile_cop1_mul,
    dynaCompile_cop1_div,
    dynaCompile_cop1_sqrt,
    dynaCompile_cop1_abs,
    dynaCompile_cop1_mov,
    dynaCompile_cop1_neg,
    dynaCompile_cop1_roundl,
    dynaCompile_cop1_truncl,
    dynaCompile_cop1_ceill,
    dynaCompile_cop1_floorl,
    dynaCompile_cop1_roundw,
    dynaCompile_cop1_truncw,
    dynaCompile_cop1_ceilw,
    dynaCompile_cop1_floorw,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_cop1_cvts,
    dynaCompile_cop1_cvtd,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_cop1_cvtw,
    dynaCompile_cop1_cvtl,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_invalid,
    dynaCompile_cop1_c,
    dynaCompile_cop1_c,
    dynaCompile_cop1_c,
    dynaCompile_cop1_c,
    dynaCompile_cop1_c,
    dynaCompile_cop1_c,
    dynaCompile_cop1_c,
    dynaCompile_cop1_c,
    dynaCompile_cop1_c,
    dynaCompile_cop1_c,
    dynaCompile_cop1_c,
    dynaCompile_cop1_c,
    dynaCompile_cop1_c,
    dynaCompile_cop1_c,
    dynaCompile_cop1_c,
    dynaCompile_cop1_c
};


ins_ptr Cop2RSInstruction[32] =
{
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented,
    dynaCompile_cop2_rs_not_implemented
};

