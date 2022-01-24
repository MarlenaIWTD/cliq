
#include "tpm.h"

void TPM1_IRQHandler(void);

static uint8_t tpm0Enabled = 0;
static int8_t color = 1; //1 bialy, 0 czerwony
int8_t mode =0;

void TPM0_Init_PWM(void) {
		
SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;		
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);  

	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; 
	PORTB->PCR[8] = PORT_PCR_MUX(2);  //trzy diody RGB
	PORTB->PCR[9] = PORT_PCR_MUX(2);
	PORTB->PCR[10] = PORT_PCR_MUX(2);	

	TPM0->SC |= TPM_SC_PS(7);  					
	TPM0->SC |= TPM_SC_CMOD(1);					

	TPM0->MOD = 98; 										
	
	TPM0->SC &= ~TPM_SC_CPWMS_MASK;
	TPM0->CONTROLS[3].CnSC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK); //trzy diody RGB
	TPM0->CONTROLS[2].CnSC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK);	
	TPM0->CONTROLS[1].CnSC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK);

	TPM0->CONTROLS[3].CnV = 50;  //trzy diody RGB
	TPM0->CONTROLS[2].CnV = 50;	
	TPM0->CONTROLS[1].CnV = 50;
	tpm0Enabled = 1;  
}

void ChangeColor(void)
	{
		if(color == 1)  //zmiany miedzy 0 a 1
			{color--;}
			else color++;
		if(color == 0)  //jesli przelaczamy na czerwony kolor, zresetuj diody GB
			{
				G_BReset();
			}
	}
	
int8_t GetColor(void) //getter koloru
{
	return color;
}

void TPM0_SetVal(uint32_t value) {  //setting wartosci PWM dla wszystkich diod
	value = value - 1; 				
	if (tpm0Enabled) TPM0->CONTROLS[3].CnV = value; //czerwona
	if (tpm0Enabled  && color) TPM0->CONTROLS[2].CnV = value;  //setting diod	GB zalezy od wartosci zmiennej color
	if (tpm0Enabled  && color) TPM0->CONTROLS[1].CnV = value;
}

void G_BReset (void)  //bezwzlegdny reset diod GB
{
	if (tpm0Enabled ) TPM0->CONTROLS[2].CnV = 0;	
	if (tpm0Enabled ) TPM0->CONTROLS[1].CnV = 0;
}

void ChangeMode(void)  //zmien
{											 //0-zmiana jasnosci diody,swiecenie ciagle
	if(mode==3)					 //1-zmiana czestotliwosci migania diody	
	{mode = 0;}				   //2-jasnosc zalezna od czujnika swiatla
	else mode++;				 //3-miganie kiedy wykryje hamowanie + tryb 2
}

int8_t GetMode(void)		//getter trybu
{
	return mode;
}


