380 jmp        3C00388                                           
384 NOP
388 jal        3C004BC                                           
38C NOP
390 jal        3C003CC                                           
394 NOP
398 jal        3C003E8                                           
39C NOP
3A0 jal        3C00430                                           
3A4 NOP
3A8 jal        3C0046C                                           
3AC NOP
3B0 jal        3C00480                                           
3B4 NOP
3B8 jal        3C0049C                                           
3BC NOP
3C0 lui        At,$FFFFB000                      At=B0000000     
3C4 jmp        3C006A8                                           
3C8 lw         R0,$A0(At)                        *(B00000A0)     

3CC lui        GP,$FFFFB000                      GP=B0000000     
3D0 movi       V0,$1                             V0=0:1          
3D4 lui        V1,$FFFFB000                      V1=B0000000     
3D8 sw         V0,$0(GP)                         *(B0000000)     
3DC ori        V1,V1,$80                         V1=0:B0000080   
3E0 jr         RA                                                
3E4 sw         R0,$0(V1)                         *(B0000080)     

3E8 lui        V0,$FFFF8000                      V0=80000000     
3EC movi       V1,$FF                            V1=0:FF         
3F0 mt0        GP,R0            INDEX            GP=FFFFFFFF:B000
3F4 Cache                                                        
3F8 Cache                                                        
3FC lw         R0,$0(V0)                         *(80000000)     
400 lw         R0,$2000(V0)                      *(80002000)     
404 NOP
408 NOP
40C NOP
410 NOP
414 Cache                                                        
418 Cache                                                        
41C addi       V0,V0,$20                         V0=FFFFFFFF:8000
420 bgtz       V1 3F4                                            
424 addi       V1,V1,$FFFFFFFF                   V1=0:FE         
428 jr         RA                                                
42C NOP

430 mt0        GP,R0            INDEX            GP=FFFFFFFF:B000
434 lui        V0,$FFFF8000                      V0=80000000     
438 movi       V1,$FF                            V1=0:FF         
43C Cache                                                        
440 Cache                                                        
444 Cache                                                        
448 Cache                                                        
44C NOP
450 Cache                                                        
454 Cache                                                        
458 addi       V0,V0,$20                         V0=FFFFFFFF:8000
45C bgtz       V1 43C                                            
460 addi       V1,V1,$FFFFFFFF                   V1=0:FE         
464 jr         RA                                                
468 NOP

46C lui        V1,$FFFFDFFF                      V1=DFFF0000     
470 ori        V1,V1,$FFFF                       V1=0:DFFFFFFF   
474 and        RA,RA,V1                          RA=0:3C0        
478 jr         RA                                                
47C NOP

480 lui        A0,$FFFF8003                      A0=80030000     
484 lui        A1,$FFFF8008                      A1=80080000     
488 addi       A0,A0,$8                          A0=FFFFFFFF:8003
48C bne        A1,A0 488                                         
490 sd         R0,$FFFFFFF8(A0)                  *(80030000)     
494 jr         RA                                                
498 NOP

49C lui        V0,$FFFF8000                      V0=80000000     
4A0 movi       V1,$2000                          V1=0:2000       
4A4 addi       V1,V1,$FFFFFFFF                   V1=0:1FFF       
4A8 lw         R0,$0(V0)                         *(80000000)     
4AC bne        R0,V1 4A4                                         
4B0 addi       V0,V0,$4                          V0=FFFFFFFF:8000
4B4 jr         RA                                                
4B8 NOP

4BC lui        V0,$3440                          V0=34400000     
4C0 mt0        T4,V0                             //status
4C4 mt0        A2,R0                                             
4C8 movi       V1,$2F                            V1=0:2F         
4CC movi       A0,$FF                            A0=0:FF         
4D0 mt0        V0,R0            INDEX            V0=0:34400000   
4D4 mt0        V1,R0            PRID             V1=0:2F         
4D8 mt0        A1,R0            INDEX            A1=FFFFFFFF:8008
4DC mt0        T2,A0                             //entryhi=0xff        
4E0 mt0        R0,V1            INDEX            //index=0x2f    
4E4 NOP
tlbwi
4EC NOP
4F0 addi       V1,V1,$FFFFFFFF                   V1=0:2E         
4F4 bgez       V1 4D0                                            
4F8 addi       A0,A0,$2000                       A0=0:20FF       
4FC movi       V0,$0                             V0=0:0          
500 mt0        R0,V0            INDEX            R0=0:0          
504 movi       V0,$0                             V0=0:0          
508 mt0        V0,V0            INDEX            V0=0:0          
50C movi       V0,$0                             V0=0:0          
510 mt0        V1,V0            EPC              V1=0:2E         
514 movi       V0,$0                             V0=0:0          
518 mt0        A0,V0            __(31)__         A0=0:20FF       
51C movi       V0,$0                             V0=0:0          
520 mt0        A1,V0            INDEX            A1=FFFFFFFF:8008
524 movi       V0,$0                             V0=0:0          
528 mt0        A2,V0                                             
52C movi       V0,$0                             V0=0:0          
530 mt0        T0,V0                                             
534 movi       V0,$0                             V0=0:0          
538 mt0        T1,V0                                             
53C movi       V0,$0                             V0=0:0          
540 mt0        T2,V0                                             
544 movi       V0,$FFFFFFFF                      V0=FFFFFFFF:FFFF
548 mt0        T3,V0                             //compare
54C movi       V0,$38                            V0=0:38         
550 mt0        T5,V0                             //cause       
554 movi       V0,$0                             V0=0:0          
558 mt0        T6,V0                                             
55C movi       V0,$3                             V0=0:3          
560 mt0        S0,V0                              //config         
564 movi       V0,$0                             V0=0:0          
568 mt0        S1,V0                                             
56C movi       V0,$0                             V0=0:0          
570 mt0        S4,V0                                             
574 movi       V0,$0                             V0=0:0          
578 mt0        K0,V0                                             
57C movi       V0,$0                             V0=0:0          
580 mt0        K1,V0                                             
584 movi       V0,$0                             V0=0:0          
588 mt0        GP,V0            INDEX            GP=FFFFFFFF:B000
58C movi       V0,$0                             V0=0:0          
590 mt0        SP,V0            INDEX            SP=0:0          
594 movi       V0,$0                             V0=0:0          
598 mt0        S8,V0                                             
59C lui        V0,$100                           V0=1000000      
5A0 ct1        RA,V0                                             
5A4 jr         RA                                                
5A8 NOP

