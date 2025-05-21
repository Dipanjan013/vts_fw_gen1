#include "port_gpio.h"

__attribute__((weak)) void port_gpio_Callback(port_gpio_pin_e pin)
{

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	port_gpio_Callback(GPIO_Pin);
}

port_gpio_fnStatus_e port_gpio_Init(port_gpio_port_t port, port_gpio_pin_e pin, port_gpio_mode_e mode, uint8_t enableIrq)
{
	GPIO_InitTypeDef config = {0};
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : LED_Pin */
	GPIO_InitStruct.Pin = LED_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);
}

void port_gpio_WritePin(port_gpio_port_t port, port_gpio_pin_e pin, port_gpio_state_e state)
{

}

port_gpio_state_e port_gpio_ReadPin(port_gpio_port_t port, port_gpio_pin_e pin)
{

}

void port_gpio_TogglePin(port_gpio_port_t port, port_gpio_pin_e pin)
{

}
