/*
 * IAMregRdWr.c
 *
 * Created: 1/15/2019 2:55:49 PM
 *  Author: pmadhvapathy
 */ 
#include <asf.h>
#include "protocol_functions.h"
#include "delay.h"
#include "IAM20685Reg.h"
#include "inv_spi.h"
#include "delay.h"
#include "crc.h"
#include "yamTests.h"
#include "arm_math.h"
#include "utils.h"

const LOWER8Of32 = 0x000000FF;
const LOWER16Of32 = 0x0000FFFF;


const DELAYrw = 10; //us

regBankType yambank;
regBitsType yamfield;   // bit field
regAddrType yamAddr;

struct regfullAttrType regArray[] = {
	{bGx, Gx, fGx, 0}, {bGy, Gy, fGy, 0}, {bGz, Gz, fGz, 0}
  , {bT1, T1, fT1, 0}
  , {bAx, Ax, fAx, 0}, {bAy, Ay, fAy, 0}, {bAz, Az, fAz, 0}
  , {bT2, T2, fT2, 0}
  , {bAxlr, Axlr, fAxlr, 0}, {bAylr, Aylr, fAylr, 0}, {bAzlr, Azlr, fAzlr, 0}
  , {bFxd, Fxd, fFxd, FIXEDYAM}
  , {bZ_flt, Z_flt, fZ_flt, 0}, {bY_flt, Y_flt, fY_flt, 0}, {bX_flt, X_flt, fX_flt, 0}, {bSt_dis, St_dis, fSt_dis, 0}, {bInorA, InorA, fInorA, 0}, {bInorG, InorG, fInorG, 0}, {bInorC, InorC, fInorC, 0}
  , {bT12, T12, fT12, 0}
  , {bGa1, Ga1, fGa1, 0}, {bGa2, Ga2, fGa2, 0}
  , {bAa1, Aa1, fAa1, 0}, {bAa2, Aa2, fAa2, 0}
  , {bOa1, Oa1, fOa1, 0}, {bOa2, Oa2, fOa2, 0}
  , {bTr, Tr, fTr, 0}
  , {bRsts, Rsts, fRsts, 0}, {bRsth, Rsth, fRsth, 0}, {bMonSt, MonSt, fMonSt, 0}
  , {bCap, Cap, fCap, 0}, {bTcd, Tcd, fTcd, 0}, {bWrlk, Wrlk, fWrlk, 0}
  , {bArev, Arev, fArev, 0}, {bGrev, Grev, fGrev, 0}, {bSrev, Srev, fSrev, 0}, {bSerIdmsb, SerIdmsb, fSerIdmsb, 0}, {bSerIdlsb, SerIdlsb, fSerIdlsb, 0} 
  , {bBank, Bank, fBank, 0}
	/* Bank 1 */
  , {bSMen1, SMen1, fSMen1, 0xFFFF}, {bSMen2, SMen2, fSMen2, 0xFFFF}
  , {bSMmsk1, SMmsk1, fSMmsk1, 0}, {bSMmsk2, SMmsk2, fSMmsk2, 0}, {bSMmsk3, SMmsk3, fSMmsk3, 0}
																, {bSMmsk4, SMmsk4, fSMmsk4, 0}, {bSMmsk5, SMmsk5, fSMmsk5, 0}, {bSMmsk6, SMmsk6, fSMmsk6, 0}
  , {bSMle1, SMle1, fSMle1, 0}, {bSMle2, SMle2, fSMle2, 0}
  , {bSMs, SMs, fSMs, 0}
	/* Bank 2 */
  , {bSMra, SMra, fSMra, 0}, {bSMrg, SMrg, fSMrg, 0}
	/* Bank 6 */
  , {bFSa, FSa, fFSa, 1}
	/* Bank 7 */
  , {bFSg, FSg, fFSg, 5}

};

uint16_t numRegArray()
{
	int i = sizeof(regArray)/sizeof(regArray[0]);
	return i;	
}