5AC dmt1       R0,R0                             R0=0:0          
5B0 dmt1       At,R0                             At=FFFFFFFF:B000
5B4 dmt1       V0,R0                             V0=0:1000000    
5B8 dmt1       V1,R0                             V1=0:2E         
5BC dmt1       A0,R0                             A0=0:20FF       
5C0 dmt1       A1,R0                             A1=FFFFFFFF:8008
5C4 dmt1       A2,R0                                             
5C8 dmt1       A3,R0                                             
5CC dmt1       T0,R0                                             
5D0 dmt1       T1,R0                                             
5D4 dmt1       T2,R0                                             
5D8 dmt1       T3,R0                                             
5DC dmt1       T4,R0                                             
5E0 dmt1       T5,R0                                             
5E4 dmt1       T6,R0                                             
5E8 dmt1       T7,R0                                             
5EC dmt1       S0,R0                                             
5F0 dmt1       S1,R0                                             
5F4 dmt1       S2,R0                                             
5F8 dmt1       S3,R0                                             
5FC dmt1       S4,R0                                             
600 dmt1       S5,R0                                             
604 dmt1       S6,R0                                             
608 dmt1       S7,R0                                             
60C dmt1       T8,R0                                             
610 dmt1       T9,R0                                             
614 dmt1       K0,R0                                             
618 dmt1       K1,R0                                             
61C dmt1       GP,R0                             GP=FFFFFFFF:B000
620 dmt1       SP,R0                             SP=0:0          
624 dmt1       S8,R0                                             
628 dmt1       RA,R0                             RA=0:3C0        
62C mov        At,R0                             At=0:0          
630 mov        V0,R0                             V0=0:0          
634 mov        V1,R0                             V1=0:0          
638 mov        A0,R0                             A0=0:0          
63C mov        A1,R0                             A1=0:0          
640 mov        A2,R0                             A2=0:0          
644 mov        A3,R0                             A3=0:0          
648 mov        T0,R0                             T0=0:0          
64C mov        T1,R0                             T1=0:0          
650 mov        T2,R0                             T2=0:0          
654 mov        T3,R0                             T3=0:0          
658 mov        T4,R0                             T4=0:0          
65C mov        T5,R0                             T5=0:0          
660 mov        T6,R0                             T6=0:0          
664 mov        T7,R0                             T7=0:0          
668 mov        S0,R0                             S0=0:0          
66C mov        S1,R0                             S1=0:0          
670 mov        S2,R0                             S2=0:0          
674 mov        S3,R0                             S3=0:0          
678 mov        S4,R0                             S4=0:0          
67C mov        S5,R0                             S5=0:0          
680 mov        S6,R0                             S6=0:0          
684 mov        S7,R0                             S7=0:0          
688 mov        T8,R0                             T8=0:0          
68C mov        T9,R0                             T9=0:0          
690 mov        K0,R0                             K0=0:0          
694 mov        K1,R0                             K1=0:0          
698 mov        GP,R0                             GP=0:0          
69C mov        SP,R0                             SP=0:0          
6A0 jr         RA                                                
6A4 mov        S8,R0                             S8=0:0          

6A8 lui        A1,$FFFFBFC0                      A1=BFC00000     
6AC addi       A1,A1,$FCC                        A1=FFFFFFFF:BFC0
6B0 lui        A0,$FFFF887F                      A0=887F0000     
6B4 addi       A1,A1,$F                          A1=FFFFFFFF:BFC0
6B8 ori        A0,A0,$FF00                       A0=0:887FFF00   
6BC srl        A1,A1,$4                          A1=0:BFC00FD    
6C0 lui        T8,$FFFFA003                      T8=A0030000     
6C4 sll        A1,A1,$4                          A1=FFFFFFFF:BFC0
6C8 movi       S6,$0                             S6=0:0          

