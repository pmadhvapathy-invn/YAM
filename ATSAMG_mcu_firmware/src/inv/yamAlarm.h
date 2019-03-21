/*
 * IncFile1.h
 *
 * Created: 2/5/2019 2:14:19 PM
 *  Author: pmadhvapathy
 */ 


#ifndef YAMALARM_H_
#define YAMALARM_H_
#include "IAM20685Reg.h"




// Index into Sts Mask Table
typedef enum alarmIndexType1 {
	almpGZquadadc=0, almpGyquadadc, almpGxquadadc, almpRcosc1freq, almpSysclk, almpGzdrvfreq, almpGydrvfreq, almpGxdrvfreq
	, almpTempVref, almpalmpAvref, almpGvref, almpExtvdd, almpVddio, almpDvdd, almpAvdd, almpVddmaster, almpT12, almpTdsp
	, almpGZdrclk, almpGYdrclk, almpGXdrclk, almpCpVref, almpAhbbus, almpOtpcrc, almpOtpreg, almpRegecc, almpAhbecc, almpOtpecc
	, almpAcp, almpVrefshldZ, almpVrefshldXY, almpGZdcst, almpGYdcst, almpGXdcst, almpRcosc2fre1, almpBg, almpRegcheck
	, almpOtpcpy, almpSpiclkcnt, almpSpicmddecod, almpSpicrc, almpMskPin, almpAZdcst, almpAYdcst, almpAXdcst
	, almpNone
} alarmIndexType;

// Index into Enable/Latch table
typedef enum alarmEnableLatchGroupIndexType1 {
	grpDrvfreq=0, grpQuadadc, grpDrvclk, grpTemp, grpVddmaster
	, grpAvdd, grpDvdd, grpExtVdd, grpVddio, grpVref
	, grpAdcst, grpVrefshldXY, grpVrefshldZ, grpAcp
	, /*grpECC, */ grpOtpCRC, grpOtpcpy, grpOtpreg, grpAhb, grpspiclk, grpspicrc, grpspicmddec, grpRegcheck, grpbg, grpGdcst, grpLast
} alarmEnableLatchGroupIndexType;

// Enable and Latch Bit definitions
typedef enum alarmEnableType1{
	fnoAlarm=0,	fenTemp=Bit14, fenAcp=Bit13, fenVrefshld=Bit12
	, fenVref=Bit10, fenExtVdd=Bit9, fenDvdd=Bit8, fenAvdd=Bit7, fenVddmaster=Bit6, fenQuadadc=Bit4
	, fenAdcst=Bit11, fenGdcst=Bit12, fenDrvfreq=Bit8, fenDrvclk=ALLREGBITS, fenBg=Bit7, fenRegcheck=Bit6, fenAhb=Bit4
	, fenECC=ALLREGBITS, fenOtpcrc=ALLREGBITS, fenOtpcpy=ALLREGBITS, fenOtpreg=ALLREGBITS, fenspi=ALLREGBITS, fenVddio= ALLREGBITS
	, fAlwaysEnabled=ALLREGBITS
} alarmEnableType;

typedef enum alarmLatchEnType1 {
	fleGyroQuadAdc=Bit11, fleSpiClkcnt=Bit9, fleSpiCmddecod=Bit8, fleSpiCrc=Bit7, fleGyrodrfreqmeas=Bit6
	, fleVrefshldZ=Bit15, fleVrefshldXY=Bit14, fleBg=Bit11
	, fleVref=Bit10, fleExtVdd=Bit9,  fleVddio=Bit8, fleDvdd=Bit7, fleAvdd=Bit6, fleVddmaster=Bit5
	, fleTempDsp=Bit3, fleAcp=Bit1
	, fleGyroDrclk=Bit0
	, fleotpcrc=ALLREGBITS, fleotpcpy=ALLREGBITS, fleotpreg=ALLREGBITS, fleAhb=ALLREGBITS
	, fleRegchk=ALLREGBITS, fleAdcst=ALLREGBITS, fleGdcst=ALLREGBITS, fleNoLatch=ALLREGBITS
} alarmLatchEnType;

