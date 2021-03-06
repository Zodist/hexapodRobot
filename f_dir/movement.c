#include "SetMacro.h"
#include "RoboPiLib.h"
#include <stdio.h>

//setting GROUP
//const int axisA[3] = {L1_1, R2_1, L3_1};
//const int axisB[3] = {R1_1, L2_1, R3_1};
const int axisA[3] = {L3_1, R2_1, L1_1};
const int axisB[3] = {R3_1, L2_1, R1_1};
const int hipA[3] = {L1_2, R2_2, L3_2};
const int hipB[3] = {R1_2, L2_2, R3_2};
const int legA[3] = {L1_3, R2_3, L3_3};
const int legB[3] = {R1_3, L2_3, R3_3};
/*
int position2[19] = {
	1500, 1200, 1200,
	1000, 1350, 1400,
	700, 1200, 1500,
	1200, 1150, 1100,
	1800, 1000, 1000,
	2200, 0 ,1100, 700
	};
int position[19] = {
	1500, 1200, 1200,
	1000, 1350, 1400,
	700, 1200, 1500,
	1200, 1150, 1100,
	1800, 1000, 1000,
	2200, 0 ,1100, 700
	};
*/
int position2[19] = {
	1500, 1200, 1300,
	1000, 1400, 1400,
	700, 1200, 1500,
	1200, 950, 1100,
	//1200, 1150, 1100,
	1800, 900, 850,
	2200, 0 ,1100, 800
	};
int position[19] = {
	1500, 1200, 1300,
	1000, 1400, 1400,
	700, 1200, 1500,
	1200, 950, 1100,
	//1200, 1150, 1100,
	1800, 900, 850,
	2200, 0 ,1100, 800
	};

const int isReversed[] = {
	1, 1, 0,
	1, 1, 0,
	1, 1, 0,
	0, 0, 1,
	0, 0, 1,
	0, 0, 0, 1
	};
const int axis = 300;
const int hips = 500; // default 500
//legmove 
/*Good move
const int t1 = 80;
const int t2 = 200;
const int t3 = -80;
const int t4 = -200;
*/
const int t1 = 40;//40
const int t2 = 200;
const int t3 = 70;//-40
const int t4 = -200;//-200

const int servoterm = 190000;

int startmove = 0;

void initializePosi();
void stablePosi();

int getStatus(int index);
void setStatus(int index, int posi);

void LegMove(int hip[], int leg[], int index, int size, int hipd, int legd);
void MoveAxisUp(int IDArray[], int size, int incrPos);
void MoveUpHipsForReady(int hip[], int leg[], int size);
void MoveUp(int IDArray[], int size, int incrPos);
void MoveDown(int IDArray[], int size, int decrPos);
void moveForward();
	
void CMoveUp(int IDArray[], int size, int incrPos);
void CMoveDown(int IDArray[], int size, int incrPos);
void changeDir();

void downHips(int deg1, int deg2);
void testServo();

void downHips(int deg1, int deg2){
	int i;
	for(i=1;i<=7;i+=3){
		position[i] = position2[i]-deg1;
		servoWrite(i, position[i]);
		usleep(servoterm);

		position[i+1] = position2[i+1]+deg2;
		servoWrite(i+1, position[i+1]);
		usleep(servoterm);

	}
	for(i=10;i<=15;i+=3){
		position[i] = position2[i]+deg1;
		servoWrite(i, position[i]);
		usleep(servoterm);

		position[i+1] = position2[i+1]-deg2;
		servoWrite(i+1, position[i+1]);
		usleep(servoterm);
	}
	position[17] = position2[17]+deg1;
	servoWrite(17, position[17]);
	usleep(servoterm);

	position[18] = position2[18]-deg2;
	servoWrite(18, position[18]);
	usleep(servoterm);
}

void testServo(){
	int i,pin;
	while(1){
		printf("Input pin,degree :");
		scanf("%d %d",&pin,&i);
		if(pin == 2000) break;
		servoWrite(pin, i);
	}
}

