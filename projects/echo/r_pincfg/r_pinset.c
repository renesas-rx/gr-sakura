/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIESREGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_pinset.c
* Version      : 1.2.1
* Device(s)    : R5F563NEDxFB
* Tool-Chain   : RXC
* Description  : Setting of port and mpc registers
* Creation Date: 27 9 2016
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_pinset.h"
#include "platform.h"
#include "r_ether_rx_if.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: R_ETHER_PinSet_ETHERC0_RMII
* Description  : This function initializes pins for r_ether_rx module
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void R_ETHER_PinSet_ETHERC0_RMII()
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);

    /* Set REF50CK0 pin */
    MPC.PB2PFS.BYTE = 0x12U;
    PORTB.PMR.BIT.B2 = 1U;

    /* Set RMII0_TXD_EN pin */
    MPC.PB4PFS.BYTE = 0x12U;
    PORTB.PMR.BIT.B4 = 1U;

    /* Set RMII0_TXD1 pin */
    MPC.PB6PFS.BYTE = 0x12U;
    PORTB.PMR.BIT.B6 = 1U;

    /* Set RMII0_TXD0 pin */
    MPC.PB5PFS.BYTE = 0x12U;
    PORTB.PMR.BIT.B5 = 1U;

    /* Set RMII0_RXD1 pin */
    MPC.PB0PFS.BYTE = 0x12U;
    PORTB.PMR.BIT.B0 = 1U;

    /* Set RMII0_RXD0 pin */
    MPC.PB1PFS.BYTE = 0x12U;
    PORTB.PMR.BIT.B1 = 1U;

    /* Set RMII0_RX_ER pin */
    MPC.PB3PFS.BYTE = 0x12U;
    PORTB.PMR.BIT.B3 = 1U;

    /* Set RMII0_CRS_DV pin */
    MPC.PB7PFS.BYTE = 0x12U;
    PORTB.PMR.BIT.B7 = 1U;

    /* Set ET0_MDC pin */
    MPC.PA4PFS.BYTE = 0x11U;
    PORTA.PMR.BIT.B4 = 1U;

    /* Set ET0_MDIO pin */
    MPC.PA3PFS.BYTE = 0x11U;
    PORTA.PMR.BIT.B3 = 1U;

#if (ETHER_CFG_USE_LINKSTA == 1)
        /* Switch to the selected input/output function for LINKSTA */
        MPC.PA5PFS.BYTE = 0x11;
        PORTA.PMR.BIT.B5 = 1;
#endif

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);

}

