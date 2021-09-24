/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2020  OpenSynergy GmbH
 *
 * This header is BSD licensed so anyone can use the definitions to
 * implement compatible drivers/servers.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of OpenSynergy GmbH nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL IBM OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#ifndef VIRTIO_SND_IF_H
#define VIRTIO_SND_IF_H

#include <linux/virtio_types.h>

/*******************************************************************************
 * DEVICE FEATURES
 */
enum {
	/* OpenSynergy extensions support */
	VIRTIO_SND_F_OPSY_EXT = 0xf
};

/*******************************************************************************
 * CONFIGURATION SPACE
 */
struct virtio_snd_config {
	/* # of available physical jacks */
	__le32 jacks;
	/* # of available PCM streams */
	__le32 streams;
	/* # of available channel maps */
	__le32 chmaps;
	/* # of available device controls */
	__le32 controls;
};

enum {
	/* device virtqueue indexes */
	VIRTIO_SND_VQ_CONTROL = 0,
	VIRTIO_SND_VQ_EVENT,
	VIRTIO_SND_VQ_TX,
	VIRTIO_SND_VQ_RX,
	/* a number of device virtqueues */
	VIRTIO_SND_VQ_MAX
};

/*******************************************************************************
 * COMMON DEFINITIONS
 */

/* supported dataflow directions */
enum {
	VIRTIO_SND_D_OUTPUT = 0,
	VIRTIO_SND_D_INPUT
};

enum {
	/* jack control request types */
	VIRTIO_SND_R_JACK_INFO = 1,
	VIRTIO_SND_R_JACK_REMAP,

	/* PCM control request types */
	VIRTIO_SND_R_PCM_INFO = 0x0100,
	VIRTIO_SND_R_PCM_SET_PARAMS,
	VIRTIO_SND_R_PCM_PREPARE,
	VIRTIO_SND_R_PCM_RELEASE,
	VIRTIO_SND_R_PCM_START,
	VIRTIO_SND_R_PCM_STOP,

	/* channel map control request types */
	VIRTIO_SND_R_CHMAP_INFO = 0x0200,

	/* jack event types */
	VIRTIO_SND_EVT_JACK_CONNECTED = 0x1000,
	VIRTIO_SND_EVT_JACK_DISCONNECTED,

	/* PCM event types */
	VIRTIO_SND_EVT_PCM_PERIOD_ELAPSED = 0x1100,
	VIRTIO_SND_EVT_PCM_XRUN,

	/* common status codes */
	VIRTIO_SND_S_OK = 0x8000,
	VIRTIO_SND_S_BAD_MSG,
	VIRTIO_SND_S_NOT_SUPP,
	VIRTIO_SND_S_IO_ERR,

	/* --- OpenSynergy extensions --------------------------------------- */
	VIRTIO_SND_R_OPSY_INFO = 0xf000,

	/* ALSA extended device information */
	VIRTIO_SND_R_ALSA_CARD_INFO = 0xf100,
	VIRTIO_SND_R_ALSA_PCM_INFO,

	/* ALSA device controls */
	VIRTIO_SND_R_DC_INFO = 0xf200,
	VIRTIO_SND_R_DC_ENUM_INFO,
	VIRTIO_SND_R_DC_READ,
	VIRTIO_SND_R_DC_WRITE,
	VIRTIO_SND_R_DC_TLV_READ,
	VIRTIO_SND_R_DC_TLV_WRITE,
	VIRTIO_SND_R_DC_TLV_COMMAND,

	/* ALSA device control events */
	VIRTIO_SND_EVT_DC_NOTIFY = 0xff00,
};

/* common header */
struct virtio_snd_hdr {
	__le32 code;
};

/* event notification */
struct virtio_snd_event {
	/* VIRTIO_SND_EVT_XXX */
	struct virtio_snd_hdr hdr;
	/* optional event data */
	__le32 data;
};

/* common control request to query an item information */
struct virtio_snd_query_info {
	/* VIRTIO_SND_R_XXX_INFO */
	struct virtio_snd_hdr hdr;
	/* item start identifier */
	__le32 start_id;
	/* item count to query */
	__le32 count;
	/* item information size in bytes */
	__le32 size;
};

/* common item information header */
struct virtio_snd_info {
	/* function group node id (High Definition Audio Specification 7.1.2) */
	__le32 hda_fn_nid;
};

/*******************************************************************************
 * JACK CONTROL MESSAGES
 */
