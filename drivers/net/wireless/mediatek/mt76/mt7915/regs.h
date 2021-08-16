/* SPDX-License-Identifier: ISC */
/* Copyright (C) 2020 MediaTek Inc. */

#ifndef __MT7915_REGS_H
#define __MT7915_REGS_H

/* MCU WFDMA0 */
#define MT_MCU_WFDMA0_BASE		0x2000
#define MT_MCU_WFDMA0(ofs)		(MT_MCU_WFDMA0_BASE + (ofs))
#define MT_MCU_WFDMA0_DUMMY_CR		MT_MCU_WFDMA0(0x120)

/* MCU WFDMA1 */
#define MT_MCU_WFDMA1_BASE		0x3000
#define MT_MCU_WFDMA1(ofs)		(MT_MCU_WFDMA1_BASE + (ofs))

#define MT_MCU_INT_EVENT		MT_MCU_WFDMA1(0x108)
#define MT_MCU_INT_EVENT_DMA_STOPPED	BIT(0)
#define MT_MCU_INT_EVENT_DMA_INIT	BIT(1)
#define MT_MCU_INT_EVENT_SER_TRIGGER	BIT(2)
#define MT_MCU_INT_EVENT_RESET_DONE	BIT(3)

#define MT_PLE_BASE			0x8000
#define MT_PLE(ofs)			(MT_PLE_BASE + (ofs))

/* Modify whether txfree struct returns latency or txcount. */
#define MT_PLE_HOST_RPT0               MT_PLE(0x030)
#define MT_PLE_HOST_RPT0_TX_LATENCY    BIT(3)

#define MT_PLE_FL_Q0_CTRL		MT_PLE(0x1b0)
#define MT_PLE_FL_Q1_CTRL		MT_PLE(0x1b4)
#define MT_PLE_FL_Q2_CTRL		MT_PLE(0x1b8)
#define MT_PLE_FL_Q3_CTRL		MT_PLE(0x1bc)

#define MT_FL_Q_EMPTY			0x0b0
#define MT_FL_Q0_CTRL			0x1b0
#define MT_FL_Q2_CTRL			0x1b8
#define MT_FL_Q3_CTRL			0x1bc

#define MT_PLE_FREEPG_CNT		MT_PLE(0x100)
#define MT_PLE_FREEPG_HEAD_TAIL		MT_PLE(0x104)
#define MT_PLE_PG_HIF_GROUP		MT_PLE(0x110)
#define MT_PLE_HIF_PG_INFO		MT_PLE(0x114)
#define MT_PLE_AC_QEMPTY(ac, n)		MT_PLE(0x500 + 0x40 * (ac) + \
					       ((n) << 2))
#define MT_PLE_AMSDU_PACK_MSDU_CNT(n)	MT_PLE(0x10e0 + ((n) << 2))

#define MT_PSE_BASE			0xc000
#define MT_PSE(ofs)			(MT_PSE_BASE + (ofs))

#define MT_MDP_BASE			0xf000
#define MT_MDP(ofs)			(MT_MDP_BASE + (ofs))

#define MT_MDP_DCR0			MT_MDP(0x000)
#define MT_MDP_DCR0_DAMSDU_EN		BIT(15)

#define MT_MDP_DCR1			MT_MDP(0x004)
#define MT_MDP_DCR1_MAX_RX_LEN		GENMASK(15, 3)

#define MT_MDP_BNRCFR0(_band)		MT_MDP(0x070 + ((_band) << 8))
#define MT_MDP_RCFR0_MCU_RX_MGMT	GENMASK(5, 4)
#define MT_MDP_RCFR0_MCU_RX_CTL_NON_BAR	GENMASK(7, 6)
#define MT_MDP_RCFR0_MCU_RX_CTL_BAR	GENMASK(9, 8)

#define MT_MDP_BNRCFR1(_band)		MT_MDP(0x074 + ((_band) << 8))
#define MT_MDP_RCFR1_MCU_RX_BYPASS	GENMASK(23, 22)
#define MT_MDP_RCFR1_RX_DROPPED_UCAST	GENMASK(28, 27)
#define MT_MDP_RCFR1_RX_DROPPED_MCAST	GENMASK(30, 29)
#define MT_MDP_TO_HIF			0
#define MT_MDP_TO_WM			1

/* TMAC: band 0(0x21000), band 1(0xa1000) */
#define MT_WF_TMAC_BASE(_band)		((_band) ? 0xa1000 : 0x21000)
#define MT_WF_TMAC(_band, ofs)		(MT_WF_TMAC_BASE(_band) + (ofs))

#define MT_TMAC_TCR0(_band)		MT_WF_TMAC(_band, 0)
#define MT_TMAC_TCR0_TX_BLINK		GENMASK(7, 6)
#define MT_TMAC_TCR0_TBTT_STOP_CTRL	BIT(25)

#define MT_TMAC_CDTR(_band)		MT_WF_TMAC(_band, 0x090)
#define MT_TMAC_ODTR(_band)		MT_WF_TMAC(_band, 0x094)
#define MT_TIMEOUT_VAL_PLCP		GENMASK(15, 0)
#define MT_TIMEOUT_VAL_CCA		GENMASK(31, 16)

#define MT_TMAC_ATCR(_band)		MT_WF_TMAC(_band, 0x098)
#define MT_TMAC_ATCR_TXV_TOUT		GENMASK(7, 0)

#define MT_TMAC_TRCR0(_band)		MT_WF_TMAC(_band, 0x09c)
#define MT_TMAC_TRCR0_TR2T_CHK		GENMASK(8, 0)
#define MT_TMAC_TRCR0_I2T_CHK		GENMASK(24, 16)

#define MT_TMAC_ICR0(_band)		MT_WF_TMAC(_band, 0x0a4)
#define MT_IFS_EIFS_OFDM			GENMASK(8, 0)
#define MT_IFS_RIFS			GENMASK(14, 10)
#define MT_IFS_SIFS			GENMASK(22, 16)
#define MT_IFS_SLOT			GENMASK(30, 24)

#define MT_TMAC_ICR1(_band)		MT_WF_TMAC(_band, 0x0b4)
#define MT_IFS_EIFS_CCK			GENMASK(8, 0)

#define MT_TMAC_CTCR0(_band)			MT_WF_TMAC(_band, 0x0f4)
#define MT_TMAC_CTCR0_INS_DDLMT_REFTIME		GENMASK(5, 0)
#define MT_TMAC_CTCR0_INS_DDLMT_EN		BIT(17)
#define MT_TMAC_CTCR0_INS_DDLMT_VHT_SMPDU_EN	BIT(18)

#define MT_TMAC_TFCR0(_band)		MT_WF_TMAC(_band, 0x1e0)

#define MT_WF_DMA_BASE(_band)		((_band) ? 0xa1e00 : 0x21e00)
#define MT_WF_DMA(_band, ofs)		(MT_WF_DMA_BASE(_band) + (ofs))

#define MT_DMA_DCR0(_band)		MT_WF_DMA(_band, 0x000)
#define MT_DMA_DCR0_MAX_RX_LEN		GENMASK(15, 3)
#define MT_DMA_DCR0_RXD_G5_EN		BIT(23)

/* ETBF: band 0(0x24000), band 1(0xa4000) */
#define MT_WF_ETBF_BASE(_band)		((_band) ? 0xa4000 : 0x24000)
#define MT_WF_ETBF(_band, ofs)		(MT_WF_ETBF_BASE(_band) + (ofs))

#define MT_ETBF_TX_NDP_BFRP(_band)	MT_WF_ETBF(_band, 0x040)
#define MT_ETBF_TX_FB_CPL		GENMASK(31, 16)
#define MT_ETBF_TX_FB_TRI		GENMASK(15, 0)

#define MT_ETBF_RX_FB_CONT(_band)	MT_WF_ETBF(_band, 0x068)
#define MT_ETBF_RX_FB_BW		GENMASK(7, 6)
#define MT_ETBF_RX_FB_NC		GENMASK(5, 3)
#define MT_ETBF_RX_FB_NR		GENMASK(2, 0)

#define MT_ETBF_TX_APP_CNT(_band)	MT_WF_ETBF(_band, 0x0f0)
#define MT_ETBF_TX_IBF_CNT		GENMASK(31, 16)
#define MT_ETBF_TX_EBF_CNT		GENMASK(15, 0)

#define MT_ETBF_RX_FB_CNT(_band)	MT_WF_ETBF(_band, 0x0f8)
#define MT_ETBF_RX_FB_ALL		GENMASK(31, 24)
#define MT_ETBF_RX_FB_HE		GENMASK(23, 16)
#define MT_ETBF_RX_FB_VHT		GENMASK(15, 8)
#define MT_ETBF_RX_FB_HT		GENMASK(7, 0)

/* LPON: band 0(0x24200), band 1(0xa4200) */
#define MT_WF_LPON_BASE(_band)		((_band) ? 0xa4200 : 0x24200)
#define MT_WF_LPON(_band, ofs)		(MT_WF_LPON_BASE(_band) + (ofs))

#define MT_LPON_UTTR0(_band)		MT_WF_LPON(_band, 0x080)
#define MT_LPON_UTTR1(_band)		MT_WF_LPON(_band, 0x084)

#define MT_LPON_TCR(_band, n)		MT_WF_LPON(_band, 0x0a8 + (n) * 4)
#define MT_LPON_TCR_SW_MODE		GENMASK(1, 0)
#define MT_LPON_TCR_SW_WRITE		BIT(0)
#define MT_LPON_TCR_SW_ADJUST		BIT(1)
#define MT_LPON_TCR_SW_READ		GENMASK(1, 0)

/* MIB: band 0(0x24800), band 1(0xa4800) */
/* These counters are (mostly?) clear-on-read.  So, some should not
 * be read at all in case firmware is already reading them.  These
 * are commented with 'DNR' below.  The DNR stats will be read by querying
 * the firmware API for the appropriate message.  For counters the driver
 * does read, the driver should accumulate the counters.
 */
#define MT_WF_MIB_BASE(_band)		((_band) ? 0xa4800 : 0x24800)
#define MT_WF_MIB(_band, ofs)		(MT_WF_MIB_BASE(_band) + (ofs))

#define MT_MIB_SDR0(_band)		MT_WF_MIB(_band, 0x010)
#define MT_MIB_SDR0_BERACON_TX_CNT_MASK	GENMASK(15, 0)

#define MT_MIB_SDR3(_band)		MT_WF_MIB(_band, 0x014)
#define MT_MIB_SDR3_FCS_ERR_MASK	GENMASK(15, 0)

#define MT_MIB_SDR4(_band)		MT_WF_MIB(_band, 0x018)
#define MT_MIB_SDR4_RX_FIFO_FULL_MASK	GENMASK(15, 0)
#define MT_MIB_SDR4_RX_OOR_MASK		GENMASK(23, 16)

/* rx mpdu counter, full 32 bits */
#define MT_MIB_SDR5(_band)		MT_WF_MIB(_band, 0x01c)

#define MT_MIB_SDR6(_band)		MT_WF_MIB(_band, 0x020)
#define MT_MIB_SDR6_CHANNEL_IDL_CNT_MASK	GENMASK(15, 0)

#define MT_MIB_SDR7(_band)		MT_WF_MIB(_band, 0x024)
#define MT_MIB_SDR7_RX_VECTOR_MISMATCH_CNT_MASK	GENMASK(15, 0)

#define MT_MIB_SDR8(_band)		MT_WF_MIB(_band, 0x028)
#define MT_MIB_SDR8_RX_DELIMITER_FAIL_CNT_MASK	GENMASK(15, 0)

/* aka CCA_NAV_TX_TIME */
#define MT_MIB_SDR9_DNR(_band)		MT_WF_MIB(_band, 0x02c)
#define MT_MIB_SDR9_CCA_BUSY_TIME_MASK	GENMASK(23, 0)

