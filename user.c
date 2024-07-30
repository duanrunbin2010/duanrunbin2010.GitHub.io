#include "user.h"

uint8_t key = 0;

ALIGN_32BYTES(uint16_t ADC1DMA[14]) __attribute__((section(".ARM.__at_0x24000000")));

int16_t comp0;
int16_t xmax;
int16_t xmin;
int16_t ymax;
int16_t ymin;
int16_t zmax;
int16_t zmin;

extern __IO uint32_t uwTick;

uint32_t seconds(void)
{
  return HAL_GetTick();
}

void reset_system_time(void)
{
	uwTick=0;
}

void sleep(uint32_t Delay)
{
	HAL_Delay(Delay);
}

void sleepus(uint32_t Delay)
{
	delay_us(Delay);  
}

void HORIZONTAL(uint8_t p)
{
	USE_HORIZONTAL = p;
	LCD_SetParam();
}

void LCD_ON(void)
{
  LCD_LED_SET;
}

void LCD_OFF(void)
{
  LCD_LED_CLR;
}

uint8_t GetButton1(void) 
{
	if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_13) != GPIO_PIN_RESET)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

uint8_t GetButton2(void) 
{
	if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_14) != GPIO_PIN_RESET)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

uint8_t GetButton3(void) 
{
	if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_15) != GPIO_PIN_RESET)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

uint8_t GetKey(void)
{
	return GetButton1()+2*GetButton2()+4*GetButton3();
}

int16_t eepread(uint16_t VirtAddress)
{
	uint16_t Data;
	EE_ReadVariable(VirtAddress,&Data);
	HAL_Delay(2);
	return (int16_t)(Data);
}

void eepwrite(uint16_t VirtAddress, int16_t Data)
{
	if(VirtAddress<180)
	{
	  HAL_FLASH_Unlock();
	  if(EE_Init() != HAL_OK) while(1);
		EE_WriteVariable(VirtAddress, (uint16_t)(Data));
	  HAL_FLASH_Lock();
		HAL_Delay(2);
	}
}

void eepwrite_Ex(uint16_t VirtAddress, int16_t Data)
{
	HAL_FLASH_Unlock();
	if(EE_Init() != HAL_OK) while(1);
	EE_WriteVariable(VirtAddress, (uint16_t)(Data));
	HAL_FLASH_Lock();
	HAL_Delay(2);
}

void eepw(uint16_t VirtAddress, float Data)  //占2个地址
{
	union com fu;
	uint16_t *buf;
	fu.fl = Data;
	buf = fu.uint16;
	HAL_FLASH_Unlock();
	if(EE_Init() != HAL_OK) while(1);
	EE_WriteVariable(VirtAddress, buf[0]);
	EE_WriteVariable(VirtAddress+1, buf[1]);
	HAL_FLASH_Lock();
	HAL_Delay(2);
}

float eepr(uint16_t VirtAddress)    //占2个地址
{
	union com fu;
	uint16_t d1;
	uint16_t d2;
	EE_ReadVariable(VirtAddress, &d1);
	EE_ReadVariable(VirtAddress+1, &d2);
	HAL_Delay(2);
	fu.uint16[0] = d1;
	fu.uint16[1] = d2;
	return fu.fl;
}

void eep_init(void)
{
	uint8_t i;

	for(i = 0;i < 10; i++)
	{
		touch_correa_data[i] = eepread(200+i);
	}
	
	comp0 = eepread(Addcomp0);
  xmax = eepread(Addxmax);
  xmin = eepread(Addxmin);
	ymax = eepread(Addymax);
  ymin = eepread(Addymin);
	zmax = eepread(Addzmax);
  zmin = eepread(Addzmin);
	
	for(i = 0; i < 7; i++)
	{
		imudata[7+i] = eepr(Addtemperature+2*i);
	}
}

uint16_t analog(uint16_t Channel)
{
	return ADC1DMA[Channel-1];
}

uint16_t analog3(uint16_t Channel)
{
	ADC_ChannelConfTypeDef sConfig;
  if(Channel == 1)
	{
		sConfig.Channel = ADC_CHANNEL_8;
	}
	else if(Channel == 2)
	{
		sConfig.Channel = ADC_CHANNEL_6;
	}
	else if(Channel == 3)
	{
		sConfig.Channel = ADC_CHANNEL_5;
	}
	else if(Channel == 4)
	{
		sConfig.Channel = ADC_CHANNEL_9;
	}
	else if(Channel == 5)
	{
		sConfig.Channel = ADC_CHANNEL_4;
	}
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_8CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
	if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    HAL_Delay(10);
  }
	HAL_ADC_Start(&hadc3);
	HAL_ADC_PollForConversion(&hadc3, 20);
	return HAL_ADC_GetValue(&hadc3);
}

