// SPDX-License-Identifier: ISC
/* Copyright (C) 2020 MediaTek Inc. */

#include "mt7915.h"
#include "eeprom.h"
#include "mcu.h"

/** global debugfs **/

static int
mt7915_implicit_txbf_set(void *data, u64 val)
{
	struct mt7915_dev *dev = data;

	if (test_bit(MT76_STATE_RUNNING, &dev->mphy.state))
		return -EBUSY;

	dev->ibf = !!val;

	return mt7915_mcu_set_txbf(dev, MT_BF_TYPE_UPDATE);
}

static int
mt7915_implicit_txbf_get(void *data, u64 *val)
{
	struct mt7915_dev *dev = data;

	*val = dev->ibf;

	return 0;
}

DEFINE_DEBUGFS_ATTRIBUTE(fops_implicit_txbf, mt7915_implicit_txbf_get,
			 mt7915_implicit_txbf_set, "%lld\n");

/* test knob of system layer 1/2 error recovery */
static int mt7915_ser_trigger_set(void *data, u64 val)
{
	enum {
		SER_SET_RECOVER_L1 = 1,
		SER_SET_RECOVER_L2,
		SER_ENABLE = 2,
		SER_RECOVER
	};
	struct mt7915_dev *dev = data;
	int ret = 0;

	switch (val) {
	case SER_SET_RECOVER_L1:
	case SER_SET_RECOVER_L2:
		ret = mt7915_mcu_set_ser(dev, SER_ENABLE, BIT(val), 0);
		if (ret)
			return ret;

		return mt7915_mcu_set_ser(dev, SER_RECOVER, val, 0);
	default:
		break;
	}

	return ret;
}

DEFINE_DEBUGFS_ATTRIBUTE(fops_ser_trigger, NULL,
			 mt7915_ser_trigger_set, "%lld\n");

static int
mt7915_radar_trigger(void *data, u64 val)
{
	struct mt7915_dev *dev = data;

	return mt7915_mcu_rdd_cmd(dev, RDD_RADAR_EMULATE, 1, 0, 0);
}

DEFINE_DEBUGFS_ATTRIBUTE(fops_radar_trigger, NULL,
			 mt7915_radar_trigger, "%lld\n");

static int
mt7915_fw_debug_set(void *data, u64 val)
{
	struct mt7915_dev *dev = data;
	enum {
		DEBUG_TXCMD = 62,
		DEBUG_CMD_RPT_TX,
		DEBUG_CMD_RPT_TRIG,
		DEBUG_SPL,
		DEBUG_RPT_RX,
	} debug;

	dev->fw_debug = !!val;

	mt7915_mcu_fw_log_2_host(dev, dev->fw_debug ? 2 : 0);

	for (debug = DEBUG_TXCMD; debug <= DEBUG_RPT_RX; debug++)
		mt7915_mcu_fw_dbg_ctrl(dev, debug, dev->fw_debug);

	return 0;
}

static int
mt7915_fw_debug_get(void *data, u64 *val)
{
	struct mt7915_dev *dev = data;

	*val = dev->fw_debug;

	return 0;
}

DEFINE_DEBUGFS_ATTRIBUTE(fops_fw_debug, mt7915_fw_debug_get,
			 mt7915_fw_debug_set, "%lld\n");

struct mt7915_txo_worker_info {
	char* buf;
	int sofar;
	int size;
};

static void mt7915_txo_worker(void *wi_data, struct ieee80211_sta *sta)
{
	struct mt7915_txo_worker_info *wi = wi_data;
	struct mt7915_sta *msta = (struct mt7915_sta *)sta->drv_priv;
	struct mt76_testmode_data *td = &msta->test;
	struct ieee80211_vif *vif;
	struct wireless_dev *wdev;

	if (wi->sofar >= wi->size)
		return; /* buffer is full */

	vif = container_of((void *)msta->vif, struct ieee80211_vif, drv_priv);
	wdev = ieee80211_vif_to_wdev(vif);

	wi->sofar += scnprintf(wi->buf + wi->sofar, wi->size - wi->sofar,
			       "vdev (%s) active=%d tpc=%d sgi=%d mcs=%d nss=%d"
			       " pream=%d retries=%d dynbw=%d bw=%d\n",
			       wdev->netdev->name,
			       td->txo_active, td->tx_power[0],
			       td->tx_rate_sgi, td->tx_rate_idx,
			       td->tx_rate_nss, td->tx_rate_mode,
			       td->tx_xmit_count, td->tx_dynbw,
			       td->txbw);
}

static ssize_t mt7915_read_set_rate_override(struct file *file,
					     char __user *user_buf,
					     size_t count, loff_t *ppos)
{
	struct mt7915_dev *dev = file->private_data;
        struct ieee80211_hw *hw = dev->mphy.hw;
	char *buf2;
	int size = 8000;
	int rv, sofar;
	struct mt7915_txo_worker_info wi;
	const char buf[] =
		"This allows specify specif tx rate parameters for all DATA"
		" frames on a vdev\n"
		"To set a value, you specify the dev-name and key-value pairs:\n"
		"tpc=10 sgi=1 mcs=x nss=x pream=x retries=x dynbw=0|1 bw=x enable=0|1\n"
		"pream: 0=cck, 1=ofdm, 2=HT, 3=VHT, 4=HE_SU\n"
		"cck-mcs: 0=1Mbps, 1=2Mbps, 3=5.5Mbps, 3=11Mbps\n"
		"ofdm-mcs: 0=6Mbps, 1=9Mbps, 2=12Mbps, 3=18Mbps, 4=24Mbps, 5=36Mbps,"
		" 6=48Mbps, 7=54Mbps\n"
		"tpc is not implemented currently, bw is 0-3 for 20-160\n"
		" For example, wlan0:\n"
		"echo \"wlan0 tpc=255 sgi=1 mcs=0 nss=1 pream=3 retries=1 dynbw=0 bw=0"
		" active=1\" > ...mt76/set_rate_override\n";

	buf2 = kzalloc(size, GFP_KERNEL);
	if (!buf2)
		return -ENOMEM;
	strcpy(buf2, buf);
	sofar = strlen(buf2);

	wi.sofar = sofar;
	wi.buf = buf2;
	wi.size = size;

	ieee80211_iterate_stations_atomic(hw, mt7915_txo_worker, &wi);

	rv = simple_read_from_buffer(user_buf, count, ppos, buf2, wi.sofar);
	kfree(buf2);
	return rv;
}

