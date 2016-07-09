//******************************************************************
// Special symbols for using with the HCS08 microcontrollers
// Author : Fábio Pereira
// 
// For the book: 
// HCS08 Unleashed: Designer's guide for the HCS08 Microcontrollers 
// Copyright by Fábio Pereira
//******************************************************************

#define BIT_0 1
#define BIT_1 2
#define BIT_2 4
#define BIT_3 8
#define BIT_4 16
#define BIT_5 32
#define BIT_6 64
#define BIT_7 128

#define STOP  {asm stop;}
#define WAIT  {asm wait;}
#define BGND  {asm bgnd;}

// SOPT /SOPT1 register
#define bCOPE    		BIT_7
#define bCOPT    		BIT_6
#define bSTOPE   		BIT_5
#define bSCIPS			BIT_4
#define bSCI2PS			BIT_4
#define bIICPS_DZ		BIT_3
#define bIICPS1			0
#define bIICPS2			BIT_2
#define bIICPS3			BIT_3
#define bIICPS_SH		BIT_2
#define bBKGDPE  		BIT_1
#define bRSTPE   		BIT_0
#define COP_DISABLED	0
#define	COP_FAST		BIT_6
#define COP_MEDIUM		BIT_7
#define COP_SLOW		(BIT_7 | BIT_6)

// SOPT2 register
#define bCOPCLKS 		BIT_7
#define bCOPW			BIT_6
#define bTPM2CH2PS		BIT_5
#define bTPM1CH2PS		BIT_4
#define bTPMCH0PS		BIT_4
#define bADHTS			BIT_4
#define bTPMCCFG		BIT_3
#define bT2CH1PS		BIT_3
#define bT2CH0PS		BIT_2
#define bT1CH1PS		BIT_1
#define bT1CH0PS		BIT_0
#define bSPI1FE			BIT_2
#define bSPI2FE			BIT_1
#define bSPIPS			BIT_2
#define bSPI1PS			BIT_3
#define bIICPS   		BIT_1
#define bIIC1PS			BIT_1
#define bACIC    		BIT_0
#define bACIC1			BIT_0
#define bACIC2			BIT_2
#define bSPIFE			BIT_0
#define MCLK_DISABLED	0
#define MCLK_DIV2		1
#define MCLK_DIV4		2
#define MCLK_DIV8		3
#define MCLK_DIV16		4
#define MCLK_DIV32		5
#define MCLK_DIV64		6
#define MCLK_DIV128		7

// SRS register
#define bPOR   BIT_7
#define bPIN   BIT_6
#define bCOP   BIT_5
#define bILOP  BIT_4
#define bILAD  BIT_3
#define bICG   BIT_2
#define bLVD   BIT_1

// SPMSC1 register
#define bLVDF    BIT_7
#define bLVDACK  BIT_6
#define bLVDIE   BIT_5
#define bLVDRE   BIT_4
#define bSAFE    BIT_4
#define bLVDSE   BIT_3
#define bLVDE    BIT_2
#define bBGBE	 BIT_0

// SPMC2 register
#define bLVWF       BIT_7
#define bLPR        BIT_7
#define bLVWACK     BIT_6
#define bLPRS       BIT_6
#define bLVDV       BIT_5
#define bLPWUI      BIT_5
#define bLVWV       BIT_4
#define bPDF        BIT_4
#define bPPDF       BIT_3
#define bPPDACK     BIT_2
#define bPDC        BIT_1
#define bPPDE       BIT_1
#define bPPDC       BIT_0

// IRQSC register
#define bIRQDD   BIT_6
#define bIRQPDD  BIT_6
#define bIRQEDG  BIT_5
#define bIRQPE   BIT_4
#define bIRQF    BIT_3
#define bIRQACK  BIT_2
#define bIRQIE   BIT_1
#define bIRQMOD  BIT_0

