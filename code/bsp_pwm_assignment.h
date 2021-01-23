#ifndef BSP_TIMER_PRESENT
#define BSP_TIMER_PRESENT

#ifdef _cplusplus
extern "C" {
  #endif

  extern volatile CPU_INT32S FTM0_CH0_period; /* da chiedere */

  void BSP_FTM0_CH0_PWM_Init(void);
  void BSP_FTM0_CH0_PWM_CnV_Update(void);

  #ifdef _cplusplus
}
#endif

#endif
