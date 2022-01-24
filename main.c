/*----------------------------------------------------------------------------
 *      Main: Initialize
 *---------------------------------------------------------------------------*/

#include "MKL05Z4.h"                    	/* Device header */
#include "ADC.h"
#include "frdm_bsp.h" 
#include "tsi.h"
#include "pit.h"
#include "tpm.h"
#include "i2c.h"
#include "buttons.h"

uint16_t temp;
static uint8_t msTicks = 0;
static uint8_t newTick = 0;
uint8_t wynik_ok=0;
float	wynik,freq,slider;
static uint8_t readout = 0;
static uint8_t arrayXYZ[6];
double acc,x_grav,y_grav,grav;



void ADC0_IRQHandler() //przerwanie ADC
{	
	temp = ADC0->R[0];
	if(!wynik_ok)				
	{
		wynik = temp;			
		wynik_ok=1;
	}
	NVIC_ClearPendingIRQ(ADC0_IRQn);
}

void delay_ms( int n) {  //funkcja opoznienia
volatile int i;
volatile int j;
for( i = 0 ; i < n; i++)
for(j = 0; j < 3500; j++) {}
}
 
int main(void) {

	SysTick_Config(SystemCoreClock / 420);
	TPM0_Init_PWM ();
	buttonsInitialize();
	TSI_Init ();	
	I2C_Init();
	I2C_WriteReg(0x1D, 0x2A, 0x01);
	freq = 1; //poczatkowa wartosc czestotliwosci
		

uint8_t	kal_error;
kal_error=ADC_Init();		// Inicjalizacja i kalibracja przetwornika A/C
	if(kal_error)
	{
		while(1);							// Klaibracja sie nie powiodla
	}
	ADC0->SC1[0] = ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(0);
	
	while(1){
					
				
				if(GetMode() == 0)			//tryb 0
				{
					if( msTicks%2 == 0 ) {
						wynik = TSI_ReadSlider();
						if (wynik > 0) {
							TPM0_SetVal(wynik);  //wynik ze slidera wprost do PWM
						}
					}
				}				 
				else if(GetMode() == 1) //tryb 1
				{						
					if( msTicks%2 == 0 ) {
						slider = TSI_ReadSlider()/11+1;	//odpowiednio przeskalowana wartosc sczytuje ze slidera
						if (slider > 1) {
							if (freq != slider) //jesli slider sczytuje nowa wartosc podmien wartosc freq na slider
								{freq=slider;}						
						}
							TPM0_SetVal(100);
							delay_ms(500/freq);	//opoznienie zalezne od freq
							TPM0_SetVal(1);
							delay_ms(500/freq);
						}				
				}
				 
				else if(GetMode()>= 2) //tryb 2 i 3
				{	
				if (newTick)
					{
					newTick = 0;					
					wynik = 100-temp/40;  //wynik to wartosc z ADC odpowiednio podzielona, aby w przyblizeniu otrzymac dla wartosci z przetwornika ADC od 0 do 4095 otrzymywac liczby od 100 do 0
					
					if(GetMode()== 3){ //tryb 3
					I2C_ReadRegBlock(0x1D, 0x01, 6, arrayXYZ);
					acc = -2*((double)((int16_t)((arrayXYZ[4]<<8)|arrayXYZ[5])>>2)/4096)*(GetColor()-0.5);	//sczytanie watosci przyspieszenia z osi Z
					y_grav = ((double)((int16_t)((arrayXYZ[2]<<8)|arrayXYZ[3])>>2)/4096);		//sczytanie osi x i y do usuniecia efektu grawitacji
					x_grav = ((double)((int16_t)((arrayXYZ[0]<<8)|arrayXYZ[1])>>2)/4096);
					grav = x_grav*x_grav + y_grav*y_grav;  					
						//wartosc acc zalezna od zmiennej color, poniewaz swiatlo zamontowane z tylu lub przodu ma inny zwrot osi Z			
					if(acc>0.4 && grav > 0.9) //wartosc progowa od ktorej rozpoznawane jest hamowanie oraz gdy w wypadkowy wektor z XY jest wiekszy od 0.9 (sqrt(0.4*0.4+0.9) > 1)
					{			 
						TPM0_SetVal(wynik);
						delay_ms(100/acc); //im wieksze hamowanie tym czesciej miga
						TPM0_SetVal(1);
						delay_ms(100/acc);				
					}
					else
						TPM0_SetVal(wynik);
					}
					else
						TPM0_SetVal(wynik); //jesli tryb 2 to po prostu przepisuje wartosc z ADC do PWM
				}
			}
	}
}

void SysTick_Handler(void) {
	msTicks++;
	newTick = 1;}