/* Set the rates for specific types of traffic.
 */
static ssize_t mt7915_write_set_rate_override(struct file *file,
					      const char __user *user_buf,
					      size_t count, loff_t *ppos)
{
	struct mt7915_dev *dev = file->private_data;
	struct mt7915_sta *msta;
	struct ieee80211_vif *vif;
	struct mt76_testmode_data *td = NULL;
	struct wireless_dev *wdev;
	struct mt76_wcid *wcid;
	struct mt76_phy *mphy = &dev->mt76.phy;
	char buf[180];
	char tmp[20];
	char *tok;
	int ret, i, j;
	unsigned int vdev_id = 0xFFFF;
	char *bufptr = buf;
	long rc;
	char dev_name_match[IFNAMSIZ + 2];

	memset(buf, 0, sizeof(buf));

	simple_write_to_buffer(buf, sizeof(buf) - 1, ppos, user_buf, count);

	/* make sure that buf is null terminated */
	buf[sizeof(buf) - 1] = 0;

	/* drop the possible '\n' from the end */
	if (buf[count - 1] == '\n')
		buf[count - 1] = 0;

	mutex_lock(&mphy->dev->mutex);

	/* Ignore empty lines, 'echo' appends them sometimes at least. */
	if (buf[0] == 0) {
		ret = count;
		goto exit;
	}

	/* String starts with vdev name, ie 'wlan0'  Find the proper vif that
	 * matches the name.
	 */
	for (i = 0; i < ARRAY_SIZE(dev->mt76.wcid_mask); i++) {
		u32 mask = dev->mt76.wcid_mask[i];
		u32 phy_mask = dev->mt76.wcid_phy_mask[i];

		if (!mask)
			continue;

		for (j = i * 32; mask; j++, mask >>= 1, phy_mask >>= 1) {
			if (!(mask & 1))
				continue;

			wcid = rcu_dereference(dev->mt76.wcid[j]);
			if (!wcid)
				continue;

			msta = container_of(wcid, struct mt7915_sta, wcid);

			vif = container_of((void *)msta->vif, struct ieee80211_vif, drv_priv);

			wdev = ieee80211_vif_to_wdev(vif);

			if (!wdev)
				continue;

			snprintf(dev_name_match, sizeof(dev_name_match) - 1, "%s ",
				 wdev->netdev->name);

			if (strncmp(dev_name_match, buf, strlen(dev_name_match)) == 0) {
				vdev_id = j;
				td = &msta->test;
				bufptr = buf + strlen(dev_name_match) - 1;
				break;
			}
		}
	}

	if (vdev_id == 0xFFFF) {
		if (strstr(buf, "active=0")) {
			/* Ignore, we are disabling it anyway */
			ret = count;
			goto exit;
		} else {
			dev_info(dev->mt76.dev,
				 "mt7915: set-rate-override, unknown netdev name: %s\n", buf);
		}
		ret = -EINVAL;
		goto exit;
	}

#define MT7915_PARSE_LTOK(a, b)					\
	do {								\
		tok = strstr(bufptr, " " #a "=");			\
		if (tok) {						\
			char *tspace;					\
			tok += 1; /* move past initial space */		\
			strncpy(tmp, tok + strlen(#a "="), sizeof(tmp) - 1); \
			tmp[sizeof(tmp) - 1] = 0;			\
			tspace = strstr(tmp, " ");			\
			if (tspace)					\
				*tspace = 0;				\
			if (kstrtol(tmp, 0, &rc) != 0)			\
				dev_info(dev->mt76.dev,			\
					 "mt7915: set-rate-override: " #a \
					 "= could not be parsed, tmp: %s\n", \
					 tmp);				\
			else						\
				td->b = rc;				\
		}							\
	} while (0)

	/* TODO:  Allow configuring LTF? */
	td->tx_ltf = 1; /* 0: HTLTF 3.2us, 1: HELTF, 6.4us, 2 HELTF 12,8us */

	MT7915_PARSE_LTOK(tpc, tx_power[0]);
	MT7915_PARSE_LTOK(sgi, tx_rate_sgi);
	MT7915_PARSE_LTOK(mcs, tx_rate_idx);
	MT7915_PARSE_LTOK(nss, tx_rate_nss);
	MT7915_PARSE_LTOK(pream, tx_rate_mode);
	MT7915_PARSE_LTOK(retries, tx_xmit_count);
	MT7915_PARSE_LTOK(dynbw, tx_dynbw);
	MT7915_PARSE_LTOK(bw, txbw);
	MT7915_PARSE_LTOK(active, txo_active);

	dev_info(dev->mt76.dev,
		 "mt7915: set-rate-overrides, vdev %i(%s) active=%d tpc=%d sgi=%d mcs=%d"
		 " nss=%d pream=%d retries=%d dynbw=%d bw=%d\n",
		 vdev_id, dev_name_match,
		 td->txo_active, td->tx_power[0], td->tx_rate_sgi, td->tx_rate_idx,
		 td->tx_rate_nss, td->tx_rate_mode, td->tx_xmit_count, td->tx_dynbw,
		 td->txbw);

	ret = count;

exit:
	mutex_unlock(&mphy->dev->mutex);
	return ret;
}

static const struct file_operations fops_set_rate_override = {
	.read = mt7915_read_set_rate_override,
	.write = mt7915_write_set_rate_override,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

static int
mt7915_txs_for_no_skb_set(void *data, u64 val)
{
	struct mt7915_dev *dev = data;

	dev->txs_for_no_skb_enabled = !!val;

	return 0;
}

static int
mt7915_txs_for_no_skb_get(void *data, u64 *val)
{
	struct mt7915_dev *dev = data;

	*val = dev->txs_for_no_skb_enabled;

	return 0;
}

DEFINE_DEBUGFS_ATTRIBUTE(fops_txs_for_no_skb, mt7915_txs_for_no_skb_get,
			 mt7915_txs_for_no_skb_set, "%lld\n");

static int
mt7915_rx_group_5_enable_set(void *data, u64 val)
{
	struct mt7915_dev *dev = data;

	mutex_lock(&dev->mt76.mutex);

	dev->rx_group_5_enable = !!val;

	/* Enabled if we requested enabled OR if monitor mode is enabled. */
	mt76_rmw_field(dev, MT_DMA_DCR0(0), MT_DMA_DCR0_RXD_G5_EN,
		       dev->phy.is_monitor_mode || dev->rx_group_5_enable);
	mt76_testmode_reset(dev->phy.mt76, true);

	mutex_unlock(&dev->mt76.mutex);

	return 0;
}

static int
mt7915_rx_group_5_enable_get(void *data, u64 *val)
{
	struct mt7915_dev *dev = data;

	*val = dev->rx_group_5_enable;

	return 0;
}

DEFINE_DEBUGFS_ATTRIBUTE(fops_rx_group_5_enable, mt7915_rx_group_5_enable_get,
			 mt7915_rx_group_5_enable_set, "%lld\n");

static void
mt7915_ampdu_stat_read_phy(struct mt7915_phy *phy,
			   struct seq_file *file)
{
	struct mt7915_dev *dev = file->private;
	bool ext_phy = phy != &dev->phy;
	int bound[15], range[4], i, n;

	if (!phy)
		return;

	/* Tx ampdu stat */
	for (i = 0; i < ARRAY_SIZE(range); i++)
		range[i] = mt76_rr(dev, MT_MIB_ARNG(ext_phy, i));

	for (i = 0; i < ARRAY_SIZE(bound); i++)
		bound[i] = MT_MIB_ARNCR_RANGE(range[i / 4], i % 4) + 1;

	seq_printf(file, "\nPhy %d\n", ext_phy);

	seq_printf(file, "Length: %8d | ", bound[0]);
	for (i = 0; i < ARRAY_SIZE(bound) - 1; i++)
		seq_printf(file, "%3d -%3d | ",
			   bound[i] + 1, bound[i + 1]);

	seq_puts(file, "\nCount:  ");
	n = ext_phy ? ARRAY_SIZE(dev->mt76.aggr_stats) / 2 : 0;
	for (i = 0; i < ARRAY_SIZE(bound); i++)
		seq_printf(file, "%8d | ", dev->mt76.aggr_stats[i + n]);
	seq_puts(file, "\n");

	seq_printf(file, "BA miss count: %d\n", phy->mib.ba_miss_cnt);
}

static void
mt7915_txbf_stat_read_phy(struct mt7915_phy *phy, struct seq_file *s)
{
	static const char * const bw[] = {
		"BW20", "BW40", "BW80", "BW160"
	};
	struct mib_stats *mib;

	if (!phy)
		return;

	mib = &phy->mib;

	/* Tx Beamformer monitor */
	seq_puts(s, "\nTx Beamformer applied PPDU counts: ");

	seq_printf(s, "iBF: %d, eBF: %d\n",
		   mib->tx_bf_ibf_ppdu_cnt,
		   mib->tx_bf_ebf_ppdu_cnt);

	/* Tx Beamformer Rx feedback monitor */
	seq_puts(s, "Tx Beamformer Rx feedback statistics: ");

	seq_printf(s, "All: %d, HE: %d, VHT: %d, HT: %d, ",
		   mib->tx_bf_rx_fb_all_cnt,
		   mib->tx_bf_rx_fb_he_cnt,
		   mib->tx_bf_rx_fb_vht_cnt,
		   mib->tx_bf_rx_fb_ht_cnt);

	seq_printf(s, "%s, NC: %d, NR: %d\n",
		   bw[mib->tx_bf_rx_fb_bw],
		   mib->tx_bf_rx_fb_nc_cnt,
		   mib->tx_bf_rx_fb_nr_cnt);

	/* Tx Beamformee Rx NDPA & Tx feedback report */
	seq_printf(s, "Tx Beamformee successful feedback frames: %d\n",
		   mib->tx_bf_fb_cpl_cnt);
	seq_printf(s, "Tx Beamformee feedback triggered counts: %d\n",
		   mib->tx_bf_fb_trig_cnt);

	/* Tx SU & MU counters */
	seq_printf(s, "Tx multi-user Beamforming counts: %d\n",
		   mib->tx_bf_cnt);
	seq_printf(s, "Tx multi-user MPDU counts: %d\n", mib->tx_mu_mpdu_cnt);
	seq_printf(s, "Tx multi-user successful MPDU counts: %d\n",
		   mib->tx_mu_acked_mpdu_cnt);
	seq_printf(s, "Tx single-user successful MPDU counts: %d\n",
		   mib->tx_su_acked_mpdu_cnt);

	seq_puts(s, "\n");
}

static int
mt7915_tx_stats_show(struct seq_file *file, void *data)
{
	struct mt7915_dev *dev = file->private;
	int i;
	long n;
	struct mib_stats *mib = &dev->phy.mib;

	mt7915_ampdu_stat_read_phy(&dev->phy, file);
	mt7915_txbf_stat_read_phy(&dev->phy, file);

	mt7915_ampdu_stat_read_phy(mt7915_ext_phy(dev), file);
	mt7915_txbf_stat_read_phy(mt7915_ext_phy(dev), file);

	/* Tx amsdu info */
	seq_puts(file, "Tx MSDU statistics:\n");
	for (i = 0, n = 0; i < ARRAY_SIZE(mib->tx_amsdu_pack_stats); i++)
		n += mib->tx_amsdu_pack_stats[i];

	for (i = 0; i < ARRAY_SIZE(mib->tx_amsdu_pack_stats); i++) {
		long si = mib->tx_amsdu_pack_stats[i];

		seq_printf(file, "AMSDU pack count of %d MSDU in TXD: %ld ",
			   i + 1, si);
		if (n != 0)
			seq_printf(file, "(%ld%%)\n", si * 100 / n);
		else
			seq_puts(file, "\n");
	}

	return 0;
}

DEFINE_SHOW_ATTRIBUTE(mt7915_tx_stats);

struct mt7915_empty_q_info {
	const char *qname;
	u32 port_id;
	u32 q_id;
};

static struct mt7915_empty_q_info pse_queue_empty_info[] = {
	{"CPU Q0",  ENUM_UMAC_CPU_PORT_1,     ENUM_UMAC_CTX_Q_0},
	{"CPU Q1",  ENUM_UMAC_CPU_PORT_1,     ENUM_UMAC_CTX_Q_1},
	{"CPU Q2",  ENUM_UMAC_CPU_PORT_1,     ENUM_UMAC_CTX_Q_2},
	{"CPU Q3",  ENUM_UMAC_CPU_PORT_1,     ENUM_UMAC_CTX_Q_3},
	{NULL, 0, 0}, {NULL, 0, 0}, {NULL, 0, 0}, {NULL, 0, 0}, /* 4~7 not defined */
	{"HIF Q0", ENUM_UMAC_HIF_PORT_0,    0}, /* Q8 */
	{"HIF Q1", ENUM_UMAC_HIF_PORT_0,    1},
	{"HIF Q2", ENUM_UMAC_HIF_PORT_0,    2},
	{"HIF Q3", ENUM_UMAC_HIF_PORT_0,    3},
	{"HIF Q4", ENUM_UMAC_HIF_PORT_0,    4},
	{"HIF Q5", ENUM_UMAC_HIF_PORT_0,    5},
	{NULL, 0, 0}, {NULL, 0, 0},  /* 14~15 not defined */
	{"LMAC Q",  ENUM_UMAC_LMAC_PORT_2,    0},
	{"MDP TX Q", ENUM_UMAC_LMAC_PORT_2, 1},
	{"MDP RX Q", ENUM_UMAC_LMAC_PORT_2, 2},
	{"SEC TX Q", ENUM_UMAC_LMAC_PORT_2, 3},
	{"SEC RX Q", ENUM_UMAC_LMAC_PORT_2, 4},
	{"SFD_PARK Q", ENUM_UMAC_LMAC_PORT_2, 5},
	{"MDP_TXIOC Q", ENUM_UMAC_LMAC_PORT_2, 6},
	{"MDP_RXIOC Q", ENUM_UMAC_LMAC_PORT_2, 7},
	{NULL, 0, 0}, {NULL, 0, 0}, {NULL, 0, 0}, {NULL, 0, 0},
	{NULL, 0, 0}, {NULL, 0, 0}, {NULL, 0, 0}, /* 24~30 not defined */
	{"RLS Q",  ENUM_PLE_CTRL_PSE_PORT_3, ENUM_UMAC_PLE_CTRL_P3_Q_0X1F}
};

static void
mt7915_pse_q_nonempty_stat_read_phy(struct mt7915_phy *phy,
				    struct seq_file *file)
{
	struct mt7915_dev *dev = file->private;
	u32 pse_stat;
	int i;
	u32 pg_flow_ctrl[22] = {0};
	u32 fpg_cnt, ffa_cnt, fpg_head, fpg_tail;
	u32 max_q, min_q, rsv_pg, used_pg;
	u32 pse_buf_ctrl, pg_sz, pg_num;


	pse_stat = mt76_rr(dev, WF_PSE_TOP_QUEUE_EMPTY_ADDR);

	pse_buf_ctrl = mt76_rr(dev, WF_PSE_TOP_PBUF_CTRL_ADDR);
	pg_flow_ctrl[0] = mt76_rr(dev, WF_PSE_TOP_FREEPG_CNT_ADDR);
	pg_flow_ctrl[1] = mt76_rr(dev, WF_PSE_TOP_FREEPG_HEAD_TAIL_ADDR);
	pg_flow_ctrl[2] = mt76_rr(dev, WF_PSE_TOP_PG_HIF0_GROUP_ADDR);
	pg_flow_ctrl[3] = mt76_rr(dev, WF_PSE_TOP_HIF0_PG_INFO_ADDR);
	pg_flow_ctrl[4] = mt76_rr(dev, WF_PSE_TOP_PG_HIF1_GROUP_ADDR);
	pg_flow_ctrl[5] = mt76_rr(dev, WF_PSE_TOP_HIF1_PG_INFO_ADDR);
	pg_flow_ctrl[6] = mt76_rr(dev, WF_PSE_TOP_PG_CPU_GROUP_ADDR);
	pg_flow_ctrl[7] = mt76_rr(dev, WF_PSE_TOP_CPU_PG_INFO_ADDR);
	pg_flow_ctrl[8] = mt76_rr(dev, WF_PSE_TOP_PG_LMAC0_GROUP_ADDR);
	pg_flow_ctrl[9] = mt76_rr(dev, WF_PSE_TOP_LMAC0_PG_INFO_ADDR);
	pg_flow_ctrl[10] = mt76_rr(dev, WF_PSE_TOP_PG_LMAC1_GROUP_ADDR);
	pg_flow_ctrl[11] = mt76_rr(dev, WF_PSE_TOP_LMAC1_PG_INFO_ADDR);
	pg_flow_ctrl[12] = mt76_rr(dev, WF_PSE_TOP_PG_LMAC2_GROUP_ADDR);
	pg_flow_ctrl[13] = mt76_rr(dev, WF_PSE_TOP_LMAC2_PG_INFO_ADDR);
	pg_flow_ctrl[14] = mt76_rr(dev, WF_PSE_TOP_PG_PLE_GROUP_ADDR);
	pg_flow_ctrl[15] = mt76_rr(dev, WF_PSE_TOP_PLE_PG_INFO_ADDR);
	pg_flow_ctrl[16] = mt76_rr(dev, WF_PSE_TOP_PG_LMAC3_GROUP_ADDR);
	pg_flow_ctrl[17] = mt76_rr(dev, WF_PSE_TOP_LMAC3_PG_INFO_ADDR);
	pg_flow_ctrl[18] = mt76_rr(dev, WF_PSE_TOP_PG_MDP_GROUP_ADDR);
	pg_flow_ctrl[19] = mt76_rr(dev, WF_PSE_TOP_MDP_PG_INFO_ADDR);
	pg_flow_ctrl[20] = mt76_rr(dev, WF_PSE_TOP_PG_PLE1_GROUP_ADDR);
	pg_flow_ctrl[21] = mt76_rr(dev, WF_PSE_TOP_PLE1_PG_INFO_ADDR);

	seq_puts(file, "PSE Configuration Info:\n");
	seq_printf(file, "\tPacket Buffer Control: 0x%08x\n", pse_buf_ctrl);

	pg_sz = (pse_buf_ctrl & WF_PSE_TOP_PBUF_CTRL_PAGE_SIZE_CFG_MASK)
		>> WF_PSE_TOP_PBUF_CTRL_PAGE_SIZE_CFG_SHFT;
	seq_printf(file, "\t\tPage Size:   %d(%d bytes per page)\n",
		   pg_sz, (pg_sz == 1 ? 256 : 128));
	seq_printf(file, "\t\tPage Offset: %d(in unit of 64KB)\n",
		   (pse_buf_ctrl & WF_PSE_TOP_PBUF_CTRL_PBUF_OFFSET_MASK)
		   >> WF_PSE_TOP_PBUF_CTRL_PBUF_OFFSET_SHFT);
	pg_num = (pse_buf_ctrl & WF_PSE_TOP_PBUF_CTRL_TOTAL_PAGE_NUM_MASK)
		>> WF_PSE_TOP_PBUF_CTRL_TOTAL_PAGE_NUM_SHFT;
	seq_printf(file, "\t\tTotal page numbers: %d pages\n", pg_num);

	/* Page Flow Control */
	seq_puts(file, "PSE Page Flow Control:\n");
	seq_printf(file, "\tFree page counter: 0x%08x\n", pg_flow_ctrl[0]);
	fpg_cnt = (pg_flow_ctrl[0] & WF_PSE_TOP_FREEPG_CNT_FREEPG_CNT_MASK)
		>> WF_PSE_TOP_FREEPG_CNT_FREEPG_CNT_SHFT;
	seq_printf(file, "\t\tThe toal page number of free: 0x%03x\n", fpg_cnt);
	ffa_cnt = (pg_flow_ctrl[0] & WF_PSE_TOP_FREEPG_CNT_FFA_CNT_MASK)
		>> WF_PSE_TOP_FREEPG_CNT_FFA_CNT_SHFT;
	seq_printf(file, "\t\tThe free page numbers of free for all: 0x%03x\n",
		   ffa_cnt);
	seq_printf(file, "\tFree page head and tail: 0x%08x\n", pg_flow_ctrl[1]);
	fpg_head = (pg_flow_ctrl[1] & WF_PSE_TOP_FREEPG_HEAD_TAIL_FREEPG_HEAD_MASK)
		>> WF_PSE_TOP_FREEPG_HEAD_TAIL_FREEPG_HEAD_SHFT;
	fpg_tail = (pg_flow_ctrl[1] & WF_PSE_TOP_FREEPG_HEAD_TAIL_FREEPG_TAIL_MASK)
		>> WF_PSE_TOP_FREEPG_HEAD_TAIL_FREEPG_TAIL_SHFT;
	seq_printf(file, "\t\tThe tail/head page of free page list: 0x%03x/0x%03x\n",
		   fpg_tail, fpg_head);

#define MT7915_MMQ(idx, type, text)					\
	do {                                                            \
		int i2 = (idx);						\
									\
		min_q = (pg_flow_ctrl[i2] & type##_MIN_QUOTA_MASK)	\
			>> type##_MIN_QUOTA_SHFT;                       \
		max_q = (pg_flow_ctrl[i2] & type##_MAX_QUOTA_MASK)	\
			>> type##_MAX_QUOTA_SHFT;                       \
		seq_printf(file, "\t\t%s: %d/%d\n",			\
			   text, max_q, min_q);				\
	} while (false)

#define MT7915_RSQ(idx, type, text)					\
	do {                                                            \
		int i3 = (idx);						\
									\
		rsv_pg = (pg_flow_ctrl[i3] & type##_RSV_CNT_MASK)	\
			>> type##_RSV_CNT_SHFT;                         \
		used_pg = (pg_flow_ctrl[i3] & type##_SRC_CNT_MASK)	\
			>> type##_SRC_CNT_SHFT;                         \
		seq_printf(file, "\t\t%s: %d/%d\n",			\
			   text, used_pg, rsv_pg);			\
	} while (false)

#define MT7915_MMQ_RSQ(idx, type)					\
	do {                                                            \
		int i4 = (idx);						\
									\
		seq_printf(file, "\tReserved page counter of "          \
			   #type " group: 0x%08x\n",			\
			   pg_flow_ctrl[i4]);				\
		seq_printf(file, "\t" #type " group page status: 0x%08x\n", \
			   pg_flow_ctrl[i4 + 1]);			\
		MT7915_MMQ(i4, WF_PSE_TOP_PG_##type##_GROUP_##type,	\
			   "The max/min quota pages of " #type " group"); \
		MT7915_RSQ(i4 + 1, WF_PSE_TOP_##type##_PG_INFO_##type, \
			   "The used/reserved pages of " #type " group"); \
	} while (false)

	MT7915_MMQ_RSQ(2, HIF0);
	MT7915_MMQ_RSQ(4, HIF1);
	MT7915_MMQ_RSQ(6, CPU);
	MT7915_MMQ_RSQ(8, LMAC0);
	MT7915_MMQ_RSQ(10, LMAC1);
	MT7915_MMQ_RSQ(12, LMAC2);
	MT7915_MMQ_RSQ(16, LMAC3);
	MT7915_MMQ_RSQ(14, PLE);
	MT7915_MMQ_RSQ(20, PLE1);
	MT7915_MMQ_RSQ(18, MDP);

	/* Queue Empty Status */
	seq_puts(file, "PSE Queue Empty Status:\n");
	seq_printf(file, "\tQUEUE_EMPTY: 0x%08x\n", pse_stat);
	seq_printf(file, "\t\tCPU Q0/1/2/3 empty=%d/%d/%d/%d\n",
		   ((pse_stat & WF_PSE_TOP_QUEUE_EMPTY_CPU_Q0_EMPTY_MASK)
		    >> WF_PSE_TOP_QUEUE_EMPTY_CPU_Q0_EMPTY_SHFT),
		   ((pse_stat & WF_PSE_TOP_QUEUE_EMPTY_CPU_Q1_EMPTY_MASK)
		    >> WF_PSE_TOP_QUEUE_EMPTY_CPU_Q1_EMPTY_SHFT),
		   ((pse_stat & WF_PSE_TOP_QUEUE_EMPTY_CPU_Q2_EMPTY_MASK)
		    >> WF_PSE_TOP_QUEUE_EMPTY_CPU_Q2_EMPTY_SHFT),
		   ((pse_stat & WF_PSE_TOP_QUEUE_EMPTY_CPU_Q3_EMPTY_MASK)
		    >> WF_PSE_TOP_QUEUE_EMPTY_CPU_Q3_EMPTY_SHFT));
	seq_printf(file, "\t\tHIF Q0/1/2/3/4/5 empty=%d/%d/%d/%d/%d/%d\n",
		   ((pse_stat & WF_PSE_TOP_QUEUE_EMPTY_HIF_0_EMPTY_MASK)
		    >> WF_PSE_TOP_QUEUE_EMPTY_HIF_0_EMPTY_SHFT),
		   ((pse_stat & WF_PSE_TOP_QUEUE_EMPTY_HIF_1_EMPTY_MASK)
		    >> WF_PSE_TOP_QUEUE_EMPTY_HIF_1_EMPTY_SHFT),
		   ((pse_stat & WF_PSE_TOP_QUEUE_EMPTY_HIF_2_EMPTY_MASK)
		    >> WF_PSE_TOP_QUEUE_EMPTY_HIF_2_EMPTY_SHFT),
		   ((pse_stat & WF_PSE_TOP_QUEUE_EMPTY_HIF_3_EMPTY_MASK)
		    >> WF_PSE_TOP_QUEUE_EMPTY_HIF_3_EMPTY_SHFT),
		   ((pse_stat & WF_PSE_TOP_QUEUE_EMPTY_HIF_4_EMPTY_MASK)
		    >> WF_PSE_TOP_QUEUE_EMPTY_HIF_4_EMPTY_SHFT),
		   ((pse_stat & WF_PSE_TOP_QUEUE_EMPTY_HIF_5_EMPTY_MASK)
		    >> WF_PSE_TOP_QUEUE_EMPTY_HIF_5_EMPTY_SHFT));
	seq_printf(file, "\t\tLMAC TX Q empty=%d\n",
		   ((pse_stat & WF_PSE_TOP_QUEUE_EMPTY_LMAC_TX_QUEUE_EMPTY_MASK)
		    >> WF_PSE_TOP_QUEUE_EMPTY_LMAC_TX_QUEUE_EMPTY_SHFT));
	seq_printf(file, "\t\tMDP TX Q/RX Q empty=%d/%d\n",
		   ((pse_stat & WF_PSE_TOP_QUEUE_EMPTY_MDP_TX_QUEUE_EMPTY_MASK)
		    >> WF_PSE_TOP_QUEUE_EMPTY_MDP_TX_QUEUE_EMPTY_SHFT),
		   ((pse_stat & WF_PSE_TOP_QUEUE_EMPTY_MDP_RX_QUEUE_EMPTY_MASK)
		    >> WF_PSE_TOP_QUEUE_EMPTY_MDP_RX_QUEUE_EMPTY_SHFT));
	seq_printf(file, "\t\tSEC TX Q/RX Q empty=%d/%d\n",
		   ((pse_stat & WF_PSE_TOP_QUEUE_EMPTY_SEC_TX_QUEUE_EMPTY_MASK)
		    >> WF_PSE_TOP_QUEUE_EMPTY_SEC_TX_QUEUE_EMPTY_SHFT),
		   ((pse_stat & WF_PSE_TOP_QUEUE_EMPTY_SEC_RX_QUEUE_EMPTY_MASK)
		    >> WF_PSE_TOP_QUEUE_EMPTY_SEC_RX_QUEUE_EMPTY_SHFT));
	seq_printf(file, "\t\tSFD PARK Q empty=%d\n",
		   ((pse_stat & WF_PSE_TOP_QUEUE_EMPTY_SFD_PARK_QUEUE_EMPTY_MASK)
		    >> WF_PSE_TOP_QUEUE_EMPTY_SFD_PARK_QUEUE_EMPTY_SHFT));
	seq_printf(file, "\t\tMDP TXIOC Q/RXIOC Q empty=%d/%d\n",
		   ((pse_stat & WF_PSE_TOP_QUEUE_EMPTY_MDP_TXIOC_QUEUE_EMPTY_MASK)
		    >> WF_PSE_TOP_QUEUE_EMPTY_MDP_TXIOC_QUEUE_EMPTY_SHFT),
		   ((pse_stat & WF_PSE_TOP_QUEUE_EMPTY_MDP_RXIOC_QUEUE_EMPTY_MASK)
		    >> WF_PSE_TOP_QUEUE_EMPTY_MDP_RXIOC_QUEUE_EMPTY_SHFT));
	seq_printf(file, "\t\tRLS Q empty=%d\n",
		   ((pse_stat & WF_PSE_TOP_QUEUE_EMPTY_RLS_Q_EMTPY_MASK)
		    >> WF_PSE_TOP_QUEUE_EMPTY_RLS_Q_EMTPY_SHFT));
	seq_printf(file,  ("Non-Empty Q info:\n"));

	for (i = 0; i < 31; i++) {
		if (((pse_stat & (0x1 << i)) >> i) == 0) {
			u32 hfid, tfid, pktcnt, fl_que_ctrl[3] = {0};

			if (pse_queue_empty_info[i].qname) {
				seq_printf(file,  "\t%s: ", pse_queue_empty_info[i].qname);
				fl_que_ctrl[0] |= WF_PSE_TOP_FL_QUE_CTRL_0_EXECUTE_MASK;
				fl_que_ctrl[0] |= (pse_queue_empty_info[i].port_id
						   << WF_PSE_TOP_FL_QUE_CTRL_0_Q_BUF_PID_SHFT);
				fl_que_ctrl[0] |= (pse_queue_empty_info[i].q_id
						   << WF_PSE_TOP_FL_QUE_CTRL_0_Q_BUF_QID_SHFT);
			} else {
				continue;
			}

			/*  Executes frame link and queue structure buffer read command */
			fl_que_ctrl[0] |= (0x1 << 31);
			mt76_wr(dev, WF_PSE_TOP_FL_QUE_CTRL_0_ADDR, fl_que_ctrl[0]);

			fl_que_ctrl[1] = mt76_rr(dev, WF_PSE_TOP_FL_QUE_CTRL_2_ADDR);
			fl_que_ctrl[2] = mt76_rr(dev, WF_PSE_TOP_FL_QUE_CTRL_3_ADDR);
			hfid = (fl_que_ctrl[1] & WF_PSE_TOP_FL_QUE_CTRL_2_QUEUE_HEAD_FID_MASK)
				>> WF_PSE_TOP_FL_QUE_CTRL_2_QUEUE_HEAD_FID_SHFT;
			tfid = (fl_que_ctrl[1] & WF_PSE_TOP_FL_QUE_CTRL_2_QUEUE_TAIL_FID_MASK)
				>> WF_PSE_TOP_FL_QUE_CTRL_2_QUEUE_TAIL_FID_SHFT;
			pktcnt = (fl_que_ctrl[2] & WF_PSE_TOP_FL_QUE_CTRL_3_QUEUE_PKT_NUM_MASK)
				>> WF_PSE_TOP_FL_QUE_CTRL_3_QUEUE_PKT_NUM_SHFT;
			seq_printf(file, "tail/head fid = 0x%03x/0x%03x, pkt cnt = 0x%03x\n",
				   tfid, hfid, pktcnt);
		}
	}
}

static int
mt7915_rx_pse_stats_show(struct seq_file *file, void *data)
{
	struct mt7915_dev *dev = file->private;

	seq_puts(file, "RX PSE Stats\n");

	mt7915_pse_q_nonempty_stat_read_phy(&dev->phy, file);

	return 0;
}

DEFINE_SHOW_ATTRIBUTE(mt7915_rx_pse_stats);

static int
mt7915_queues_acq(struct seq_file *s, void *data)
{
	struct mt7915_dev *dev = dev_get_drvdata(s->private);
	int i;

	for (i = 0; i < 16; i++) {
		int j, acs = i / 4, index = i % 4;
		u32 ctrl, val, qlen = 0;

		val = mt76_rr(dev, MT_PLE_AC_QEMPTY(acs, index));
		ctrl = BIT(31) | BIT(15) | (acs << 8);

		for (j = 0; j < 32; j++) {
			if (val & BIT(j))
				continue;

			mt76_wr(dev, MT_PLE_FL_Q0_CTRL,
				ctrl | (j + (index << 5)));
			qlen += mt76_get_field(dev, MT_PLE_FL_Q3_CTRL,
					       GENMASK(11, 0));
		}
		seq_printf(s, "AC%d%d: queued=%d\n", acs, index, qlen);
	}

	return 0;
}

static int
mt7915_queues_read(struct seq_file *s, void *data)
{
	struct mt7915_dev *dev = dev_get_drvdata(s->private);
	struct mt76_phy *mphy_ext = dev->mt76.phy2;
	struct mt76_queue *ext_q = mphy_ext ? mphy_ext->q_tx[MT_TXQ_BE] : NULL;
	struct {
		struct mt76_queue *q;
		char *queue;
	} queue_map[] = {
		{ dev->mphy.q_tx[MT_TXQ_BE],	 "WFDMA0" },
		{ ext_q,			 "WFDMA1" },
		{ dev->mphy.q_tx[MT_TXQ_BE],	 "WFDMA0" },
		{ dev->mt76.q_mcu[MT_MCUQ_WM],	 "MCUWM"  },
		{ dev->mt76.q_mcu[MT_MCUQ_WA],	 "MCUWA"  },
		{ dev->mt76.q_mcu[MT_MCUQ_FWDL], "MCUFWQ" },
	};
	int i;

	for (i = 0; i < ARRAY_SIZE(queue_map); i++) {
		struct mt76_queue *q = queue_map[i].q;

		if (!q)
			continue;

		seq_printf(s,
			   "%s:	queued=%d head=%d tail=%d\n",
			   queue_map[i].queue, q->queued, q->head,
			   q->tail);
	}

	return 0;
}

static void
mt7915_puts_rate_txpower(struct seq_file *s, struct mt7915_phy *phy)
{
	static const char * const sku_group_name[] = {
		"CCK", "OFDM", "HT20", "HT40",
		"VHT20", "VHT40", "VHT80", "VHT160",
		"RU26", "RU52", "RU106", "RU242/SU20",
		"RU484/SU40", "RU996/SU80", "RU2x996/SU160"
	};
	s8 txpower[MT7915_SKU_RATE_NUM], *buf;
	int i;

	if (!phy)
		return;

	seq_printf(s, "\nBand %d\n", phy != &phy->dev->phy);

	mt7915_mcu_get_txpower_sku(phy, txpower, sizeof(txpower));
	for (i = 0, buf = txpower; i < ARRAY_SIZE(mt7915_sku_group_len); i++) {
		u8 mcs_num = mt7915_sku_group_len[i];

		if (i >= SKU_VHT_BW20 && i <= SKU_VHT_BW160)
			mcs_num = 10;

		mt76_seq_puts_array(s, sku_group_name[i], buf, mcs_num);
		buf += mt7915_sku_group_len[i];
	}
}

static int
mt7915_read_rate_txpower(struct seq_file *s, void *data)
{
	struct mt7915_dev *dev = dev_get_drvdata(s->private);

	mt7915_puts_rate_txpower(s, &dev->phy);
	mt7915_puts_rate_txpower(s, mt7915_ext_phy(dev));

	return 0;
}

int mt7915_init_debugfs(struct mt7915_dev *dev)
{
	struct dentry *dir;

	dir = mt76_register_debugfs(&dev->mt76);
	if (!dir)
		return -ENOMEM;

	debugfs_create_devm_seqfile(dev->mt76.dev, "queues", dir,
				    mt7915_queues_read);
	debugfs_create_devm_seqfile(dev->mt76.dev, "acq", dir,
				    mt7915_queues_acq);
	debugfs_create_file("tx_stats", 0400, dir, dev, &mt7915_tx_stats_fops);
	debugfs_create_file("rx_pse_stats", 0400, dir, dev, &mt7915_rx_pse_stats_fops);
	debugfs_create_file("fw_debug", 0600, dir, dev, &fops_fw_debug);
	debugfs_create_file("force_txs", 0600, dir, dev, &fops_txs_for_no_skb);
	debugfs_create_file("rx_group_5_enable", 0600, dir, dev, &fops_rx_group_5_enable);
	debugfs_create_file("implicit_txbf", 0600, dir, dev,
			    &fops_implicit_txbf);
	debugfs_create_u32("dfs_hw_pattern", 0400, dir, &dev->hw_pattern);
	/* test knobs */
	debugfs_create_file("radar_trigger", 0200, dir, dev,
			    &fops_radar_trigger);
	debugfs_create_file("ser_trigger", 0200, dir, dev, &fops_ser_trigger);
	debugfs_create_devm_seqfile(dev->mt76.dev, "txpower_sku", dir,
				    mt7915_read_rate_txpower);
	debugfs_create_file("set_rate_override", 0600, dir,
			    dev, &fops_set_rate_override);

	return 0;
}

#ifdef CONFIG_MAC80211_DEBUGFS
/** per-station debugfs **/

/* usage: <tx mode> <ldpc> <stbc> <bw> <gi> <nss> <mcs> */
static int mt7915_sta_fixed_rate_set(void *data, u64 rate)
{
	struct ieee80211_sta *sta = data;
	struct mt7915_sta *msta = (struct mt7915_sta *)sta->drv_priv;

	return mt7915_mcu_set_fixed_rate(msta->vif->phy->dev, sta, rate);
}

DEFINE_DEBUGFS_ATTRIBUTE(fops_fixed_rate, NULL,
			 mt7915_sta_fixed_rate_set, "%llx\n");

void mt7915_sta_add_debugfs(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
			    struct ieee80211_sta *sta, struct dentry *dir)
{
	debugfs_create_file("fixed_rate", 0600, dir, sta, &fops_fixed_rate);
}
#endif