struct virtio_snd_jack_hdr {
	/* VIRTIO_SND_R_JACK_XXX */
	struct virtio_snd_hdr hdr;
	/* 0 ... virtio_snd_config::jacks - 1 */
	__le32 jack_id;
};

/* supported jack features */
enum {
	VIRTIO_SND_JACK_F_REMAP = 0
};

struct virtio_snd_jack_info {
	/* common header */
	struct virtio_snd_info hdr;
	/* supported feature bit map (1 << VIRTIO_SND_JACK_F_XXX) */
	__le32 features;
	/* pin configuration (High Definition Audio Specification 7.3.3.31) */
	__le32 hda_reg_defconf;
	/* pin capabilities (High Definition Audio Specification 7.3.4.9) */
	__le32 hda_reg_caps;
	/* current jack connection status (0: disconnected, 1: connected) */
	__u8 connected;

	__u8 padding[7];
};

/* jack remapping control request */
struct virtio_snd_jack_remap {
	/* .code = VIRTIO_SND_R_JACK_REMAP */
	struct virtio_snd_jack_hdr hdr;
	/* selected association number */
	__le32 association;
	/* selected sequence number */
	__le32 sequence;
};

/*******************************************************************************
 * PCM CONTROL MESSAGES
 */
struct virtio_snd_pcm_hdr {
	/* VIRTIO_SND_R_PCM_XXX */
	struct virtio_snd_hdr hdr;
	/* 0 ... virtio_snd_config::streams - 1 */
	__le32 stream_id;
};

/* supported PCM stream features */
enum {
	VIRTIO_SND_PCM_F_SHMEM_HOST = 0,
	VIRTIO_SND_PCM_F_SHMEM_GUEST,
	VIRTIO_SND_PCM_F_MSG_POLLING,
	VIRTIO_SND_PCM_F_EVT_SHMEM_PERIODS,
	VIRTIO_SND_PCM_F_EVT_XRUNS
};

/* supported PCM sample formats */
enum {
	/* analog formats (width / physical width) */
	VIRTIO_SND_PCM_FMT_IMA_ADPCM = 0,	/*  4 /  4 bits */
	VIRTIO_SND_PCM_FMT_MU_LAW,		/*  8 /  8 bits */
	VIRTIO_SND_PCM_FMT_A_LAW,		/*  8 /  8 bits */
	VIRTIO_SND_PCM_FMT_S8,			/*  8 /  8 bits */
	VIRTIO_SND_PCM_FMT_U8,			/*  8 /  8 bits */
	VIRTIO_SND_PCM_FMT_S16,			/* 16 / 16 bits */
	VIRTIO_SND_PCM_FMT_U16,			/* 16 / 16 bits */
	VIRTIO_SND_PCM_FMT_S18_3,		/* 18 / 24 bits */
	VIRTIO_SND_PCM_FMT_U18_3,		/* 18 / 24 bits */
	VIRTIO_SND_PCM_FMT_S20_3,		/* 20 / 24 bits */
	VIRTIO_SND_PCM_FMT_U20_3,		/* 20 / 24 bits */
	VIRTIO_SND_PCM_FMT_S24_3,		/* 24 / 24 bits */
	VIRTIO_SND_PCM_FMT_U24_3,		/* 24 / 24 bits */
	VIRTIO_SND_PCM_FMT_S20,			/* 20 / 32 bits */
	VIRTIO_SND_PCM_FMT_U20,			/* 20 / 32 bits */
	VIRTIO_SND_PCM_FMT_S24,			/* 24 / 32 bits */
	VIRTIO_SND_PCM_FMT_U24,			/* 24 / 32 bits */
	VIRTIO_SND_PCM_FMT_S32,			/* 32 / 32 bits */
	VIRTIO_SND_PCM_FMT_U32,			/* 32 / 32 bits */
	VIRTIO_SND_PCM_FMT_FLOAT,		/* 32 / 32 bits */
	VIRTIO_SND_PCM_FMT_FLOAT64,		/* 64 / 64 bits */
	/* digital formats (width / physical width) */
	VIRTIO_SND_PCM_FMT_DSD_U8,		/*  8 /  8 bits */
	VIRTIO_SND_PCM_FMT_DSD_U16,		/* 16 / 16 bits */
	VIRTIO_SND_PCM_FMT_DSD_U32,		/* 32 / 32 bits */
	VIRTIO_SND_PCM_FMT_IEC958_SUBFRAME	/* 32 / 32 bits */
};