#define MT_MIB_SDR10_DNR(_band)		MT_WF_MIB(_band, 0x030)
#define MT_MIB_SDR10_MRDY_COUNT_MASK	GENMASK(25, 0)

#define MT_MIB_SDR11(_band)		MT_WF_MIB(_band, 0x034)
#define MT_MIB_SDR11_RX_LEN_MISMATCH_CNT_MASK	GENMASK(15, 0)

/* tx ampdu cnt, full 32 bits */
#define MT_MIB_SDR12(_band)		MT_WF_MIB(_band, 0x038)

#define MT_MIB_SDR13(_band)		MT_WF_MIB(_band, 0x03c)
#define MT_MIB_SDR13_TX_STOP_Q_EMPTY_CNT_MASK	GENMASK(15, 0)

/* counts all mpdus in ampdu, regardless of success */
#define MT_MIB_SDR14(_band)		MT_WF_MIB(_band, 0x040)
#define MT_MIB_SDR14_TX_MPDU_ATTEMPTS_CNT_MASK	GENMASK(23, 0)

/* counts all successfully tx'd mpdus in ampdu */
#define MT_MIB_SDR15(_band)		MT_WF_MIB(_band, 0x044)
#define MT_MIB_SDR15_TX_MPDU_SUCCESS_CNT_MASK	GENMASK(23, 0)

/* in units of 'us' */
#define MT_MIB_SDR16_DNR(_band)		MT_WF_MIB(_band, 0x048)
#define MT_MIB_SDR16_PRIMARY_CCA_BUSY_TIME_MASK	GENMASK(23, 0)

#define MT_MIB_SDR17_DNR(_band)		MT_WF_MIB(_band, 0x04c)
#define MT_MIB_SDR17_SECONDARY_CCA_BUSY_TIME_MASK	GENMASK(23, 0)

#define MT_MIB_SDR18(_band)		MT_WF_MIB(_band, 0x050)
#define MT_MIB_SDR18_PRIMARY_ENERGY_DETECT_TIME_MASK	GENMASK(23, 0)

/* units are us */
#define MT_MIB_SDR19_DNR(_band)		MT_WF_MIB(_band, 0x054)
#define MT_MIB_SDR19_CCK_MDRDY_TIME_MASK	GENMASK(23, 0)

#define MT_MIB_SDR20_DNR(_band)		MT_WF_MIB(_band, 0x058)
#define MT_MIB_SDR20_OFDM_VHT_MDRDY_TIME_MASK	GENMASK(23, 0)

#define MT_MIB_SDR21_DNR(_band)		MT_WF_MIB(_band, 0x05c)
#define MT_MIB_SDR20_GREEN_MDRDY_TIME_MASK	GENMASK(23, 0)

/* rx ampdu count, 32-bit */
#define MT_MIB_SDR22(_band)		MT_WF_MIB(_band, 0x060)

/* rx ampdu bytes count, 32-bit */
#define MT_MIB_SDR23(_band)		MT_WF_MIB(_band, 0x064)

/* rx ampdu valid subframe count */
#define MT_MIB_SDR24(_band)		MT_WF_MIB(_band, 0x068)
#define MT_MIB_SDR24_RX_AMPDU_SF_CNT_MASK	GENMASK(23, 0)

/* rx ampdu valid subframe bytes count, 32bits */
#define MT_MIB_SDR25(_band)		MT_WF_MIB(_band, 0x06c)

/* remaining windows protected stats */
#define MT_MIB_SDR27(_band)		MT_WF_MIB(_band, 0x074)
#define MT_MIB_SDR27_TX_RWP_FAIL_CNT_MASK	GENMASK(15, 0)

#define MT_MIB_SDR28(_band)		MT_WF_MIB(_band, 0x078)
#define MT_MIB_SDR28_TX_RWP_NEED_CNT_MASK	GENMASK(15, 0)

#define MT_MIB_SDR29(_band)		MT_WF_MIB(_band, 0x07c)
#define MT_MIB_SDR29_RX_PFDROP_CNT_MASK	GENMASK(7, 0)

#define MT_MIB_SDR30(_band)		MT_WF_MIB(_band, 0x080)
#define MT_MIB_SDR30_RX_VEC_QUEUE_OVERFLOW_DROP_CNT_MASK	GENMASK(15, 0)

/* rx blockack count, 32 bits */
#define MT_MIB_SDR31(_band)		MT_WF_MIB(_band, 0x084)

#define MT_MIB_SDR32(_band)		MT_WF_MIB(_band, 0x088)
#define MT_MIB_SDR32_TX_PKT_EBF_CNT_MASK	GENMASK(15, 0)

#define MT_MIB_SDR33(_band)		MT_WF_MIB(_band, 0x08c)
#define MT_MIB_SDR33_TX_PKT_IBF_CNT_MASK	GENMASK(15, 0)

#define MT_MIB_SDR34(_band)		MT_WF_MIB(_band, 0x090)
#define MT_MIB_MU_BF_TX_CNT		GENMASK(15, 0)

/* 36, 37 both DNR */

#define MT_MIB_SDR38(_band)		MT_WF_MIB(_band, 0x0d0)
#define MT_MIB_CTRL_TX_CNT		GENMASK(23, 0)

#define MT_MIB_SDR39(_band)		MT_WF_MIB(_band, 0x0d4)
#define MT_MIB_MGT_RETRY_CNT		GENMASK(23, 0)

#define MT_MIB_SDR40(_band)		MT_WF_MIB(_band, 0x0d8)
#define MT_MIB_DATA_RETRY_CNT		GENMASK(23, 0)

#define MT_MIB_SDR42(_band)		MT_WF_MIB(_band, 0x0e0)
#define MT_MIB_RX_PARTIAL_BEACON_BSSID0	GENMASK(15, 0)
#define MT_MIB_RX_PARTIAL_BEACON_BSSID1	GENMASK(31, 16)

#define MT_MIB_SDR43(_band)		MT_WF_MIB(_band, 0x0e4)
#define MT_MIB_RX_PARTIAL_BEACON_BSSID2	GENMASK(15, 0)
#define MT_MIB_RX_PARTIAL_BEACON_BSSID3	GENMASK(31, 16)

/* This counter shall increment when  PPDUs dropped by the oppo_ps_rx_dis
 * mechanism
 */
#define MT_MIB_SDR46(_band)		MT_WF_MIB(_band, 0x0f0)
#define MT_MIB_OPPO_PS_RX_DIS_DROP_COUNT GENMASK(15, 0)

#define MT_MIB_DR8(_band)		MT_WF_MIB(_band, 0x0c0)
#define MT_MIB_DR9(_band)		MT_WF_MIB(_band, 0x0c4)
#define MT_MIB_DR11(_band)		MT_WF_MIB(_band, 0x0cc)

#define MT_MIB_MB_SDR0(_band, n)	MT_WF_MIB(_band, 0x100 + ((n) << 4))
#define MT_MIB_RTS_RETRIES_COUNT_MASK	GENMASK(31, 16)
#define MT_MIB_RTS_COUNT_MASK		GENMASK(15, 0)

#define MT_MIB_MB_SDR1(_band, n)	MT_WF_MIB(_band, 0x104 + ((n) << 4))
#define MT_MIB_BA_MISS_COUNT_MASK	GENMASK(15, 0)
#define MT_MIB_ACK_FAIL_COUNT_MASK	GENMASK(31, 16)

#define MT_TX_AGG_CNT(_band, n)		MT_WF_MIB(_band, 0x0a8 + ((n) << 2))
#define MT_TX_AGG_CNT2(_band, n)	MT_WF_MIB(_band, 0x164 + ((n) << 2))
#define MT_MIB_ARNG(_band, n)		MT_WF_MIB(_band, 0x4b8 + ((n) << 2))
#define MT_MIB_ARNCR_RANGE(val, n)	(((val) >> ((n) << 3)) & GENMASK(7, 0))

/* drop due to retries being exhausted */
#define MT_MIB_M0DROPSR00(_band)	MT_WF_MIB(_band, 0x190)
#define MT_MIB_RTS_RETRY_FAIL_DROP_MASK	GENMASK(15, 0)
#define MT_MIB_MPDU_RETRY_FAIL_DROP_MASK GENMASK(31, 16)

/* life time out limit */
#define MT_MIB_M0DROPSR01(_band)	MT_WF_MIB(_band, 0x194)
#define MT_MIB_LTO_FAIL_DROP_MASK	GENMASK(15, 0)

/* increment when using double number of spatial streams */
#define MT_MIB_SDR50(_band)		MT_WF_MIB(_band, 0x1dc)
#define MT_MIB_DBNSS_CNT_DROP_MASK	GENMASK(15, 0)

/* NOTE:  Would need to poll this quickly since it is 16-bit */
#define MT_MIB_SDR51(_band)		MT_WF_MIB(_band, 0x1e0)
#define MT_MIB_RX_FCS_OK_MASK		GENMASK(15, 0)

/* VHT MU rx fcs ok, fcs fail.  NSS: 0,1  MCS: 0..9  */
#define MT_MIB_VHT_RX_FCS_HISTOGRAM_BASE_M0NSS1MCS0(_band)	MT_WF_MIB(_band, 0x400)
#define MT_MIB_VHT_RX_FCS_HIST_OK_MASK	GENMASK(15, 0)
#define MT_MIB_VHT_RX_FCS_HIST_ERR_MASK	GENMASK(31, 16)
#define MT_MIB_VHT_RX_FCS_HISTOGRAM(_band, nss, mcs)			\
	(MT_MIB_VHT_RX_FCS_HISTOGRAM_BASE_M0NSS1MCS0(_band) + (nss) * 4 + (mcs))

#define MT_WTBLON_TOP_BASE		0x34000
#define MT_WTBLON_TOP(ofs)		(MT_WTBLON_TOP_BASE + (ofs))
#define MT_WTBLON_TOP_WDUCR		MT_WTBLON_TOP(0x0)
#define MT_WTBLON_TOP_WDUCR_GROUP	GENMASK(2, 0)

#define MT_WTBL_UPDATE			MT_WTBLON_TOP(0x030)
#define MT_WTBL_UPDATE_WLAN_IDX		GENMASK(9, 0)
#define MT_WTBL_UPDATE_ADM_COUNT_CLEAR	BIT(12)
#define MT_WTBL_UPDATE_BUSY		BIT(31)

#define MT_WTBL_BASE			0x38000
#define MT_WTBL_LMAC_ID			GENMASK(14, 8)
#define MT_WTBL_LMAC_DW			GENMASK(7, 2)
#define MT_WTBL_LMAC_OFFS(_id, _dw)	(MT_WTBL_BASE | \
					FIELD_PREP(MT_WTBL_LMAC_ID, _id) | \
					FIELD_PREP(MT_WTBL_LMAC_DW, _dw))

/* AGG: band 0(0x20800), band 1(0xa0800) */
#define MT_WF_AGG_BASE(_band)		((_band) ? 0xa0800 : 0x20800)
#define MT_WF_AGG(_band, ofs)		(MT_WF_AGG_BASE(_band) + (ofs))

#define MT_AGG_AWSCR0(_band, _n)	MT_WF_AGG(_band, 0x05c + (_n) * 4)
#define MT_AGG_PCR0(_band, _n)		MT_WF_AGG(_band, 0x06c + (_n) * 4)
#define MT_AGG_PCR0_MM_PROT		BIT(0)
#define MT_AGG_PCR0_GF_PROT		BIT(1)
#define MT_AGG_PCR0_BW20_PROT		BIT(2)
#define MT_AGG_PCR0_BW40_PROT		BIT(4)
#define MT_AGG_PCR0_BW80_PROT		BIT(6)
#define MT_AGG_PCR0_ERP_PROT		GENMASK(12, 8)
#define MT_AGG_PCR0_VHT_PROT		BIT(13)
#define MT_AGG_PCR0_PTA_WIN_DIS		BIT(15)

#define MT_AGG_PCR1_RTS0_NUM_THRES	GENMASK(31, 23)
#define MT_AGG_PCR1_RTS0_LEN_THRES	GENMASK(19, 0)

