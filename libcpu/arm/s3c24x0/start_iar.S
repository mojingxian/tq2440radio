;Filename: startup.s
;By zhnyang@21cn.com
;@2009-4-4
;For S3C2440A(Run after Reset)
;Copyright Reserved

;Note: @Little Endian

;##########################################################
; Watchdog Timer Definitions
;----------------------------------------------------------
WT_BASE       EQU     0x53000000      ;Watchdog Base Address
WTCON_OFS     EQU     0x00          
WTDAT_OFS     EQU     0x04
WTCNT_OFS     EQU     0x08

WTCON_Val     EQU     0x00007C18
WTDAT_Val     EQU     0x00007530
;----------------------------------------------------------
; Clock Management Definitions
;----------------------------------------------------------
CLK_BASE      EQU     0x4C000000      ;Clock Base Address
LOCKTIME_OFS  EQU     0x00
MPLLCON_OFS   EQU     0x04
UPLLCON_OFS   EQU     0x08
CLKCON_OFS    EQU     0x0C
CLKSLOW_OFS   EQU     0x10
CLKDIVN_OFS   EQU     0x14
CAMDIVN_OFS   EQU     0x18

LOCKTIME_Val  EQU     0x00FFFFFF
MPLLCON_Val   EQU     0x0007F021   ;FCLK=405MHz
UPLLCON_Val   EQU     0x00038022   ;UCLK=48MHz
CLKCON_Val    EQU     0x0007FFF0
CLKSLOW_Val   EQU     0x00000004
CLKDIVN_Val   EQU     0x00000005   ;FCLK:HCLK:PCLK=8:4:1
CAMDIVN_Val   EQU     0x00000000   
;----------------------------------------------------------
; Memory Controller Definitions
;----------------------------------------------------------
MC_BASE       EQU     0x48000000      ;Memory Controller Base Address

BWSCON_Val    EQU     0x22111110      ;32-bit data bus
BANKCON0_Val  EQU     0x00007FFC
BANKCON1_Val  EQU     0x00007FFC
BANKCON2_Val  EQU     0x00000700
BANKCON3_Val  EQU     0x00000700
BANKCON4_Val  EQU     0x00002E50
BANKCON5_Val  EQU     0x00002E50
BANKCON6_Val  EQU     0x00018005
BANKCON7_Val  EQU     0x00018005
REFRESH_Val   EQU     0x00A804F5
BANKSIZE_Val  EQU     0x000000B1      ;64MB
MRSRB6_Val    EQU     0x00000030
MRSRB7_Val    EQU     0x00000030
;----------------------------------------------------------
; Some ARM920T CPSR bit discriptions
;----------------------------------------------------------
ModeUSR     EQU     0x10
ModeFIQ     EQU     0x11
ModeIRQ     EQU     0x12
ModeSVC     EQU     0x13
ModeABT     EQU     0x17
ModeUND     EQU     0x1B
ModeSYS     EQU     0x1F

I_Bit       DEFINE     0x80    ;I=1,Disable IRQ
F_Bit       DEFINE     0x40    ;F=1,Disable FIQ
;----------------------------------------------------------

    MODULE      startup
    SECTION     CSTACK:DATA:NOROOT(3)
    SECTION     SVC_STACK:DATA:NOROOT(3)
    SECTION     IRQ_STACK:DATA:NOROOT(3)
    SECTION     FIQ_STACK:DATA:NOROOT(3) 
    SECTION     UND_STACK:DATA:NOROOT(3)
    SECTION     ABT_STACK:DATA:NOROOT(3)
    
    SECTION     .intvec:CODE:NOROOT(2)
    PUBLIC      __iar_program_start       ;Must use this label
    ARM
    
__vectors:    ;0~0x3F, 16 Entries
    LDR       PC,=__iar_program_start                 ;RESET    
    NOP                                   ;Undef
    NOP                                   ;SWI
    NOP                                   ;PAbt
    NOP                                   ;DAbt
    NOP                                   ;
    NOP                                   ;IRQ
    NOP                                   ;FIQ