/* supported PCM frame rates */
enum {
	VIRTIO_SND_PCM_RATE_5512 = 0,
	VIRTIO_SND_PCM_RATE_8000,
	VIRTIO_SND_PCM_RATE_11025,
	VIRTIO_SND_PCM_RATE_16000,
	VIRTIO_SND_PCM_RATE_22050,
	VIRTIO_SND_PCM_RATE_32000,
	VIRTIO_SND_PCM_RATE_44100,
	VIRTIO_SND_PCM_RATE_48000,
	VIRTIO_SND_PCM_RATE_64000,
	VIRTIO_SND_PCM_RATE_88200,
	VIRTIO_SND_PCM_RATE_96000,
	VIRTIO_SND_PCM_RATE_176400,
	VIRTIO_SND_PCM_RATE_192000,
	VIRTIO_SND_PCM_RATE_384000
};

struct virtio_snd_pcm_info {
	/* common header */
	struct virtio_snd_info hdr;
	/* supported feature bit map (1 << VIRTIO_SND_PCM_F_XXX) */
	__le32 features;
	/* supported sample format bit map (1 << VIRTIO_SND_PCM_FMT_XXX) */
	__le64 formats;
	/* supported frame rate bit map (1 << VIRTIO_SND_PCM_RATE_XXX) */
	__le64 rates;
	/* dataflow direction (VIRTIO_SND_D_XXX) */
	__u8 direction;
	/* minimum # of supported channels */
	__u8 channels_min;
	/* maximum # of supported channels */
	__u8 channels_max;

	__u8 padding[5];
};

/* set PCM stream format */
struct virtio_snd_pcm_set_params {
	/* .code = VIRTIO_SND_R_PCM_SET_PARAMS */
	struct virtio_snd_pcm_hdr hdr;
	/* size of the hardware buffer */
	__le32 buffer_bytes;
	/* size of the hardware period */
	__le32 period_bytes;
	/* selected feature bit map (1 << VIRTIO_SND_PCM_F_XXX) */
	__le32 features;
	/* selected # of channels */
	__u8 channels;
	/* selected sample format (VIRTIO_SND_PCM_FMT_XXX) */
	__u8 format;
	/* selected frame rate (VIRTIO_SND_PCM_RATE_XXX) */
	__u8 rate;

	__u8 padding;
};

/*******************************************************************************
 * PCM I/O MESSAGES
 */

/* I/O request header */
struct virtio_snd_pcm_xfer {
	/* 0 ... virtio_snd_config::streams - 1 */
	__le32 stream_id;
};

/* I/O request status */
struct virtio_snd_pcm_status {
	/* VIRTIO_SND_S_XXX */
	__le32 status;
	/* current device latency */
	__le32 latency_bytes;
};

/*******************************************************************************
 * CHANNEL MAP CONTROL MESSAGES
 */
struct virtio_snd_chmap_hdr {
	/* VIRTIO_SND_R_CHMAP_XXX */
	struct virtio_snd_hdr hdr;
	/* 0 ... virtio_snd_config::chmaps - 1 */
	__le32 chmap_id;
};

/* standard channel position definition */
enum {
	VIRTIO_SND_CHMAP_NONE = 0,	/* undefined */
	VIRTIO_SND_CHMAP_NA,		/* silent */
	VIRTIO_SND_CHMAP_MONO,		/* mono stream */
	VIRTIO_SND_CHMAP_FL,		/* front left */
	VIRTIO_SND_CHMAP_FR,		/* front right */
	VIRTIO_SND_CHMAP_RL,		/* rear left */
	VIRTIO_SND_CHMAP_RR,		/* rear right */
	VIRTIO_SND_CHMAP_FC,		/* front center */
	VIRTIO_SND_CHMAP_LFE,		/* low frequency (LFE) */
	VIRTIO_SND_CHMAP_SL,		/* side left */
	VIRTIO_SND_CHMAP_SR,		/* side right */
	VIRTIO_SND_CHMAP_RC,		/* rear center */
	VIRTIO_SND_CHMAP_FLC,		/* front left center */
	VIRTIO_SND_CHMAP_FRC,		/* front right center */
	VIRTIO_SND_CHMAP_RLC,		/* rear left center */
	VIRTIO_SND_CHMAP_RRC,		/* rear right center */
	VIRTIO_SND_CHMAP_FLW,		/* front left wide */
	VIRTIO_SND_CHMAP_FRW,		/* front right wide */
	VIRTIO_SND_CHMAP_FLH,		/* front left high */
	VIRTIO_SND_CHMAP_FCH,		/* front center high */
	VIRTIO_SND_CHMAP_FRH,		/* front right high */
	VIRTIO_SND_CHMAP_TC,		/* top center */
	VIRTIO_SND_CHMAP_TFL,		/* top front left */
	VIRTIO_SND_CHMAP_TFR,		/* top front right */
	VIRTIO_SND_CHMAP_TFC,		/* top front center */
	VIRTIO_SND_CHMAP_TRL,		/* top rear left */
	VIRTIO_SND_CHMAP_TRR,		/* top rear right */
	VIRTIO_SND_CHMAP_TRC,		/* top rear center */
	VIRTIO_SND_CHMAP_TFLC,		/* top front left center */
	VIRTIO_SND_CHMAP_TFRC,		/* top front right center */
	VIRTIO_SND_CHMAP_TSL,		/* top side left */
	VIRTIO_SND_CHMAP_TSR,		/* top side right */
	VIRTIO_SND_CHMAP_LLFE,		/* left LFE */
	VIRTIO_SND_CHMAP_RLFE,		/* right LFE */
	VIRTIO_SND_CHMAP_BC,		/* bottom center */
	VIRTIO_SND_CHMAP_BLC,		/* bottom left center */
	VIRTIO_SND_CHMAP_BRC		/* bottom right center */
};

