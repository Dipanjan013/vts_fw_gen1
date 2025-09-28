#include "port_gpio.h"

__attribute__((weak)) void port_gpio_Callback(port_gpio_pin_e pin)
{

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	port_gpio_Callback(GPIO_Pin);
}

port_gpio_fnStatus_e port_gpio_Init(port_gpio_port_t *pPort, port_gpio_pin_e pin, port_gpio_mode_e mode, port_gpio_pull_t type)
{
	GPIO_InitTypeDef config = {0};
	/* GPIO Ports Clock Enable */
	if(pPort == GPIOA){
		__HAL_RCC_GPIOA_CLK_ENABLE();
	}else if(pPort == GPIOB){
		__HAL_RCC_GPIOB_CLK_ENABLE();
	}else if(pPort == GPIOC){
		__HAL_RCC_GPIOC_CLK_ENABLE();
	}else{
	}

  /*Configure GPIO pin */
	switch(mode){
		case PORT_GPIO_MODE_OUTPUT:
			HAL_GPIO_WritePin(pPort, pin, PORT_GPIO_STATE_RESET);
			config.Pin = pin;
			config.Mode = mode;
			config.Pull = type;
			config.Speed = GPIO_SPEED_FREQ_LOW;
			break;
		case PORT_GPIO_MODE_INPUT:
		case PORT_GPIO_MODE_ANALOG:
		case PORT_GPIO_MODE_INPUT_IRQ_RISE:
		case PORT_GPIO_MODE_INPUT_IRQ_FALL:
		case PORT_GPIO_MODE_INPUT_IRQ_BOTH:
			config.Pin = pin;
			config.Mode = mode;
			config.Pull = type;
			break;
		default:
			return PORT_GPIO_FN_STATUS_INVALID_ARG;
	}
	HAL_GPIO_Init(pPort, &config);
	return PORT_GPIO_FN_STATUS_OK;
}

port_gpio_fnStatus_e port_gpio_DeInit(port_gpio_port_t *pPort, port_gpio_pin_e pin)
{
	HAL_GPIO_DeInit(pPort, pin);
	return PORT_GPIO_FN_STATUS_OK;
}

void port_gpio_EnableIrq(port_gpio_interrupt_t type)
{
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void port_gpio_DisableIrq(port_gpio_port_t port, port_gpio_pin_e pin)
{
	HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
}

void port_gpio_WritePin(port_gpio_port_t *pPort, port_gpio_pin_e pin, port_gpio_state_e state)
{
	HAL_GPIO_WritePin(pPort, pin, state);
}

port_gpio_state_e port_gpio_ReadPin(port_gpio_port_t *pPort, port_gpio_pin_e pin)
{
	if(GPIO_PIN_SET == HAL_GPIO_ReadPin(pPort, pin)){
		return PORT_GPIO_STATE_SET;
	}else{
		return PORT_GPIO_STATE_RESET;
	}
}

void port_gpio_TogglePin(port_gpio_port_t *pPort, port_gpio_pin_e pin)
{
	HAL_GPIO_TogglePin(pPort, pin);
}