6CC jal        3C00D14	//memcpy, but seems to reorder...
6D0 addi       T3,A0,$10                         T3=FFFFFFFF:887F

6D4 jal        3C00D38  //s0,s1,s2
6D8 NOP

6DC jal        3C00D50                                           
6E0 NOP

6E4 movi       At,$7262                          At=0:7262       
6E8 beq        At,V0 6F4                                         
6EC NOP
SysCall 
6F4 jal        3C00D58                                           
6F8 NOP
6FC mov        S5,V0                             S5=0:0          
700 beq        R0,S5 708                                         
704 NOP
708 jal        3C00D58                                           
70C NOP
710 beq        R0,V0 71C                                         
714 NOP
SysCall 
71C jal        3C00D48                                           
720 NOP
724 jal        3C00D48                                           
728 mov        T2,V0                             T2=0:0          
72C dsll32     A3,V0,$20                                         
730 bne        R0,S6 73C                                         
734 dsra32     A3,A3,$20                                         
738 mov        S6,A3                             S6=0:0          
73C beq        R0,T2 C74                                         
740 NOP
744 jal        3C00CB0                                           
748 NOP
74C addi       A2,T3,$40                         A2=FFFFFFFF:887F
750 addi       T0,T3,$A0                         T0=FFFFFFFF:887F
754 sub        A0,R0,S2                          A0=0:0          
758 jal        3C00DA0                                           
75C andi       A0,A0,$7                          A0=0:0          
760 jal        3C00D50                                           
764 NOP
768 jal        3C00DDC                                           
76C mov        At,V0                             At=0:0          
770 sw         V0,$0(A2)                         *(887FFF50)     
774 jal        3C00DC8                                           
778 add        V0,V0,At                          V0=0:0          
77C addi       A2,A2,$4                          A2=FFFFFFFF:887F
780 bne        T0,A2 754                                         
784 NOP