uint8_t digital(uint8_t Channel)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	if(Channel==1)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7) != GPIO_PIN_RESET)
	  {
		  return 0;
	  }
	  else
	  {
		  return 1;
	  }
	}
	else if(Channel==2)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_4;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_4) != GPIO_PIN_RESET)
	  {
		  return 0;
	  }
	  else
	  {
		  return 1;
	  }
	}
	else if(Channel==3)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_5;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_5) != GPIO_PIN_RESET)
	  {
		  return 0;
	  }
	  else
	  {
		  return 1;
	  }
	}
	else if(Channel==4)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0) != GPIO_PIN_RESET)
	  {
		  return 0;
	  }
	  else
	  {
		  return 1;
	  }
	}
	else if(Channel==5)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) != GPIO_PIN_RESET)
	  {
		  return 0;
	  }
	  else
	  {
		  return 1;
	  }
	}
	else if(Channel==6)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_11;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		if(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_11) != GPIO_PIN_RESET)
	  {
		  return 0;
	  }
	  else
	  {
		  return 1;
	  }
	}
	else if(Channel==7)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_12;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		if(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_12) != GPIO_PIN_RESET)
	  {
		  return 0;
	  }
	  else
	  {
		  return 1;
	  }
	}
	else if(Channel==8)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_6;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6) != GPIO_PIN_RESET)
	  {
		  return 0;
	  }
	  else
	  {
		  return 1;
	  }
	}
	else if(Channel==9)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_5;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5) != GPIO_PIN_RESET)
	  {
		  return 0;
	  }
	  else
	  {
		  return 1;
	  }
	}
	else if(Channel==10)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_4;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) != GPIO_PIN_RESET)
	  {
		  return 0;
	  }
	  else
	  {
		  return 1;
	  }
	}
	else if(Channel==11)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3) != GPIO_PIN_RESET)
	  {
		  return 0;
	  }
	  else
	  {
		  return 1;
	  }
	}
	else if(Channel==12)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2) != GPIO_PIN_RESET)
	  {
		  return 0;
	  }
	  else
	  {
		  return 1;
	  }
	}
	else if(Channel==13)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_1) != GPIO_PIN_RESET)
	  {
		  return 0;
	  }
	  else
	  {
		  return 1;
	  }
	}
	else if(Channel==14)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0) != GPIO_PIN_RESET)
	  {
		  return 0;
	  }
	  else
	  {
		  return 1;
	  }
	}
	else
	{
		return 0;
	}
}

void set_digital_out(uint8_t Channel,uint8_t level)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  if(Channel==1)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		if(level==1)
	  {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7,  GPIO_PIN_SET);
	  }
	  else
	  {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7,  GPIO_PIN_RESET);
	  }	
	}
	else if(Channel==2)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_4;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		if(level==1)
	  {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4,  GPIO_PIN_SET);
	  }
	  else
	  {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4,  GPIO_PIN_RESET);
	  }	
	}
	else if(Channel==3)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_5;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		if(level==1)
	  {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,  GPIO_PIN_SET);
	  }
	  else
	  {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,  GPIO_PIN_RESET);
	  }	
	}
	else if(Channel==4)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		if(level==1)
	  {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0,  GPIO_PIN_SET);
	  }
	  else
	  {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0,  GPIO_PIN_RESET);
	  }		
	}
	else if(Channel==5)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		if(level==1)
	  {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1,  GPIO_PIN_SET);
	  }
	  else
	  {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1,  GPIO_PIN_RESET);
	  }		
	}
	else if(Channel==6)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_11;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		if(level==1)
	  {
      HAL_GPIO_WritePin(GPIOF, GPIO_PIN_11,  GPIO_PIN_SET);
	  }
	  else
	  {
      HAL_GPIO_WritePin(GPIOF, GPIO_PIN_11,  GPIO_PIN_RESET);
	  }	
	}
	else if(Channel==7)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_12;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		if(level==1)
	  {
      HAL_GPIO_WritePin(GPIOF, GPIO_PIN_12,  GPIO_PIN_SET);
	  }
	  else
	  {
      HAL_GPIO_WritePin(GPIOF, GPIO_PIN_12,  GPIO_PIN_RESET);
	  }		
	}
	else if(Channel==8)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_6;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		if(level==1)
	  {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6,  GPIO_PIN_SET);
	  }
	  else
	  {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6,  GPIO_PIN_RESET);
	  }		
	}
	else if(Channel==9)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_5;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		if(level==1)
	  {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,  GPIO_PIN_SET);
	  }
	  else
	  {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,  GPIO_PIN_RESET);
	  }	
	}
	else if(Channel==10)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_4;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		if(level==1)
	  {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4,  GPIO_PIN_SET);
	  }
	  else
	  {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4,  GPIO_PIN_RESET);
	  }		
	}
	else if(Channel==11)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		if(level==1)
	  {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3,  GPIO_PIN_SET);
	  }
	  else
	  {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3,  GPIO_PIN_RESET);
	  }	
	}
	else if(Channel==12)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		if(level==1)
	  {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2,  GPIO_PIN_SET);
	  }
	  else
	  {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2,  GPIO_PIN_RESET);
	  }	
	}
	else if(Channel==13)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		if(level==1)
	  {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1,  GPIO_PIN_SET);
	  }
	  else
	  {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1,  GPIO_PIN_RESET);
	  }	
	}
	else if(Channel==14)	
	{
		GPIO_InitStruct.Pin = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		if(level==1)
	  {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0,  GPIO_PIN_SET);
	  }
	  else
	  {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0,  GPIO_PIN_RESET);
	  }		
	}
}