// SRTISC register
#define bRTIF         BIT_7
#define bRTIACK       BIT_6
#define bRTICLKS      BIT_5
#define bRTIE         BIT_4
#define bRTIS2        BIT_2
#define bRTIS1        BIT_1
#define bRTIS0        BIT_0
#define RTI_OFF       0
#define RTI_8ms       1
#define RTI_DIV256    bRTICLKS | 1
#define RTI_32ms      2
#define RTI_DIV1024   bRTICLKS | 2
#define RTI_64ms      3
#define RTI_DIV2048   bRTICLKS | 3
#define RTI_128ms     4
#define RTI_DIV4096   bRTICLKS | 4
#define RTI_256ms     5
#define RTI_DIV8192   bRTICLKS | 5
#define RTI_512ms     6
#define RTI_DIV16384  bRTICLKS | 6
#define RTI_1024ms    7
#define RTI_DIV32768  bRTICLKS | 7

// RTCSC register
#define bRTC_RTIF     BIT_7
#define bRTCLKS1      BIT_6
#define bRTCLKS0      BIT_5
#define bRTC_RTIE     BIT_4
#define RTCLKS_LPO    0
#define RTCLKS_EXT    bRTCLKS0
#define RTCLKS_INT    bRTCLKS1
#define RTC_OFF       0
#define RTC_PRE8      1
#define RTC_PRE32     2
#define RTC_PRE64     3
#define RTC_PRE128    4
#define RTC_PRE256    5
#define RTC_PRE512    6
#define RTC_PRE1024   7
#define RTC_PRE1      8
#define RTC_PRE2      9
#define RTC_PRE4      10
#define RTC_PRE10     11
#define RTC_PRE16     12
#define RTC_PRE100    13
#define RTC_PRE500    14
#define RTC_PRE1000   15
#define RTC_PRE_1024  (bRTCLKS0 | 1)
#define RTC_PRE_2048  (bRTCLKS0 | 2)
#define RTC_PRE_4096  (bRTCLKS0 | 3)
#define RTC_PRE_8192  (bRTCLKS0 | 4)
#define RTC_PRE_16384 (bRTCLKS0 | 5)
#define RTC_PRE_32768 (bRTCLKS0 | 6)
#define RTC_PRE_65536 (bRTCLKS0 | 7)
#define RTC_PRE_1k    (bRTCLKS0 | 8)
#define RTC_PRE_2k    (bRTCLKS0 | 9)
#define RTC_PRE_5k    (bRTCLKS0 | 10)
#define RTC_PRE_10k   (bRTCLKS0 | 11)
#define RTC_PRE_20k   (bRTCLKS0 | 12)
#define RTC_PRE_50k   (bRTCLKS0 | 13)
#define RTC_PRE_100k  (bRTCLKS0 | 14)
#define RTC_PRE_200k  (bRTCLKS0 | 15)

// GNGC register
#define bGNGPS7		BIT_7
#define bGNGPS6		BIT_6
#define bGNGPS5		BIT_5
#define bGNGPS4		BIT_4
#define bGNGPS3		BIT_3
#define bGNGPS2		BIT_2
#define bGNGPS1		BIT_1
#define bGNGEN		BIT_0

// KBISC register
#define bKBEDG7   BIT_7
#define bKBEDG6   BIT_6
#define bKBEDG5   BIT_5
#define bKBEDG4   BIT_4
#define bKBF      BIT_3
#define bKBACK    BIT_2
#define bKBIE     BIT_1
#define bKBIMOD   BIT_0

// KBIPE register
#define bKBIPE7		BIT_7
#define bKBIPE6		BIT_6
#define bKBIPE5		BIT_5
#define bKBIPE4		BIT_4
#define bKBIPE3		BIT_3
#define bKBIPE2		BIT_2
#define bKBIPE1		BIT_1
#define bKBIPE0		BIT_0