//part2
788 movi       T0,$0                             T0=0:0          
78C movi       T4,$0                             T4=0:0          
790 movi       T5,$0                             T5=0:0          
794 movi       T6,$0                             T6=0:0          
798 movi       T7,$0                             T7=0:0          
79C movi       S3,$0                             S3=0:0          
7A0 jal        3C00DEC                                           
7A4 lw         V0,$40(T3)                        *(887FFF50)     
7A8 lbu        T1,$0(V0)                         *(0)            
7AC lui        S4,$FFFFBFC0                      S4=BFC00000     
7B0 addi       S4,S4,$F48                        S4=FFFFFFFF:BFC0
7B4 add        S4,S4,T1                          S4=FFFFFFFF:BFC0
7B8 add        S4,S4,T1                          S4=FFFFFFFF:BFC0
7BC lhu        S4,$0(S4)                         *(BFC00F48)     
7C0 beq        R0,T1 7F8                                         
7C4 movi       At,$1                             At=0:1          
7C8 beq        At,T1 83C                                         
7CC ori        At,R0,$FFFF                       At=0:FFFF       
7D0 beq        At,S4 8A0                                         
7D4 NOP
7D8 beq        R0,S4 974                                         
7DC movi       At,$4000                          At=0:4000       
7E0 beq        At,S4 988                                         
7E4 movi       At,$2000                          At=0:2000       
7E8 beq        At,S4 998                                         
7EC NOP
7F0 jmp        3C009A4                                           
7F4 NOP
7F8 jal        3C00DEC                                           
7FC lw         V0,$44(T3)                        *(887FFF54)     
800 lbu        T4,$0(V0)                         *(0)            
804 lui        S4,$FFFFBFC0                      S4=BFC00000     
808 addi       S4,S4,$E48                        S4=FFFFFFFF:BFC0
80C add        S4,S4,T4                          S4=FFFFFFFF:BFC0
810 add        S4,S4,T4                          S4=FFFFFFFF:BFC0
814 lhu        S4,$0(S4)                         *(BFC00E48)     
818 bne        R0,S4 834                                         
81C NOP
820 jal        3C00D5C                                           
824 movi       A0,$14                            A0=0:14         
828 sll        V0,V0,$6                          V0=0:0          
82C or         T0,T0,V0                          T0=0:0          
830 movi       S4,$0                             S4=0:0          
834 jmp        3C009A4                                           
838 NOP
83C jal        3C00DEC                                           
840 lw         V0,$48(T3)                        *(887FFF58)     
844 lbu        T6,$0(V0)                         *(0)            
848 lui        S4,$FFFFBFC0                      S4=BFC00000     
84C addi       S4,S4,$EC8                        S4=FFFFFFFF:BFC0
850 add        S4,S4,T6                          S4=FFFFFFFF:BFC0
854 add        S4,S4,T6                          S4=FFFFFFFF:BFC0
858 lhu        S4,$0(S4)                         *(BFC00EC8)     
85C bne        R0,S4 898                                         
860 NOP
864 jal        3C00DEC                                           
868 lw         V0,$94(T3)                        *(887FFFA4)     
86C lbu        V0,$0(V0)                         *(0)            
870 or         T5,T5,V0                          T5=0:0          
874 jal        3C00DEC                                           
878 lw         V0,$64(T3)                        *(887FFF74)     
87C lbu        V0,$0(V0)                         *(0)            
880 sll        V0,V0,$8                          V0=0:0          
884 or         T4,T4,V0                          T4=0:0          
888 jal        3C00DEC                                           
88C lw         V0,$68(T3)                        *(887FFF78)     
890 lbu        V0,$0(V0)                         *(0)            
894 or         T4,T4,V0                          T4=0:0          
898 jmp        3C009A4                                           
89C NOP
8A0 jal        3C00DEC                                           
8A4 lw         V0,$98(T3)                        *(887FFFA8)     
8A8 lbu        V0,$0(V0)                         *(0)            
8AC lui        S4,$FFFFBFC0                      S4=BFC00000     
8B0 addi       S4,S4,$F08                        S4=FFFFFFFF:BFC0
8B4 add        S4,S4,V0                          S4=FFFFFFFF:BFC0
8B8 add eq     S4,V0                             S4=FFFFFFFF:BFC0
8BC lhu        S4,$0(S4)                         *(BFC00F08)     
8C0 or         T5,T5,V0                          T5=0:0          
8C4 bne        R0,S4 900                                         
8C8 NOP
8CC jal        3C00DEC                                           
8D0 lw         V0,$94(T3)                        *(887FFFA4)     
8D4 lbu        V0,$0(V0)                         *(0)            
8D8 or         T6,T6,V0                          T6=0:0          
8DC jal        3C00DEC                                           
8E0 lw         V0,$64(T3)                        *(887FFF74)     
8E4 lbu        V0,$0(V0)                         *(0)            
8E8 sll        V0,V0,$8                          V0=0:0          
8EC or         T4,T4,V0                          T4=0:0          
8F0 jal        3C00DEC                                           
8F4 lw         V0,$68(T3)                        *(887FFF78)     
8F8 lbu        V0,$0(V0)                         *(0)            
8FC or         T4,T4,V0                          T4=0:0          
900 movi       At,$C00                           At=0:C00        
904 bne        At,S4 950                                         
908 NOP
90C jal        3C00DEC                                           
910 lw         V0,$9C(T3)                        *(887FFFAC)     
914 lbu        V0,$0(V0)                         *(0)            
918 or         T6,T6,V0                          T6=0:0          
91C slti       At,T6,$4                                          
920 bne        R0,At 950                                         
924 NOP
928 jal        3C00DEC                                           
92C lw         V0,$64(T3)                        *(887FFF74)     
930 lbu        V0,$0(V0)                         *(0)            
934 sll        V0,V0,$8                          V0=0:0          
938 or         T4,T4,V0                          T4=0:0          
93C jal        3C00DEC                                           
940 lw         V0,$68(T3)                        *(887FFF78)     
944 lbu        V0,$0(V0)                         *(0)            
948 movi       S4,$0                             S4=0:0          
94C or         T4,T4,V0                          T4=0:0          
950 ori        At,R0,$FFFF                       At=0:FFFF       
954 bne        At,S4 96C                                         
958 NOP
95C jal        3C00D5C                                           
960 movi       A0,$15                            A0=0:15         
964 or         T0,T0,V0                          T0=0:0          
968 movi       S4,$0                             S4=0:0          
96C jmp        3C009A4                                           
970 NOP
974 jal        3C00D5C                                           
978 movi       A0,$1A                            A0=0:1A         
97C or         T0,T0,V0                          T0=0:0          
980 jmp        3C009A4                                           
984 movi       S4,$0                             S4=0:0          
988 lui        T0,$3E0                           T0=3E00000      
98C ori        T0,T0,$8                          T0=0:3E00008    
990 jmp        3C009A4                                           
994 movi       T1,$0                             T1=0:0          
998 movi       T0,$0                             T0=0:0          
99C jmp        3C009A4                                           
9A0 movi       T1,$0                             T1=0:0          

9A4 andi       V0,S4,$4                          V0=0:0          
9A8 beq        R0,V0 9D8                                         
9AC NOP
9B0 jal        3C00DEC                                           
9B4 lw         V0,$50(T3)                        *(887FFF60)     
9B8 lbu        A2,$0(V0)                         *(0)            
9BC addi       A1,T3,$0                          A1=FFFFFFFF:887F
9C0 add        A2,A2,T3                          A2=0:10FFFE64   
9C4 lb         A2,$0(A2)                         *(10FFFE64)     
9C8 jal        3C00CD4                                           
9CC or         T6,T6,A2                          T6=0:10FFFE64   
9D0 jmp        3C009F4                                           
9D4 NOP