;start_up      DCD       __iar_program_start
    DS32      8
    
    SECTION     .text:CODE:NOROOT(2)
    IMPORT      main
    ARM
    
;Memory Controller Configuration
MC_CFG      DCD     BWSCON_Val
            DCD     BANKCON0_Val
            DCD     BANKCON1_Val
            DCD     BANKCON2_Val
            DCD     BANKCON3_Val
            DCD     BANKCON4_Val
            DCD     BANKCON5_Val
            DCD     BANKCON6_Val
            DCD     BANKCON7_Val
            DCD     REFRESH_Val
            DCD     BANKSIZE_Val
            DCD     MRSRB6_Val
            DCD     MRSRB7_Val
            
;Clock Management Configuration
CLK_CFG     DCD     LOCKTIME_Val
            DCD     CLKDIVN_Val
            DCD     MPLLCON_Val
            DCD     UPLLCON_Val
            DCD     CLKSLOW_Val
            DCD     CLKCON_Val
            DCD     CAMDIVN_Val
            
__iar_program_start:   ;Run after Power ON RESETog Timer                        
;Disable the Reset Function of Watchd 
    LDR     R0,=WT_BASE
    LDR     R1,=WTCON_Val
    LDR     R2,=WTDAT_Val
    STR     R2,[R0,#WTCNT_OFS]
    STR     R2,[R0,#WTDAT_OFS]
    STR     R1,[R0,#WTCON_OFS]   ;0.1Hz(10s)
    
;Set CPU Clock(405MHz),PCLK=405/8MHz
    LDR     R0,=CLK_BASE
    ADR     R8,CLK_CFG
    LDMIA   R8,{R1-R7}
    STR     R1,[R0,#LOCKTIME_OFS]
    STR     R2,[R0,#CLKDIVN_OFS]
    STR     R3,[R0,#MPLLCON_OFS]
    STR     R4,[R0,#UPLLCON_OFS]       
    STR     R5,[R0,#CLKSLOW_OFS]
    STR     R6,[R0,#CLKCON_OFS]
    STR     R7,[R0,#CAMDIVN_OFS]
//    
    MRC     p15,0,R0,c1,c0,0
    ORR     R0,R0,#0xC0000000
    MCR     p15,0,R0,c1,c0,0
;Config Memory(0x3000 0000, 32MB, Bank 6)
        ADR     R14,MC_CFG
        LDMIA   R14,{R0-R12}
        LDR     R14,=MC_BASE
        STMIA   R14,{R0-R12}
        
;Setup Stack for Each Mode        
;Enter Undefined Instruction Mode and set its Stack Pointer
        MSR     CPSR_c, #ModeUND | I_Bit | F_Bit
        LDR     SP,=SFE(UND_STACK)
        
;Enter Abort Mode and set its Stack Pointer
        MSR     CPSR_c, #ModeABT | I_Bit | F_Bit
        LDR     SP,=SFE(ABT_STACK)
        
;Enter FIQ Mode and set its Stack Pointer
        MSR     CPSR_c, #ModeFIQ | I_Bit | F_Bit
        LDR     SP,=SFE(FIQ_STACK)

;Enter IRQ Mode and set its Stack Pointer
        MSR     CPSR_c, #ModeIRQ | I_Bit | F_Bit
        LDR     SP,=SFE(IRQ_STACK)

;Enter Supervisor Mode and set its Stack Pointer
        MSR     CPSR_c, #ModeSVC | I_Bit | F_Bit
        LDR     SP,=SFE(SVC_STACK)
        
;Enter System Mode and set its Stack Pointer
;        MSR     CPSR_c, #modeSYS
;        LDR     SP,=SFE(CSTACK)
;Enter the C Code
    LDR   R0,=main
    BX    R0
    
    END