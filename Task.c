#include "user.h"         /*该行是必须的*/
/*****V3.0*****/
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

void program1(void)   
{
	uint16_t sum=0,anss=0,peiyt=0,ansx=0;
	int16_t sp[20];
	int16_t pyt[100];
	for(uint16_t i=0;i<20;i++)
	{
		pyt[i]=0;
		sp[i]=0;
	}
	sleep(100);
	uint32_t pt;
	while(1)
	{
		if(GetKey())
		{
			sleep(300);
				LCD_Clear(BLUE);
			while(1)
			{
				LCD_XY(10,10);
					printf("En:%4d",En());
				sport(100);
				if(touch())
				{	motor1(0);
					motor2(0);
					motor3(0);
					motor4(0);
					LCD_Clear(RED);
						sleep(300);
					while(1)
					{
						if(touch())
						{
							pyt[sum++]=En();
							sleep(300);
							LCD_Clear(BLUE);
							break;
						}
						else if(GetKey())
					{
					motor1(0);
					motor2(0);
					motor3(0);
					motor4(0);
					LCD_Clear(BACK_COLOR);
					sleep(300);
					for(uint16_t i=0;i<sum;i++)
					{
						LCD_XY(10,10+i*20);
						printf(" %d :%d",i+1,pyt[i]);
					}
					LCD_XY(10,10+20*sum);
					printf(" %d :%d",sum+1,En());
					while(1);
					}
						LCD_XY(10,10);
						printf("En:%4d",En());
						
					}
					
				}
				if(GetKey())
				{
					motor1(0);
					motor2(0);
					motor3(0);
					motor4(0);
					LCD_Clear(BACK_COLOR);
					sleep(300);
					for(uint16_t i=0;i<sum;i++)
					{
						LCD_XY(10,10+i*20);
						printf(" %d :%d",i+1,pyt[i]);
					}
					while(1);
				}
			}
		}			
		else if(touch())
		{
			sleep(300);
			sum=1;
			while(1)
	{
		sport(100);
		if(touch())
		{
			motor1(0);
			motor2(0);
			motor3(0);
			motor4(0);
			uint32_t tim;
			uint16_t last;
			uint16_t yt=0;
			
			LCD_Clear(RED);
			pyt[sum++]=En()+pt;
			last=pyt[sum-1];
			printf("En:%d",pyt[sum-1]);
			LCD_XY(0,40);
			printf("sp:%d",sp[sum-2]);
			sleep(300);
			while(1)
			{
				LCD_XY(0,0);
				printf("En:%5d",En()+pt);
				if(touch())
				{
					pt+=pyt[sum-1]-last;
					sleep(300);
					break;
				}
				/******* */
					if(GetButton3())
					{
						while(1)
						{
							if(!GetButton3())
							{
								break;
							}
						}
						yt++;			
					}
					if(yt>1)
					{
						yt=0;
					}			
					
				/**/
				if(yt==0)
				{	LCD_XY(0,20);
					LCD_POINT(Modify);
					printf("ChangeEn:%5d",pyt[sum-1]);
					LCD_XY(0,40);
					LCD_POINT(BLACK);
					printf("sp:%4d",sp[sum-2]);
					if(GetButton1())
					{
						pyt[sum-1]=pyt[sum-1]-(seconds()-tim)/100;			
						
					}
					else if(GetButton2())
					{
						pyt[sum-1]=pyt[sum-1]+(seconds()-tim)/100;
					}
					else
					{
						tim=seconds();
					}
				}
				if(yt==1)
				{
					LCD_XY(0,40);
					LCD_POINT(Modify);
					printf("sp:%4d",sp[sum-2]);
					LCD_XY(0,20);
					LCD_POINT(BLACK);
					printf("ChangeEn:%5d",pyt[sum-1]);
					if(GetButton1())
					{
						sp[sum-2]=sp[sum-2]-(seconds()-tim)/100;			
						
					}
					else if(GetButton2())
					{
						sp[sum-2]=sp[sum-2]+(seconds()-tim)/100;
					}
					else
					{
						tim=seconds();
					}
				}
				
			}
			LCD_Clear(GREEN);
		}
		if(GetKey())
		{
			motor1(0);
			motor2(0);
			motor3(0);
			motor4(0);
			LCD_Clear(BACK_COLOR);
			LCD_XY(10,10);
			printf("Waiting...");
			
			sleep(300);
			while(1)
			{
				if(GetKey())
				{
					LCD_Clear(BACK_COLOR);
					LCD_XY(10,10);
					eepwrite(99,sp[0]);
						for(uint16_t i = 0; i < sum; i++)
					{
								eepwrite(2*i,sp[i]);
								eepwrite(2*i+1,pyt[i+1]);
						
					}
					//eepwrite(2*sum+1,0);
					printf("OK!");
					sleep(300 );
					for(uint16_t i=1;i<sum;i++)
					{
							LCD_XY(10,10+i*20);
						printf(" %d :%d",i,pyt[i]);
					}
					
					LCD_Print = 0;

					for(uint16_t i=1;i<sum;i++)
					{
						printf(" \r\n%d :%d",i,pyt[i]);
					}
					printf("\r\n");
					for(uint16_t i=1;i<sum;i++)
					{
						printf("%d,",pyt[i]);
					}
					
				}
			}
		}
	}
		}
	}
	
	//while(1);

	
}