/*
static  regdefaultType regDefaults[] =	{
											{0, Fxd, 0xAA55},
											{1, SMen1, 0xFFFF}, {1, SMen2, 0xFFFF},
											{6, FSa, 1},   {7, FSg, 5}
										}; */

static const  writeBitmask = 0x80000000;  
//**********************************
int16_t yamRead(regBankType b, regAddrType a)
{
	uint16_t v;
	uint8_t value[3];
   {
	  uint32_t temp;
      temp = (uint32_t) ((a<<2) |  READ_BIT_MASK );
	  temp <<= 16;
	  uint8_t crc = IAM20685_crc_calculatefast(temp);
      value[0] = crc;
   }
	
//	spi_master_read_register(a<<2, 2, value);
	spi_master_read_register(a<<2, 3, value);
	
	v= value[0]<<8 | value[1];
	
	delay_us(DELAYrw);
	return v;
	
}
//**********************************
void LKUNLKWrite(regAddrType a, uint32_t v)
{
	uint32_t writeVal;
	//uint8_t crc;
	uint8_t value[3];

	writeVal = v;
	
	value[0] = (writeVal>>16) & LOWER8Of32;
	value[1] = (writeVal>>8) & LOWER8Of32;
	value[2] = writeVal & LOWER8Of32;
	
    spi_master_write_register((a), 3, value); // to do err checking
	   
	delay_us(DELAYrw);
}

// if preservebits == false, *a=(v*f)  else, do a rmw
// if f == 0, only write the cmd/addr
Bool yamWrite(regBankType b, regAddrType a, uint32_t v, uint32_t f, bitpreserveType preservebits)
{
	bool err=false;
	uint32_t writeVal;
	uint32_t readVal;
	uint8_t crc;
	uint8_t value[3];

	writeVal = v & f;

	if (preservebits == preserve) {
//		readVal=yamRead(b, a);
//		readVal &= ~f;
//		writeVal |= readVal;
	}
	
	  
	//spi_master_read_register(a<<2, 0, value);
	
	writeVal = (((uint32_t)a)<<18 | writeVal) & ~(writeBitmask>>8);
	crc = IAM20685_crc_calculate1(writeVal);
	writeVal = (writeVal <<8) | (((uint32_t) crc) & LOWER8Of32);
	value[0] = (writeVal>>16) & LOWER8Of32;
	value[1] = (writeVal>>8) & LOWER8Of32;
	value[2] = writeVal & LOWER8Of32;
	if(value[2] == 0)
	  printf("CRC = 0 \r\n");
    spi_master_write_register((a<<2), 3, value); // to do err checking
	   
	delay_us(DELAYrw);
	return err;
}
//**********************************
Bool yamWriteCmdAddr(regBankType b, regAddrType a)
{
   	    spi_master_set_read_register_cmd(a<<2)	;
       // delay_us(DELAYrw);
}


uint32_t appendcrcRegaddrData(regAddrType a, uint16_t v)
{
	uint8_t crc;
	uint32_t temp, protocolValue;
	
	temp = (uint32_t)  (  a<<16 | (v&LOWER16Of32));
 	crc = IAM20685_crc_calculatefast(temp);
	protocolValue =  temp<<8 | ((uint32_t) crc & LOWER8Of32);
    return protocolValue;
}

char tbuf[100];
#define PROTOSTATUSMASK (uint32_t) (Bit24|Bit25)
#define PROTOSTATUSERR  (uint32_t) (Bit24|Bit25)
#define PROTOSTATUSWIP  (uint32_t) (      Bit25)
#define PROTOSTATUSSUCC (uint32_t) (Bit24)
#define PROTOSTATUSRSV  (uint32_t) (0)


// cmd response logs
uint32_t lastCmdsent = 0xFFFFFFFFL;
uint32_t lastRespReceived = 0xFFFFFFFFL;
void getlastcmdresp( uint32_t * cmd, uint32_t * resp)
{
	*resp = lastRespReceived;
	*cmd  = lastCmdsent;
}

#if 1 //LOGCMDRESP
#define CMDRESPHISTDEPTH 16

