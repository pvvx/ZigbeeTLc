/********************************************************************************************************
 * @file    mac_phy.c
 *
 * @brief   This is the source file for mac_phy
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/

/**********************************************************************
 * INCLUDES
 */
#include "zb_common.h"
#include "compiler.h"


#define RF_SRX_MODE                             0

/**********************************************************************
 * LOCAL CONSTANTS
 */
#define	LOGICCHANNEL_TO_PHYSICAL(p)             (((p)-10)*5)

#define RF_DROP_REASON_INVALID_CRC              0x01
#define RF_DROP_REASON_RF_BUSY                  0x02
#define RF_DROP_REASON_EXPECT_ACK               0x03
#define RF_DROP_REASON_INVALIC_BEACON           0x04
#define RF_DROP_REASON_FILTER_PANID             0x05
#define RF_DROP_REASON_FILTER_DSTADDR           0x06
#define RF_DROP_REASON_FILTER_LEN               0x07
#define RF_DROP_REASON_INVALIC_FRAME_TYPE       0x08

/**********************************************************************
 * LOCAL TYPEDEFS
 */
typedef enum {
    RF_GAIN_MODE_AUTO,
    RF_GAIN_MODE_MANU_MAX,
} rf_rxGainMode_t;

/**********************************************************************
 * GLOBAL VARIABLES
 */
u8 g_zb_txPowerSet = ZB_DEFAULT_TX_POWER_IDX;

/**********************************************************************
 * LOCAL VARIABLES
 */
_attribute_aligned_(4) u8 rf_tx_buf[ZB_RADIO_TX_HDR_LEN + 127];
_attribute_aligned_(4) u8 rf_ack_buf[ZB_RADIO_TX_HDR_LEN + 7];

u8 *rf_rxBuf = NULL;
u8 rfMode = RF_STATE_TX;
volatile u8 rf_busyFlag = 0;

volatile s8 soft_rssi;
volatile s32 sum_rssi, cnt_rssi = 1;

/* RSSI of the last MAC ACK received while waiting for one: for an end device the parent's answer (LCD signal level) */
#if USE_DISPLAY_SIGNAL_LEVEL

g_ack_t g_ack = {
	.rssi = -110
};

#endif

u8 fPaEn = 0;
u32 rf_pa_txen_pin = 0;
u32 rf_pa_rxen_pin = 0;

u8 fPtaEn = 0;
u32 rf_pta_priority_pin = 0;
u32 rf_pta_active_pin = 0;

static bool isWLANActive(void);

/**********************************************************************
 * LOCAL FUNCTIONS
 */
#if RF_SRX_MODE
//switch single RX to manual TX
#define ZB_SWITCH_TO_TXMODE()   do{ \
                                    if (rfMode != RF_STATE_TX || ZB_RADIO_TRX_STA_GET() != RF_MODE_TX) { \
                                        rfMode = RF_STATE_TX; \
                                        if (fPaEn) { \
                                            drv_gpio_write(rf_pa_txen_pin, 1); \
                                            drv_gpio_write(rf_pa_rxen_pin, 0); \
                                        } \
                                        if (fPtaEn) { \
                                            drv_gpio_write(rf_pta_priority_pin, 1); \
                                        } \
                                        ZB_RADIO_TRX_OFF_AUTO_MODE(); \
                                        ZB_RADIO_TRX_SWITCH(RF_MODE_TX, LOGICCHANNEL_TO_PHYSICAL(rf_getChannel())); \
                                    } \
                                }while(0)

//switch manual TX to single RX
#define ZB_SWITCH_TO_RXMODE()   do{ \
                                    if (rfMode != RF_STATE_RX || ZB_RADIO_TRX_STA_GET() != RF_MODE_AUTO) { \
                                        rfMode = RF_STATE_RX; \
                                        if (fPaEn) { \
                                            drv_gpio_write(rf_pa_txen_pin, 0); \
                                            drv_gpio_write(rf_pa_rxen_pin, 1); \
                                        } \
                                        if (fPtaEn) { \
                                            drv_gpio_write(rf_pta_priority_pin, 0); \
                                        } \
                                        ZB_RADIO_TRX_SWITCH(RF_MODE_AUTO, LOGICCHANNEL_TO_PHYSICAL(rf_getChannel())); \
                                    } \
                                    ZB_RADIO_SRX_START(clock_time()); \
                                }while(0)