void CMoveUp(int IDArray[], int size, int incrPos){
	int i;
	for(i = 0; i < size; i++){
		int index = IDArray[i]; 
		int pos;
		if(1){
			if(index==7){
				pos = getStatus(index) - incrPos;// -100;
			}else
				pos = getStatus(index) - incrPos;
		}else{
				pos = getStatus(index) + incrPos;
		}
		servoWrite(index, pos);
		setStatus(index, pos);
	}
	usleep(servoterm);
}
void CMoveDown(int IDArray[], int size, int decrPos){
	int i;
	for(i = 0; i < size; i++){
		int index = IDArray[i];
		int pos;
		if(1){
			if(index==7){
				pos = getStatus(index) + decrPos;// +100;
			}else
				pos = getStatus(index) + decrPos;
		}else{
			pos = getStatus(index) - decrPos;
		}
		servoWrite(index, pos);
		setStatus(index, pos);
	}
	usleep(servoterm);
}

void changeDir(int dir){
	const int trunTime = 200000;
	const int turnAxis = 250;
	// dir == 1 : Turn Right
	if(dir){
		MoveUp(hipB, 3, hips); 		//hips group B move up 100
		CMoveUp(axisA, 3, turnAxis);	// axis group B move forward 60
		usleep(trunTime);
		MoveDown(hipB, 3, hips);		//hips group B move down 100
		CMoveDown(axisA, 3, turnAxis);	// axis group B move forward 60
		usleep(trunTime);
		MoveUp(hipA, 3, hips);		//hips group A move up 100
		CMoveUp(axisB, 3, turnAxis);		// axis group A move backward 60
		usleep(trunTime);
		MoveDown(hipA, 3, hips);		//hips group A move up 100
		CMoveDown(axisB, 3, turnAxis);		// axis group A move backward 60
		usleep(trunTime);
	}else{
	// dir == 0 : Turn Left
		MoveUp(hipB, 3, hips); 		//hips group B move up 100
		CMoveDown(axisA, 3, turnAxis);	// axis group B move forward 60
		usleep(trunTime);
		MoveDown(hipB, 3, hips);		//hips group B move down 100
		CMoveUp(axisA, 3, turnAxis);	// axis group B move forward 60		
		usleep(trunTime);
		MoveUp(hipA, 3, hips);		//hips group A move up 100
		CMoveDown(axisB, 3, turnAxis);		// axis group A move backward 60
		usleep(trunTime);
		MoveDown(hipA, 3, hips);		//hips group A move up 100
		CMoveUp(axisB, 3, turnAxis);		// axis group A move backward 60
		usleep(trunTime);
	}
}
void LegMove(int hip[], int leg[], int index, int size, int hipd, int legd){
	int i = index;
	int hipidx = hip[i];
	int legidx = leg[i];
	int pos;
	int legtime = 100000;
	if(isReversed[hipidx]){
		if(hipidx==7){
			pos = getStatus(hipidx) + hipd;// + 100;
		}else{
			pos = getStatus(hipidx) + hipd;
		}
	}else{
		pos = getStatus(hipidx) - hipd;
	}
	servoWrite(hipidx, pos);
	setStatus(hipidx, pos);
	usleep(legtime);
	if(isReversed[legidx]){
		pos = getStatus(legidx) - legd;
	}else{
		pos = getStatus(legidx) + legd;
	}
	servoWrite(legidx, pos);
	setStatus(legidx, pos);
	usleep(legtime);
}

void MoveAxisUp(int IDArray[], int size, int incrPos){
	int i;
	for(i = 0; i < size; i++){
		//if((IDArray[i]==L3_1 || IDArray[i]==R3_1) && i==2) incrPos -= 250;
		//if((IDArray[i]==L1_1 || IDArray[i]==R1_1) && i==0) incrPos -= 250;
		int index = IDArray[i];
		int pos;
		if(isReversed[index]){
				pos = getStatus(index) - incrPos;
		}else{
				pos = getStatus(index) + incrPos;
		}
		servoWrite(index, pos);
		switch(index){
		case R1_1:LegMove(hipB, legB, 0, 3, -t1, -t2);	break;//Pull
		//case R2_1:LegMove(hipB, legB, 1, 3, +200, +100);	break;//Push
		case R3_1:LegMove(hipB, legB, 2, 3, -t3, -t4);	break;//Push
		case L1_1:LegMove(hipA, legA, 0, 3, -t1, -t2);	break;//Pull
		//case L2_1:LegMove(hipA, legA, 1, 3, +200, +100);	break;//Pull
		case L3_1:LegMove(hipA, legA, 2, 3, -t3, -t4);	break;//Push
		default: break;
		}
		//servoWrite(index, pos);
		setStatus(index, pos);
	}
}
void MoveUpHipsForReady(int hip[], int leg[], int size){
	LegMove(hip, leg, 0, size, -hips+t1, t2);	//Push
	//LegMove(hip, leg, 1, size, -hips-200, -100);		//Push
	LegMove(hip, leg, 1, size, -hips, 0);		//Push
	LegMove(hip, leg, 2, size, -hips+t3, t4);	//Push
	//LegMove(hip, leg, 2, size, -hips, 0);	//Push
}

