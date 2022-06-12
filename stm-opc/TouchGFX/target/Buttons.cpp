/******************************************************************************
 *
 * @brief     This file is part of the TouchGFX 4.8.0 evaluation distribution.
 *
 * @author    Draupner Graphics A/S <http://www.touchgfx.com>
 *
 ******************************************************************************
 *
 * @section Copyright
 *
 * Copyright (C) 2014-2016 Draupner Graphics A/S <http://www.touchgfx.com>.
 * All rights reserved.
 *
 * TouchGFX is protected by international copyright laws and the knowledge of
 * this source code may not be used to write a similar product. This file may
 * only be used in accordance with a license and should not be re-
 * distributed in any way without the prior permission of Draupner Graphics.
 *
 * This is licensed software for evaluation use, any use must strictly comply
 * with the evaluation license agreement provided with delivery of the
 * TouchGFX software.
 *
 * The evaluation license agreement can be seen on www.touchgfx.com
 *
 * @section Disclaimer
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Draupner Graphics A/S has
 * no obligation to support this software. Draupner Graphics A/S is providing
 * the software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Draupner Graphics A/S can not be held liable for any consequential,
 * incidental, or special damages, or any other relief, or for any claim by
 * any third party, arising from your use of this software.
 *
 *****************************************************************************/
#include <touchgfx/hal/Buttons.hpp>
#include "stm32f7xx_hal.h"

using namespace touchgfx;

extern "C"
{
    /** @addtogroup STM32F769I_DISCOVERY_LOW_LEVEL_BUTTON STM32F769I Discovery Low Level Button
      * @{
      */
    /* Only one User/Wakeup button */
#define BUTTONn                             ((uint8_t)1)

    /**
      * @brief Wakeup push-button
      */
#define WAKEUP_BUTTON_PIN                   GPIO_PIN_0
#define WAKEUP_BUTTON_GPIO_PORT             GPIOA
#define WAKEUP_BUTTON_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define WAKEUP_BUTTON_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOA_CLK_DISABLE()
#define WAKEUP_BUTTON_EXTI_IRQn             EXTI0_IRQn

    /* Define the USER button as an alias of the Wakeup button */
#define USER_BUTTON_PIN                   WAKEUP_BUTTON_PIN
#define USER_BUTTON_GPIO_PORT             WAKEUP_BUTTON_GPIO_PORT
#define USER_BUTTON_GPIO_CLK_ENABLE()     WAKEUP_BUTTON_GPIO_CLK_ENABLE()
#define USER_BUTTON_GPIO_CLK_DISABLE()    WAKEUP_BUTTON_GPIO_CLK_DISABLE()
#define USER_BUTTON_EXTI_IRQn             WAKEUP_BUTTON_EXTI_IRQn

#define BUTTON_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOA_CLK_ENABLE()

    /** @brief Button_TypeDef
      *  STM32F769I_DISCOVERY board Buttons definitions.
      */
    typedef enum
    {
        BUTTON_WAKEUP = 0,
    } Button_TypeDef;

    /** @brief ButtonMode_TypeDef
      *  STM32F769I_DISCOVERY board Buttons Modes definitions.
      */
    typedef enum
    {
        BUTTON_MODE_GPIO = 0,
        BUTTON_MODE_EXTI = 1

    } ButtonMode_TypeDef;

    static GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {WAKEUP_BUTTON_GPIO_PORT };
    static const uint16_t BUTTON_PIN[BUTTONn] = {WAKEUP_BUTTON_PIN };
    static const uint16_t BUTTON_IRQn[BUTTONn] = {WAKEUP_BUTTON_EXTI_IRQn };

    void             BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode);
    uint32_t         BSP_PB_GetState(Button_TypeDef Button);
}

void Buttons::init()
{
    BSP_PB_Init(BUTTON_WAKEUP, BUTTON_MODE_GPIO);
}

unsigned int Buttons::sample()
{
    uint32_t result = 0;

    result = BSP_PB_GetState(BUTTON_WAKEUP);

    return result;
}

extern "C"
{
    /**
      * @brief  Configures button GPIO and EXTI Line.
      * @param  Button: Button to be configured
      *          This parameter can be one of the following values:
      *            @arg  BUTTON_WAKEUP: Wakeup Push Button
      *            @arg  BUTTON_USER: User Push Button
      * @param  Button_Mode: Button mode
      *          This parameter can be one of the following values:
      *            @arg  BUTTON_MODE_GPIO: Button will be used as simple IO
      *            @arg  BUTTON_MODE_EXTI: Button will be connected to EXTI line
      *                                    with interrupt generation capability
      * @retval None
      */
    __weak void BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode)
    {
        GPIO_InitTypeDef gpio_init_structure;

        /* Enable the BUTTON clock */
        BUTTON_GPIO_CLK_ENABLE();

        if (Button_Mode == BUTTON_MODE_GPIO)
        {
            /* Configure Button pin as input */
            gpio_init_structure.Pin = BUTTON_PIN[Button];
            gpio_init_structure.Mode = GPIO_MODE_INPUT;
            gpio_init_structure.Pull = GPIO_NOPULL;
            gpio_init_structure.Speed = GPIO_SPEED_FAST;
            HAL_GPIO_Init(BUTTON_PORT[Button], &gpio_init_structure);
        }

        if (Button_Mode == BUTTON_MODE_EXTI)
        {
            /* Configure Button pin as input with External interrupt */
            gpio_init_structure.Pin = BUTTON_PIN[Button];
            gpio_init_structure.Pull = GPIO_NOPULL;
            gpio_init_structure.Speed = GPIO_SPEED_FAST;

            gpio_init_structure.Mode = GPIO_MODE_IT_RISING;

            HAL_GPIO_Init(BUTTON_PORT[Button], &gpio_init_structure);

            /* Enable and set Button EXTI Interrupt to the lowest priority */
            HAL_NVIC_SetPriority((IRQn_Type)(BUTTON_IRQn[Button]), 0x0F, 0x00);
            HAL_NVIC_EnableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
        }
    }

    /**
      * @brief  Returns the selected button state.
      * @param  Button: Button to be checked
      *          This parameter can be one of the following values:
      *            @arg  BUTTON_WAKEUP: Wakeup Push Button
      *            @arg  BUTTON_USER: User Push Button
      * @retval The Button GPIO pin value
      */
    __weak uint32_t BSP_PB_GetState(Button_TypeDef Button)
    {
        return HAL_GPIO_ReadPin(BUTTON_PORT[Button], BUTTON_PIN[Button]);
    }
}
