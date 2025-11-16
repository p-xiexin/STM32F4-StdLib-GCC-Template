#include "main.h"
#include "led.h"
#include "stm32f4xx.h"                  // Device header


#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

extern void xPortSysTickHandler(void);
void SysTick_Handler(void)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        xPortSysTickHandler();
    }
}

#define START_TASK_PRIO 1
#define START_STK_SIZE 512
static TaskHandle_t StartTask_Handler;

#define LED0_TASK_PRIO 5
#define LED0_TASK_SIZE 128
static TaskHandle_t LED0Task_Handler;

#define LED1_TASK_PRIO 6
#define LED1_STK_SIZE 128
TaskHandle_t LED1Task_Handler;

void led0_task(void *pvParameters)
{
    while(1)
    {
        led_blue_toggle();
        vTaskDelay(1000);
    }
}   

void led1_task(void *pvParameters)
{
    while(1)
    {
        led_green_toggle();
        vTaskDelay(500);
    }
}

void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();

    xTaskCreate((TaskFunction_t)led0_task,
                (const char *)"LEDBlue_Task",
                (uint16_t)LED0_TASK_SIZE,
                (void *)NULL,
                (UBaseType_t)LED0_TASK_PRIO,
                (TaskHandle_t *)&LED0Task_Handler);

    xTaskCreate((TaskFunction_t)led1_task,
                (const char *)"LEDGreen_task",
                (uint16_t)LED1_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)LED1_TASK_PRIO,
                (TaskHandle_t *)&LED1Task_Handler);

    vTaskDelete(StartTask_Handler); 
    taskEXIT_CRITICAL();            
}

void startTast(void)
{
    xTaskCreate((TaskFunction_t)start_task,         
                (const char *)"start_task",         
                (uint16_t)START_STK_SIZE,           
                (void *)NULL,                       
                (UBaseType_t)START_TASK_PRIO,       
                (TaskHandle_t *)&StartTask_Handler);
}


int main(void)
{
	led_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    
	startTast();
	vTaskStartScheduler();
	
	while(1)
	{
		led_red_on();
	}
}