9D8 andi       V0,S4,$8                          V0=0:0          
9DC beq        R0,V0 9F4                                         
9E0 NOP
9E4 jal        3C00DEC                                           
9E8 lw         V0,$60(T3)                        *(887FFF70)     
9EC lbu        V0,$0(V0)                         *(0)            
9F0 or         T6,T6,V0                          T6=0:10FFFE64   
9F4 andi       V0,S4,$1                          V0=0:0          
9F8 beq        R0,V0 A5C                                         
9FC NOP
A00 andi       V0,S4,$200                        V0=0:0          
A04 beq        R0,V0 A34                                         
A08 NOP
A0C jal        3C00DEC                                           
A10 lw         V0,$5C(T3)                        *(887FFF6C)     
A14 lbu        A2,$0(V0)                         *(0)            
A18 addi       A1,T3,$20                         A1=FFFFFFFF:887F
A1C add        A2,A2,T3                          A2=FFFFFFFF:997F
A20 lb         A2,$20(A2)                        *(997FFD94)     
A24 jal        3C00CD4                                           
A28 or         T5,T5,A2                          T5=FFFFFFFF:997F
A2C jmp        3C00A54                                           
A30 NOP
A34 jal        3C00DEC                                           
A38 lw         V0,$4C(T3)                        *(887FFF5C)     
A3C lbu        A2,$0(V0)                         *(0)            
A40 addi       A1,T3,$0                          A1=FFFFFFFF:887F
A44 add        A2,A2,T3                          A2=0:21FFFC84   
A48 lb         A2,$0(A2)                         *(21FFFC84)     
A4C jal        3C00CD4                                           
A50 or         T5,T5,A2                          T5=FFFFFFFF:B9FF
A54 jmp        3C00A78                                           
A58 NOP
A5C andi       V0,S4,$2                          V0=0:0          
A60 beq        R0,V0 A78                                         
A64 NOP
A68 jal        3C00DEC                                           
A6C lw         V0,$60(T3)                        *(887FFF70)     
A70 lbu        V0,$0(V0)                         *(0)            
A74 or         T5,T5,V0                          T5=FFFFFFFF:B9FF
A78 andi       V0,S4,$10                         V0=0:0          
A7C beq        R0,V0 AAC                                         
A80 NOP
A84 jal        3C00DEC                                           
A88 lw         V0,$54(T3)                        *(887FFF64)     
A8C lbu        A2,$0(V0)                         *(0)            
A90 addi       A1,T3,$0                          A1=FFFFFFFF:887F
A94 add        A2,A2,T3                          A2=FFFFFFFF:AA7F
A98 lb         A2,$0(A2)                         *(AA7FFB94)     
A9C jal        3C00CD4                                           
AA0 or         T7,T7,A2                          T7=FFFFFFFF:AA7F
AA4 jmp        3C00AC8                                           
AA8 NOP
AAC andi       V0,S4,$20                         V0=0:0          
AB0 beq        R0,V0 AC8                                         
AB4 NOP
AB8 jal        3C00DEC                                           
ABC lw         V0,$60(T3)                        *(887FFF70)     
AC0 lbu        V0,$0(V0)                         *(0)            
AC4 or         T7,T7,V0                          T7=FFFFFFFF:AA7F
AC8 andi       V0,S4,$40                         V0=0:0          
ACC beq        R0,V0 AE4                                         
AD0 NOP
AD4 jal        3C00DEC                                           
AD8 lw         V0,$58(T3)                        *(887FFF68)     
ADC jmp        3C00AFC                                           
AE0 lbu        S3,$0(V0)                         *(0)            
AE4 andi       V0,S4,$80                         V0=0:0          
AE8 beq        R0,V0 AFC                                         
AEC NOP
AF0 jal        3C00DEC                                           
AF4 lw         V0,$60(T3)                        *(887FFF70)     
AF8 lbu        S3,$0(V0)                         *(0)            
AFC andi       V0,S4,$100                        V0=0:0          
B00 beq        R0,V0 B54                                         
B04 NOP
B08 jal        3C00DEC                                           
B0C lw         V0,$6C(T3)                        *(887FFF7C)     
B10 lbu        V0,$0(V0)                         *(0)            
B14 sll        V0,V0,$14                         V0=0:0          
B18 or         T0,T0,V0                          T0=0:0          
B1C jal        3C00DEC                                           
B20 lw         V0,$70(T3)                        *(887FFF80)     
B24 lbu        V0,$0(V0)                         *(0)            
B28 sll        V0,V0,$E                          V0=0:0          
B2C or         T0,T0,V0                          T0=0:0          
B30 jal        3C00DEC                                           
B34 lw         V0,$74(T3)                        *(887FFF84)     
B38 lbu        V0,$0(V0)                         *(0)            
B3C sll        V0,V0,$7                          V0=0:0          
B40 or         T0,T0,V0                          T0=0:0          
B44 jal        3C00DEC                                           
B48 lw         V0,$78(T3)                        *(887FFF88)     
B4C lbu        V0,$0(V0)                         *(0)            
B50 or         T0,T0,V0                          T0=0:0          
B54 andi       V0,S4,$C00                        V0=0:0          
B58 movi       At,$400                           At=0:400        
B5C bne        At,V0 B88                                         
B60 NOP
B64 jal        3C00DEC                                           
B68 lw         V0,$7C(T3)                        *(887FFF8C)     
B6C lbu        V0,$0(V0)                         *(0)            
B70 sll        V0,V0,$8                          V0=0:0          
B74 or         T4,T4,V0                          T4=0:0          
B78 jal        3C00DEC                                           
B7C lw         V0,$80(T3)                        *(887FFF90)     
B80 lbu        V0,$0(V0)                         *(0)            
B84 or         T4,T4,V0                          T4=0:0          
B88 andi       V0,S4,$C00                        V0=0:0          
B8C movi       At,$800                           At=0:800        
B90 bne        At,V0 BBC                                         
B94 NOP
B98 jal        3C00DEC                                           
B9C lw         V0,$84(T3)                        *(887FFF94)     
BA0 lbu        V0,$0(V0)                         *(0)            
BA4 sll        V0,V0,$8                          V0=0:0          
BA8 or         T4,T4,V0                          T4=0:0          
BAC jal        3C00DEC                                           
BB0 lw         V0,$88(T3)                        *(887FFF98)     
BB4 lbu        V0,$0(V0)                         *(0)            
BB8 or         T4,T4,V0                          T4=0:0          
BBC andi       V0,S4,$C00                        V0=0:0          
BC0 movi       At,$C00                           At=0:C00        
BC4 bne        At,V0 BF0                                         
BC8 NOP
BCC jal        3C00DEC                                           
BD0 lw         V0,$8C(T3)                        *(887FFF9C)     
BD4 lbu        V0,$0(V0)                         *(0)            
BD8 sll        V0,V0,$8                          V0=0:0          
BDC or         T4,T4,V0                          T4=0:0          
BE0 jal        3C00DEC                                           
BE4 lw         V0,$90(T3)                        *(887FFFA0)     
BE8 lbu        V0,$0(V0)                         *(0)            
BEC or         T4,T4,V0                          T4=0:0          
BF0 andi       V0,S4,$1000                       V0=0:0          
BF4 beq        R0,V0 C0C                                         
BF8 NOP
BFC jal        3C00DEC                                           
C00 lw         V0,$60(T3)                        *(887FFF70)     

