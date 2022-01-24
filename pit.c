/******************************************************************************
 * This file is a part of the Sysytem Microprocessor Tutorial (C).            *
 ******************************************************************************/

/**
 * @file pit.c
 * @author Koryciak
 * @date Nov 2020
 * @brief File containing definitions for PIT.
 * @ver 0.1
 */
#include "MKL05Z4.h" 
#include "pit.h"

/******************************************************************************\
* Private definitions
\******************************************************************************/

/******************************************************************************\
* Private prototypes
\******************************************************************************/
void PIT_IRQHandler(void);
/******************************************************************************\
* Private memory declarations
\******************************************************************************/
static uint8_t pitEnabled = 0;

void PIT_Init_Generator(void) {

	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK; /* Enable PIT mask in SCGC6 register */
	
	PIT->MCR &= ~PIT_MCR_MDIS_MASK; /* Disable MDIS mask in MCR register */
	PIT->MCR |= PIT_MCR_FRZ_MASK;   /* Enable FRZ mask in MCR register */
	 
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(40000*50); /* count down from */
	
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK; /* Generate interrupts */

  NVIC_SetPriority(PIT_IRQn, 3); // 0, 1, 2 or 3 /* PIT interrupt priority level  */

	NVIC_ClearPendingIRQ(PIT_IRQn); 
	NVIC_EnableIRQ(PIT_IRQn);	/* Enable Interrupts */
	
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK; /* Enable counter */
	
	pitEnabled = 1;

}

void PIT_SetTSV(uint32_t period) {

	if (pitEnabled) PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(40000*period); /* count down from */
}
/**
 * @brief Interrupt handler for PIT.
 */
void PIT_IRQHandler(void) {

	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
		PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK; /* clear status flag */
	}
}
