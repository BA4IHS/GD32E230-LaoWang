/*!
    \file    gd32e23x_cmp.c
    \brief   CMP driver

    \version 2026-02-04, V2.5.0, firmware for GD32E23x
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "gd32e23x_cmp.h"

/*!
    \brief      CMP deinit
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
    \param[out] none
    \retval     none
*/
void cmp_deinit(cmp_enum cmp_periph)
{
    if(CMP0 == cmp_periph) {
        CMP_CS &= ((uint32_t)0xFFFF0000U);
    } else {
    }
}

/*!
    \brief      CMP mode init
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
    \param[in]  operating_mode
      \arg        CMP_MODE_HIGHSPEED: high speed mode
      \arg        CMP_MODE_MIDDLESPEED: medium speed mode
      \arg        CMP_MODE_LOWSPEED: low speed mode
      \arg        CMP_MODE_VERYLOWSPEED: very-low speed mode
    \param[in]  inverting_input
      \arg        CMP_INVERTING_INPUT_1_4VREFINT: VREFINT *1/4 input
      \arg        CMP_INVERTING_INPUT_1_2VREFINT: VREFINT *1/2 input
      \arg        CMP_INVERTING_INPUT_3_4VREFINT: VREFINT *3/4 input
      \arg        CMP_INVERTING_INPUT_VREFINT: VREFINT input
      \arg        CMP_INVERTING_INPUT_PA4: PA4 input
      \arg        CMP_INVERTING_INPUT_PA5: PA5 input
      \arg        CMP_INVERTING_INPUT_PA0: PA0 input
      \arg        CMP_INVERTING_INPUT_PA2: PA2 input
    \param[in]  output_hysteresis
      \arg        CMP_HYSTERESIS_NO: output no hysteresis
      \arg        CMP_HYSTERESIS_LOW: output low hysteresis
      \arg        CMP_HYSTERESIS_MIDDLE: output middle hysteresis
      \arg        CMP_HYSTERESIS_HIGH: output high hysteresis
    \param[out] none
    \retval     none
*/
void cmp_mode_init(cmp_enum cmp_periph, uint32_t operating_mode, uint32_t inverting_input, uint32_t output_hysteresis)
{
    uint32_t temp = 0U;

    if(CMP0 == cmp_periph) {
        /* initialize comparator 0 mode */
        temp = CMP_CS;
        temp &= ~(uint32_t)(CMP_CS_CMP0M | CMP_CS_CMP0MSEL | CMP_CS_CMP0HST);
        temp |= (uint32_t)(operating_mode | inverting_input | output_hysteresis);
        CMP_CS = temp;
    } else {
    }
}

/*!
    \brief      CMP output init
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
    \param[in]  output_selection
      \arg        CMP_OUTPUT_NONE: CMP output none
      \arg        CMP_OUTPUT_TIMER0_BKIN: CMP output TIMER0 break input
      \arg        CMP_OUTPUT_TIMER0_IC0: CMP output TIMER0_CH0 input capture
      \arg        CMP_OUTPUT_TIMER0_OCPRECLR: CMP output TIMER0 OCPRE_CLR input
      \arg        CMP_OUTPUT_TIMER2_IC0: CMP output TIMER2_CH0 input capture
      \arg        CMP_OUTPUT_TIMER2_OCPRECLR: CMP output TIMER2 OCPRE_CLR input
    \param[in]  output_polarity
      \arg        CMP_OUTPUT_POLARITY_INVERTED: output is inverted
      \arg        CMP_OUTPUT_POLARITY_NONINVERTED: output is not inverted
    \param[out] none
    \retval     none
*/
void cmp_output_init(cmp_enum cmp_periph, uint32_t output_selection, uint32_t output_polarity)
{
    uint32_t temp = 0U;

    if(CMP0 == cmp_periph) {
        /* initialize comparator 0 output */
        temp = CMP_CS;
        temp &= ~(uint32_t)CMP_CS_CMP0OSEL;
        temp |= (uint32_t)output_selection;
        /* output polarity */
        if(CMP_OUTPUT_POLARITY_INVERTED == output_polarity) {
            temp |= (uint32_t)CMP_CS_CMP0PL;
        } else {
            temp &= ~(uint32_t)CMP_CS_CMP0PL;
        }
        CMP_CS = temp;
    } else {
    }
}

/*!
    \brief      enable CMP
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
    \param[out] none
    \retval     none
*/
void cmp_enable(cmp_enum cmp_periph)
{
    if(CMP0 == cmp_periph) {
        CMP_CS |= (uint32_t)CMP_CS_CMP0EN;
    } else {
    }
}

/*!
    \brief      disable CMP
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
    \param[out] none
    \retval     none
*/
void cmp_disable(cmp_enum cmp_periph)
{
    if(CMP0 == cmp_periph) {
        CMP_CS &= ~(uint32_t)CMP_CS_CMP0EN;
    } else {
    }
}

/*!
    \brief      enable CMP switch
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cmp_switch_enable(void)
{
    CMP_CS |= (uint32_t)CMP_CS_CMP0SW;
}

/*!
    \brief      disable CMP switch
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cmp_switch_disable(void)
{
    CMP_CS &= ~(uint32_t)CMP_CS_CMP0SW;
}

/*!
    \brief      lock the CMP
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
    \param[out] none
    \retval     none
*/
void cmp_lock_enable(cmp_enum cmp_periph)
{
    if(CMP0 == cmp_periph) {
        /* lock CMP0 */
        CMP_CS |= (uint32_t)CMP_CS_CMP0LK;
    } else {
    }
}

/*!
    \brief      get output level
    \param[in]  cmp_periph
      \arg        CMP0: comparator 0
    \param[out] none
    \retval     the output level
*/
uint32_t cmp_output_level_get(cmp_enum cmp_periph)
{
    uint32_t reval = 0U;
    if(CMP0 == cmp_periph){
        /* get output level of CMP0 */
        if((uint32_t)RESET != (CMP_CS & CMP_CS_CMP0O)) {
            reval = CMP_OUTPUTLEVEL_HIGH;
        }else{
            reval = CMP_OUTPUTLEVEL_LOW;
        }
    }else{
        /* illegal parameters */
    }
    return reval;
}
