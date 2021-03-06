
extern char dasmRegName[32][3];
extern char dasmCRegName[32][9];

extern char **dasmLookupName;
extern DWORD *dasmLookupAddy;
extern DWORD dasmNumLookups;

extern int dynaNumBPoints;
extern DWORD dynaBPoint[1024];
extern bool dynaBreakOnKnown;
extern bool dasmRSPMode;

extern DWORD dasmOpCode;
extern DWORD dasmNextOpCode;
extern DWORD dasmPC;
extern BYTE dasmIsValid[32];
extern DWORD *dasmRam;
extern __int64 dasmReg[32];
extern __int64 dasmRegHi;
extern __int64 dasmRegLo;
extern __int64 dasmCPR0[32];
extern __int64 dasmCPR1[32];
extern __int64 dasmCPC0[32];
extern __int64 dasmCPC1[32];
char *dasmDo(DWORD *where,DWORD Address,DWORD EndAddress);
void dasmReset();
void dasmClose();

extern WORD dasm_sll(BYTE *dst);
extern WORD dasm_srl(BYTE *dst);
extern WORD dasm_sra(BYTE *dst);
extern WORD dasm_sllv(BYTE *dst);
extern WORD dasm_srlv(BYTE *dst);
extern WORD dasm_srav(BYTE *dst);
extern WORD dasm_jr(BYTE *dst);
extern WORD dasm_jalr(BYTE *dst);
extern WORD dasm_syscall(BYTE *dst);
extern WORD dasm_Break(BYTE *dst);
extern WORD dasm_reserved(BYTE *dst);
extern WORD dasm_sync(BYTE *dst);
extern WORD dasm_mfhi(BYTE *dst);
extern WORD dasm_mthi(BYTE *dst);
extern WORD dasm_mflo(BYTE *dst);
extern WORD dasm_mtlo(BYTE *dst);
extern WORD dasm_dsllv(BYTE *dst);
extern WORD dasm_dsrlv(BYTE *dst);
extern WORD dasm_dsrav(BYTE *dst);
extern WORD dasm_mult(BYTE *dst);
extern WORD dasm_multu(BYTE *dst);
extern WORD dasm_div(BYTE *dst);
extern WORD dasm_divu(BYTE *dst);
extern WORD dasm_dmult(BYTE *dst);
extern WORD dasm_dmultu(BYTE *dst);
extern WORD dasm_ddiv(BYTE *dst);
extern WORD dasm_ddivu(BYTE *dst);
extern WORD dasm_add(BYTE *dst);
extern WORD dasm_addu(BYTE *dst);
extern WORD dasm_sub(BYTE *dst);
extern WORD dasm_subu(BYTE *dst);
extern WORD dasm_and(BYTE *dst);
extern WORD dasm_or(BYTE *dst);
extern WORD dasm_xor(BYTE *dst);
extern WORD dasm_nor(BYTE *dst);
extern WORD dasm_slt(BYTE *dst);
extern WORD dasm_sltu(BYTE *dst);
extern WORD dasm_dadd(BYTE *dst);
extern WORD dasm_daddu(BYTE *dst);
extern WORD dasm_dsub(BYTE *dst);
extern WORD dasm_dsubu(BYTE *dst);
extern WORD dasm_tge(BYTE *dst);
extern WORD dasm_tgeu(BYTE *dst);
extern WORD dasm_tlt(BYTE *dst);
extern WORD dasm_tltu(BYTE *dst);
extern WORD dasm_teq(BYTE *dst);
extern WORD dasm_tne(BYTE *dst);
extern WORD dasm_dsll(BYTE *dst);
extern WORD dasm_dsrl(BYTE *dst);
extern WORD dasm_dsra(BYTE *dst);
extern WORD dasm_dsll32(BYTE *dst);
extern WORD dasm_dsrl32(BYTE *dst);
extern WORD dasm_dsra3(BYTE *dst);
extern WORD dasm_special(BYTE *dst);
extern WORD dasm_regimm(BYTE *dst);
extern WORD dasm_j(BYTE *dst);
extern WORD dasm_jal(BYTE *dst);
extern WORD dasm_beq(BYTE *dst);
extern WORD dasm_bne(BYTE *dst);
extern WORD dasm_blez(BYTE *dst);
extern WORD dasm_bgtz(BYTE *dst);
extern WORD dasm_addi(BYTE *dst);
extern WORD dasm_addiu(BYTE *dst);
extern WORD dasm_slti(BYTE *dst);
extern WORD dasm_sltiu(BYTE *dst);
extern WORD dasm_andi(BYTE *dst);
extern WORD dasm_ori(BYTE *dst);
extern WORD dasm_xori(BYTE *dst);
extern WORD dasm_lui(BYTE *dst);
extern WORD dasm_cop0(BYTE *dst);
extern WORD dasm_cop1(BYTE *dst);
extern WORD dasm_cop2(BYTE *dst);
extern WORD dasm_beql(BYTE *dst);
extern WORD dasm_bnel(BYTE *dst);
extern WORD dasm_blezl(BYTE *dst);
extern WORD dasm_bgtzl(BYTE *dst);
extern WORD dasm_daddi(BYTE *dst);
extern WORD dasm_daddiu(BYTE *dst);
extern WORD dasm_ldl(BYTE *dst);
extern WORD dasm_ldr(BYTE *dst);
extern WORD dasm_lb(BYTE *dst);
extern WORD dasm_lh(BYTE *dst);
extern WORD dasm_lwl(BYTE *dst);
extern WORD dasm_lw(BYTE *dst);
extern WORD dasm_lbu(BYTE *dst);
extern WORD dasm_lhu(BYTE *dst);
extern WORD dasm_lwr(BYTE *dst);
extern WORD dasm_lwu(BYTE *dst);
extern WORD dasm_sb(BYTE *dst);
extern WORD dasm_sh(BYTE *dst);
extern WORD dasm_swl(BYTE *dst);
extern WORD dasm_sw(BYTE *dst);
extern WORD dasm_sdl(BYTE *dst);
extern WORD dasm_sdr(BYTE *dst);
extern WORD dasm_swr(BYTE *dst);
extern WORD dasm_cache(BYTE *dst);
extern WORD dasm_ll(BYTE *dst);
extern WORD dasm_lwc1(BYTE *dst);
extern WORD dasm_lwc2(BYTE *dst);
extern WORD dasm_lld(BYTE *dst);
extern WORD dasm_ldc1(BYTE *dst);
extern WORD dasm_ldc2(BYTE *dst);
extern WORD dasm_ld(BYTE *dst);
extern WORD dasm_sc(BYTE *dst);
extern WORD dasm_swc1(BYTE *dst);
extern WORD dasm_swc2(BYTE *dst);
extern WORD dasm_scd(BYTE *dst);
extern WORD dasm_sdc1(BYTE *dst);
extern WORD dasm_sdc2(BYTE *dst);
extern WORD dasm_s(BYTE *dst);
extern WORD dasm_bltz(BYTE *dst);
extern WORD dasm_bgez(BYTE *dst);
extern WORD dasm_bltzl(BYTE *dst);
extern WORD dasm_bgezl(BYTE *dst);
extern WORD dasm_tgei(BYTE *dst);
extern WORD dasm_tgeiu(BYTE *dst);
extern WORD dasm_tlti(BYTE *dst);
extern WORD dasm_tltiu(BYTE *dst);
extern WORD dasm_teqi(BYTE *dst);
extern WORD dasm_tnei(BYTE *dst);
extern WORD dasm_bltzal(BYTE *dst);
extern WORD dasm_bgezal(BYTE *dst);
extern WORD dasm_bltzall(BYTE *dst);
extern WORD dasm_bgezall(BYTE *dst);
extern WORD dasm_cop0_rs_mf(BYTE *dst);
extern WORD dasm_cop0_rs_dmf(BYTE *dst);
extern WORD dasm_cop0_rs_cf(BYTE *dst);
extern WORD dasm_cop0_rs_mt(BYTE *dst);
extern WORD dasm_cop0_rs_dmt(BYTE *dst);
extern WORD dasm_cop0_rs_ct(BYTE *dst);
extern WORD dasm_cop0_rs_bc(BYTE *dst);
extern WORD dasm_cop0_rs_co(BYTE *dst);
extern WORD dasm_cop0_rt_bcf(BYTE *dst);
extern WORD dasm_cop0_rt_bct(BYTE *dst);
extern WORD dasm_cop0_rt_bcfl(BYTE *dst);
extern WORD dasm_cop0_rt_bctl(BYTE *dst);
extern WORD dasm_invalid(BYTE *dst);
extern WORD dasm_cop0_tlbr(BYTE *dst);
extern WORD dasm_cop0_tlbwi(BYTE *dst);
extern WORD dasm_cop0_tlbwr(BYTE *dst);
extern WORD dasm_cop0_tlbp(BYTE *dst);
extern WORD dasm_cop0_eret(BYTE *dst);
extern WORD dasm_cop1_rs_mf(BYTE *dst);
extern WORD dasm_cop1_rs_dmf(BYTE *dst);
extern WORD dasm_cop1_rs_cf(BYTE *dst);
extern WORD dasm_cop1_rs_mt(BYTE *dst);
extern WORD dasm_cop1_rs_dmt(BYTE *dst);
extern WORD dasm_cop1_rs_ct(BYTE *dst);
extern WORD dasm_cop1_rs_bc(BYTE *dst);
extern WORD dasm_cop1_rs_co(BYTE *dst);
extern WORD dasm_cop1_rt_bcf(BYTE *dst);
extern WORD dasm_cop1_rt_bct(BYTE *dst);
extern WORD dasm_cop1_rt_bcfl(BYTE *dst);
extern WORD dasm_cop1_rt_bctl(BYTE *dst);
extern WORD dasm_cop1_add(BYTE *dst);
extern WORD dasm_cop1_sub(BYTE *dst);
extern WORD dasm_cop1_mul(BYTE *dst);
extern WORD dasm_cop1_div(BYTE *dst);
extern WORD dasm_cop1_sqrt(BYTE *dst);
extern WORD dasm_cop1_abs(BYTE *dst);
extern WORD dasm_cop1_mov(BYTE *dst);
extern WORD dasm_cop1_neg(BYTE *dst);
extern WORD dasm_cop1_roundl(BYTE *dst);
extern WORD dasm_cop1_truncl(BYTE *dst);
extern WORD dasm_cop1_ceill(BYTE *dst);
extern WORD dasm_cop1_floorl(BYTE *dst);
extern WORD dasm_cop1_roundw(BYTE *dst);
extern WORD dasm_cop1_truncw(BYTE *dst);
extern WORD dasm_cop1_ceilw(BYTE *dst);
extern WORD dasm_cop1_floorw(BYTE *dst);
extern WORD dasm_cop1_cvts(BYTE *dst);
extern WORD dasm_cop1_cvtd(BYTE *dst);
extern WORD dasm_cop1_cvtw(BYTE *dst);
extern WORD dasm_cop1_cvtl(BYTE *dst);
extern WORD dasm_cop1_c(BYTE *dst);
extern WORD dasm_cop2_rs_not_implemented(BYTE *dst);
extern WORD dasm_dsra32(BYTE *dst);
extern WORD dasm_sd(BYTE *dst);

