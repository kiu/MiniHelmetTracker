/**
 * \file
 *
 * \brief PWM Basic driver implementation.
 *
 (c) 2020 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms,you may use this software and
    any derivatives exclusively with Microchip products.It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

/**
 * \defgroup doc_driver_pwm_basic PWM Basic Driver
 * \ingroup doc_driver_pwm
 *
 * \section doc_driver_pwm_normal_rev Revision History
 * - v0.0.0.1 Initial Commit
 *
 *@{
 */
#include <pwm_basic.h>

/**
 * \brief Initialize PWM
 * If module is configured to disabled state, the clock to the PWM is disabled
 * if this is supported by the device's clock system.
 *
 * \return Initialization status.
 */
int8_t PWM_0_init()
{

	TCB0.CCMPL = 0xff; /* PWM Period: 0xff */

	// TCB0.CCMPH = 0x0; /* PWM Compare: 0x0 */

	// TCB0.CNT = 0x0; /* Count: 0x0 */

	TCB0.CTRLB = 0 << TCB_ASYNC_bp      /* Asynchronous Enable: disabled */
	             | 1 << TCB_CCMPEN_bp   /* Pin Output Enable: enabled */
	             | 0 << TCB_CCMPINIT_bp /* Pin Initial State: disabled */
	             | TCB_CNTMODE_PWM8_gc; /* 8-bit PWM */

	// TCB0.DBGCTRL = 0 << TCB_DBGRUN_bp; /* Debug Run: disabled */

	// TCB0.EVCTRL = 0 << TCB_CAPTEI_bp /* Event Input Enable: disabled */
	//		 | 0 << TCB_EDGE_bp /* Event Edge: disabled */
	//		 | 0 << TCB_FILTER_bp; /* Input Capture Noise Cancellation Filter: disabled */

	// TCB0.INTCTRL = 0 << TCB_CAPT_bp /* Setting: disabled */;

	TCB0.CTRLA = TCB_CLKSEL_TCA0_gc     /* Use CLK_TCA from TCA0 */
	             | 0 << TCB_ENABLE_bp   /* Enable: disabled */
	             | 0 << TCB_RUNSTDBY_bp /* Run Standby: disabled */
	             | 0 << TCB_SYNCUPD_bp /* Synchronize Update: disabled */;

	return 0;
}

/**
 * \brief Enable PWM_0
 * 1. If supported by the clock system, enables the clock to the PWM
 * 2. Enables the PWM module by setting the enable-bit in the PWM control register
 *
 * \return Nothing
 */
void PWM_0_enable()
{
	TCB0.CTRLA |= TCB_ENABLE_bm;
}

/**
 * \brief Disable PWM_0
 * 1. Disables the PWM module by clearing the enable-bit in the PWM control register
 * 2. If supported by the clock system, disables the clock to the PWM
 *
 * \return Nothing
 */
void PWM_0_disable()
{
	TCB0.CTRLA &= ~TCB_ENABLE_bm;
}

/**
 * \brief Enable PWM output on channel 0
 *
 * \return Nothing
 */
void PWM_0_enable_output_ch0()
{
	TCB0.CTRLA |= TCB_ENABLE_bm;
}

/**
 * \brief Disable PWM output on channel 0
 *
 * \return Nothing
 */
void PWM_0_disable_output_ch0()
{
	TCB0.CTRLA &= ~TCB_ENABLE_bm;
}

/**
 * \brief Load COUNTER register in PWM_0
 *
 * \param[in] counter_value The value to load into COUNTER
 *
 * \return Nothing
 */
void PWM_0_load_counter(PWM_0_register_t counter_value)
{
	TCB0.CNT = counter_value;
}

/**
 * \brief Load TOP register in PWM_0.
 * The physical register may different names, depending on the hardware and module mode.
 *
 * \param[in] counter_value The value to load into TOP.
 *
 * \return Nothing
 */
void PWM_0_load_top(PWM_0_register_t top_value)
{
	TCB0.CCMPL = top_value;
}

/**
 * \brief Load duty cycle register in for channel 0.
 * The physical register may have different names, depending on the hardware.
 * This is not the duty cycle as percentage of the whole period, but the actual
 * counter compare value.
 *
 * \param[in] counter_value The value to load into the duty cycle register.
 *
 * \return Nothing
 */