#else
//switch manual RX to manual TX
#define ZB_SWITCH_TO_TXMODE()   do{ \
                                    if (rfMode != RF_STATE_TX || ZB_RADIO_TRX_STA_GET() != RF_MODE_TX) { \
                                        rfMode = RF_STATE_TX; \
                                        if (fPaEn) { \
                                            drv_gpio_write(rf_pa_txen_pin, 1); \
                                            drv_gpio_write(rf_pa_rxen_pin, 0); \
                                        } \
                                        if (fPtaEn) { \
                                            drv_gpio_write(rf_pta_priority_pin, 1); \
                                        } \
                                        ZB_RADIO_TRX_SWITCH(RF_MODE_TX, LOGICCHANNEL_TO_PHYSICAL(rf_getChannel())); \
                                    } \
                                }while(0)

//switch manual TX to manual RX
#define ZB_SWITCH_TO_RXMODE()   do{ \
                                    if (rfMode != RF_STATE_RX || ZB_RADIO_TRX_STA_GET() != RF_MODE_RX) { \
                                        rfMode = RF_STATE_RX; \
                                        if (fPaEn) { \
                                            drv_gpio_write(rf_pa_txen_pin, 0); \
                                            drv_gpio_write(rf_pa_rxen_pin, 1); \
                                        } \
                                        if (fPtaEn) { \
                                            drv_gpio_write(rf_pta_priority_pin, 0); \
                                        } \
                                        ZB_RADIO_TRX_SWITCH(RF_MODE_RX, LOGICCHANNEL_TO_PHYSICAL(rf_getChannel())); \
                                    } \
                                }while(0)
#endif

#define ZB_SWITCH_TO_OFFMODE()  do{ \
                                    if (rfMode != RF_STATE_OFF || ZB_RADIO_TRX_STA_GET() != RF_MODE_OFF) { \
                                        rfMode = RF_STATE_OFF; \
                                        rf_paShutDown(); \
                                        if (fPtaEn) { \
                                            drv_gpio_write(rf_pta_priority_pin, 0); \
                                        } \
                                        ZB_RADIO_TRX_SWITCH(RF_MODE_OFF, LOGICCHANNEL_TO_PHYSICAL(rf_getChannel())); \
                                    } \
                                }while(0)

_attribute_ram_code_ u32 mac_currentTickGet(void)
{
     return clock_time();
}

/*********************************************************************
 * @fn      rf_reset
 *
 * @brief   Reset RF module and do the RF power up.
 *
 * @param   none
 *
 * @return  none
 */
void rf_reset(void)
{
    rf_setTrxState(RF_STATE_OFF);

    rf_setTxPower(g_zb_txPowerSet);

    rf_setChannel(g_zbMacPib.phyChannelCur);

    if (rf_rxBuf) {
        rf_setRxBuf(rf_rxBuf);
    } else {
        rf_setRxBuf(tl_getRxBuf());
    }

    ZB_RADIO_TRX_CFG(RF_PKT_BUFF_LEN);

    ZB_RADIO_RX_ENABLE;
    ZB_RADIO_TX_ENABLE;
    ZB_TIMESTAMP_ENABLE;
}

static void rf_edDetect(void)
{
    s8 rssi;
    rssi = ZB_RADIO_RSSI_GET();

    //soft_rssi = rssi;//(rssi + soft_rssi)/2;
    sum_rssi += rssi;
    if (++cnt_rssi >= 0xfffffe) {
    	sum_rssi = sum_rssi / cnt_rssi;
    	cnt_rssi = 1;
    }
}

static void rf_mac_ack_build(void)
{
    /* ack buf */
    memset(rf_ack_buf, 0, 12);

    ZB_RADIO_DMA_HDR_BUILD(rf_ack_buf, 3);

    rf_ack_buf[4] = 5;
    rf_ack_buf[5] = 0x02;
    rf_ack_buf[6] = 0x00;
}

/*********************************************************************
 * @fn      rf_init
 *
 * @brief   Initialize RF module and do the RF power up.
 *
 * @param   none
 *
 * @return  none
 */
void rf_init(void)
{
    rf_reset();

    rf_mac_ack_build();

    /* Register ED-Scan polling function, but disable it at beginning. */
    ev_on_poll(EV_POLL_ED_DETECT, rf_edDetect);
    ev_disable_poll(EV_POLL_ED_DETECT);
}

