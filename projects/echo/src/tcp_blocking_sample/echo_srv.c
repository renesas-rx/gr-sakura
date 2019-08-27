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
* Copyright (C) 2011(2016) Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

/*******************************************************************************
* File Name     : echo_srv_tcp_blocking.c
* Version       : 2.00
* Device(s)     : Renesas microcomputer
* Tool-Chain    : Renesas compilers
* OS            :
* H/W Platform  :
* Description   : T4 sample program.
* Limitations   :
******************************************************************************/
/******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 21.06.2010 1.00 First Release
*               : 30.08.2011 1.01 Clean up source code
*               : 01.04.2014 2.00 Release for V.2.00
******************************************************************************/

/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "r_t4_itcpip.h"
#include "r_ether_rx_if.h"
#include "r_sys_time_rx_if.h"
#include "echo_srv_sample.h"

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct _CEP
{
    uint32_t    status;
    T_IPV4EP    dst_addr;
    T_IPV4EP    src_addr;
    int32_t     current_rcv_data_len;
    int32_t     total_rcv_len;
    UB          rcv_buf[1460];
    UB          snd_buf[1460];
    int32_t     _1sec_counter;
    int32_t     _1sec_timer;
    int32_t     pre_1sec_timer;
} CEP;


/******************************************************************************
Macro definitions
******************************************************************************/
#if(APPLICATION_T4_CHANNEL_NUM == 2)
#define CEP_NUM          (2)
#elif(APPLICATION_T4_CHANNEL_NUM == 1)
#define CEP_NUM          (1)
#endif

#define T4_CLOSED        0
#define T4_ACCEPT        1
#define T4_ESTABLISHED   2
#define T4_CLOSE_WAIT    3
#define T4_LAST_ACK      4
#define T4_FIN_WAIT1     5
#define T4_TIME_WAIT     6
#define T4_CANCELING     7
#define T4_SENDING       8
#define T4_RESET_WAIT    255

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
void echo_srv(void);
ER    t4_tcp_callback(ID cepid, FN fncd , VP p_parblk);

/******************************************************************************
Private global variables and functions
******************************************************************************/
static CEP cep[CEP_NUM];
static UB guc_event[APPLICATION_T4_CHANNEL_NUM];
static DHCP* gpt_dhcp[APPLICATION_T4_CHANNEL_NUM];

/******************************************************************************
* ID                : 1.0
* Outline           : echo_srv
* Include           : r_t4_itcpip.h
* Declaration       : void echo_srv(void)
* Function Name     : echo_srv
* Description       : echo server main function.
* Argument          : none
* Return Value      : none
******************************************************************************/
void echo_srv(void)
{
    ID cepid;
    ID repid;
    UB interface;
    ER ercd;
    uint32_t i;
    uint32_t k;
    uint32_t n;

    /* initialize cep status */
    for (i = 0; i < CEP_NUM; i++)
    {
        cep[i].status = T4_CLOSED;
    }

    repid = 1;      /* refer to tcp_crep[0] */
    n = 0;
    while (n < CEP_NUM)
    {
        if(1 == _t4_dhcp_enable)                                                                                /*dhcp use*/
        {
            /* check all connections */
            for (i = 0; i < _t4_channel_num; i++)
            {
                interface = i;
                if (DHCP_EV_PLEASE_RESET == guc_event[interface])                                                   /*t4 reset request*/
                {
                    guc_event[interface] = 0;                                                                       /*reset callback event */
#if defined(DEBUG_PRINT)
                    printf("main:call tcpudp_reset(channel:%d)\n", interface);
#endif /*#if defined(DEBUG_PRINT)*/
                    tcpudp_reset(interface);                                                                        /*reset DHCP use interface*/

                    /* reset application data same as interface. */
                    for (k = 0; k < CEP_NUM; k++)
                    {
                        if (interface == tcp_ccep[k].cepatr)
                        {
                            memset(&cep[k], 0, sizeof(CEP));
                            cep[k].status = T4_CLOSED;
                        }
                    }
                }
            }
        }

        cepid = n + 1;
        if(1 != _t4_dhcp_enable || (DHCP_EV_LEASE_IP == guc_event[n]) || (DHCP_EV_APIPA == guc_event[n]))
        {
            ercd = tcp_acp_cep(cepid, repid, &cep[n].dst_addr, TMO_FEVR);
            if(E_OK == ercd)
            {
                while(1)
                {
                    ercd = tcp_rcv_dat(cepid, cep[n].rcv_buf, sizeof(cep[n].rcv_buf), TMO_FEVR);
                    if(ercd <= 0)
                    {
                        break;
                    }
                    ercd = tcp_snd_dat(cepid, cep[n].rcv_buf, ercd, TMO_FEVR);
                    if(ercd < 0)
                    {
                        break;
                    }
                }
                tcp_sht_cep(cepid);
                ercd = tcp_cls_cep(cepid, TMO_FEVR);
                if(ercd == E_OK)
                {
                    n++;
                }
            }
        }
    }
}