#define MT_AGG_ACR0(_band)		MT_WF_AGG(_band, 0x084)
#define MT_AGG_ACR_CFEND_RATE		GENMASK(13, 0)
#define MT_AGG_ACR_BAR_RATE		GENMASK(29, 16)

#define MT_AGG_MRCR(_band)		MT_WF_AGG(_band, 0x098)
#define MT_AGG_MRCR_BAR_CNT_LIMIT	GENMASK(15, 12)
#define MT_AGG_MRCR_LAST_RTS_CTS_RN	BIT(6)
#define MT_AGG_MRCR_RTS_FAIL_LIMIT	GENMASK(11, 7)
#define MT_AGG_MRCR_TXCMD_RTS_FAIL_LIMIT	GENMASK(28, 24)

#define MT_AGG_ATCR1(_band)		MT_WF_AGG(_band, 0x0f0)
#define MT_AGG_ATCR3(_band)		MT_WF_AGG(_band, 0x0f4)

/* ARB: band 0(0x20c00), band 1(0xa0c00) */
#define MT_WF_ARB_BASE(_band)		((_band) ? 0xa0c00 : 0x20c00)
#define MT_WF_ARB(_band, ofs)		(MT_WF_ARB_BASE(_band) + (ofs))

#define MT_ARB_SCR(_band)		MT_WF_ARB(_band, 0x080)
#define MT_ARB_SCR_TX_DISABLE		BIT(8)
#define MT_ARB_SCR_RX_DISABLE		BIT(9)

#define MT_ARB_DRNGR0(_band, _n)	MT_WF_ARB(_band, 0x194 + (_n) * 4)

/* RMAC: band 0(0x21400), band 1(0xa1400) */
#define MT_WF_RMAC_BASE(_band)		((_band) ? 0xa1400 : 0x21400)
#define MT_WF_RMAC(_band, ofs)		(MT_WF_RMAC_BASE(_band) + (ofs))

#define MT_WF_RFCR(_band)		MT_WF_RMAC(_band, 0x000)
#define MT_WF_RFCR_DROP_STBC_MULTI	BIT(0)
#define MT_WF_RFCR_DROP_FCSFAIL		BIT(1)
#define MT_WF_RFCR_DROP_VERSION		BIT(3)
#define MT_WF_RFCR_DROP_PROBEREQ	BIT(4)
#define MT_WF_RFCR_DROP_MCAST		BIT(5)
#define MT_WF_RFCR_DROP_BCAST		BIT(6)
#define MT_WF_RFCR_DROP_MCAST_FILTERED	BIT(7)
#define MT_WF_RFCR_DROP_A3_MAC		BIT(8)
#define MT_WF_RFCR_DROP_A3_BSSID	BIT(9)
#define MT_WF_RFCR_DROP_A2_BSSID	BIT(10)
#define MT_WF_RFCR_DROP_OTHER_BEACON	BIT(11)
#define MT_WF_RFCR_DROP_FRAME_REPORT	BIT(12)
#define MT_WF_RFCR_DROP_CTL_RSV		BIT(13)
#define MT_WF_RFCR_DROP_CTS		BIT(14)
#define MT_WF_RFCR_DROP_RTS		BIT(15)
#define MT_WF_RFCR_DROP_DUPLICATE	BIT(16)
#define MT_WF_RFCR_DROP_OTHER_BSS	BIT(17)
#define MT_WF_RFCR_DROP_OTHER_UC	BIT(18)
#define MT_WF_RFCR_DROP_OTHER_TIM	BIT(19)
#define MT_WF_RFCR_DROP_NDPA		BIT(20)
#define MT_WF_RFCR_DROP_UNWANTED_CTL	BIT(21)

#define MT_WF_RFCR1(_band)		MT_WF_RMAC(_band, 0x004)
#define MT_WF_RFCR1_DROP_ACK		BIT(4)
#define MT_WF_RFCR1_DROP_BF_POLL	BIT(5)
#define MT_WF_RFCR1_DROP_BA		BIT(6)
#define MT_WF_RFCR1_DROP_CFEND		BIT(7)
#define MT_WF_RFCR1_DROP_CFACK		BIT(8)

#define MT_WF_RMAC_MIB_AIRTIME0(_band)	MT_WF_RMAC(_band, 0x0380)
#define MT_WF_RMAC_MIB_RXTIME_CLR	BIT(31)
#define MT_WF_RMAC_MIB_RXTIME_EN	BIT(30)

/* WFDMA0 */
#define MT_WFDMA0_BASE			0xd4000
#define MT_WFDMA0(ofs)			(MT_WFDMA0_BASE + (ofs))

#define MT_WFDMA0_RST			MT_WFDMA0(0x100)
#define MT_WFDMA0_RST_LOGIC_RST		BIT(4)
#define MT_WFDMA0_RST_DMASHDL_ALL_RST	BIT(5)

#define MT_WFDMA0_BUSY_ENA		MT_WFDMA0(0x13c)
#define MT_WFDMA0_BUSY_ENA_TX_FIFO0	BIT(0)
#define MT_WFDMA0_BUSY_ENA_TX_FIFO1	BIT(1)
#define MT_WFDMA0_BUSY_ENA_RX_FIFO	BIT(2)

#define MT_WFDMA0_GLO_CFG		MT_WFDMA0(0x208)
#define MT_WFDMA0_GLO_CFG_TX_DMA_EN	BIT(0)
#define MT_WFDMA0_GLO_CFG_RX_DMA_EN	BIT(2)

#define MT_WFDMA0_RST_DTX_PTR		MT_WFDMA0(0x20c)
#define MT_WFDMA0_PRI_DLY_INT_CFG0	MT_WFDMA0(0x2f0)

#define MT_RX_DATA_RING_BASE		MT_WFDMA0(0x500)

#define MT_WFDMA0_RX_RING0_EXT_CTRL	MT_WFDMA0(0x680)
#define MT_WFDMA0_RX_RING1_EXT_CTRL	MT_WFDMA0(0x684)
#define MT_WFDMA0_RX_RING2_EXT_CTRL	MT_WFDMA0(0x688)

/* WFDMA1 */
#define MT_WFDMA1_BASE			0xd5000
#define MT_WFDMA1(ofs)			(MT_WFDMA1_BASE + (ofs))

#define MT_WFDMA1_RST			MT_WFDMA1(0x100)
#define MT_WFDMA1_RST_LOGIC_RST		BIT(4)
#define MT_WFDMA1_RST_DMASHDL_ALL_RST	BIT(5)

#define MT_WFDMA1_BUSY_ENA		MT_WFDMA1(0x13c)
#define MT_WFDMA1_BUSY_ENA_TX_FIFO0	BIT(0)
#define MT_WFDMA1_BUSY_ENA_TX_FIFO1	BIT(1)
#define MT_WFDMA1_BUSY_ENA_RX_FIFO	BIT(2)

#define MT_MCU_CMD			MT_WFDMA1(0x1f0)
#define MT_MCU_CMD_STOP_DMA_FW_RELOAD	BIT(1)
#define MT_MCU_CMD_STOP_DMA		BIT(2)
#define MT_MCU_CMD_RESET_DONE		BIT(3)
#define MT_MCU_CMD_RECOVERY_DONE	BIT(4)
#define MT_MCU_CMD_NORMAL_STATE		BIT(5)
#define MT_MCU_CMD_ERROR_MASK		GENMASK(5, 1)

#define MT_WFDMA1_GLO_CFG		MT_WFDMA1(0x208)
#define MT_WFDMA1_GLO_CFG_TX_DMA_EN	BIT(0)
#define MT_WFDMA1_GLO_CFG_RX_DMA_EN	BIT(2)
#define MT_WFDMA1_GLO_CFG_OMIT_TX_INFO	BIT(28)
#define MT_WFDMA1_GLO_CFG_OMIT_RX_INFO	BIT(27)

#define MT_WFDMA1_RST_DTX_PTR		MT_WFDMA1(0x20c)
#define MT_WFDMA1_PRI_DLY_INT_CFG0	MT_WFDMA1(0x2f0)

#define MT_TX_RING_BASE			MT_WFDMA1(0x300)
#define MT_RX_EVENT_RING_BASE		MT_WFDMA1(0x500)

#define MT_WFDMA1_TX_RING0_EXT_CTRL	MT_WFDMA1(0x600)
#define MT_WFDMA1_TX_RING1_EXT_CTRL	MT_WFDMA1(0x604)
#define MT_WFDMA1_TX_RING2_EXT_CTRL	MT_WFDMA1(0x608)
#define MT_WFDMA1_TX_RING3_EXT_CTRL	MT_WFDMA1(0x60c)
#define MT_WFDMA1_TX_RING4_EXT_CTRL	MT_WFDMA1(0x610)
#define MT_WFDMA1_TX_RING5_EXT_CTRL	MT_WFDMA1(0x614)
#define MT_WFDMA1_TX_RING6_EXT_CTRL	MT_WFDMA1(0x618)
#define MT_WFDMA1_TX_RING7_EXT_CTRL	MT_WFDMA1(0x61c)

#define MT_WFDMA1_TX_RING16_EXT_CTRL	MT_WFDMA1(0x640)
#define MT_WFDMA1_TX_RING17_EXT_CTRL	MT_WFDMA1(0x644)
#define MT_WFDMA1_TX_RING18_EXT_CTRL	MT_WFDMA1(0x648)
#define MT_WFDMA1_TX_RING19_EXT_CTRL	MT_WFDMA1(0x64c)
#define MT_WFDMA1_TX_RING20_EXT_CTRL	MT_WFDMA1(0x650)
#define MT_WFDMA1_TX_RING21_EXT_CTRL	MT_WFDMA1(0x654)
#define MT_WFDMA1_TX_RING22_EXT_CTRL	MT_WFDMA1(0x658)
#define MT_WFDMA1_TX_RING23_EXT_CTRL	MT_WFDMA1(0x65c)

#define MT_WFDMA1_RX_RING0_EXT_CTRL	MT_WFDMA1(0x680)
#define MT_WFDMA1_RX_RING1_EXT_CTRL	MT_WFDMA1(0x684)
#define MT_WFDMA1_RX_RING2_EXT_CTRL	MT_WFDMA1(0x688)
#define MT_WFDMA1_RX_RING3_EXT_CTRL	MT_WFDMA1(0x68c)

/* WFDMA CSR */
#define MT_WFDMA_EXT_CSR_BASE		0xd7000
#define MT_WFDMA_EXT_CSR(ofs)		(MT_WFDMA_EXT_CSR_BASE + (ofs))

#define MT_INT_SOURCE_CSR		MT_WFDMA_EXT_CSR(0x10)
#define MT_INT_MASK_CSR			MT_WFDMA_EXT_CSR(0x14)
#define MT_INT_RX_DONE_DATA0		BIT(16)
#define MT_INT_RX_DONE_DATA1		BIT(17)
#define MT_INT_RX_DONE_WM		BIT(0)
#define MT_INT_RX_DONE_WA		BIT(1)
#define MT_INT_RX_DONE_WA_EXT		BIT(2)
#define MT_INT_RX_DONE_ALL		(GENMASK(2, 0) | GENMASK(17, 16))
#define MT_INT_TX_DONE_MCU_WA		BIT(15)
#define MT_INT_TX_DONE_FWDL		BIT(26)
#define MT_INT_TX_DONE_MCU_WM		BIT(27)
#define MT_INT_TX_DONE_BAND0		BIT(30)
#define MT_INT_TX_DONE_BAND1		BIT(31)

#define MT_INT_BAND1_MASK		(MT_INT_RX_DONE_WA_EXT |	\
					 MT_INT_TX_DONE_BAND1)

#define MT_INT_MCU_CMD			BIT(29)

#define MT_INT_TX_DONE_MCU		(MT_INT_TX_DONE_MCU_WA |	\
					 MT_INT_TX_DONE_MCU_WM |	\
					 MT_INT_TX_DONE_FWDL)