/*********************************************************************
 * @fn      rf_setRxBuf
 *
 * @brief   Set RX buffer to dma and enable DMA.
 *
 * @param   pBuf - the allocated rx buffer
 *
 * @return  none
 */
void rf_setRxBuf(u8 *pBuf)
{
    rf_rxBuf = pBuf;
    ZB_RADIO_RX_BUF_CLEAR(rf_rxBuf);
    ZB_RADIO_RX_BUF_SET(rf_rxBuf);//todo: 826x/8258 need fix rf driver
}

/*********************************************************************
 * @fn      rf_TrxStateGet
 *
 * @brief   Get current TRX state.
 *
 * @param   none
 *
 * @return  state
 */
u8 rf_TrxStateGet(void)
{
    return rfMode;
}

/*********************************************************************
 * @fn      rf_setTrxState
 *
 * @brief   Go to specified state and set related register.
 *
 * @param   state - Specified state
 *
 * @return  none
 */
_attribute_ram_code_ void rf_setTrxState(u8 state)
{
#ifndef WIN32
    if (RF_STATE_RX == state || RF_STATE_ED == state) {
        if (TL_ZB_MAC_STATUS_GET() == ZB_MAC_STATE_ACTIVE_SCAN || RF_STATE_ED == state) {
            ZB_RADIO_MODE_AUTO_GAIN();
        } else {
            ZB_RADIO_MODE_MAX_GAIN();
        }

        ZB_SWITCH_TO_RXMODE();
    } else if (RF_STATE_TX == state) {
        ZB_SWITCH_TO_TXMODE();
        WaitUs(ZB_TX_WAIT_US);
    } else {
        /* Close RF */
        ZB_SWITCH_TO_OFFMODE();
    }
#endif  /* WIN32 */
}

/*********************************************************************
 * @fn      rf_setChannel
 *
 * @brief   Set specified channel to RF module.
 *
 * @param   chn - 11~26
 *
 * @return  none
 */
void rf_setChannel(u8 chn)
{
    if ((chn < TL_ZB_MAC_CHANNEL_START) ||
        (chn > TL_ZB_MAC_CHANNEL_STOP)) {
        return;
    }
    g_zbMacPib.phyChannelCur = chn;

    u32 r = drv_disable_irq();

    u8 phySta = ZB_RADIO_TRX_STA_GET();
    ZB_RADIO_TRX_SWITCH(phySta, LOGICCHANNEL_TO_PHYSICAL(chn));
    if (phySta == RF_MODE_RX || phySta == RF_MODE_AUTO) {
        rfMode = RF_STATE_RX;
    } else if (phySta == RF_MODE_TX) {
        rfMode = RF_STATE_TX;
    } else if (phySta == RF_MODE_OFF) {
        rfMode = RF_STATE_OFF;
    }

#if RF_SRX_MODE
    if (phySta == RF_MODE_AUTO && rfMode == RF_STATE_RX) {
        ZB_RADIO_SRX_START(clock_time());
    }
#endif

    drv_restore_irq(r);
}

/*********************************************************************
 * @fn      rf_getChannel
 *
 * @brief   Get specified channel.
 *
 * @param   none
 *
 * @return  chn
 */
inline u8 rf_getChannel(void)
{
    return g_zbMacPib.phyChannelCur;
}

/*********************************************************************
 * @fn      rf_setTxPower
 *
 * @brief   Set specified transmitted power.
 *
 * @param   txPower - Specified power
 *
 * @return  none
 */
void rf_setTxPower(u8 power)
{
    if (fPaEn) {
        ZB_RADIO_TX_POWER_SET(ZB_RADIO_TX_0DBM);
    } else {
        ZB_RADIO_TX_POWER_SET(power);
    }
}

/*********************************************************************
 * @fn      rf_getLqi
 *
 * @brief   Get calculated Link Quality value
 *
 * @param   rssi
 *
 * @return  lqi result
 */
u8 rf_getLqi(s8 rssi)
{
    rf_rxGainMode_t mode = RF_GAIN_MODE_MANU_MAX;
    if (TL_ZB_MAC_STATUS_GET() == ZB_MAC_STATE_ACTIVE_SCAN) {
        mode = RF_GAIN_MODE_AUTO;
    }

    u8 lqi = 0;
    ZB_RADIO_RSSI_TO_LQI(mode, rssi, lqi);
    return lqi;
}