extern WORD dasm_cop2_reserved(BYTE *dst);
extern WORD dasm_cop2_mf(BYTE *dst);
extern WORD dasm_cop2_cf(BYTE *dst);
extern WORD dasm_cop2_mt(BYTE *dst);
extern WORD dasm_cop2_ct(BYTE *dst);
extern WORD dasm_cop2_vectop(BYTE *dst);
extern WORD dasm_cop2_vmulf(BYTE *dst);
extern WORD dasm_cop2_vmulu(BYTE *dst);
extern WORD dasm_cop2_vrndp(BYTE *dst);
extern WORD dasm_cop2_vmulq(BYTE *dst);
extern WORD dasm_cop2_vmudl(BYTE *dst);
extern WORD dasm_cop2_vmudm(BYTE *dst);
extern WORD dasm_cop2_vmudn(BYTE *dst);
extern WORD dasm_cop2_vmudh(BYTE *dst);
extern WORD dasm_cop2_vmacf(BYTE *dst);
extern WORD dasm_cop2_vmacu(BYTE *dst);
extern WORD dasm_cop2_vrndn(BYTE *dst);
extern WORD dasm_cop2_vmacq(BYTE *dst);
extern WORD dasm_cop2_vmadl(BYTE *dst);
extern WORD dasm_cop2_vmadm(BYTE *dst);
extern WORD dasm_cop2_vmadn(BYTE *dst);
extern WORD dasm_cop2_vmadh(BYTE *dst);
extern WORD dasm_cop2_vadd(BYTE *dst);
extern WORD dasm_cop2_vsub(BYTE *dst);
extern WORD dasm_cop2_vsut(BYTE *dst);
extern WORD dasm_cop2_vabs(BYTE *dst);
extern WORD dasm_cop2_vaddc(BYTE *dst);
extern WORD dasm_cop2_vsubc(BYTE *dst);
extern WORD dasm_cop2_vaddb(BYTE *dst);
extern WORD dasm_cop2_vsubb(BYTE *dst);
extern WORD dasm_cop2_vaccb(BYTE *dst);
extern WORD dasm_cop2_vsucb(BYTE *dst);
extern WORD dasm_cop2_vsad(BYTE *dst);
extern WORD dasm_cop2_vsac(BYTE *dst);
extern WORD dasm_cop2_vsum(BYTE *dst);
extern WORD dasm_cop2_vsaw(BYTE *dst);
extern WORD dasm_cop2_reserved(BYTE *dst);
extern WORD dasm_cop2_vlt(BYTE *dst);
extern WORD dasm_cop2_veq(BYTE *dst);
extern WORD dasm_cop2_vne(BYTE *dst);
extern WORD dasm_cop2_vge(BYTE *dst);
extern WORD dasm_cop2_vcl(BYTE *dst);
extern WORD dasm_cop2_vch(BYTE *dst);
extern WORD dasm_cop2_vcr(BYTE *dst);
extern WORD dasm_cop2_vmrg(BYTE *dst);
extern WORD dasm_cop2_vand(BYTE *dst);
extern WORD dasm_cop2_vnand(BYTE *dst);
extern WORD dasm_cop2_vor(BYTE *dst);
extern WORD dasm_cop2_vnor(BYTE *dst);
extern WORD dasm_cop2_vxor(BYTE *dst);
extern WORD dasm_cop2_vnxor(BYTE *dst);
extern WORD dasm_cop2_reserved(BYTE *dst);
extern WORD dasm_cop2_vrcp(BYTE *dst);
extern WORD dasm_cop2_vrcpl(BYTE *dst);
extern WORD dasm_cop2_vrcph(BYTE *dst);
extern WORD dasm_cop2_vmov(BYTE *dst);
extern WORD dasm_cop2_vrsq(BYTE *dst);
extern WORD dasm_cop2_vrsql(BYTE *dst);
extern WORD dasm_cop2_vrsqh(BYTE *dst);
extern WORD dasm_cop2_vnoop(BYTE *dst);
extern WORD dasm_cop2_vextt(BYTE *dst);
extern WORD dasm_cop2_vextq(BYTE *dst);
extern WORD dasm_cop2_vextn(BYTE *dst);
extern WORD dasm_cop2_vinst(BYTE *dst);
extern WORD dasm_cop2_vinsq(BYTE *dst);
extern WORD dasm_cop2_vinsn(BYTE *dst);
extern void dasmChangeEndian(WORD Mode,BYTE *src,DWORD Length);
