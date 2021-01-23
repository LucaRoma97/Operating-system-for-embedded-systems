#include <lib_def.h>
#include "bsp_adc_assignment.h"
#include "S32K144.h"
#include <os.h>

extern CPU_INT16U ADC0_adc_chx;
extern OS_SEM ADC0sem;

void BSP_ADC0_init_interrupt(void) {
  PCC->PCCn[PCC_ADC0_INDEX] &= ~PCC_PCCn_CGC_MASK;
  PCC->PCCn[PCC_ADC0_INDEX] |= PCC_PCCn_PCS(1);
  PCC->PCCn[PCC_ADC0_INDEX] |= PCC_PCCn_CGC_MASK;
  ADC0->SC1[0] = 0x00007F;
  ADC0->CFG1 = 0x000000004;
  ADC0->CFG2 = 0x00000000C;
  ADC0->SC2 = 0x00000000;
  ADC0->SC3 = 0x00000000;

  BSP_IntVectSet(ADC0_IRQn,0,0,ADC0_IRQHandler);
  BSP_IntEnable(ADC0_IRQn);

}

void BSP_ADC0_convertAdcChan_interrupt(CPU_INT16U adcChan) {
  ADC0->SC1[0] &= ~ADC_SC1_ADCH_MASK;
  ADC0->SC1[0] = ADC_SC1_ADCH(adcChan) | 0x40;
}

void ADC0_IRQHandler(void) {
  CPU_INT16U adc_result = 0;
  OS_ERR os_err;

  OSIntEnter(); /* all interrputs diabled */

  adc_result=ADC0->R[0];

  ADC0_adc_chx = (CPU_INT16U) ((4095*adc_result)/0xFFF);

  ADC0->SC1[0] = 0x00007F;

  OSSemPost(&ADC0sem, OS_OPT_POST_1 | OS_OPT_POST_NO_SCHED, &os_err);
  OSIntExit(); /* all interrupts enabled again */
}