/******************************************************************************
* ID                : 1.0
* Outline           : t4_tcp_callback
* Include           : r_t4_itcpip.h
* Declaration       : ER t4_tcp_callback(ID cepid, FN fncd , VP p_parblk)
* Function Name     : t4_tcp_callback
* Description       : Callback function.
* Argument          : ID        cepid   ;    Communication endpoint ID
*                   : FN        fncd    ;    Event code
*                   : VP        p_parblk;    Parameter block
* Return Value      : ER                ;    always 0 (not in use)
******************************************************************************/
ER    t4_tcp_callback(ID cepid, FN fncd , VP p_parblk)
{
    return 0;
}


/******************************************************************************
* ID                : 1.0
* Outline           : user_cb
* Include           : r_t4_itcpip.h
* Declaration       : ER user_cb(UB channel, FN eventid , VP param)
* Function Name     : user_cb
* Description       : Callback function.
* Argument          : UB        channel ;    RJ45 interface
*                   : FN        eventid ;    Event code
*                   : VP        param   ;    Parameter block
* Return Value      : ER                ;    always 0 (not in use)
******************************************************************************/
ER user_cb(UB channel, UW eventid, VP param)
{
#if defined(DEBUG_PRINT)
    uint8_t*    ev_tbl[] =
    {
        "ETHER_EV_LINK_OFF",
        "ETHER_EV_LINK_ON",
        "ETHER_EV_COLLISION_IP",
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
        "DHCP_EV_LEASE_IP",
        "DHCP_EV_LEASE_OVER",
        "DHCP_EV_INIT",
        "DHCP_EV_INIT_REBOOT",
        "DHCP_EV_APIPA",
        "DHCP_EV_NAK",
        "DHCP_EV_FATAL_ERROR",
        "DHCP_EV_PLEASE_RESET"
    };
    printf("^^>>>user_cb<<< ch:%d,eventID = %s\n", channel, ev_tbl[eventid]);
#endif /*#if defined(DEBUG_PRINT)*/
    guc_event[channel] = eventid;
    switch(eventid)
    {
        case ETHER_EV_LINK_OFF:
            break;

        case ETHER_EV_LINK_ON:
            break;

        case ETHER_EV_COLLISION_IP:
            break;

        case DHCP_EV_LEASE_IP:
            gpt_dhcp[channel] = (DHCP*)param;
#if defined(DEBUG_PRINT)
            printf("DHCP.ipaddr[4]   %d.%d.%d.%d\n",
                   gpt_dhcp[channel]->ipaddr[0], gpt_dhcp[channel]->ipaddr[1],
                   gpt_dhcp[channel]->ipaddr[2], gpt_dhcp[channel]->ipaddr[3]);
            printf("DHCP.maskaddr[4] %d.%d.%d.%d\n",
                   gpt_dhcp[channel]->maskaddr[0], gpt_dhcp[channel]->maskaddr[1],
                   gpt_dhcp[channel]->maskaddr[2], gpt_dhcp[channel]->maskaddr[3]);
            printf("DHCP.gwaddr[4]   %d.%d.%d.%d\n",
                   gpt_dhcp[channel]->gwaddr[0], gpt_dhcp[channel]->gwaddr[1],
                   gpt_dhcp[channel]->gwaddr[2], gpt_dhcp[channel]->gwaddr[3]);
            printf("DHCP.dnsaddr[4]  %d.%d.%d.%d\n",
                   gpt_dhcp[channel]->dnsaddr[0], gpt_dhcp[channel]->dnsaddr[1],
                   gpt_dhcp[channel]->dnsaddr[2], gpt_dhcp[channel]->dnsaddr[3]);
            printf("DHCP.dnsaddr2[4] %d.%d.%d.%d\n",
                   gpt_dhcp[channel]->dnsaddr2[0], gpt_dhcp[channel]->dnsaddr2[1],
                   gpt_dhcp[channel]->dnsaddr2[2], gpt_dhcp[channel]->dnsaddr2[3]);
            printf("DHCP.macaddr[6]  %02X:%02X:%02X:%02X:%02X:%02X\n",
                   gpt_dhcp[channel]->macaddr[0],  gpt_dhcp[channel]->macaddr[1],  gpt_dhcp[channel]->macaddr[2],
                   gpt_dhcp[channel]->macaddr[3],  gpt_dhcp[channel]->macaddr[4],  gpt_dhcp[channel]->macaddr[5]);
            printf("DHCP.domain[%d] %s\n", strlen(gpt_dhcp[channel]->domain), gpt_dhcp[channel]->domain);
            printf("\n");
#endif /*#if defined(DEBUG_PRINT)*/
            break;
        case DHCP_EV_LEASE_OVER:
            break;

        case DHCP_EV_INIT:
            break;

        case DHCP_EV_INIT_REBOOT:
            break;

        case DHCP_EV_APIPA:
            gpt_dhcp[channel] = (DHCP*)param;
#if defined(DEBUG_PRINT)
            printf("DHCP.ipaddr[4]   %d.%d.%d.%d\n",
                   gpt_dhcp[channel]->ipaddr[0], gpt_dhcp[channel]->ipaddr[1],
                   gpt_dhcp[channel]->ipaddr[2], gpt_dhcp[channel]->ipaddr[3]);
            printf("DHCP.maskaddr[4] %d.%d.%d.%d\n",
                   gpt_dhcp[channel]->maskaddr[0], gpt_dhcp[channel]->maskaddr[1],
                   gpt_dhcp[channel]->maskaddr[2], gpt_dhcp[channel]->maskaddr[3]);
            printf("DHCP.gwaddr[4]   %d.%d.%d.%d\n",
                   gpt_dhcp[channel]->gwaddr[0], gpt_dhcp[channel]->gwaddr[1],
                   gpt_dhcp[channel]->gwaddr[2], gpt_dhcp[channel]->gwaddr[3]);
            printf("DHCP.dnsaddr[4]  %d.%d.%d.%d\n",
                   gpt_dhcp[channel]->dnsaddr[0], gpt_dhcp[channel]->dnsaddr[1],
                   gpt_dhcp[channel]->dnsaddr[2], gpt_dhcp[channel]->dnsaddr[3]);
            printf("DHCP.dnsaddr2[4] %d.%d.%d.%d\n",
                   gpt_dhcp[channel]->dnsaddr2[0], gpt_dhcp[channel]->dnsaddr2[1],
                   gpt_dhcp[channel]->dnsaddr2[2], gpt_dhcp[channel]->dnsaddr2[3]);
            printf("DHCP.macaddr[6]  %02X:%02X:%02X:%02X:%02X:%02X\n",
                   gpt_dhcp[channel]->macaddr[0],  gpt_dhcp[channel]->macaddr[1],  gpt_dhcp[channel]->macaddr[2],
                   gpt_dhcp[channel]->macaddr[3],  gpt_dhcp[channel]->macaddr[4],  gpt_dhcp[channel]->macaddr[5]);
            printf("DHCP.domain[%d] %s\n", strlen(gpt_dhcp[channel]->domain), gpt_dhcp[channel]->domain);
            printf("\n");
#endif /*#if defined(DEBUG_PRINT)*/
            break;

        case DHCP_EV_NAK:
            break;

        case DHCP_EV_FATAL_ERROR:
            break;

        case DHCP_EV_PLEASE_RESET:
            break;

        default:
            break;
    }
    return 0;
}