C04 lbu        V0,$0(V0)                         *(0)            
C08 or         T4,T4,V0                          T4=0:0          
C0C sll        T1,T1,$1A                         T1=0:0          
C10 or         T0,T0,T1                          T0=0:0          
C14 or         T0,T0,T4                          T0=0:0          
C18 sll        T5,T5,$15                         T5=FFFFFFFF:BE80
C1C or         T0,T0,T5                          T0=FFFFFFFF:BE80
C20 sll        T6,T6,$10                         T6=FFFFFFFF:FE64
C24 or         T0,T0,T6                          T0=FFFFFFFF:FEE4
C28 sll        T7,T7,$B                          T7=FFFFFFFF:FFDC
C2C or         T0,T0,T7                          T0=FFFFFFFF:FFFC
C30 sll        S3,S3,$6                          S3=0:0          
C34 andi       V0,A3,$1F                         V0=0:0          
C38 bne        R0,V0 C64                                         
C3C or         T0,T0,S3                          T0=FFFFFFFF:FFFC
C40 movi       V0,$7C00                          V0=0:7C00       
C44 dsll       T4,V0,$10                                         
C48 or         V0,V0,T4                          V0=0:7C00       
C4C lui        At,$FFFFB000                      At=B0000000     
C50 dsll32     T4,V0,$20                                         
C54 lw         R0,$A0(At)                        *(B00000A0)     
C58 or         V0,V0,T4                          V0=0:7C00       
C5C sd         V0,$0(T8)                         *(A0030000)     
C60 addi       T8,T8,$8                          T8=FFFFFFFF:A003
C64 addi       T2,T2,$FFFFFFFC                   T2=FFFFFFFF:FFFF
C68 sw         T0,$0(A3)                         *(0)            
C6C bne        R0,T2 788                                         
C70 addi       A3,A3,$4                          A3=0:4          
C74 addi       S5,S5,$FFFFFFFF                   S5=FFFFFFFF:FFFF
C78 bne        R0,S5 708                                         
C7C NOP
C80 lui        A0,$FFFF8007                      A0=80070000     
C84 lui        V1,$FFFF8003                      V1=80030000     
C88 ori        A0,A0,$F000                       A0=0:8007F000   
C8C addi       V1,V1,$20                         V1=FFFFFFFF:8003
C90 bne        V1,V1 C8C                                         
C94 lw         R0,$FFFFFFE0(V1)                  *(80030000)     
C98 jal        3C0049C                                           
C9C NOP
CA0 jal        3C00430                                           
CA4 NOP
CA8 jmp        3C005AC                                           
CAC mov        RA,S6  
                           RA=0:0          
