/**
  ******************************************************************************
  * @file    USB_Device/HID_Standalone/Src/main.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    09-October-2015
  * @brief   USB device HID demo main file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CURSOR_STEP     5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USBD_HandleTypeDef USBD_Device;
extern PCD_HandleTypeDef hpcd;
uint8_t HID_Buffer[8] = {0,4,0,0,0,0,0,0};

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void GetPointerData(uint8_t *pbuf);

/* Private functions ---------------------------------------------------------*/
#define UID_BASE                     0x1FFF7A10U           /*!< Unique device ID register base address */
/*****************************************************
**函数功能：获取STM32芯片ID号
**参	数：ID号为96位12Byte长度，所以必须定义为数组元素为u32类型且长度大于3
**返回	值：0-----------获取成功
**			1-----------获取失败
******************************************************/
uint8_t Get_ChipID(uint32_t* CPU_ID)
{
//	u32 Dev_ID_Addr0,Dev_ID_Addr1,Dev_ID_Addr2;
	
	//获取芯片ID
//	CPU_ID[0]=*(volatile uint32_t*)(0x1fff7a10);
//	CPU_ID[1]=*(volatile uint32_t*)(0x1fff7a14);
//	CPU_ID[2]=*(volatile uint32_t*)(0x1fff7a18);
	CPU_ID[0] = (uint32_t)(READ_REG(*((uint32_t *)UID_BASE)));
	CPU_ID[1] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
	CPU_ID[2] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 8U))));
	
	if((CPU_ID[0] != 0)&&(CPU_ID[1] != 0)&&(CPU_ID[2] != 0))
		return 0;
	
	return 1;
}

void HAL_GetUID(uint32_t *UID)
{
	UID[0] = (uint32_t)(READ_REG(*((uint32_t *)UID_BASE)));
	UID[1] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
	UID[2] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 8U))));
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	uint32_t CPU_ID[4] = {0};
	/* STM32F469xx HAL library initialization */
	HAL_Init();

	/* Configure the System clock to have a frequency of 180 MHz */
	SystemClock_Config();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/* Configure LED1, LED2, LED3 and LED4 */
	BSP_LED_Init(LED1);
	BSP_LED_Init(LED2);
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);
	
	BSP_LED_On(LED2);
	
	printf("Hello USB HID Function!\r\n");
	printf("My name is Wade!\r\n");
	
	if(!Get_ChipID(CPU_ID))
	{
		printf("ChipID: [0] = 0x%8x [1] = 0x%8x [2] = 0x%8x\n",CPU_ID[0],CPU_ID[1],CPU_ID[2]);
	}
	else printf("Get ChipID error\r\n");
	
//	HAL_GetUID(CPU_ID);
	printf("ChipID: [0] = 0x%8x [1] = 0x%8x [2] = 0x%8x\n",CPU_ID[0],CPU_ID[1],CPU_ID[2]);

#if 0  
/* Configure Key button for remote wakeup */
	BSP_PB_Init(BUTTON_WAKEUP, BUTTON_MODE_EXTI);

/* Init Device Library */
	USBD_Init(&USBD_Device, &HID_Desc, 0);

/* Add Supported Class */
	USBD_RegisterClass(&USBD_Device, USBD_HID_CLASS);

/* Start Device Process */
	USBD_Start(&USBD_Device);
#endif
	SetupKeyboard();
	BSP_PB_Init(BUTTON_WAKEUP, BUTTON_MODE_GPIO);

#if 1
	int flashDelay = 0;
	while (1)
	{
		UpdateKeyboard();

		if (flashDelay >= 60000)
		{
			flashDelay = 0;
			BSP_LED_Toggle(LED1);

			BSP_LED_Toggle(LED4);
		}

		if (!HAL_GPIO_ReadPin(keys[0].Pin.Port, keys[0].Pin.Pin))
		{
			BSP_LED_On(LED2);
			BSP_LED_Off(LED3);
		}
		else
		{
			BSP_LED_Off(LED2);
			BSP_LED_On(LED3);
		}

		flashDelay += 1;
	}
#else
int flashDelay = 0;
	while (1)
	{
		UpdateKeyboard();

		if (flashDelay >= 10000)
		{
			flashDelay = 0;
			BSP_LED_Toggle(LED1);

			BSP_LED_Toggle(LED4);
		}

		for (int i = 0; i < keyCount; i++)
		{
			if (!HAL_GPIO_ReadPin(keys[i].Pin.Port, keys[i].Pin.Pin))
			{
				BSP_LED_On(LED2);
				BSP_LED_Off(LED3);
			}
			else
			{
				BSP_LED_Off(LED2);
				BSP_LED_On(LED3);
			}
		}

		flashDelay += 1;
	}
#endif
#if 0
while (1)
  {
    HAL_Delay(100);
    
    /* Toggle LEDs  */
    BSP_LED_Toggle(LED1);
    BSP_LED_Toggle(LED2);
    BSP_LED_Toggle(LED3);
    BSP_LED_Toggle(LED4);
    HAL_Delay(100);  
 //   GetPointerData(HID_Buffer);
    USBD_HID_SendReport(&USBD_Device, HID_Buffer, 8);
  }
#endif
}

/**
  * @brief  Gets Pointer Data.
  * @param  pbuf: Pointer to report
  * @retval None
  */
void GetPointerData(uint8_t *pbuf)
{
  static int8_t cnt = 0;
  int8_t  x = 0, y = 0 ;
  
  if(cnt++ > 0)
  {
    x = CURSOR_STEP;
  }
  else
  {
    x = -CURSOR_STEP;
  }
  pbuf[0] = 0;
  pbuf[1] = x;
  pbuf[2] = y;
  pbuf[3] = 0;
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  *         The USB clock configuration from PLLSAI:
  *            PLLSAIM                        = 8
  *            PLLSAIN                        = 384
  *            PLLSAIP                        = 8
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
  
  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
#if defined(USE_STM32469I_DISCO_REVA)
  RCC_OscInitStruct.PLL.PLLM = 25;
#else
  RCC_OscInitStruct.PLL.PLLM = 8;
#endif /* USE_STM32469I_DISCO_REVA */
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 2;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Activate the OverDrive to reach the 180 MHz Frequency */  
  HAL_PWREx_EnableOverDrive();
  
  /* Select PLLSAI output as USB clock source */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CK48;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CK48CLKSOURCE_PLLSAIP;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 384;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 7; 
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV8;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

/**
  * @brief This function provides accurate delay (in milliseconds) based 
  *        on SysTick counter flag.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @param Delay: specifies the delay time length, in milliseconds.
  * @retval None
  */

void HAL_Delay(__IO uint32_t Delay)
{
  while(Delay) 
  {
    if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) 
    {
      Delay--;
    }
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