#define MT_WFDMA_HOST_CONFIG		MT_WFDMA_EXT_CSR(0x30)
#define MT_WFDMA_HOST_CONFIG_PDMA_BAND	BIT(0)

#define MT_WFDMA_EXT_CSR_HIF_MISC	MT_WFDMA_EXT_CSR(0x44)
#define MT_WFDMA_EXT_CSR_HIF_MISC_BUSY	BIT(0)

#define MT_INT1_SOURCE_CSR		MT_WFDMA_EXT_CSR(0x88)
#define MT_INT1_MASK_CSR		MT_WFDMA_EXT_CSR(0x8c)

#define MT_PCIE_RECOG_ID		MT_WFDMA_EXT_CSR(0x90)
#define MT_PCIE_RECOG_ID_MASK		GENMASK(30, 0)
#define MT_PCIE_RECOG_ID_SEM		BIT(31)

/* WFDMA0 PCIE1 */
#define MT_WFDMA0_PCIE1_BASE			0xd8000
#define MT_WFDMA0_PCIE1(ofs)			(MT_WFDMA0_PCIE1_BASE + (ofs))

#define MT_WFDMA0_PCIE1_BUSY_ENA		MT_WFDMA0_PCIE1(0x13c)
#define MT_WFDMA0_PCIE1_BUSY_ENA_TX_FIFO0	BIT(0)
#define MT_WFDMA0_PCIE1_BUSY_ENA_TX_FIFO1	BIT(1)
#define MT_WFDMA0_PCIE1_BUSY_ENA_RX_FIFO	BIT(2)

/* WFDMA1 PCIE1 */
#define MT_WFDMA1_PCIE1_BASE			0xd9000
#define MT_WFDMA1_PCIE1(ofs)			(MT_WFDMA0_PCIE1_BASE + (ofs))

#define MT_WFDMA1_PCIE1_BUSY_ENA		MT_WFDMA1_PCIE1(0x13c)
#define MT_WFDMA1_PCIE1_BUSY_ENA_TX_FIFO0	BIT(0)
#define MT_WFDMA1_PCIE1_BUSY_ENA_TX_FIFO1	BIT(1)
#define MT_WFDMA1_PCIE1_BUSY_ENA_RX_FIFO	BIT(2)

#define MT_TOP_RGU_BASE				0xf0000
#define MT_TOP_PWR_CTRL				(MT_TOP_RGU_BASE + (0x0))
#define MT_TOP_PWR_KEY				(0x5746 << 16)
#define MT_TOP_PWR_SW_RST			BIT(0)
#define MT_TOP_PWR_SW_PWR_ON			GENMASK(3, 2)
#define MT_TOP_PWR_HW_CTRL			BIT(4)
#define MT_TOP_PWR_PWR_ON			BIT(7)

#define MT_INFRA_CFG_BASE		0xf1000
#define MT_INFRA(ofs)			(MT_INFRA_CFG_BASE + (ofs))

#define MT_HIF_REMAP_L1			MT_INFRA(0x1ac)
#define MT_HIF_REMAP_L1_MASK		GENMASK(15, 0)
#define MT_HIF_REMAP_L1_OFFSET		GENMASK(15, 0)
#define MT_HIF_REMAP_L1_BASE		GENMASK(31, 16)
#define MT_HIF_REMAP_BASE_L1		0xe0000

#define MT_HIF_REMAP_L2			MT_INFRA(0x1b0)
#define MT_HIF_REMAP_L2_MASK		GENMASK(19, 0)
#define MT_HIF_REMAP_L2_OFFSET		GENMASK(11, 0)
#define MT_HIF_REMAP_L2_BASE		GENMASK(31, 12)
#define MT_HIF_REMAP_BASE_L2		0x00000

#define MT_SWDEF_BASE			0x41f200
#define MT_SWDEF(ofs)			(MT_SWDEF_BASE + (ofs))
#define MT_SWDEF_MODE			MT_SWDEF(0x3c)
#define MT_SWDEF_NORMAL_MODE		0
#define MT_SWDEF_ICAP_MODE		1
#define MT_SWDEF_SPECTRUM_MODE		2

#define MT_LED_TOP_BASE			0x18013000
#define MT_LED_PHYS(_n)			(MT_LED_TOP_BASE + (_n))

#define MT_LED_CTRL(_n)			MT_LED_PHYS(0x00 + ((_n) * 4))
#define MT_LED_CTRL_KICK		BIT(7)
#define MT_LED_CTRL_BLINK_MODE		BIT(2)
#define MT_LED_CTRL_POLARITY		BIT(1)

#define MT_LED_TX_BLINK(_n)		MT_LED_PHYS(0x10 + ((_n) * 4))
#define MT_LED_TX_BLINK_ON_MASK		GENMASK(7, 0)
#define MT_LED_TX_BLINK_OFF_MASK        GENMASK(15, 8)

#define MT_LED_EN(_n)			MT_LED_PHYS(0x40 + ((_n) * 4))

#define MT_TOP_BASE			0x18060000
#define MT_TOP(ofs)			(MT_TOP_BASE + (ofs))

#define MT_TOP_LPCR_HOST_BAND0		MT_TOP(0x10)
#define MT_TOP_LPCR_HOST_FW_OWN		BIT(0)
#define MT_TOP_LPCR_HOST_DRV_OWN	BIT(1)

#define MT_TOP_MISC			MT_TOP(0xf0)
#define MT_TOP_MISC_FW_STATE		GENMASK(2, 0)

#define MT_LED_GPIO_MUX2                0x70005058 /* GPIO 18 */
#define MT_LED_GPIO_MUX3                0x7000505C /* GPIO 26 */
#define MT_LED_GPIO_SEL_MASK            GENMASK(11, 8)

#define MT_HW_BOUND			0x70010020
#define MT_HW_CHIPID			0x70010200
#define MT_HW_REV			0x70010204

#define MT_PCIE1_MAC_BASE		0x74020000
#define MT_PCIE1_MAC(ofs)		(MT_PCIE1_MAC_BASE + (ofs))
#define MT_PCIE1_MAC_INT_ENABLE		MT_PCIE1_MAC(0x188)

#define MT_PCIE_MAC_BASE		0x74030000
#define MT_PCIE_MAC(ofs)		(MT_PCIE_MAC_BASE + (ofs))
#define MT_PCIE_MAC_INT_ENABLE		MT_PCIE_MAC(0x188)

#define MT_WF_IRPI_BASE			0x83006000
#define MT_WF_IRPI(ofs)			(MT_WF_IRPI_BASE + ((ofs) << 16))

/* PHY: band 0(0x83080000), band 1(0x83090000) */
#define MT_WF_PHY_BASE			0x83080000
#define MT_WF_PHY(ofs)			(MT_WF_PHY_BASE + (ofs))

#define MT_WF_PHY_RX_CTRL1(_phy)	MT_WF_PHY(0x2004 + ((_phy) << 16))
#define MT_WF_PHY_RX_CTRL1_IPI_EN	GENMASK(2, 0)
#define MT_WF_PHY_RX_CTRL1_STSCNT_EN	GENMASK(11, 9)

#define MT_WF_PHY_RXTD12(_phy)		MT_WF_PHY(0x8230 + ((_phy) << 16))
#define MT_WF_PHY_RXTD12_IRPI_SW_CLR_ONLY	BIT(18)
#define MT_WF_PHY_RXTD12_IRPI_SW_CLR	BIT(29)

/* PSE queue related registers and enums */

/* PLE info */
enum ENUM_UMAC_PORT {
	ENUM_UMAC_HIF_PORT_0         = 0,
	ENUM_UMAC_CPU_PORT_1         = 1,
	ENUM_UMAC_LMAC_PORT_2        = 2,
	ENUM_PLE_CTRL_PSE_PORT_3     = 3,
	ENUM_UMAC_PSE_PLE_PORT_TOTAL_NUM = 4
};

/* N9 MCU QUEUE LIST */
enum ENUM_UMAC_CPU_P_QUEUE {
	ENUM_UMAC_CTX_Q_0 = 0,
	ENUM_UMAC_CTX_Q_1 = 1,
	ENUM_UMAC_CTX_Q_2 = 2,
	ENUM_UMAC_CTX_Q_3 = 3,
	ENUM_UMAC_CRX     = 0,
	ENUM_UMAC_CIF_QUEUE_TOTAL_NUM = 4
};

/* LMAC PLE TX QUEUE LIST */
enum ENUM_UMAC_LMAC_PLE_TX_P_QUEUE {
	ENUM_UMAC_LMAC_PLE_TX_Q_00           = 0x00,
	ENUM_UMAC_LMAC_PLE_TX_Q_01           = 0x01,
	ENUM_UMAC_LMAC_PLE_TX_Q_02           = 0x02,
	ENUM_UMAC_LMAC_PLE_TX_Q_03           = 0x03,

	ENUM_UMAC_LMAC_PLE_TX_Q_10           = 0x04,
	ENUM_UMAC_LMAC_PLE_TX_Q_11           = 0x05,
	ENUM_UMAC_LMAC_PLE_TX_Q_12           = 0x06,
	ENUM_UMAC_LMAC_PLE_TX_Q_13           = 0x07,

	ENUM_UMAC_LMAC_PLE_TX_Q_20           = 0x08,
	ENUM_UMAC_LMAC_PLE_TX_Q_21           = 0x09,
	ENUM_UMAC_LMAC_PLE_TX_Q_22           = 0x0a,
	ENUM_UMAC_LMAC_PLE_TX_Q_23           = 0x0b,

	ENUM_UMAC_LMAC_PLE_TX_Q_30           = 0x0c,
	ENUM_UMAC_LMAC_PLE_TX_Q_31           = 0x0d,
	ENUM_UMAC_LMAC_PLE_TX_Q_32           = 0x0e,
	ENUM_UMAC_LMAC_PLE_TX_Q_33           = 0x0f,

	ENUM_UMAC_LMAC_PLE_TX_Q_ALTX_0      = 0x10,
	ENUM_UMAC_LMAC_PLE_TX_Q_BMC_0       = 0x11,
	ENUM_UMAC_LMAC_PLE_TX_Q_BNC_0       = 0x12,
	ENUM_UMAC_LMAC_PLE_TX_Q_PSMP_0      = 0x13,

	ENUM_UMAC_LMAC_PLE_TX_Q_ALTX_1      = 0x14,
	ENUM_UMAC_LMAC_PLE_TX_Q_BMC_1       = 0x15,
	ENUM_UMAC_LMAC_PLE_TX_Q_BNC_1       = 0x16,
	ENUM_UMAC_LMAC_PLE_TX_Q_PSMP_1      = 0x17,
	ENUM_UMAC_LMAC_PLE_TX_Q_NAF         = 0x18,
	ENUM_UMAC_LMAC_PLE_TX_Q_NBCN        = 0x19,
	/* DE suggests not to use 0x1f, it's only for hw free queue */
	ENUM_UMAC_LMAC_PLE_TX_Q_RELEASE     = 0x1f,
	ENUM_UMAC_LMAC_QUEUE_TOTAL_NUM      = 24,
};

/* LMAC PLE For PSE Control P3 */
enum _ENUM_UMAC_PLE_CTRL_P3_QUEUE {
	ENUM_UMAC_PLE_CTRL_P3_Q_0X1E            = 0x1e,
	ENUM_UMAC_PLE_CTRL_P3_Q_0X1F            = 0x1f,
	ENUM_UMAC_PLE_CTRL_P3_TOTAL_NUM         = 2
};

#define WF_PSE_TOP_BASE                                        0x820C8000

