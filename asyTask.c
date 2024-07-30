#include "user.h"         /*该行是必须的*/

#define symbol( x ) ( x < 0  ? -1 : ( x > 0 ? 1 : 0 ) )

float dT = 1.0f;          /*积分周期、定时任务周期，毫秒*/

uint16_t gray_th[13];

int8_t state;
uint32_t out_time;

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
void setEn(uint8_t ch, uint32_t count);
int32_t En(void);
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

void program22(void)   
{
	float lv;
	float rv;
	while(seconds() < 2000)
	{
	  es = state-5;
		//es = LIMIT(es, -10, 10);
    lv =300*(1.0f-0.1f*abs(es));           // 	
	  rv = 20*es+0.1f*is;       //
    mydriver((int16_t)lv, (int16_t)rv);
	}
}

void program1(void)   
{
	while(1)
	{
		sport(100);
		if(touch())
		{
			break;
		}
	}
}

void program2(void)   
{
	int8_t last;
	int16_t H;
	int16_t dg;
	int16_t sp = eepread(0);
	int16_t Ens = eepread(1);
  while(En() < Ens && seconds() < 10000)
	{
		if(last != state)
		{
			H = H_angle();
			last = state;
		}
		dg = (H+50*state+3600)%3600;  //30
		if(abs(state) > 5)
		{
			mydriver(sp/10, (int16_t)(0.06f*sp*state));  //10,15, 0.06
		}
		else
		{
		  line(sp, dg);
		}
		if(seconds() > out_time+200 && abs(state) > 5)
		{
      break;
		}
	}
}
  