CB0 mov        A1,T3                             A1=FFFFFFFF:887F
CB4 movi       V1,$1F                            V1=0:1F         
CB8 sb         V1,$0(A1)                         *(887FFF10)     
CBC sb         V1,$20(A1)                        *(887FFF30)     
CC0 addi       V1,V1,$FFFFFFFF                   V1=0:1E         
CC4 bgez       V1 CB8                                            
CC8 addi       A1,A1,$1                          A1=FFFFFFFF:887F
CCC jr         RA                                                
CD0 NOP
CD4 movi       V1,$FFFFFFE1                      V1=FFFFFFFF:FFFF
CD8 lb         V0,$0(A1)                         *(887FFF11)     
CDC beq        A2,V0 CF0                                         
CE0 NOP
CE4 addi       V1,V1,$1                          V1=FFFFFFFF:FFFF
CE8 jmp        3C00CD8                                           
CEC addi       A1,A1,$1                          A1=FFFFFFFF:887F
CF0 bgez       V1 D0C                                            
CF4 NOP
CF8 lb         V0,$1(A1)                         *(887FFF13)     
CFC addi       V1,V1,$1                          V1=FFFFFFFF:FFFF
D00 addi       A1,A1,$1                          A1=FFFFFFFF:887F
D04 jmp        3C00CF0                                           
D08 sb         V0,$FFFFFFFF(A1)                  *(887FFF12)     
D0C jr         RA                                                
D10 sb         A2,$0(A1)                         *(887FFF13)     

D14 ldl        At,$7(A1)                         *(887FFF1A)     
D18 ldr        At,$0(A1)                         *(887FFF13)     
D1C addi       A1,A1,$7                          A1=FFFFFFFF:887F
D20 sw         A1,$8(A0)                         *(8007F008)     
D24 sd         At,$0(A0)                         *(8007F000)     
D28 movi       At,$FFFFFFC8                      At=FFFFFFFF:FFFF
D2C sb         At,$C(A0)                         *(8007F00C)     
D30 jr         RA                                                
D34 addi       A1,A1,$FFFFFFF9                   A1=FFFFFFFF:887F

D38 ld         S0,$0(A0)                         *(8007F000)     
D3C lw         S1,$8(A0)                         *(8007F008)     
D40 jr         RA                                                
D44 lb         S2,$C(A0)                         *(8007F00C)     

D48 jmp        3C00D5C                                           
D4C movi       A0,$20                            A0=0:20         

D50 jmp        3C00D5C                                           
D54 movi       A0,$10                            A0=0:10         
D58 movi       A0,$8                             A0=0:8          

D5C ldl        V1,$7(S1)                         *(7)            
D60 ldr        V1,$0(S1)                         *(0)            
D64 sub        S2,R0,S2                          S2=0:0          
D68 dsllv      V0,V1,S2                          V0=FFFFFFFF:FFFF
D6C or         S0,S0,V0                          S0=FFFFFFFF:FFFF
D70 sub        S2,A0,S2                          S2=0:8          
D74 movi       V0,$1                             V0=0:1          
D78 dsllv      V0,V0,A0                          V0=0:100        
D7C daddi      V0,V0,$FFFFFFFF                   V0=0:FF         
D80 blez       S2 D98                                            
D84 and        V0,S0,V0                          V0=0:E3         
D88 addi       S1,S1,$7                          S1=0:7          
D8C dsrlv      S0,V1,S2                                          
D90 jr         RA                                                
D94 addi       S2,S2,$FFFFFFC8                   S2=FFFFFFFF:FFFF

D98 jr         RA                                                
D9C dsrlv      S0,S0,A0                                          

DA0 add eq     S2,A0                             S2=FFFFFFFF:FFFF
DA4 blez       S2 DC0                                            
DA8 dsrlv      S0,S0,A0                                          
DAC ldl        S0,$7(S1)                         *(E)            
DB0 ldr        S0,$0(S1)                         *(7)            
DB4 addi       S1,S1,$7                          S1=0:E          
DB8 dsrlv      S0,S0,S2                                          
DBC addi       S2,S2,$FFFFFFC8                   S2=FFFFFFFF:FFFF
DC0 jr         RA                                                
DC4 NOP
DC8 ldl        S0,$7(V0)                         *(EA)           
DCC movi       S2,$FFFFFFC8                      S2=FFFFFFFF:FFFF
DD0 addi       S1,V0,$7                          S1=0:EA         
DD4 jr         RA                                                
DD8 ldr        S0,$0(V0)                         *(E3)           
DDC sub        V0,R0,S2                          V0=0:38         
DE0 srl        V0,V0,$3                          V0=0:7          
DE4 jr         RA                                                
DE8 sub        V0,S1,V0                          V0=0:E3         


input: v0,s0,s2,s1
output:

void SomeFunc()
{

	do()
	{
		a0=v0[0];
		v0++;
		a1=s0&1;
		if(a0<=0)
		{
			tmp=a0;
			a0&=0x7f;
			if(tmp!=0)
			{
				return;
			}
			a0<<=8;
			at=v0[0];		//note, v0++ above
			a0+=at;
			v0++;
		}
		s2++;
		if(s2>0)
		{
			s0>>=1;
			s0=s1[0];
			s1+=7;
			s0>>=s2;
			s2-=56;
		}
		v0+=a0;
	} while(1);
}

