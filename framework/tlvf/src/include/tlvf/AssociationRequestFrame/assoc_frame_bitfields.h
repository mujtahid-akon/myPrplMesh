
#ifndef _ASSOCIATION_FRAME_STRUCTS_
#define _ASSOCIATION_FRAME_STRUCTS_

#include <asm/byteorder.h>
#include <cstddef>
#include <memory>

namespace assoc_frame {

constexpr uint8_t MAC_ADDR_LEN = 6;

typedef struct sStaHtCapabilityInfo {
#if defined(__LITTLE_ENDIAN_BITFIELD)
    uint16_t ldcp_coding_capability : 1;
    uint16_t support_ch_width_set : 1;
    uint16_t sm_power_save : 2;
    uint16_t ht_greenfield : 1;
    uint16_t short_gi20mhz : 1;
    uint16_t short_gi40mhz : 1;
    uint16_t tx_stbc : 1;
    uint16_t rx_stbc : 2;
    uint16_t ht_delayed_block_ack : 1;
    uint16_t max_a_msdu_length : 1;
    uint16_t dsss_cck_mode40mhz : 1;
    uint16_t reserved : 1;
    uint16_t forty_mhz_intolerant : 1;
    uint16_t l_sig_txop_protection_support : 1;
#elif defined(__BIG_ENDIAN_BITFIELD)
    uint16_t l_sig_txop_protection_support : 1;
    uint16_t forty_mhz_intolerant : 1;
    uint16_t reserved : 1;
    uint16_t dsss_cck_mode40mhz : 1;
    uint16_t max_a_msdu_length : 1;
    uint16_t ht_delayed_block_ack : 1;
    uint16_t rx_stbc : 2;
    uint16_t tx_stbc : 1;
    uint16_t short_gi40mhz : 1;
    uint16_t short_gi20mhz : 1;
    uint16_t ht_greenfield : 1;
    uint16_t sm_power_save : 2;
    uint16_t support_ch_width_set : 1;
    uint16_t ldcp_coding_capability : 1;
#else
#error "Bitfield macros are not defined"
#endif
    void struct_swap() {}
    void struct_init() {}
} __attribute__((packed)) sStaHtCapabilityInfo;

typedef struct sStaVhtCapInfo {
#if defined(__LITTLE_ENDIAN_BITFIELD)
    uint32_t max_mpdu_len : 2;
    uint32_t support_ch_width_set : 2;
    uint32_t rx_ldpc : 1;
    uint32_t short_gi80mhz_tvht_mode4c : 1;
    uint32_t short_gi160mhz80_80mhz : 1;
    uint32_t tx_stbc : 1;
    uint32_t rx_stbc : 3;
    uint32_t su_beamformer : 1;           // SU beamformer capable
    uint32_t su_beamformee : 1;           // SU beamformee capable
    uint32_t beamformee_sts : 3;          // Beamformee STS capability
    uint32_t sound_dimensions : 3;        // Number of sounding dimensions
    uint32_t mu_beamformer : 1;           // MU beamformer capable
    uint32_t mu_beamformee : 1;           // MU beamformee capable
    uint32_t txop_ps : 1;                 // TXOP PS
    uint32_t htc_vht : 1;                 // +HTC VHT capable
    uint32_t max_a_mpdu_len : 3;          // Maximum A-MPDU length exponent
    uint32_t vht_link_adaptation : 2;     // VHT Link adaptation capable
    uint32_t rx_antenna_pattern : 1;      // rx antenna pattern consistency
    uint32_t tx_antenna_pattern : 1;      // tx antenna pattern consistency
    uint32_t extended_nss_bw_support : 2; // Extended NSS BW Support
#elif defined(__BIG_ENDIAN_BITFIELD)
    uint32_t extended_nss_bw_support : 2;
    uint32_t tx_antenna_pattern : 1;
    uint32_t rx_antenna_pattern : 1;
    uint32_t vht_link_adaptation : 2;
    uint32_t max_a_mpdu_len : 3;
    uint32_t htc_vht : 1;
    uint32_t txop_ps : 1;
    uint32_t mu_beamformee : 1;
    uint32_t mu_beamformer : 1;
    uint32_t sound_dimensions : 3;
    uint32_t beamformee_sts : 3;
    uint32_t su_beamformee : 1;
    uint32_t su_beamformer : 1;
    uint32_t rx_stbc : 3;
    uint32_t tx_stbc : 1;
    uint32_t short_gi160mhz80_80mhz : 1;
    uint32_t short_gi80mhz_tvht_mode4c : 1;
    uint32_t rx_ldpc : 1;
    uint32_t support_ch_width_set : 2;
    uint32_t max_mpdu_len : 2;
#endif
    void struct_swap() {}
    void struct_init() {}
} __attribute__((packed)) sStaVhtCapInfo;

typedef struct sSupportedVhtMcsSet {
#if defined(__LITTLE_ENDIAN_BITFIELD)
    uint64_t rx_mcs_map : 16;
    uint64_t rx_highest_supp_long_gi_data_rate : 13;
    uint64_t max_nsts_total : 3;
    uint64_t tx_mcs_map : 16;
    uint64_t tx_highest_supp_long_gi_data_rate : 13;
    uint64_t extended_nss_bw_capable : 1;
    uint64_t reserved : 2;
#elif defined(__BIG_ENDIAN_BITFIELD)
    uint64_t reserved : 2;
    uint64_t extended_nss_bw_capable : 1;
    uint64_t tx_highest_supp_long_gi_data_rate : 13;
    uint64_t tx_mcs_map : 16;
    uint64_t max_nsts_total : 3;
    uint64_t rx_highest_supp_long_gi_data_rate : 13;
    uint64_t rx_mcs_map : 16;
#endif
    void struct_swap() {}
    void struct_init() {}
} __attribute__((packed)) sSupportedVhtMcsSet;

typedef struct sStaHeMacCapInfo1 {
#if defined(__LITTLE_ENDIAN_BITFIELD)
    uint32_t htc_he_support : 1;
    uint32_t twt_req_support : 1;
    uint32_t twt_resp_support : 1;
    uint32_t dynamic_frag_support : 2;
    uint32_t max_num_frag_msdus : 3;
    uint32_t min_frag_size : 2;
    uint32_t trigger_frame_mac_pad_dur : 2;
    uint32_t multi_tid_aggr_rx_support : 3;
    uint32_t he_link_adapt_support : 2;
    uint32_t all_ack_support : 1;
    uint32_t trs_support : 1;
    uint32_t bsr_support : 1;
    uint32_t broadcast_twt_support : 1;
    uint32_t ba_bitmap_32bit_support : 1;
    uint32_t mu_cascading_support : 1;
    uint32_t ack_enabled_aggr_support : 1;
    uint32_t reserved : 1;
    uint32_t om_control_support : 1;
    uint32_t ofdma_ra_support : 1;
    uint32_t max_ampdu_length_exp_ext : 2;
    uint32_t amsdu_frag_support : 1;
    uint32_t flex_twt_sched_support : 1;
    uint32_t rx_control_frame_multi_bss : 1;
#elif defined(__BIG_ENDIAN_BITFIELD)
    uint32_t rx_control_frame_multi_bss : 1;
    uint32_t flex_twt_sched_support : 1;
    uint32_t amsdu_frag_support : 1;
    uint32_t max_ampdu_length_exp_ext : 2;
    uint32_t ofdma_ra_support : 1;
    uint32_t om_control_support : 1;
    uint32_t reserved : 1;
    uint32_t ack_enabled_aggr_support : 1;
    uint32_t mu_cascading_support : 1;
    uint32_t ba_bitmap_32bit_support : 1;
    uint32_t broadcast_twt_support : 1;
    uint32_t bsr_support : 1;
    uint32_t trs_support : 1;
    uint32_t all_ack_support : 1;
    uint32_t he_link_adapt_support : 2;
    uint32_t multi_tid_aggr_rx_support : 3;
    uint32_t trigger_frame_mac_pad_dur : 2;
    uint32_t min_frag_size : 2;
    uint32_t max_num_frag_msdus : 3;
    uint32_t dynamic_frag_support : 2;
    uint32_t twt_resp_support : 1;
    uint32_t twt_req_support : 1;
    uint32_t htc_he_support : 1;
#endif
    void struct_swap() {}
    void struct_init() {}
} __attribute__((packed)) sStaHeMacCapInfo1;

typedef struct sStaHeMacCapInfo2 {
#if defined(__LITTLE_ENDIAN_BITFIELD)
    uint16_t bsrp_bqrp_ampdu_aggr : 1;
    uint16_t qtp_support : 1;
    uint16_t bqr_support : 1;
    uint16_t psr_responder : 1;
    uint16_t ndp_feedback_rep_support : 1;
    uint16_t ops_support : 1;
    uint16_t amsdu_not_ba_ack_en_ampdu_support : 1;
    uint16_t multi_tid_aggr_tx_support : 3;
    uint16_t he_sub_sel_trans_support : 1;
    uint16_t ul_double_966tone_ru_support : 1;
    uint16_t om_control_ul_mu_data_dis_support : 1;
    uint16_t he_dyn_sm_power_support : 1;
    uint16_t punctured_sounding_support : 1;
    uint16_t ht_vht_trig_frame_rx_support : 1;
#elif defined(__BIG_ENDIAN_BITFIELD)
    uint16_t ht_vht_trig_frame_rx_support : 1;
    uint16_t punctured_sounding_support : 1;
    uint16_t he_dyn_sm_power_support : 1;
    uint16_t om_control_ul_mu_data_dis_support : 1;
    uint16_t ul_double_966tone_ru_support : 1;
    uint16_t he_sub_sel_trans_support : 1;
    uint16_t multi_tid_aggr_tx_support : 3;
    uint16_t amsdu_not_ba_ack_en_ampdu_support : 1;
    uint16_t ops_support : 1;
    uint16_t ndp_feedback_rep_support : 1;
    uint16_t psr_responder : 1;
    uint16_t bqr_support : 1;
    uint16_t qtp_support : 1;
    uint16_t bsrp_bqrp_ampdu_aggr : 1;
#endif
    void struct_swap() {}
    void struct_init() {}
} __attribute__((packed)) sStaHeMacCapInfo2;

typedef struct sStaHePhyCapInfo1 {
#if defined(__LITTLE_ENDIAN_BITFIELD)
    uint64_t punctured_preamble_rx : 4;
    uint64_t device_class : 1;
    uint64_t ldpc_coding_payload : 1;
    uint64_t su_ppdu_1x_ltf_0d8us_gi : 1;
    uint64_t mid_tx_rx_max_nsts : 2;
    uint64_t ndp_4x_ltf_3d2us_gi : 1;
    uint64_t stbc_tx_less_80mhz : 1;
    uint64_t stbc_rx_less_80mhz : 1;
    uint64_t doppler_tx : 1;
    uint64_t doppler_rx : 1;
    uint64_t full_band_ul_mu_mimo : 1;
    uint64_t part_band_ul_mu_mimo : 1;
    uint64_t dcm_max_const_tx : 2;
    uint64_t dcm_max_nss_tx : 1;
    uint64_t dcm_max_const_rx : 2;
    uint64_t dcm_max_nss_rx : 1;
    uint64_t rx_part_bw_su_20mhz_mu_ppdu : 1;
    uint64_t su_beamformer : 1;
    uint64_t su_beamformee : 1;
    uint64_t mu_beamformer : 1;
    uint64_t beamformee_sts_less_80mhz : 3;
    uint64_t beamformee_sts_great_80mhz : 3;
    uint64_t num_sound_dim_less_80mhz : 3;
    uint64_t num_sound_dim_great_80mhz : 3;
    uint64_t ng_16_su_fb : 1;
    uint64_t ng_16_mu_fb : 1;
    uint64_t codebook_f_ps_4_2_su_fb : 1;
    uint64_t codebook_f_ps_7_5_su_fb : 1;
    uint64_t trig_su_beamform_fb : 1;
    uint64_t trig_mu_beamform_part_bw_fb : 1;
    uint64_t trig_cqi_fb : 1;
    uint64_t part_band_ext_range : 1;
    uint64_t part_band_dl_mu_mimo : 1;
    uint64_t ppe_thresholds_pres : 1;
    uint64_t psr_based_sr_support : 1;
    uint64_t power_boost_fac_support : 1;
    uint64_t su_mu_ppdu_4x_ltf_0d8us_gi : 1;
    uint64_t max_nc : 3;
    uint64_t stbc_tx_great_80mhz : 1;
    uint64_t stbc_rx_great_80mhz : 1;
    uint64_t er_su_ppdu_4x_ltf_0d8us_gi : 1;
    uint64_t width_20_in_40mhz_ppdu_2d4_band : 1;
    uint64_t width_20_in_160_80mhz_paired_ppdu : 1;
    uint64_t width_80_in_160_80mhz_paired_ppdu : 1;
    uint64_t er_su_ppdu_1x_ltf_0d8us_gi : 1;
    uint64_t mid_tx_rx_2x_ltf_1x : 1;
    uint64_t dcm_max_ru : 2;
#elif defined(__BIG_ENDIAN_BITFIELD)
    uint64_t dcm_max_ru : 2;
    uint64_t mid_tx_rx_2x_ltf_1x : 1;
    uint64_t er_su_ppdu_1x_ltf_0d8us_gi : 1;
    uint64_t width_80_in_160_80mhz_paired_ppdu : 1;
    uint64_t width_20_in_160_80mhz_paired_ppdu : 1;
    uint64_t width_20_in_40mhz_ppdu_2d4_band : 1;
    uint64_t er_su_ppdu_4x_ltf_0d8us_gi : 1;
    uint64_t stbc_rx_great_80mhz : 1;
    uint64_t stbc_tx_great_80mhz : 1;
    uint64_t max_nc : 3;
    uint64_t su_mu_ppdu_4x_ltf_0d8us_gi : 1;
    uint64_t power_boost_fac_support : 1;
    uint64_t psr_based_sr_support : 1;
    uint64_t ppe_thresholds_pres : 1;
    uint64_t part_band_dl_mu_mimo : 1;
    uint64_t part_band_ext_range : 1;
    uint64_t trig_cqi_fb : 1;
    uint64_t trig_mu_beamform_part_bw_fb : 1;
    uint64_t trig_su_beamform_fb : 1;
    uint64_t codebook_f_ps_7_5_su_fb : 1;
    uint64_t codebook_f_ps_4_2_su_fb : 1;
    uint64_t ng_16_mu_fb : 1;
    uint64_t ng_16_su_fb : 1;
    uint64_t num_sound_dim_great_80mhz : 3;
    uint64_t num_sound_dim_less_80mhz : 3;
    uint64_t beamformee_sts_great_80mhz : 3;
    uint64_t beamformee_sts_less_80mhz : 3;
    uint64_t mu_beamformer : 1;
    uint64_t su_beamformee : 1;
    uint64_t su_beamformer : 1;
    uint64_t rx_part_bw_su_20mhz_mu_ppdu : 1;
    uint64_t dcm_max_nss_rx : 1;
    uint64_t dcm_max_const_rx : 2;
    uint64_t dcm_max_nss_tx : 1;
    uint64_t dcm_max_const_tx : 2;
    uint64_t part_band_ul_mu_mimo : 1;
    uint64_t full_band_ul_mu_mimo : 1;
    uint64_t doppler_rx : 1;
    uint64_t doppler_tx : 1;
    uint64_t stbc_rx_less_80mhz : 1;
    uint64_t stbc_tx_less_80mhz : 1;
    uint64_t ndp_4x_ltf_3d2us_gi : 1;
    uint64_t mid_tx_rx_max_nsts : 2;
    uint64_t su_ppdu_1x_ltf_0d8us_gi : 1;
    uint64_t ldpc_coding_payload : 1;
    uint64_t device_class : 1;
    uint64_t punctured_preamble_rx : 4;
#endif
    void struct_swap() {}
    void struct_init() {}
} __attribute__((packed)) sStaHePhyCapInfo1;

typedef struct sStaHePhyCapInfo2 {
#if defined(__LITTLE_ENDIAN_BITFIELD)
    uint16_t longer_16_sigb_ofdm_symb_support : 1;
    uint16_t non_trig_cqi_fb : 1;
    uint16_t tx_1024qam_less_242tone_ru_support : 1;
    uint16_t rx_1024qam_less_242tone_ru_support : 1;
    uint16_t rx_full_bw_su_mu_ppdu_comp_sigb : 1;
    uint16_t rx_full_bw_su_mu_ppdu_noncomp_sigb : 1;
    uint16_t normal_packet_padding : 2;
    uint16_t mu_ppdu_great_1_ru_rx_max_n_lft : 1;
    uint16_t reserved : 7;
#elif defined(__BIG_ENDIAN_BITFIELD)
    uint16_t reserved : 7;
    uint16_t mu_ppdu_great_1_ru_rx_max_n_lft : 1;
    uint16_t normal_packet_padding : 2;
    uint16_t rx_full_bw_su_mu_ppdu_noncomp_sigb : 1;
    uint16_t rx_full_bw_su_mu_ppdu_comp_sigb : 1;
    uint16_t rx_1024qam_less_242tone_ru_support : 1;
    uint16_t tx_1024qam_less_242tone_ru_support : 1;
    uint16_t non_trig_cqi_fb : 1;
    uint16_t longer_16_sigb_ofdm_symb_support : 1;
#endif
    void struct_swap() {}
    void struct_init() {}
} __attribute__((packed)) sStaHePhyCapInfo2;

typedef struct sStaEhtMacCapInfo {
#if defined(__LITTLE_ENDIAN_BITFIELD)
    uint16_t epcs_priority_access_support : 1;
    uint16_t eht_om_control_support : 1;
    uint16_t txs_mode_1_support : 1;
    uint16_t txs_mode_2_support : 1;
    uint16_t restricted_twt_support : 1;
    uint16_t scs_traffic_descr_support : 1;
    uint16_t max_mpdu_length : 2;
    uint16_t max_ampdu_length_exp_ext : 1;
    uint16_t eht_trs_support : 1;
    uint16_t txop_ret_support_txs_mode_2 : 1;
    uint16_t two_bqrs_support : 1;
    uint16_t eht_link_adaptation_support : 2;
    uint16_t unsolicited_epcs_prio_access_param_update : 1;
    uint16_t reserved : 1;
#elif defined(__BIG_ENDIAN_BITFIELD)
    uint16_t reserved : 1;
    uint16_t unsolicited_epcs_prio_access_param_update : 1;
    uint16_t eht_link_adaptation_support : 2;
    uint16_t two_bqrs_support : 1;
    uint16_t txop_ret_support_txs_mode_2 : 1;
    uint16_t eht_trs_support : 1;
    uint16_t max_ampdu_length_exp_ext : 1;
    uint16_t max_mpdu_length : 2;
    uint16_t scs_traffic_descr_support : 1;
    uint16_t restricted_twt_support : 1;
    uint16_t txs_mode_2_support : 1;
    uint16_t txs_mode_1_support : 1;
    uint16_t eht_om_control_support : 1;
    uint16_t epcs_priority_access_support : 1;
#endif
    void struct_swap() {}
    void struct_init() {}
} __attribute__((packed)) sStaEhtMacCapInfo;

typedef struct sStaEhtPhyCapInfo1 {
#if defined(__LITTLE_ENDIAN_BITFIELD)
    uint32_t reserved : 1;
    uint32_t width_320mhz_in_6_support : 1;
    uint32_t ru_242tone_bw_wider_20_support : 1;
    uint32_t ndp_4_eht_ltf_3_2_gi : 1;
    uint32_t partial_bw_ul_mumimo : 1;
    uint32_t su_beamformer : 1;
    uint32_t su_beamformee : 1;
    uint32_t beamformee_ss_less_80mhz : 3;
    uint32_t beamformee_ss_160mhz : 3;
    uint32_t beamformee_ss_320mhz : 3;
    uint32_t sounding_dim_nb_less_80mhz : 3;
    uint32_t sounding_dim_nb_160mhz : 3;
    uint32_t sounding_dim_nb_320mhz : 3;
    uint32_t ng_16_su_feedback : 1;
    uint32_t ng_16_mu_feedback : 1;
    uint32_t codebook_size_4_2_su_feedback : 1;
    uint32_t codebook_size_7_5_mu_feedback : 1;
    uint32_t triggered_su_beamforming_feedback : 1;
    uint32_t triggered_mu_beamforming_partial_bw_feedback : 1;
    uint32_t triggered_cqi_feedback : 1;
#elif defined(__BIG_ENDIAN_BITFIELD)
    uint32_t triggered_cqi_feedback : 1;
    uint32_t triggered_mu_beamforming_partial_bw_feedback : 1;
    uint32_t triggered_su_beamforming_feedback : 1;
    uint32_t codebook_size_7_5_mu_feedback : 1;
    uint32_t codebook_size_4_2_su_feedback : 1;
    uint32_t ng_16_mu_feedback : 1;
    uint32_t ng_16_su_feedback : 1;
    uint32_t sounding_dim_nb_320mhz : 3;
    uint32_t sounding_dim_nb_160mhz : 3;
    uint32_t sounding_dim_nb_less_80mhz : 3;
    uint32_t beamformee_ss_320mhz : 3;
    uint32_t beamformee_ss_160mhz : 3;
    uint32_t beamformee_ss_less_80mhz : 3;
    uint32_t su_beamformee : 1;
    uint32_t su_beamformer : 1;
    uint32_t partial_bw_ul_mumimo : 1;
    uint32_t ndp_4_eht_ltf_3_2_gi : 1;
    uint32_t ru_242tone_bw_wider_20_support : 1;
    uint32_t width_320mhz_in_6_support : 1;
    uint32_t reserved : 1;
#endif
    void struct_swap() {}
    void struct_init() {}
} __attribute__((packed)) sStaEhtPhyCapInfo1;

typedef struct sStaEhtPhyCapInfo2 {
#if defined(__LITTLE_ENDIAN_BITFIELD)
    uint32_t partial_bw_dl_mumimo : 1;
    uint32_t eht_psr_based_sr_support : 1;
    uint32_t pwr_boost_factor_support : 1;
    uint32_t eht_mu_ppdu_4_eht_ltf_0_8_gi : 1;
    uint32_t max_nc : 4;
    uint32_t non_triggered_cqi_feedback : 1;
    uint32_t tx_1024qam_4096qam_less_242tone_ru_support : 1;
    uint32_t rx_1024qam_4096qam_less_242tone_ru_support : 1;
    uint32_t ppe_thresholds_present : 1;
    uint32_t common_nominal_pkt_padding : 2;
    uint32_t max_nb_supported_eht_ltf : 5;
    uint32_t eht_mcs_15_mru_support : 4;
    uint32_t eht_dup_mcs14_in_6_support : 1;
    uint32_t width_20mhz_op_sta_rcv_ndp_wider_bw_support : 1;
    uint32_t non_ofdma_ul_mumimo_less_80mhz : 1;
    uint32_t non_ofdma_ul_mumimo_160mhz : 1;
    uint32_t non_ofdma_ul_mumimo_320mhz : 1;
    uint32_t mu_beamformer_less_80mhz : 1;
    uint32_t mu_beamformer_160mhz : 1;
    uint32_t mu_beamformer_320mhz : 1;
    uint32_t tb_sounding_feedback_rate_limit : 1;
#elif defined(__BIG_ENDIAN_BITFIELD)
    uint32_t tb_sounding_feedback_rate_limit : 1;
    uint32_t mu_beamformer_320mhz : 1;
    uint32_t mu_beamformer_160mhz : 1;
    uint32_t mu_beamformer_less_80mhz : 1;
    uint32_t non_ofdma_ul_mumimo_320mhz : 1;
    uint32_t non_ofdma_ul_mumimo_160mhz : 1;
    uint32_t non_ofdma_ul_mumimo_less_80mhz : 1;
    uint32_t width_20mhz_op_sta_rcv_ndp_wider_bw_support : 1;
    uint32_t eht_dup_mcs14_in_6_support : 1;
    uint32_t eht_mcs_15_mru_support : 4;
    uint32_t max_nb_supported_eht_ltf : 5;
    uint32_t common_nominal_pkt_padding : 2;
    uint32_t ppe_thresholds_present : 1;
    uint32_t rx_1024qam_4096qam_less_242tone_ru_support : 1;
    uint32_t tx_1024qam_4096qam_less_242tone_ru_support : 1;
    uint32_t non_triggered_cqi_feedback : 1;
    uint32_t max_nc : 4;
    uint32_t eht_mu_ppdu_4_eht_ltf_0_8_gi : 1;
    uint32_t pwr_boost_factor_support : 1;
    uint32_t eht_psr_based_sr_support : 1;
    uint32_t partial_bw_dl_mumimo : 1;
#endif
    void struct_swap() {}
    void struct_init() {}
} __attribute__((packed)) sStaEhtPhyCapInfo2;

typedef struct sStaEhtPhyCapInfo3 {
#if defined(__LITTLE_ENDIAN_BITFIELD)
    uint8_t rw_1024qam_wider_bw_dl_ofdma_support : 1;
    uint8_t rw_4096qam_wider_bw_dl_ofdma_support : 1;
    uint8_t width_20mhz_only_limited_cap_support : 1;
    uint8_t width_20mhz_only_triggered_mu_beamforming_full_bw_feedback_dl_mumimo : 1;
    uint8_t width_20mhz_only_mru_support : 1;
    uint8_t reserved : 3;
#elif defined(__BIG_ENDIAN_BITFIELD)
    uint8_t reserved : 3;
    uint8_t width_20mhz_only_mru_support : 1;
    uint8_t width_20mhz_only_triggered_mu_beamforming_full_bw_feedback_dl_mumimo : 1;
    uint8_t width_20mhz_only_limited_cap_support : 1;
    uint8_t rw_4096qam_wider_bw_dl_ofdma_support : 1;
    uint8_t rw_1024qam_wider_bw_dl_ofdma_support : 1;
#endif
    void struct_swap() {}
    void struct_init() {}
} __attribute__((packed)) sStaEhtPhyCapInfo3;

////////////////////////////////////////////////
// Capability Information field for non DMG STA
///////////////////////////////////////////////

/**
 * @brief This struct used in (Re)Association Request frame
 * to represent Capability Information field transmitted by a NON DMG STA.
 */
typedef struct sCapabilityInfoNonDmgSta {
#if defined(__LITTLE_ENDIAN_BITFIELD)
    uint16_t ess : 1;
    uint16_t ibss : 1;
    uint16_t cf_pollable : 1;
    uint16_t cf_poll_request : 1;
    uint16_t privacy : 1;
    uint16_t short_preamble : 1;
    uint16_t reserved2 : 2;
    uint16_t spectrum_management : 1;
    uint16_t qos : 1;
    uint16_t short_slot_time : 1;
    uint16_t apsd : 1;
    uint16_t radio_measurement : 1;
    uint16_t reserved1 : 1;
    uint16_t delayed_block_ack : 1;
    uint16_t immediate_block_ack : 1;
#elif defined(__BIG_ENDIAN_BITFIELD)
    uint16_t immediate_block_ack : 1;
    uint16_t delayed_block_ack : 1;
    uint16_t reserved1 : 1;
    uint16_t radio_measurement : 1;
    uint16_t apsd : 1;
    uint16_t short_slot_time : 1;
    uint16_t qos : 1;
    uint16_t spectrum_management : 1;
    uint16_t reserved2 : 2;
    uint16_t short_preamble : 1;
    uint16_t privacy : 1;
    uint16_t cf_poll_request : 1;
    uint16_t cf_pollable : 1;
    uint16_t ibss : 1;
    uint16_t ess : 1;
#endif
    // sCapabilityInfoNonDmgSta(){};
    void struct_swap() {}
    void struct_init() {}
} __attribute__((packed)) sCapabilityInfoNonDmgSta;

typedef struct sRmEnabledCaps1 {
#if defined(__LITTLE_ENDIAN_BITFIELD)
    uint32_t link_measurement : 1;
    uint32_t neighbor_report : 1;
    uint32_t parallel_measure : 1;
    uint32_t repeated_measure : 1;
    uint32_t beacon_passive_measure : 1;
    uint32_t beacon_active_measure : 1;
    uint32_t beacon_table_measure : 1;
    uint32_t
        beacon_measure_report_cond : 1; // Beacon measurement reporting conditions capability enabled
    uint32_t frame_measurement : 1;     // Frame measurement capability enabled
    uint32_t ch_load_measure : 1;       // Channel load measurement capability enabled
    uint32_t noise_histogram_measure : 1; // Noise histogram measurement capability enabled
    uint32_t stat_measure : 1;            // Statistics measurement capability enabled
    uint32_t lci_measure : 1;
    uint32_t lci_azimuth : 1;
    uint32_t tx_stream : 1;    // Transmit Stream/category measurement capability enabled
    uint32_t trigger_tx : 1;   // Triggered tx stream/category measurement capability enabled
    uint32_t ap_ch_report : 1; // AP channel report capability enabled
    uint32_t rm_mib : 1;
    uint32_t op_ch_max_measure_dur : 3;    // Operating channel Max measurement Duration
    uint32_t nonop_ch_max_measure_dur : 3; // Nonoperating channel Max measurement Duration
    uint32_t measure_pilot_cap : 3;        // Measurement pilot Capability
    uint32_t measure_pilot_trans_info : 1; // Measurement pilot Transmission information cap enabled
    uint32_t neighbor_report_tsf_offset : 1;
    uint32_t rcpi_measure : 1;
    uint32_t rsni_measure : 1;
    uint32_t bss_average_ac_delay : 1;
#elif defined(__BIG_ENDIAN_BITFIELD)
    uint32_t bss_average_ac_delay : 1;
    uint32_t rsni_measure : 1;
    uint32_t rcpi_measure : 1;
    uint32_t neighbor_report_tsf_offset : 1;
    uint32_t measure_pilot_trans_info : 1;
    uint32_t measure_pilot_cap : 3;
    uint32_t nonop_ch_max_measure_dur : 3;
    uint32_t op_ch_max_measure_dur : 3;
    uint32_t rm_mib : 1;
    uint32_t ap_ch_report : 1;
    uint32_t trigger_tx : 1;
    uint32_t tx_stream : 1;
    uint32_t lci_azimuth : 1;
    uint32_t lci_measure : 1;
    uint32_t stat_measure : 1;
    uint32_t noise_histogram_measure : 1;
    uint32_t ch_load_measure : 1;
    uint32_t frame_measurement : 1;
    uint32_t beacon_measure_report_cond : 1;
    uint32_t beacon_table_measure : 1;
    uint32_t beacon_active_measure : 1;
    uint32_t beacon_passive_measure : 1;
    uint32_t repeated_measure : 1;
    uint32_t parallel_measure : 1;
    uint32_t neighbor_report : 1;
    uint32_t link_measurement : 1;
#endif
    // sRmEnabledCaps1(){};
    void struct_swap() {}
    void struct_init() {}
} __attribute__((packed)) sRmEnabledCaps1;

typedef struct sRmEnabledCaps2 {
#if defined(__LITTLE_ENDIAN_BITFIELD)
    uint8_t bss_available_adm_capacity : 1;
    uint8_t antenna : 1;
    uint8_t ftm_range_report : 1;
    uint8_t civic_location_measure : 1;
    uint8_t reserved : 4;
#elif defined(__BIG_ENDIAN_BITFIELD)
    uint8_t reserved : 4;
    uint8_t civic_location_measure : 1;
    uint8_t ftm_range_report : 1;
    uint8_t antenna : 1;
    uint8_t bss_available_adm_capacity : 1;
#endif
    // sRmEnabledCaps2(){};
    void struct_swap() {}
    void struct_init() {}
} __attribute__((packed)) sRmEnabledCaps2;

} // namespace assoc_frame

#endif // _ASSOCIATION_FRAME_STRUCTS_
