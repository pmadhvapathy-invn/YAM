/*
 * IncFile1.h
 *
 * Created: 1/22/2019 2:27:34 PM
 *  Author: pmadhvapathy
 */ 


#ifndef LOCKUNLOCK_H_
#define LOCKUNLOCK_H_

void IAM20685_chipunlock(); //Crystal-2 chip unlock, if locked
void IAM20685_chipunlock_PRM();


void IAM20685_writeunlock();
void IAM20685_writeunlock_PRM();



#endif /* LOCKUNLOCK_H_ */