void USART3_baud(uint32_t baud)
{
  huart3.Instance = USART3;
  huart3.Init.BaudRate = baud;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    HAL_Delay(10);
  }
}

void USART1_baud(uint32_t baud)
{
	huart1.Instance = USART1;
  huart1.Init.BaudRate = baud;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    HAL_Delay(10);
  }
}

uint8_t spi_eye1(uint8_t command)
{
	static uint8_t tx[10];
	static uint8_t rx[10];
	if(HAL_SPI_TransmitReceive_IT(&hspi6, tx, rx, 10) != HAL_OK)
	{
		HAL_Delay(1);
	}
	if(HAL_SPI_GetState(&hspi6) != HAL_SPI_STATE_READY)
	{
		HAL_Delay(1);
	}
	return rx[command];
}

uint8_t spi_eye2(uint8_t command)
{
	static uint8_t tx[10];
	static uint8_t rx[10];
	if(HAL_SPI_TransmitReceive_IT(&hspi4, tx, rx, 10) != HAL_OK)
	{
		HAL_Delay(1);
	}
	if(HAL_SPI_GetState(&hspi4) != HAL_SPI_STATE_READY)
	{
		HAL_Delay(1);
	}
	return rx[command];
}

int16_t Temperature(void)
{
	uint8_t tmpRead[2];
	MPU9255_ReadRegs(0x41, tmpRead, 2);       
	return (Byte16(int16_t, tmpRead[0],  tmpRead[1])); 
}

int16_t Gyro(uint8_t ADDR)
{
	uint8_t tmpRead[2];
	MPU9255_ReadRegs(0x43+2*ADDR, tmpRead, 2);
	return (Byte16(int16_t, tmpRead[0],  tmpRead[1])); 
}

int16_t Accel(uint8_t ADDR)
{
	uint8_t tmpRead[2];
	MPU9255_ReadRegs(0x3B+2*ADDR, tmpRead, 2);
	return (Byte16(int16_t, tmpRead[0],  tmpRead[1])); 
}

int16_t getcompass(void)
{
	uint8_t tmpRead[6];
	static double magx;
	static double magy;
	static double magz;
	double angle;
  MPU9255_ReadRegs(0x4A, tmpRead, 6);
	sleep(2);
	magx = 0.85*magx+0.15*(double)(Byte16(int16_t, tmpRead[1], tmpRead[0])-0.5*(xmax+xmin));
	magy = 0.85*magy+0.15*(double)(Byte16(int16_t, tmpRead[3], tmpRead[2])-0.5*(ymax+ymin));
	magz = 0.85*magz+0.15*(double)(Byte16(int16_t, tmpRead[5], tmpRead[4])-0.5*(zmax+zmin));
	angle = atan2((double)magy,(double)magx);
	angle = 572.968*angle;        //1800/3.14		
	return (int16_t)angle;
}