void PWM_0_load_duty_cycle_ch0(PWM_0_register_t duty_value)
{
	TCB0.CTRLA &= ~TCB_ENABLE_bm;
	TCB0.CCMPH = duty_value;
	TCB0.CNT   = 0;
	TCB0.CTRLA |= TCB_ENABLE_bm;
}

/**
 * \brief Initialize PWM
 * If module is configured to disabled state, the clock to the PWM is disabled
 * if this is supported by the device's clock system.
 *
 * \return Initialization status.
 */
int8_t PWM_1_init()
{

	TCB1.CCMPL = 0xff; /* PWM Period: 0xff */

	// TCB1.CCMPH = 0x0; /* PWM Compare: 0x0 */

	// TCB1.CNT = 0x0; /* Count: 0x0 */

	TCB1.CTRLB = 0 << TCB_ASYNC_bp      /* Asynchronous Enable: disabled */
	             | 1 << TCB_CCMPEN_bp   /* Pin Output Enable: enabled */
	             | 0 << TCB_CCMPINIT_bp /* Pin Initial State: disabled */
	             | TCB_CNTMODE_PWM8_gc; /* 8-bit PWM */

	// TCB1.DBGCTRL = 0 << TCB_DBGRUN_bp; /* Debug Run: disabled */

	// TCB1.EVCTRL = 0 << TCB_CAPTEI_bp /* Event Input Enable: disabled */
	//		 | 0 << TCB_EDGE_bp /* Event Edge: disabled */
	//		 | 0 << TCB_FILTER_bp; /* Input Capture Noise Cancellation Filter: disabled */

	// TCB1.INTCTRL = 0 << TCB_CAPT_bp /* Setting: disabled */;

	TCB1.CTRLA = TCB_CLKSEL_TCA0_gc     /* Use CLK_TCA from TCA0 */
	             | 0 << TCB_ENABLE_bp   /* Enable: disabled */
	             | 0 << TCB_RUNSTDBY_bp /* Run Standby: disabled */
	             | 0 << TCB_SYNCUPD_bp /* Synchronize Update: disabled */;

	return 0;
}

/**
 * \brief Enable PWM_1
 * 1. If supported by the clock system, enables the clock to the PWM
 * 2. Enables the PWM module by setting the enable-bit in the PWM control register
 *
 * \return Nothing
 */
void PWM_1_enable()
{
	TCB1.CTRLA |= TCB_ENABLE_bm;
}

/**
 * \brief Disable PWM_1
 * 1. Disables the PWM module by clearing the enable-bit in the PWM control register
 * 2. If supported by the clock system, disables the clock to the PWM
 *
 * \return Nothing
 */
void PWM_1_disable()
{
	TCB1.CTRLA &= ~TCB_ENABLE_bm;
}

/**
 * \brief Enable PWM output on channel 0
 *
 * \return Nothing
 */
void PWM_1_enable_output_ch0()
{
	TCB1.CTRLA |= TCB_ENABLE_bm;
}

/**
 * \brief Disable PWM output on channel 0
 *
 * \return Nothing
 */
void PWM_1_disable_output_ch0()
{
	TCB1.CTRLA &= ~TCB_ENABLE_bm;
}

/**
 * \brief Load COUNTER register in PWM_1
 *
 * \param[in] counter_value The value to load into COUNTER
 *
 * \return Nothing
 */
void PWM_1_load_counter(PWM_1_register_t counter_value)
{
	TCB1.CNT = counter_value;
}

/**
 * \brief Load TOP register in PWM_1.
 * The physical register may different names, depending on the hardware and module mode.
 *
 * \param[in] counter_value The value to load into TOP.
 *
 * \return Nothing
 */
void PWM_1_load_top(PWM_1_register_t top_value)
{
	TCB1.CCMPL = top_value;
}

/**
 * \brief Load duty cycle register in for channel 0.
 * The physical register may have different names, depending on the hardware.
 * This is not the duty cycle as percentage of the whole period, but the actual
 * counter compare value.
 *
 * \param[in] counter_value The value to load into the duty cycle register.
 *
 * \return Nothing
 */
void PWM_1_load_duty_cycle_ch0(PWM_1_register_t duty_value)
{
	TCB1.CTRLA &= ~TCB_ENABLE_bm;
	TCB1.CCMPH = duty_value;
	TCB1.CNT   = 0;
	TCB1.CTRLA |= TCB_ENABLE_bm;
}