/*********************************************************************
 * @fn      rf_lqi2cost
 *
 * @brief   Get calculated cost value
 *
 * @param   lqi
 *
 * @return  cost
 */
u8 rf_lqi2cost(u8 lqi)
{
    u8 cost = 0;
    ZB_LQI_TO_PATH_COST(lqi, cost);
    return cost;
}

/*********************************************************************
 * @fn      rf_startEDScan
 *
 * @brief   Start ED detect
 *
 * @param   none
 *
 * @return  none
 */
void rf_startEDScan(void)
{
#ifndef WIN32
    soft_rssi = -110;
    sum_rssi = 0;
    cnt_rssi = 0;
    ev_enable_poll(EV_POLL_ED_DETECT);
    /* Change to ED mode */
    rf_setTrxState(RF_STATE_ED);
#endif
}

/*********************************************************************
 * @fn      rf_stopEDScan
 *
 * @brief   Stop Energy Detect
 *
 * @param   none
 *
 * @return  ED result
 */
u8 rf_stopEDScan(void)
{
#ifndef WIN32
    u8 ed;

    if(cnt_rssi == 0) cnt_rssi = 1;
    soft_rssi = sum_rssi / cnt_rssi;

    ev_disable_poll(EV_POLL_ED_DETECT);/*WISE_FIX_ME*/
    /* Transfer the RSSI value to ED value */
    ed = rf_getLqi(soft_rssi);
    return ed;
#else
    return 0;
#endif
}

volatile s8 T_rssiPeak = 0;
_attribute_ram_code_ u8 rf_performCCA(void)
{
    if (isWLANActive()) {
        return PHY_CCA_BUSY;
    }

    u32 t1 = clock_time();
    s8 rssi_peak = -110;
    s8 rssi_cur = -110;
    s32 rssiSum = 0;
    s32 cnt = 1;

    rssi_cur = ZB_RADIO_RSSI_GET();
    rssiSum += rssi_cur;
    while (!clock_time_exceed(t1, 128)) {
        rssi_cur = ZB_RADIO_RSSI_GET();
        rssiSum += rssi_cur;
        cnt++;

#if RF_SRX_MODE
        if (ZB_RADIO_RX_DONE) {
            ZB_RADIO_RX_DONE_CLR;
            if (ZB_RADIO_TRX_STA_GET() == RF_MODE_AUTO) {
                ZB_RADIO_SRX_START(clock_time());
            }
        }
#endif
    }
    rssi_peak = rssiSum / cnt;
    T_rssiPeak = rssi_peak;

    if (rssi_peak > CCA_THRESHOLD || (rf_busyFlag & TX_BUSY)) {//Return if currently in TX state
        return PHY_CCA_BUSY;
    } else {
        return PHY_CCA_IDLE;
    }
}

void rf802154_tx_ready(u8 *buf, u8 len)
{
    /* Fill the telink RF header */
    ZB_RADIO_DMA_HDR_BUILD(rf_tx_buf, len);

    rf_tx_buf[4] = len + 2;
    memcpy(rf_tx_buf + 5, buf, len);
}

_attribute_ram_code_ void rf802154_tx(void)
{
    rf_setTrxState(RF_STATE_TX);

    ZB_RADIO_TX_DONE_CLR;
    ZB_RADIO_RX_DONE_CLR;
    ZB_RADIO_TX_START(rf_tx_buf);//Manual mode
}

#if PA_ENABLE
/*********************************************************************
 * @fn      rf_paInit
 *
 * @brief   Initialize PA.
 *
 * @param   none
 *
 * @param   none
 *
 * @return  none
 */
void rf_paInit(u32 TXEN_pin, u32 RXEN_pin)
{
    rf_pa_txen_pin = TXEN_pin;
    rf_pa_rxen_pin = RXEN_pin;

    drv_gpio_func_set(rf_pa_txen_pin);
    drv_gpio_output_en(rf_pa_txen_pin, 1);
    drv_gpio_write(rf_pa_txen_pin, 0);

    drv_gpio_func_set(rf_pa_rxen_pin);
    drv_gpio_output_en(rf_pa_rxen_pin, 1);
    drv_gpio_write(rf_pa_rxen_pin, 1);

    fPaEn = 1;
}
#endif