void moveForward(){
	{// Ready for first step
		MoveUpHipsForReady(hipB, legB, 3);
		MoveDown(axisB, 3, axis);				//axis group B move forward 60
		MoveDown(hipB, 3, hips);				//hips group B move down 100
	}
	{
		MoveUpHipsForReady(hipA, legA, 3);		// Ready for second step
		// Move 1 step forward 
		MoveAxisUp(axisB, 3, axis);				//axis group B move backward 60
		//usleep(80000);
		MoveDown(axisA, 3, axis);				//axis group A move forward 60
	}
	{
		MoveDown(hipA, 3, hips);				//hips group A move down 100
		usleep(100000);
		MoveUp(hipB, 3, hips);					//hips group A move up 100
		usleep(80000);
		// Move 2 step forward
		MoveAxisUp(axisA, 3, axis);				//axis group B move backward 60
	}
	usleep(80000);
	{// End walking
		MoveDown(hipB, 3, hips);
	}
	usleep(100000);
	printf("One Cycle Forward\n");
}


void initializePosi(){
	int init;
	int i;
	printf("Do you want to reset position? : (0:no, 1:yes)");
	scanf("%d",&init);
	if(init == 1){
		for(i=0;i<19;i++){
			servoWrite(i, position[i]);
			usleep(servoterm);
		}
		downHips(200,-200);
		stablePosi();
	}
	printf("DONE ===== robot position initialization \n");
}

void stablePosi(){
	int i;
	for(i=0;i<18;i+=3){
		if(i<=6){
			servoWrite(i+1, position[i+1]-500);
			usleep(servoterm);
			servoWrite(i+2, position[i+2]+300);
			usleep(servoterm);
		}else{
			servoWrite(i+1, position[i+1]+500);
			usleep(servoterm);
			servoWrite(i+2, position[i+2]+300);
			usleep(servoterm);
		}
			servoWrite(i+2, position[i+2]);
			usleep(servoterm);
			servoWrite(i+1, position[i+1]);
			usleep(servoterm);
	}
}

int getStatus(int index){
	return position[index];
}

void setStatus(int index, int posi){
	position[index] = posi;
}

void MoveUp(int IDArray[], int size, int incrPos){
	int i;
	for(i = 0; i < size; i++){
		int index = IDArray[i]; 
		int pos;
		if(isReversed[index]){
			if(index==7){
				pos = getStatus(index) - incrPos;// -100;
			}else
				pos = getStatus(index) - incrPos;
		}else{
				pos = getStatus(index) + incrPos;
		}
		servoWrite(index, pos);
		setStatus(index, pos);	
	}
	usleep(servoterm);
}
void MoveDown(int IDArray[], int size, int decrPos){
	int i;
	for(i = 0; i < size; i++){
		//if((IDArray[i]==L3_1 || IDArray[i]==R3_1) && i==2) decrPos -= 250;
		//if((IDArray[i]==L1_1 || IDArray[i]==R1_1) && i==0) decrPos -= 250;
		int index = IDArray[i];
		int pos;
		if(isReversed[index]){
			if(index==7){
				pos = getStatus(index) + decrPos;// +100;
			}else
				pos = getStatus(index) + decrPos;
		}else{
			pos = getStatus(index) - decrPos;
		}
		servoWrite(index, pos);
		setStatus(index, pos);
	}
	usleep(servoterm);
}