#define WF_PSE_TOP_GC_ADDR                                     (WF_PSE_TOP_BASE + 0x00) /* 8000 */
#define WF_PSE_TOP_PBUF_CTRL_ADDR                              (WF_PSE_TOP_BASE + 0x14) /* 8014 */
#define WF_PSE_TOP_INT_N9_EN_MASK_ADDR                         (WF_PSE_TOP_BASE + 0x20) /* 8020 */
#define WF_PSE_TOP_INT_N9_STS_ADDR                             (WF_PSE_TOP_BASE + 0x24) /* 8024 */
#define WF_PSE_TOP_INT_N9_ERR_STS_ADDR                         (WF_PSE_TOP_BASE + 0x28) /* 8028 */
#define WF_PSE_TOP_INT_N9_ERR_MASK_ADDR                        (WF_PSE_TOP_BASE + 0x2C) /* 802C */
#define WF_PSE_TOP_INT_N9_ERR1_STS_ADDR                        (WF_PSE_TOP_BASE + 0x30) /* 8030 */
#define WF_PSE_TOP_INT_N9_ERR1_MASK_ADDR                       (WF_PSE_TOP_BASE + 0x34) /* 8034 */
#define WF_PSE_TOP_C_GET_FID_0_ADDR                            (WF_PSE_TOP_BASE + 0x40) /* 8040 */
#define WF_PSE_TOP_C_GET_FID_1_ADDR                            (WF_PSE_TOP_BASE + 0x44) /* 8044 */
#define WF_PSE_TOP_C_EN_QUEUE_0_ADDR                           (WF_PSE_TOP_BASE + 0x60) /* 8060 */
#define WF_PSE_TOP_C_EN_QUEUE_1_ADDR                           (WF_PSE_TOP_BASE + 0x64) /* 8064 */
#define WF_PSE_TOP_C_EN_QUEUE_2_ADDR                           (WF_PSE_TOP_BASE + 0x68) /* 8068 */
#define WF_PSE_TOP_C_DE_QUEUE_0_ADDR                           (WF_PSE_TOP_BASE + 0x80) /* 8080 */
#define WF_PSE_TOP_C_DE_QUEUE_1_ADDR                           (WF_PSE_TOP_BASE + 0x84) /* 8084 */
#define WF_PSE_TOP_C_DE_QUEUE_2_ADDR                           (WF_PSE_TOP_BASE + 0x88) /* 8088 */
#define WF_PSE_TOP_C_DE_QUEUE_3_ADDR                           (WF_PSE_TOP_BASE + 0x8c) /* 808C */
#define WF_PSE_TOP_C_DE_QUEUE_4_ADDR                           (WF_PSE_TOP_BASE + 0x90) /* 8090 */
#define WF_PSE_TOP_ALLOCATE_0_ADDR                             (WF_PSE_TOP_BASE + 0xA0) /* 80A0 */
#define WF_PSE_TOP_ALLOCATE_1_ADDR                             (WF_PSE_TOP_BASE + 0xA4) /* 80A4 */
#define WF_PSE_TOP_QUEUE_EMPTY_ADDR                            (WF_PSE_TOP_BASE + 0xB0) /* 80B0 */
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_ADDR                       (WF_PSE_TOP_BASE + 0xB4) /* 80B4 */
#define WF_PSE_TOP_FREEPG_START_END_ADDR                       (WF_PSE_TOP_BASE + 0xC0) /* 80C0 */
#define WF_PSE_TOP_PSE_MODULE_CKG_DIS_ADDR                     (WF_PSE_TOP_BASE + 0xc4) /* 80C4 */
#define WF_PSE_TOP_TO_N9_INT_ADDR                              (WF_PSE_TOP_BASE + 0xf0) /* 80F0 */
#define WF_PSE_TOP_FREEPG_CNT_ADDR                             (WF_PSE_TOP_BASE + 0x100) /* 8100 */
#define WF_PSE_TOP_FREEPG_HEAD_TAIL_ADDR                       (WF_PSE_TOP_BASE + 0x104) /* 8104 */
#define WF_PSE_TOP_GROUP_REFILL_CTRL_ADDR                      (WF_PSE_TOP_BASE + 0x108) /* 8108 */
#define WF_PSE_TOP_PG_HIF0_GROUP_ADDR                          (WF_PSE_TOP_BASE + 0x110) /* 8110 */
#define WF_PSE_TOP_HIF0_PG_INFO_ADDR                           (WF_PSE_TOP_BASE + 0x114) /* 8114 */
#define WF_PSE_TOP_PG_HIF1_GROUP_ADDR                          (WF_PSE_TOP_BASE + 0x118) /* 8118 */
#define WF_PSE_TOP_HIF1_PG_INFO_ADDR                           (WF_PSE_TOP_BASE + 0x11C) /* 811C */
#define WF_PSE_TOP_PG_CPU_GROUP_ADDR                           (WF_PSE_TOP_BASE + 0x150) /* 8150 */
#define WF_PSE_TOP_CPU_PG_INFO_ADDR                            (WF_PSE_TOP_BASE + 0x154) /* 8154 */
#define WF_PSE_TOP_PG_PLE_GROUP_ADDR                           (WF_PSE_TOP_BASE + 0x160) /* 8160 */
#define WF_PSE_TOP_PLE_PG_INFO_ADDR                            (WF_PSE_TOP_BASE + 0x164) /* 8164 */
#define WF_PSE_TOP_PG_PLE1_GROUP_ADDR                          (WF_PSE_TOP_BASE + 0x168) /* 8168 */
#define WF_PSE_TOP_PLE1_PG_INFO_ADDR                           (WF_PSE_TOP_BASE + 0x16C) /* 816C */
#define WF_PSE_TOP_PG_LMAC0_GROUP_ADDR                         (WF_PSE_TOP_BASE + 0x170) /* 8170 */
#define WF_PSE_TOP_LMAC0_PG_INFO_ADDR                          (WF_PSE_TOP_BASE + 0x174) /* 8174 */
#define WF_PSE_TOP_PG_LMAC1_GROUP_ADDR                         (WF_PSE_TOP_BASE + 0x178) /* 8178 */
#define WF_PSE_TOP_LMAC1_PG_INFO_ADDR                          (WF_PSE_TOP_BASE + 0x17C) /* 817C */
#define WF_PSE_TOP_PG_LMAC2_GROUP_ADDR                         (WF_PSE_TOP_BASE + 0x180) /* 8180 */
#define WF_PSE_TOP_LMAC2_PG_INFO_ADDR                          (WF_PSE_TOP_BASE + 0x184) /* 8184 */
#define WF_PSE_TOP_PG_LMAC3_GROUP_ADDR                         (WF_PSE_TOP_BASE + 0x188) /* 8188 */
#define WF_PSE_TOP_LMAC3_PG_INFO_ADDR                          (WF_PSE_TOP_BASE + 0x18C) /* 818C */
#define WF_PSE_TOP_PG_MDP_GROUP_ADDR                           (WF_PSE_TOP_BASE + 0x198) /* 8198 */
#define WF_PSE_TOP_MDP_PG_INFO_ADDR                            (WF_PSE_TOP_BASE + 0x19C) /* 819C */
#define WF_PSE_TOP_RL_BUF_CTRL_0_ADDR                          (WF_PSE_TOP_BASE + 0x1A0) /* 81A0 */
#define WF_PSE_TOP_RL_BUF_CTRL_1_ADDR                          (WF_PSE_TOP_BASE + 0x1A4) /* 81A4 */
#define WF_PSE_TOP_FL_QUE_CTRL_0_ADDR                          (WF_PSE_TOP_BASE + 0x1B0) /* 81B0 */
#define WF_PSE_TOP_FL_QUE_CTRL_1_ADDR                          (WF_PSE_TOP_BASE + 0x1B4) /* 81B4 */
#define WF_PSE_TOP_FL_QUE_CTRL_2_ADDR                          (WF_PSE_TOP_BASE + 0x1B8) /* 81B8 */
#define WF_PSE_TOP_FL_QUE_CTRL_3_ADDR                          (WF_PSE_TOP_BASE + 0x1BC) /* 81BC */
#define WF_PSE_TOP_PL_QUE_CTRL_0_ADDR                          (WF_PSE_TOP_BASE + 0x1C0) /* 81C0 */
#define WF_PSE_TOP_PSE_LP_CTRL_ADDR                            (WF_PSE_TOP_BASE + 0x1D0) /* 81D0 */
#define WF_PSE_TOP_PSE_WFDMA_BUF_CTRL_ADDR                     (WF_PSE_TOP_BASE + 0x1E0) /* 81E0 */
#define WF_PSE_TOP_PSE_CT_PRI_CTRL_ADDR                        (WF_PSE_TOP_BASE + 0x1EC) /* 81EC */
#define WF_PSE_TOP_PLE_ENQ_PKT_NUM_ADDR                        (WF_PSE_TOP_BASE + 0x1F0) /* 81F0 */
#define WF_PSE_TOP_CPU_ENQ_PKT_NUM_ADDR                        (WF_PSE_TOP_BASE + 0x1F4) /* 81F4 */
#define WF_PSE_TOP_LMAC_ENQ_PKT_NUM_ADDR                       (WF_PSE_TOP_BASE + 0x1F8) /* 81F8 */
#define WF_PSE_TOP_HIF_ENQ_PKT_NUM_ADDR                        (WF_PSE_TOP_BASE + 0x1FC) /* 81FC */
#define WF_PSE_TOP_MDP_ENQ_PKT_NUM_ADDR                        (WF_PSE_TOP_BASE + 0x200) /* 8200 */
#define WF_PSE_TOP_TIMEOUT_CTRL_ADDR                           (WF_PSE_TOP_BASE + 0x244) /* 8244 */
#define WF_PSE_TOP_FSM_IDLE_WD_CTRL_ADDR                       (WF_PSE_TOP_BASE + 0x24C) /* 824C */
#define WF_PSE_TOP_FSM_IDLE_WD_EN_ADDR                         (WF_PSE_TOP_BASE + 0x250) /* 8250 */
#define WF_PSE_TOP_PSE_INTER_ERR_FLAG_ADDR                     (WF_PSE_TOP_BASE + 0x280) /* 8280 */
#define WF_PSE_TOP_PSE_SER_CTRL_ADDR                           (WF_PSE_TOP_BASE + 0x2a0) /* 82A0 */
#define WF_PSE_TOP_PSE_MBIST_RP_FUSE_ADDR                      (WF_PSE_TOP_BASE + 0x2b0) /* 82B0 */
#define WF_PSE_TOP_PSE_MBIST_BSEL_ADDR                         (WF_PSE_TOP_BASE + 0x2b4) /* 82B4 */
#define WF_PSE_TOP_SRAM_MBIST_BACKGROUND_ADDR                  (WF_PSE_TOP_BASE + 0x2d0) /* 82D0 */
#define WF_PSE_TOP_PSE_MISC_FUNC_CTRL_ADDR                     (WF_PSE_TOP_BASE + 0x2d4) /* 82D4 */
#define WF_PSE_TOP_SRAM_MBIST_DONE_ADDR                        (WF_PSE_TOP_BASE + 0x2d8) /* 82D8 */
#define WF_PSE_TOP_SRAM_MBIST_FAIL_ADDR                        (WF_PSE_TOP_BASE + 0x2dc) /* 82DC */
#define WF_PSE_TOP_SRAM_MBIST_CTRL_ADDR                        (WF_PSE_TOP_BASE + 0x2e0) /* 82E0 */
#define WF_PSE_TOP_SRAM_MBIST_DELSEL_ADDR                      (WF_PSE_TOP_BASE + 0x2e4) /* 82E4 */
#define WF_PSE_TOP_SRAM_AWT_HDEN_CTRL_ADDR                     (WF_PSE_TOP_BASE + 0x2e8) /* 82E8 */
#define WF_PSE_TOP_PSE_SEEK_CR_00_ADDR                         (WF_PSE_TOP_BASE + 0x3d0) /* 83D0 */
#define WF_PSE_TOP_PSE_SEEK_CR_01_ADDR                         (WF_PSE_TOP_BASE + 0x3d4) /* 83D4 */
#define WF_PSE_TOP_PSE_SEEK_CR_02_ADDR                         (WF_PSE_TOP_BASE + 0x3d8) /* 83D8 */
#define WF_PSE_TOP_PSE_SEEK_CR_03_ADDR                         (WF_PSE_TOP_BASE + 0x3dc) /* 83DC */
#define WF_PSE_TOP_PSE_SEEK_CR_04_ADDR                         (WF_PSE_TOP_BASE + 0x3e0) /* 83E0 */
#define WF_PSE_TOP_PSE_SEEK_CR_05_ADDR                         (WF_PSE_TOP_BASE + 0x3e4) /* 83E4 */
#define WF_PSE_TOP_PSE_SEEK_CR_06_ADDR                         (WF_PSE_TOP_BASE + 0x3e8) /* 83E8 */
#define WF_PSE_TOP_PSE_SEEK_CR_07_ADDR                         (WF_PSE_TOP_BASE + 0x3ec) /* 83EC */
#define WF_PSE_TOP_PSE_SEEK_CR_08_ADDR                         (WF_PSE_TOP_BASE + 0x3f0) /* 83F0 */
#define WF_PSE_TOP_PSE_SEEK_CR_09_ADDR                         (WF_PSE_TOP_BASE + 0x3f4) /* 83F4 */