_attribute_ram_code_ void rf_paShutDown(void)
{
#if PA_ENABLE
    if (fPaEn) {
    	drv_gpio_write(rf_pa_txen_pin, 0); // PA TX_DIS
    	drv_gpio_write(rf_pa_rxen_pin, 0); // PA RX_DIS
    }
#endif
}

#if PTA_ENABLE
void rf_ptaInit(u32 ZB_priorityPin, u32 WLAN_activePin)
{
    rf_pta_priority_pin = ZB_priorityPin;
    rf_pta_active_pin = WLAN_activePin;

    drv_gpio_func_set(rf_pta_priority_pin);
    drv_gpio_output_en(rf_pta_priority_pin, 1);
    drv_gpio_input_en(rf_pta_priority_pin, 0);
    drv_gpio_write(rf_pta_priority_pin, 0);

    drv_gpio_func_set(rf_pta_active_pin);
    drv_gpio_output_en(rf_pta_active_pin, 0);
    drv_gpio_input_en(rf_pta_active_pin, 1);

    fPtaEn = 1;
}
#endif

_attribute_ram_code_ bool isWLANActive(void)
{
#if PTA_ENABLE
    if (fPtaEn) {
        if (drv_gpio_read(rf_pta_active_pin)) {
            WaitUs(2);
            if (drv_gpio_read(rf_pta_active_pin)) {
                return TRUE;
            }
        }
    }
#endif

    return FALSE;
}

/*********************************************************************
 * @fn      rf_rx_irq_handler
 *
 * @brief   RX irq handler
 *
 * @param   none
 *
 * @return  none
 */
