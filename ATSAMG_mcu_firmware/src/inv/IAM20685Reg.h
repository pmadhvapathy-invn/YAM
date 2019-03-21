/*
 * IncFile1.h
 *
 * Created: 1/14/2019 4:14:03 PM
 *  Author: pmadhvapathy
 */ 
#include "yamTests.h"   //not good -- need to move WhoAmIType out of here
#include "bitdefns.h"
#ifndef IAM20685REG_H_
#define IAM20685REG_H_

#define ALLREGBITS (0xFFFF)
#define NOREGBITS  (0x0000)

#define FIXEDYAM ((uint16_t) 0xAA55)


typedef enum regBankType1
{
	bnone=-1, 
	bGx=0, bGy=0, bGz=0,
	bT1=0,
	bAx=0, bAy=0, bAz=0,
	bT2=0,
	bAxlr=0, bAylr=0, bAzlr=0,
	bFxd=0,
	bZ_flt=0, bY_flt=0, bX_flt=0, bSt_dis=0, bInorA=0, bInorG=0, bInorC=0,
	bT12=0,
	bGa1=0, bGa2 =0,
	bAa1=0, bAa2=0,
	bOa1=0, bOa2=0,
	bTr=0,
	bRsts=0, bRsth=0, bMonSt=0,
	bCap=0, bTcd=0, bWrlk=0,
	bArev=0, bGrev=0, bSrev=0, bSerIdmsb=0, bSerIdlsb=0,
	bBank=0,
	/* Bank 1 */
	bSMen1=1, bSMen2=1,
	bSMmsk1=1, bSMmsk2=1, bSMmsk3=1, bSMmsk4=1, bSMmsk5=1, bSMmsk6=1,
	bSMle1=1, bSMle2=1,
	bSMs=1,
	/* Bank 2 */
	bSMra=2, bSMrg=2,
	/* Bank 6 */
	bFSa=6,
	/* Bank 7 */
	bFSg=7
	
} regBankType;


typedef enum regAddrType1
{
	Gx=0x00, Gy=0x01, Gz=0x02,
	T1=0x03,
	Ax=0x04, Ay=0x05, Az=0x06,
	T2=0x07,
	Axlr=0x08, Aylr=0x09, Azlr=0x0A,
	Fxd=0x0B,
	Z_flt=0x0C, Y_flt=0x0C, X_flt=0x0E, St_dis=0x0E, InorA=0x0E,InorG=0x0E,InorC=0x0E,
	T12=0x0F,
	Ga1=0x10, Ga2 =0x11,
	Aa1=0x12, Aa2=0x13,
	Oa1=0x14, Oa2=0x15,
	Tr=0x17,
	Rsts=0x18, Rsth=0x18, MonSt=0x18,
	Cap=0x19, Tcd=0x19, Wrlk=0x19,
	Arev=0x1B, Grev=0x1B, Srev=0x1C, SerIdmsb=0x1D, SerIdlsb=0x1E,
	Bank=0x1F,
	/* Bank 1 */
	SMen1=0x11, SMen2=0x12, 
	SMmsk1=0x13, SMmsk2=0x14, SMmsk3=0x15, SMmsk4=0x16, SMmsk5=0x17, SMmsk6=0x18,
	SMle1=0x19, SMle2=0x1A, 
	SMs=0x1C,
	/* Bank 2 */
	SMra=0x1C, SMrg=0x1D,
	/* Bank 6 */
	FSa=0x14, 
	/* Bank 7 */
	FSg=0x14,
	rNun=0xff
}regAddrType;
 