// Alarm Mask Bit Definitions
typedef enum alarmMaskType1 {
	almMskGZquadAdc=Bit13, almMskGYquadAdc=Bit12, almMskGXquadAdc=Bit11, almMskRcosc1Freqmeas=Bit10,
	almMskSysClk=Bit9, almMskGZdrfreqmeas=Bit8, almMskGYdrfreqmeas=Bit7, almMskGXdrfreqmeas=Bit6,
	almMskTempVref=Bit12, almMskAccelVref=Bit11, almMskGyroVref=Bit10, almMskVdd=Bit9, almMskVddio=Bit8,
	almMskDvdd=Bit7, almMskAvdd=Bit6, almMskVddmaster=Bit5, almMskTemp12=Bit4, almMskTempDsp=Bit3,
	almMskGZdrclk=Bit2, almMskGYdrclk=Bit1, almMskGXdrclk=Bit0, almMskCpVref=Bit15, almMskAhbBus=Bit14,
	almMskOtpCrc=Bit12, almMskOtpReg=Bit11, almMskRegEccerr=Bit7, almMskAhbEccerr=Bit6, almMskOtpEccerr=Bit5,
	almMskAccelCp=Bit2, almMskVrefshldZ=Bit1, almMskVrefshldXY=Bit0, almMskGZdcst=Bit15, almMskGYdcst=Bit14,
	almMskGXdcst=Bit13, almMskRcosc2Freqmeas=Bit6, almMskBg=Bit5, almMskRegcheck=Bit4, almMskOtpCpy=Bit3,
	almMskSpiClkcnt=Bit2, almMskSpiCmddecod=Bit1, almMskSpiCrc=Bit0, almMskPin=Bit15,
	almMskAZdcst=Bit12, almMskAYdcst=Bit11, almMskAXdcst=Bit10
} alarmMaskType;


// Alarm Status Bit Definitions
typedef enum alarmGType1 {
	  alarmGZquadadc=Bit12,  alarmGYquadadc=Bit11, alarmGXquadadc=Bit10
	, alarmGZdrfreqmeas=Bit8, alarmGYdrfreqmeas=Bit7, alarmGXdrfreqmeas=Bit6
	, alarmGZdcst=Bit14, alarmGYdcst=Bit13, alarmGXdcst=Bit12
	, alarmGVref=Bit4, alarmGZdrclk=Bit3, alarmGYdrclk=Bit2, alarmGXdrclk=Bit1,
} alarmGtype;

typedef enum alarmAType1 {
	alarmVrefshldZ=Bit13,  alarmVrefshldXY=Bit12, alarmAccelCp=Bit1
	, alarmAccelVref=Bit0, alarmAXdcst=Bit2, alarmAYdcst=Bit1, alarmAZdcst=Bit0
} alarmAtype;

typedef enum alarmOthersType1 {
	  alarmOtpCrc=Bit13,  alarmOtpReg=Bit12, alarmBg=Bit11, alarmCpVref=Bit10
	, alarmTempVref=Bit9, alarmVdd=Bit8, alarmVddio=Bit7, alarmDvdd=Bit6, alarmAvdd=Bit5
	, alarmVddmaster=Bit4, alarmTemp12=Bit3, alarmTempDsp=Bit2
	, alarmRcosc2Freqmeas=Bit1, alarmRcosc1Freqmeas=Bit0
	, alarmSysClk=Bit13, alarmOtpCpy=Bit12,  alarmRegcheck=Bit5, alarmSpiClkcnt=Bit4
	, alarmSpiCmddecod=Bit3, alarmSpiCrc=Bit2, alarmAhbbus=Bit1
} alarmOthersType;

