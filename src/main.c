#include "main.h"
#include "vrs_cv5.h"

uint8_t status = 0;
char receivedChar;
char ch[10];

uint8_t pom = 0;
RCC_ClocksTypeDef RCC_Clocks;

uint32_t adcValue;
uint8_t switchMode = 0;

int main(void) {

	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);

	adc_init();
	NVICInit();
	UART_init();
	//enable interrupt
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
	//enable USART
	USART_Cmd(USART1, ENABLE);

	USART_puts("init\r");
	char str[10];
	//Initialize string to zero
	memset(str, 0, 10);

	float voltValue = 0.00;
	while (1) {

		if (receivedChar == 'm') {
			if (switchMode == 0) {
				sprintf(str, "%u\r", adcValue);
				switchMode = 1;
			} else {
				voltValue = (adcValue * 3.3) / 4096; //koli debilnym doskam a baudrate
				sprintf(str, "%.2fV\r", voltValue);
				switchMode = 0;
			}

			receivedChar = 0;
			USART_puts(str);
		}

	}
}

void ADC1_IRQHandler(void) {
	if (ADC1->SR & ADC_SR_EOC) {
		adcValue = ADC1->DR;
	}
}

void USART1_IRQHandler(void) {
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		receivedChar = USART_ReceiveData(USART1);
	}else if(USART_GetITStatus(USART2, USART_IT_TC) != RESET){
		USART_ClearITPendingBit(USART2, USART_IT_TC);
	}
}

void USART_puts(volatile char *s) {

	while (*s) {
		// wait until data register is empty
		while (!(USART1->SR & 0x00000040))
			;
		USART_SendData(USART1, *s);
		*s++;
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
