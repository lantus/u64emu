typedef WORD (*ins_ptr)(BYTE *dst);

ins_ptr Special[64] =
{
    dasm_sll,
    dasm_reserved,
    dasm_srl,
    dasm_sra,
    dasm_sllv,
    dasm_reserved,
    dasm_srlv,
    dasm_srav,
    dasm_jr,
    dasm_jalr,
    dasm_reserved,
    dasm_reserved,
    dasm_syscall,
    dasm_Break,
    dasm_reserved,
    dasm_sync,
    dasm_mfhi,
    dasm_mthi,
    dasm_mflo,
    dasm_mtlo,
    dasm_dsllv,
    dasm_reserved,
    dasm_dsrlv,
    dasm_dsrav,
    dasm_mult,
    dasm_multu,
    dasm_div,
    dasm_divu,
    dasm_dmult,
    dasm_dmultu,
    dasm_ddiv,
    dasm_ddivu,
    dasm_add,
    dasm_addu,
    dasm_sub,
    dasm_subu,
    dasm_and,
    dasm_or,
    dasm_xor,
    dasm_nor,
    dasm_reserved,
    dasm_reserved,
    dasm_slt,
    dasm_sltu,
    dasm_dadd,
    dasm_daddu,
    dasm_dsub,
    dasm_dsubu,
    dasm_tge,
    dasm_tgeu,
    dasm_tlt,
    dasm_tltu,
    dasm_teq,
    dasm_reserved,
    dasm_tne,
    dasm_reserved,
    dasm_dsll,
    dasm_reserved,
    dasm_dsrl,
    dasm_dsra,
    dasm_dsll32,
    dasm_reserved,
    dasm_dsrl32,
    dasm_dsra32
};


ins_ptr Main[64] =
{
	dasm_special,
    dasm_regimm,
    dasm_j,
    dasm_jal,
    dasm_beq,
    dasm_bne,
    dasm_blez,
    dasm_bgtz,
    dasm_addi,
    dasm_addiu,
    dasm_slti,
    dasm_sltiu,
    dasm_andi,
    dasm_ori,
    dasm_xori,
    dasm_lui,
    dasm_cop0,
    dasm_cop1,
    dasm_cop2,
    dasm_reserved,
    dasm_beql,
    dasm_bnel,
    dasm_blezl,
    dasm_bgtzl,
    dasm_daddi,
    dasm_daddiu,
    dasm_ldl,
    dasm_ldr,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_lb,
    dasm_lh,
    dasm_lwl,
    dasm_lw,
    dasm_lbu,
    dasm_lhu,
    dasm_lwr,
    dasm_lwu,
    dasm_sb,
    dasm_sh,
    dasm_swl,
    dasm_sw,
    dasm_sdl,
    dasm_sdr,
    dasm_swr,
    dasm_cache,
    dasm_ll,
    dasm_lwc1,
    dasm_lwc2,
    dasm_reserved,
    dasm_lld,
    dasm_ldc1,
    dasm_ldc2,
    dasm_ld,
    dasm_sc,
    dasm_swc1,
    dasm_swc2,
    dasm_reserved,
    dasm_scd,
    dasm_sdc1,
    dasm_sdc2,
    dasm_sd
};




ins_ptr Regimm[32] =
{
    dasm_bltz,
    dasm_bgez,
    dasm_bltzl,
    dasm_bgezl,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_tgei,
    dasm_tgeiu,
    dasm_tlti,
    dasm_tltiu,
    dasm_teqi,
    dasm_reserved,
    dasm_tnei,
    dasm_reserved,
    dasm_bltzal,
    dasm_bgezal,
    dasm_bltzall,
    dasm_bgezall,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved
};





ins_ptr Cop0RS[32] =
{
    dasm_cop0_rs_mf,
    dasm_cop0_rs_dmf,
    dasm_cop0_rs_cf,
    dasm_reserved,
    dasm_cop0_rs_mt,
    dasm_cop0_rs_dmt,
    dasm_cop0_rs_ct,
    dasm_reserved,
    dasm_cop0_rs_bc,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_cop0_rs_co,
    dasm_cop0_rs_co,
    dasm_cop0_rs_co,
    dasm_cop0_rs_co,
    dasm_cop0_rs_co,
    dasm_cop0_rs_co,
    dasm_cop0_rs_co,
    dasm_cop0_rs_co,
    dasm_cop0_rs_co,
    dasm_cop0_rs_co,
    dasm_cop0_rs_co,
    dasm_cop0_rs_co,
    dasm_cop0_rs_co,
    dasm_cop0_rs_co,
    dasm_cop0_rs_co,
    dasm_cop0_rs_co
};





ins_ptr Cop0RT[32] =
{
    dasm_cop0_rt_bcf,
    dasm_cop0_rt_bct,
    dasm_cop0_rt_bcfl,
    dasm_cop0_rt_bctl,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved
};


ins_ptr Cop0[64]=
{
	dasm_invalid,
	dasm_cop0_tlbr,
    dasm_cop0_tlbwi,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_cop0_tlbwr,
    dasm_invalid,
    dasm_cop0_tlbp,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_reserved,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_cop0_eret,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid
};



