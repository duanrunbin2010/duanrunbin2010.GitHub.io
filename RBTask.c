#include "user.h"         /*该行是必须的*/

float dT = 1.0f;          /*积分周期、定时任务周期，毫秒*/

uint32_t black_time[20];

int16_t sp0;
int16_t sp1;
int16_t time_limit;
int16_t light;

uint16_t gray_th[9];

int8_t state;
uint32_t black = 0;

uint32_t buff;

int16_t ls;
int16_t es;
int16_t is;
int16_t ds;

float laz;
float eaz;
float iaz;
float daz;

float lrz;
float erz;
float irz;
float drz;

void mydriver(int16_t lv, int16_t rv);
void gray_init(void);
int8_t gray_state(uint8_t ch);
void states_out(void);
void sport(int16_t speed);
void line(int16_t speed, int16_t angle);

void program1(void);
void program2(void);
void program3(void);
void start(void);
void stop(void);

void RateZ_test(void);
 
void Task(void)   
{
	start();	
  if(key == 1)
	{
		program1();
	}
	else if(key == 2)
	{
		program2();
	}
	else if(key == 4)
	{
		program3();
	}
	else 
	{
		gray_init();
	}
	stop();
}

void program1(void)   
{ 
//  while(seconds() < time_limit)
//	{
//		//line(sp0, 0);
//		mydriver(sp0, 0);
//	}
}
                                                                                   
void program2(void)   
{ 
//  while(seconds() < time_limit)
//	{
//		sport(sp0); 		
//	}
}

void program3(void)    //比赛程序 
{
	int16_t lv;
	int16_t rv;
	LCD_Clear(BACK_COLOR);
	
  while(1)
	{
		if(analog(10) > light)
		{
			break;
		}
	}	

	reset_system_time();
	H_reset(0);

	while(seconds() < time_limit)
	{
		lv = seconds();
		if(lv < 300)
		{
			lv = 300;
		}
		else if(lv < sp0)
		{
			lv = sp0;
		}
		else if(lv > sp1)  
		{
			lv = sp1;
		}
		else if(lv > sp1)
		{
			lv = sp1;
		}
		es = state;
		rv = (int16_t)(10.0f*es+0.1*is+50.0f*ds);  
		eaz = 0.5f*angle_d(0)+rv;
		rv = (int16_t)(100*eaz/lv+0.001f*iaz+10.0f*daz);    //待调试
		rv = LIMIT(rv, -900, 900);
		lv = (lv*(900-abs(rv)))/900; 
	  mydriver(lv, rv);
	}
}

void RateZ_test(void)   
{
	LCD_Clear(BACK_COLOR);
	while(1)
	{
		LCD_XY(10,10);
	  printf("%f",imudata[14]);
	  LCD_XY(10,30);
	  printf("%f",imudata[15]);
		LCD_XY(10,50);
	  printf("%4d",H_angle());
		if(touch())
		{
		  motor1(160);
	    motor2(160);
			motor3(160);
	    motor4(160);
		  sleep(2000);
		  LCD_XY(10,70);
	    printf("%f",imudata[6]-imudata[13]);
		  motor1(0);
	    motor2(0);
	    motor3(0);
	    motor4(0);
		}
	}
}

void start(void)
{
	uint8_t i; 
	for(i = 0; i < 9; i++)
	{
		gray_th[i] = eepread(100+i);
		sleep(10);
	}
	sp0 = eepread(0);
	sp1 = eepread(1);
	time_limit = eepread(2);
	light = eepread(3);
	
	motor1_start();
	motor2_start();
	motor3_start();
	motor4_start();

  Callbacktime((uint32_t)dT);                        /*开启定时线程*/ 
	LCD_Clear(GREEN);
	
  while(1)
	{
		LCD_XY(10,10);
	  printf("%4d",H_angle());
		LCD_XY(10,30);
	  printf("%3d",state);
		LCD_XY(10,50);
	  printf("%f",imudata[14]);
		LCD_XY(10,70);
		printf("%f",imudata[15]);
		LCD_XY(10,90);
		printf("%f",imudata[16]);
		if(touch() || GetKey())
		{
			break;
		}
	}	
	reset_system_time();
	H_reset(0);
}

void stop(void)    
{
  uint32_t time = seconds(); 
	motor1(0);
	motor2(0);
	motor3(0);
	motor4(0);
	LCD_Clear(BACK_COLOR);
	sleep(1000);
	LCD_XY(10,10);
	printf("time = %4d",time);
}