#define WF_PSE_TOP_PBUF_CTRL_PAGE_SIZE_CFG_ADDR    WF_PSE_TOP_PBUF_CTRL_ADDR
#define WF_PSE_TOP_PBUF_CTRL_PAGE_SIZE_CFG_MASK    0x80000000   /* PAGE_SIZE_CFG[31] */
#define WF_PSE_TOP_PBUF_CTRL_PAGE_SIZE_CFG_SHFT    31
#define WF_PSE_TOP_PBUF_CTRL_PBUF_OFFSET_ADDR      WF_PSE_TOP_PBUF_CTRL_ADDR
#define WF_PSE_TOP_PBUF_CTRL_PBUF_OFFSET_MASK      0x03FE0000   /* PBUF_OFFSET[25..17] */
#define WF_PSE_TOP_PBUF_CTRL_PBUF_OFFSET_SHFT      17
#define WF_PSE_TOP_PBUF_CTRL_TOTAL_PAGE_NUM_ADDR   WF_PSE_TOP_PBUF_CTRL_ADDR
#define WF_PSE_TOP_PBUF_CTRL_TOTAL_PAGE_NUM_MASK   0x00000FFF   /* TOTAL_PAGE_NUM[11..0] */
#define WF_PSE_TOP_PBUF_CTRL_TOTAL_PAGE_NUM_SHFT   0

#define WF_PSE_TOP_FREEPG_CNT_FFA_CNT_ADDR         WF_PSE_TOP_FREEPG_CNT_ADDR
#define WF_PSE_TOP_FREEPG_CNT_FFA_CNT_MASK         0x0FFF0000   /* FFA_CNT[27..16] */
#define WF_PSE_TOP_FREEPG_CNT_FFA_CNT_SHFT         16
#define WF_PSE_TOP_FREEPG_CNT_FREEPG_CNT_ADDR      WF_PSE_TOP_FREEPG_CNT_ADDR
#define WF_PSE_TOP_FREEPG_CNT_FREEPG_CNT_MASK      0x00000FFF   /* FREEPG_CNT[11..0] */
#define WF_PSE_TOP_FREEPG_CNT_FREEPG_CNT_SHFT      0

#define WF_PSE_TOP_FREEPG_HEAD_TAIL_FREEPG_TAIL_ADDR  WF_PSE_TOP_FREEPG_HEAD_TAIL_ADDR
#define WF_PSE_TOP_FREEPG_HEAD_TAIL_FREEPG_TAIL_MASK  0x0FFF0000  /* FREEPG_TAIL[27..16] */
#define WF_PSE_TOP_FREEPG_HEAD_TAIL_FREEPG_TAIL_SHFT  16
#define WF_PSE_TOP_FREEPG_HEAD_TAIL_FREEPG_HEAD_ADDR  WF_PSE_TOP_FREEPG_HEAD_TAIL_ADDR
#define WF_PSE_TOP_FREEPG_HEAD_TAIL_FREEPG_HEAD_MASK  0x00000FFF  /* FREEPG_HEAD[11..0] */
#define WF_PSE_TOP_FREEPG_HEAD_TAIL_FREEPG_HEAD_SHFT  0

#define WF_PSE_TOP_PG_HIF0_GROUP_HIF0_MAX_QUOTA_ADDR  WF_PSE_TOP_PG_HIF0_GROUP_ADDR
#define WF_PSE_TOP_PG_HIF0_GROUP_HIF0_MAX_QUOTA_MASK  0x0FFF0000  /* HIF0_MAX_QUOTA[27..16] */
#define WF_PSE_TOP_PG_HIF0_GROUP_HIF0_MAX_QUOTA_SHFT  16
#define WF_PSE_TOP_PG_HIF0_GROUP_HIF0_MIN_QUOTA_ADDR  WF_PSE_TOP_PG_HIF0_GROUP_ADDR
#define WF_PSE_TOP_PG_HIF0_GROUP_HIF0_MIN_QUOTA_MASK  0x00000FFF  /* HIF0_MIN_QUOTA[11..0] */
#define WF_PSE_TOP_PG_HIF0_GROUP_HIF0_MIN_QUOTA_SHFT  0

#define WF_PSE_TOP_HIF0_PG_INFO_HIF0_SRC_CNT_ADDR     WF_PSE_TOP_HIF0_PG_INFO_ADDR
#define WF_PSE_TOP_HIF0_PG_INFO_HIF0_SRC_CNT_MASK     0x0FFF0000  /* HIF0_SRC_CNT[27..16] */
#define WF_PSE_TOP_HIF0_PG_INFO_HIF0_SRC_CNT_SHFT     16
#define WF_PSE_TOP_HIF0_PG_INFO_HIF0_RSV_CNT_ADDR     WF_PSE_TOP_HIF0_PG_INFO_ADDR
#define WF_PSE_TOP_HIF0_PG_INFO_HIF0_RSV_CNT_MASK     0x00000FFF  /* HIF0_RSV_CNT[11..0] */
#define WF_PSE_TOP_HIF0_PG_INFO_HIF0_RSV_CNT_SHFT     0

#define WF_PSE_TOP_PG_HIF1_GROUP_HIF1_MAX_QUOTA_ADDR  WF_PSE_TOP_PG_HIF1_GROUP_ADDR
#define WF_PSE_TOP_PG_HIF1_GROUP_HIF1_MAX_QUOTA_MASK  0x0FFF0000  /* HIF1_MAX_QUOTA[27..16] */
#define WF_PSE_TOP_PG_HIF1_GROUP_HIF1_MAX_QUOTA_SHFT  16
#define WF_PSE_TOP_PG_HIF1_GROUP_HIF1_MIN_QUOTA_ADDR  WF_PSE_TOP_PG_HIF1_GROUP_ADDR
#define WF_PSE_TOP_PG_HIF1_GROUP_HIF1_MIN_QUOTA_MASK  0x00000FFF  /* HIF1_MIN_QUOTA[11..0] */
#define WF_PSE_TOP_PG_HIF1_GROUP_HIF1_MIN_QUOTA_SHFT  0

#define WF_PSE_TOP_HIF1_PG_INFO_HIF1_SRC_CNT_ADDR     WF_PSE_TOP_HIF1_PG_INFO_ADDR
#define WF_PSE_TOP_HIF1_PG_INFO_HIF1_SRC_CNT_MASK     0x0FFF0000  /* HIF1_SRC_CNT[27..16] */
#define WF_PSE_TOP_HIF1_PG_INFO_HIF1_SRC_CNT_SHFT     16
#define WF_PSE_TOP_HIF1_PG_INFO_HIF1_RSV_CNT_ADDR     WF_PSE_TOP_HIF1_PG_INFO_ADDR
#define WF_PSE_TOP_HIF1_PG_INFO_HIF1_RSV_CNT_MASK     0x00000FFF  /* HIF1_RSV_CNT[11..0] */
#define WF_PSE_TOP_HIF1_PG_INFO_HIF1_RSV_CNT_SHFT     0

#define WF_PSE_TOP_PG_CPU_GROUP_CPU_MAX_QUOTA_ADDR    WF_PSE_TOP_PG_CPU_GROUP_ADDR
#define WF_PSE_TOP_PG_CPU_GROUP_CPU_MAX_QUOTA_MASK    0x0FFF0000  /* CPU_MAX_QUOTA[27..16] */
#define WF_PSE_TOP_PG_CPU_GROUP_CPU_MAX_QUOTA_SHFT    16
#define WF_PSE_TOP_PG_CPU_GROUP_CPU_MIN_QUOTA_ADDR    WF_PSE_TOP_PG_CPU_GROUP_ADDR
#define WF_PSE_TOP_PG_CPU_GROUP_CPU_MIN_QUOTA_MASK    0x00000FFF  /* CPU_MIN_QUOTA[11..0] */
#define WF_PSE_TOP_PG_CPU_GROUP_CPU_MIN_QUOTA_SHFT    0

#define WF_PSE_TOP_CPU_PG_INFO_CPU_SRC_CNT_ADDR       WF_PSE_TOP_CPU_PG_INFO_ADDR
#define WF_PSE_TOP_CPU_PG_INFO_CPU_SRC_CNT_MASK       0x0FFF0000  /* CPU_SRC_CNT[27..16] */
#define WF_PSE_TOP_CPU_PG_INFO_CPU_SRC_CNT_SHFT       16
#define WF_PSE_TOP_CPU_PG_INFO_CPU_RSV_CNT_ADDR       WF_PSE_TOP_CPU_PG_INFO_ADDR
#define WF_PSE_TOP_CPU_PG_INFO_CPU_RSV_CNT_MASK       0x00000FFF  /* CPU_RSV_CNT[11..0] */
#define WF_PSE_TOP_CPU_PG_INFO_CPU_RSV_CNT_SHFT       0

#define WF_PSE_TOP_PG_PLE_GROUP_PLE_MAX_QUOTA_ADDR    WF_PSE_TOP_PG_PLE_GROUP_ADDR
#define WF_PSE_TOP_PG_PLE_GROUP_PLE_MAX_QUOTA_MASK    0x0FFF0000  /* PLE_MAX_QUOTA[27..16] */
#define WF_PSE_TOP_PG_PLE_GROUP_PLE_MAX_QUOTA_SHFT    16
#define WF_PSE_TOP_PG_PLE_GROUP_PLE_MIN_QUOTA_ADDR    WF_PSE_TOP_PG_PLE_GROUP_ADDR
#define WF_PSE_TOP_PG_PLE_GROUP_PLE_MIN_QUOTA_MASK    0x00000FFF  /* PLE_MIN_QUOTA[11..0] */
#define WF_PSE_TOP_PG_PLE_GROUP_PLE_MIN_QUOTA_SHFT    0

#define WF_PSE_TOP_PLE_PG_INFO_PLE_SRC_CNT_ADDR       WF_PSE_TOP_PLE_PG_INFO_ADDR
#define WF_PSE_TOP_PLE_PG_INFO_PLE_SRC_CNT_MASK       0x0FFF0000  /* PLE_SRC_CNT[27..16] */
#define WF_PSE_TOP_PLE_PG_INFO_PLE_SRC_CNT_SHFT       16
#define WF_PSE_TOP_PLE_PG_INFO_PLE_RSV_CNT_ADDR       WF_PSE_TOP_PLE_PG_INFO_ADDR
#define WF_PSE_TOP_PLE_PG_INFO_PLE_RSV_CNT_MASK       0x00000FFF  /* PLE_RSV_CNT[11..0] */
#define WF_PSE_TOP_PLE_PG_INFO_PLE_RSV_CNT_SHFT       0