void program3(void)   
{
	int16_t i;
	int16_t sp[20];
	int16_t Ens[20];
	float buf = 200.0f;
	int8_t last;
	int16_t H;
	int16_t dg;
	float lv;
	float rv;
	for(i = 0; i < 20; i++)
	{
		sp[i] = eepread(2*i);
		Ens[i] = eepread(2*i+1);
	}
	for(i = 0; i < 20; i++)
	{
		if(Ens[i] == 0 || sp[i] == 0)
		{
			break;
		}
		while(En() < Ens[i])
	  {
			if(buf > (float)sp[i])
			{
				buf = (float)sp[i];
			}
			else if(buf < (float)sp[i])
			{
				buf = buf+0.01f;   //0.01
			}
      if(last != state)
		  {
			  H = H_angle();
			  last = state;
		  }
//			es = state;
		  dg = (H+50*state+3600)%3600;  //50
//			dg = (int16_t)(H+50.0f*es+0.3f*is+12.0f*ds);
//			dg = (H+dg+3600)%3600;
			es = state;
		  if(abs(state) > 5)
		  {
				lv = 0.5f*abs(is);
				rv = 50.0f*state-0.2f*is;
				lv = LIMIT(lv, 0, 0.1f*buf);
			  mydriver((int16_t)lv, rv);     //10,15, 0.06
		  }
		  else
		  {
		    line((int16_t)buf, dg);
		  }
			if(seconds() > out_time+300 && abs(state) > 5)   //
			{
				sp[i+1] = 0;
				Ens[i+1] = 0;
				break;
			}
	  }
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
		  motor1(-200);
	    motor2(-200);
	    motor3(-200);
	    motor4(-200);
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
	for(i = 0; i < 13; i++)
	{
		gray_th[i] = eepread(100+i);
		sleep(10);
	}                                  //涵道无刷电机初始化
	motor1_start();
	motor2_start();
	motor3_start();
	motor4_start();
  Encoder1_init();
	Encoder2_init();
	Callbacktime((uint32_t)dT);                   /*开启定时线程*/ 
	LCD_Clear(GREEN);

  while(1)
	{
		LCD_XY(10,10);
	  printf("%4d",H_angle());
		LCD_XY(10,30);
	  printf("%3d",state);
		if(touch() || GetKey())
		{
			break;
		}
	}	
	setEn(1,0);
	setEn(2,0);
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
	printf("time = %d",time);
	LCD_XY(10,30);
	printf("En = %d",En());
}

void setEn(uint8_t ch, uint32_t count)  
{
	if(ch == 1)
	{
		__HAL_TIM_SET_COUNTER(&htim5, count);
	}
	else if(ch == 2)
	{
		__HAL_TIM_SET_COUNTER(&htim2, count);
	}
}

int32_t En(void)
{
	return (Encoder1()-Encoder2())/2;  
}

void gray_init(void)  
{
	uint16_t i;
	uint16_t max[13];
	uint16_t min[13];
	LCD_Clear(BACK_COLOR);
	LCD_FONT(12);
	for(i = 0; i < 13; i++)
	{
		max[i] = 0;
		min[i] = 4095;
	}
	while(1)
	{
		for(i = 0; i < 13; i++)
	  {
		  LCD_XY(10,10+10*i);
	    printf("%4d %4d",analog(i+1),gray_state(i));
	  }
		for(i = 0; i < 13; i++)
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
			for(i = 0; i < 13; i++)
		  {
			  gray_th[i] = (max[i]+min[i])/2;
		  }		
			for(i = 0; i < 13; i++)
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
	if(gray_state(5) && gray_state(6) && gray_state(7))
	{
		state = 0;	
		out_time = seconds();
	}
	else if(gray_state(5) && !gray_state(7))
	{	
		state = 1;	
		out_time = seconds();
	}
	else if(!gray_state(5) && gray_state(7))
	{	
		state = -1;	
		out_time = seconds();
	}
	else if(gray_state(6))
	{	
		state = 0;	
		out_time = seconds();
	}
	else if(gray_state(4) && !gray_state(8))
	{	
		state = 2;	
		out_time = seconds();
	}
	else if(!gray_state(4) && gray_state(8))
	{	
		state = -2;	
		out_time = seconds();
	}
	else if(gray_state(3) && !gray_state(9))
	{	
		state = 3;	
		out_time = seconds();
	}
	else if(!gray_state(3) && gray_state(9))
	{	
		state = -3;	
		out_time = seconds();
	}
	else if(gray_state(2) && !gray_state(10))
	{	
		state = 4;	
		out_time = seconds();
	}
	else if(!gray_state(2) && gray_state(10))
	{	
		state = -4;	
		out_time = seconds();
	}
	else if(gray_state(1) && !gray_state(11))
	{	
		state = 5;	
		out_time = seconds();
	}
	else if(!gray_state(1) && gray_state(11))
	{	
		state = -5;	
		out_time = seconds();
	}
	else if(gray_state(0) && !gray_state(12))
	{	
		state =6;	
		out_time = seconds();
	}
	else if(!gray_state(0) && gray_state(12))
	{	
		state = -6;	
		out_time = seconds();
	}
}

void states_out1(void)
{
	if(gray_state(0) && !gray_state(12))
	{	
		state = 10;	
		out_time = seconds();
	}
	else if(!gray_state(0) && gray_state(12))
	{	
		state = -10;	
		out_time = seconds();
	}
	else if(gray_state(1) && !gray_state(11))
	{	
		state = 6;	
		out_time = seconds();
	}
	else if(!gray_state(1) && gray_state(11))
	{	
		state = -6;	
		out_time = seconds();
	}
	else if(gray_state(2) && !gray_state(10))
	{	
		state = 4;	
		out_time = seconds();
	}
	else if(!gray_state(2) && gray_state(10))
	{	
		state = -4;	
		out_time = seconds();
	}
	else if(gray_state(3) && !gray_state(9))
	{	
		state = 3;	
		out_time = seconds();
	}
	else if(!gray_state(3) && gray_state(9))
	{	
		state = -3;	
		out_time = seconds();
	}
	else if(gray_state(4) && !gray_state(8))
	{	
		state = 2;	
		out_time = seconds();
	}
	else if(!gray_state(4) && gray_state(8))
	{	
		state = -2;	
		out_time = seconds();
	}
	else if(gray_state(5) && !gray_state(7))
	{	
		state = 1;	
		out_time = seconds();
	}
	else if(!gray_state(5) && gray_state(7))
	{	
		state = -1;	
		out_time = seconds();
	}
	else if(gray_state(6))
	{	
		state = 0;	
		out_time = seconds();
	}
}

void mydriver(int16_t lv, int16_t rv)
{
	int16_t spl;
	int16_t spr; 
	erz = rv-0.43f*(imudata[6]-imudata[13]);          
	//rv = rv+(int16_t)(2.5f*erz+0.01f*irz+10.0f*drz); 	//1_0.5~3.0 2   i d
	rv = rv+(int16_t)(1.5f*erz+0.01f*irz+10.0f*drz);
	spl = rv-lv;
	spr = rv+lv;
	motor1(-spl);
	motor2(-spr);
	motor3(-spl);
	motor4(-spr);
}

void line(int16_t speed, int16_t angle)
{
	int16_t lv;
	int16_t rv;
	rv = angle_d(angle);
	eaz = rv;
	rv = LIMIT(rv, -900, 900);
	lv = (speed*(900-abs(rv)))/900; 
	//rv = (int16_t)(0.8f*eaz+0.001f*iaz+10.0f*daz);  //
	rv = (int16_t)(0.8f*eaz+0.001f*iaz+10.0f*daz);
	mydriver(lv, rv);
}

void sport(int16_t speed) 
{
	float lv;
	float rv;
	es = state;
  lv = speed*(1.0f-0.15f*abs(state));    // 	
	if(abs(state) > 5)
	{
		rv = 0.135f*speed*es+12.0f*ds;     //
	}
	else
	{
		rv = 0.135f*speed*es+0.3f*is+12.0f*ds;     //
	}
  mydriver((int16_t)lv, (int16_t)rv);
}

void Callback(void)
{
	states_out();
	getdata( imudata );
	GetAngle(imudata, 100.0f, 0.001f, 0.001f, dT);
	
	is = is+symbol( es );
	is = LIMIT(is, -32766, 32766);
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
	if(erz == 0 || (erz > 0 && irz < 0) || (erz < 0 && irz > 0))
	{
		irz = 0;
	}
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