ins_ptr Cop1RS[32] =
{
    dasm_cop1_rs_mf,
    dasm_cop1_rs_dmf,
    dasm_cop1_rs_cf,
    dasm_reserved,
    dasm_cop1_rs_mt,
    dasm_cop1_rs_dmt,
    dasm_cop1_rs_ct,
    dasm_reserved,
    dasm_cop1_rs_bc,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_cop1_rs_co,
    dasm_cop1_rs_co,
    dasm_cop1_rs_co,
    dasm_cop1_rs_co,
    dasm_cop1_rs_co,
    dasm_cop1_rs_co,
    dasm_cop1_rs_co,
    dasm_cop1_rs_co,
    dasm_cop1_rs_co,
    dasm_cop1_rs_co,
    dasm_cop1_rs_co,
    dasm_cop1_rs_co,
    dasm_cop1_rs_co,
    dasm_cop1_rs_co,
    dasm_cop1_rs_co,
    dasm_cop1_rs_co
};

ins_ptr Cop1RT[32] =
{
    dasm_cop1_rt_bcf,
    dasm_cop1_rt_bct,
    dasm_cop1_rt_bcfl,
    dasm_cop1_rt_bctl,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved,
    dasm_reserved
};

ins_ptr Cop1[64] =
{
    dasm_cop1_add,
    dasm_cop1_sub,
    dasm_cop1_mul,
    dasm_cop1_div,
    dasm_cop1_sqrt,
    dasm_cop1_abs,
    dasm_cop1_mov,
    dasm_cop1_neg,
    dasm_cop1_roundl,
    dasm_cop1_truncl,
    dasm_cop1_ceill,
    dasm_cop1_floorl,
    dasm_cop1_roundw,
    dasm_cop1_truncw,
    dasm_cop1_ceilw,
    dasm_cop1_floorw,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_cop1_cvts,
    dasm_cop1_cvtd,
    dasm_invalid,
    dasm_invalid,
    dasm_cop1_cvtw,
    dasm_cop1_cvtl,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_invalid,
    dasm_cop1_c,
    dasm_cop1_c,
    dasm_cop1_c,
    dasm_cop1_c,
    dasm_cop1_c,
    dasm_cop1_c,
    dasm_cop1_c,
    dasm_cop1_c,
    dasm_cop1_c,
    dasm_cop1_c,
    dasm_cop1_c,
    dasm_cop1_c,
    dasm_cop1_c,
    dasm_cop1_c,
    dasm_cop1_c,
    dasm_cop1_c
};


ins_ptr Cop2RS[32] =
{
    dasm_cop2_mf,
    dasm_cop2_reserved,
    dasm_cop2_cf,
    dasm_cop2_reserved,
    dasm_cop2_mt,
    dasm_cop2_reserved,
    dasm_cop2_ct,
    dasm_cop2_reserved,
    dasm_cop2_reserved,
    dasm_cop2_reserved,
    dasm_cop2_reserved,
    dasm_cop2_reserved,
    dasm_cop2_reserved,
    dasm_cop2_reserved,
    dasm_cop2_reserved,
    dasm_cop2_reserved,
    dasm_cop2_vectop,
    dasm_cop2_vectop,
    dasm_cop2_vectop,
    dasm_cop2_vectop,
    dasm_cop2_vectop,
    dasm_cop2_vectop,
    dasm_cop2_vectop,
    dasm_cop2_vectop,
    dasm_cop2_vectop,
    dasm_cop2_vectop,
    dasm_cop2_vectop,
    dasm_cop2_vectop,
    dasm_cop2_vectop,
    dasm_cop2_vectop,
    dasm_cop2_vectop,
    dasm_cop2_vectop
};



ins_ptr Cop2Vector[64] =
{
    dasm_cop2_vmulf,
    dasm_cop2_vmulu,
    dasm_cop2_vrndp,
    dasm_cop2_vmulq,
    dasm_cop2_vmudl,
    dasm_cop2_vmudm,
    dasm_cop2_vmudn,
    dasm_cop2_vmudh,
    dasm_cop2_vmacf,
    dasm_cop2_vmacu,
    dasm_cop2_vrndn,
    dasm_cop2_vmacq,
    dasm_cop2_vmadl,
    dasm_cop2_vmadm,
    dasm_cop2_vmadn,
    dasm_cop2_vmadh,
    dasm_cop2_vadd,
    dasm_cop2_vsub,
    dasm_cop2_vsut,
    dasm_cop2_vabs,
    dasm_cop2_vaddc,
    dasm_cop2_vsubc,
    dasm_cop2_vaddb,
    dasm_cop2_vsubb,
    dasm_cop2_vaccb,
    dasm_cop2_vsucb,
    dasm_cop2_vsad,
    dasm_cop2_vsac,
    dasm_cop2_vsum,
    dasm_cop2_vsaw,
    dasm_cop2_reserved,
    dasm_cop2_reserved,
    dasm_cop2_vlt,
    dasm_cop2_veq,
    dasm_cop2_vne,
    dasm_cop2_vge,
    dasm_cop2_vcl,
    dasm_cop2_vch,
    dasm_cop2_vcr,
    dasm_cop2_vmrg,
    dasm_cop2_vand,
    dasm_cop2_vnand,
    dasm_cop2_vor,
    dasm_cop2_vnor,
    dasm_cop2_vxor,
    dasm_cop2_vnxor,
    dasm_cop2_reserved,
    dasm_cop2_reserved,
    dasm_cop2_vrcp,
    dasm_cop2_vrcpl,
    dasm_cop2_vrcph,
    dasm_cop2_vmov,
    dasm_cop2_vrsq,
    dasm_cop2_vrsql,
    dasm_cop2_vrsqh,
    dasm_cop2_vnoop,
    dasm_cop2_vextt,
    dasm_cop2_vextq,
    dasm_cop2_vextn,
    dasm_cop2_reserved,
    dasm_cop2_vinst,
    dasm_cop2_vinsq,
    dasm_cop2_vinsn,
    dasm_cop2_reserved
};