typedef struct protocolHistType { uint32_t cmd; uint32_t rsp ; } protocolHistType1;
static struct protocolHistType protocolHist[CMDRESPHISTDEPTH+1] = {{0, 0}, };
static uint32_t CMDRESPindex=CMDRESPHISTDEPTH;

//static uint32_t cmdhist[CMDRESPHISTDEPTH+2] = {0,};
//static uint32_t rsphist[CMDRESPHISTDEPTH+2] = {0,};



void logCmdRespHist(uint32_t cmd, uint32_t rsp)
{
	uint32_t * buffp;
	
	uint32_t i = protocolHist[CMDRESPindex].cmd;  
	uint32_t d = protocolHist[CMDRESPindex].rsp;
	
	protocolHist[i].cmd = cmd;
	protocolHist[i].rsp = rsp;	
	i++;
	i =  i == CMDRESPHISTDEPTH ? 0: i;	
	protocolHist[CMDRESPindex].cmd = i;

	d = d == CMDRESPHISTDEPTH ? CMDRESPHISTDEPTH : d+1;
	protocolHist[CMDRESPindex].rsp = d;

}




static char sts[5][5] = {"Rsvd", "Succ", "WIP ", "ERR ", " OK "};

void parseprintproto (uint32_t proto, uint32_t iter, bool Cmd_resp, bool eol)
{
	char rw;
	uint8_t adr;
	uint16_t data;
	uint8_t crc;
	uint32_t stsval;
	
	stsval = (proto >> 24) & 3;
	if (Cmd_resp && stsval == 0)  stsval=4;  // Succ instead of Rsvd for Cmd.
	
	rw = proto & Bit31 ? 'W' : 'R';		  	
	adr = (proto >> 26) & 0x0000001F;		  	
	data = (proto >> 8) & LOWER16Of32;		  	
	crc = proto & LOWER8Of32;
		  	
	if (!eol) { printf("%03d", iter);   printf(", "); }
	if (Cmd_resp) { printf("%08X", proto); printf(", "); }
	printf("%s", sts[stsval]); printf(", ");
	printf("%2c", rw);  printf(", ");
	printf("%02X", adr);  printf(", ");
	printf("%04X", data);  printf(", ");
	printf("%02X", crc);  printf(", ");
    if (!Cmd_resp) { printf("%08X", proto); printf(", "); }
		//else printf("|");

	if (eol) printf("\r\n");

	
}
void printCmdRespHistory ()
{
  uint32_t i, ibuf, dpth;
  uint32_t cmd, rsp;
  
  dpth = protocolHist[CMDRESPindex].rsp;

  printf ("Printing %d items: \r\n", dpth);
  printf ("Indx, CMDWord, STS, RW, Adr, Data, CRC,STS,  RW, Adr,Data,CRC, RespWord\r\n");
  
  ibuf = protocolHist[CMDRESPindex].cmd; // print backwards
  for (i=0; i<dpth; i++)
  {
	  ibuf = ibuf == 0 ? CMDRESPHISTDEPTH-1 : ibuf-1;	  
	  cmd = protocolHist[ibuf].cmd;
	  rsp = protocolHist[ibuf].rsp;	  

      parseprintproto (cmd, i, true, false);
      parseprintproto (rsp, i, false, true);	    
  }
  	
}
#endif

static bool enableLogCmdRespHist = false;
void enableProtocolHistory(bool En_dis) { enableLogCmdRespHist = En_dis; }
bool isEnabledCmdRespHist () { return enableLogCmdRespHist; }
void logprotocmdresp(uint32_t cmd, uint32_t resp)    {lastCmdsent	= cmd; lastRespReceived = resp;    if(isEnabledCmdRespHist()) logCmdRespHist(cmd, resp);  }
//void logprotoresp( uint32_t resp) {lastRespReceived = resp;	   if(isEnabledCmdRespHist()) logCmdRespHist(resp, rsphist); }