//KBIES register
#define bKBIEDG3	BIT_3
#define bKBIEDG2	BIT_2
#define bKBIEDG1	BIT_1
#define bKBIEDG0	BIT_0

// ICSC1 register
#define ICS_FLL     0
#define ICS_FBI     BIT_6
#define ICS_FBE     BIT_7
#define RDIV_1      0
#define RDIV_2      BIT_3
#define RDIV_4      BIT_4
#define RDIV_8      (BIT_4 | BIT_3)
#define RDIV_16     BIT_5
#define RDIV_32     (BIT_5 | BIT_3)
#define RDIV_64     (BIT_5 | BIT_4)
#define RDIV_128    (BIT_5 | BIT_4 | BIT_3)
#define bIREFS      BIT_2
#define bIRCLKEN    BIT_1
#define bIREFSTEN   BIT_0

// ICSC2 register
#define BDIV_1      0
#define BDIV_2      BIT_6
#define BDIV_4      BIT_7
#define BDIV_8      (BIT_7 | BIT_6)
#define bICS_RANGE  BIT_5
#define bICS_HGO    BIT_4
#define bICS_LP     BIT_3
#define bEREFS      BIT_2
#define bERCLKEN    BIT_1
#define bEREFSTEN   BIT_0

// ICSSC register
#define DCO_LOW       0
#define DCO_MID       BIT_6
#define DCO_HIGH      BIT_7
#define bDMX32        BIT_5
#define bIREFST       BIT_4
#define bOSCINIT      BIT_1
#define bFTRIM        BIT_0

// ICGC1 register
#define bICG_HGO      BIT_7
#define bICG_RANGE    BIT_6
#define bREFS         BIT_5
#define bCLKS1        BIT_4
#define bCLKS0        BIT_3
#define bOSCSTEN      BIT_2
#define bLOCD         BIT_1
#define ICG_SCM       0
#define ICG_FEI       bCLKS0
#define ICG_FBE       bCLKS1
#define ICG_FEE       (bCLKS1 | bCLKS0)

// ICGC2 register
#define bLOLRE      BIT_7
#define bMFD2       BIT_6
#define bMFD1       BIT_5
#define bMFD0       BIT_4
#define bLOCRE      BIT_3
#define bRFD2       BIT_2
#define bRFD1       BIT_1
#define bRFD0       BIT_0
#define MFDx4       0
#define MFDx6       bMFD0
#define MFDx8       bMFD1
#define MFDx10      (bMFD1 | bMFD0)
#define MFDx12      bMFD2
#define MFDx14      (bMFD2 | bMFD0)
#define MFDx16      (bMFD2 | bMFD1)
#define MFDx18      (bMFD2 | bMFD1 | bMFD0)
#define RFD_DIV1    0
#define RFD_DIV2    1
#define RFD_DIV4    2
#define RFD_DIV8    3
#define RFD_DIV16   4
#define RFD_DIV32   5
#define RFD_DIV64   6
#define RFD_DIV128  7

// ICGS1 register
#define bCLKST1       BIT_7
#define bCLKST0       BIT_6
#define bREFST        BIT_5
#define bLOLS         BIT_4
#define bLOCK         BIT_3
#define bLOCS         BIT_2
#define bERCS         BIT_1
#define bICGIF        BIT_0

// ICGS2 register
#define bDCOS       BIT_0

// SCGC1 register
#define bCKENMTIM		BIT_7
#define bCKENTPM3		BIT_7
#define bCKENTPM2		BIT_6
#define bCKENTPM1		BIT_5
#define bCKENTPM		BIT_5
#define bCKENADC		BIT_4
#define bCKENIIC2		BIT_3
#define bCKENIIC1		BIT_2
#define bCKENSCI2		BIT_1
#define bCKENSCI1		BIT_0