#define WF_PSE_TOP_PG_PLE1_GROUP_PLE1_MAX_QUOTA_ADDR  WF_PSE_TOP_PG_PLE1_GROUP_ADDR
#define WF_PSE_TOP_PG_PLE1_GROUP_PLE1_MAX_QUOTA_MASK  0x0FFF0000  /* PLE_MAX_QUOTA[27..16] */
#define WF_PSE_TOP_PG_PLE1_GROUP_PLE1_MAX_QUOTA_SHFT  16
#define WF_PSE_TOP_PG_PLE1_GROUP_PLE1_MIN_QUOTA_ADDR  WF_PSE_TOP_PG_PLE1_GROUP_ADDR
#define WF_PSE_TOP_PG_PLE1_GROUP_PLE1_MIN_QUOTA_MASK  0x00000FFF  /* PLE_MIN_QUOTA[11..0] */
#define WF_PSE_TOP_PG_PLE1_GROUP_PLE1_MIN_QUOTA_SHFT  0

#define WF_PSE_TOP_PLE1_PG_INFO_PLE1_SRC_CNT_ADDR     WF_PSE_TOP_PLE1_PG_INFO_ADDR
#define WF_PSE_TOP_PLE1_PG_INFO_PLE1_SRC_CNT_MASK     0x0FFF0000  /* PLE_SRC_CNT[27..16] */
#define WF_PSE_TOP_PLE1_PG_INFO_PLE1_SRC_CNT_SHFT     16
#define WF_PSE_TOP_PLE1_PG_INFO_PLE1_RSV_CNT_ADDR     WF_PSE_TOP_PLE1_PG_INFO_ADDR
#define WF_PSE_TOP_PLE1_PG_INFO_PLE1_RSV_CNT_MASK     0x00000FFF  /* PLE_RSV_CNT[11..0] */
#define WF_PSE_TOP_PLE1_PG_INFO_PLE1_RSV_CNT_SHFT     0

#define WF_PSE_TOP_PG_LMAC0_GROUP_LMAC0_MAX_QUOTA_ADDR  WF_PSE_TOP_PG_LMAC0_GROUP_ADDR
#define WF_PSE_TOP_PG_LMAC0_GROUP_LMAC0_MAX_QUOTA_MASK  0x0FFF0000  /* LMAC0_MAX_QUOTA[27..16] */
#define WF_PSE_TOP_PG_LMAC0_GROUP_LMAC0_MAX_QUOTA_SHFT  16
#define WF_PSE_TOP_PG_LMAC0_GROUP_LMAC0_MIN_QUOTA_ADDR  WF_PSE_TOP_PG_LMAC0_GROUP_ADDR
#define WF_PSE_TOP_PG_LMAC0_GROUP_LMAC0_MIN_QUOTA_MASK  0x00000FFF  /* LMAC0_MIN_QUOTA[11..0] */
#define WF_PSE_TOP_PG_LMAC0_GROUP_LMAC0_MIN_QUOTA_SHFT  0

#define WF_PSE_TOP_LMAC0_PG_INFO_LMAC0_SRC_CNT_ADDR     WF_PSE_TOP_LMAC0_PG_INFO_ADDR
#define WF_PSE_TOP_LMAC0_PG_INFO_LMAC0_SRC_CNT_MASK     0x0FFF0000  /* LMAC0_SRC_CNT[27..16] */
#define WF_PSE_TOP_LMAC0_PG_INFO_LMAC0_SRC_CNT_SHFT     16
#define WF_PSE_TOP_LMAC0_PG_INFO_LMAC0_RSV_CNT_ADDR     WF_PSE_TOP_LMAC0_PG_INFO_ADDR
#define WF_PSE_TOP_LMAC0_PG_INFO_LMAC0_RSV_CNT_MASK     0x00000FFF  /* LMAC0_RSV_CNT[11..0] */
#define WF_PSE_TOP_LMAC0_PG_INFO_LMAC0_RSV_CNT_SHFT     0

#define WF_PSE_TOP_PG_LMAC1_GROUP_LMAC1_MAX_QUOTA_ADDR  WF_PSE_TOP_PG_LMAC1_GROUP_ADDR
#define WF_PSE_TOP_PG_LMAC1_GROUP_LMAC1_MAX_QUOTA_MASK  0x0FFF0000  /* LMAC1_MAX_QUOTA[27..16] */
#define WF_PSE_TOP_PG_LMAC1_GROUP_LMAC1_MAX_QUOTA_SHFT  16
#define WF_PSE_TOP_PG_LMAC1_GROUP_LMAC1_MIN_QUOTA_ADDR  WF_PSE_TOP_PG_LMAC1_GROUP_ADDR
#define WF_PSE_TOP_PG_LMAC1_GROUP_LMAC1_MIN_QUOTA_MASK  0x00000FFF  /* LMAC1_MIN_QUOTA[11..0] */
#define WF_PSE_TOP_PG_LMAC1_GROUP_LMAC1_MIN_QUOTA_SHFT  0

#define WF_PSE_TOP_LMAC1_PG_INFO_LMAC1_SRC_CNT_ADDR     WF_PSE_TOP_LMAC1_PG_INFO_ADDR
#define WF_PSE_TOP_LMAC1_PG_INFO_LMAC1_SRC_CNT_MASK     0x0FFF0000  /* LMAC1_SRC_CNT[27..16] */
#define WF_PSE_TOP_LMAC1_PG_INFO_LMAC1_SRC_CNT_SHFT     16
#define WF_PSE_TOP_LMAC1_PG_INFO_LMAC1_RSV_CNT_ADDR     WF_PSE_TOP_LMAC1_PG_INFO_ADDR
#define WF_PSE_TOP_LMAC1_PG_INFO_LMAC1_RSV_CNT_MASK     0x00000FFF  /* LMAC1_RSV_CNT[11..0] */
#define WF_PSE_TOP_LMAC1_PG_INFO_LMAC1_RSV_CNT_SHFT     0

#define WF_PSE_TOP_PG_LMAC2_GROUP_LMAC2_MAX_QUOTA_ADDR  WF_PSE_TOP_PG_LMAC2_GROUP_ADDR
#define WF_PSE_TOP_PG_LMAC2_GROUP_LMAC2_MAX_QUOTA_MASK  0x0FFF0000  /* LMAC2_MAX_QUOTA[27..16] */
#define WF_PSE_TOP_PG_LMAC2_GROUP_LMAC2_MAX_QUOTA_SHFT  16
#define WF_PSE_TOP_PG_LMAC2_GROUP_LMAC2_MIN_QUOTA_ADDR  WF_PSE_TOP_PG_LMAC2_GROUP_ADDR
#define WF_PSE_TOP_PG_LMAC2_GROUP_LMAC2_MIN_QUOTA_MASK  0x00000FFF  /* LMAC2_MIN_QUOTA[11..0] */
#define WF_PSE_TOP_PG_LMAC2_GROUP_LMAC2_MIN_QUOTA_SHFT  0

#define WF_PSE_TOP_LMAC2_PG_INFO_LMAC2_SRC_CNT_ADDR     WF_PSE_TOP_LMAC2_PG_INFO_ADDR
#define WF_PSE_TOP_LMAC2_PG_INFO_LMAC2_SRC_CNT_MASK     0x0FFF0000  /* LMAC2_SRC_CNT[27..16] */
#define WF_PSE_TOP_LMAC2_PG_INFO_LMAC2_SRC_CNT_SHFT     16
#define WF_PSE_TOP_LMAC2_PG_INFO_LMAC2_RSV_CNT_ADDR     WF_PSE_TOP_LMAC2_PG_INFO_ADDR
#define WF_PSE_TOP_LMAC2_PG_INFO_LMAC2_RSV_CNT_MASK     0x00000FFF  /* LMAC2_RSV_CNT[11..0] */
#define WF_PSE_TOP_LMAC2_PG_INFO_LMAC2_RSV_CNT_SHFT     0

#define WF_PSE_TOP_PG_LMAC3_GROUP_LMAC3_MAX_QUOTA_ADDR  WF_PSE_TOP_PG_LMAC3_GROUP_ADDR
#define WF_PSE_TOP_PG_LMAC3_GROUP_LMAC3_MAX_QUOTA_MASK  0x0FFF0000  /* LMAC3_MAX_QUOTA[27..16] */
#define WF_PSE_TOP_PG_LMAC3_GROUP_LMAC3_MAX_QUOTA_SHFT  16
#define WF_PSE_TOP_PG_LMAC3_GROUP_LMAC3_MIN_QUOTA_ADDR  WF_PSE_TOP_PG_LMAC3_GROUP_ADDR
#define WF_PSE_TOP_PG_LMAC3_GROUP_LMAC3_MIN_QUOTA_MASK  0x00000FFF  /* LMAC3_MIN_QUOTA[11..0] */
#define WF_PSE_TOP_PG_LMAC3_GROUP_LMAC3_MIN_QUOTA_SHFT  0

#define WF_PSE_TOP_LMAC3_PG_INFO_LMAC3_SRC_CNT_ADDR     WF_PSE_TOP_LMAC3_PG_INFO_ADDR
#define WF_PSE_TOP_LMAC3_PG_INFO_LMAC3_SRC_CNT_MASK     0x0FFF0000  /* LMAC3_SRC_CNT[27..16] */
#define WF_PSE_TOP_LMAC3_PG_INFO_LMAC3_SRC_CNT_SHFT     16
#define WF_PSE_TOP_LMAC3_PG_INFO_LMAC3_RSV_CNT_ADDR     WF_PSE_TOP_LMAC3_PG_INFO_ADDR
#define WF_PSE_TOP_LMAC3_PG_INFO_LMAC3_RSV_CNT_MASK     0x00000FFF  /* LMAC3_RSV_CNT[11..0] */
#define WF_PSE_TOP_LMAC3_PG_INFO_LMAC3_RSV_CNT_SHFT     0

#define WF_PSE_TOP_PG_MDP_GROUP_MDP_MAX_QUOTA_ADDR      WF_PSE_TOP_PG_MDP_GROUP_ADDR
#define WF_PSE_TOP_PG_MDP_GROUP_MDP_MAX_QUOTA_MASK      0x0FFF0000  /* MDP_MAX_QUOTA[27..16] */
#define WF_PSE_TOP_PG_MDP_GROUP_MDP_MAX_QUOTA_SHFT      16
#define WF_PSE_TOP_PG_MDP_GROUP_MDP_MIN_QUOTA_ADDR      WF_PSE_TOP_PG_MDP_GROUP_ADDR
#define WF_PSE_TOP_PG_MDP_GROUP_MDP_MIN_QUOTA_MASK      0x00000FFF  /* MDP_MIN_QUOTA[11..0] */
#define WF_PSE_TOP_PG_MDP_GROUP_MDP_MIN_QUOTA_SHFT      0

#define WF_PSE_TOP_MDP_PG_INFO_MDP_SRC_CNT_ADDR         WF_PSE_TOP_MDP_PG_INFO_ADDR
#define WF_PSE_TOP_MDP_PG_INFO_MDP_SRC_CNT_MASK         0x0FFF0000  /* MDP_SRC_CNT[27..16] */
#define WF_PSE_TOP_MDP_PG_INFO_MDP_SRC_CNT_SHFT         16
#define WF_PSE_TOP_MDP_PG_INFO_MDP_RSV_CNT_ADDR         WF_PSE_TOP_MDP_PG_INFO_ADDR
#define WF_PSE_TOP_MDP_PG_INFO_MDP_RSV_CNT_MASK         0x00000FFF  /* MDP_RSV_CNT[11..0] */
#define WF_PSE_TOP_MDP_PG_INFO_MDP_RSV_CNT_SHFT         0