// protocol handling
void reportProtErr(enum errWriteType e)
{
    uint32_t c, r;
	bool errhist = true;
	getlastcmdresp(&c, &r);  
	switch (e)
	{  
	   case noProtocolErr:  sprintf(tbuf, "No Protocol Error, handler should not have been called ");  errhist=false;
	   break;
	   case stsProtoWIP: sprintf(tbuf, "YAM read/Write in progress");
	   break;
	   case crcProtoErr:  sprintf(tbuf, "CRC Error, last command %08x, last response %08x", c, r);
	   break;
	   case stsProtoRsvd: sprintf(tbuf, "YAM protocol Status Reserved, last command %08x, last response %08x", c, r);
	   break;
	   case stsProtoErr: sprintf(tbuf, "YAM protocol Status Error,  last command %08x, last response %08x", c, r);
	   break;
	   default:
	   break;
	}	
	
	//{ uint16_t temp;  getWhoAmI(&temp); }
	if (errhist) printCmdRespHistory(); else yamMsg(tbuf, keepgoin, e);

	
}

#define CHECK_CRC 1
enum errWriteType checkProtocolErrors( uint32_t proto)
{
	enum errWriteType err = noProtocolErr;
	uint8_t crcRx, crcCalc;
	uint32_t  c, r;
	uint32_t rxerr;
	
	getlastcmdresp(&c, &r);

	rxerr = proto & PROTOSTATUSMASK;
	switch (rxerr)
	{
	  case PROTOSTATUSWIP:  err=stsProtoWIP;   break;		 
	  case PROTOSTATUSERR:  err=stsProtoErr;   break;
	  case PROTOSTATUSRSV:  err=stsProtoRsvd;  break;
	  case PROTOSTATUSSUCC:  err=noProtocolErr;  break;
	  default:				err=noProtocolErr; break;
	}

	#if (CHECK_CRC == 1)
	uint32_t addrdata=0;
	addrdata =  (proto >> 8) & 0x00FFFFFFL;
	crcCalc = IAM20685_crc_calculatefast (addrdata);
	crcRx = proto & LOWER8Of32;
	if (crcRx != crcCalc) {
	    sprintf(tbuf, "Yam sends bad CRC. {lastCmd sent, Rx response} = { %08X, %08x }, {RxCRC, calccrc} = %02X, %02x", c, proto, crcRx, crcCalc);
		err=crcProtoErr;
	    yamMsg(tbuf, yamwait, err);
	}
	#endif
	if (err) 
	  reportProtErr(err);
	return err;
}

uint32_t assembleWord(uint16_t *inp)
{
	int i, shift;
	uint32_t v;
	for (i=0, v=0; i<4; i++) {
		shift=8*(3-i);
		v |=  ((uint32_t)inp[i] << shift) & (LOWER8Of32<<shift)  ;
	}
	return v;
}

void Send4Rcv4bytes (uint16_t * inp, uint16_t *outp)
{
	int i;
	uint8_t uc_pcs=0;
	for (i=0; i<4; i++) {
		spi_write(SPI_MASTER_BASE, inp[i], 0, 0); /* write to generate clock: but protocol needs good addrss and crc */
		while ((spi_read_status(SPI_MASTER_BASE) & SPI_SR_TDRE) == 0); /* Wait transfer data reg done. */
		spi_read(SPI_MASTER_BASE, &outp[i], &uc_pcs); /* read actual register data */
	}

}

void word2byteArr(uint32_t inVal, uint16_t *outp)
{
	outp[0] = inVal >> 24 & LOWER8Of32;
	outp[1] = inVal >> 16 & LOWER8Of32;
	outp[2] = inVal >>  8 & LOWER8Of32;
	outp[3] = inVal >>  0 & LOWER8Of32;
}


enum errWriteType sendaddrdata(regAddrType areg, uint16_t v,  uint32_t *readBack)
{
	enum errWriteType err= noProtocolErr;
	uint32_t writeVal, readVal;
	uint16_t wrarr[4];
	uint16_t rdarr[4];
	
