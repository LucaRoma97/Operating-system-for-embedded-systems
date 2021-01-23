#include <lib_def.h>
#include <bsp_int.h>
#include <os.h>
#include "S32K144.h"
#include <bsp_adc_assignment.h>

/*void FTM0_IRQHandler_Single_Edge(void); da chiedere no */

volatile CPU_INT32S FTM0_CH0_period;
extern CPU_INT16U ADC0_adc_chx; /* variable to get the result in the main from the PWM */
extern OS_SEM ADC0sem;
OS_ERR os_err;

void BSP_FTM0_CH0_PWM_Init(void)
{
  
  PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK;
  PCC->PCCn[PCC_FTM0_INDEX] = PCC_PCCn_PCS(6) | PCC_PCCn_CGC_MASK;
  
  PORTD->PCR[15] = PORT_PCR_MUX(2); 
  PORTD->PCR[16] = PORT_PCR_MUX(2); /* */
  PORTD->PCR[0] = PORT_PCR_MUX(2); 

  FTM0->MODE = FTM_MODE_FTMEN_MASK;

  FTM0->MOD = FTM_MOD_MOD(8000-1);

  FTM0->CNTIN = FTM_CNTIN_INIT(0);

  /*Enable high true pulse of PWM signals */
  FTM0->CONTROLS[0].CnSC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
  FTM0->CONTROLS[1].CnSC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
  FTM0->CONTROLS[2].CnSC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;

  BSP_ADC0_convertAdcChan_interrupt(12);

  /* in the main we need to create the semaphore */
  OSSemPend(&ADC0sem, 0u, OS_OPT_PEND_BLOCKING, 0u, &os_err);

  /*Set channel value in initialization stage */
  FTM0->CONTROLS[0].CnV = FTM_CnV_VAL((8000-1)*(4095-ADC0_adc_chx)/0xFFF); /* the read duty cycle */
  FTM0->CONTROLS[1].CnV = FTM_CnV_VAL((8000-1)*(4095-ADC0_adc_chx)/0xFFF);
  FTM0->CONTROLS[2].CnV = FTM_CnV_VAL((8000-1)*(4095-ADC0_adc_chx)/0xFFF);
  FTM0->CNT = 0;

  FTM0->SC = FTM_SC_CLKS(1) | FTM_SC_PS(0) | FTM_SC_PWMEN0_MASK; /* clock selection and enabling PWM generation */

  //FTM0->SC = FTM_SC_CLKS(1) | FTM_SC_PS(7); /* cloch selection*/

  FTM0->MODE |= FTM_MODE_FTMEN_MASK; /* 0x1u DO WE HAVE TO MAKE A BOOLEAN OPERATION OR NOT?  */

  FTM0->MODE &= 0xF7; /* to clear the PWMSYNC bit of the MODE register */

  FTM0->SYNCONF |= 0x00000080; /* to set the bit SYNCMODE of FTM_SYNCONF register */

  FTM0->SYNCONF |= 0x00000005; /* to set the CNTINC bit of FTM_SYNCONF register */

  FTM0->COMBINE |= 0x00002020; /*to set  SYNCEN0 bit of the COMBINE register */
}

void BSP_FTM0_CH0_PWM_CnV_Update(void)
{

  FTM0->CONTROLS[0].CnV = FTM_CnV_VAL((8000-1)*(4095-ADC0_adc_chx)/0xFFF);
  FTM0->CONTROLS[1].CnV = FTM_CnV_VAL((8000-1)*(4095-ADC0_adc_chx)/0xFFF);
  FTM0->CONTROLS[2].CnV = FTM_CnV_VAL((8000-1)*(4095-ADC0_adc_chx)/0xFFF);

  FTM0->PWMLOAD |= 0x200; /* to set the LDOK bit of the PWMLOAD register */
}
