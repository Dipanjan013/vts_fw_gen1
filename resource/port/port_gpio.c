#include "port_gpio.h"

__attribute__((weak)) void port_gpio_Callback(port_gpio_pin_e pin)
{

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	port_gpio_Callback(GPIO_Pin);
}

port_gpio_fnStatus_e port_gpio_Init(port_gpio_port_t port, port_gpio_pin_e pin, port_gpio_mode_e mode, port_gpio_pull_t type)
{
	GPIO_InitTypeDef config = {0};
	/* GPIO Ports Clock Enable */
	if(port == GPIOA){
		__HAL_RCC_GPIOA_CLK_ENABLE();
	}else if(port == GPIOB){
		__HAL_RCC_GPIOB_CLK_ENABLE();
	}else if(port == GPIOC){
		__HAL_RCC_GPIOC_CLK_ENABLE();
	}else{
	}

  /*Configure GPIO pin */
	switch(mode){
		case PORT_GPIO_MODE_OUTPUT:
			HAL_GPIO_WritePin(port, pin, PORT_GPIO_STATE_RESET);
			config.Pin = pin;
			config.Mode = GPIO_MODE_OUTPUT_PP;
			config.Pull = type;
			config.Speed = GPIO_SPEED_FREQ_LOW;
			break;
		case PORT_GPIO_MODE_INPUT:
			GPIO_InitStruct.Pin = pin;
			GPIO_InitStruct.Mode = mode;
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
			break;
	}


	return PORT_GPIO_FN_STATUS_OK;
}

void port_gpio_WritePin(port_gpio_port_t port, port_gpio_pin_e pin, port_gpio_state_e state)
{
}

port_gpio_state_e port_gpio_ReadPin(port_gpio_port_t port, port_gpio_pin_e pin)
{
	return PORT_GPIO_STATE_RESET;
}

void port_gpio_TogglePin(port_gpio_port_t port, port_gpio_pin_e pin)
{
}
