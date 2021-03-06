
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_hal.h"

/* USER CODE BEGIN Includes */
#include "MY_NRF24.h"
#include "stdbool.h"
#define BIAS_LEVEL 127
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
unsigned char buffer[3];

void dacOutput(char eightBitValue)
{

	//int twelveBitValue = eightBitValue * ((1 << 12) - 1) / ((1 << 8) - 1);
	int twelveBitValue = eightBitValue << 4 | eightBitValue >> 4;
	buffer[0] = 0x40;
	buffer[1] = twelveBitValue>>4;
	buffer[2] = twelveBitValue<<4;
	HAL_I2C_Master_Transmit(&hi2c1, 0x61<<1, buffer, 3, 50);
}


char flashRead(int location)
{
	uint8_t spiTxBuf[4], spiRxBuf[1];

	HAL_GPIO_WritePin(GPIOA, Flash_CS_Pin, RESET);

	spiTxBuf[0] = 0x03;
	spiTxBuf[1] = location>>16; //first byte of address
	spiTxBuf[2] = location>>8; //second byte of address
	spiTxBuf[3] = location; //third byte of address
	HAL_SPI_Transmit(&hspi1, spiTxBuf, 4, 10);
	HAL_SPI_Receive(&hspi1, spiRxBuf, 1, 10);
	//HAL_SPI_TransmitReceive(&hspi1, spiTxBuf, spiRxBuf, 4, 50);
	HAL_GPIO_WritePin(GPIOA, Flash_CS_Pin, SET);
	return spiRxBuf[0];
}


char rnd_number(uint8_t lower, uint8_t upper)
{
	srand(HAL_GetTick());
	return (rand() % (upper - lower + 1)) + lower;
}


void debias_speaker()
{
	for (int i = BIAS_LEVEL; i < 0; i--)
	{
		dacOutput(i);
	}
}

void playTrack(uint8_t track)
{
	static const unsigned int starts[25] = {
		0x005A16,
		0x00AD4A,
		0x00FE98,
		0x013185,
		0x01AA17,
		0x01F4B7,
		0x024641,
		0x02B936,
		0x032681,
		0x03700B,
		0x03DAAB,
		0x0433A1,
		0x046F2B,
		0x049637,
		0x04B8EB,
		0x051160,
		0x056E40,
		0x0592E0,
		0x065801,
		0x069C60,
		0x06FC6A,
		0x078ECA,
		0x07F0EB,
		0x0829F5,
		0x085F95
	};

	static const unsigned int ends[25] = {
		0x00AB8D,
		0x00FC63,
		0x012F64,
		0x01A708,
		0x01F339,
		0x0243AA,
		0x02B659,
		0x032151,
		0x036C90,
		0x03D568,
		0x043107,
		0x046D29,
		0x049342,
		0x04B685,
		0x050E0C,
		0x056BDF,
		0x0590B0,
		0x06566E,
		0x069AC0,
		0x06F9D5,
		0x078BEB,
		0x07EE27,
		0x082815,
		0x085CFA,
		0x08991E
	};

	for (int i = starts[track - 1]; i < ends[track - 1]; i++)
	{
		dacOutput(flashRead(i));
	}
	debias_speaker();
}


char shots = 0;
char waiting_for_reload = 0;
char pairing = 1;


void reload()
{
	shots = 0;
    waiting_for_reload = 0;

    for (int i = 0x002699; i < 0x005739; i++)
    {
	    dacOutput(flashRead(i));
    }
    debias_speaker();
}