//SCGC2 register
#define bCKENDBG		BIT_7
#define bCKENFLS		BIT_6
#define bCKENIRQ		BIT_5
#define bCKENKBI		BIT_4
#define bCKENACMP		BIT_3
#define bCKENRTC		BIT_2
#define bCKENTOD		BIT_2
#define bCKENSPI2		BIT_1
#define bCKENSPI1		BIT_0
#define bCKENLCD		BIT_1

// TPMSC register
#define bTOF        BIT_7
#define bTOIE       BIT_6
#define bCPWMS      BIT_5
#define bCLKSB      BIT_4
#define bCLKSA      BIT_3
#define bPS2        BIT_2
#define bPS1        BIT_1
#define bPS0        BIT_0
#define TPM_OFF     (bCLKSA | bCLKSB)
#define TPM_BUSCLK  bCLKSA
#define TPM_XCLK    bCLKSB
#define TPM_EXT     (bCLKSB | bCLKSA)
#define TPM_DIV1    0
#define TPM_DIV2    1
#define TPM_DIV4    2
#define TPM_DIV8    3
#define TPM_DIV16   4
#define TPM_DIV32   5
#define TPM_DIV64   6
#define TPM_DIV128  7

// TPMxCxSC register
#define bCHF                      BIT_7
#define bCHIE                     BIT_6
#define bMSB                      BIT_5
#define bMSA                      BIT_4
#define bELSB                     BIT_3
#define bELSA                     BIT_2
#define TPM_CAPTURE_RISING_EDGE   bELSA
#define TPM_CAPTURE_FALLING_EDGE  bELSB
#define TPM_CAPTURE_BOTH_EDGES    (bELSA | bELSB)
#define TPM_COMPARE_INT           bMSA
#define TPM_COMPARE_TOGGLE        (bMSA | bELSA)
#define TPM_COMPARE_CLEAR         (bMSA | bELSB)
#define TPM_COMPARE_SET           (bMSA | bELSB | bELSA)
#define TPM_PWM_HIGH              (bMSB | bELSB)
#define TPM_PWM_LOW               (bMSB | bELSA)

// MTIMSC register
#define bTOF    BIT_7
#define bTOIE   BIT_6
#define bTRST   BIT_5
#define bTSTP   BIT_4

// MTIMCLK register
#define MTIM_BUSCLK             0
#define MTIM_XCLK               BIT_4
#define MTIM_EXT_FALLING_EDGE   BIT_5
#define MTIM_EXT_RISING_EDGE    (BIT_5 | BIT_4)
#define MTIM_DIV1               0
#define MTIM_DIV2               1
#define MTIM_DIV4               2
#define MTIM_DIV8               3
#define MTIM_DIV16              4
#define MTIM_DIV32              5
#define MTIM_DIV64              6
#define MTIM_DIV128             7
#define MTIM_DIV256             8

// ATDC register
#define bATDPU    BIT_7
#define bDJM      BIT_6
#define bRES8     BIT_5
#define bSGN      BIT_4
#define ATD_PRS2  0
#define ATD_PRS4  1
#define ATD_PRS6  2
#define ATD_PRS8  3
#define ATD_PRS10 4
#define ATD_PRS12 5
#define ATD_PRS14 6
#define ATD_PRS16 7
#define ATD_PRS18 8
#define ATD_PRS20 9
#define ATD_PRS22 10
#define ATD_PRS24 11
#define ATD_PRS26 12
#define ATD_PRS28 13
#define ATD_PRS30 14
#define ATD_PRS32 15

// ATDSC register
#define bCCF      BIT_7
#define bATDIE    BIT_6
#define bATDCO    BIT_5
#define ATD_AD0   0
#define ATD_AD1   1
#define ATD_AD2   2
#define ATD_AD3   3
#define ATD_AD4   4
#define ATD_AD5   5
#define ATD_AD6   6
#define ATD_AD7   7
#define ATD_VREFH 30
#define ATD_VREFL 31

