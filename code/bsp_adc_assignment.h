#ifdef _cplusplus
extern "C" {
#endif

  void BSP_ADC0_init_interrupt(void);
  
  void BSP_ADC0_convertAdcChan_interrupt(CPU_INT16U adcChan);

  void ADC0_IRQHandler(void);

  #ifdef _cplusplus
}
#endif