void shoot(int delay)
{
	shots++;
    if (shots & 1)
    {
	    HAL_GPIO_WritePin(GPIOF, Top_LED_Pin, SET);
    }
    else
    {
	    HAL_GPIO_WritePin(GPIOF, Bottom_LED_Pin, SET);
    }


    for (int i = 0x00002c; i < 0x002474; i++)
    {
	    dacOutput(flashRead(i));
	    if (i == 0x0002c + delay)
	    {
		    HAL_GPIO_WritePin(GPIOF, Top_LED_Pin|Bottom_LED_Pin, RESET);
	    }
    }
    debias_speaker();
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  char rxData[52];
  char step = 1;
  char master = 0;
  char channel_buffer[32];
  char channel_number;

  char track_buffer[32];
  char track_number;
  uint32_t last_time;



  /* Speaker Test
	for (int i = 0x00002c; i < 0x002474; i++)
	{
	  dacOutput(flashRead(i));
	}
	*/
	/*
	for (int i = 0x002699; i < 0x005739; i++)
	{
	  dacOutput(flashRead(i));
	}
	*/

	/* Init NRF24 */
	HAL_GPIO_WritePin(GPIOA, Flash_CS_Pin, SET);
	HAL_GPIO_WritePin(GPIOA, Radio_CS_Pin, RESET);

	HAL_Delay(100);

	NRF24_begin(GPIOA, Radio_CS_Pin, NULL, hspi1);

	NRF24_setAutoAck(true);
	NRF24_setChannel(0);
	NRF24_setPayloadSize(32);

	NRF24_stopListening();
	NRF24_openWritingPipe(0x11223344AA);
	NRF24_openReadingPipe(1, 0x11223344AA);

	last_time = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (pairing)
	  {
		  /* This if block handles the "pairing blink" of the status laser */
		  if (HAL_GetTick() - last_time > 250)
		  {
			  last_time = HAL_GetTick();
			  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
		  }

		  if (step == 1)
		  {
			  /* Try a shitload of times to broadcast successfully */
			  for (int i = 0; i < 100; i++)
			  {
				  if (NRF24_write("here", 32))
				  {
					  /* There's already a gun ready and waiting; Assume Slave */
					  master = 0;
					  i = 100;
				  }
				  else
				  {
					  /* Nobody here; Assume Master */
					  master = 1;
				  }
			  }

			  /* Attempts at broadcasting "here" complete, start listening */
			  NRF24_startListening();

			  /* Advance to next step */
			  step++;
		  }

		  if (step == 2)
		  {
			  if (master)
			  {
				  /* Listen for Slave "here" */
				  if (NRF24_available())
				  {
					  NRF24_read(rxData, 32);
					  if (strcmp(rxData, "here") == 0)
					  {
						  channel_number = rnd_number(1, 124);
						  itoa(channel_number, channel_buffer, 10);
						  step++;
					  }
				  }
			  }
			  else
			  {
				  step++;
			  }
		  }

		  if (step == 3)
		  {
			  if (master)
			  {
				  NRF24_stopListening();

				  for (int i = 0; i < 100; i++)
				  {
					  if (NRF24_write(channel_buffer, 32))
					  {
						  /* Finish */

						  /* Set Channel to picked channel */
						  NRF24_setChannel(channel_number);
						  pairing = 0;
						  i = 100;
					  }
				  }
			  }
			  else
			  {
				  if (NRF24_available())
				  {
					  NRF24_read(rxData, 32);

					  NRF24_setChannel(atoi(rxData));
					  NRF24_stopListening();
					  pairing = 0;
				  }
			  }
		  }
	  }
	  else
	  {
		  /* Pairing done, turning status laser to solid */
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, SET);

		  if (waiting_for_reload)
		  {
			  if (master)
			  {
				  if (NRF24_write(track_buffer, 32))
				  {
					  /*
					   * At this point, the Slave has accepted our reload instruction.
					   *
					   * Both guns are now at the same point in our code, and we can assume they are perfectly synchronized.
					   */
					  reload();
					  playTrack(track_number);
				  }
			  }
			  else
			  {
				  if (NRF24_available())
				  {
					  /*
					   * At this point, the Master has broadcasted the reload instruction.
					   *
					   * Both guns are now at the same point in our code, and we can assume they are perfectly synchronized.
					   */
					  NRF24_read(rxData, 32);

					  reload();
					  playTrack(atoi(rxData));
				  }
			  }
		  }
		  else
		  {
			  /* Turn off radio for duration of shots */
			  NRF24_powerDown();


			  /* Trigger_Pin is active low */
			  if (!HAL_GPIO_ReadPin(GPIOA, Trigger_Pin))
			  {
				  /*
				   * Integer passed to shoot() function determines barrel LED flash duration.
				   *
				   * Calculation is roughly "(integer / 10) milliseconds"
				   *
				   * Thus "1000" is about 100ms
				   *
				   */
				  shoot(1000);




				  /* Personal preference, but I like this delay in here... feels better */
				  HAL_Delay(10);

				  if (shots == 4)
				  {
					  if (master)
					  {
						  /* Randomly pick, and then store track number in global variable */
						  track_number = rnd_number(0, 24);
						  itoa(track_number, track_buffer, 10);
					  }
					  else
					  {
						  /* Now we turn the radio back on, because we need to listen for the reload instruction from the Master */
						  NRF24_powerUp();
					  }
					  waiting_for_reload = 1;
				  }
			  }
		  }
	  }

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL7;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x20000203;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Analogue filter 
    */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Digital filter 
    */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**I2C Fast mode Plus enable 
    */
  __HAL_SYSCFG_FASTMODEPLUS_ENABLE(I2C_FASTMODEPLUS_I2C1);

}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, Bottom_LED_Pin|Top_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, Radio_CS_Pin|Flash_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pins : Bottom_LED_Pin Top_LED_Pin */
  GPIO_InitStruct.Pin = Bottom_LED_Pin|Top_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : Radio_CS_Pin Flash_CS_Pin */
  GPIO_InitStruct.Pin = Radio_CS_Pin|Flash_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : Trigger_Pin */
  GPIO_InitStruct.Pin = Trigger_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(Trigger_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
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
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
