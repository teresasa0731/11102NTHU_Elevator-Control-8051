#include "delay.h"
#include "oled_i2c.h"
#include "8051.h"
#include "i2c.h"
#include "adc.h"
#include "param.h"

// #define SCL   P2_6
// #define SDA   P2_5
// #define CLK  P2_0           //MCP3202串列時鐘  
// #define DIN  P2_1                
// #define DOUT P2_2          
// #define LOAD P2_3           //MCP3202片選
// #define LOAD_MAX P2_4       //MAX7219片選

#define PB1 P0_0
#define PB2 P0_1
#define PB3 P0_2
#define PB4 P0_3
#define HOLD P0_4
#define Hall_In P3_2


#define MOVE_Trigger P1_7
#define MOTOR1 P1_0
#define MOTOR2 P1_1

//flag
char ARRIVE,ACTIVE;
unsigned int Hall_cnt = 0;
unsigned int motorMode = 0,step = 0;



//hall sensor interrupt
void INT0_isr(void) __interrupt (0)
{
		if(motorMode){
			Hall_cnt ++;
			if(Hall_cnt == step){
				ARRIVE = 1;
				MOTOR1 = 0;
				MOTOR2 = 0;
			}
	}

}

unsigned int distanceDetect(void){
	unsigned int adc_value,distance;
	P1_4 = 0;
	delay_ms(500);
	adc_value =  read_adc3202(1);
    distance = 100000/(0.2554*adc_value-40.9);
	distance = distance % 1000 / 100;
	P1_4 = 1;
	if(distance < 3) return (1); 
	else if (distance >= 6) return (3);
	else return (2);
}



void main(void)
{
	unsigned int pre_pb1 = 0,pre_pb2 = 0,pre_pb3 = 0,pre_pb4 = 0,pre_hold = 0; 
	unsigned int targetFloor = 0,currentFloor = 1,idx;
	

	// Initialization INT0
	EA = 1;														
	EX0 = 1;
	TCON = 0x01;

	DOUT = 1;
  	LOAD = 1; 
  	CLK = 0;
  	DIN = 0;


	// Initialization OLED
	SDA = 1;
    SCL = 1;
    OLED_Init();
    
    OLED_SetCursor(0,0);
	OLED_DisplayString("**Elevator Control**");	
	OLED_SetCursor(2,10);
	OLED_DisplayString("Current floor : 1 F");	
	OLED_SetCursor(4,10);
	OLED_DisplayString("Target  floor :   F");	
	OLED_SetCursor(6,10);
	OLED_DisplayString("Status :");	
	currentFloor = distanceDetect();
	OLED_SetCursor(2,106);
	OLED_DisplayChar(currentFloor + '0');


	MOTOR1 = MOTOR2 = 0;
	ACTIVE = 0;
	motorMode = 0;
	MOVE_Trigger = 0;

	while (1)
	{
		//distance detect
		if((HOLD == 0) && (pre_hold == 1)){
			delay_ms(10);
			if(HOLD == 0){
				OLED_SetCursor(6,10);
				OLED_DisplayString("FLOOR detect...       ");
				idx = distanceDetect();
				OLED_SetCursor(6,106);
				OLED_DisplayChar(idx + '0');
				if(idx != currentFloor){
					targetFloor = currentFloor;
					currentFloor = idx;
					ACTIVE = 1;
					MOVE_Trigger = 1;
				}
				OLED_SetCursor(2,106);
				OLED_DisplayChar(currentFloor + '0');
			}
		}
		
		//targetFloor 1
		if((PB1 == 0) && (pre_pb1 == 1)){
			delay_ms(10);
			if(PB1 == 0){
				targetFloor = 1;
				OLED_SetCursor(4,106);
				OLED_DisplayChar(targetFloor + '0');
				ACTIVE = 0;
			}
		}

		//targetFloor 2
		if((PB2 == 0) && (pre_pb2 == 1)){
			delay_ms(10);
			if(PB2 == 0){
				targetFloor = 2;
				OLED_SetCursor(4,106);
				OLED_DisplayChar(targetFloor + '0');
				ACTIVE = 0;
			}
		}
		
		//targetFloor 3
		if((PB3 == 0) && (pre_pb3 == 1)){
			delay_ms(10);
			if(PB3 == 0){
				targetFloor = 3;
				OLED_SetCursor(4,106);
				OLED_DisplayChar(targetFloor + '0');
				ACTIVE = 0;
			}
		}

		//check
		if((PB4 == 0) && (pre_pb4 == 1)){
			delay_ms(10);
			if(PB4 == 0){
				ACTIVE = 1;
				MOVE_Trigger = 1;
			}
		}

		//motor control
		if(ACTIVE){
			if(MOVE_Trigger){
				MOVE_Trigger = 0;
				if(currentFloor < targetFloor){
					step = targetFloor - currentFloor;
					OLED_SetCursor(6,10);
					OLED_DisplayString("Status :goes   up...");
					motorMode = 1;
					Hall_cnt = 0;
					MOTOR1 = 1;
				}else if (currentFloor > targetFloor){
					step = currentFloor - targetFloor;
					OLED_SetCursor(6,10);
					OLED_DisplayString("Status :goes down...");
					motorMode = 2;
					Hall_cnt = 0;
					MOTOR2 = 1;
				}else{
					ARRIVE = 1;
					motorMode = 0;
					Hall_cnt = 1;
				}
			}

			if(Hall_cnt && step >= 2){
				if(motorMode == 1) currentFloor ++; else if (motorMode == 2) currentFloor --;
				OLED_SetCursor(2,106);
				OLED_DisplayChar(currentFloor + '0');
			}

			if(ARRIVE){
				currentFloor = targetFloor;
				OLED_SetCursor(2,106);
				OLED_DisplayChar(currentFloor + '0');
				OLED_SetCursor(6,10);
				OLED_DisplayString("Status :   arrive!!!");
				ACTIVE = 0;
				ARRIVE = 0;
				motorMode = 0;
			}
		}else MOTOR1 = MOTOR2 = 0;

		//debounce
		pre_pb1 = PB1;
		pre_pb2 = PB2;
		pre_pb3 = PB3;
		pre_pb4 = PB4;
		pre_hold = HOLD;

		delay_ms(1);								// Delay 1 ms
	}
}