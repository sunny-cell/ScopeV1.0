#include "headfile.h"
int clear_flag = 0;//清屏标志位
int T_point;
uint16_t T_value=3072;//3072
float ADC_ConvertedValueLocal;  
char dispBuff[100];
int disp_value[320];
float Mag[512];
long FFT_input[1024];
long FFT_output[1024];
int main(void)
{	
	//LCD 初始化
	ILI9341_Init ();         
	ADCx_Init();
	DAC_Mode_Init();
	Key_GPIO_Config();
	//USART_Config();  
 //其中0、3、5、6 模式适合从左至右显示文字，
 //不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果			
 //其中 6 模式为大部分液晶例程的默认显示方向  
	ILI9341_GramScan ( DIRECT );
	LCD_SetFont(&Font8x16);//设置字体类型
  LCD_SetBackColor(BLACK);//设置背景颜色
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
	
	
	
	while ( 1 )
	{	

		int i=0,j;

		Trigger();
		Disp();
		if(i==50||i==0)
		{
		
			LCD_SetTextColor(BLACK);
			for(j=0 ; j<513 ; j++)
			{
					if(j!=0)
					ILI9341_DrawLine(20+j*0.4,160+Mag[j-1],20+(1+j)*0.4,160+Mag[j]);
			}
			measure();
			fft();
			XY_F();
			i=0;
	}
	}
		
}
/*"当"字符的字模16x16 */ 	  
unsigned char charater_matrix[] =    
{ /*"当",0*/
0x01,0x00,0x21,0x08,0x11,0x08,0x09,0x10,0x09,0x20,0x01,0x00,0x7F,0xF8,0x00,	0x08,
0x00,0x08,0x00,0x08,0x3F,0xF8,0x00,0x08,0x00,0x08,0x00,0x08,0x7F,0xF8,0x00,0x08,

};   
  



int Trigger()
{
	for(T_point=0 ; T_point<NP/5 ; T_point++)
	{
		if((ADC_ConvertedValue[T_point-1]<T_value)&&(ADC_ConvertedValue[T_point+1]>T_value))
		{
			break;
		}
	}
	return T_point;
	
}
void Disp()
{
	
	int i;
	ILI9341_GramScan ( DIRECT );
	for(i=0 ; i<360 ; i++)//241
	{		
		ADC_ConvertedValueLocal =(float) ADC_ConvertedValue[i]/4096*3.3; // 读取转换的AD值
	/*使用c标准库把变量转化成字符串*/
		sprintf(dispBuff,"VOL%.2f",ADC_ConvertedValueLocal);
	ILI9341_DispStringLine_EN_CH(LINE(9),dispBuff);		
	disp_value[i]=(ADC_ConvertedValue[i+T_point]/35);
		
		LCD_SetTextColor(GREEN);
		
		if(i!=0)
		ILI9341_DrawLine(i-1,120 - disp_value[i-1],i,120 - disp_value[i] );
		
		if(i==320)
		{
				LCD_SetTextColor(BLACK);
			for(i=0 ; i<320 ; i++)
			{
				if(i!=0)
				ILI9341_DrawLine(i-1,120- disp_value[i-1],i,120 - disp_value[i]);
			}
			
			XY_T();
			DMA_Cmd(ADC_DMA_CHANNEL , DISABLE);
			DMA_SetCurrDataCounter(ADC_DMA_CHANNEL,NP);
			DMA_Cmd(ADC_DMA_CHANNEL , ENABLE);	
		}
	}
}
void XY_T()
{
	int x=0,y=0;
	ILI9341_GramScan ( DIRECT );
	LCD_SetTextColor(BLUE);
	for(y=0 ; y<=130 ; y=y+20)
	ILI9341_DrawDottedLine_(1,y,240,y);

	for(x=0 ; x<=240 ; x=x+20)
	ILI9341_DrawDottedLine_(x+20,0,x+20,100);
}
void measure()
{
	uint16_t max,min,i;
	float dispvalue_max,dispvalue_min,F,A=3.3,B=4096,fs=176.471;
	int min_n,max_n,F_n;
	ILI9341_GramScan ( DIRECT );

	for(i=NP/10 ; i<NP/5 ; i++)
	{
		if(i==NP/10)
		{
			max_n=0;
			min_n=0;
			max=ADC_ConvertedValue[i];
			min=ADC_ConvertedValue[i];
		}
		if(max<ADC_ConvertedValue[i])
		{
			max=ADC_ConvertedValue[i];
			max_n=i;
		}
		if(min>ADC_ConvertedValue[i])
		{
			min=ADC_ConvertedValue[i];
			min_n=i;

		}

	}
	
		//显示
	
		dispvalue_max =(float) ADC_ConvertedValue[max_n]/B*A;
		dispvalue_min =(float) ADC_ConvertedValue[min_n]/B*A;
		LCD_SetFont(&Font8x16);
		LCD_SetTextColor(GREEN);
	  sprintf(dispBuff,"Vmax(V):%.2f",dispvalue_max);
		ILI9341_DispStringLine_EN(LINE(10),dispBuff);
		sprintf(dispBuff,"Vmin(V):%.2f",dispvalue_min);
		ILI9341_DispStringLine_EN(LINE(11),dispBuff);
		sprintf(dispBuff,"Vave(V):%.2f",(dispvalue_min+dispvalue_max)/2);
		ILI9341_DispStringLine_EN(LINE(12),dispBuff);
		F=Mag[1];
		F_n=0;
		for(i=1 ; i<512 ; i++)
	{
		if(F<Mag[i])
		{
			F=Mag[i];
			F_n=i;
		}
	}
		sprintf(dispBuff,"Fre(Khz):% .2f",(F_n)*0.175);
		ILI9341_DispStringLine_EN(LINE(13),dispBuff);
	
	
}
void XY_F()
{

	ILI9341_GramScan ( DIRECT );
}
void fft()
{
	int i;
	signed short lX,lY;
	float X,Y;
	ILI9341_GramScan ( DIRECT );
	for(i=0 ; i<1024 ; i++)
	{
		FFT_input[i]=ADC_ConvertedValue[i]<<16;
	}
	
	cr4_fft_1024_stm32(FFT_output,FFT_input,1024);

	for(i=0 ; i<513 ; i++)
	{
		lX  = (FFT_output[i] << 16) >> 16;
    lY  = (FFT_output[i] >> 16);
    X = 1024 * ((float)lX);
    Y = 1024 * ((float)lY);
    Mag[i]= sqrt(X * X + Y * Y) / 4096;
		if(Mag[i]>80)
			Mag[i]=80;

		
	}
}
void delay_us(uint32_t us)
{
	uint32_t num,t;
	for(num = 0;num < us;num++)
	{
		t = 11;
		while(t != 0)
		{
			t--;
		}
	}
}
void delay_ms(uint32_t ms)
{
	uint32_t num;
	for(num = 0; num < ms;num++)
	{
		delay_us(1000);
	}
}

		
/* ------------------------------------------end of file---------------------------------------- */