void correa(void)
{
	int16_t x;
	int16_t y;
	int16_t z;
  uint8_t tmpRead[6];
	xmax = -500;
	xmin = 500;
	ymax = -500;
	ymin = 500;
	zmax = -500;
	zmin = 500;
	while(1)
	{
		MPU9255_ReadRegs(0x4A, tmpRead, 6);
		x = (Byte16(int16_t, tmpRead[1], tmpRead[0]));
	  y = (Byte16(int16_t, tmpRead[3], tmpRead[2]));
		z = (Byte16(int16_t, tmpRead[5], tmpRead[4])); 
		if(x > xmax)
		{
		 xmax = x;
	  }
		if(x < xmin)
	  {
			xmin = x;
		}
	  if(y > ymax)
		{
			ymax = y;
		}
		if(y < ymin)
		{
			ymin = y;
		}
		if(z > zmax)
		{
			zmax = z;
		}
		if(z < zmin)
		{
			zmin = z;
		}
		LCD_XY(10,10);
		printf("x = %4d  %4d",xmax, xmin);
		LCD_XY(10,30);
		printf("y = %4d  %4d",ymax, ymin);
	  LCD_XY(10,50);
	  printf("z = %4d  %4d",zmax, zmin);
		LCD_XY(10,90);
	  printf("comp = %4d",comp());
		if(GetKey())
		{
			while(GetKey());
			eepwrite_Ex(Addxmax, xmax);
      eepwrite_Ex(Addxmin, xmin);
	    eepwrite_Ex(Addymax, ymax);
      eepwrite_Ex(Addymin, ymin);
	    eepwrite_Ex(Addzmax, zmax);
      eepwrite_Ex(Addzmin, zmin);
	    sleep(1);
	    LCD_Clear(BACK_COLOR);
			break;
		}
	}
}

int16_t comp(void)
{
	return (getcompass()-comp0+7200)%3600;
}

void logo(void)
{
	LCD_Clear(BACK_COLOR);
	LCD_Drawbmp16(0,0,64,64,trademark);
	LCD_Drawbmp16(64,0,64,64,qrcode);
	LCD_XY(12,125);
	LCD_FONT(12);
	printf("QQ:639573784");
	LCD_XY(12,140);
	printf("COPYRIGHT 2017 Frg");
	LCD_FONT(16);
}

void init(void)
{
	uint8_t tempy;
	if(GetKey()==1)
	{
		touch_correa();
	}
	else if(GetKey()==2)
	{
		LCD_Clear(BACK_COLOR);
		printf("Clear the data?");
		while(1)
		{
			if(GetKey()==4)
			{
				break;
			}
		}
		LCD_Clear(RED);
		
		for(uint16_t i=0;i<100;i++)
		{
			eepwrite(i,0);
		}
		while(1)
		{
			eepwrite(0,0);
			sleep(50);
			if(eepread(0)==0)break;
			sleep(50);
		}
		LCD_Clear(BACK_COLOR);
		printf("OK!");
		while(1);
	}
	
	else if(GetKey()==4)
	{	
		while(GetKey());
		LCD_Clear(BACK_COLOR);
		LCD_XY(10,10);
	  printf("wait a moment");
		sleep(2000);
		imu_error(imudata);
		for(tempy = 0; tempy < 7; tempy++)
	  {
      eepw(Addtemperature+2*tempy, imudata[7+tempy]); 
	  }

		LCD_Clear(BACK_COLOR);
		LCD_XY(10,10);
		printf("%f", imudata[7]);
		LCD_XY(10,30);
		printf("%f", imudata[8]);
		LCD_XY(10,50);
		printf("%f", imudata[9]);
		LCD_XY(10,70);
		printf("%f", imudata[10]);
		LCD_XY(10,90);
		printf("%f", imudata[11]);
		LCD_XY(10,110);
		printf("%f", imudata[12]);
		LCD_XY(10,130);
		printf("%f", imudata[13]);
		
		while(1);
	}
	
	else
	{
		logo();	
		while(1)
		{
			LCD_XY(10,80);
	    printf("Comp = %-4d", comp());
	    LCD_XY(10,100);
	    printf("Vbat = %-4d",analog3(2));
			tempy = touchY();
			key = GetKey();
			if(tempy > 0 && tempy < 64)
			{
				systest();
			}
			else if(key || (tempy > 64 && tempy < 160))
			{
				break;
			}
		}
		LCD_Clear(RED);
		while(GetKey());
		reset_system_time();
	}
}