typedef enum alarmEventType {
		  almEvntGZquadadc,  almEvntGYquadadc, almEvntGXquadadc
		, almEvntGZdrfreqmeas, almEvntGYdrfreqmeas, almEvntGXdrfreqmeas
		, almEvntGZdcst, almEvntGYdcst, almEvntGXdcst
		, almEvntGVref, almEvntGZdrclk, almEvntGYdrclk, almEvntGXdrclk
		, almEvntVrefshldZ,  almEvntVrefshldXY, almEvntAccelCp
		, almEvntAccelVref, almEvntAXdcst, almEvntAYdcst, almEvntAZdcst
		, almEvntOtpCrc,  almEvntOtpReg, almEvntBg, almEvntCpVref
		, almEvntTempVref, almEvntVdd, almEvntVddio, almEvntDvdd, almEvntAvdd
		, almEvntVddmaster, almEvntTemp12, almEvntTempDsp
		, almEvntRcosc2Freqmeas, almEvntRcosc1Freqmeas
		, almEvntSysClk, almEvntOtpCpy,  almEvntRegcheck, almEvntSpiClkcnt
		, almEvntSpiCmddecod, almEvntSpiCrc, almEvntAhbbus		
} alarmEventType1;

typedef enum alarmFlagsType {
	  stsEvntGZquadadc=Bit1,  stsEvntGYquadadc=Bit2, stsEvntGXquadadc=Bit3
	, stsEvntGZdrfreqmeas=Bit4, stsEvntGYdrfreqmeas=Bit5, stsEvntGXdrfreqmeas=Bit6
	, stsEvntGZdcst=Bit7, stsEvntGYdcst=Bit8, stsEvntGXdcst=Bit9
	, stsEvntGVref=Bit10, stsEvntGZdrclk=Bit11, stsEvntGYdrclk=Bit12, stsEvntGXdrclk=Bit13
	, stsEvntVrefshldZ=Bit14,  stsEvntVrefshldXY=Bit15, stsEvntAccelCp=Bit16
	, stsEvntAccelVref=Bit17, stsEvntAXdcst=Bit18, stsEvntAYdcst=Bit19, stsEvntAZdcst=Bit20
	, stsEvntOtpCrc=Bit21,  stsEvntOtpReg=Bit22, stsEvntBg=Bit23, stsEvntCpVref=Bit24
	, stsEvntTempVref=Bit25, stsEvntVdd=Bit26, stsEvntVddio=Bit27, stsEvntDvdd=Bit28, stsEvntAvdd=Bit29
	, stsEvntVddmaster=Bit30, stsEvntTemp12=Bit31, stsEvntTempDsp=Bit32
	, stsEvntRcosc2Freqmeas=Bit33, stsEvntRcosc1Freqmeas=Bit34
	, stsEvntSysClk=Bit35, stsEvntOtpCpy=Bit36,  stsEvntRegcheck=Bit37, stsEvntSpiClkcnt=Bit38
	, stsEvntSpiCmddecod=Bit39, stsEvntSpiCrc=Bit40, stsEvntAhbbus=Bit41	
} alarmFlagsType1;


typedef enum alarmTopStatusType { ferGX=Bit15, ferGY=Bit14, ferGZ=Bit13, ferAX=Bit12, ferAY=Bit11, ferAZ=Bit10, ferT1=Bit9, ferT2=Bit8, ferSTon=Bit4, ferAny=Bit3
	, ferGany=(ferGX|ferGY|ferGZ), ferAany=(ferAX|ferAY|ferAZ), ferTany=(ferT1|ferT2)
} alarmTopStatusType1;


typedef struct alarmEnLtchType {regfullAddrType1 enable; regfullAddrType1 latch; regfullAddrType1 mask; regfullAddrType1 status;} alarmEnLtchType1;
typedef struct alarmMskStsType {regfullAddrType1 pinMask; regfullAddrType1 almEvent;  alarmEnableLatchGroupIndexType almGrp; bool wantEnable; bool wantLatch; } alarmMskStsType1;

typedef enum eventEnableCmdType {enable, disable, keepEnState} eventEnableCmdType1;
//typedef enum eventMaskCmdType {alarmBturnOff, alarmBturnOn, keepMskState} eventEMaskCmdType1;

void getEvents();


#endif /* YAMALARM_H_ */