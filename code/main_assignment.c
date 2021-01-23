#include <cpu.h>
#include <lib_mem.h>
#include <os.h>
#include <bsp_os.h>
#include <bsp_clk.h>
#include <bsp_int.h>
#include <S32K144.h>
// #include <bsp_led.h> /* ci servirà quando aggiungeremo il LED */

#include "os_app_hooks.h"
#include "../app_cfg.h"

#include <bsp_adc_assignment.h>
#include <bsp_pwm_assignment.h>
#include <bsp_switch_assignment.h>

static OS_TCB StartupTaskTCB;
static CPU_STK StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE];

// CPUINT FTM_SC_PWMEN0_MASK;
volatile CPU_INT16U ADC0_result;
volatile CPU_INT16U ADC0_adc_chx;
OS_SEM ADC0sem;
extern CPU_INT08U SW2_status;
extern CPU_INT08U SW3_status;


static void StartupTask(void *p_arg);

int main(void)
{
	OS_ERR os_err;
  /* deve andare fuori dal main??? (OS_ERR os_err */
  BSP_ClkInit();
  BSP_IntInit();
  BSP_OS_TickInit();

  Mem_Init();
  CPU_IntDis();
  CPU_Init();

  OSInit(&os_err);
  if (os_err != OS_ERR_NONE) {
    while (1);
  }

  App_OS_SetAllHooks();

  OSTaskCreate(&StartupTaskTCB,
	       "Startup Task",
	       StartupTask,
	       0u,
	       APP_CFG_STARTUP_TASK_PRIO,
	       &StartupTaskStk[0u],
	       StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE / 10u],
	       APP_CFG_STARTUP_TASK_STK_SIZE,
	       0u,
	       0u,
	       0u,
	       (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
	       &os_err);
  if (os_err |= OS_ERR_NONE) {
    while (1);
  }

  OSStart(&os_err);

  while (DEF_ON) {
    ;
  }
}


static void StartupTask (void *p_arg)
{

  OS_ERR os_err;
  (void)p_arg;

  OS_TRACE_INIT();

  BSP_OS_TickEnable();

  #if OS_CFG_STAT_TASK_EN > 0u
  OSStatTaskCPUUsageInit(&os_err);
  #endif

  #ifdef CPU_CFG_INT_DIS_MEAS_EN
  CPU_IntDisMeasMaxCurReset();
  #endif

  /*
****************************************************

DO WE HAVE TO INITIALIZE THE RGB LEDS WITH LED_Init ???
di sicuro switch init poi ci servirà???

****************************************************
*/

  OSSemCreate(&ADC0sem, "ADC0 Semaphore", 0u, &os_err);

  BSP_ADC0_init_interrupt(); /* initialization of the ADC0 */
  
  BSP_FTM0_CH0_PWM_Init(); /* initialization of the PWM */

 // FTM0->SC |= FTM_SC_PWMEN0_MASK; /* we try to switch on the LED RED */

  BSP_Switch_Init(); /* initialization of the switch */ /*it swotch on all led*/

  while (DEF_TRUE) {
    
    BSP_ADC0_convertAdcChan_interrupt(12); /* start conversion with ADC0 */

    /* change of colour */
      if(SW2_status)
      {
        BSP_CH_switching_2();
        SW2_status = 0;
      }

      if(SW3_status)
      {
        BSP_CH_switching_3();
        SW3_status = 0;
      }

    /* updating of the duty-cycle */

    OSSemPend(&ADC0sem, 0u, OS_OPT_PEND_BLOCKING, 0u, &os_err);

    BSP_FTM0_CH0_PWM_CnV_Update();
  }
}
