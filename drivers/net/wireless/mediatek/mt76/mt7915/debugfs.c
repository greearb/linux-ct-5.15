// SPDX-License-Identifier: ISC
/* Copyright (C) 2020 MediaTek Inc. */

#include "mt7915.h"
#include "eeprom.h"
#include "mcu.h"

/** global debugfs **/

struct hw_queue_map {
	const char *name;
	u8 index;
	u8 pid;
	u8 qid;
};

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
	enum mt_debug debug;

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
		"tpc: adjust power from defaults, in 1/2 db units 0 - 31, 16 is default\n"
		"bw is 0-3 for 20-160\n"
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

static void
mt7915_hw_queue_read(struct seq_file *s, u32 base, u32 size,
		     const struct hw_queue_map *map)
{
	struct mt7915_dev *dev = dev_get_drvdata(s->private);
	u32 i, val;

	val = mt76_rr(dev, base + MT_FL_Q_EMPTY);
	for (i = 0; i < size; i++) {
		u32 ctrl, head, tail, queued;

		if (val & BIT(map[i].index))
			continue;

		ctrl = BIT(31) | (map[i].pid << 10) | (map[i].qid << 24);
		mt76_wr(dev, base + MT_FL_Q0_CTRL, ctrl);

		head = mt76_get_field(dev, base + MT_FL_Q2_CTRL,
				      GENMASK(11, 0));
		tail = mt76_get_field(dev, base + MT_FL_Q2_CTRL,
				      GENMASK(27, 16));
		queued = mt76_get_field(dev, base + MT_FL_Q3_CTRL,
					GENMASK(11, 0));

		seq_printf(s, "\t%s: ", map[i].name);
		seq_printf(s, "queued:0x%03x head:0x%03x tail:0x%03x\n",
			   queued, head, tail);
	}
}

static void
mt7915_sta_hw_queue_read(void *data, struct ieee80211_sta *sta)
{
	struct mt7915_sta *msta = (struct mt7915_sta *)sta->drv_priv;
	struct mt7915_dev *dev = msta->vif->phy->dev;
	struct seq_file *s = data;
	u8 ac;

	for (ac = 0; ac < 4; ac++) {
		u32 qlen, ctrl, val;
		u32 idx = msta->wcid.idx >> 5;
		u8 offs = msta->wcid.idx & GENMASK(4, 0);

		ctrl = BIT(31) | BIT(11) | (ac << 24);
		val = mt76_rr(dev, MT_PLE_AC_QEMPTY(ac, idx));

		if (val & BIT(offs))
			continue;

		mt76_wr(dev, MT_PLE_BASE + MT_FL_Q0_CTRL, ctrl | msta->wcid.idx);
		qlen = mt76_get_field(dev, MT_PLE_BASE + MT_FL_Q3_CTRL,
				      GENMASK(11, 0));
		seq_printf(s, "\tSTA %pM wcid %d: AC%d%d queued:%d\n",
			   sta->addr, msta->wcid.idx, msta->vif->wmm_idx,
			   ac, qlen);
	}
}