void gray_init(void)  
{
	uint16_t i;
	uint16_t max[9];
	uint16_t min[9];
	LCD_Clear(BACK_COLOR);
	LCD_FONT(12);
	for(i = 0; i < 9; i++)
	{
		max[i] = 0;
		min[i] = 4095;
	}
	while(1)
	{
		for(i = 0; i < 9; i++)
	  {
		  LCD_XY(10,10+10*i);
	    printf("%4d %4d",analog(i+1),gray_state(i));
	  }
		for(i = 0; i < 9; i++)
	  {
		  if(max[i] < analog(i+1))
		  {
			  max[i] = analog(i+1);
		  }
			if(min[i] > analog(i+1))
		  {
			  min[i] = analog(i+1);
		  }
	  }
		if(GetKey())
		{
			while(GetKey());
			LCD_Clear(BACK_COLOR);
			for(i = 0; i < 9; i++)
		  {
			  gray_th[i] = (max[i]+min[i])/2;
		  }		
			for(i = 0; i < 9; i++)
		  {
			  eepwrite(100+i, gray_th[i]);
				sleep(50);
		  }
			RateZ_test();
		}	
	}
}

int8_t gray_state(uint8_t ch)
{
	if(analog(ch+1) < gray_th[ch])
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void states_out(void)
{
	if(gray_state(3) && gray_state(4) && gray_state(5))
	{	
		state = 0;
		black = seconds();
	}
	
	else if(gray_state(3) && gray_state(4))
	{	
		state = 1;
	}
	else if(gray_state(4) && gray_state(5))
	{	
		state = -1;
	}
	else if(gray_state(4))
	{	
		state = 0;
	}
	
	else if(gray_state(2) && gray_state(3))
	{	
		state = 3;
	}
	else if(gray_state(3))
	{	
		state = 2;
	}
	
	else if(gray_state(5) && gray_state(6))
	{	
		state = -3;
	}
	else if(gray_state(5))
	{	
		state = -2;
	}
	
	else if(gray_state(1) && gray_state(2))
	{	
		state = 5;	
	}
	else if(gray_state(2))
	{	
		state = 4;
	}
	
	else if(gray_state(6) && gray_state(7))
	{	
		state = -5;
	}
	else if(gray_state(6))
	{	
		state = -4;
	}
	
	else if(gray_state(0) && gray_state(1))
	{	
		state = 7;
	}
	else if(gray_state(1))
	{	
		state = 6;
	}
	
	else if(gray_state(7) && gray_state(8))
	{	
		state = -7;
	}
	else if(gray_state(7))
	{	
		state = -6;
	}
	
	else if(gray_state(0))
	{	
		state = 8;
	}
	else if(gray_state(8))
	{	
		state = -8;
	}
}

void mydriver(int16_t lv, int16_t rv)
{
	int16_t spl;
	int16_t spr; 
	erz = rv-0.125f*(imudata[6]-imudata[13]);             //4轮0.2，5s0.31， 6s:0.19 0.375 40宽 0.136，0.136
	rv = (int16_t)(rv+0.5f*erz+0.001f*irz+20.0f*drz); 
	spl = rv-lv;
	spr = rv+lv;
	motor1(-spl);
	motor2(-spr);
	motor3(-spl);
	motor4(-spr);
}

void line(int16_t speed, int16_t angle)
{
	int16_t rv;
	rv = angle_d(angle);
	eaz = rv;
	rv = (int16_t)(0.5f*eaz+0.000f*iaz+0.0f*daz);   //
	mydriver(speed, rv);
}

void sport(int16_t speed) 
{
	float rv;
	es = state;              
	rv = 5.0f*es+0.01*is+20.0f*ds;   //
  mydriver(speed, (int16_t)rv);
}

void Callback(void)
{
	states_out();
	getdata( imudata );
	GetAngle(imudata, 100.0f, 0.001f, 0.001f, dT);
	
	is = LIMIT(is + es, -32766, 32766);
	ds = es-ls;
	if(ls != es)
	{
		is = 0;
		ls = es;
	}
	
	iaz = iaz+eaz;
	daz = eaz-laz;
	laz = eaz;
	
	irz = irz+erz;
//	if(erz == 0 || (erz > 0 && irz < 0) || (erz < 0 && irz > 0))
//	{
//		irz = 0;
//	}
	drz = erz-lrz;
	lrz = erz;
}

void copyright(void)
{
	LCD_Init();
	imu_init();
	eep_init();
	LCD_Init();	
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);  
  HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED); 
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC1DMA, 14);
	init();
}