#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
_attribute_ram_code_ __attribute__((optimize("-Os")))
#else
_attribute_ram_code_
#endif
void rf_rx_irq_handler(void)
{
    u8 *p = rf_rxBuf;
    u8 fAck = 0;
    u8 fDrop = 0;
    s32 txTime = 0;
    s32 txDelayUs = 0;

    if (RF_DMA_BUSY()) {
    	return;
    }

    ZB_RADIO_RX_DISABLE;

    g_sysDiags.macRxIrqCnt++;

    /*************************************************************
     * rf_busyFlag & TX_BUSY, means a packet received before during csma delay call back interrupt called,
     * should drop this packet
     */
    if ((!ZB_RADIO_CRC_OK(p)) || (!ZB_RADIO_PACKET_LENGTH_OK(p)) || (rf_busyFlag & TX_BUSY)) {
    	/* diagnostics MAC layer receive error packet */
    	g_sysDiags.macRxCrcFail++;

    	ZB_RADIO_RX_BUF_CLEAR(rf_rxBuf);
        ZB_RADIO_RX_ENABLE;
#if RF_SRX_MODE
        ZB_SWITCH_TO_RXMODE();
#endif
        return;
    }

    /* Parse necessary field to be used later */
    u8 len = (u8)ZB_RADIO_ACTUAL_PAYLOAD_LEN(p);
    u8 *macPld = p + ZB_RADIO_RX_HDR_LEN;

    /*----------------------------------------------------------
     *  Do the filter
     */
    u8 *pSrcAddr = zb_macDataFilter(macPld, len, &fDrop, &fAck);
    if (fDrop) {
        /* Drop the packet and recover the DMA */
    	ZB_RADIO_RX_BUF_CLEAR(rf_rxBuf);
        ZB_RADIO_RX_ENABLE;
#if RF_SRX_MODE
        ZB_SWITCH_TO_RXMODE();
#endif
        return;
    }

    /* switch to tx in advance to let the pll stable */
    if (macPld[0] & MAC_FCF_ACK_REQ_BIT) {
        ZB_SWITCH_TO_TXMODE();
        txTime = clock_time();
    }

    /* if don't have enough buffer, use current rxBuf, and drop it */
    u8 *rxNextBuf = tl_getRxBuf();
    if (!rxNextBuf) {
        if (macPld[0] & MAC_FCF_ACK_REQ_BIT) {
            ZB_SWITCH_TO_RXMODE();
        }

        /* diagnostics PHY to MAC queue limit */
        g_sysDiags.phytoMACqueuelimitreached++;

        ZB_RADIO_RX_BUF_CLEAR(rf_rxBuf);
        ZB_RADIO_RX_ENABLE;
#if RF_SRX_MODE
        ZB_SWITCH_TO_RXMODE();
#endif
        return;
    }

    /* Use the backup buffer to receive next packet */
    rf_rxBuf = rxNextBuf;
    ZB_RADIO_RX_BUF_CLEAR(rf_rxBuf);
    ZB_RADIO_RX_BUF_SET(rf_rxBuf);

    /*----------------------------------------------------------
     *  Send ACK
     */
    if (macPld[0] & MAC_FCF_ACK_REQ_BIT) {
        rf_ack_buf[ZB_RADIO_TX_HDR_LEN+2] = macPld[2];  //seqnno
        rf_ack_buf[ZB_RADIO_TX_HDR_LEN] = 0x02;

#if defined ZB_COORDINATOR_ROLE || defined ZB_ROUTER_ROLE
        u16 fcf = (macPld[1] << 8) | macPld[0];     // frame control
        u8  pldOft = tl_zbMacHdrSize(fcf);
        u8  frameType = (fcf & MAC_FCF_FRAME_TYPE_MASK) >> MAC_FCF_FRAME_TYPE_POS;

        bool cmdDataReq = ((frameType == MAC_FRAME_COMMAND) && (macPld[pldOft] == MAC_CMD_DATA_REQUEST));

        /* if has pending data, set the pending bit as 1 */
        u8 srcAddrMode = ZB_ADDR_NO_ADDR;
        if ((macPld[1] & MAC_FCF_SRC_ADDR_BIT) == 0x80) {
            srcAddrMode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
        } else if ((macPld[1] & MAC_FCF_SRC_ADDR_BIT) == 0xc0) {
            srcAddrMode = ZB_ADDR_64BIT_DEV;
        }
        if (cmdDataReq && srcAddrMode) {
            extern u8 tl_zbMacPendingDataCheck(u8 addrMode, u8 *addr, u8 send);
            if (MAC_SUCCESS == tl_zbMacPendingDataCheck(srcAddrMode, pSrcAddr, 1)) {
                rf_ack_buf[ZB_RADIO_TX_HDR_LEN] |= (1 << 4);// (rf_framePending<<4 );
            }
        }
#else
        (void)pSrcAddr;
#endif

        txDelayUs = (clock_time() - txTime) / S_TIMER_CLOCK_1US;
        if (txDelayUs < ZB_TX_WAIT_US) {
            WaitUs(ZB_TX_WAIT_US - txDelayUs);
        }

        /* wait until tx done,
         * disable tx irq here, rfMode still is RF_STATE_RX;
         * */
        /* clear rf interrupt mask bit*/
        ZB_RADIO_IRQ_MASK_CLR;
        /* start to send ack */
        ZB_RADIO_TX_START(rf_ack_buf);//Manual Mode

        /* wait until tx done or timeout */
        u32 cur_tick = clock_time();
        while(!ZB_RADIO_TX_DONE && !clock_time_exceed(cur_tick, 1000));

        /* clear the tx done status */
        ZB_RADIO_TX_DONE_CLR;
        /* set interrupt mask bit again */
        ZB_RADIO_IRQ_MASK_SET;
        /* rf is set to rx mode again */
        ZB_SWITCH_TO_RXMODE();
    }

    /* enable rf rx dma again */
    ZB_RADIO_RX_ENABLE;
#if RF_SRX_MODE
    ZB_SWITCH_TO_RXMODE();
#endif
#if USE_DISPLAY_SIGNAL_LEVEL
    if (fAck) {
        g_ack.rssi = ZB_RADION_PKT_RSSI_GET(p) - 110;
        g_ack.cnt++;
    }
#endif
    /* zb_mac_receive_data handler */
    zb_macDataRecvHandler(p, macPld, len, fAck, ZB_RADIO_TIMESTAMP_GET(p), ZB_RADION_PKT_RSSI_GET(p) - 110);
}


/*********************************************************************
 * @fn      rf_tx_irq_handler
 *
 * @brief   Tx Interrupt handler for RF module.
 *
 * @param   none
 *
 * @return  none
 */
#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
_attribute_ram_code_ __attribute__((optimize("-Os")))
#else
_attribute_ram_code_
#endif
void rf_tx_irq_handler(void)
{
    rf_busyFlag &= ~TX_BUSY;//Clear TX busy flag after receive TX done signal

    g_sysDiags.macTxIrqCnt++;

    ZB_SWITCH_TO_RXMODE();

    zb_macDataSendHandler();
}