// ADCSC1 register
#define bCOCO     BIT_7
#define bAIEN     BIT_6
#define bADCO     BIT_5
#define ADC_CH0   0
#define ADC_CH1   1
#define ADC_CH2   2
#define ADC_CH3   3
#define ADC_CH4   4
#define ADC_CH5   5
#define ADC_CH6   6
#define ADC_CH7   7
#define ADC_CH8   8
#define ADC_CH9   9
#define ADC_CH10  10
#define ADC_CH11  11
#define ADC_CH12  12
#define ADC_CH13  13
#define ADC_CH14  14
#define ADC_CH15  15
#define ADC_CH16  16
#define ADC_CH17  17
#define ADC_CH18  18
#define ADC_CH19  19
#define ADC_CH20  20
#define ADC_CH21  21
#define ADC_CH22  22
#define ADC_CH23  23
#define ADC_CH24  24
#define ADC_CH25  25
#define ADCH_TEMP_SENSOR 26
#define ADCH_BANDGAP  27
#define ADCH_REFH 29
#define ADCH_REFL 30
#define ADC_OFF   31

// ADCSC2 register
#define bADACT    BIT_7
#define bADTRG    BIT_6
#define bACFE     BIT_5
#define bACFGT    BIT_4

// ADCCFG register
#define bADLPC          BIT_7
#define ADIV_1          0
#define ADIV_2          BIT_5
#define ADIV_4          BIT_6
#define ADIV_8          (BIT_6 | BIT_5)
#define bADLSMP         BIT_4
#define ADC_8BITS       0
#define ADC_10BITS      BIT_3
#define ADC_12BITS      BIT_2
#define ADC_BUSCLK      0
#define ADC_BUSCLK_DIV2 1
#define ADC_ALTCLK      2
#define ADC_INTCLK      3

// APCTL1 register
#define bADPC0   BIT_0
#define bADPC1   BIT_1
#define bADPC2   BIT_2
#define bADPC3   BIT_3
#define bADPC4   BIT_4
#define bADPC5   BIT_5
#define bADPC6   BIT_6
#define bADPC7   BIT_7

// APCTL2
#define bADPC8   BIT_0
#define bADPC9   BIT_1
#define bADPC10  BIT_2
#define bADPC11  BIT_3
#define bADPC12  BIT_4
#define bADPC13  BIT_5
#define bADPC14  BIT_6
#define bADPC15  BIT_7

// APCTL3
#define bADPC16  BIT_0
#define bADPC17  BIT_1
#define bADPC18  BIT_2
#define bADPC19  BIT_3
#define bADPC20  BIT_4
#define bADPC21  BIT_5
#define bADPC22  BIT_6
#define bADPC23  BIT_7     

// ACMPSC register
#define bACME   BIT_7
#define bACBGS  BIT_6
#define bACF    BIT_5
#define bACIE   BIT_4
#define bACO    BIT_3
#define bACOPE  BIT_2
#define ACMOD_0 0
#define ACMOD_1 BIT_0
#define ACMOD_2 BIT_1
#define ACMOD_3 (BIT_1 | BIT_0)

// SCIC1 register
#define bLOOPS    BIT_7
#define bSCISWAI  BIT_6
#define bRSRC     BIT_5
#define bM        BIT_4
#define bWAKE     BIT_3
#define bILT      BIT_2
#define bPE       BIT_1
#define bPT       BIT_0

// SCIC2 register
#define bTIE      BIT_7
#define bTCIE     BIT_6
#define bRIE      BIT_5
#define bILIE     BIT_4
#define bTE       BIT_3
#define bRE       BIT_2
#define bRWU      BIT_1
#define bSBK      BIT_0

// SCIC3 register
#define bR8       BIT_7
#define bT8       BIT_6
#define bTXDIR    BIT_5
#define bTXINV    BIT_4
#define bORIE     BIT_3
#define bNEIE     BIT_2
#define bFEIE     BIT_1
#define bPEIE     BIT_0