/* maximum possible number of channels */
#define VIRTIO_SND_CHMAP_MAX_SIZE	18

struct virtio_snd_chmap_info {
	/* common header */
	struct virtio_snd_info hdr;
	/* dataflow direction (VIRTIO_SND_D_XXX) */
	__u8 direction;
	/* # of valid channel position values */
	__u8 channels;
	/* channel position values (VIRTIO_SND_CHMAP_XXX) */
	__u8 positions[VIRTIO_SND_CHMAP_MAX_SIZE];
};

/*******************************************************************************
 * OPENSYNERGY EXTENSIONS
 */
enum {
	/* ALSA extended device information support */
	VIRTIO_SND_OPSY_F_DEV_EXT_INFO = 0,
	/* ALSA device controls support */
	VIRTIO_SND_OPSY_F_DEV_CTLS
};

struct virtio_snd_opsy_info {
	/* VIRTIO_SND_S_XXX */
	struct virtio_snd_hdr hdr;
	/* supported extension bit map (1 << VIRTIO_SND_OPSY_F_XXX) */
	__le32 extensions;
};

/*******************************************************************************
 * ALSA EXTENDED DEVICE INFORMATION
 */
struct virtio_snd_alsa_card_info {
	/* ID string of soundcard */
	__u8 id[16];
	/* Driver name */
	__u8 driver[16];
	/* short name of soundcard */
	__u8 name[32];
	/* name + info text about soundcard */
	__u8 longname[80];
	/* visual mixer identification */
	__u8 mixername[80];
	/* card components delimited with space */
	__u8 components[128];
};

struct virtio_snd_alsa_query_pcm_info {
	/* .code = VIRTIO_SND_R_ALSA_PCM_INFO */
	struct virtio_snd_hdr hdr;
	/* function group node id */
	__le32 hda_fn_nid;
};

struct virtio_snd_alsa_pcm_info {
	/* ID string of PCM device */
	__u8 id[64];
	/* name of PCM device */
	__u8 name[80];
};

/*******************************************************************************
 * ALSA DEVICE CONTROLS
 */
struct virtio_snd_dc_hdr {
	/* VIRTIO_SND_R_DC_XXX */
	struct virtio_snd_hdr hdr;
	/* 0 ... virtio_snd_config::controls - 1 */
	__le16 control_id;
	/* subcontrol identifier */
	__le16 subcontrol_id;
};

/*
 * The following structure is de facto part of the protocol, but cannot be
 * defined here because it contains the snd_ctl_elem_info body. It represents
 * information about a device control in response to the VIRTIO_SND_R_DC_INFO
 * command.
 *
 * struct virtio_snd_dc_info {
 *     struct virtio_snd_info hdr;
 *     struct snd_ctl_elem_info elem_info;
 * };
 */

#define VIRTIO_SND_DC_NAME_MAXLEN	64

struct virtio_snd_dc_enum_value {
	/* null-terminated ASCII value name */
	__u8 name[VIRTIO_SND_DC_NAME_MAXLEN];
};

struct virtio_snd_dc_event {
	/* VIRTIO_SND_EVT_DC_XXX */
	struct virtio_snd_hdr hdr;
	/* 0 ... virtio_snd_config::controls - 1 */
	__le16 control_id;
	/* event mask (SNDRV_CTL_EVENT_MASK_XXX) */
	__le16 mask;
};

#endif /* VIRTIO_SND_IF_H */