	writeVal = appendcrcRegaddrData(areg, v);
	word2byteArr(writeVal, wrarr);
	Send4Rcv4bytes (wrarr, rdarr);
	readVal = assembleWord(rdarr);
    logprotocmdresp(writeVal, readVal);

	
	err = checkProtocolErrors(readVal);

	
	*readBack = readVal;
	delay_us(DELAYrw);

    return err;
}
//*****************
enum errWriteType WriteReg(regAddrType a, uint16_t v)
{
	enum errWriteType err = noProtocolErr;
	uint32_t writeVal;
    regAddrType aw, ar;
	uint32_t readval;
	int i;
	
	aw = (a<<2) | READ_BIT_MASK;
	err = sendaddrdata(aw, v,  &readval);
	return err;
}

//**********************
uint32_t ReadReg(regAddrType a, enum errWriteType *e)
{
	enum errWriteType err = noProtocolErr;
	uint32_t writeVal = 0, readval, dataRed;
	uint32_t aw, ar;
   
    // write address first
    aw= (a<<2) & WRITE_BIT_MASK;
	err = sendaddrdata(aw, 0,  &readval);
 
	// Now issue read command and collect the data 
	ar =  aw; //(a<<2) | READ_BIT_MASK;
	err = sendaddrdata(ar, 0,  &readval);
	*e = err;
	
	dataRed = (readval >> 8) & LOWER16Of32;
    return dataRed;
}

//**********************************
int16_t readReg(regBankType b, regAddrType a)
{
	int16_t result;
	
	//spi_master_read_register(a<<2, 0, &result);
	//delay_us(DELAYrw);
	// select bank 
	//bankselect(b);
	yamWrite(0, a, 0, ALLREGBITS, overwrite);
	result = yamRead(0, a);

    return result;
}
uint32_t keepvars = 0;
//**********************************
Bool oldSplitReadModifyWrite(regAddrType a, uint32_t v, uint16_t mask)
{
	bool		err=false;
	uint32_t	writeVal;
	uint32_t	readVal;
	uint8_t		crc;
	uint8_t		value[3];

    readVal = readReg(0, a);
	readVal &= ~mask;
	writeVal =  v&mask;
	writeVal |= readVal;
//printf(" Address %d , readVal: %04X, mask: %04X, v: %04X, writeVal: %04X \r\n", a, readVal, mask, v, writeVal);  
	writeVal = (a<<18 | (writeVal&LOWER16Of32)) | (writeBitmask>>8);
	// printf(" Before crc .. writeVal: %04X \r\n", writeVal)	;

	crc = IAM20685_crc_calculatefast(writeVal);
	writeVal = (writeVal << 8) | (((uint32_t)crc & LOWER8Of32));
   //printf(" After crc .. writeVal: %04X \r\n", writeVal); getchar();
//keepvars = writeVal;
	value[1] = (uint8_t) (writeVal & 0x000000FF);
	value[0] = (uint8_t) ((writeVal>>8) & 0x000000FF);
	value[2] = (uint8_t) ((writeVal>>16) & 0x000000FF);
	yamWriteCmdAddr(0, a);

	spi_master_write_register(value[2], 2, value); // to do err checking

	delay_us(DELAYrw);
	return err;
}
	
//**********************************
Bool oldsplitWrite(regAddrType a, uint32_t v)
{
	bool		err=false;
	uint32_t	writeVal;
	//uint32_t	readVal;
	uint8_t		crc;
	uint8_t		value[3];


	writeVal = (a<<18 | (v&LOWER8Of32)) | (writeBitmask>>8);
	crc = IAM20685_crc_calculatefast(writeVal);
    writeVal = (writeVal << 8) | (((uint32_t)crc & LOWER8Of32));
	
	value[1] = (uint8_t) (writeVal & 0x000000FF);
	value[0] = (uint8_t) ((writeVal>>8) & 0x000000FF);

	  	
    yamWriteCmdAddr(0, a);

	spi_master_write_register(0, 2, value); // to do err checking
	
/*

    spi_master_write_register(a<<2, 2, value); // to do err checking
*/
	delay_us(DELAYrw);
	return err;
}

