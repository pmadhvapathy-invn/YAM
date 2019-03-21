/*
 * protocol_functions.h
 *
 * Created: 7/14/2017 10:13:53 AM
 *  Author: TDK InvenSense
 */ 


#ifndef PROTOCOL_FUNCTIONS_H_
#define PROTOCOL_FUNCTIONS_H_

#include "string.h"

#define MAXDATABUF 16384

/**
 * Helper to create a mask
 */
#ifndef TO_MASK
#define TO_MASK(a) (1U << (unsigned)(a))
#endif
//void     processUartData(void);
typedef enum  yerrType1 {xit, yamwait, keepgoin} yErrType;
void yamMsg (char * s, yErrType next, uint32_t err);

void achievedsuccess ();
void IAM20685() ;
void IAM20685_PRM() ;
void IAM20685Original();
void IAM20685NotSoOriginal();
void boardAndChipInit( bool boardRev);

#define FXDARRINDEX 19

#endif /* PROTOCOL_FUNCTIONS_H_ */