// SCIS1 register
#define bTDRE     BIT_7
#define bTC       BIT_6
#define bRDRF     BIT_5
#define bIDLE     BIT_4
#define bOR       BIT_3
#define bNF       BIT_2
#define bFE       BIT_1
#define bPF       BIT_0

// SCIS2 register
#define bBRK13    BIT_2
#define bRAF      BIT_0

// SPIC1 register
#define bSPIE     BIT_7
#define bSPE      BIT_6
#define bSPTIE    BIT_5
#define bMSTR     BIT_4
#define bCPOL     BIT_3
#define bCPHA     BIT_2
#define bSSOE     BIT_1
#define bLSBFE    BIT_0

// SPIC2 register
#define bMODFEN   BIT_4
#define bBIDIROE  BIT_3
#define bSPISWAI  BIT_1
#define bSPC0     BIT_0

// SPIS register
#define bSPRF    BIT_7
#define bSPTEF   BIT_5
#define bMODF    BIT_4

// SPIBR register
#define bSPPR2      BIT_6
#define bSPPR1      BIT_5
#define bSPPR0      BIT_4
#define bSPR2       BIT_2
#define bSPR1       BIT_1
#define bSPR0       BIT_0
#define SPI_PRE1    0
#define SPI_PRE2    bSPPR0
#define SPI_PRE3    bSPPR1
#define SPI_PRE4    (bSPPR1|bSPPR0)
#define SPI_PRE5    bSPPR2
#define SPI_PRE6    (bSPPR2|bSPPR0)
#define SPI_PRE7    (bSPPR2|bSPPR1)
#define SPI_PRE8    (bSPPR2|bSPPR1|bSPPR0)
#define SPI_DIV2    0
#define SPI_DIV4    bSPR0
#define SPI_DIV8    bSPR1
#define SPI_DIV16   (bSPR1|bSPR0)
#define SPI_DIV32   bSPR2
#define SPI_DIV64   (bSPR2|bSPR0)
#define SPI_DIV128  (bSPR2|bSPR1)
#define SPI_DIV256  (bSPR2|bSPR1|bSPR0)

// IICC register
#define bIICEN    BIT_7
#define bIICIE    BIT_6
#define bMST      BIT_5
#define bTX       BIT_4
#define bTXAK     BIT_3
#define bRSTA     BIT_2

// IICS register
#define bTCF      BIT_7
#define bIAAS     BIT_6
#define bBUSY     BIT_5
#define bARBL     BIT_4
#define bSRW      BIT_2
#define bIICIF    BIT_1
#define bRXAK     BIT_0

// IICF register
#define I2C_M1    0
#define I2C_M2    BIT_6
#define I2C_M4    BIT_7

// FCDIV register
#define bDIVLD    BIT_7
#define bPRDIV8   BIT_6

// FOPT e NVOPT register
#define bKEYEN        	BIT_7
#define bFNORED       	BIT_6
#define bEPGMOD			BIT_5
#define bSEC01        	BIT_1
#define bSEC00        	BIT_0
#define MEM_SECURE    	bSEC00
#define MEM_UNSECURE  	bSEC01

// FCNFG register
#define bKEYACC   BIT_5

// FPROT e NVPROT register
#define bFPOEN    BIT_7
#define bFPDIS    BIT_6
#define bFPS2     BIT_5
#define bFPS1     BIT_4
#define bFPS0     BIT_3
#define bFPDIS0   BIT_0

// FSTAT register
#define bFCBEF    BIT_7
#define bFCCF     BIT_6
#define bFPVIOL   BIT_5
#define bFACCERR  BIT_4
#define bFBLANK   BIT_2

// FCMD register
#define CMD_BLANK     0x05
#define CMD_BYTEPROG  0x20
#define CMD_BURSTPROG 0x25
#define CMD_PAGEERASE 0x40
#define CMD_MASSERASE 0x41