uint32_t gtemp;
//**********************************
Bool indirectWrite(regAddrType a, uint32_t v)
{
	bool		err=false;
	uint32_t	writeVal;

	uint8_t		crc;
	uint8_t		value[3];

 	writeVal = (a<<18 | (v&LOWER16Of32)) | (writeBitmask>>8);
 	crc = IAM20685_crc_calculatefast(writeVal);
 	writeVal = (writeVal << 8) | (((uint32_t)crc & LOWER8Of32));
 	
 	value[2] = (uint8_t)((writeVal>>16) & 0x000000FF);
 	value[0] = (uint8_t) ((writeVal>>8) & 0x000000FF);
 	value[1] = (uint8_t) (writeVal & 0x000000FF);
	 
	yamWriteCmdAddr(0, a);

	spi_master_write_register(value[2], 2, value); // to do err checking
	
	gtemp = writeVal;  // to prevent being optimized-out

	delay_us(DELAYrw);
	return err;
}


Bool oldregWrite(regAddrType a, uint32_t v)
{
	bool		err=false;
	uint32_t	writeVal;

	uint8_t		crc;
	uint8_t		value[3];
    uint32_t    addr = (a<<2) & 0x7F;  //! hard code :( 
	
 	writeVal = (addr<<16 | (v&LOWER16Of32)) | (writeBitmask>>8);
 	crc = IAM20685_crc_calculatefast(writeVal);
 	writeVal = (writeVal << 8) | (((uint32_t)crc & LOWER8Of32));
 	
 	value[2] = (uint8_t)((v>>8) & 0x000000FF);
 	value[0] = (uint8_t) ((v) & 0x000000FF);
 	value[1] = (uint8_t) (crc & 0x000000FF);
	 
	yamWriteCmdAddr(0, a);

	spi_master_write_register(value[2], 2, value); // to do err checking
	
	gtemp = writeVal;  // to prevent being optimized-out

	delay_us(DELAYrw);
	return err;
}

//**********************************
static void tcodeDontdoNothin()
{
	

	
	//	v = readReg(0, Tcd);
	//	printf("Tcd at start: %x \r\n", v);
	//	splitWrite(Tcd, 1);
	//	v = readReg(0, Tcd);
	//	printf("Tcd after 2: %x \r\n", v);
	//	splitWrite(Tcd, 2);
	//	v = readReg(0, Tcd);
	//	printf("Tcd after 1: %x \r\n", v);
	//	splitWrite(Tcd, 4);
	//	v = readReg(0, Tcd);
	//	printf("Tcd after 4: %x \r\n", v);
}

regBankType oldBank = 0;
//**********************************
void bankselect(regBankType banknum)
{
	enum errWriteType err=noProtocolErr;
	//uint16_t v;
	       //to do:  send tcode for new chips
	err = WriteReg(Bank, banknum);
}

//******** return current bank selected
uint16_t currentBank()
{
	enum errWriteType err;
	uint16_t cb = ReadReg(Bank, &err);
	return cb;
}


//*** bitmask with hard and soft reset OR-ed
void resetChip( chipResetType r)
{
	uint16_t v=0;
	if (r&HARD_RESET)  v|= fRsth;
	if (r&SOFT_RESET)  v|= fRsts;
	
	WriteReg(Rsts, v);    // soft and hard reset share same register
}

void selfteststart()
{
	uint16_t v=fMonSt;
	WriteReg(MonSt, v);  // soft and hard reset share same register
}


void selftestDisable(bool Dis_en)
{
	uint16_t selfTestRegVal;
	enum errWriteType err=noProtocolErr;
	
	selfTestRegVal = ReadReg(St_dis, &err);
	printf(" ST Disable Before: press any key %04X \r\n", selfTestRegVal); 
	if (Dis_en)
	  selfTestRegVal |=  fSt_dis;
	else
	  selfTestRegVal &= ~fSt_dis;
	    
	WriteReg(St_dis, selfTestRegVal);
	selfTestRegVal = ReadReg(St_dis, &err);
	printf(" ST Disable After: press any key %04X \r\n", selfTestRegVal); getchar();

}
