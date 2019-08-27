/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2012(2016) Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

/*******************************************************************************
* File Name     : main.c
* Version       : 1.01
* Device(s)     : RX family
* Tool-Chain    : C/C++ Compiler Package for RX Family
* OS            :
* H/W Platform  : RX family board
* Description   : T4 sample program
* Operation     : 1. Compile and download the sample code. Click 'Reset Go'
*                    to start the software.
* Limitations   :
******************************************************************************/
/******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 15.02.2011 1.00    First Release
*               : 30.08.2011 1.01    Clean up source code.
*               : 01.08.2012 1.02    Correct accessing version info code.
******************************************************************************/

/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "r_t4_itcpip.h"
#include "r_sys_time_rx_if.h"
#include "echo_srv_sample.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#if (APPLICATION_T4_CHANNEL_NUM  == (2))
#define T4_WORK_SIZE (9112)  /* Maximum size used in echo server sample program */
#elif (APPLICATION_T4_CHANNEL_NUM == (1))
#define T4_WORK_SIZE (4412)  /* Maximum size used in echo server sample program */
#endif

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern void echo_srv(void);

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
void main(void);

/******************************************************************************
Private global variables and functions
******************************************************************************/
static UW tcpudp_work[T4_WORK_SIZE / 4 + 1];

/******************************************************************************
* Function Name : main
* Declaration   : void main(void)
* Description   : This function is main processing
* Arguments     : none
* Return value  : none
******************************************************************************/
void main(void)
{
    ER ercd;
    W    size;
    sys_time_err_t systime_ercd;
    char    ver[128];

    strcpy(ver, (char*)R_t4_version.library);

    systime_ercd = R_SYS_TIME_Open();
    if (systime_ercd != SYS_TIME_SUCCESS)
    {
        while ((bool)1);
    }

    /* start LAN controller */
    ercd = lan_open();
    if (ercd != E_OK)
    {
        while ((bool)1);
    }

    /* initialize TCP/IP */
    size = tcpudp_get_ramsize();
    if (size > (sizeof(tcpudp_work)))
    {
        while ((bool)1);
    }
    ercd = tcpudp_open(tcpudp_work);
    if (ercd != E_OK)
    {
        while ((bool)1);
    }

    /* start echo server */
    echo_srv();

    /* end TCP/IP */
    tcpudp_close();
    lan_close();
    R_SYS_TIME_Close();

    while ((bool)1);
}