void program2(void)   
{
	
	int8_t last;
	int16_t H;
	int16_t dg;
	int16_t sp = 250;
	int16_t Ens = 21000;
  while(En() < Ens && seconds() < 16000)
	{
		if(last != state)
		{
			H = H_angle();
			last = state;
		}
		dg = (H+30*state+3600)%3600;  //30
		if(abs(state) > 5)
		{
			mydriver(sp/13.5, (int16_t)(0.05f*sp*state));  //10,15, 0.06
		}
		else
		{
		  line(sp, dg);
		}
		if(seconds() > out_time+400 && abs(state) > 5)
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
			
		  dg = (H+37*state+3600)%3600;  //30
			es=state;
		  if(abs(state) > 5)
		  {
				lv = 0.5f*abs(is);
				rv = 50.0f * state-0.2f*is;
				lv = LIMIT(lv , 0 , 0.1f * buf); 
			  mydriver((int16_t)(0.1f*buf), (int16_t)(0.1f*buf*state));  //10,15, 0.06
				
				//mydriver(abs((int16_t )(is*0.1f)),35*state-0.56f*is);

		  }
		  else
		  {
		    line((int16_t)buf, dg);
		  }
			if(seconds() > out_time+400 && abs(state) > 6)
			{
				sp[i + 1] = 0;
				Ens[i + 1] = 0;
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
//void states_out(void)
//{
//				if(gray_state(6))
//				{	
//					state = 0;	
//					out_time = seconds();
//				}
//				else if(gray_state(5) && !gray_state(7))
//				{	
//					state = 1;	
//					out_time = seconds();
//				}
//				else if(!gray_state(5) && gray_state(7))
//				{	
//					state = -1;	
//					out_time = seconds();
//				}
//				else if(gray_state(4) && !gray_state(8))
//				{	
//					state = 2;	
//					out_time = seconds();
//				}
//				else if(!gray_state(4) && gray_state(8))
//				{	
//					state = -2;	
//					out_time = seconds();
//				}
//				else if(gray_state(3) && !gray_state(9))
//				{	
//					state = 3;	
//					out_time = seconds();
//				}
//				else if(!gray_state(3) && gray_state(9))
//				{	
//					state = -3;	
//					out_time = seconds();
//				}
//				else if(gray_state(2) && !gray_state(10))
//				{	
//					state = 4;	
//					out_time = seconds();
//				}
//				else if(!gray_state(2) && gray_state(10))
//				{	
//					state = -4;	
//					out_time = seconds();
//				}
//				else if(gray_state(1) && !gray_state(11))
//				{	
//					state = 6;	
//					out_time = seconds();
//				}
//				else if(!gray_state(1) && gray_state(11))
//				{	
//					state = -6;	
//					out_time = seconds();
//				}
//				if(gray_state(0) && !gray_state(12))
//				{	
//					state = 10;	
//					out_time = seconds();
//				}
//				else if(!gray_state(0) && gray_state(12))
//				{	
//					state = -10;	
//					out_time = seconds();
//				}
//}

void states_out(void)
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
	rv = rv+(int16_t)(2.0f*erz+0.01f*irz+10.0f*drz);     //
	//rv = rv+(int16_t)(1.5f*erz+0.0f*irz+0.0f*drz); 
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
	rv = (int16_t)(0.8f*eaz+0.001f*iaz+10.0f*daz);  //
	//rv = (int16_t)(0.8f*eaz+0.0f*iaz+0.0f*daz); 
	mydriver(lv, rv);
}

void sport(int16_t speed) 
{
	float lv;
	float rv;
	es = state;
  lv = speed*(1.0f-0.12f*abs(state));    // 	
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
