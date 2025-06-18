#include "port_led.h"

uint8_t port_led_Init(void)
{
	port_gpio_fnStatus_e ret;
	ret = port_gpio_Init(PORT_LED_BLUE_PORT, PORT_LED_BLUE_PIN, PORT_GPIO_MODE_OUTPUT, PORT_LED_PULL_TYPE);
	if(ret != PORT_GPIO_FN_STATUS_OK){
		return 0;
	}
	return 1;
}

uint8_t port_led_DeInit(void)
{
	port_gpio_fnStatus_e ret;
	ret = port_gpio_DeInit(PORT_LED_BLUE_PORT, PORT_LED_BLUE_PIN);
	if(ret != PORT_GPIO_FN_STATUS_OK){
		return 0;
	}
	return 1;
}

void port_led_On(port_led_colour_e colour)
{
	port_gpio_state_e pinState;
	pinState = (PORT_LED_PULL_TYPE == PORT_GPIO_PULL_DOWN) ? PORT_GPIO_STATE_SET : PORT_GPIO_STATE_RESET;
	switch(colour){
		case PORT_LED_COLOUR_BLUE:
			port_gpio_WritePin(PORT_LED_BLUE_PORT, PORT_LED_BLUE_PIN, pinState);
			break;
		default:
	}
}

void port_led_Off(port_led_colour_e colour)
{
	port_gpio_state_e pinState;
	pinState = (PORT_LED_PULL_TYPE == PORT_GPIO_PULL_DOWN) ? PORT_GPIO_STATE_RESET : PORT_GPIO_STATE_SET;
	switch(colour){
		case PORT_LED_COLOUR_BLUE:
			port_gpio_WritePin(PORT_LED_BLUE_PORT, PORT_LED_BLUE_PIN, pinState);
			break;
		default:
	}
}

void port_led_Toggle(port_led_colour_e colour)
{
	switch(colour){
		case PORT_LED_COLOUR_BLUE:
			port_gpio_TogglePin(PORT_LED_BLUE_PORT, PORT_LED_BLUE_PIN);
			break;
		default:
	}
}