#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_6_EMPTY_MASK_ADDR      WF_PSE_TOP_QUEUE_EMPTY_MASK_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_6_EMPTY_MASK_MASK      0x00004000 /* HIF_6_EMPTY_MASK[14] */
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_6_EMPTY_MASK_SHFT      14
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_5_EMPTY_MASK_ADDR      WF_PSE_TOP_QUEUE_EMPTY_MASK_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_5_EMPTY_MASK_MASK      0x00002000 /* HIF_5_EMPTY_MASK[13] */
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_5_EMPTY_MASK_SHFT      13
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_4_EMPTY_MASK_ADDR      WF_PSE_TOP_QUEUE_EMPTY_MASK_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_4_EMPTY_MASK_MASK      0x00001000 /* HIF_4_EMPTY_MASK[12] */
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_4_EMPTY_MASK_SHFT      12
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_3_EMPTY_MASK_ADDR      WF_PSE_TOP_QUEUE_EMPTY_MASK_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_3_EMPTY_MASK_MASK      0x00000800 /* HIF_3_EMPTY_MASK[11] */
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_3_EMPTY_MASK_SHFT      11
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_2_EMPTY_MASK_ADDR      WF_PSE_TOP_QUEUE_EMPTY_MASK_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_2_EMPTY_MASK_MASK      0x00000400 /* HIF_2_EMPTY_MASK[10] */
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_2_EMPTY_MASK_SHFT      10
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_1_EMPTY_MASK_ADDR      WF_PSE_TOP_QUEUE_EMPTY_MASK_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_1_EMPTY_MASK_MASK      0x00000200 /* HIF_1_EMPTY_MASK[9] */
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_1_EMPTY_MASK_SHFT      9
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_0_EMPTY_MASK_ADDR      WF_PSE_TOP_QUEUE_EMPTY_MASK_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_0_EMPTY_MASK_MASK      0x00000100 /* HIF_0_EMPTY_MASK[8] */
#define WF_PSE_TOP_QUEUE_EMPTY_MASK_HIF_0_EMPTY_MASK_SHFT      8

#define WF_PSE_TOP_QUEUE_EMPTY_RLS_Q_EMTPY_ADDR                WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_RLS_Q_EMTPY_MASK                0x80000000 /* RLS_Q_EMTPY[31] */
#define WF_PSE_TOP_QUEUE_EMPTY_RLS_Q_EMTPY_SHFT                31
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_RXIOC1_QUEUE_EMPTY_ADDR     WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_RXIOC1_QUEUE_EMPTY_MASK     0x08000000 /* MDP_RXIOC1_QE[27] */
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_RXIOC1_QUEUE_EMPTY_SHFT     27
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TXIOC1_QUEUE_EMPTY_ADDR     WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TXIOC1_QUEUE_EMPTY_MASK     0x04000000 /* MDP_TXIOC1_QE[26] */
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TXIOC1_QUEUE_EMPTY_SHFT     26
#define WF_PSE_TOP_QUEUE_EMPTY_SEC_TX1_QUEUE_EMPTY_ADDR        WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_SEC_TX1_QUEUE_EMPTY_MASK        0x02000000 /* SEC_TX1_QE[25] */
#define WF_PSE_TOP_QUEUE_EMPTY_SEC_TX1_QUEUE_EMPTY_SHFT        25
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TX1_QUEUE_EMPTY_ADDR        WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TX1_QUEUE_EMPTY_MASK        0x01000000 /* MDP_TX1_QE[24] */
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TX1_QUEUE_EMPTY_SHFT        24
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_RXIOC_QUEUE_EMPTY_ADDR      WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_RXIOC_QUEUE_EMPTY_MASK      0x00800000 /* MDP_RXIOC_QE[23] */
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_RXIOC_QUEUE_EMPTY_SHFT      23
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TXIOC_QUEUE_EMPTY_ADDR      WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TXIOC_QUEUE_EMPTY_MASK      0x00400000 /* MDP_TXIOC_QE[22] */
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TXIOC_QUEUE_EMPTY_SHFT      22
#define WF_PSE_TOP_QUEUE_EMPTY_SFD_PARK_QUEUE_EMPTY_ADDR       WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_SFD_PARK_QUEUE_EMPTY_MASK       0x00200000 /* SFD_PARK_QE[21] */
#define WF_PSE_TOP_QUEUE_EMPTY_SFD_PARK_QUEUE_EMPTY_SHFT       21
#define WF_PSE_TOP_QUEUE_EMPTY_SEC_RX_QUEUE_EMPTY_ADDR         WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_SEC_RX_QUEUE_EMPTY_MASK         0x00100000 /* SEC_RX_QE[20] */
#define WF_PSE_TOP_QUEUE_EMPTY_SEC_RX_QUEUE_EMPTY_SHFT         20
#define WF_PSE_TOP_QUEUE_EMPTY_SEC_TX_QUEUE_EMPTY_ADDR         WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_SEC_TX_QUEUE_EMPTY_MASK         0x00080000 /* SEC_TX_QE[19] */
#define WF_PSE_TOP_QUEUE_EMPTY_SEC_TX_QUEUE_EMPTY_SHFT         19
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_RX_QUEUE_EMPTY_ADDR         WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_RX_QUEUE_EMPTY_MASK         0x00040000 /* MDP_RX_QE[18] */
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_RX_QUEUE_EMPTY_SHFT         18
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TX_QUEUE_EMPTY_ADDR         WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TX_QUEUE_EMPTY_MASK         0x00020000 /* MDP_TX_QE[17] */
#define WF_PSE_TOP_QUEUE_EMPTY_MDP_TX_QUEUE_EMPTY_SHFT         17
#define WF_PSE_TOP_QUEUE_EMPTY_LMAC_TX_QUEUE_EMPTY_ADDR        WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_LMAC_TX_QUEUE_EMPTY_MASK        0x00010000 /* LMAC_TX_QE[16] */
#define WF_PSE_TOP_QUEUE_EMPTY_LMAC_TX_QUEUE_EMPTY_SHFT        16
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_6_EMPTY_ADDR                WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_6_EMPTY_MASK                0x00004000 /* HIF_6_EMPTY[14] */
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_6_EMPTY_SHFT                14
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_5_EMPTY_ADDR                WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_5_EMPTY_MASK                0x00002000 /* HIF_5_EMPTY[13] */
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_5_EMPTY_SHFT                13
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_4_EMPTY_ADDR                WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_4_EMPTY_MASK                0x00001000 /* HIF_4_EMPTY[12] */
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_4_EMPTY_SHFT                12
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_3_EMPTY_ADDR                WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_3_EMPTY_MASK                0x00000800 /* HIF_3_EMPTY[11] */
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_3_EMPTY_SHFT                11
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_2_EMPTY_ADDR                WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_2_EMPTY_MASK                0x00000400 /* HIF_2_EMPTY[10] */
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_2_EMPTY_SHFT                10
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_1_EMPTY_ADDR                WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_1_EMPTY_MASK                0x00000200 /* HIF_1_EMPTY[9] */
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_1_EMPTY_SHFT                9
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_0_EMPTY_ADDR                WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_0_EMPTY_MASK                0x00000100 /* HIF_0_EMPTY[8] */
#define WF_PSE_TOP_QUEUE_EMPTY_HIF_0_EMPTY_SHFT                8
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q3_EMPTY_ADDR               WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q3_EMPTY_MASK               0x00000008 /* CPU_Q3_EMPTY[3] */
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q3_EMPTY_SHFT               3
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q2_EMPTY_ADDR               WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q2_EMPTY_MASK               0x00000004 /* CPU_Q2_EMPTY[2] */
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q2_EMPTY_SHFT               2
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q1_EMPTY_ADDR               WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q1_EMPTY_MASK               0x00000002 /* CPU_Q1_EMPTY[1] */
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q1_EMPTY_SHFT               1
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q0_EMPTY_ADDR               WF_PSE_TOP_QUEUE_EMPTY_ADDR
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q0_EMPTY_MASK               0x00000001 /* CPU_Q0_EMPTY[0] */
#define WF_PSE_TOP_QUEUE_EMPTY_CPU_Q0_EMPTY_SHFT               0

#define WF_PSE_TOP_FL_QUE_CTRL_0_EXECUTE_ADDR         WF_PSE_TOP_FL_QUE_CTRL_0_ADDR
#define WF_PSE_TOP_FL_QUE_CTRL_0_EXECUTE_MASK         0x80000000 /* EXECUTE[31] */
#define WF_PSE_TOP_FL_QUE_CTRL_0_EXECUTE_SHFT         31
#define WF_PSE_TOP_FL_QUE_CTRL_0_Q_BUF_QID_ADDR       WF_PSE_TOP_FL_QUE_CTRL_0_ADDR
#define WF_PSE_TOP_FL_QUE_CTRL_0_Q_BUF_QID_MASK       0x7F000000 /* Q_BUF_QID[30..24] */
#define WF_PSE_TOP_FL_QUE_CTRL_0_Q_BUF_QID_SHFT       24
#define WF_PSE_TOP_FL_QUE_CTRL_0_FL_BUFFER_ADDR_ADDR  WF_PSE_TOP_FL_QUE_CTRL_0_ADDR
#define WF_PSE_TOP_FL_QUE_CTRL_0_FL_BUFFER_ADDR_MASK  0x00FFF000 /* FL_BUFFER_ADDR[23..12] */
#define WF_PSE_TOP_FL_QUE_CTRL_0_FL_BUFFER_ADDR_SHFT  12
#define WF_PSE_TOP_FL_QUE_CTRL_0_Q_BUF_PID_ADDR       WF_PSE_TOP_FL_QUE_CTRL_0_ADDR
#define WF_PSE_TOP_FL_QUE_CTRL_0_Q_BUF_PID_MASK       0x00000C00 /* Q_BUF_PID[11..10] */
#define WF_PSE_TOP_FL_QUE_CTRL_0_Q_BUF_PID_SHFT       10
#define WF_PSE_TOP_FL_QUE_CTRL_0_Q_BUF_WLANID_ADDR    WF_PSE_TOP_FL_QUE_CTRL_0_ADDR
#define WF_PSE_TOP_FL_QUE_CTRL_0_Q_BUF_WLANID_MASK    0x000003FF /* Q_BUF_WLANID[9..0] */
#define WF_PSE_TOP_FL_QUE_CTRL_0_Q_BUF_WLANID_SHFT    0

#define WF_PSE_TOP_FL_QUE_CTRL_2_QUEUE_TAIL_FID_ADDR  WF_PSE_TOP_FL_QUE_CTRL_2_ADDR
#define WF_PSE_TOP_FL_QUE_CTRL_2_QUEUE_TAIL_FID_MASK  0x0FFF0000 /* QUEUE_TAIL_FID[27..16] */
#define WF_PSE_TOP_FL_QUE_CTRL_2_QUEUE_TAIL_FID_SHFT  16
#define WF_PSE_TOP_FL_QUE_CTRL_2_QUEUE_HEAD_FID_ADDR  WF_PSE_TOP_FL_QUE_CTRL_2_ADDR
#define WF_PSE_TOP_FL_QUE_CTRL_2_QUEUE_HEAD_FID_MASK  0x00000FFF /* QUEUE_HEAD_FID[11..0] */
#define WF_PSE_TOP_FL_QUE_CTRL_2_QUEUE_HEAD_FID_SHFT  0

#define WF_PSE_TOP_FL_QUE_CTRL_3_QUEUE_PAGE_NUM_ADDR  WF_PSE_TOP_FL_QUE_CTRL_3_ADDR
#define WF_PSE_TOP_FL_QUE_CTRL_3_QUEUE_PAGE_NUM_MASK  0x00FFF000 /* QUEUE_PAGE_NUM[23..12] */
#define WF_PSE_TOP_FL_QUE_CTRL_3_QUEUE_PAGE_NUM_SHFT  12
#define WF_PSE_TOP_FL_QUE_CTRL_3_QUEUE_PKT_NUM_ADDR   WF_PSE_TOP_FL_QUE_CTRL_3_ADDR
#define WF_PSE_TOP_FL_QUE_CTRL_3_QUEUE_PKT_NUM_MASK   0x00000FFF /* QUEUE_PKT_NUM[11..0] */
#define WF_PSE_TOP_FL_QUE_CTRL_3_QUEUE_PKT_NUM_SHFT   0

#endif