typedef enum regBitsType1
{
	fGx=0xFFFF, fGy=0xFFFF, fGz=0xFFFF,
	fT1=0xFFFF,
	fAx=0xFFFF, fAy=0xFFFF, fAz=0xFFFF,
	fT2=0xFFFF,
	fAxlr=0xFFFF, fAylr=0xFFFF, fAzlr=0xFFFF,
	fFxd=0xFFFF,
	fZ_flt=0x0FC0, fY_flt=0x003F, fX_flt=0x03F00, fSt_dis=0x4000, fInorA=0x0008, fInorG=0x0001, fInorC=0x0040,
	fT12=0xFFFF,
	fGa1=0x1DC0, fGa2 =0x701E,
	fAa1=0x3003, fAa2=0x0007,
	fOa1=0x3FFF, fOa2=0x303E,
	fTr=0xFFFF,
	fRsts=0x0002, fRsth=0x0004, fMonSt=0x0008,
	fCap=0x0008, fTcd=0x0007, fWrlk=0x8000, 
	fArev=0x001F, fGrev=0x0F00, fSrev=0x001F, fSerIdmsb=0xFFFF, fSerIdlsb=0xFFFF,
	fBank=0xFFFF,
	/* Bank 1 */
	fSMen1=0x7FD0, fSMen2=0x11D0,
	fSMmsk1=0x3FC0, fSMmsk2=0x1FFF, fSMmsk3=0xD8E7, fSMmsk4=0xE07F, fSMmsk5=0x9C00, fSMmsk6=0xE000,
	fSMle1=0x00BC, fSMle2=0xCFEB,
	fSMs=0xFF18, fSM33ContRegChkPeriod=0xE000, fSMalarmBpin=Bit15,
	/* Bank 2 */
	fSMra=0xFFFF, fSMrg=0xFFFF,
	/* Bank 6 */
	fFSa=0x0007,
	/* Bank 7 */
	fFSg=0x000F
	
} regBitsType;


 



WhoAmIType getWhoAmI(uint16_t * wmi);

typedef enum errWriteType {noProtocolErr=0, crcProtoErr=1, stsProtoRsvd= 2, stsProtoWIP=3, stsProtoErr=4} errWriteType1;

typedef struct   {int bank; char offset; uint16_t defVal; } regdefaultType;
typedef struct regfullAddrType {regBankType b; regAddrType r; uint16_t regfld;} regfullAddrType1;
typedef struct regfullAttrType {regBankType b; regAddrType r; uint16_t regfld; uint16_t defval;} regfullAttrType1;
struct regfullAttrType regArray[];
uint16_t numRegArray();  
typedef enum bitPreserve_Type1 {overwrite=false, preserve= true} bitpreserveType;
// function prototypes
Bool yamWrite(regBankType b, regAddrType a, uint32_t v, uint32_t f, bitpreserveType preservebits);
void LKUNLKWrite(regAddrType a, uint32_t v);
int16_t yamRead(regBankType b, regAddrType a);
int16_t readReg(regBankType b, regAddrType a);		
Bool splitWrite(regAddrType a, uint32_t v);
Bool indirectWrite(regAddrType a, uint32_t v);
Bool regWrite(regAddrType a, uint32_t v);
enum errWriteType onestepWrite(regAddrType a, uint16_t v);
uint32_t ReadReg(regAddrType a, enum errWriteType *e);
enum errWriteType WriteReg(regAddrType a, uint16_t v);
//uint32_t onestepRead(regAddrType a, enum errWriteType *e);
Bool yamWriteCmdAddr(regBankType b, regAddrType a);
Bool SplitReadModifyWrite(regAddrType a, uint32_t v, uint16_t mask);
void bankselect(regBankType banknum);
void handleProtoErr(enum errWriteType e);
void printCmdRespHistory ();
void enableProtocolHistory(bool En_dis);
typedef enum chipResetType1 { HARD_RESET=1, SOFT_RESET=2 } chipResetType;

void selfteststart();
void resetChip( chipResetType r);
uint16_t currentBank();

float accel2gee (int16_t accelVal, bool hiLoRes);
float faccel2gee(float fAccel, bool hiLoRes);
float gyro2dps (int16_t gyroVal);
float fgyro2dps (float gyroVal);

#define TEMP_SENSOR_ROOMTEMP (25.0f)
#define TEMP2CENTgrad (20.0f)
float temp2Centigrade(int16_t temperatureBits);
float accelnormfloat(int16_t x1, int16_t  x2, int16_t x3, bool Lores  // input
,float *nfx1, float *nfx2, float *nfx3   //output
);
void selftestDisable(bool Dis_en);
#endif /* IAM20685REG_H_ */