static int
mt7915_hw_queues_read(struct seq_file *s, void *data)
{
	struct mt7915_dev *dev = dev_get_drvdata(s->private);
	struct mt7915_phy *phy = mt7915_ext_phy(dev);
	static const struct hw_queue_map ple_queue_map[] = {
		{"CPU_Q0",  0,  1, MT_CTX0},
		{"CPU_Q1",  1,  1, MT_CTX0 + 1},
		{"CPU_Q2",  2,  1, MT_CTX0 + 2},
		{"CPU_Q3",  3,  1, MT_CTX0 + 3},
		{"ALTX_Q0", 8,  2, MT_LMAC_ALTX0},
		{"BMC_Q0",  9,  2, MT_LMAC_BMC0},
		{"BCN_Q0",  10, 2, MT_LMAC_BCN0},
		{"PSMP_Q0", 11, 2, MT_LMAC_PSMP0},
		{"ALTX_Q1", 12, 2, MT_LMAC_ALTX0 + 4},
		{"BMC_Q1",  13, 2, MT_LMAC_BMC0 + 4},
		{"BCN_Q1",  14, 2, MT_LMAC_BCN0 + 4},
		{"PSMP_Q1", 15, 2, MT_LMAC_PSMP0 + 4},
	};
	static const struct hw_queue_map pse_queue_map[] = {
		{"CPU Q0",  0,  1, MT_CTX0},
		{"CPU Q1",  1,  1, MT_CTX0 + 1},
		{"CPU Q2",  2,  1, MT_CTX0 + 2},
		{"CPU Q3",  3,  1, MT_CTX0 + 3},
		{"HIF_Q0",  8,  0, MT_HIF0},
		{"HIF_Q1",  9,  0, MT_HIF0 + 1},
		{"HIF_Q2",  10, 0, MT_HIF0 + 2},
		{"HIF_Q3",  11, 0, MT_HIF0 + 3},
		{"HIF_Q4",  12, 0, MT_HIF0 + 4},
		{"HIF_Q5",  13, 0, MT_HIF0 + 5},
		{"LMAC_Q",  16, 2, 0},
		{"MDP_TXQ", 17, 2, 1},
		{"MDP_RXQ", 18, 2, 2},
		{"SEC_TXQ", 19, 2, 3},
		{"SEC_RXQ", 20, 2, 4},
	};
	u32 val, head, tail;

	/* ple queue */
	val = mt76_rr(dev, MT_PLE_FREEPG_CNT);
	head = mt76_get_field(dev, MT_PLE_FREEPG_HEAD_TAIL, GENMASK(11, 0));
	tail = mt76_get_field(dev, MT_PLE_FREEPG_HEAD_TAIL, GENMASK(27, 16));
	seq_puts(s, "PLE page info:\n");
	seq_printf(s, "\tTotal free page: 0x%08x head: 0x%03x tail: 0x%03x\n",
		   val, head, tail);

	val = mt76_rr(dev, MT_PLE_PG_HIF_GROUP);
	head = mt76_get_field(dev, MT_PLE_HIF_PG_INFO, GENMASK(11, 0));
	tail = mt76_get_field(dev, MT_PLE_HIF_PG_INFO, GENMASK(27, 16));
	seq_printf(s, "\tHIF free page: 0x%03x res: 0x%03x used: 0x%03x\n",
		   val, head, tail);

	seq_puts(s, "PLE non-empty queue info:\n");
	mt7915_hw_queue_read(s, MT_PLE_BASE, ARRAY_SIZE(ple_queue_map),
			     &ple_queue_map[0]);

	/* iterate per-sta ple queue */
	ieee80211_iterate_stations_atomic(dev->mphy.hw,
					  mt7915_sta_hw_queue_read, s);
	if (phy)
		ieee80211_iterate_stations_atomic(phy->mt76->hw,
						  mt7915_sta_hw_queue_read, s);

	/* pse queue */
	seq_puts(s, "PSE non-empty queue info:\n");
	mt7915_hw_queue_read(s, MT_PSE_BASE, ARRAY_SIZE(pse_queue_map),
			     &pse_queue_map[0]);

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
		{ dev->mphy.q_tx[MT_TXQ_BE],	 "MAIN" },
		{ ext_q,			 "EXT" },
		{ dev->mt76.q_mcu[MT_MCUQ_WM],	 "MCUWM"  },
		{ dev->mt76.q_mcu[MT_MCUQ_WA],	 "MCUWA"  },
		{ dev->mt76.q_mcu[MT_MCUQ_FWDL], "MCUFWDL" },
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

static int
mt7915_last_mcu_read(struct seq_file *file, void *data)
{
	struct mt7915_dev *dev = dev_get_drvdata(file->private);

	seq_puts(file, "MCU Settings:\n");

#define O_MCU_LE32(o, a)					\
	seq_printf(file,					\
		   "\t%-42s %d\n", #a,				\
		   le32_to_cpu((o)->last_mcu.a))		\

#define O_MCU_LE32X(o, a)					\
	seq_printf(file,					\
		   "\t%-42s 0x%0x\n", #a,			\
		   le32_to_cpu((o)->last_mcu.a))		\

#define O_MCU_LE16(o, a)					\
	seq_printf(file,					\
		   "\t%-42s %d\n", #a,				\
		   le16_to_cpu((o)->last_mcu.a))		\

#define O_MCU_LE16X(o, a)					\
	seq_printf(file,					\
		   "\t%-42s 0x%0x\n", #a,			\
		   le16_to_cpu((o)->last_mcu.a))		\

#define O_MCU_MAC(o, a)						\
	seq_printf(file,					\
		   "\t%-42s %pM\n", #a,				\
		   (o)->last_mcu.a)				\

#define O_MCU_U8(o, a)						\
	seq_printf(file,					\
		   "\t%-42s %d\n", #a,				\
		   (unsigned int)((o)->last_mcu.a))		\

#define O_MCU_U8X(o, a)						\
	seq_printf(file,					\
		   "\t%-42s 0x%x\n", #a,			\
		   (unsigned int)((o)->last_mcu.a))		\

#define O_MCU_U16(o, a)						\
	seq_printf(file,					\
		   "\t%-42s %d\n", #a,				\
		   (unsigned int)((o)->last_mcu.a))		\

#define O_MCU_S16(o, a)						\
	seq_printf(file,					\
		   "\t%-42s %d\n", #a,				\
		   (unsigned int)((o)->last_mcu.a))		\

/* dev variants */
#define D_MCU_LE32X(a)	O_MCU_LE32X(dev, a)
#define D_MCU_LE32(a)	O_MCU_LE32(dev, a)
#define D_MCU_LE16X(a)	O_MCU_LE16X(dev, a)
#define D_MCU_LE16(a)	O_MCU_LE16(dev, a)
#define D_MCU_MAC(a)	O_MCU_MAC(dev, a)
#define D_MCU_U8(a)	O_MCU_U8(dev, a)
#define D_MCU_U8X(a)	O_MCU_U8X(dev, a)
#define D_MCU_U16(a)	O_MCU_U16(dev, a)
#define D_MCU_S16(a)	O_MCU_S16(dev, a)

	D_MCU_U8(mcu_chan_info.control_ch);
	D_MCU_U8(mcu_chan_info.center_ch);
	D_MCU_U8(mcu_chan_info.bw);
	D_MCU_U8(mcu_chan_info.tx_streams_num);
	D_MCU_U8(mcu_chan_info.rx_streams);
	D_MCU_U8(mcu_chan_info.switch_reason);
	D_MCU_U8(mcu_chan_info.band_idx);
	D_MCU_U8(mcu_chan_info.center_ch2);
	D_MCU_LE16(mcu_chan_info.cac_case);
	D_MCU_U8(mcu_chan_info.channel_band);
	D_MCU_LE32(mcu_chan_info.outband_freq);
	D_MCU_U8(mcu_chan_info.txpower_drop);
	D_MCU_U8(mcu_chan_info.ap_bw);
	D_MCU_U8(mcu_chan_info.ap_center_ch);

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

	seq_puts(s, "Per-chain txpower in 1/2 db units.\n");
	seq_printf(s, "\nPhy %d\n", phy != &phy->dev->phy);

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

	debugfs_create_devm_seqfile(dev->mt76.dev, "tx-queues", dir,
				    mt7915_queues_read);
	debugfs_create_devm_seqfile(dev->mt76.dev, "last_mcu", dir,
				    mt7915_last_mcu_read);
	debugfs_create_devm_seqfile(dev->mt76.dev, "hw-queues", dir,
				    mt7915_hw_queues_read);
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

static int mt7915_sta_fixed_rate_set(void *data, u64 rate)
{
	struct ieee80211_sta *sta = data;
	struct mt7915_sta *msta = (struct mt7915_sta *)sta->drv_priv;

	/* usage: <he ltf> <tx mode> <ldpc> <stbc> <bw> <gi> <nss> <mcs>
	 * <tx mode>: see enum mt76_phy_type
	 */
	return mt7915_mcu_set_fixed_rate(msta->vif->phy->dev, sta, rate);
}

DEFINE_DEBUGFS_ATTRIBUTE(fops_fixed_rate, NULL,
			 mt7915_sta_fixed_rate_set, "%llx\n");

static int
mt7915_mcu_settings_show(struct seq_file *file, void *data)
{
	struct ieee80211_sta *sta = file->private;
	struct mt7915_sta *msta = (struct mt7915_sta *)sta->drv_priv;
	struct mt7915_vif *mvif = msta->vif;
	int i;

	/* Last-applied settings to MCU */
	seq_puts(file, "MCU Settings:\n");
	seq_puts(file, " VIF Settings:\n");

#define MCU_LE32X(a)	O_MCU_LE32X(mvif, a)
#define MCU_LE32(a)	O_MCU_LE32(mvif, a)
#define MCU_LE16X(a)	O_MCU_LE16X(mvif, a)
#define MCU_LE16(a)	O_MCU_LE16(mvif, a)
#define MCU_MAC(a)	O_MCU_MAC(mvif, a)
#define MCU_U8(a)	O_MCU_U8(mvif, a)
#define MCU_U8X(a)	O_MCU_U8X(mvif, a)
#define MCU_U16(a)	O_MCU_U16(mvif, a)
#define MCU_S16(a)	O_MCU_S16(mvif, a)

/* mSTA variants */
#define S_MCU_LE32X(a)	O_MCU_LE32X(msta, a)
#define S_MCU_LE32(a)	O_MCU_LE32(msta, a)
#define S_MCU_LE16X(a)	O_MCU_LE16X(msta, a)
#define S_MCU_LE16(a)	O_MCU_LE16(msta, a)
#define S_MCU_MAC(a)	O_MCU_MAC(msta, a)
#define S_MCU_U8(a)	O_MCU_U8(msta, a)
#define S_MCU_U8X(a)	O_MCU_U8X(msta, a)
#define S_MCU_U16(a)	O_MCU_U16(msta, a)
#define S_MCU_S16(a)	O_MCU_S16(msta, a)

	MCU_LE32(bss_info_basic.network_type);
	MCU_U8(bss_info_basic.active);
	MCU_U8(bss_info_basic.rsv0);
	MCU_LE16(bss_info_basic.bcn_interval);
	MCU_MAC(bss_info_basic.bssid);
	MCU_U8(bss_info_basic.wmm_idx);
	MCU_U8(bss_info_basic.dtim_period);
	MCU_U8(bss_info_basic.bmc_wcid_lo);
	MCU_U8(bss_info_basic.cipher);
	MCU_U8(bss_info_basic.phy_mode);
	MCU_U8(bss_info_basic.max_bssid);
	MCU_U8(bss_info_basic.non_tx_bssid);
	MCU_U8(bss_info_basic.bmc_wcid_hi);

	seq_puts(file, "\n");
	MCU_U8(bss_info_omac.hw_bss_idx);
	MCU_U8(bss_info_omac.omac_idx);
	MCU_U8(bss_info_omac.band_idx);

	seq_puts(file, "\n");
	MCU_U8(bss_info_rf_ch.pri_ch);
	MCU_U8(bss_info_rf_ch.center_ch0);
	MCU_U8(bss_info_rf_ch.center_ch1);
	MCU_U8(bss_info_rf_ch.bw);
	MCU_U8(bss_info_rf_ch.he_ru26_block);
	MCU_U8(bss_info_rf_ch.he_all_disable);

	seq_puts(file, "\n");
	MCU_U8(bss_info_ra.op_mode);
	MCU_U8(bss_info_ra.adhoc_en);
	MCU_U8(bss_info_ra.short_preamble);
	MCU_U8(bss_info_ra.tx_streams);
	MCU_U8(bss_info_ra.rx_streams);
	MCU_U8(bss_info_ra.algo);
	MCU_U8(bss_info_ra.force_sgi);
	MCU_U8(bss_info_ra.force_gf);
	MCU_U8(bss_info_ra.ht_mode);
	MCU_U8(bss_info_ra.has_20_sta);
	MCU_U8(bss_info_ra.bss_width_trigger_events);
	MCU_U8(bss_info_ra.vht_nss_cap);
	/* these are not used, evidently */
	/* MCU_U8(bss_info_ra.vht_bw_signal); */
	/* MCU_U8(bss_info_ra.vht_force_sgi); */
	MCU_U8(bss_info_ra.se_off);
	MCU_U8(bss_info_ra.antenna_idx);
	MCU_U8(bss_info_ra.train_up_rule);
	MCU_U16(bss_info_ra.train_up_high_thres);
	MCU_S16(bss_info_ra.train_up_rule_rssi);
	MCU_U16(bss_info_ra.low_traffic_thres);
	MCU_LE16(bss_info_ra.max_phyrate);
	MCU_LE32(bss_info_ra.phy_cap);
	MCU_LE32(bss_info_ra.interval);
	MCU_LE32(bss_info_ra.fast_interval);

	seq_puts(file, "\n");
	MCU_U8(bss_info_he.he_pe_duration);
	MCU_U8(bss_info_he.vht_op_info_present);
	MCU_LE16(bss_info_he.he_rts_thres);
	MCU_LE16(bss_info_he.max_nss_mcs[CMD_HE_MCS_BW80]);
	MCU_LE16(bss_info_he.max_nss_mcs[CMD_HE_MCS_BW160]);
	MCU_LE16(bss_info_he.max_nss_mcs[CMD_HE_MCS_BW8080]);

	seq_puts(file, "\n");
	MCU_LE32(bss_info_hw_amsdu.cmp_bitmap_0);
	MCU_LE32(bss_info_hw_amsdu.cmp_bitmap_1);
	MCU_LE16(bss_info_hw_amsdu.trig_thres);
	MCU_U8(bss_info_hw_amsdu.enable);

	seq_puts(file, "\n");
	MCU_LE32(bss_info_ext_bss.mbss_tsf_offset);

	seq_puts(file, "\n");
	MCU_LE16(bss_info_bmc_rate.bc_trans);
	MCU_LE16(bss_info_bmc_rate.mc_trans);
	MCU_U8(bss_info_bmc_rate.short_preamble);

	seq_puts(file, "\n STA Settings:\n");

	S_MCU_U8(sta_rec_ba.tid);
	S_MCU_U8(sta_rec_ba.ba_type);
	S_MCU_U8(sta_rec_ba.amsdu);
	S_MCU_U8(sta_rec_ba.ba_en);
	S_MCU_LE16(sta_rec_ba.ssn);
	S_MCU_LE16(sta_rec_ba.winsize);

	seq_puts(file, "\n");
	S_MCU_U8(wtbl_ba.tid);
	S_MCU_U8(wtbl_ba.ba_type);
	S_MCU_LE16(wtbl_ba.sn);
	S_MCU_U8(wtbl_ba.ba_en);
	S_MCU_U8(wtbl_ba.ba_winsize_idx);
	S_MCU_LE16(wtbl_ba.ba_winsize);
	S_MCU_MAC(wtbl_ba.peer_addr);
	S_MCU_U8(wtbl_ba.rst_ba_tid);
	S_MCU_U8(wtbl_ba.rst_ba_sel);
	S_MCU_U8(wtbl_ba.rst_ba_sb);
	S_MCU_U8(wtbl_ba.band_idx);

	seq_puts(file, "\n");
	S_MCU_U8(wtbl_rx.rcid);
	S_MCU_U8(wtbl_rx.rca1);
	S_MCU_U8(wtbl_rx.rca2);
	S_MCU_U8(wtbl_rx.rv);

	seq_puts(file, "\n");
	S_MCU_LE32(sta_rec_basic.conn_type);
	S_MCU_U8(sta_rec_basic.conn_state);
	S_MCU_U8(sta_rec_basic.qos);
	S_MCU_LE16(sta_rec_basic.aid);
	S_MCU_MAC(sta_rec_basic.peer_addr);
	S_MCU_LE16(sta_rec_basic.extra_info);

	seq_puts(file, "\n");
	S_MCU_LE32X(sta_rec_he.he_cap);
	S_MCU_U8(sta_rec_he.t_frame_dur);
	S_MCU_U8(sta_rec_he.max_ampdu_exp);
	S_MCU_U8(sta_rec_he.bw_set);
	S_MCU_U8(sta_rec_he.device_class);
	S_MCU_U8(sta_rec_he.dcm_tx_mode);
	S_MCU_U8(sta_rec_he.dcm_tx_max_nss);
	S_MCU_U8(sta_rec_he.dcm_rx_mode);
	S_MCU_U8(sta_rec_he.dcm_rx_max_nss);
	S_MCU_U8(sta_rec_he.dcm_max_ru);
	S_MCU_U8(sta_rec_he.punc_pream_rx);
	S_MCU_U8(sta_rec_he.pkt_ext);
	S_MCU_LE16(sta_rec_he.max_nss_mcs[CMD_HE_MCS_BW80]);
	S_MCU_LE16(sta_rec_he.max_nss_mcs[CMD_HE_MCS_BW160]);
	S_MCU_LE16(sta_rec_he.max_nss_mcs[CMD_HE_MCS_BW8080]);

	seq_puts(file, "\n");
	S_MCU_U8X(sta_rec_uapsd.dac_map);
	S_MCU_U8X(sta_rec_uapsd.tac_map);
	S_MCU_U8(sta_rec_uapsd.max_sp);
	S_MCU_LE16(sta_rec_uapsd.listen_interval);

	seq_puts(file, "\n");
	S_MCU_U8(sta_rec_muru.cfg.ofdma_dl_en);
	S_MCU_U8(sta_rec_muru.cfg.ofdma_ul_en);
	S_MCU_U8(sta_rec_muru.cfg.mimo_dl_en);
	S_MCU_U8(sta_rec_muru.cfg.mimo_ul_en);
	S_MCU_U8(sta_rec_muru.ofdma_dl.punc_pream_rx);
	S_MCU_U8(sta_rec_muru.ofdma_dl.he_20m_in_40m_2g);
	S_MCU_U8(sta_rec_muru.ofdma_dl.he_20m_in_160m);
	S_MCU_U8(sta_rec_muru.ofdma_dl.he_80m_in_160m);
	S_MCU_U8(sta_rec_muru.ofdma_dl.lt16_sigb);
	S_MCU_U8(sta_rec_muru.ofdma_dl.rx_su_comp_sigb);
	S_MCU_U8(sta_rec_muru.ofdma_dl.rx_su_non_comp_sigb);
	S_MCU_U8(sta_rec_muru.ofdma_ul.t_frame_dur);
	S_MCU_U8(sta_rec_muru.ofdma_ul.mu_cascading);
	S_MCU_U8(sta_rec_muru.ofdma_ul.uo_ra);
	S_MCU_U8(sta_rec_muru.ofdma_ul.he_2x996_tone);
	S_MCU_U8(sta_rec_muru.ofdma_ul.rx_t_frame_11ac);
	S_MCU_U8(sta_rec_muru.mimo_dl.vht_mu_bfee);
	S_MCU_U8(sta_rec_muru.mimo_dl.partial_bw_dl_mimo);
	S_MCU_U8(sta_rec_muru.mimo_ul.full_ul_mimo);
	S_MCU_U8(sta_rec_muru.mimo_ul.partial_ul_mimo);

	seq_puts(file, "\n");
	S_MCU_LE32X(sta_rec_vht.vht_cap);
	S_MCU_LE16X(sta_rec_vht.vht_rx_mcs_map);
	S_MCU_LE16X(sta_rec_vht.vht_tx_mcs_map);

	seq_puts(file, "\n");
	S_MCU_U8(sta_rec_amsdu.max_amsdu_num);
	S_MCU_U8(sta_rec_amsdu.max_mpdu_size);
	S_MCU_U8(sta_rec_amsdu.amsdu_en);

	seq_puts(file, "\n");
	S_MCU_LE16X(sta_rec_ht.ht_cap);

	seq_puts(file, "\n");
	S_MCU_U8(wtbl_smps.smps);

	seq_puts(file, "\n");
	S_MCU_U8(wtbl_ht.ht);
	S_MCU_U8(wtbl_ht.ldpc);
	S_MCU_U8(wtbl_ht.af);
	S_MCU_U8(wtbl_ht.mm);

	seq_puts(file, "\n");
	S_MCU_U8(wtbl_vht.ldpc);
	S_MCU_U8(wtbl_vht.dyn_bw);
	S_MCU_U8(wtbl_vht.vht);
	S_MCU_U8(wtbl_vht.txop_ps);

	seq_puts(file, "\n");
	S_MCU_U8(wtbl_hdr_trans.to_ds);
	S_MCU_U8(wtbl_hdr_trans.from_ds);
	S_MCU_U8(wtbl_hdr_trans.no_rx_trans);

	seq_puts(file, "\n");
	S_MCU_LE16X(sta_rec_bf.pfmu);
	S_MCU_U8(sta_rec_bf.su_mu);
	S_MCU_U8(sta_rec_bf.bf_cap);
	S_MCU_U8(sta_rec_bf.sounding_phy);
	S_MCU_U8(sta_rec_bf.ndpa_rate);
	S_MCU_U8(sta_rec_bf.ndp_rate);
	S_MCU_U8(sta_rec_bf.rept_poll_rate);
	S_MCU_U8(sta_rec_bf.tx_mode);
	S_MCU_U8(sta_rec_bf.bf_cap);
	S_MCU_U8(sta_rec_bf.ncol);
	S_MCU_U8(sta_rec_bf.nrow);
	S_MCU_U8(sta_rec_bf.bw);
	S_MCU_U8(sta_rec_bf.mem_total);
	S_MCU_U8(sta_rec_bf.mem_20m);
	for (i = 0; i < 4; i++) {
		S_MCU_U8(sta_rec_bf.mem[i].row);
		S_MCU_U8(sta_rec_bf.mem[i].col);
		S_MCU_U8(sta_rec_bf.mem[i].row_msb);
	}
	S_MCU_LE16X(sta_rec_bf.smart_ant);
	S_MCU_U8(sta_rec_bf.se_idx);
	S_MCU_U8X(sta_rec_bf.auto_sounding);
	S_MCU_U8(sta_rec_bf.ibf_timeout);
	S_MCU_U8(sta_rec_bf.ibf_dbw);
	S_MCU_U8(sta_rec_bf.ibf_ncol);
	S_MCU_U8(sta_rec_bf.ibf_nrow);
	S_MCU_U8(sta_rec_bf.nrow_bw160);
	S_MCU_U8(sta_rec_bf.ncol_bw160);
	S_MCU_U8(sta_rec_bf.ru_start_idx);
	S_MCU_U8(sta_rec_bf.ru_end_idx);
	S_MCU_U8(sta_rec_bf.trigger_su);
	S_MCU_U8(sta_rec_bf.trigger_mu);
	S_MCU_U8(sta_rec_bf.ng16_su);
	S_MCU_U8(sta_rec_bf.ng16_mu);
	S_MCU_U8(sta_rec_bf.codebook42_su);
	S_MCU_U8(sta_rec_bf.codebook75_mu);
	S_MCU_U8(sta_rec_bf.he_ltf);

	seq_puts(file, "\n");
	S_MCU_U8(sta_rec_bfee.fb_identity_matrix);
	S_MCU_U8(sta_rec_bfee.ignore_feedback);

	seq_puts(file, "\n");
	S_MCU_U8(sta_rec_ra.valid);
	S_MCU_U8(sta_rec_ra.auto_rate);
	S_MCU_U8(sta_rec_ra.phy_mode);
	S_MCU_U8(sta_rec_ra.channel);
	S_MCU_U8(sta_rec_ra.bw);
	S_MCU_U8(sta_rec_ra.disable_cck);
	S_MCU_U8(sta_rec_ra.ht_mcs32);
	S_MCU_U8(sta_rec_ra.ht_gf);
	S_MCU_U8(sta_rec_ra.ht_mcs[0]);
	S_MCU_U8(sta_rec_ra.ht_mcs[1]);
	S_MCU_U8(sta_rec_ra.ht_mcs[2]);
	S_MCU_U8(sta_rec_ra.ht_mcs[3]);
	S_MCU_U8(sta_rec_ra.mmps_mode);
	S_MCU_U8(sta_rec_ra.gband_256);
	S_MCU_U8(sta_rec_ra.af);
	S_MCU_U8(sta_rec_ra.auth_wapi_mode);
	S_MCU_U8(sta_rec_ra.rate_len);
	S_MCU_U8(sta_rec_ra.supp_mode);
	S_MCU_U8(sta_rec_ra.supp_cck_rate);
	S_MCU_U8(sta_rec_ra.supp_ofdm_rate);
	S_MCU_LE32X(sta_rec_ra.supp_ht_mcs);
	S_MCU_LE16X(sta_rec_ra.supp_vht_mcs[0]);
	S_MCU_LE16X(sta_rec_ra.supp_vht_mcs[1]);
	S_MCU_LE16X(sta_rec_ra.supp_vht_mcs[2]);
	S_MCU_LE16X(sta_rec_ra.supp_vht_mcs[3]);
	S_MCU_U8(sta_rec_ra.op_mode);
	S_MCU_U8(sta_rec_ra.op_vht_chan_width);
	S_MCU_U8(sta_rec_ra.op_vht_rx_nss);
	S_MCU_U8(sta_rec_ra.op_vht_rx_nss_type);
	S_MCU_LE32X(sta_rec_ra.sta_cap);
	S_MCU_U8(sta_rec_ra.phy.type);
	S_MCU_U8X(sta_rec_ra.phy.flag);
	S_MCU_U8(sta_rec_ra.phy.stbc);
	S_MCU_U8(sta_rec_ra.phy.sgi);
	S_MCU_U8(sta_rec_ra.phy.bw);
	S_MCU_U8(sta_rec_ra.phy.ldpc);
	S_MCU_U8(sta_rec_ra.phy.mcs);
	S_MCU_U8(sta_rec_ra.phy.nss);
	S_MCU_U8(sta_rec_ra.phy.he_ltf);

	seq_puts(file, "\n");
	S_MCU_LE32X(sta_rec_ra_fixed.field);
	S_MCU_U8(sta_rec_ra_fixed.op_mode);
	S_MCU_U8(sta_rec_ra_fixed.op_vht_chan_width);
	S_MCU_U8(sta_rec_ra_fixed.op_vht_rx_nss);
	S_MCU_U8(sta_rec_ra_fixed.op_vht_rx_nss_type);
	S_MCU_U8(sta_rec_ra_fixed.phy.type);
	S_MCU_U8X(sta_rec_ra_fixed.phy.flag);
	S_MCU_U8(sta_rec_ra_fixed.phy.stbc);
	S_MCU_U8(sta_rec_ra_fixed.phy.sgi);
	S_MCU_U8(sta_rec_ra_fixed.phy.bw);
	S_MCU_U8(sta_rec_ra_fixed.phy.ldpc);
	S_MCU_U8(sta_rec_ra_fixed.phy.mcs);
	S_MCU_U8(sta_rec_ra_fixed.phy.nss);
	S_MCU_U8(sta_rec_ra_fixed.phy.he_ltf);

	S_MCU_U8(sta_rec_ra_fixed.spe_en);
	S_MCU_U8(sta_rec_ra_fixed.short_preamble);
	S_MCU_U8(sta_rec_ra_fixed.is_5g);
	S_MCU_U8(sta_rec_ra_fixed.mmps_mode);

	return 0;
}

DEFINE_SHOW_ATTRIBUTE(mt7915_mcu_settings);

static int
mt7915_queues_show(struct seq_file *s, void *data)
{
	struct ieee80211_sta *sta = s->private;

	mt7915_sta_hw_queue_read(s, sta);

	return 0;
}

DEFINE_SHOW_ATTRIBUTE(mt7915_queues);

void mt7915_sta_add_debugfs(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
			    struct ieee80211_sta *sta, struct dentry *dir)
{
	debugfs_create_file("fixed_rate", 0600, dir, sta, &fops_fixed_rate);
	debugfs_create_file("mcu_settings", 0400, dir, sta, &mt7915_mcu_settings_fops);
	debugfs_create_file("hw-queues", 0400, dir, sta, &mt7915_queues_fops);
}

#endif