*/

DEC lb         A0,$0(V0)                         *(E3)           
DF0 addi       V0,V0,$1                          V0=0:E4         
DF4 bgtz       A0 E14                                            
DF8 andi       A1,S0,$1                          A1=0:1          
DFC beq        R0,A0 E40                                         
E00 andi       A0,A0,$7F                         A0=0:8          
E04 sll        A0,A0,$8                          A0=0:800        
E08 lbu        At,$0(V0)                         *(E4)           
E0C add eq     A0,At                             A0=0:7C8        
E10 addi       V0,V0,$1                          V0=0:E5         
E14 addi       S2,S2,$1                          S2=FFFFFFFF:FFFF
E18 blez       S2 E34                                            
E1C dsrl       S0,S0,$1                                          
E20 ldl        S0,$7(S1)                         *(F1)           
E24 ldr        S0,$0(S1)                         *(EA)           
E28 addi       S1,S1,$7                          S1=0:F1         
E2C dsrlv      S0,S0,S2                                          
E30 addi       S2,S2,$FFFFFFC8                   S2=FFFFFFFF:FFFF
E34 bne        R0,A1 DEC                                         
E38 beq        R0,A1 DEC                                         
E3C add eq     V0,A0                             V0=0:8AD        
E40 jr         RA                                                
E44 NOP


E48 dsrlv      R0,R0,R0                                          
E4C dsrlv      R0,S6,V0                                          
Reserved 
Reserved 
Reserved 
E5C NOP
E60 NOP

E64 NOP
E68 div        A1,T1                                             
E6C div        A1,T1                                             
Reserved 
Reserved 
E78 or         R0,A1,A1                          R0=0:1          
E7C or         R0,A1,A1                          R0=0:1          
E80 or         R0,A1,A1                          R0=0:1          
E84 or         R0,A1,A1                          R0=0:1          
Reserved 
Reserved 
Reserved 
Reserved 
E98 NOP
Reserved 
Reserved 
Reserved 
Reserved 
Reserved 
Reserved 
Reserved 
EB8 dsrlv      R0,R0,R0                                          
EBC dsrlv      R0,S6,V0                                          
EC0 dsrlv      R0,R0,R0                                          
EC4 dsrlv      R0,S6,V0                                          
EC8 jal        43004                                             
ECC jal        43004                                             
ED0 NOP
ED4 NOP
ED8 jmp        42004                                             
EDC jmp        42004                                             
Reserved 
Reserved 
EE8 jal        43004                                             
EEC jal        43004                                             
EF0 NOP
EF4 NOP
EF8 NOP
EFC NOP
F00 NOP
F04 NOP
F08 beq        S4,A0 515C                                        
F0C dsllv      V0,R0,R0                          V0=0:2          
F10 beq        S4,A0 5164                                        
F14 dsllv      V0,R0,R0                          V0=0:2          
F18 sll        At,R0,$10                         At=0:10000      
F1C NOP
F20 NOP
F24 NOP
F28 sd         RA,$FFFFFFFF(RA)                  *(EF3)          
F2C sd         RA,$FFFFFFFF(RA)                  *(EF3)          
F30 sd         RA,$FFFFFFFF(RA)                  *(EF3)          
F34 sd         RA,$FFFFFFFF(RA)                  *(EF3)          
F38 sd         RA,$FFFFFFFF(RA)                  *(EF3)          
F3C sd         RA,$FFFFFFFF(RA)                  *(EF3)          
F40 sd         RA,$FFFFFFFF(RA)                  *(EF3)          
F44 sd         RA,$FFFFFFFF(RA)                  *(EF3)          
F48 NOP
F4C sll        R0,R0,$4                          R0=0:10         
F50 jal        143014                                            
F54 jal        243024                                            
F58 jmp        142014                                            
F5C jmp        142014                                            
F60 jmp        142014                                            
F64 jmp        182014                                            
F68 sd         RA,$FFFFFFFF(RA)                  *(F5B)          
F6C dsra32     RA,R0,$3F                                         
F70 jal        143014                                            
F74 jal        243024                                            
F78 jmp        142014                                            
Reserved 
F80 NOP
F84 NOP
Reserved 
Reserved 
Reserved 
Reserved 
Reserved 
Reserved 
Reserved 
Reserved 
Reserved 
Reserved 
Reserved 
Reserved 
Reserved 
Reserved 
Reserved 
Reserved 
FC8 mf0        R0,A0            BADVADDR         R0=0:0          
FCC NOP
FD0 sub        T6,R0,V1                          T6=0:1D         
TGE 
FD8 lwl        R0,$0(R0)                         *(0)            
TLTI 
FE0 srl        R0,R0,$0                          R0=0:0          
FE4 jal        88C                                               
FE8 movi       R0,$20E                           R0=0:20E        
FEC srl        R0,T4,$0                          R0=0:0          
FF0 sll        R0,V0,$4                          R0=0:20         
FF4 bltzl      T1 108C                                           
FF8 srl        R0,T2,$0                          R0=0:FFFFFFFC   
Bcf0 
