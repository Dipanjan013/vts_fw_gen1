#include <stdint.h>
#include "gpio.h"
#include "stm32f4xx_hal.h"

#define PORT_GPIO_PORT_A GPIOA
#define PORT_GPIO_PORT_B GPIOB
#define PORT_GPIO_PORT_C GPIOC
#define PORT_GPIO_PORT_D GPIOD
#define PORT_GPIO_PORT_E GPIOE
#define PORT_GPIO_PORT_H GPIOH

typedef GPIO_TypeDef port_gpio_port_t;

typedef enum {
	PORT_GPIO_PIN_0  = GPIO_PIN_0,
	PORT_GPIO_PIN_1  = GPIO_PIN_1,
	PORT_GPIO_PIN_2  = GPIO_PIN_2,
	PORT_GPIO_PIN_3  = GPIO_PIN_3,
	PORT_GPIO_PIN_4  = GPIO_PIN_4,
	PORT_GPIO_PIN_5  = GPIO_PIN_5,
	PORT_GPIO_PIN_6  = GPIO_PIN_6,
	PORT_GPIO_PIN_7  = GPIO_PIN_7,
	PORT_GPIO_PIN_8  = GPIO_PIN_8,
	PORT_GPIO_PIN_9  = GPIO_PIN_9,
	PORT_GPIO_PIN_10 = GPIO_PIN_10,
	PORT_GPIO_PIN_11 = GPIO_PIN_11,
	PORT_GPIO_PIN_12 = GPIO_PIN_12,
	PORT_GPIO_PIN_13 = GPIO_PIN_13,
	PORT_GPIO_PIN_14 = GPIO_PIN_14,
	PORT_GPIO_PIN_15 = GPIO_PIN_15,
	PORT_GPIO_PIN_ALL = GPIO_PIN_All
} port_gpio_pin_e;

typedef enum{
	PORT_GPIO_FN_STATUS_INVALID_ARG = -2,
	PORT_GPIO_FN_STATUS_FAIL = -1,
	PORT_GPIO_FN_STATUS_OK = 0,
}port_gpio_fnStatus_e;

typedef enum{
	PORT_GPIO_MODE_INPUT = GPIO_MODE_INPUT,
	PORT_GPIO_MODE_OUTPUT = GPIO_MODE_OUTPUT_PP,
	PORT_GPIO_MODE_ANALOG = GPIO_MODE_ANALOG
}port_gpio_mode_e;

typedef enum{
	PORT_GPIO_PULL_DOWN = GPIO_PULLDOWN,
	PORT_GPIO_PULL_UP = GPIO_PULLUP,
	PORT_GPIO_NO_PULL = GPIO_NOPULL,
}port_gpio_pull_t;

typedef enum{
	PORT_GPIO_INTERRUPT_RISE_EDGE = GPIO_MODE_IT_RISING,
	PORT_GPIO_INTERRUPT_FALL_EDGE = GPIO_MODE_IT_FALLING,
	PORT_GPIO_INTERRUPT_BOTH_EDGE = GPIO_MODE_IT_RISING_FALLING,
}port_gpio_interrupt_t;

typedef enum{
	PORT_GPIO_STATE_SET = GPIO_PIN_SET,
	PORT_GPIO_STATE_RESET = GPIO_PIN_RESET,
}port_gpio_state_e;

port_gpio_fnStatus_e port_gpio_Init(port_gpio_port_t *pPort, port_gpio_pin_e pin, port_gpio_mode_e mode, port_gpio_pull_t type);
port_gpio_fnStatus_e port_gpio_EnableIrq(port_gpio_interrupt_t type);
port_gpio_fnStatus_e port_gpio_DisableIrq(port_gpio_port_t port, port_gpio_pin_e pin);

/*!
 * @fn port_gpio_WritePin(port_gpio_port_t port, port_gpio_pin_e pin, port_gpio_state_e state);
 * @param *pPort GPIO port
 * @param pin GPIO pin
 * @param state port_gpio_state_e
 */
void port_gpio_WritePin(port_gpio_port_t *pPort, port_gpio_pin_e pin, port_gpio_state_e state);

/*!
 * @fn port_gpio_ReadPin(port_gpio_port_t port, port_gpio_pin_e pin);
 * @param *pPort GPIO port
 * @param pin GPIO pin
 * @return port_gpio_state_e
 */
port_gpio_state_e port_gpio_ReadPin(port_gpio_port_t *pPort, port_gpio_pin_e pin);

/*!
 * @fn port_gpio_TogglePin(port_gpio_port_t port, port_gpio_pin_e pin);
 * @param *pPort GPIO port
 * @param pin GPIO pin
 */
void port_gpio_TogglePin(port_gpio_port_t *pPort, port_gpio_pin_e pin);

/*!
 * @fn port_gpio_Callback(port_gpio_pin_e pin);
 * @brief This is a weak function that is invoked when an IRQ is received. Add your own function in this Callback
 * @param pin GPIO pin
 */
void port_gpio_Callback(port_gpio_pin_e pin);
