/*
 * IncFile1.h
 *
 * Created: 2/5/2019 2:37:01 PM
 *  Author: pmadhvapathy
 */ 


#ifndef YAMAPI_H_
#define YAMAPI_H_

typedef enum AccelFsrtype1 {afsr2=0, afsr4, afsr16, afsr32} AccelFsrtype;
typedef enum AccelFlRestype1 {flr4=0, flr8, flr16, flr32, flr64, flrdontcare} AccellRestype;
typedef enum gyroFSRtype1 {gfsr491=0, gfsr983, gfsr1966, gfsr2949, gfsr328, gfsr655, gfsr1311, gfsr1966_1, gfsr92, gfsr184, gfsr369, gfsr737, gfsr61, gfsr123, gfsr246, gfsr491_1} GyroFSRtype;

typedef enum Axistype1 {X, Y, Z} AxisType;
typedef enum gyrofilterBWType1 {g10hz=0, g12_5hz, g27hz, g30hz, g46hz, g60hz} gyrofilterBWType;
typedef enum accelfilterBWType1 {a10hz=0, a46hz, a60hz, a250hz, a300hz, a400hz} accelfilterBWType;
typedef enum accelfiltersAdditionalType1 { a10ahz, a10bhz, a60ahz, a60bhz, a60chz, a60dhz, a60ehz } accelfiltersAdditionalType ;  
typedef struct filterInfoType {uint16_t filterIndex; float BW; char * str; }filterInfoType1;
	
void AccelFSRset (AccelFsrtype afsr, AccellRestype alrfsr);
void GyroFSRset(GyroFSRtype fsr );
void getFSRval(uint16_t *afsrp,  uint16_t *alrfsrp, uint16_t *gfsrp);
void SetFilter (AxisType axis,  gyrofilterBWType gyroflt, accelfilterBWType accelflt);

void ReadAccel(int16_t *p);
void ReadAxesAndTemp(int16_t *accelp, int16_t *accelLowresp, int16_t *gyrop, int16_t *temperaturep, int16_t *anythingelsep);
void ReadTempSensors(int16_t *readArr);
void ReadAccelLowRes(int16_t * p);


void getCurrFilter(struct filterInfoType * gfp, struct filterInfoType *afp, struct filterInfoType *afstr);


void ReadID(uint8_t *gyroRevp, uint8_t *accelRevp, uint8_t *sensorRevp, uint32_t *serialIDp);






#endif /* YAMAPI_H_ */