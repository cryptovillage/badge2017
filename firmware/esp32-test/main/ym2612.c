/*
**
** software implementation of Yamaha FM sound generator (YM2612/YM3438)
**
** Original code (MAME fm.c)
**
** Copyright (C) 2001, 2002, 2003 Jarek Burczynski (bujar at mame dot net)
** Copyright (C) 1998 Tatsuyuki Satoh , MultiArcadeMachineEmulator development
**
** Version 1.4 (final beta)
**
** Additional code & fixes by Eke-Eke for Genesis Plus GX
**
** Some changes made by Karl Koscher for the CPV Badge Demo
**
** Huge thanks to Nemesis, most of those fixes came from his tests on Sega Genesis hardware
** More informations at http://gendev.spritesmind.net/forum/viewtopic.php?t=386
**
**  TODO:
**  - better documentation
**  - BUSY flag emulation
*/

/*
**  CHANGELOG:
**
** 01-09-2012 Eke-Eke (Genesis Plus GX):
**  - removed input clock / output samplerate frequency ratio, chip now always run at (original) internal sample frequency
**  - removed now uneeded extra bits of precision
**
** 2006~2012  Eke-Eke (Genesis Plus GX):
**  - removed unused multichip support
**  - added YM2612 Context external access functions
**  - fixed LFO implementation:
**      .added support for CH3 special mode: fixes various sound effects (birds in Warlock, bug sound in Aladdin...)
**      .inverted LFO AM waveform: fixes Spider-Man & Venom : Separation Anxiety (intro), California Games (surfing event)
**      .improved LFO timing accuracy: now updated AFTER sample output, like EG/PG updates, and without any precision loss anymore.
**  - improved internal timers emulation
**  - adjusted lowest EG rates increment values
**  - fixed Attack Rate not being updated in some specific cases (Batman & Robin intro)
**  - fixed EG behavior when Attack Rate is maximal
**  - fixed EG behavior when SL=0 (Mega Turrican tracks 03,09...) or/and Key ON occurs at minimal attenuation
**  - implemented EG output immediate changes on register writes
**  - fixed YM2612 initial values (after the reset): fixes missing intro in B.O.B
**  - implemented Detune overflow (Ariel, Comix Zone, Shaq Fu, Spiderman & many other games using GEMS sound engine)
**  - implemented accurate CSM mode emulation
**  - implemented accurate SSG-EG emulation (Asterix, Beavis&Butthead, Bubba'n Stix & many other games)
**  - implemented accurate address/data ports behavior
**  - added preliminar support for DAC precision
**
** 03-08-2003 Jarek Burczynski:
**  - fixed YM2608 initial values (after the reset)
**  - fixed flag and irqmask handling (YM2608)
**  - fixed BUFRDY flag handling (YM2608)
**
** 14-06-2003 Jarek Burczynski:
**  - implemented all of the YM2608 status register flags
**  - implemented support for external memory read/write via YM2608
**  - implemented support for deltat memory limit register in YM2608 emulation
**
** 22-05-2003 Jarek Burczynski:
**  - fixed LFO PM calculations (copy&paste bugfix)
**
** 08-05-2003 Jarek Burczynski:
**  - fixed SSG support
**
** 22-04-2003 Jarek Burczynski:
**  - implemented 100% correct LFO generator (verified on real YM2610 and YM2608)
**
** 15-04-2003 Jarek Burczynski:
**  - added support for YM2608's register 0x110 - status mask
**
** 01-12-2002 Jarek Burczynski:
**  - fixed register addressing in YM2608, YM2610, YM2610B chips. (verified on real YM2608)
**    The addressing patch used for early Neo-Geo games can be removed now.
**
** 26-11-2002 Jarek Burczynski, Nicola Salmoria:
**  - recreated YM2608 ADPCM ROM using data from real YM2608's output which leads to:
**  - added emulation of YM2608 drums.
**  - output of YM2608 is two times lower now - same as YM2610 (verified on real YM2608)
**
** 16-08-2002 Jarek Burczynski:
**  - binary exact Envelope Generator (verified on real YM2203);
**    identical to YM2151
**  - corrected 'off by one' error in feedback calculations (when feedback is off)
**  - corrected connection (algorithm) calculation (verified on real YM2203 and YM2610)
**
** 18-12-2001 Jarek Burczynski:
**  - added SSG-EG support (verified on real YM2203)
**
** 12-08-2001 Jarek Burczynski:
**  - corrected sin_tab and tl_tab data (verified on real chip)
**  - corrected feedback calculations (verified on real chip)
**  - corrected phase generator calculations (verified on real chip)
**  - corrected envelope generator calculations (verified on real chip)
**  - corrected FM volume level (YM2610 and YM2610B).
**  - changed YMxxxUpdateOne() functions (YM2203, YM2608, YM2610, YM2610B, YM2612) :
**    this was needed to calculate YM2610 FM channels output correctly.
**    (Each FM channel is calculated as in other chips, but the output of the channel
**    gets shifted right by one *before* sending to accumulator. That was impossible to do
**    with previous implementation).
**
** 23-07-2001 Jarek Burczynski, Nicola Salmoria:
**  - corrected YM2610 ADPCM type A algorithm and tables (verified on real chip)
**
** 11-06-2001 Jarek Burczynski:
**  - corrected end of sample bug in ADPCMA_calc_cha().
**    Real YM2610 checks for equality between current and end addresses (only 20 LSB bits).
**
** 08-12-98 hiro-shi:
** rename ADPCMA -> ADPCMB, ADPCMB -> ADPCMA
** move ROM limit check.(CALC_CH? -> 2610Write1/2)
** test program (ADPCMB_TEST)
** move ADPCM A/B end check.
** ADPCMB repeat flag(no check)
** change ADPCM volume rate (8->16) (32->48).
**
** 09-12-98 hiro-shi:
** change ADPCM volume. (8->16, 48->64)
** replace ym2610 ch0/3 (YM-2610B)
** change ADPCM_SHIFT (10->8) missing bank change 0x4000-0xffff.
** add ADPCM_SHIFT_MASK
** change ADPCMA_DECODE_MIN/MAX.
*/

/************************************************************************/
/*    comment of hiro-shi(Hiromitsu Shioya)                             */
/*    YM2610(B) = OPN-B                                                 */
/*    YM2610  : PSG:3ch FM:4ch ADPCM(18.5KHz):6ch DeltaT ADPCM:1ch      */
/*    YM2610B : PSG:3ch FM:6ch ADPCM(18.5KHz):6ch DeltaT ADPCM:1ch      */
/************************************************************************/

#include "shared.h"

/* envelope generator */
#define ENV_BITS    10
#define ENV_LEN      (1<<ENV_BITS)
#define ENV_STEP    (128.0/ENV_LEN)

#define MAX_ATT_INDEX  (ENV_LEN-1) /* 1023 */
#define MIN_ATT_INDEX  (0)      /* 0 */

#define EG_ATT      4
#define EG_DEC      3
#define EG_SUS      2
#define EG_REL      1
#define EG_OFF      0

/* phase generator (detune mask) */
#define DT_BITS     17
#define DT_LEN      (1l << DT_BITS)
#define DT_MASK     (DT_LEN - 1)

/* operator unit */
#define SIN_BITS    10
#define SIN_LEN      (1<<SIN_BITS)
#define SIN_MASK    (SIN_LEN-1)

#define TL_RES_LEN    (256) /* 8 bits addressing (real chip) */

#define TL_BITS    14 /* channel output */

/*  TL_TAB_LEN is calculated as:
*   13 - sinus amplitude bits     (Y axis)
*   2  - sinus sign bit           (Y axis)
*   TL_RES_LEN - sinus resolution (X axis)
*/
#define TL_TAB_LEN (13*2*TL_RES_LEN)
static const signed short tl_tab[TL_TAB_LEN] = {
		0x1fe8, 0xe018, 0x1fd4, 0xe02c, 0x1fbc, 0xe044, 0x1fa8, 0xe058,
		0x1f90, 0xe070, 0x1f7c, 0xe084, 0x1f68, 0xe098, 0x1f50, 0xe0b0,
		0x1f3c, 0xe0c4, 0x1f24, 0xe0dc, 0x1f10, 0xe0f0, 0x1efc, 0xe104,
		0x1ee4, 0xe11c, 0x1ed0, 0xe130, 0x1eb8, 0xe148, 0x1ea4, 0xe15c,
		0x1e90, 0xe170, 0x1e7c, 0xe184, 0x1e64, 0xe19c, 0x1e50, 0xe1b0,
		0x1e3c, 0xe1c4, 0x1e28, 0xe1d8, 0x1e10, 0xe1f0, 0x1dfc, 0xe204,
		0x1de8, 0xe218, 0x1dd4, 0xe22c, 0x1dc0, 0xe240, 0x1da8, 0xe258,
		0x1d94, 0xe26c, 0x1d80, 0xe280, 0x1d6c, 0xe294, 0x1d58, 0xe2a8,
		0x1d44, 0xe2bc, 0x1d30, 0xe2d0, 0x1d1c, 0xe2e4, 0x1d08, 0xe2f8,
		0x1cf4, 0xe30c, 0x1ce0, 0xe320, 0x1ccc, 0xe334, 0x1cb8, 0xe348,
		0x1ca4, 0xe35c, 0x1c90, 0xe370, 0x1c7c, 0xe384, 0x1c68, 0xe398,
		0x1c54, 0xe3ac, 0x1c40, 0xe3c0, 0x1c2c, 0xe3d4, 0x1c18, 0xe3e8,
		0x1c08, 0xe3f8, 0x1bf4, 0xe40c, 0x1be0, 0xe420, 0x1bcc, 0xe434,
		0x1bb8, 0xe448, 0x1ba4, 0xe45c, 0x1b94, 0xe46c, 0x1b80, 0xe480,
		0x1b6c, 0xe494, 0x1b58, 0xe4a8, 0x1b48, 0xe4b8, 0x1b34, 0xe4cc,
		0x1b20, 0xe4e0, 0x1b10, 0xe4f0, 0x1afc, 0xe504, 0x1ae8, 0xe518,
		0x1ad4, 0xe52c, 0x1ac4, 0xe53c, 0x1ab0, 0xe550, 0x1aa0, 0xe560,
		0x1a8c, 0xe574, 0x1a78, 0xe588, 0x1a68, 0xe598, 0x1a54, 0xe5ac,
		0x1a44, 0xe5bc, 0x1a30, 0xe5d0, 0x1a20, 0xe5e0, 0x1a0c, 0xe5f4,
		0x19fc, 0xe604, 0x19e8, 0xe618, 0x19d8, 0xe628, 0x19c4, 0xe63c,
		0x19b4, 0xe64c, 0x19a0, 0xe660, 0x1990, 0xe670, 0x197c, 0xe684,
		0x196c, 0xe694, 0x195c, 0xe6a4, 0x1948, 0xe6b8, 0x1938, 0xe6c8,
		0x1924, 0xe6dc, 0x1914, 0xe6ec, 0x1904, 0xe6fc, 0x18f0, 0xe710,
		0x18e0, 0xe720, 0x18d0, 0xe730, 0x18c0, 0xe740, 0x18ac, 0xe754,
		0x189c, 0xe764, 0x188c, 0xe774, 0x1878, 0xe788, 0x1868, 0xe798,
		0x1858, 0xe7a8, 0x1848, 0xe7b8, 0x1838, 0xe7c8, 0x1824, 0xe7dc,
		0x1814, 0xe7ec, 0x1804, 0xe7fc, 0x17f4, 0xe80c, 0x17e4, 0xe81c,
		0x17d4, 0xe82c, 0x17c0, 0xe840, 0x17b0, 0xe850, 0x17a0, 0xe860,
		0x1790, 0xe870, 0x1780, 0xe880, 0x1770, 0xe890, 0x1760, 0xe8a0,
		0x1750, 0xe8b0, 0x1740, 0xe8c0, 0x1730, 0xe8d0, 0x1720, 0xe8e0,
		0x1710, 0xe8f0, 0x1700, 0xe900, 0x16f0, 0xe910, 0x16e0, 0xe920,
		0x16d0, 0xe930, 0x16c0, 0xe940, 0x16b0, 0xe950, 0x16a0, 0xe960,
		0x1690, 0xe970, 0x1680, 0xe980, 0x1670, 0xe990, 0x1664, 0xe99c,
		0x1654, 0xe9ac, 0x1644, 0xe9bc, 0x1634, 0xe9cc, 0x1624, 0xe9dc,
		0x1614, 0xe9ec, 0x1604, 0xe9fc, 0x15f8, 0xea08, 0x15e8, 0xea18,
		0x15d8, 0xea28, 0x15c8, 0xea38, 0x15bc, 0xea44, 0x15ac, 0xea54,
		0x159c, 0xea64, 0x158c, 0xea74, 0x1580, 0xea80, 0x1570, 0xea90,
		0x1560, 0xeaa0, 0x1550, 0xeab0, 0x1544, 0xeabc, 0x1534, 0xeacc,
		0x1524, 0xeadc, 0x1518, 0xeae8, 0x1508, 0xeaf8, 0x14f8, 0xeb08,
		0x14ec, 0xeb14, 0x14dc, 0xeb24, 0x14d0, 0xeb30, 0x14c0, 0xeb40,
		0x14b0, 0xeb50, 0x14a4, 0xeb5c, 0x1494, 0xeb6c, 0x1488, 0xeb78,
		0x1478, 0xeb88, 0x146c, 0xeb94, 0x145c, 0xeba4, 0x1450, 0xebb0,
		0x1440, 0xebc0, 0x1430, 0xebd0, 0x1424, 0xebdc, 0x1418, 0xebe8,
		0x1408, 0xebf8, 0x13fc, 0xec04, 0x13ec, 0xec14, 0x13e0, 0xec20,
		0x13d0, 0xec30, 0x13c4, 0xec3c, 0x13b4, 0xec4c, 0x13a8, 0xec58,
		0x139c, 0xec64, 0x138c, 0xec74, 0x1380, 0xec80, 0x1370, 0xec90,
		0x1364, 0xec9c, 0x1358, 0xeca8, 0x1348, 0xecb8, 0x133c, 0xecc4,
		0x1330, 0xecd0, 0x1320, 0xece0, 0x1314, 0xecec, 0x1308, 0xecf8,
		0x12f8, 0xed08, 0x12ec, 0xed14, 0x12e0, 0xed20, 0x12d4, 0xed2c,
		0x12c4, 0xed3c, 0x12b8, 0xed48, 0x12ac, 0xed54, 0x12a0, 0xed60,
		0x1290, 0xed70, 0x1284, 0xed7c, 0x1278, 0xed88, 0x126c, 0xed94,
		0x1260, 0xeda0, 0x1250, 0xedb0, 0x1244, 0xedbc, 0x1238, 0xedc8,
		0x122c, 0xedd4, 0x1220, 0xede0, 0x1214, 0xedec, 0x1208, 0xedf8,
		0x11f8, 0xee08, 0x11ec, 0xee14, 0x11e0, 0xee20, 0x11d4, 0xee2c,
		0x11c8, 0xee38, 0x11bc, 0xee44, 0x11b0, 0xee50, 0x11a4, 0xee5c,
		0x1198, 0xee68, 0x118c, 0xee74, 0x1180, 0xee80, 0x1174, 0xee8c,
		0x1168, 0xee98, 0x115c, 0xeea4, 0x1150, 0xeeb0, 0x1144, 0xeebc,
		0x1138, 0xeec8, 0x112c, 0xeed4, 0x1120, 0xeee0, 0x1114, 0xeeec,
		0x1108, 0xeef8, 0x10fc, 0xef04, 0x10f0, 0xef10, 0x10e4, 0xef1c,
		0x10d8, 0xef28, 0x10cc, 0xef34, 0x10c0, 0xef40, 0x10b4, 0xef4c,
		0x10a8, 0xef58, 0x10a0, 0xef60, 0x1094, 0xef6c, 0x1088, 0xef78,
		0x107c, 0xef84, 0x1070, 0xef90, 0x1064, 0xef9c, 0x1058, 0xefa8,
		0x1050, 0xefb0, 0x1044, 0xefbc, 0x1038, 0xefc8, 0x102c, 0xefd4,
		0x1020, 0xefe0, 0x1018, 0xefe8, 0x100c, 0xeff4, 0x1000, 0xf000,
		0x0ff4, 0xf00c, 0x0fea, 0xf016, 0x0fde, 0xf022, 0x0fd4, 0xf02c,
		0x0fc8, 0xf038, 0x0fbe, 0xf042, 0x0fb4, 0xf04c, 0x0fa8, 0xf058,
		0x0f9e, 0xf062, 0x0f92, 0xf06e, 0x0f88, 0xf078, 0x0f7e, 0xf082,
		0x0f72, 0xf08e, 0x0f68, 0xf098, 0x0f5c, 0xf0a4, 0x0f52, 0xf0ae,
		0x0f48, 0xf0b8, 0x0f3e, 0xf0c2, 0x0f32, 0xf0ce, 0x0f28, 0xf0d8,
		0x0f1e, 0xf0e2, 0x0f14, 0xf0ec, 0x0f08, 0xf0f8, 0x0efe, 0xf102,
		0x0ef4, 0xf10c, 0x0eea, 0xf116, 0x0ee0, 0xf120, 0x0ed4, 0xf12c,
		0x0eca, 0xf136, 0x0ec0, 0xf140, 0x0eb6, 0xf14a, 0x0eac, 0xf154,
		0x0ea2, 0xf15e, 0x0e98, 0xf168, 0x0e8e, 0xf172, 0x0e84, 0xf17c,
		0x0e7a, 0xf186, 0x0e70, 0xf190, 0x0e66, 0xf19a, 0x0e5c, 0xf1a4,
		0x0e52, 0xf1ae, 0x0e48, 0xf1b8, 0x0e3e, 0xf1c2, 0x0e34, 0xf1cc,
		0x0e2a, 0xf1d6, 0x0e20, 0xf1e0, 0x0e16, 0xf1ea, 0x0e0c, 0xf1f4,
		0x0e04, 0xf1fc, 0x0dfa, 0xf206, 0x0df0, 0xf210, 0x0de6, 0xf21a,
		0x0ddc, 0xf224, 0x0dd2, 0xf22e, 0x0dca, 0xf236, 0x0dc0, 0xf240,
		0x0db6, 0xf24a, 0x0dac, 0xf254, 0x0da4, 0xf25c, 0x0d9a, 0xf266,
		0x0d90, 0xf270, 0x0d88, 0xf278, 0x0d7e, 0xf282, 0x0d74, 0xf28c,
		0x0d6a, 0xf296, 0x0d62, 0xf29e, 0x0d58, 0xf2a8, 0x0d50, 0xf2b0,
		0x0d46, 0xf2ba, 0x0d3c, 0xf2c4, 0x0d34, 0xf2cc, 0x0d2a, 0xf2d6,
		0x0d22, 0xf2de, 0x0d18, 0xf2e8, 0x0d10, 0xf2f0, 0x0d06, 0xf2fa,
		0x0cfe, 0xf302, 0x0cf4, 0xf30c, 0x0cec, 0xf314, 0x0ce2, 0xf31e,
		0x0cda, 0xf326, 0x0cd0, 0xf330, 0x0cc8, 0xf338, 0x0cbe, 0xf342,
		0x0cb6, 0xf34a, 0x0cae, 0xf352, 0x0ca4, 0xf35c, 0x0c9c, 0xf364,
		0x0c92, 0xf36e, 0x0c8a, 0xf376, 0x0c82, 0xf37e, 0x0c78, 0xf388,
		0x0c70, 0xf390, 0x0c68, 0xf398, 0x0c60, 0xf3a0, 0x0c56, 0xf3aa,
		0x0c4e, 0xf3b2, 0x0c46, 0xf3ba, 0x0c3c, 0xf3c4, 0x0c34, 0xf3cc,
		0x0c2c, 0xf3d4, 0x0c24, 0xf3dc, 0x0c1c, 0xf3e4, 0x0c12, 0xf3ee,
		0x0c0a, 0xf3f6, 0x0c02, 0xf3fe, 0x0bfa, 0xf406, 0x0bf2, 0xf40e,
		0x0bea, 0xf416, 0x0be0, 0xf420, 0x0bd8, 0xf428, 0x0bd0, 0xf430,
		0x0bc8, 0xf438, 0x0bc0, 0xf440, 0x0bb8, 0xf448, 0x0bb0, 0xf450,
		0x0ba8, 0xf458, 0x0ba0, 0xf460, 0x0b98, 0xf468, 0x0b90, 0xf470,
		0x0b88, 0xf478, 0x0b80, 0xf480, 0x0b78, 0xf488, 0x0b70, 0xf490,
		0x0b68, 0xf498, 0x0b60, 0xf4a0, 0x0b58, 0xf4a8, 0x0b50, 0xf4b0,
		0x0b48, 0xf4b8, 0x0b40, 0xf4c0, 0x0b38, 0xf4c8, 0x0b32, 0xf4ce,
		0x0b2a, 0xf4d6, 0x0b22, 0xf4de, 0x0b1a, 0xf4e6, 0x0b12, 0xf4ee,
		0x0b0a, 0xf4f6, 0x0b02, 0xf4fe, 0x0afc, 0xf504, 0x0af4, 0xf50c,
		0x0aec, 0xf514, 0x0ae4, 0xf51c, 0x0ade, 0xf522, 0x0ad6, 0xf52a,
		0x0ace, 0xf532, 0x0ac6, 0xf53a, 0x0ac0, 0xf540, 0x0ab8, 0xf548,
		0x0ab0, 0xf550, 0x0aa8, 0xf558, 0x0aa2, 0xf55e, 0x0a9a, 0xf566,
		0x0a92, 0xf56e, 0x0a8c, 0xf574, 0x0a84, 0xf57c, 0x0a7c, 0xf584,
		0x0a76, 0xf58a, 0x0a6e, 0xf592, 0x0a68, 0xf598, 0x0a60, 0xf5a0,
		0x0a58, 0xf5a8, 0x0a52, 0xf5ae, 0x0a4a, 0xf5b6, 0x0a44, 0xf5bc,
		0x0a3c, 0xf5c4, 0x0a36, 0xf5ca, 0x0a2e, 0xf5d2, 0x0a28, 0xf5d8,
		0x0a20, 0xf5e0, 0x0a18, 0xf5e8, 0x0a12, 0xf5ee, 0x0a0c, 0xf5f4,
		0x0a04, 0xf5fc, 0x09fe, 0xf602, 0x09f6, 0xf60a, 0x09f0, 0xf610,
		0x09e8, 0xf618, 0x09e2, 0xf61e, 0x09da, 0xf626, 0x09d4, 0xf62c,
		0x09ce, 0xf632, 0x09c6, 0xf63a, 0x09c0, 0xf640, 0x09b8, 0xf648,
		0x09b2, 0xf64e, 0x09ac, 0xf654, 0x09a4, 0xf65c, 0x099e, 0xf662,
		0x0998, 0xf668, 0x0990, 0xf670, 0x098a, 0xf676, 0x0984, 0xf67c,
		0x097c, 0xf684, 0x0976, 0xf68a, 0x0970, 0xf690, 0x096a, 0xf696,
		0x0962, 0xf69e, 0x095c, 0xf6a4, 0x0956, 0xf6aa, 0x0950, 0xf6b0,
		0x0948, 0xf6b8, 0x0942, 0xf6be, 0x093c, 0xf6c4, 0x0936, 0xf6ca,
		0x0930, 0xf6d0, 0x0928, 0xf6d8, 0x0922, 0xf6de, 0x091c, 0xf6e4,
		0x0916, 0xf6ea, 0x0910, 0xf6f0, 0x090a, 0xf6f6, 0x0904, 0xf6fc,
		0x08fc, 0xf704, 0x08f6, 0xf70a, 0x08f0, 0xf710, 0x08ea, 0xf716,
		0x08e4, 0xf71c, 0x08de, 0xf722, 0x08d8, 0xf728, 0x08d2, 0xf72e,
		0x08cc, 0xf734, 0x08c6, 0xf73a, 0x08c0, 0xf740, 0x08ba, 0xf746,
		0x08b4, 0xf74c, 0x08ae, 0xf752, 0x08a8, 0xf758, 0x08a2, 0xf75e,
		0x089c, 0xf764, 0x0896, 0xf76a, 0x0890, 0xf770, 0x088a, 0xf776,
		0x0884, 0xf77c, 0x087e, 0xf782, 0x0878, 0xf788, 0x0872, 0xf78e,
		0x086c, 0xf794, 0x0866, 0xf79a, 0x0860, 0xf7a0, 0x085a, 0xf7a6,
		0x0854, 0xf7ac, 0x0850, 0xf7b0, 0x084a, 0xf7b6, 0x0844, 0xf7bc,
		0x083e, 0xf7c2, 0x0838, 0xf7c8, 0x0832, 0xf7ce, 0x082c, 0xf7d4,
		0x0828, 0xf7d8, 0x0822, 0xf7de, 0x081c, 0xf7e4, 0x0816, 0xf7ea,
		0x0810, 0xf7f0, 0x080c, 0xf7f4, 0x0806, 0xf7fa, 0x0800, 0xf800,
		0x07fa, 0xf806, 0x07f5, 0xf80b, 0x07ef, 0xf811, 0x07ea, 0xf816,
		0x07e4, 0xf81c, 0x07df, 0xf821, 0x07da, 0xf826, 0x07d4, 0xf82c,
		0x07cf, 0xf831, 0x07c9, 0xf837, 0x07c4, 0xf83c, 0x07bf, 0xf841,
		0x07b9, 0xf847, 0x07b4, 0xf84c, 0x07ae, 0xf852, 0x07a9, 0xf857,
		0x07a4, 0xf85c, 0x079f, 0xf861, 0x0799, 0xf867, 0x0794, 0xf86c,
		0x078f, 0xf871, 0x078a, 0xf876, 0x0784, 0xf87c, 0x077f, 0xf881,
		0x077a, 0xf886, 0x0775, 0xf88b, 0x0770, 0xf890, 0x076a, 0xf896,
		0x0765, 0xf89b, 0x0760, 0xf8a0, 0x075b, 0xf8a5, 0x0756, 0xf8aa,
		0x0751, 0xf8af, 0x074c, 0xf8b4, 0x0747, 0xf8b9, 0x0742, 0xf8be,
		0x073d, 0xf8c3, 0x0738, 0xf8c8, 0x0733, 0xf8cd, 0x072e, 0xf8d2,
		0x0729, 0xf8d7, 0x0724, 0xf8dc, 0x071f, 0xf8e1, 0x071a, 0xf8e6,
		0x0715, 0xf8eb, 0x0710, 0xf8f0, 0x070b, 0xf8f5, 0x0706, 0xf8fa,
		0x0702, 0xf8fe, 0x06fd, 0xf903, 0x06f8, 0xf908, 0x06f3, 0xf90d,
		0x06ee, 0xf912, 0x06e9, 0xf917, 0x06e5, 0xf91b, 0x06e0, 0xf920,
		0x06db, 0xf925, 0x06d6, 0xf92a, 0x06d2, 0xf92e, 0x06cd, 0xf933,
		0x06c8, 0xf938, 0x06c4, 0xf93c, 0x06bf, 0xf941, 0x06ba, 0xf946,
		0x06b5, 0xf94b, 0x06b1, 0xf94f, 0x06ac, 0xf954, 0x06a8, 0xf958,
		0x06a3, 0xf95d, 0x069e, 0xf962, 0x069a, 0xf966, 0x0695, 0xf96b,
		0x0691, 0xf96f, 0x068c, 0xf974, 0x0688, 0xf978, 0x0683, 0xf97d,
		0x067f, 0xf981, 0x067a, 0xf986, 0x0676, 0xf98a, 0x0671, 0xf98f,
		0x066d, 0xf993, 0x0668, 0xf998, 0x0664, 0xf99c, 0x065f, 0xf9a1,
		0x065b, 0xf9a5, 0x0657, 0xf9a9, 0x0652, 0xf9ae, 0x064e, 0xf9b2,
		0x0649, 0xf9b7, 0x0645, 0xf9bb, 0x0641, 0xf9bf, 0x063c, 0xf9c4,
		0x0638, 0xf9c8, 0x0634, 0xf9cc, 0x0630, 0xf9d0, 0x062b, 0xf9d5,
		0x0627, 0xf9d9, 0x0623, 0xf9dd, 0x061e, 0xf9e2, 0x061a, 0xf9e6,
		0x0616, 0xf9ea, 0x0612, 0xf9ee, 0x060e, 0xf9f2, 0x0609, 0xf9f7,
		0x0605, 0xf9fb, 0x0601, 0xf9ff, 0x05fd, 0xfa03, 0x05f9, 0xfa07,
		0x05f5, 0xfa0b, 0x05f0, 0xfa10, 0x05ec, 0xfa14, 0x05e8, 0xfa18,
		0x05e4, 0xfa1c, 0x05e0, 0xfa20, 0x05dc, 0xfa24, 0x05d8, 0xfa28,
		0x05d4, 0xfa2c, 0x05d0, 0xfa30, 0x05cc, 0xfa34, 0x05c8, 0xfa38,
		0x05c4, 0xfa3c, 0x05c0, 0xfa40, 0x05bc, 0xfa44, 0x05b8, 0xfa48,
		0x05b4, 0xfa4c, 0x05b0, 0xfa50, 0x05ac, 0xfa54, 0x05a8, 0xfa58,
		0x05a4, 0xfa5c, 0x05a0, 0xfa60, 0x059c, 0xfa64, 0x0599, 0xfa67,
		0x0595, 0xfa6b, 0x0591, 0xfa6f, 0x058d, 0xfa73, 0x0589, 0xfa77,
		0x0585, 0xfa7b, 0x0581, 0xfa7f, 0x057e, 0xfa82, 0x057a, 0xfa86,
		0x0576, 0xfa8a, 0x0572, 0xfa8e, 0x056f, 0xfa91, 0x056b, 0xfa95,
		0x0567, 0xfa99, 0x0563, 0xfa9d, 0x0560, 0xfaa0, 0x055c, 0xfaa4,
		0x0558, 0xfaa8, 0x0554, 0xfaac, 0x0551, 0xfaaf, 0x054d, 0xfab3,
		0x0549, 0xfab7, 0x0546, 0xfaba, 0x0542, 0xfabe, 0x053e, 0xfac2,
		0x053b, 0xfac5, 0x0537, 0xfac9, 0x0534, 0xfacc, 0x0530, 0xfad0,
		0x052c, 0xfad4, 0x0529, 0xfad7, 0x0525, 0xfadb, 0x0522, 0xfade,
		0x051e, 0xfae2, 0x051b, 0xfae5, 0x0517, 0xfae9, 0x0514, 0xfaec,
		0x0510, 0xfaf0, 0x050c, 0xfaf4, 0x0509, 0xfaf7, 0x0506, 0xfafa,
		0x0502, 0xfafe, 0x04ff, 0xfb01, 0x04fb, 0xfb05, 0x04f8, 0xfb08,
		0x04f4, 0xfb0c, 0x04f1, 0xfb0f, 0x04ed, 0xfb13, 0x04ea, 0xfb16,
		0x04e7, 0xfb19, 0x04e3, 0xfb1d, 0x04e0, 0xfb20, 0x04dc, 0xfb24,
		0x04d9, 0xfb27, 0x04d6, 0xfb2a, 0x04d2, 0xfb2e, 0x04cf, 0xfb31,
		0x04cc, 0xfb34, 0x04c8, 0xfb38, 0x04c5, 0xfb3b, 0x04c2, 0xfb3e,
		0x04be, 0xfb42, 0x04bb, 0xfb45, 0x04b8, 0xfb48, 0x04b5, 0xfb4b,
		0x04b1, 0xfb4f, 0x04ae, 0xfb52, 0x04ab, 0xfb55, 0x04a8, 0xfb58,
		0x04a4, 0xfb5c, 0x04a1, 0xfb5f, 0x049e, 0xfb62, 0x049b, 0xfb65,
		0x0498, 0xfb68, 0x0494, 0xfb6c, 0x0491, 0xfb6f, 0x048e, 0xfb72,
		0x048b, 0xfb75, 0x0488, 0xfb78, 0x0485, 0xfb7b, 0x0482, 0xfb7e,
		0x047e, 0xfb82, 0x047b, 0xfb85, 0x0478, 0xfb88, 0x0475, 0xfb8b,
		0x0472, 0xfb8e, 0x046f, 0xfb91, 0x046c, 0xfb94, 0x0469, 0xfb97,
		0x0466, 0xfb9a, 0x0463, 0xfb9d, 0x0460, 0xfba0, 0x045d, 0xfba3,
		0x045a, 0xfba6, 0x0457, 0xfba9, 0x0454, 0xfbac, 0x0451, 0xfbaf,
		0x044e, 0xfbb2, 0x044b, 0xfbb5, 0x0448, 0xfbb8, 0x0445, 0xfbbb,
		0x0442, 0xfbbe, 0x043f, 0xfbc1, 0x043c, 0xfbc4, 0x0439, 0xfbc7,
		0x0436, 0xfbca, 0x0433, 0xfbcd, 0x0430, 0xfbd0, 0x042d, 0xfbd3,
		0x042a, 0xfbd6, 0x0428, 0xfbd8, 0x0425, 0xfbdb, 0x0422, 0xfbde,
		0x041f, 0xfbe1, 0x041c, 0xfbe4, 0x0419, 0xfbe7, 0x0416, 0xfbea,
		0x0414, 0xfbec, 0x0411, 0xfbef, 0x040e, 0xfbf2, 0x040b, 0xfbf5,
		0x0408, 0xfbf8, 0x0406, 0xfbfa, 0x0403, 0xfbfd, 0x0400, 0xfc00,
		0x03fd, 0xfc03, 0x03fa, 0xfc06, 0x03f7, 0xfc09, 0x03f5, 0xfc0b,
		0x03f2, 0xfc0e, 0x03ef, 0xfc11, 0x03ed, 0xfc13, 0x03ea, 0xfc16,
		0x03e7, 0xfc19, 0x03e4, 0xfc1c, 0x03e2, 0xfc1e, 0x03df, 0xfc21,
		0x03dc, 0xfc24, 0x03da, 0xfc26, 0x03d7, 0xfc29, 0x03d4, 0xfc2c,
		0x03d2, 0xfc2e, 0x03cf, 0xfc31, 0x03cc, 0xfc34, 0x03ca, 0xfc36,
		0x03c7, 0xfc39, 0x03c5, 0xfc3b, 0x03c2, 0xfc3e, 0x03bf, 0xfc41,
		0x03bd, 0xfc43, 0x03ba, 0xfc46, 0x03b8, 0xfc48, 0x03b5, 0xfc4b,
		0x03b2, 0xfc4e, 0x03b0, 0xfc50, 0x03ad, 0xfc53, 0x03ab, 0xfc55,
		0x03a8, 0xfc58, 0x03a6, 0xfc5a, 0x03a3, 0xfc5d, 0x03a1, 0xfc5f,
		0x039e, 0xfc62, 0x039c, 0xfc64, 0x0399, 0xfc67, 0x0397, 0xfc69,
		0x0394, 0xfc6c, 0x0392, 0xfc6e, 0x038f, 0xfc71, 0x038d, 0xfc73,
		0x038a, 0xfc76, 0x0388, 0xfc78, 0x0385, 0xfc7b, 0x0383, 0xfc7d,
		0x0381, 0xfc7f, 0x037e, 0xfc82, 0x037c, 0xfc84, 0x0379, 0xfc87,
		0x0377, 0xfc89, 0x0374, 0xfc8c, 0x0372, 0xfc8e, 0x0370, 0xfc90,
		0x036d, 0xfc93, 0x036b, 0xfc95, 0x0369, 0xfc97, 0x0366, 0xfc9a,
		0x0364, 0xfc9c, 0x0362, 0xfc9e, 0x035f, 0xfca1, 0x035d, 0xfca3,
		0x035a, 0xfca6, 0x0358, 0xfca8, 0x0356, 0xfcaa, 0x0354, 0xfcac,
		0x0351, 0xfcaf, 0x034f, 0xfcb1, 0x034d, 0xfcb3, 0x034a, 0xfcb6,
		0x0348, 0xfcb8, 0x0346, 0xfcba, 0x0344, 0xfcbc, 0x0341, 0xfcbf,
		0x033f, 0xfcc1, 0x033d, 0xfcc3, 0x033b, 0xfcc5, 0x0338, 0xfcc8,
		0x0336, 0xfcca, 0x0334, 0xfccc, 0x0332, 0xfcce, 0x032f, 0xfcd1,
		0x032d, 0xfcd3, 0x032b, 0xfcd5, 0x0329, 0xfcd7, 0x0327, 0xfcd9,
		0x0324, 0xfcdc, 0x0322, 0xfcde, 0x0320, 0xfce0, 0x031e, 0xfce2,
		0x031c, 0xfce4, 0x031a, 0xfce6, 0x0318, 0xfce8, 0x0315, 0xfceb,
		0x0313, 0xfced, 0x0311, 0xfcef, 0x030f, 0xfcf1, 0x030d, 0xfcf3,
		0x030b, 0xfcf5, 0x0309, 0xfcf7, 0x0307, 0xfcf9, 0x0304, 0xfcfc,
		0x0302, 0xfcfe, 0x0300, 0xfd00, 0x02fe, 0xfd02, 0x02fc, 0xfd04,
		0x02fa, 0xfd06, 0x02f8, 0xfd08, 0x02f6, 0xfd0a, 0x02f4, 0xfd0c,
		0x02f2, 0xfd0e, 0x02f0, 0xfd10, 0x02ee, 0xfd12, 0x02ec, 0xfd14,
		0x02ea, 0xfd16, 0x02e8, 0xfd18, 0x02e6, 0xfd1a, 0x02e4, 0xfd1c,
		0x02e2, 0xfd1e, 0x02e0, 0xfd20, 0x02de, 0xfd22, 0x02dc, 0xfd24,
		0x02da, 0xfd26, 0x02d8, 0xfd28, 0x02d6, 0xfd2a, 0x02d4, 0xfd2c,
		0x02d2, 0xfd2e, 0x02d0, 0xfd30, 0x02ce, 0xfd32, 0x02cc, 0xfd34,
		0x02ca, 0xfd36, 0x02c8, 0xfd38, 0x02c6, 0xfd3a, 0x02c4, 0xfd3c,
		0x02c2, 0xfd3e, 0x02c0, 0xfd40, 0x02bf, 0xfd41, 0x02bd, 0xfd43,
		0x02bb, 0xfd45, 0x02b9, 0xfd47, 0x02b7, 0xfd49, 0x02b5, 0xfd4b,
		0x02b3, 0xfd4d, 0x02b1, 0xfd4f, 0x02b0, 0xfd50, 0x02ae, 0xfd52,
		0x02ac, 0xfd54, 0x02aa, 0xfd56, 0x02a8, 0xfd58, 0x02a6, 0xfd5a,
		0x02a4, 0xfd5c, 0x02a3, 0xfd5d, 0x02a1, 0xfd5f, 0x029f, 0xfd61,
		0x029d, 0xfd63, 0x029b, 0xfd65, 0x029a, 0xfd66, 0x0298, 0xfd68,
		0x0296, 0xfd6a, 0x0294, 0xfd6c, 0x0292, 0xfd6e, 0x0291, 0xfd6f,
		0x028f, 0xfd71, 0x028d, 0xfd73, 0x028b, 0xfd75, 0x028a, 0xfd76,
		0x0288, 0xfd78, 0x0286, 0xfd7a, 0x0284, 0xfd7c, 0x0283, 0xfd7d,
		0x0281, 0xfd7f, 0x027f, 0xfd81, 0x027d, 0xfd83, 0x027c, 0xfd84,
		0x027a, 0xfd86, 0x0278, 0xfd88, 0x0276, 0xfd8a, 0x0275, 0xfd8b,
		0x0273, 0xfd8d, 0x0271, 0xfd8f, 0x0270, 0xfd90, 0x026e, 0xfd92,
		0x026c, 0xfd94, 0x026b, 0xfd95, 0x0269, 0xfd97, 0x0267, 0xfd99,
		0x0266, 0xfd9a, 0x0264, 0xfd9c, 0x0262, 0xfd9e, 0x0261, 0xfd9f,
		0x025f, 0xfda1, 0x025d, 0xfda3, 0x025c, 0xfda4, 0x025a, 0xfda6,
		0x0258, 0xfda8, 0x0257, 0xfda9, 0x0255, 0xfdab, 0x0254, 0xfdac,
		0x0252, 0xfdae, 0x0250, 0xfdb0, 0x024f, 0xfdb1, 0x024d, 0xfdb3,
		0x024c, 0xfdb4, 0x024a, 0xfdb6, 0x0248, 0xfdb8, 0x0247, 0xfdb9,
		0x0245, 0xfdbb, 0x0244, 0xfdbc, 0x0242, 0xfdbe, 0x0241, 0xfdbf,
		0x023f, 0xfdc1, 0x023d, 0xfdc3, 0x023c, 0xfdc4, 0x023a, 0xfdc6,
		0x0239, 0xfdc7, 0x0237, 0xfdc9, 0x0236, 0xfdca, 0x0234, 0xfdcc,
		0x0233, 0xfdcd, 0x0231, 0xfdcf, 0x0230, 0xfdd0, 0x022e, 0xfdd2,
		0x022d, 0xfdd3, 0x022b, 0xfdd5, 0x022a, 0xfdd6, 0x0228, 0xfdd8,
		0x0227, 0xfdd9, 0x0225, 0xfddb, 0x0224, 0xfddc, 0x0222, 0xfdde,
		0x0221, 0xfddf, 0x021f, 0xfde1, 0x021e, 0xfde2, 0x021c, 0xfde4,
		0x021b, 0xfde5, 0x0219, 0xfde7, 0x0218, 0xfde8, 0x0216, 0xfdea,
		0x0215, 0xfdeb, 0x0214, 0xfdec, 0x0212, 0xfdee, 0x0211, 0xfdef,
		0x020f, 0xfdf1, 0x020e, 0xfdf2, 0x020c, 0xfdf4, 0x020b, 0xfdf5,
		0x020a, 0xfdf6, 0x0208, 0xfdf8, 0x0207, 0xfdf9, 0x0205, 0xfdfb,
		0x0204, 0xfdfc, 0x0203, 0xfdfd, 0x0201, 0xfdff, 0x0200, 0xfe00,
		0x01fe, 0xfe02, 0x01fd, 0xfe03, 0x01fb, 0xfe05, 0x01fa, 0xfe06,
		0x01f9, 0xfe07, 0x01f7, 0xfe09, 0x01f6, 0xfe0a, 0x01f5, 0xfe0b,
		0x01f3, 0xfe0d, 0x01f2, 0xfe0e, 0x01f1, 0xfe0f, 0x01ef, 0xfe11,
		0x01ee, 0xfe12, 0x01ed, 0xfe13, 0x01eb, 0xfe15, 0x01ea, 0xfe16,
		0x01e9, 0xfe17, 0x01e7, 0xfe19, 0x01e6, 0xfe1a, 0x01e5, 0xfe1b,
		0x01e3, 0xfe1d, 0x01e2, 0xfe1e, 0x01e1, 0xfe1f, 0x01df, 0xfe21,
		0x01de, 0xfe22, 0x01dd, 0xfe23, 0x01dc, 0xfe24, 0x01da, 0xfe26,
		0x01d9, 0xfe27, 0x01d8, 0xfe28, 0x01d6, 0xfe2a, 0x01d5, 0xfe2b,
		0x01d4, 0xfe2c, 0x01d3, 0xfe2d, 0x01d1, 0xfe2f, 0x01d0, 0xfe30,
		0x01cf, 0xfe31, 0x01ce, 0xfe32, 0x01cc, 0xfe34, 0x01cb, 0xfe35,
		0x01ca, 0xfe36, 0x01c9, 0xfe37, 0x01c7, 0xfe39, 0x01c6, 0xfe3a,
		0x01c5, 0xfe3b, 0x01c4, 0xfe3c, 0x01c2, 0xfe3e, 0x01c1, 0xfe3f,
		0x01c0, 0xfe40, 0x01bf, 0xfe41, 0x01be, 0xfe42, 0x01bc, 0xfe44,
		0x01bb, 0xfe45, 0x01ba, 0xfe46, 0x01b9, 0xfe47, 0x01b8, 0xfe48,
		0x01b6, 0xfe4a, 0x01b5, 0xfe4b, 0x01b4, 0xfe4c, 0x01b3, 0xfe4d,
		0x01b2, 0xfe4e, 0x01b1, 0xfe4f, 0x01af, 0xfe51, 0x01ae, 0xfe52,
		0x01ad, 0xfe53, 0x01ac, 0xfe54, 0x01ab, 0xfe55, 0x01aa, 0xfe56,
		0x01a8, 0xfe58, 0x01a7, 0xfe59, 0x01a6, 0xfe5a, 0x01a5, 0xfe5b,
		0x01a4, 0xfe5c, 0x01a3, 0xfe5d, 0x01a2, 0xfe5e, 0x01a0, 0xfe60,
		0x019f, 0xfe61, 0x019e, 0xfe62, 0x019d, 0xfe63, 0x019c, 0xfe64,
		0x019b, 0xfe65, 0x019a, 0xfe66, 0x0199, 0xfe67, 0x0197, 0xfe69,
		0x0196, 0xfe6a, 0x0195, 0xfe6b, 0x0194, 0xfe6c, 0x0193, 0xfe6d,
		0x0192, 0xfe6e, 0x0191, 0xfe6f, 0x0190, 0xfe70, 0x018f, 0xfe71,
		0x018e, 0xfe72, 0x018d, 0xfe73, 0x018c, 0xfe74, 0x018a, 0xfe76,
		0x0189, 0xfe77, 0x0188, 0xfe78, 0x0187, 0xfe79, 0x0186, 0xfe7a,
		0x0185, 0xfe7b, 0x0184, 0xfe7c, 0x0183, 0xfe7d, 0x0182, 0xfe7e,
		0x0181, 0xfe7f, 0x0180, 0xfe80, 0x017f, 0xfe81, 0x017e, 0xfe82,
		0x017d, 0xfe83, 0x017c, 0xfe84, 0x017b, 0xfe85, 0x017a, 0xfe86,
		0x0179, 0xfe87, 0x0178, 0xfe88, 0x0177, 0xfe89, 0x0176, 0xfe8a,
		0x0175, 0xfe8b, 0x0174, 0xfe8c, 0x0173, 0xfe8d, 0x0172, 0xfe8e,
		0x0171, 0xfe8f, 0x0170, 0xfe90, 0x016f, 0xfe91, 0x016e, 0xfe92,
		0x016d, 0xfe93, 0x016c, 0xfe94, 0x016b, 0xfe95, 0x016a, 0xfe96,
		0x0169, 0xfe97, 0x0168, 0xfe98, 0x0167, 0xfe99, 0x0166, 0xfe9a,
		0x0165, 0xfe9b, 0x0164, 0xfe9c, 0x0163, 0xfe9d, 0x0162, 0xfe9e,
		0x0161, 0xfe9f, 0x0160, 0xfea0, 0x015f, 0xfea1, 0x015e, 0xfea2,
		0x015d, 0xfea3, 0x015c, 0xfea4, 0x015b, 0xfea5, 0x015a, 0xfea6,
		0x0159, 0xfea7, 0x0158, 0xfea8, 0x0158, 0xfea8, 0x0157, 0xfea9,
		0x0156, 0xfeaa, 0x0155, 0xfeab, 0x0154, 0xfeac, 0x0153, 0xfead,
		0x0152, 0xfeae, 0x0151, 0xfeaf, 0x0150, 0xfeb0, 0x014f, 0xfeb1,
		0x014e, 0xfeb2, 0x014d, 0xfeb3, 0x014d, 0xfeb3, 0x014c, 0xfeb4,
		0x014b, 0xfeb5, 0x014a, 0xfeb6, 0x0149, 0xfeb7, 0x0148, 0xfeb8,
		0x0147, 0xfeb9, 0x0146, 0xfeba, 0x0145, 0xfebb, 0x0145, 0xfebb,
		0x0144, 0xfebc, 0x0143, 0xfebd, 0x0142, 0xfebe, 0x0141, 0xfebf,
		0x0140, 0xfec0, 0x013f, 0xfec1, 0x013e, 0xfec2, 0x013e, 0xfec2,
		0x013d, 0xfec3, 0x013c, 0xfec4, 0x013b, 0xfec5, 0x013a, 0xfec6,
		0x0139, 0xfec7, 0x0138, 0xfec8, 0x0138, 0xfec8, 0x0137, 0xfec9,
		0x0136, 0xfeca, 0x0135, 0xfecb, 0x0134, 0xfecc, 0x0133, 0xfecd,
		0x0133, 0xfecd, 0x0132, 0xfece, 0x0131, 0xfecf, 0x0130, 0xfed0,
		0x012f, 0xfed1, 0x012e, 0xfed2, 0x012e, 0xfed2, 0x012d, 0xfed3,
		0x012c, 0xfed4, 0x012b, 0xfed5, 0x012a, 0xfed6, 0x012a, 0xfed6,
		0x0129, 0xfed7, 0x0128, 0xfed8, 0x0127, 0xfed9, 0x0126, 0xfeda,
		0x0126, 0xfeda, 0x0125, 0xfedb, 0x0124, 0xfedc, 0x0123, 0xfedd,
		0x0122, 0xfede, 0x0122, 0xfede, 0x0121, 0xfedf, 0x0120, 0xfee0,
		0x011f, 0xfee1, 0x011e, 0xfee2, 0x011e, 0xfee2, 0x011d, 0xfee3,
		0x011c, 0xfee4, 0x011b, 0xfee5, 0x011b, 0xfee5, 0x011a, 0xfee6,
		0x0119, 0xfee7, 0x0118, 0xfee8, 0x0118, 0xfee8, 0x0117, 0xfee9,
		0x0116, 0xfeea, 0x0115, 0xfeeb, 0x0115, 0xfeeb, 0x0114, 0xfeec,
		0x0113, 0xfeed, 0x0112, 0xfeee, 0x0112, 0xfeee, 0x0111, 0xfeef,
		0x0110, 0xfef0, 0x010f, 0xfef1, 0x010f, 0xfef1, 0x010e, 0xfef2,
		0x010d, 0xfef3, 0x010c, 0xfef4, 0x010c, 0xfef4, 0x010b, 0xfef5,
		0x010a, 0xfef6, 0x010a, 0xfef6, 0x0109, 0xfef7, 0x0108, 0xfef8,
		0x0107, 0xfef9, 0x0107, 0xfef9, 0x0106, 0xfefa, 0x0105, 0xfefb,
		0x0105, 0xfefb, 0x0104, 0xfefc, 0x0103, 0xfefd, 0x0102, 0xfefe,
		0x0102, 0xfefe, 0x0101, 0xfeff, 0x0100, 0xff00, 0x0100, 0xff00,
		0x00ff, 0xff01, 0x00fe, 0xff02, 0x00fd, 0xff03, 0x00fd, 0xff03,
		0x00fc, 0xff04, 0x00fb, 0xff05, 0x00fb, 0xff05, 0x00fa, 0xff06,
		0x00f9, 0xff07, 0x00f9, 0xff07, 0x00f8, 0xff08, 0x00f7, 0xff09,
		0x00f7, 0xff09, 0x00f6, 0xff0a, 0x00f5, 0xff0b, 0x00f5, 0xff0b,
		0x00f4, 0xff0c, 0x00f3, 0xff0d, 0x00f3, 0xff0d, 0x00f2, 0xff0e,
		0x00f1, 0xff0f, 0x00f1, 0xff0f, 0x00f0, 0xff10, 0x00ef, 0xff11,
		0x00ef, 0xff11, 0x00ee, 0xff12, 0x00ee, 0xff12, 0x00ed, 0xff13,
		0x00ec, 0xff14, 0x00ec, 0xff14, 0x00eb, 0xff15, 0x00ea, 0xff16,
		0x00ea, 0xff16, 0x00e9, 0xff17, 0x00e8, 0xff18, 0x00e8, 0xff18,
		0x00e7, 0xff19, 0x00e7, 0xff19, 0x00e6, 0xff1a, 0x00e5, 0xff1b,
		0x00e5, 0xff1b, 0x00e4, 0xff1c, 0x00e3, 0xff1d, 0x00e3, 0xff1d,
		0x00e2, 0xff1e, 0x00e2, 0xff1e, 0x00e1, 0xff1f, 0x00e0, 0xff20,
		0x00e0, 0xff20, 0x00df, 0xff21, 0x00df, 0xff21, 0x00de, 0xff22,
		0x00dd, 0xff23, 0x00dd, 0xff23, 0x00dc, 0xff24, 0x00dc, 0xff24,
		0x00db, 0xff25, 0x00da, 0xff26, 0x00da, 0xff26, 0x00d9, 0xff27,
		0x00d9, 0xff27, 0x00d8, 0xff28, 0x00d7, 0xff29, 0x00d7, 0xff29,
		0x00d6, 0xff2a, 0x00d6, 0xff2a, 0x00d5, 0xff2b, 0x00d5, 0xff2b,
		0x00d4, 0xff2c, 0x00d3, 0xff2d, 0x00d3, 0xff2d, 0x00d2, 0xff2e,
		0x00d2, 0xff2e, 0x00d1, 0xff2f, 0x00d1, 0xff2f, 0x00d0, 0xff30,
		0x00cf, 0xff31, 0x00cf, 0xff31, 0x00ce, 0xff32, 0x00ce, 0xff32,
		0x00cd, 0xff33, 0x00cd, 0xff33, 0x00cc, 0xff34, 0x00cb, 0xff35,
		0x00cb, 0xff35, 0x00ca, 0xff36, 0x00ca, 0xff36, 0x00c9, 0xff37,
		0x00c9, 0xff37, 0x00c8, 0xff38, 0x00c8, 0xff38, 0x00c7, 0xff39,
		0x00c7, 0xff39, 0x00c6, 0xff3a, 0x00c6, 0xff3a, 0x00c5, 0xff3b,
		0x00c4, 0xff3c, 0x00c4, 0xff3c, 0x00c3, 0xff3d, 0x00c3, 0xff3d,
		0x00c2, 0xff3e, 0x00c2, 0xff3e, 0x00c1, 0xff3f, 0x00c1, 0xff3f,
		0x00c0, 0xff40, 0x00c0, 0xff40, 0x00bf, 0xff41, 0x00bf, 0xff41,
		0x00be, 0xff42, 0x00be, 0xff42, 0x00bd, 0xff43, 0x00bd, 0xff43,
		0x00bc, 0xff44, 0x00bc, 0xff44, 0x00bb, 0xff45, 0x00bb, 0xff45,
		0x00ba, 0xff46, 0x00ba, 0xff46, 0x00b9, 0xff47, 0x00b9, 0xff47,
		0x00b8, 0xff48, 0x00b8, 0xff48, 0x00b7, 0xff49, 0x00b7, 0xff49,
		0x00b6, 0xff4a, 0x00b6, 0xff4a, 0x00b5, 0xff4b, 0x00b5, 0xff4b,
		0x00b4, 0xff4c, 0x00b4, 0xff4c, 0x00b3, 0xff4d, 0x00b3, 0xff4d,
		0x00b2, 0xff4e, 0x00b2, 0xff4e, 0x00b1, 0xff4f, 0x00b1, 0xff4f,
		0x00b0, 0xff50, 0x00b0, 0xff50, 0x00af, 0xff51, 0x00af, 0xff51,
		0x00ae, 0xff52, 0x00ae, 0xff52, 0x00ad, 0xff53, 0x00ad, 0xff53,
		0x00ac, 0xff54, 0x00ac, 0xff54, 0x00ac, 0xff54, 0x00ab, 0xff55,
		0x00ab, 0xff55, 0x00aa, 0xff56, 0x00aa, 0xff56, 0x00a9, 0xff57,
		0x00a9, 0xff57, 0x00a8, 0xff58, 0x00a8, 0xff58, 0x00a7, 0xff59,
		0x00a7, 0xff59, 0x00a6, 0xff5a, 0x00a6, 0xff5a, 0x00a6, 0xff5a,
		0x00a5, 0xff5b, 0x00a5, 0xff5b, 0x00a4, 0xff5c, 0x00a4, 0xff5c,
		0x00a3, 0xff5d, 0x00a3, 0xff5d, 0x00a2, 0xff5e, 0x00a2, 0xff5e,
		0x00a2, 0xff5e, 0x00a1, 0xff5f, 0x00a1, 0xff5f, 0x00a0, 0xff60,
		0x00a0, 0xff60, 0x009f, 0xff61, 0x009f, 0xff61, 0x009f, 0xff61,
		0x009e, 0xff62, 0x009e, 0xff62, 0x009d, 0xff63, 0x009d, 0xff63,
		0x009c, 0xff64, 0x009c, 0xff64, 0x009c, 0xff64, 0x009b, 0xff65,
		0x009b, 0xff65, 0x009a, 0xff66, 0x009a, 0xff66, 0x0099, 0xff67,
		0x0099, 0xff67, 0x0099, 0xff67, 0x0098, 0xff68, 0x0098, 0xff68,
		0x0097, 0xff69, 0x0097, 0xff69, 0x0097, 0xff69, 0x0096, 0xff6a,
		0x0096, 0xff6a, 0x0095, 0xff6b, 0x0095, 0xff6b, 0x0095, 0xff6b,
		0x0094, 0xff6c, 0x0094, 0xff6c, 0x0093, 0xff6d, 0x0093, 0xff6d,
		0x0093, 0xff6d, 0x0092, 0xff6e, 0x0092, 0xff6e, 0x0091, 0xff6f,
		0x0091, 0xff6f, 0x0091, 0xff6f, 0x0090, 0xff70, 0x0090, 0xff70,
		0x008f, 0xff71, 0x008f, 0xff71, 0x008f, 0xff71, 0x008e, 0xff72,
		0x008e, 0xff72, 0x008d, 0xff73, 0x008d, 0xff73, 0x008d, 0xff73,
		0x008c, 0xff74, 0x008c, 0xff74, 0x008c, 0xff74, 0x008b, 0xff75,
		0x008b, 0xff75, 0x008a, 0xff76, 0x008a, 0xff76, 0x008a, 0xff76,
		0x0089, 0xff77, 0x0089, 0xff77, 0x0089, 0xff77, 0x0088, 0xff78,
		0x0088, 0xff78, 0x0087, 0xff79, 0x0087, 0xff79, 0x0087, 0xff79,
		0x0086, 0xff7a, 0x0086, 0xff7a, 0x0086, 0xff7a, 0x0085, 0xff7b,
		0x0085, 0xff7b, 0x0085, 0xff7b, 0x0084, 0xff7c, 0x0084, 0xff7c,
		0x0083, 0xff7d, 0x0083, 0xff7d, 0x0083, 0xff7d, 0x0082, 0xff7e,
		0x0082, 0xff7e, 0x0082, 0xff7e, 0x0081, 0xff7f, 0x0081, 0xff7f,
		0x0081, 0xff7f, 0x0080, 0xff80, 0x0080, 0xff80, 0x0080, 0xff80,
		0x007f, 0xff81, 0x007f, 0xff81, 0x007e, 0xff82, 0x007e, 0xff82,
		0x007e, 0xff82, 0x007d, 0xff83, 0x007d, 0xff83, 0x007d, 0xff83,
		0x007c, 0xff84, 0x007c, 0xff84, 0x007c, 0xff84, 0x007b, 0xff85,
		0x007b, 0xff85, 0x007b, 0xff85, 0x007a, 0xff86, 0x007a, 0xff86,
		0x007a, 0xff86, 0x0079, 0xff87, 0x0079, 0xff87, 0x0079, 0xff87,
		0x0078, 0xff88, 0x0078, 0xff88, 0x0078, 0xff88, 0x0077, 0xff89,
		0x0077, 0xff89, 0x0077, 0xff89, 0x0077, 0xff89, 0x0076, 0xff8a,
		0x0076, 0xff8a, 0x0076, 0xff8a, 0x0075, 0xff8b, 0x0075, 0xff8b,
		0x0075, 0xff8b, 0x0074, 0xff8c, 0x0074, 0xff8c, 0x0074, 0xff8c,
		0x0073, 0xff8d, 0x0073, 0xff8d, 0x0073, 0xff8d, 0x0072, 0xff8e,
		0x0072, 0xff8e, 0x0072, 0xff8e, 0x0071, 0xff8f, 0x0071, 0xff8f,
		0x0071, 0xff8f, 0x0071, 0xff8f, 0x0070, 0xff90, 0x0070, 0xff90,
		0x0070, 0xff90, 0x006f, 0xff91, 0x006f, 0xff91, 0x006f, 0xff91,
		0x006e, 0xff92, 0x006e, 0xff92, 0x006e, 0xff92, 0x006e, 0xff92,
		0x006d, 0xff93, 0x006d, 0xff93, 0x006d, 0xff93, 0x006c, 0xff94,
		0x006c, 0xff94, 0x006c, 0xff94, 0x006b, 0xff95, 0x006b, 0xff95,
		0x006b, 0xff95, 0x006b, 0xff95, 0x006a, 0xff96, 0x006a, 0xff96,
		0x006a, 0xff96, 0x0069, 0xff97, 0x0069, 0xff97, 0x0069, 0xff97,
		0x0069, 0xff97, 0x0068, 0xff98, 0x0068, 0xff98, 0x0068, 0xff98,
		0x0067, 0xff99, 0x0067, 0xff99, 0x0067, 0xff99, 0x0067, 0xff99,
		0x0066, 0xff9a, 0x0066, 0xff9a, 0x0066, 0xff9a, 0x0065, 0xff9b,
		0x0065, 0xff9b, 0x0065, 0xff9b, 0x0065, 0xff9b, 0x0064, 0xff9c,
		0x0064, 0xff9c, 0x0064, 0xff9c, 0x0064, 0xff9c, 0x0063, 0xff9d,
		0x0063, 0xff9d, 0x0063, 0xff9d, 0x0063, 0xff9d, 0x0062, 0xff9e,
		0x0062, 0xff9e, 0x0062, 0xff9e, 0x0061, 0xff9f, 0x0061, 0xff9f,
		0x0061, 0xff9f, 0x0061, 0xff9f, 0x0060, 0xffa0, 0x0060, 0xffa0,
		0x0060, 0xffa0, 0x0060, 0xffa0, 0x005f, 0xffa1, 0x005f, 0xffa1,
		0x005f, 0xffa1, 0x005f, 0xffa1, 0x005e, 0xffa2, 0x005e, 0xffa2,
		0x005e, 0xffa2, 0x005e, 0xffa2, 0x005d, 0xffa3, 0x005d, 0xffa3,
		0x005d, 0xffa3, 0x005d, 0xffa3, 0x005c, 0xffa4, 0x005c, 0xffa4,
		0x005c, 0xffa4, 0x005c, 0xffa4, 0x005b, 0xffa5, 0x005b, 0xffa5,
		0x005b, 0xffa5, 0x005b, 0xffa5, 0x005a, 0xffa6, 0x005a, 0xffa6,
		0x005a, 0xffa6, 0x005a, 0xffa6, 0x0059, 0xffa7, 0x0059, 0xffa7,
		0x0059, 0xffa7, 0x0059, 0xffa7, 0x0058, 0xffa8, 0x0058, 0xffa8,
		0x0058, 0xffa8, 0x0058, 0xffa8, 0x0057, 0xffa9, 0x0057, 0xffa9,
		0x0057, 0xffa9, 0x0057, 0xffa9, 0x0056, 0xffaa, 0x0056, 0xffaa,
		0x0056, 0xffaa, 0x0056, 0xffaa, 0x0056, 0xffaa, 0x0055, 0xffab,
		0x0055, 0xffab, 0x0055, 0xffab, 0x0055, 0xffab, 0x0054, 0xffac,
		0x0054, 0xffac, 0x0054, 0xffac, 0x0054, 0xffac, 0x0053, 0xffad,
		0x0053, 0xffad, 0x0053, 0xffad, 0x0053, 0xffad, 0x0053, 0xffad,
		0x0052, 0xffae, 0x0052, 0xffae, 0x0052, 0xffae, 0x0052, 0xffae,
		0x0051, 0xffaf, 0x0051, 0xffaf, 0x0051, 0xffaf, 0x0051, 0xffaf,
		0x0051, 0xffaf, 0x0050, 0xffb0, 0x0050, 0xffb0, 0x0050, 0xffb0,
		0x0050, 0xffb0, 0x004f, 0xffb1, 0x004f, 0xffb1, 0x004f, 0xffb1,
		0x004f, 0xffb1, 0x004f, 0xffb1, 0x004e, 0xffb2, 0x004e, 0xffb2,
		0x004e, 0xffb2, 0x004e, 0xffb2, 0x004e, 0xffb2, 0x004d, 0xffb3,
		0x004d, 0xffb3, 0x004d, 0xffb3, 0x004d, 0xffb3, 0x004c, 0xffb4,
		0x004c, 0xffb4, 0x004c, 0xffb4, 0x004c, 0xffb4, 0x004c, 0xffb4,
		0x004b, 0xffb5, 0x004b, 0xffb5, 0x004b, 0xffb5, 0x004b, 0xffb5,
		0x004b, 0xffb5, 0x004a, 0xffb6, 0x004a, 0xffb6, 0x004a, 0xffb6,
		0x004a, 0xffb6, 0x004a, 0xffb6, 0x0049, 0xffb7, 0x0049, 0xffb7,
		0x0049, 0xffb7, 0x0049, 0xffb7, 0x0049, 0xffb7, 0x0048, 0xffb8,
		0x0048, 0xffb8, 0x0048, 0xffb8, 0x0048, 0xffb8, 0x0048, 0xffb8,
		0x0047, 0xffb9, 0x0047, 0xffb9, 0x0047, 0xffb9, 0x0047, 0xffb9,
		0x0047, 0xffb9, 0x0046, 0xffba, 0x0046, 0xffba, 0x0046, 0xffba,
		0x0046, 0xffba, 0x0046, 0xffba, 0x0046, 0xffba, 0x0045, 0xffbb,
		0x0045, 0xffbb, 0x0045, 0xffbb, 0x0045, 0xffbb, 0x0045, 0xffbb,
		0x0044, 0xffbc, 0x0044, 0xffbc, 0x0044, 0xffbc, 0x0044, 0xffbc,
		0x0044, 0xffbc, 0x0043, 0xffbd, 0x0043, 0xffbd, 0x0043, 0xffbd,
		0x0043, 0xffbd, 0x0043, 0xffbd, 0x0043, 0xffbd, 0x0042, 0xffbe,
		0x0042, 0xffbe, 0x0042, 0xffbe, 0x0042, 0xffbe, 0x0042, 0xffbe,
		0x0041, 0xffbf, 0x0041, 0xffbf, 0x0041, 0xffbf, 0x0041, 0xffbf,
		0x0041, 0xffbf, 0x0041, 0xffbf, 0x0040, 0xffc0, 0x0040, 0xffc0,
		0x0040, 0xffc0, 0x0040, 0xffc0, 0x0040, 0xffc0, 0x0040, 0xffc0,
		0x003f, 0xffc1, 0x003f, 0xffc1, 0x003f, 0xffc1, 0x003f, 0xffc1,
		0x003f, 0xffc1, 0x003e, 0xffc2, 0x003e, 0xffc2, 0x003e, 0xffc2,
		0x003e, 0xffc2, 0x003e, 0xffc2, 0x003e, 0xffc2, 0x003d, 0xffc3,
		0x003d, 0xffc3, 0x003d, 0xffc3, 0x003d, 0xffc3, 0x003d, 0xffc3,
		0x003d, 0xffc3, 0x003c, 0xffc4, 0x003c, 0xffc4, 0x003c, 0xffc4,
		0x003c, 0xffc4, 0x003c, 0xffc4, 0x003c, 0xffc4, 0x003b, 0xffc5,
		0x003b, 0xffc5, 0x003b, 0xffc5, 0x003b, 0xffc5, 0x003b, 0xffc5,
		0x003b, 0xffc5, 0x003b, 0xffc5, 0x003a, 0xffc6, 0x003a, 0xffc6,
		0x003a, 0xffc6, 0x003a, 0xffc6, 0x003a, 0xffc6, 0x003a, 0xffc6,
		0x0039, 0xffc7, 0x0039, 0xffc7, 0x0039, 0xffc7, 0x0039, 0xffc7,
		0x0039, 0xffc7, 0x0039, 0xffc7, 0x0038, 0xffc8, 0x0038, 0xffc8,
		0x0038, 0xffc8, 0x0038, 0xffc8, 0x0038, 0xffc8, 0x0038, 0xffc8,
		0x0038, 0xffc8, 0x0037, 0xffc9, 0x0037, 0xffc9, 0x0037, 0xffc9,
		0x0037, 0xffc9, 0x0037, 0xffc9, 0x0037, 0xffc9, 0x0037, 0xffc9,
		0x0036, 0xffca, 0x0036, 0xffca, 0x0036, 0xffca, 0x0036, 0xffca,
		0x0036, 0xffca, 0x0036, 0xffca, 0x0035, 0xffcb, 0x0035, 0xffcb,
		0x0035, 0xffcb, 0x0035, 0xffcb, 0x0035, 0xffcb, 0x0035, 0xffcb,
		0x0035, 0xffcb, 0x0034, 0xffcc, 0x0034, 0xffcc, 0x0034, 0xffcc,
		0x0034, 0xffcc, 0x0034, 0xffcc, 0x0034, 0xffcc, 0x0034, 0xffcc,
		0x0033, 0xffcd, 0x0033, 0xffcd, 0x0033, 0xffcd, 0x0033, 0xffcd,
		0x0033, 0xffcd, 0x0033, 0xffcd, 0x0033, 0xffcd, 0x0032, 0xffce,
		0x0032, 0xffce, 0x0032, 0xffce, 0x0032, 0xffce, 0x0032, 0xffce,
		0x0032, 0xffce, 0x0032, 0xffce, 0x0032, 0xffce, 0x0031, 0xffcf,
		0x0031, 0xffcf, 0x0031, 0xffcf, 0x0031, 0xffcf, 0x0031, 0xffcf,
		0x0031, 0xffcf, 0x0031, 0xffcf, 0x0030, 0xffd0, 0x0030, 0xffd0,
		0x0030, 0xffd0, 0x0030, 0xffd0, 0x0030, 0xffd0, 0x0030, 0xffd0,
		0x0030, 0xffd0, 0x0030, 0xffd0, 0x002f, 0xffd1, 0x002f, 0xffd1,
		0x002f, 0xffd1, 0x002f, 0xffd1, 0x002f, 0xffd1, 0x002f, 0xffd1,
		0x002f, 0xffd1, 0x002f, 0xffd1, 0x002e, 0xffd2, 0x002e, 0xffd2,
		0x002e, 0xffd2, 0x002e, 0xffd2, 0x002e, 0xffd2, 0x002e, 0xffd2,
		0x002e, 0xffd2, 0x002e, 0xffd2, 0x002d, 0xffd3, 0x002d, 0xffd3,
		0x002d, 0xffd3, 0x002d, 0xffd3, 0x002d, 0xffd3, 0x002d, 0xffd3,
		0x002d, 0xffd3, 0x002d, 0xffd3, 0x002c, 0xffd4, 0x002c, 0xffd4,
		0x002c, 0xffd4, 0x002c, 0xffd4, 0x002c, 0xffd4, 0x002c, 0xffd4,
		0x002c, 0xffd4, 0x002c, 0xffd4, 0x002b, 0xffd5, 0x002b, 0xffd5,
		0x002b, 0xffd5, 0x002b, 0xffd5, 0x002b, 0xffd5, 0x002b, 0xffd5,
		0x002b, 0xffd5, 0x002b, 0xffd5, 0x002b, 0xffd5, 0x002a, 0xffd6,
		0x002a, 0xffd6, 0x002a, 0xffd6, 0x002a, 0xffd6, 0x002a, 0xffd6,
		0x002a, 0xffd6, 0x002a, 0xffd6, 0x002a, 0xffd6, 0x0029, 0xffd7,
		0x0029, 0xffd7, 0x0029, 0xffd7, 0x0029, 0xffd7, 0x0029, 0xffd7,
		0x0029, 0xffd7, 0x0029, 0xffd7, 0x0029, 0xffd7, 0x0029, 0xffd7,
		0x0028, 0xffd8, 0x0028, 0xffd8, 0x0028, 0xffd8, 0x0028, 0xffd8,
		0x0028, 0xffd8, 0x0028, 0xffd8, 0x0028, 0xffd8, 0x0028, 0xffd8,
		0x0028, 0xffd8, 0x0027, 0xffd9, 0x0027, 0xffd9, 0x0027, 0xffd9,
		0x0027, 0xffd9, 0x0027, 0xffd9, 0x0027, 0xffd9, 0x0027, 0xffd9,
		0x0027, 0xffd9, 0x0027, 0xffd9, 0x0027, 0xffd9, 0x0026, 0xffda,
		0x0026, 0xffda, 0x0026, 0xffda, 0x0026, 0xffda, 0x0026, 0xffda,
		0x0026, 0xffda, 0x0026, 0xffda, 0x0026, 0xffda, 0x0026, 0xffda,
		0x0025, 0xffdb, 0x0025, 0xffdb, 0x0025, 0xffdb, 0x0025, 0xffdb,
		0x0025, 0xffdb, 0x0025, 0xffdb, 0x0025, 0xffdb, 0x0025, 0xffdb,
		0x0025, 0xffdb, 0x0025, 0xffdb, 0x0024, 0xffdc, 0x0024, 0xffdc,
		0x0024, 0xffdc, 0x0024, 0xffdc, 0x0024, 0xffdc, 0x0024, 0xffdc,
		0x0024, 0xffdc, 0x0024, 0xffdc, 0x0024, 0xffdc, 0x0024, 0xffdc,
		0x0023, 0xffdd, 0x0023, 0xffdd, 0x0023, 0xffdd, 0x0023, 0xffdd,
		0x0023, 0xffdd, 0x0023, 0xffdd, 0x0023, 0xffdd, 0x0023, 0xffdd,
		0x0023, 0xffdd, 0x0023, 0xffdd, 0x0023, 0xffdd, 0x0022, 0xffde,
		0x0022, 0xffde, 0x0022, 0xffde, 0x0022, 0xffde, 0x0022, 0xffde,
		0x0022, 0xffde, 0x0022, 0xffde, 0x0022, 0xffde, 0x0022, 0xffde,
		0x0022, 0xffde, 0x0021, 0xffdf, 0x0021, 0xffdf, 0x0021, 0xffdf,
		0x0021, 0xffdf, 0x0021, 0xffdf, 0x0021, 0xffdf, 0x0021, 0xffdf,
		0x0021, 0xffdf, 0x0021, 0xffdf, 0x0021, 0xffdf, 0x0021, 0xffdf,
		0x0020, 0xffe0, 0x0020, 0xffe0, 0x0020, 0xffe0, 0x0020, 0xffe0,
		0x0020, 0xffe0, 0x0020, 0xffe0, 0x0020, 0xffe0, 0x0020, 0xffe0,
		0x0020, 0xffe0, 0x0020, 0xffe0, 0x0020, 0xffe0, 0x0020, 0xffe0,
		0x001f, 0xffe1, 0x001f, 0xffe1, 0x001f, 0xffe1, 0x001f, 0xffe1,
		0x001f, 0xffe1, 0x001f, 0xffe1, 0x001f, 0xffe1, 0x001f, 0xffe1,
		0x001f, 0xffe1, 0x001f, 0xffe1, 0x001f, 0xffe1, 0x001e, 0xffe2,
		0x001e, 0xffe2, 0x001e, 0xffe2, 0x001e, 0xffe2, 0x001e, 0xffe2,
		0x001e, 0xffe2, 0x001e, 0xffe2, 0x001e, 0xffe2, 0x001e, 0xffe2,
		0x001e, 0xffe2, 0x001e, 0xffe2, 0x001e, 0xffe2, 0x001d, 0xffe3,
		0x001d, 0xffe3, 0x001d, 0xffe3, 0x001d, 0xffe3, 0x001d, 0xffe3,
		0x001d, 0xffe3, 0x001d, 0xffe3, 0x001d, 0xffe3, 0x001d, 0xffe3,
		0x001d, 0xffe3, 0x001d, 0xffe3, 0x001d, 0xffe3, 0x001d, 0xffe3,
		0x001c, 0xffe4, 0x001c, 0xffe4, 0x001c, 0xffe4, 0x001c, 0xffe4,
		0x001c, 0xffe4, 0x001c, 0xffe4, 0x001c, 0xffe4, 0x001c, 0xffe4,
		0x001c, 0xffe4, 0x001c, 0xffe4, 0x001c, 0xffe4, 0x001c, 0xffe4,
		0x001c, 0xffe4, 0x001b, 0xffe5, 0x001b, 0xffe5, 0x001b, 0xffe5,
		0x001b, 0xffe5, 0x001b, 0xffe5, 0x001b, 0xffe5, 0x001b, 0xffe5,
		0x001b, 0xffe5, 0x001b, 0xffe5, 0x001b, 0xffe5, 0x001b, 0xffe5,
		0x001b, 0xffe5, 0x001b, 0xffe5, 0x001a, 0xffe6, 0x001a, 0xffe6,
		0x001a, 0xffe6, 0x001a, 0xffe6, 0x001a, 0xffe6, 0x001a, 0xffe6,
		0x001a, 0xffe6, 0x001a, 0xffe6, 0x001a, 0xffe6, 0x001a, 0xffe6,
		0x001a, 0xffe6, 0x001a, 0xffe6, 0x001a, 0xffe6, 0x001a, 0xffe6,
		0x0019, 0xffe7, 0x0019, 0xffe7, 0x0019, 0xffe7, 0x0019, 0xffe7,
		0x0019, 0xffe7, 0x0019, 0xffe7, 0x0019, 0xffe7, 0x0019, 0xffe7,
		0x0019, 0xffe7, 0x0019, 0xffe7, 0x0019, 0xffe7, 0x0019, 0xffe7,
		0x0019, 0xffe7, 0x0019, 0xffe7, 0x0019, 0xffe7, 0x0018, 0xffe8,
		0x0018, 0xffe8, 0x0018, 0xffe8, 0x0018, 0xffe8, 0x0018, 0xffe8,
		0x0018, 0xffe8, 0x0018, 0xffe8, 0x0018, 0xffe8, 0x0018, 0xffe8,
		0x0018, 0xffe8, 0x0018, 0xffe8, 0x0018, 0xffe8, 0x0018, 0xffe8,
		0x0018, 0xffe8, 0x0018, 0xffe8, 0x0017, 0xffe9, 0x0017, 0xffe9,
		0x0017, 0xffe9, 0x0017, 0xffe9, 0x0017, 0xffe9, 0x0017, 0xffe9,
		0x0017, 0xffe9, 0x0017, 0xffe9, 0x0017, 0xffe9, 0x0017, 0xffe9,
		0x0017, 0xffe9, 0x0017, 0xffe9, 0x0017, 0xffe9, 0x0017, 0xffe9,
		0x0017, 0xffe9, 0x0017, 0xffe9, 0x0016, 0xffea, 0x0016, 0xffea,
		0x0016, 0xffea, 0x0016, 0xffea, 0x0016, 0xffea, 0x0016, 0xffea,
		0x0016, 0xffea, 0x0016, 0xffea, 0x0016, 0xffea, 0x0016, 0xffea,
		0x0016, 0xffea, 0x0016, 0xffea, 0x0016, 0xffea, 0x0016, 0xffea,
		0x0016, 0xffea, 0x0016, 0xffea, 0x0015, 0xffeb, 0x0015, 0xffeb,
		0x0015, 0xffeb, 0x0015, 0xffeb, 0x0015, 0xffeb, 0x0015, 0xffeb,
		0x0015, 0xffeb, 0x0015, 0xffeb, 0x0015, 0xffeb, 0x0015, 0xffeb,
		0x0015, 0xffeb, 0x0015, 0xffeb, 0x0015, 0xffeb, 0x0015, 0xffeb,
		0x0015, 0xffeb, 0x0015, 0xffeb, 0x0015, 0xffeb, 0x0014, 0xffec,
		0x0014, 0xffec, 0x0014, 0xffec, 0x0014, 0xffec, 0x0014, 0xffec,
		0x0014, 0xffec, 0x0014, 0xffec, 0x0014, 0xffec, 0x0014, 0xffec,
		0x0014, 0xffec, 0x0014, 0xffec, 0x0014, 0xffec, 0x0014, 0xffec,
		0x0014, 0xffec, 0x0014, 0xffec, 0x0014, 0xffec, 0x0014, 0xffec,
		0x0014, 0xffec, 0x0013, 0xffed, 0x0013, 0xffed, 0x0013, 0xffed,
		0x0013, 0xffed, 0x0013, 0xffed, 0x0013, 0xffed, 0x0013, 0xffed,
		0x0013, 0xffed, 0x0013, 0xffed, 0x0013, 0xffed, 0x0013, 0xffed,
		0x0013, 0xffed, 0x0013, 0xffed, 0x0013, 0xffed, 0x0013, 0xffed,
		0x0013, 0xffed, 0x0013, 0xffed, 0x0013, 0xffed, 0x0013, 0xffed,
		0x0012, 0xffee, 0x0012, 0xffee, 0x0012, 0xffee, 0x0012, 0xffee,
		0x0012, 0xffee, 0x0012, 0xffee, 0x0012, 0xffee, 0x0012, 0xffee,
		0x0012, 0xffee, 0x0012, 0xffee, 0x0012, 0xffee, 0x0012, 0xffee,
		0x0012, 0xffee, 0x0012, 0xffee, 0x0012, 0xffee, 0x0012, 0xffee,
		0x0012, 0xffee, 0x0012, 0xffee, 0x0012, 0xffee, 0x0012, 0xffee,
		0x0011, 0xffef, 0x0011, 0xffef, 0x0011, 0xffef, 0x0011, 0xffef,
		0x0011, 0xffef, 0x0011, 0xffef, 0x0011, 0xffef, 0x0011, 0xffef,
		0x0011, 0xffef, 0x0011, 0xffef, 0x0011, 0xffef, 0x0011, 0xffef,
		0x0011, 0xffef, 0x0011, 0xffef, 0x0011, 0xffef, 0x0011, 0xffef,
		0x0011, 0xffef, 0x0011, 0xffef, 0x0011, 0xffef, 0x0011, 0xffef,
		0x0011, 0xffef, 0x0010, 0xfff0, 0x0010, 0xfff0, 0x0010, 0xfff0,
		0x0010, 0xfff0, 0x0010, 0xfff0, 0x0010, 0xfff0, 0x0010, 0xfff0,
		0x0010, 0xfff0, 0x0010, 0xfff0, 0x0010, 0xfff0, 0x0010, 0xfff0,
		0x0010, 0xfff0, 0x0010, 0xfff0, 0x0010, 0xfff0, 0x0010, 0xfff0,
		0x0010, 0xfff0, 0x0010, 0xfff0, 0x0010, 0xfff0, 0x0010, 0xfff0,
		0x0010, 0xfff0, 0x0010, 0xfff0, 0x0010, 0xfff0, 0x0010, 0xfff0,
		0x000f, 0xfff1, 0x000f, 0xfff1, 0x000f, 0xfff1, 0x000f, 0xfff1,
		0x000f, 0xfff1, 0x000f, 0xfff1, 0x000f, 0xfff1, 0x000f, 0xfff1,
		0x000f, 0xfff1, 0x000f, 0xfff1, 0x000f, 0xfff1, 0x000f, 0xfff1,
		0x000f, 0xfff1, 0x000f, 0xfff1, 0x000f, 0xfff1, 0x000f, 0xfff1,
		0x000f, 0xfff1, 0x000f, 0xfff1, 0x000f, 0xfff1, 0x000f, 0xfff1,
		0x000f, 0xfff1, 0x000f, 0xfff1, 0x000f, 0xfff1, 0x000e, 0xfff2,
		0x000e, 0xfff2, 0x000e, 0xfff2, 0x000e, 0xfff2, 0x000e, 0xfff2,
		0x000e, 0xfff2, 0x000e, 0xfff2, 0x000e, 0xfff2, 0x000e, 0xfff2,
		0x000e, 0xfff2, 0x000e, 0xfff2, 0x000e, 0xfff2, 0x000e, 0xfff2,
		0x000e, 0xfff2, 0x000e, 0xfff2, 0x000e, 0xfff2, 0x000e, 0xfff2,
		0x000e, 0xfff2, 0x000e, 0xfff2, 0x000e, 0xfff2, 0x000e, 0xfff2,
		0x000e, 0xfff2, 0x000e, 0xfff2, 0x000e, 0xfff2, 0x000e, 0xfff2,
		0x000e, 0xfff2, 0x000d, 0xfff3, 0x000d, 0xfff3, 0x000d, 0xfff3,
		0x000d, 0xfff3, 0x000d, 0xfff3, 0x000d, 0xfff3, 0x000d, 0xfff3,
		0x000d, 0xfff3, 0x000d, 0xfff3, 0x000d, 0xfff3, 0x000d, 0xfff3,
		0x000d, 0xfff3, 0x000d, 0xfff3, 0x000d, 0xfff3, 0x000d, 0xfff3,
		0x000d, 0xfff3, 0x000d, 0xfff3, 0x000d, 0xfff3, 0x000d, 0xfff3,
		0x000d, 0xfff3, 0x000d, 0xfff3, 0x000d, 0xfff3, 0x000d, 0xfff3,
		0x000d, 0xfff3, 0x000d, 0xfff3, 0x000d, 0xfff3, 0x000d, 0xfff3,
		0x000c, 0xfff4, 0x000c, 0xfff4, 0x000c, 0xfff4, 0x000c, 0xfff4,
		0x000c, 0xfff4, 0x000c, 0xfff4, 0x000c, 0xfff4, 0x000c, 0xfff4,
		0x000c, 0xfff4, 0x000c, 0xfff4, 0x000c, 0xfff4, 0x000c, 0xfff4,
		0x000c, 0xfff4, 0x000c, 0xfff4, 0x000c, 0xfff4, 0x000c, 0xfff4,
		0x000c, 0xfff4, 0x000c, 0xfff4, 0x000c, 0xfff4, 0x000c, 0xfff4,
		0x000c, 0xfff4, 0x000c, 0xfff4, 0x000c, 0xfff4, 0x000c, 0xfff4,
		0x000c, 0xfff4, 0x000c, 0xfff4, 0x000c, 0xfff4, 0x000c, 0xfff4,
		0x000c, 0xfff4, 0x000c, 0xfff4, 0x000b, 0xfff5, 0x000b, 0xfff5,
		0x000b, 0xfff5, 0x000b, 0xfff5, 0x000b, 0xfff5, 0x000b, 0xfff5,
		0x000b, 0xfff5, 0x000b, 0xfff5, 0x000b, 0xfff5, 0x000b, 0xfff5,
		0x000b, 0xfff5, 0x000b, 0xfff5, 0x000b, 0xfff5, 0x000b, 0xfff5,
		0x000b, 0xfff5, 0x000b, 0xfff5, 0x000b, 0xfff5, 0x000b, 0xfff5,
		0x000b, 0xfff5, 0x000b, 0xfff5, 0x000b, 0xfff5, 0x000b, 0xfff5,
		0x000b, 0xfff5, 0x000b, 0xfff5, 0x000b, 0xfff5, 0x000b, 0xfff5,
		0x000b, 0xfff5, 0x000b, 0xfff5, 0x000b, 0xfff5, 0x000b, 0xfff5,
		0x000b, 0xfff5, 0x000b, 0xfff5, 0x000a, 0xfff6, 0x000a, 0xfff6,
		0x000a, 0xfff6, 0x000a, 0xfff6, 0x000a, 0xfff6, 0x000a, 0xfff6,
		0x000a, 0xfff6, 0x000a, 0xfff6, 0x000a, 0xfff6, 0x000a, 0xfff6,
		0x000a, 0xfff6, 0x000a, 0xfff6, 0x000a, 0xfff6, 0x000a, 0xfff6,
		0x000a, 0xfff6, 0x000a, 0xfff6, 0x000a, 0xfff6, 0x000a, 0xfff6,
		0x000a, 0xfff6, 0x000a, 0xfff6, 0x000a, 0xfff6, 0x000a, 0xfff6,
		0x000a, 0xfff6, 0x000a, 0xfff6, 0x000a, 0xfff6, 0x000a, 0xfff6,
		0x000a, 0xfff6, 0x000a, 0xfff6, 0x000a, 0xfff6, 0x000a, 0xfff6,
		0x000a, 0xfff6, 0x000a, 0xfff6, 0x000a, 0xfff6, 0x000a, 0xfff6,
		0x000a, 0xfff6, 0x0009, 0xfff7, 0x0009, 0xfff7, 0x0009, 0xfff7,
		0x0009, 0xfff7, 0x0009, 0xfff7, 0x0009, 0xfff7, 0x0009, 0xfff7,
		0x0009, 0xfff7, 0x0009, 0xfff7, 0x0009, 0xfff7, 0x0009, 0xfff7,
		0x0009, 0xfff7, 0x0009, 0xfff7, 0x0009, 0xfff7, 0x0009, 0xfff7,
		0x0009, 0xfff7, 0x0009, 0xfff7, 0x0009, 0xfff7, 0x0009, 0xfff7,
		0x0009, 0xfff7, 0x0009, 0xfff7, 0x0009, 0xfff7, 0x0009, 0xfff7,
		0x0009, 0xfff7, 0x0009, 0xfff7, 0x0009, 0xfff7, 0x0009, 0xfff7,
		0x0009, 0xfff7, 0x0009, 0xfff7, 0x0009, 0xfff7, 0x0009, 0xfff7,
		0x0009, 0xfff7, 0x0009, 0xfff7, 0x0009, 0xfff7, 0x0009, 0xfff7,
		0x0009, 0xfff7, 0x0009, 0xfff7, 0x0009, 0xfff7, 0x0009, 0xfff7,
		0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8,
		0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8,
		0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8,
		0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8,
		0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8,
		0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8,
		0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8,
		0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8,
		0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8,
		0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8,
		0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8, 0x0008, 0xfff8,
		0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9,
		0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9,
		0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9,
		0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9,
		0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9,
		0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9,
		0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9,
		0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9,
		0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9,
		0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9,
		0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9,
		0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9, 0x0007, 0xfff9,
		0x0007, 0xfff9, 0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa,
		0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa,
		0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa,
		0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa,
		0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa,
		0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa,
		0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa,
		0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa,
		0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa,
		0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa,
		0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa,
		0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa,
		0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa,
		0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa, 0x0006, 0xfffa,
		0x0006, 0xfffa, 0x0006, 0xfffa, 0x0005, 0xfffb, 0x0005, 0xfffb,
		0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb,
		0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb,
		0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb,
		0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb,
		0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb,
		0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb,
		0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb,
		0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb,
		0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb,
		0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb,
		0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb,
		0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb,
		0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb,
		0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb,
		0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb,
		0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb, 0x0005, 0xfffb,
		0x0005, 0xfffb, 0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc,
		0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc,
		0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc,
		0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc,
		0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc,
		0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc,
		0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc,
		0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc,
		0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc,
		0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc,
		0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc,
		0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc,
		0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc,
		0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc,
		0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc,
		0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc,
		0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc,
		0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc,
		0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc,
		0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc,
		0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc, 0x0004, 0xfffc,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd, 0x0003, 0xfffd,
		0x0003, 0xfffd, 0x0003, 0xfffd, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe, 0x0002, 0xfffe,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
		0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff, 0x0001, 0xffff,
};

#define ENV_QUIET    (TL_TAB_LEN>>3)

/* sin waveform table in 'decibel' scale */
static const unsigned short sin_tab[SIN_LEN] = {
		0x10b2, 0x0d86, 0x0c0e, 0x0b16, 0x0a5c, 0x09c8, 0x094c, 0x08e2,
		0x0886, 0x0834, 0x07ea, 0x07a6, 0x076a, 0x0730, 0x06fc, 0x06ca,
		0x069c, 0x0672, 0x0648, 0x0622, 0x05fe, 0x05da, 0x05b8, 0x059a,
		0x057a, 0x055e, 0x0540, 0x0526, 0x050c, 0x04f2, 0x04da, 0x04c2,
		0x04ac, 0x0496, 0x0480, 0x046c, 0x0458, 0x0444, 0x0430, 0x041e,
		0x040c, 0x03fa, 0x03ea, 0x03d8, 0x03c8, 0x03b8, 0x03a8, 0x039a,
		0x038a, 0x037c, 0x036e, 0x0360, 0x0352, 0x0344, 0x0336, 0x032a,
		0x031e, 0x0310, 0x0304, 0x02f8, 0x02ee, 0x02e2, 0x02d6, 0x02cc,
		0x02c0, 0x02b6, 0x02aa, 0x02a0, 0x0296, 0x028c, 0x0282, 0x0278,
		0x026e, 0x0266, 0x025c, 0x0252, 0x024a, 0x0242, 0x0238, 0x0230,
		0x0228, 0x021e, 0x0216, 0x020e, 0x0206, 0x01fe, 0x01f6, 0x01f0,
		0x01e8, 0x01e0, 0x01d8, 0x01d2, 0x01ca, 0x01c4, 0x01bc, 0x01b6,
		0x01ae, 0x01a8, 0x01a2, 0x019a, 0x0194, 0x018e, 0x0188, 0x0182,
		0x017c, 0x0176, 0x0170, 0x016a, 0x0164, 0x015e, 0x0158, 0x0152,
		0x014e, 0x0148, 0x0142, 0x013e, 0x0138, 0x0132, 0x012e, 0x0128,
		0x0124, 0x011e, 0x011a, 0x0114, 0x0110, 0x010c, 0x0106, 0x0102,
		0x00fe, 0x00fa, 0x00f4, 0x00f0, 0x00ec, 0x00e8, 0x00e4, 0x00e0,
		0x00dc, 0x00d8, 0x00d4, 0x00d0, 0x00cc, 0x00c8, 0x00c4, 0x00c0,
		0x00bc, 0x00b8, 0x00b6, 0x00b2, 0x00ae, 0x00aa, 0x00a6, 0x00a4,
		0x00a0, 0x009c, 0x009a, 0x0096, 0x0094, 0x0090, 0x008c, 0x008a,
		0x0086, 0x0084, 0x0080, 0x007e, 0x007c, 0x0078, 0x0076, 0x0072,
		0x0070, 0x006e, 0x006a, 0x0068, 0x0066, 0x0062, 0x0060, 0x005e,
		0x005c, 0x005a, 0x0056, 0x0054, 0x0052, 0x0050, 0x004e, 0x004c,
		0x004a, 0x0048, 0x0046, 0x0044, 0x0042, 0x0040, 0x003e, 0x003c,
		0x003a, 0x0038, 0x0036, 0x0034, 0x0032, 0x0030, 0x002e, 0x002e,
		0x002c, 0x002a, 0x0028, 0x0028, 0x0026, 0x0024, 0x0022, 0x0022,
		0x0020, 0x001e, 0x001e, 0x001c, 0x001a, 0x001a, 0x0018, 0x0018,
		0x0016, 0x0014, 0x0014, 0x0012, 0x0012, 0x0010, 0x0010, 0x000e,
		0x000e, 0x000e, 0x000c, 0x000c, 0x000a, 0x000a, 0x000a, 0x0008,
		0x0008, 0x0008, 0x0006, 0x0006, 0x0006, 0x0004, 0x0004, 0x0004,
		0x0004, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0004,
		0x0004, 0x0004, 0x0004, 0x0006, 0x0006, 0x0006, 0x0008, 0x0008,
		0x0008, 0x000a, 0x000a, 0x000a, 0x000c, 0x000c, 0x000e, 0x000e,
		0x000e, 0x0010, 0x0010, 0x0012, 0x0012, 0x0014, 0x0014, 0x0016,
		0x0018, 0x0018, 0x001a, 0x001a, 0x001c, 0x001e, 0x001e, 0x0020,
		0x0022, 0x0022, 0x0024, 0x0026, 0x0028, 0x0028, 0x002a, 0x002c,
		0x002e, 0x002e, 0x0030, 0x0032, 0x0034, 0x0036, 0x0038, 0x003a,
		0x003c, 0x003e, 0x0040, 0x0042, 0x0044, 0x0046, 0x0048, 0x004a,
		0x004c, 0x004e, 0x0050, 0x0052, 0x0054, 0x0056, 0x005a, 0x005c,
		0x005e, 0x0060, 0x0062, 0x0066, 0x0068, 0x006a, 0x006e, 0x0070,
		0x0072, 0x0076, 0x0078, 0x007c, 0x007e, 0x0080, 0x0084, 0x0086,
		0x008a, 0x008c, 0x0090, 0x0094, 0x0096, 0x009a, 0x009c, 0x00a0,
		0x00a4, 0x00a6, 0x00aa, 0x00ae, 0x00b2, 0x00b6, 0x00b8, 0x00bc,
		0x00c0, 0x00c4, 0x00c8, 0x00cc, 0x00d0, 0x00d4, 0x00d8, 0x00dc,
		0x00e0, 0x00e4, 0x00e8, 0x00ec, 0x00f0, 0x00f4, 0x00fa, 0x00fe,
		0x0102, 0x0106, 0x010c, 0x0110, 0x0114, 0x011a, 0x011e, 0x0124,
		0x0128, 0x012e, 0x0132, 0x0138, 0x013e, 0x0142, 0x0148, 0x014e,
		0x0152, 0x0158, 0x015e, 0x0164, 0x016a, 0x0170, 0x0176, 0x017c,
		0x0182, 0x0188, 0x018e, 0x0194, 0x019a, 0x01a2, 0x01a8, 0x01ae,
		0x01b6, 0x01bc, 0x01c4, 0x01ca, 0x01d2, 0x01d8, 0x01e0, 0x01e8,
		0x01f0, 0x01f6, 0x01fe, 0x0206, 0x020e, 0x0216, 0x021e, 0x0228,
		0x0230, 0x0238, 0x0242, 0x024a, 0x0252, 0x025c, 0x0266, 0x026e,
		0x0278, 0x0282, 0x028c, 0x0296, 0x02a0, 0x02aa, 0x02b6, 0x02c0,
		0x02cc, 0x02d6, 0x02e2, 0x02ee, 0x02f8, 0x0304, 0x0310, 0x031e,
		0x032a, 0x0336, 0x0344, 0x0352, 0x0360, 0x036e, 0x037c, 0x038a,
		0x039a, 0x03a8, 0x03b8, 0x03c8, 0x03d8, 0x03ea, 0x03fa, 0x040c,
		0x041e, 0x0430, 0x0444, 0x0458, 0x046c, 0x0480, 0x0496, 0x04ac,
		0x04c2, 0x04da, 0x04f2, 0x050c, 0x0526, 0x0540, 0x055e, 0x057a,
		0x059a, 0x05b8, 0x05da, 0x05fe, 0x0622, 0x0648, 0x0672, 0x069c,
		0x06ca, 0x06fc, 0x0730, 0x076a, 0x07a6, 0x07ea, 0x0834, 0x0886,
		0x08e2, 0x094c, 0x09c8, 0x0a5c, 0x0b16, 0x0c0e, 0x0d86, 0x10b2,
		0x10b3, 0x0d87, 0x0c0f, 0x0b17, 0x0a5d, 0x09c9, 0x094d, 0x08e3,
		0x0887, 0x0835, 0x07eb, 0x07a7, 0x076b, 0x0731, 0x06fd, 0x06cb,
		0x069d, 0x0673, 0x0649, 0x0623, 0x05ff, 0x05db, 0x05b9, 0x059b,
		0x057b, 0x055f, 0x0541, 0x0527, 0x050d, 0x04f3, 0x04db, 0x04c3,
		0x04ad, 0x0497, 0x0481, 0x046d, 0x0459, 0x0445, 0x0431, 0x041f,
		0x040d, 0x03fb, 0x03eb, 0x03d9, 0x03c9, 0x03b9, 0x03a9, 0x039b,
		0x038b, 0x037d, 0x036f, 0x0361, 0x0353, 0x0345, 0x0337, 0x032b,
		0x031f, 0x0311, 0x0305, 0x02f9, 0x02ef, 0x02e3, 0x02d7, 0x02cd,
		0x02c1, 0x02b7, 0x02ab, 0x02a1, 0x0297, 0x028d, 0x0283, 0x0279,
		0x026f, 0x0267, 0x025d, 0x0253, 0x024b, 0x0243, 0x0239, 0x0231,
		0x0229, 0x021f, 0x0217, 0x020f, 0x0207, 0x01ff, 0x01f7, 0x01f1,
		0x01e9, 0x01e1, 0x01d9, 0x01d3, 0x01cb, 0x01c5, 0x01bd, 0x01b7,
		0x01af, 0x01a9, 0x01a3, 0x019b, 0x0195, 0x018f, 0x0189, 0x0183,
		0x017d, 0x0177, 0x0171, 0x016b, 0x0165, 0x015f, 0x0159, 0x0153,
		0x014f, 0x0149, 0x0143, 0x013f, 0x0139, 0x0133, 0x012f, 0x0129,
		0x0125, 0x011f, 0x011b, 0x0115, 0x0111, 0x010d, 0x0107, 0x0103,
		0x00ff, 0x00fb, 0x00f5, 0x00f1, 0x00ed, 0x00e9, 0x00e5, 0x00e1,
		0x00dd, 0x00d9, 0x00d5, 0x00d1, 0x00cd, 0x00c9, 0x00c5, 0x00c1,
		0x00bd, 0x00b9, 0x00b7, 0x00b3, 0x00af, 0x00ab, 0x00a7, 0x00a5,
		0x00a1, 0x009d, 0x009b, 0x0097, 0x0095, 0x0091, 0x008d, 0x008b,
		0x0087, 0x0085, 0x0081, 0x007f, 0x007d, 0x0079, 0x0077, 0x0073,
		0x0071, 0x006f, 0x006b, 0x0069, 0x0067, 0x0063, 0x0061, 0x005f,
		0x005d, 0x005b, 0x0057, 0x0055, 0x0053, 0x0051, 0x004f, 0x004d,
		0x004b, 0x0049, 0x0047, 0x0045, 0x0043, 0x0041, 0x003f, 0x003d,
		0x003b, 0x0039, 0x0037, 0x0035, 0x0033, 0x0031, 0x002f, 0x002f,
		0x002d, 0x002b, 0x0029, 0x0029, 0x0027, 0x0025, 0x0023, 0x0023,
		0x0021, 0x001f, 0x001f, 0x001d, 0x001b, 0x001b, 0x0019, 0x0019,
		0x0017, 0x0015, 0x0015, 0x0013, 0x0013, 0x0011, 0x0011, 0x000f,
		0x000f, 0x000f, 0x000d, 0x000d, 0x000b, 0x000b, 0x000b, 0x0009,
		0x0009, 0x0009, 0x0007, 0x0007, 0x0007, 0x0005, 0x0005, 0x0005,
		0x0005, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003,
		0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001,
		0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001,
		0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0005,
		0x0005, 0x0005, 0x0005, 0x0007, 0x0007, 0x0007, 0x0009, 0x0009,
		0x0009, 0x000b, 0x000b, 0x000b, 0x000d, 0x000d, 0x000f, 0x000f,
		0x000f, 0x0011, 0x0011, 0x0013, 0x0013, 0x0015, 0x0015, 0x0017,
		0x0019, 0x0019, 0x001b, 0x001b, 0x001d, 0x001f, 0x001f, 0x0021,
		0x0023, 0x0023, 0x0025, 0x0027, 0x0029, 0x0029, 0x002b, 0x002d,
		0x002f, 0x002f, 0x0031, 0x0033, 0x0035, 0x0037, 0x0039, 0x003b,
		0x003d, 0x003f, 0x0041, 0x0043, 0x0045, 0x0047, 0x0049, 0x004b,
		0x004d, 0x004f, 0x0051, 0x0053, 0x0055, 0x0057, 0x005b, 0x005d,
		0x005f, 0x0061, 0x0063, 0x0067, 0x0069, 0x006b, 0x006f, 0x0071,
		0x0073, 0x0077, 0x0079, 0x007d, 0x007f, 0x0081, 0x0085, 0x0087,
		0x008b, 0x008d, 0x0091, 0x0095, 0x0097, 0x009b, 0x009d, 0x00a1,
		0x00a5, 0x00a7, 0x00ab, 0x00af, 0x00b3, 0x00b7, 0x00b9, 0x00bd,
		0x00c1, 0x00c5, 0x00c9, 0x00cd, 0x00d1, 0x00d5, 0x00d9, 0x00dd,
		0x00e1, 0x00e5, 0x00e9, 0x00ed, 0x00f1, 0x00f5, 0x00fb, 0x00ff,
		0x0103, 0x0107, 0x010d, 0x0111, 0x0115, 0x011b, 0x011f, 0x0125,
		0x0129, 0x012f, 0x0133, 0x0139, 0x013f, 0x0143, 0x0149, 0x014f,
		0x0153, 0x0159, 0x015f, 0x0165, 0x016b, 0x0171, 0x0177, 0x017d,
		0x0183, 0x0189, 0x018f, 0x0195, 0x019b, 0x01a3, 0x01a9, 0x01af,
		0x01b7, 0x01bd, 0x01c5, 0x01cb, 0x01d3, 0x01d9, 0x01e1, 0x01e9,
		0x01f1, 0x01f7, 0x01ff, 0x0207, 0x020f, 0x0217, 0x021f, 0x0229,
		0x0231, 0x0239, 0x0243, 0x024b, 0x0253, 0x025d, 0x0267, 0x026f,
		0x0279, 0x0283, 0x028d, 0x0297, 0x02a1, 0x02ab, 0x02b7, 0x02c1,
		0x02cd, 0x02d7, 0x02e3, 0x02ef, 0x02f9, 0x0305, 0x0311, 0x031f,
		0x032b, 0x0337, 0x0345, 0x0353, 0x0361, 0x036f, 0x037d, 0x038b,
		0x039b, 0x03a9, 0x03b9, 0x03c9, 0x03d9, 0x03eb, 0x03fb, 0x040d,
		0x041f, 0x0431, 0x0445, 0x0459, 0x046d, 0x0481, 0x0497, 0x04ad,
		0x04c3, 0x04db, 0x04f3, 0x050d, 0x0527, 0x0541, 0x055f, 0x057b,
		0x059b, 0x05b9, 0x05db, 0x05ff, 0x0623, 0x0649, 0x0673, 0x069d,
		0x06cb, 0x06fd, 0x0731, 0x076b, 0x07a7, 0x07eb, 0x0835, 0x0887,
		0x08e3, 0x094d, 0x09c9, 0x0a5d, 0x0b17, 0x0c0f, 0x0d87, 0x10b3,
};

/* sustain level table (3dB per step) */
/* bit0, bit1, bit2, bit3, bit4, bit5, bit6 */
/* 1,    2,    4,    8,    16,   32,   64   (value)*/
/* 0.75, 1.5,  3,    6,    12,   24,   48   (dB)*/

/* 0 - 15: 0, 3, 6, 9,12,15,18,21,24,27,30,33,36,39,42,93 (dB)*/
/* attenuation value (10 bits) = (SL << 2) << 3 */
#define SC(db) (UINT32) ( db * (4.0/ENV_STEP) )
static const UINT32 sl_table[16]={
 SC( 0),SC( 1),SC( 2),SC(3 ),SC(4 ),SC(5 ),SC(6 ),SC( 7),
 SC( 8),SC( 9),SC(10),SC(11),SC(12),SC(13),SC(14),SC(31)
};
#undef SC


#define RATE_STEPS (8)
static const UINT8 eg_inc[19*RATE_STEPS]={

/*cycle:0 1  2 3  4 5  6 7*/

/* 0 */ 0,1, 0,1, 0,1, 0,1, /* rates 00..11 0 (increment by 0 or 1) */
/* 1 */ 0,1, 0,1, 1,1, 0,1, /* rates 00..11 1 */
/* 2 */ 0,1, 1,1, 0,1, 1,1, /* rates 00..11 2 */
/* 3 */ 0,1, 1,1, 1,1, 1,1, /* rates 00..11 3 */

/* 4 */ 1,1, 1,1, 1,1, 1,1, /* rate 12 0 (increment by 1) */
/* 5 */ 1,1, 1,2, 1,1, 1,2, /* rate 12 1 */
/* 6 */ 1,2, 1,2, 1,2, 1,2, /* rate 12 2 */
/* 7 */ 1,2, 2,2, 1,2, 2,2, /* rate 12 3 */

/* 8 */ 2,2, 2,2, 2,2, 2,2, /* rate 13 0 (increment by 2) */
/* 9 */ 2,2, 2,4, 2,2, 2,4, /* rate 13 1 */
/*10 */ 2,4, 2,4, 2,4, 2,4, /* rate 13 2 */
/*11 */ 2,4, 4,4, 2,4, 4,4, /* rate 13 3 */

/*12 */ 4,4, 4,4, 4,4, 4,4, /* rate 14 0 (increment by 4) */
/*13 */ 4,4, 4,8, 4,4, 4,8, /* rate 14 1 */
/*14 */ 4,8, 4,8, 4,8, 4,8, /* rate 14 2 */
/*15 */ 4,8, 8,8, 4,8, 8,8, /* rate 14 3 */

/*16 */ 8,8, 8,8, 8,8, 8,8, /* rates 15 0, 15 1, 15 2, 15 3 (increment by 8) */
/*17 */ 16,16,16,16,16,16,16,16, /* rates 15 2, 15 3 for attack */
/*18 */ 0,0, 0,0, 0,0, 0,0, /* infinity rates for attack and decay(s) */
};


#define O(a) (a*RATE_STEPS)

/*note that there is no O(17) in this table - it's directly in the code */
static const UINT8 eg_rate_select[32+64+32]={  /* Envelope Generator rates (32 + 64 rates + 32 RKS) */
/* 32 infinite time rates (same as Rate 0) */
O(18),O(18),O(18),O(18),O(18),O(18),O(18),O(18),
O(18),O(18),O(18),O(18),O(18),O(18),O(18),O(18),
O(18),O(18),O(18),O(18),O(18),O(18),O(18),O(18),
O(18),O(18),O(18),O(18),O(18),O(18),O(18),O(18),

/* rates 00-11 */
/*
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
*/
O(18),O(18),O( 0),O( 0),
O( 0),O( 0),O( 2),O( 2),   /* Nemesis's tests */

O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),
O( 0),O( 1),O( 2),O( 3),

/* rate 12 */
O( 4),O( 5),O( 6),O( 7),

/* rate 13 */
O( 8),O( 9),O(10),O(11),

/* rate 14 */
O(12),O(13),O(14),O(15),

/* rate 15 */
O(16),O(16),O(16),O(16),

/* 32 dummy rates (same as 15 3) */
O(16),O(16),O(16),O(16),O(16),O(16),O(16),O(16),
O(16),O(16),O(16),O(16),O(16),O(16),O(16),O(16),
O(16),O(16),O(16),O(16),O(16),O(16),O(16),O(16),
O(16),O(16),O(16),O(16),O(16),O(16),O(16),O(16)

};
#undef O

/*rate  0,    1,    2,   3,   4,   5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15*/
/*shift 11,   10,   9,   8,   7,   6,  5,  4,  3,  2, 1,  0,  0,  0,  0,  0 */
/*mask  2047, 1023, 511, 255, 127, 63, 31, 15, 7,  3, 1,  0,  0,  0,  0,  0 */

#define O(a) (a*1)
static const UINT8 eg_rate_shift[32+64+32]={  /* Envelope Generator counter shifts (32 + 64 rates + 32 RKS) */
/* 32 infinite time rates */
/* O(0),O(0),O(0),O(0),O(0),O(0),O(0),O(0),
O(0),O(0),O(0),O(0),O(0),O(0),O(0),O(0),
O(0),O(0),O(0),O(0),O(0),O(0),O(0),O(0),
O(0),O(0),O(0),O(0),O(0),O(0),O(0),O(0), */

/* fixed (should be the same as rate 0, even if it makes no difference since increment value is 0 for these rates) */
O(11),O(11),O(11),O(11),O(11),O(11),O(11),O(11),
O(11),O(11),O(11),O(11),O(11),O(11),O(11),O(11),
O(11),O(11),O(11),O(11),O(11),O(11),O(11),O(11),
O(11),O(11),O(11),O(11),O(11),O(11),O(11),O(11),

/* rates 00-11 */
O(11),O(11),O(11),O(11),
O(10),O(10),O(10),O(10),
O( 9),O( 9),O( 9),O( 9),
O( 8),O( 8),O( 8),O( 8),
O( 7),O( 7),O( 7),O( 7),
O( 6),O( 6),O( 6),O( 6),
O( 5),O( 5),O( 5),O( 5),
O( 4),O( 4),O( 4),O( 4),
O( 3),O( 3),O( 3),O( 3),
O( 2),O( 2),O( 2),O( 2),
O( 1),O( 1),O( 1),O( 1),
O( 0),O( 0),O( 0),O( 0),

/* rate 12 */
O( 0),O( 0),O( 0),O( 0),

/* rate 13 */
O( 0),O( 0),O( 0),O( 0),

/* rate 14 */
O( 0),O( 0),O( 0),O( 0),

/* rate 15 */
O( 0),O( 0),O( 0),O( 0),

/* 32 dummy rates (same as 15 3) */
O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),
O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),
O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),
O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0),O( 0)

};
#undef O

static const UINT8 dt_tab[4 * 32]={
/* this is YM2151 and YM2612 phase increment data (in 10.10 fixed point format)*/
/* FD=0 */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* FD=1 */
  0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2,
  2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7, 8, 8, 8, 8,
/* FD=2 */
  1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5,
  5, 6, 6, 7, 8, 8, 9,10,11,12,13,14,16,16,16,16,
/* FD=3 */
  2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7,
  8 , 8, 9,10,11,12,13,14,16,17,19,20,22,22,22,22
};


/* OPN key frequency number -> key code follow table */
/* fnum higher 4bit -> keycode lower 2bit */
static const UINT8 opn_fktable[16] = {0,0,0,0,0,0,0,1,2,3,3,3,3,3,3,3};


/* 8 LFO speed parameters */
/* each value represents number of samples that one LFO level will last for */
static const UINT32 lfo_samples_per_step[8] = {108, 77, 71, 67, 62, 44, 8, 5};


/*There are 4 different LFO AM depths available, they are:
  0 dB, 1.4 dB, 5.9 dB, 11.8 dB
  Here is how it is generated (in EG steps):

  11.8 dB = 0, 2, 4, 6, 8, 10,12,14,16...126,126,124,122,120,118,....4,2,0
   5.9 dB = 0, 1, 2, 3, 4, 5, 6, 7, 8....63, 63, 62, 61, 60, 59,.....2,1,0
   1.4 dB = 0, 0, 0, 0, 1, 1, 1, 1, 2,...15, 15, 15, 15, 14, 14,.....0,0,0

  (1.4 dB is loosing precision as you can see)

  It's implemented as generator from 0..126 with step 2 then a shift
  right N times, where N is:
    8 for 0 dB
    3 for 1.4 dB
    1 for 5.9 dB
    0 for 11.8 dB
*/
static const UINT8 lfo_ams_depth_shift[4] = {8, 3, 1, 0};



/*There are 8 different LFO PM depths available, they are:
  0, 3.4, 6.7, 10, 14, 20, 40, 80 (cents)

  Modulation level at each depth depends on F-NUMBER bits: 4,5,6,7,8,9,10
  (bits 8,9,10 = FNUM MSB from OCT/FNUM register)

  Here we store only first quarter (positive one) of full waveform.
  Full table (lfo_pm_table) containing all 128 waveforms is build
  at run (init) time.

  One value in table below represents 4 (four) basic LFO steps
  (1 PM step = 4 AM steps).

  For example:
   at LFO SPEED=0 (which is 108 samples per basic LFO step)
   one value from "lfo_pm_output" table lasts for 432 consecutive
   samples (4*108=432) and one full LFO waveform cycle lasts for 13824
   samples (32*432=13824; 32 because we store only a quarter of whole
            waveform in the table below)
*/
static const UINT8 lfo_pm_output[7*8][8]={
/* 7 bits meaningful (of F-NUMBER), 8 LFO output levels per one depth (out of 32), 8 LFO depths */
/* FNUM BIT 4: 000 0001xxxx */
/* DEPTH 0 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 1 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 2 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 3 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 4 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 5 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 6 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 7 */ {0,   0,   0,   0,   1,   1,   1,   1},

/* FNUM BIT 5: 000 0010xxxx */
/* DEPTH 0 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 1 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 2 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 3 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 4 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 5 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 6 */ {0,   0,   0,   0,   1,   1,   1,   1},
/* DEPTH 7 */ {0,   0,   1,   1,   2,   2,   2,   3},

/* FNUM BIT 6: 000 0100xxxx */
/* DEPTH 0 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 1 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 2 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 3 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 4 */ {0,   0,   0,   0,   0,   0,   0,   1},
/* DEPTH 5 */ {0,   0,   0,   0,   1,   1,   1,   1},
/* DEPTH 6 */ {0,   0,   1,   1,   2,   2,   2,   3},
/* DEPTH 7 */ {0,   0,   2,   3,   4,   4,   5,   6},

/* FNUM BIT 7: 000 1000xxxx */
/* DEPTH 0 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 1 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 2 */ {0,   0,   0,   0,   0,   0,   1,   1},
/* DEPTH 3 */ {0,   0,   0,   0,   1,   1,   1,   1},
/* DEPTH 4 */ {0,   0,   0,   1,   1,   1,   1,   2},
/* DEPTH 5 */ {0,   0,   1,   1,   2,   2,   2,   3},
/* DEPTH 6 */ {0,   0,   2,   3,   4,   4,   5,   6},
/* DEPTH 7 */ {0,   0,   4,   6,   8,   8, 0xa, 0xc},

/* FNUM BIT 8: 001 0000xxxx */
/* DEPTH 0 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 1 */ {0,   0,   0,   0,   1,   1,   1,   1},
/* DEPTH 2 */ {0,   0,   0,   1,   1,   1,   2,   2},
/* DEPTH 3 */ {0,   0,   1,   1,   2,   2,   3,   3},
/* DEPTH 4 */ {0,   0,   1,   2,   2,   2,   3,   4},
/* DEPTH 5 */ {0,   0,   2,   3,   4,   4,   5,   6},
/* DEPTH 6 */ {0,   0,   4,   6,   8,   8, 0xa, 0xc},
/* DEPTH 7 */ {0,   0,   8, 0xc,0x10,0x10,0x14,0x18},

/* FNUM BIT 9: 010 0000xxxx */
/* DEPTH 0 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 1 */ {0,   0,   0,   0,   2,   2,   2,   2},
/* DEPTH 2 */ {0,   0,   0,   2,   2,   2,   4,   4},
/* DEPTH 3 */ {0,   0,   2,   2,   4,   4,   6,   6},
/* DEPTH 4 */ {0,   0,   2,   4,   4,   4,   6,   8},
/* DEPTH 5 */ {0,   0,   4,   6,   8,   8, 0xa, 0xc},
/* DEPTH 6 */ {0,   0,   8, 0xc,0x10,0x10,0x14,0x18},
/* DEPTH 7 */ {0,   0,0x10,0x18,0x20,0x20,0x28,0x30},

/* FNUM BIT10: 100 0000xxxx */
/* DEPTH 0 */ {0,   0,   0,   0,   0,   0,   0,   0},
/* DEPTH 1 */ {0,   0,   0,   0,   4,   4,   4,   4},
/* DEPTH 2 */ {0,   0,   0,   4,   4,   4,   8,   8},
/* DEPTH 3 */ {0,   0,   4,   4,   8,   8, 0xc, 0xc},
/* DEPTH 4 */ {0,   0,   4,   8,   8,   8, 0xc,0x10},
/* DEPTH 5 */ {0,   0,   8, 0xc,0x10,0x10,0x14,0x18},
/* DEPTH 6 */ {0,   0,0x10,0x18,0x20,0x20,0x28,0x30},
/* DEPTH 7 */ {0,   0,0x20,0x30,0x40,0x40,0x50,0x60},

};

/* all 128 LFO PM waveforms */
const static INT16 lfo_pm_table[128*8*32] = { /* 128 combinations of 7 bits meaningful (of F-NUMBER), 8 LFO depths, 32 LFO output levels per one depth */
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			3,      2,      2,      2,      1,      1,      0,      0,
			0,      0,      1,      1,      2,      2,      2,      3,
			-3,     -2,     -2,     -2,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -2,     -2,     -2,     -3,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			4,      3,      3,      3,      1,      1,      0,      0,
			0,      0,      1,      1,      3,      3,      3,      4,
			-4,     -3,     -3,     -3,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -3,     -3,     -3,     -4,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      1,
			-1,     0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      -1,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			3,      2,      2,      2,      1,      1,      0,      0,
			0,      0,      1,      1,      2,      2,      2,      3,
			-3,     -2,     -2,     -2,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -2,     -2,     -2,     -3,
			6,      5,      4,      4,      3,      2,      0,      0,
			0,      0,      2,      3,      4,      4,      5,      6,
			-6,     -5,     -4,     -4,     -3,     -2,     0,      0,
			0,      0,      -2,     -3,     -4,     -4,     -5,     -6,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      1,
			-1,     0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      -1,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			3,      2,      2,      2,      1,      1,      0,      0,
			0,      0,      1,      1,      2,      2,      2,      3,
			-3,     -2,     -2,     -2,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -2,     -2,     -2,     -3,
			7,      6,      5,      5,      3,      2,      0,      0,
			0,      0,      2,      3,      5,      5,      6,      7,
			-7,     -6,     -5,     -5,     -3,     -2,     0,      0,
			0,      0,      -2,     -3,     -5,     -5,     -6,     -7,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      1,
			-1,     0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      -1,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			4,      3,      3,      3,      1,      1,      0,      0,
			0,      0,      1,      1,      3,      3,      3,      4,
			-4,     -3,     -3,     -3,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -3,     -3,     -3,     -4,
			9,      7,      6,      6,      4,      3,      0,      0,
			0,      0,      3,      4,      6,      6,      7,      9,
			-9,     -7,     -6,     -6,     -4,     -3,     0,      0,
			0,      0,      -3,     -4,     -6,     -6,     -7,     -9,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      1,
			-1,     0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      -1,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			4,      3,      3,      3,      1,      1,      0,      0,
			0,      0,      1,      1,      3,      3,      3,      4,
			-4,     -3,     -3,     -3,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -3,     -3,     -3,     -4,
			10,     8,      7,      7,      4,      3,      0,      0,
			0,      0,      3,      4,      7,      7,      8,      10,
			-10,    -8,     -7,     -7,     -4,     -3,     0,      0,
			0,      0,      -3,     -4,     -7,     -7,     -8,     -10,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      1,      1,
			-1,     -1,     0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      -1,     -1,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			2,      1,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      1,      2,
			-2,     -1,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -1,     -2,
			3,      2,      2,      2,      1,      1,      0,      0,
			0,      0,      1,      1,      2,      2,      2,      3,
			-3,     -2,     -2,     -2,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -2,     -2,     -2,     -3,
			6,      5,      4,      4,      3,      2,      0,      0,
			0,      0,      2,      3,      4,      4,      5,      6,
			-6,     -5,     -4,     -4,     -3,     -2,     0,      0,
			0,      0,      -2,     -3,     -4,     -4,     -5,     -6,
			12,     10,     8,      8,      6,      4,      0,      0,
			0,      0,      4,      6,      8,      8,      10,     12,
			-12,    -10,    -8,     -8,     -6,     -4,     0,      0,
			0,      0,      -4,     -6,     -8,     -8,     -10,    -12,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      1,      1,
			-1,     -1,     0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      -1,     -1,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			2,      1,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      1,      2,
			-2,     -1,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -1,     -2,
			3,      2,      2,      2,      1,      1,      0,      0,
			0,      0,      1,      1,      2,      2,      2,      3,
			-3,     -2,     -2,     -2,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -2,     -2,     -2,     -3,
			6,      5,      4,      4,      3,      2,      0,      0,
			0,      0,      2,      3,      4,      4,      5,      6,
			-6,     -5,     -4,     -4,     -3,     -2,     0,      0,
			0,      0,      -2,     -3,     -4,     -4,     -5,     -6,
			13,     11,     9,      9,      6,      4,      0,      0,
			0,      0,      4,      6,      9,      9,      11,     13,
			-13,    -11,    -9,     -9,     -6,     -4,     0,      0,
			0,      0,      -4,     -6,     -9,     -9,     -11,    -13,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      1,      1,
			-1,     -1,     0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      -1,     -1,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			2,      1,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      1,      2,
			-2,     -1,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -1,     -2,
			3,      2,      2,      2,      1,      1,      0,      0,
			0,      0,      1,      1,      2,      2,      2,      3,
			-3,     -2,     -2,     -2,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -2,     -2,     -2,     -3,
			7,      6,      5,      5,      3,      2,      0,      0,
			0,      0,      2,      3,      5,      5,      6,      7,
			-7,     -6,     -5,     -5,     -3,     -2,     0,      0,
			0,      0,      -2,     -3,     -5,     -5,     -6,     -7,
			15,     12,     10,     10,     7,      5,      0,      0,
			0,      0,      5,      7,      10,     10,     12,     15,
			-15,    -12,    -10,    -10,    -7,     -5,     0,      0,
			0,      0,      -5,     -7,     -10,    -10,    -12,    -15,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      1,      1,
			-1,     -1,     0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      -1,     -1,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			2,      1,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      1,      2,
			-2,     -1,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -1,     -2,
			3,      2,      2,      2,      1,      1,      0,      0,
			0,      0,      1,      1,      2,      2,      2,      3,
			-3,     -2,     -2,     -2,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -2,     -2,     -2,     -3,
			7,      6,      5,      5,      3,      2,      0,      0,
			0,      0,      2,      3,      5,      5,      6,      7,
			-7,     -6,     -5,     -5,     -3,     -2,     0,      0,
			0,      0,      -2,     -3,     -5,     -5,     -6,     -7,
			16,     13,     11,     11,     7,      5,      0,      0,
			0,      0,      5,      7,      11,     11,     13,     16,
			-16,    -13,    -11,    -11,    -7,     -5,     0,      0,
			0,      0,      -5,     -7,     -11,    -11,    -13,    -16,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      1,      1,
			-1,     -1,     0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      -1,     -1,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			3,      1,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      1,      3,
			-3,     -1,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -1,     -3,
			4,      3,      3,      3,      1,      1,      0,      0,
			0,      0,      1,      1,      3,      3,      3,      4,
			-4,     -3,     -3,     -3,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -3,     -3,     -3,     -4,
			9,      7,      6,      6,      4,      3,      0,      0,
			0,      0,      3,      4,      6,      6,      7,      9,
			-9,     -7,     -6,     -6,     -4,     -3,     0,      0,
			0,      0,      -3,     -4,     -6,     -6,     -7,     -9,
			18,     15,     12,     12,     9,      6,      0,      0,
			0,      0,      6,      9,      12,     12,     15,     18,
			-18,    -15,    -12,    -12,    -9,     -6,     0,      0,
			0,      0,      -6,     -9,     -12,    -12,    -15,    -18,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      1,      1,
			-1,     -1,     0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      -1,     -1,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			3,      1,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      1,      3,
			-3,     -1,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -1,     -3,
			4,      3,      3,      3,      1,      1,      0,      0,
			0,      0,      1,      1,      3,      3,      3,      4,
			-4,     -3,     -3,     -3,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -3,     -3,     -3,     -4,
			9,      7,      6,      6,      4,      3,      0,      0,
			0,      0,      3,      4,      6,      6,      7,      9,
			-9,     -7,     -6,     -6,     -4,     -3,     0,      0,
			0,      0,      -3,     -4,     -6,     -6,     -7,     -9,
			19,     16,     13,     13,     9,      6,      0,      0,
			0,      0,      6,      9,      13,     13,     16,     19,
			-19,    -16,    -13,    -13,    -9,     -6,     0,      0,
			0,      0,      -6,     -9,     -13,    -13,    -16,    -19,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      1,      1,
			-1,     -1,     0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      -1,     -1,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			3,      1,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      1,      3,
			-3,     -1,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -1,     -3,
			4,      3,      3,      3,      1,      1,      0,      0,
			0,      0,      1,      1,      3,      3,      3,      4,
			-4,     -3,     -3,     -3,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -3,     -3,     -3,     -4,
			10,     8,      7,      7,      4,      3,      0,      0,
			0,      0,      3,      4,      7,      7,      8,      10,
			-10,    -8,     -7,     -7,     -4,     -3,     0,      0,
			0,      0,      -3,     -4,     -7,     -7,     -8,     -10,
			21,     17,     14,     14,     10,     7,      0,      0,
			0,      0,      7,      10,     14,     14,     17,     21,
			-21,    -17,    -14,    -14,    -10,    -7,     0,      0,
			0,      0,      -7,     -10,    -14,    -14,    -17,    -21,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      1,      1,
			-1,     -1,     0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      -1,     -1,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			3,      1,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      1,      3,
			-3,     -1,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -1,     -3,
			4,      3,      3,      3,      1,      1,      0,      0,
			0,      0,      1,      1,      3,      3,      3,      4,
			-4,     -3,     -3,     -3,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -3,     -3,     -3,     -4,
			10,     8,      7,      7,      4,      3,      0,      0,
			0,      0,      3,      4,      7,      7,      8,      10,
			-10,    -8,     -7,     -7,     -4,     -3,     0,      0,
			0,      0,      -3,     -4,     -7,     -7,     -8,     -10,
			22,     18,     15,     15,     10,     7,      0,      0,
			0,      0,      7,      10,     15,     15,     18,     22,
			-22,    -18,    -15,    -15,    -10,    -7,     0,      0,
			0,      0,      -7,     -10,    -15,    -15,    -18,    -22,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			2,      2,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      2,      2,
			-2,     -2,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -2,     -2,
			3,      3,      2,      2,      1,      1,      0,      0,
			0,      0,      1,      1,      2,      2,      3,      3,
			-3,     -3,     -2,     -2,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -2,     -2,     -3,     -3,
			4,      3,      2,      2,      2,      1,      0,      0,
			0,      0,      1,      2,      2,      2,      3,      4,
			-4,     -3,     -2,     -2,     -2,     -1,     0,      0,
			0,      0,      -1,     -2,     -2,     -2,     -3,     -4,
			6,      5,      4,      4,      3,      2,      0,      0,
			0,      0,      2,      3,      4,      4,      5,      6,
			-6,     -5,     -4,     -4,     -3,     -2,     0,      0,
			0,      0,      -2,     -3,     -4,     -4,     -5,     -6,
			12,     10,     8,      8,      6,      4,      0,      0,
			0,      0,      4,      6,      8,      8,      10,     12,
			-12,    -10,    -8,     -8,     -6,     -4,     0,      0,
			0,      0,      -4,     -6,     -8,     -8,     -10,    -12,
			24,     20,     16,     16,     12,     8,      0,      0,
			0,      0,      8,      12,     16,     16,     20,     24,
			-24,    -20,    -16,    -16,    -12,    -8,     0,      0,
			0,      0,      -8,     -12,    -16,    -16,    -20,    -24,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			2,      2,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      2,      2,
			-2,     -2,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -2,     -2,
			3,      3,      2,      2,      1,      1,      0,      0,
			0,      0,      1,      1,      2,      2,      3,      3,
			-3,     -3,     -2,     -2,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -2,     -2,     -3,     -3,
			4,      3,      2,      2,      2,      1,      0,      0,
			0,      0,      1,      2,      2,      2,      3,      4,
			-4,     -3,     -2,     -2,     -2,     -1,     0,      0,
			0,      0,      -1,     -2,     -2,     -2,     -3,     -4,
			6,      5,      4,      4,      3,      2,      0,      0,
			0,      0,      2,      3,      4,      4,      5,      6,
			-6,     -5,     -4,     -4,     -3,     -2,     0,      0,
			0,      0,      -2,     -3,     -4,     -4,     -5,     -6,
			12,     10,     8,      8,      6,      4,      0,      0,
			0,      0,      4,      6,      8,      8,      10,     12,
			-12,    -10,    -8,     -8,     -6,     -4,     0,      0,
			0,      0,      -4,     -6,     -8,     -8,     -10,    -12,
			25,     21,     17,     17,     12,     8,      0,      0,
			0,      0,      8,      12,     17,     17,     21,     25,
			-25,    -21,    -17,    -17,    -12,    -8,     0,      0,
			0,      0,      -8,     -12,    -17,    -17,    -21,    -25,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			2,      2,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      2,      2,
			-2,     -2,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -2,     -2,
			3,      3,      2,      2,      1,      1,      0,      0,
			0,      0,      1,      1,      2,      2,      3,      3,
			-3,     -3,     -2,     -2,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -2,     -2,     -3,     -3,
			4,      3,      2,      2,      2,      1,      0,      0,
			0,      0,      1,      2,      2,      2,      3,      4,
			-4,     -3,     -2,     -2,     -2,     -1,     0,      0,
			0,      0,      -1,     -2,     -2,     -2,     -3,     -4,
			6,      5,      4,      4,      3,      2,      0,      0,
			0,      0,      2,      3,      4,      4,      5,      6,
			-6,     -5,     -4,     -4,     -3,     -2,     0,      0,
			0,      0,      -2,     -3,     -4,     -4,     -5,     -6,
			13,     11,     9,      9,      6,      4,      0,      0,
			0,      0,      4,      6,      9,      9,      11,     13,
			-13,    -11,    -9,     -9,     -6,     -4,     0,      0,
			0,      0,      -4,     -6,     -9,     -9,     -11,    -13,
			27,     22,     18,     18,     13,     9,      0,      0,
			0,      0,      9,      13,     18,     18,     22,     27,
			-27,    -22,    -18,    -18,    -13,    -9,     0,      0,
			0,      0,      -9,     -13,    -18,    -18,    -22,    -27,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			2,      2,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      2,      2,
			-2,     -2,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -2,     -2,
			3,      3,      2,      2,      1,      1,      0,      0,
			0,      0,      1,      1,      2,      2,      3,      3,
			-3,     -3,     -2,     -2,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -2,     -2,     -3,     -3,
			4,      3,      2,      2,      2,      1,      0,      0,
			0,      0,      1,      2,      2,      2,      3,      4,
			-4,     -3,     -2,     -2,     -2,     -1,     0,      0,
			0,      0,      -1,     -2,     -2,     -2,     -3,     -4,
			6,      5,      4,      4,      3,      2,      0,      0,
			0,      0,      2,      3,      4,      4,      5,      6,
			-6,     -5,     -4,     -4,     -3,     -2,     0,      0,
			0,      0,      -2,     -3,     -4,     -4,     -5,     -6,
			13,     11,     9,      9,      6,      4,      0,      0,
			0,      0,      4,      6,      9,      9,      11,     13,
			-13,    -11,    -9,     -9,     -6,     -4,     0,      0,
			0,      0,      -4,     -6,     -9,     -9,     -11,    -13,
			28,     23,     19,     19,     13,     9,      0,      0,
			0,      0,      9,      13,     19,     19,     23,     28,
			-28,    -23,    -19,    -19,    -13,    -9,     0,      0,
			0,      0,      -9,     -13,    -19,    -19,    -23,    -28,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			2,      2,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      2,      2,
			-2,     -2,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -2,     -2,
			3,      3,      2,      2,      1,      1,      0,      0,
			0,      0,      1,      1,      2,      2,      3,      3,
			-3,     -3,     -2,     -2,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -2,     -2,     -3,     -3,
			5,      3,      2,      2,      2,      1,      0,      0,
			0,      0,      1,      2,      2,      2,      3,      5,
			-5,     -3,     -2,     -2,     -2,     -1,     0,      0,
			0,      0,      -1,     -2,     -2,     -2,     -3,     -5,
			7,      6,      5,      5,      3,      2,      0,      0,
			0,      0,      2,      3,      5,      5,      6,      7,
			-7,     -6,     -5,     -5,     -3,     -2,     0,      0,
			0,      0,      -2,     -3,     -5,     -5,     -6,     -7,
			15,     12,     10,     10,     7,      5,      0,      0,
			0,      0,      5,      7,      10,     10,     12,     15,
			-15,    -12,    -10,    -10,    -7,     -5,     0,      0,
			0,      0,      -5,     -7,     -10,    -10,    -12,    -15,
			30,     25,     20,     20,     15,     10,     0,      0,
			0,      0,      10,     15,     20,     20,     25,     30,
			-30,    -25,    -20,    -20,    -15,    -10,    0,      0,
			0,      0,      -10,    -15,    -20,    -20,    -25,    -30,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			2,      2,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      2,      2,
			-2,     -2,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -2,     -2,
			3,      3,      2,      2,      1,      1,      0,      0,
			0,      0,      1,      1,      2,      2,      3,      3,
			-3,     -3,     -2,     -2,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -2,     -2,     -3,     -3,
			5,      3,      2,      2,      2,      1,      0,      0,
			0,      0,      1,      2,      2,      2,      3,      5,
			-5,     -3,     -2,     -2,     -2,     -1,     0,      0,
			0,      0,      -1,     -2,     -2,     -2,     -3,     -5,
			7,      6,      5,      5,      3,      2,      0,      0,
			0,      0,      2,      3,      5,      5,      6,      7,
			-7,     -6,     -5,     -5,     -3,     -2,     0,      0,
			0,      0,      -2,     -3,     -5,     -5,     -6,     -7,
			15,     12,     10,     10,     7,      5,      0,      0,
			0,      0,      5,      7,      10,     10,     12,     15,
			-15,    -12,    -10,    -10,    -7,     -5,     0,      0,
			0,      0,      -5,     -7,     -10,    -10,    -12,    -15,
			31,     26,     21,     21,     15,     10,     0,      0,
			0,      0,      10,     15,     21,     21,     26,     31,
			-31,    -26,    -21,    -21,    -15,    -10,    0,      0,
			0,      0,      -10,    -15,    -21,    -21,    -26,    -31,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			2,      2,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      2,      2,
			-2,     -2,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -2,     -2,
			3,      3,      2,      2,      1,      1,      0,      0,
			0,      0,      1,      1,      2,      2,      3,      3,
			-3,     -3,     -2,     -2,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -2,     -2,     -3,     -3,
			5,      3,      2,      2,      2,      1,      0,      0,
			0,      0,      1,      2,      2,      2,      3,      5,
			-5,     -3,     -2,     -2,     -2,     -1,     0,      0,
			0,      0,      -1,     -2,     -2,     -2,     -3,     -5,
			7,      6,      5,      5,      3,      2,      0,      0,
			0,      0,      2,      3,      5,      5,      6,      7,
			-7,     -6,     -5,     -5,     -3,     -2,     0,      0,
			0,      0,      -2,     -3,     -5,     -5,     -6,     -7,
			16,     13,     11,     11,     7,      5,      0,      0,
			0,      0,      5,      7,      11,     11,     13,     16,
			-16,    -13,    -11,    -11,    -7,     -5,     0,      0,
			0,      0,      -5,     -7,     -11,    -11,    -13,    -16,
			33,     27,     22,     22,     16,     11,     0,      0,
			0,      0,      11,     16,     22,     22,     27,     33,
			-33,    -27,    -22,    -22,    -16,    -11,    0,      0,
			0,      0,      -11,    -16,    -22,    -22,    -27,    -33,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			2,      2,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      2,      2,
			-2,     -2,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -2,     -2,
			3,      3,      2,      2,      1,      1,      0,      0,
			0,      0,      1,      1,      2,      2,      3,      3,
			-3,     -3,     -2,     -2,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -2,     -2,     -3,     -3,
			5,      3,      2,      2,      2,      1,      0,      0,
			0,      0,      1,      2,      2,      2,      3,      5,
			-5,     -3,     -2,     -2,     -2,     -1,     0,      0,
			0,      0,      -1,     -2,     -2,     -2,     -3,     -5,
			7,      6,      5,      5,      3,      2,      0,      0,
			0,      0,      2,      3,      5,      5,      6,      7,
			-7,     -6,     -5,     -5,     -3,     -2,     0,      0,
			0,      0,      -2,     -3,     -5,     -5,     -6,     -7,
			16,     13,     11,     11,     7,      5,      0,      0,
			0,      0,      5,      7,      11,     11,     13,     16,
			-16,    -13,    -11,    -11,    -7,     -5,     0,      0,
			0,      0,      -5,     -7,     -11,    -11,    -13,    -16,
			34,     28,     23,     23,     16,     11,     0,      0,
			0,      0,      11,     16,     23,     23,     28,     34,
			-34,    -28,    -23,    -23,    -16,    -11,    0,      0,
			0,      0,      -11,    -16,    -23,    -23,    -28,    -34,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			3,      3,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      3,      3,
			-3,     -3,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -3,     -3,
			4,      4,      3,      3,      1,      1,      0,      0,
			0,      0,      1,      1,      3,      3,      4,      4,
			-4,     -4,     -3,     -3,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -3,     -3,     -4,     -4,
			6,      4,      3,      3,      3,      1,      0,      0,
			0,      0,      1,      3,      3,      3,      4,      6,
			-6,     -4,     -3,     -3,     -3,     -1,     0,      0,
			0,      0,      -1,     -3,     -3,     -3,     -4,     -6,
			9,      7,      6,      6,      4,      3,      0,      0,
			0,      0,      3,      4,      6,      6,      7,      9,
			-9,     -7,     -6,     -6,     -4,     -3,     0,      0,
			0,      0,      -3,     -4,     -6,     -6,     -7,     -9,
			18,     15,     12,     12,     9,      6,      0,      0,
			0,      0,      6,      9,      12,     12,     15,     18,
			-18,    -15,    -12,    -12,    -9,     -6,     0,      0,
			0,      0,      -6,     -9,     -12,    -12,    -15,    -18,
			36,     30,     24,     24,     18,     12,     0,      0,
			0,      0,      12,     18,     24,     24,     30,     36,
			-36,    -30,    -24,    -24,    -18,    -12,    0,      0,
			0,      0,      -12,    -18,    -24,    -24,    -30,    -36,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			3,      3,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      3,      3,
			-3,     -3,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -3,     -3,
			4,      4,      3,      3,      1,      1,      0,      0,
			0,      0,      1,      1,      3,      3,      4,      4,
			-4,     -4,     -3,     -3,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -3,     -3,     -4,     -4,
			6,      4,      3,      3,      3,      1,      0,      0,
			0,      0,      1,      3,      3,      3,      4,      6,
			-6,     -4,     -3,     -3,     -3,     -1,     0,      0,
			0,      0,      -1,     -3,     -3,     -3,     -4,     -6,
			9,      7,      6,      6,      4,      3,      0,      0,
			0,      0,      3,      4,      6,      6,      7,      9,
			-9,     -7,     -6,     -6,     -4,     -3,     0,      0,
			0,      0,      -3,     -4,     -6,     -6,     -7,     -9,
			18,     15,     12,     12,     9,      6,      0,      0,
			0,      0,      6,      9,      12,     12,     15,     18,
			-18,    -15,    -12,    -12,    -9,     -6,     0,      0,
			0,      0,      -6,     -9,     -12,    -12,    -15,    -18,
			37,     31,     25,     25,     18,     12,     0,      0,
			0,      0,      12,     18,     25,     25,     31,     37,
			-37,    -31,    -25,    -25,    -18,    -12,    0,      0,
			0,      0,      -12,    -18,    -25,    -25,    -31,    -37,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			3,      3,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      3,      3,
			-3,     -3,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -3,     -3,
			4,      4,      3,      3,      1,      1,      0,      0,
			0,      0,      1,      1,      3,      3,      4,      4,
			-4,     -4,     -3,     -3,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -3,     -3,     -4,     -4,
			6,      4,      3,      3,      3,      1,      0,      0,
			0,      0,      1,      3,      3,      3,      4,      6,
			-6,     -4,     -3,     -3,     -3,     -1,     0,      0,
			0,      0,      -1,     -3,     -3,     -3,     -4,     -6,
			9,      7,      6,      6,      4,      3,      0,      0,
			0,      0,      3,      4,      6,      6,      7,      9,
			-9,     -7,     -6,     -6,     -4,     -3,     0,      0,
			0,      0,      -3,     -4,     -6,     -6,     -7,     -9,
			19,     16,     13,     13,     9,      6,      0,      0,
			0,      0,      6,      9,      13,     13,     16,     19,
			-19,    -16,    -13,    -13,    -9,     -6,     0,      0,
			0,      0,      -6,     -9,     -13,    -13,    -16,    -19,
			39,     32,     26,     26,     19,     13,     0,      0,
			0,      0,      13,     19,     26,     26,     32,     39,
			-39,    -32,    -26,    -26,    -19,    -13,    0,      0,
			0,      0,      -13,    -19,    -26,    -26,    -32,    -39,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			3,      3,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      3,      3,
			-3,     -3,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -3,     -3,
			4,      4,      3,      3,      1,      1,      0,      0,
			0,      0,      1,      1,      3,      3,      4,      4,
			-4,     -4,     -3,     -3,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -3,     -3,     -4,     -4,
			6,      4,      3,      3,      3,      1,      0,      0,
			0,      0,      1,      3,      3,      3,      4,      6,
			-6,     -4,     -3,     -3,     -3,     -1,     0,      0,
			0,      0,      -1,     -3,     -3,     -3,     -4,     -6,
			9,      7,      6,      6,      4,      3,      0,      0,
			0,      0,      3,      4,      6,      6,      7,      9,
			-9,     -7,     -6,     -6,     -4,     -3,     0,      0,
			0,      0,      -3,     -4,     -6,     -6,     -7,     -9,
			19,     16,     13,     13,     9,      6,      0,      0,
			0,      0,      6,      9,      13,     13,     16,     19,
			-19,    -16,    -13,    -13,    -9,     -6,     0,      0,
			0,      0,      -6,     -9,     -13,    -13,    -16,    -19,
			40,     33,     27,     27,     19,     13,     0,      0,
			0,      0,      13,     19,     27,     27,     33,     40,
			-40,    -33,    -27,    -27,    -19,    -13,    0,      0,
			0,      0,      -13,    -19,    -27,    -27,    -33,    -40,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			3,      3,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      3,      3,
			-3,     -3,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -3,     -3,
			4,      4,      3,      3,      1,      1,      0,      0,
			0,      0,      1,      1,      3,      3,      4,      4,
			-4,     -4,     -3,     -3,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -3,     -3,     -4,     -4,
			7,      4,      3,      3,      3,      1,      0,      0,
			0,      0,      1,      3,      3,      3,      4,      7,
			-7,     -4,     -3,     -3,     -3,     -1,     0,      0,
			0,      0,      -1,     -3,     -3,     -3,     -4,     -7,
			10,     8,      7,      7,      4,      3,      0,      0,
			0,      0,      3,      4,      7,      7,      8,      10,
			-10,    -8,     -7,     -7,     -4,     -3,     0,      0,
			0,      0,      -3,     -4,     -7,     -7,     -8,     -10,
			21,     17,     14,     14,     10,     7,      0,      0,
			0,      0,      7,      10,     14,     14,     17,     21,
			-21,    -17,    -14,    -14,    -10,    -7,     0,      0,
			0,      0,      -7,     -10,    -14,    -14,    -17,    -21,
			42,     35,     28,     28,     21,     14,     0,      0,
			0,      0,      14,     21,     28,     28,     35,     42,
			-42,    -35,    -28,    -28,    -21,    -14,    0,      0,
			0,      0,      -14,    -21,    -28,    -28,    -35,    -42,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			3,      3,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      3,      3,
			-3,     -3,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -3,     -3,
			4,      4,      3,      3,      1,      1,      0,      0,
			0,      0,      1,      1,      3,      3,      4,      4,
			-4,     -4,     -3,     -3,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -3,     -3,     -4,     -4,
			7,      4,      3,      3,      3,      1,      0,      0,
			0,      0,      1,      3,      3,      3,      4,      7,
			-7,     -4,     -3,     -3,     -3,     -1,     0,      0,
			0,      0,      -1,     -3,     -3,     -3,     -4,     -7,
			10,     8,      7,      7,      4,      3,      0,      0,
			0,      0,      3,      4,      7,      7,      8,      10,
			-10,    -8,     -7,     -7,     -4,     -3,     0,      0,
			0,      0,      -3,     -4,     -7,     -7,     -8,     -10,
			21,     17,     14,     14,     10,     7,      0,      0,
			0,      0,      7,      10,     14,     14,     17,     21,
			-21,    -17,    -14,    -14,    -10,    -7,     0,      0,
			0,      0,      -7,     -10,    -14,    -14,    -17,    -21,
			43,     36,     29,     29,     21,     14,     0,      0,
			0,      0,      14,     21,     29,     29,     36,     43,
			-43,    -36,    -29,    -29,    -21,    -14,    0,      0,
			0,      0,      -14,    -21,    -29,    -29,    -36,    -43,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			3,      3,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      3,      3,
			-3,     -3,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -3,     -3,
			4,      4,      3,      3,      1,      1,      0,      0,
			0,      0,      1,      1,      3,      3,      4,      4,
			-4,     -4,     -3,     -3,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -3,     -3,     -4,     -4,
			7,      4,      3,      3,      3,      1,      0,      0,
			0,      0,      1,      3,      3,      3,      4,      7,
			-7,     -4,     -3,     -3,     -3,     -1,     0,      0,
			0,      0,      -1,     -3,     -3,     -3,     -4,     -7,
			10,     8,      7,      7,      4,      3,      0,      0,
			0,      0,      3,      4,      7,      7,      8,      10,
			-10,    -8,     -7,     -7,     -4,     -3,     0,      0,
			0,      0,      -3,     -4,     -7,     -7,     -8,     -10,
			22,     18,     15,     15,     10,     7,      0,      0,
			0,      0,      7,      10,     15,     15,     18,     22,
			-22,    -18,    -15,    -15,    -10,    -7,     0,      0,
			0,      0,      -7,     -10,    -15,    -15,    -18,    -22,
			45,     37,     30,     30,     22,     15,     0,      0,
			0,      0,      15,     22,     30,     30,     37,     45,
			-45,    -37,    -30,    -30,    -22,    -15,    0,      0,
			0,      0,      -15,    -22,    -30,    -30,    -37,    -45,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			1,      1,      1,      1,      0,      0,      0,      0,
			0,      0,      0,      0,      1,      1,      1,      1,
			-1,     -1,     -1,     -1,     0,      0,      0,      0,
			0,      0,      0,      0,      -1,     -1,     -1,     -1,
			3,      3,      1,      1,      1,      0,      0,      0,
			0,      0,      0,      1,      1,      1,      3,      3,
			-3,     -3,     -1,     -1,     -1,     0,      0,      0,
			0,      0,      0,      -1,     -1,     -1,     -3,     -3,
			4,      4,      3,      3,      1,      1,      0,      0,
			0,      0,      1,      1,      3,      3,      4,      4,
			-4,     -4,     -3,     -3,     -1,     -1,     0,      0,
			0,      0,      -1,     -1,     -3,     -3,     -4,     -4,
			7,      4,      3,      3,      3,      1,      0,      0,
			0,      0,      1,      3,      3,      3,      4,      7,
			-7,     -4,     -3,     -3,     -3,     -1,     0,      0,
			0,      0,      -1,     -3,     -3,     -3,     -4,     -7,
			10,     8,      7,      7,      4,      3,      0,      0,
			0,      0,      3,      4,      7,      7,      8,      10,
			-10,    -8,     -7,     -7,     -4,     -3,     0,      0,
			0,      0,      -3,     -4,     -7,     -7,     -8,     -10,
			22,     18,     15,     15,     10,     7,      0,      0,
			0,      0,      7,      10,     15,     15,     18,     22,
			-22,    -18,    -15,    -15,    -10,    -7,     0,      0,
			0,      0,      -7,     -10,    -15,    -15,    -18,    -22,
			46,     38,     31,     31,     22,     15,     0,      0,
			0,      0,      15,     22,     31,     31,     38,     46,
			-46,    -38,    -31,    -31,    -22,    -15,    0,      0,
			0,      0,      -15,    -22,    -31,    -31,    -38,    -46,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			2,      2,      2,      2,      0,      0,      0,      0,
			0,      0,      0,      0,      2,      2,      2,      2,
			-2,     -2,     -2,     -2,     0,      0,      0,      0,
			0,      0,      0,      0,      -2,     -2,     -2,     -2,
			4,      4,      2,      2,      2,      0,      0,      0,
			0,      0,      0,      2,      2,      2,      4,      4,
			-4,     -4,     -2,     -2,     -2,     0,      0,      0,
			0,      0,      0,      -2,     -2,     -2,     -4,     -4,
			6,      6,      4,      4,      2,      2,      0,      0,
			0,      0,      2,      2,      4,      4,      6,      6,
			-6,     -6,     -4,     -4,     -2,     -2,     0,      0,
			0,      0,      -2,     -2,     -4,     -4,     -6,     -6,
			8,      6,      4,      4,      4,      2,      0,      0,
			0,      0,      2,      4,      4,      4,      6,      8,
			-8,     -6,     -4,     -4,     -4,     -2,     0,      0,
			0,      0,      -2,     -4,     -4,     -4,     -6,     -8,
			12,     10,     8,      8,      6,      4,      0,      0,
			0,      0,      4,      6,      8,      8,      10,     12,
			-12,    -10,    -8,     -8,     -6,     -4,     0,      0,
			0,      0,      -4,     -6,     -8,     -8,     -10,    -12,
			24,     20,     16,     16,     12,     8,      0,      0,
			0,      0,      8,      12,     16,     16,     20,     24,
			-24,    -20,    -16,    -16,    -12,    -8,     0,      0,
			0,      0,      -8,     -12,    -16,    -16,    -20,    -24,
			48,     40,     32,     32,     24,     16,     0,      0,
			0,      0,      16,     24,     32,     32,     40,     48,
			-48,    -40,    -32,    -32,    -24,    -16,    0,      0,
			0,      0,      -16,    -24,    -32,    -32,    -40,    -48,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			2,      2,      2,      2,      0,      0,      0,      0,
			0,      0,      0,      0,      2,      2,      2,      2,
			-2,     -2,     -2,     -2,     0,      0,      0,      0,
			0,      0,      0,      0,      -2,     -2,     -2,     -2,
			4,      4,      2,      2,      2,      0,      0,      0,
			0,      0,      0,      2,      2,      2,      4,      4,
			-4,     -4,     -2,     -2,     -2,     0,      0,      0,
			0,      0,      0,      -2,     -2,     -2,     -4,     -4,
			6,      6,      4,      4,      2,      2,      0,      0,
			0,      0,      2,      2,      4,      4,      6,      6,
			-6,     -6,     -4,     -4,     -2,     -2,     0,      0,
			0,      0,      -2,     -2,     -4,     -4,     -6,     -6,
			8,      6,      4,      4,      4,      2,      0,      0,
			0,      0,      2,      4,      4,      4,      6,      8,
			-8,     -6,     -4,     -4,     -4,     -2,     0,      0,
			0,      0,      -2,     -4,     -4,     -4,     -6,     -8,
			12,     10,     8,      8,      6,      4,      0,      0,
			0,      0,      4,      6,      8,      8,      10,     12,
			-12,    -10,    -8,     -8,     -6,     -4,     0,      0,
			0,      0,      -4,     -6,     -8,     -8,     -10,    -12,
			24,     20,     16,     16,     12,     8,      0,      0,
			0,      0,      8,      12,     16,     16,     20,     24,
			-24,    -20,    -16,    -16,    -12,    -8,     0,      0,
			0,      0,      -8,     -12,    -16,    -16,    -20,    -24,
			49,     41,     33,     33,     24,     16,     0,      0,
			0,      0,      16,     24,     33,     33,     41,     49,
			-49,    -41,    -33,    -33,    -24,    -16,    0,      0,
			0,      0,      -16,    -24,    -33,    -33,    -41,    -49,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			2,      2,      2,      2,      0,      0,      0,      0,
			0,      0,      0,      0,      2,      2,      2,      2,
			-2,     -2,     -2,     -2,     0,      0,      0,      0,
			0,      0,      0,      0,      -2,     -2,     -2,     -2,
			4,      4,      2,      2,      2,      0,      0,      0,
			0,      0,      0,      2,      2,      2,      4,      4,
			-4,     -4,     -2,     -2,     -2,     0,      0,      0,
			0,      0,      0,      -2,     -2,     -2,     -4,     -4,
			6,      6,      4,      4,      2,      2,      0,      0,
			0,      0,      2,      2,      4,      4,      6,      6,
			-6,     -6,     -4,     -4,     -2,     -2,     0,      0,
			0,      0,      -2,     -2,     -4,     -4,     -6,     -6,
			8,      6,      4,      4,      4,      2,      0,      0,
			0,      0,      2,      4,      4,      4,      6,      8,
			-8,     -6,     -4,     -4,     -4,     -2,     0,      0,
			0,      0,      -2,     -4,     -4,     -4,     -6,     -8,
			12,     10,     8,      8,      6,      4,      0,      0,
			0,      0,      4,      6,      8,      8,      10,     12,
			-12,    -10,    -8,     -8,     -6,     -4,     0,      0,
			0,      0,      -4,     -6,     -8,     -8,     -10,    -12,
			25,     21,     17,     17,     12,     8,      0,      0,
			0,      0,      8,      12,     17,     17,     21,     25,
			-25,    -21,    -17,    -17,    -12,    -8,     0,      0,
			0,      0,      -8,     -12,    -17,    -17,    -21,    -25,
			51,     42,     34,     34,     25,     17,     0,      0,
			0,      0,      17,     25,     34,     34,     42,     51,
			-51,    -42,    -34,    -34,    -25,    -17,    0,      0,
			0,      0,      -17,    -25,    -34,    -34,    -42,    -51,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			2,      2,      2,      2,      0,      0,      0,      0,
			0,      0,      0,      0,      2,      2,      2,      2,
			-2,     -2,     -2,     -2,     0,      0,      0,      0,
			0,      0,      0,      0,      -2,     -2,     -2,     -2,
			4,      4,      2,      2,      2,      0,      0,      0,
			0,      0,      0,      2,      2,      2,      4,      4,
			-4,     -4,     -2,     -2,     -2,     0,      0,      0,
			0,      0,      0,      -2,     -2,     -2,     -4,     -4,
			6,      6,      4,      4,      2,      2,      0,      0,
			0,      0,      2,      2,      4,      4,      6,      6,
			-6,     -6,     -4,     -4,     -2,     -2,     0,      0,
			0,      0,      -2,     -2,     -4,     -4,     -6,     -6,
			8,      6,      4,      4,      4,      2,      0,      0,
			0,      0,      2,      4,      4,      4,      6,      8,
			-8,     -6,     -4,     -4,     -4,     -2,     0,      0,
			0,      0,      -2,     -4,     -4,     -4,     -6,     -8,
			12,     10,     8,      8,      6,      4,      0,      0,
			0,      0,      4,      6,      8,      8,      10,     12,
			-12,    -10,    -8,     -8,     -6,     -4,     0,      0,
			0,      0,      -4,     -6,     -8,     -8,     -10,    -12,
			25,     21,     17,     17,     12,     8,      0,      0,
			0,      0,      8,      12,     17,     17,     21,     25,
			-25,    -21,    -17,    -17,    -12,    -8,     0,      0,
			0,      0,      -8,     -12,    -17,    -17,    -21,    -25,
			52,     43,     35,     35,     25,     17,     0,      0,
			0,      0,      17,     25,     35,     35,     43,     52,
			-52,    -43,    -35,    -35,    -25,    -17,    0,      0,
			0,      0,      -17,    -25,    -35,    -35,    -43,    -52,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			2,      2,      2,      2,      0,      0,      0,      0,
			0,      0,      0,      0,      2,      2,      2,      2,
			-2,     -2,     -2,     -2,     0,      0,      0,      0,
			0,      0,      0,      0,      -2,     -2,     -2,     -2,
			4,      4,      2,      2,      2,      0,      0,      0,
			0,      0,      0,      2,      2,      2,      4,      4,
			-4,     -4,     -2,     -2,     -2,     0,      0,      0,
			0,      0,      0,      -2,     -2,     -2,     -4,     -4,
			6,      6,      4,      4,      2,      2,      0,      0,
			0,      0,      2,      2,      4,      4,      6,      6,
			-6,     -6,     -4,     -4,     -2,     -2,     0,      0,
			0,      0,      -2,     -2,     -4,     -4,     -6,     -6,
			9,      6,      4,      4,      4,      2,      0,      0,
			0,      0,      2,      4,      4,      4,      6,      9,
			-9,     -6,     -4,     -4,     -4,     -2,     0,      0,
			0,      0,      -2,     -4,     -4,     -4,     -6,     -9,
			13,     11,     9,      9,      6,      4,      0,      0,
			0,      0,      4,      6,      9,      9,      11,     13,
			-13,    -11,    -9,     -9,     -6,     -4,     0,      0,
			0,      0,      -4,     -6,     -9,     -9,     -11,    -13,
			27,     22,     18,     18,     13,     9,      0,      0,
			0,      0,      9,      13,     18,     18,     22,     27,
			-27,    -22,    -18,    -18,    -13,    -9,     0,      0,
			0,      0,      -9,     -13,    -18,    -18,    -22,    -27,
			54,     45,     36,     36,     27,     18,     0,      0,
			0,      0,      18,     27,     36,     36,     45,     54,
			-54,    -45,    -36,    -36,    -27,    -18,    0,      0,
			0,      0,      -18,    -27,    -36,    -36,    -45,    -54,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			2,      2,      2,      2,      0,      0,      0,      0,
			0,      0,      0,      0,      2,      2,      2,      2,
			-2,     -2,     -2,     -2,     0,      0,      0,      0,
			0,      0,      0,      0,      -2,     -2,     -2,     -2,
			4,      4,      2,      2,      2,      0,      0,      0,
			0,      0,      0,      2,      2,      2,      4,      4,
			-4,     -4,     -2,     -2,     -2,     0,      0,      0,
			0,      0,      0,      -2,     -2,     -2,     -4,     -4,
			6,      6,      4,      4,      2,      2,      0,      0,
			0,      0,      2,      2,      4,      4,      6,      6,
			-6,     -6,     -4,     -4,     -2,     -2,     0,      0,
			0,      0,      -2,     -2,     -4,     -4,     -6,     -6,
			9,      6,      4,      4,      4,      2,      0,      0,
			0,      0,      2,      4,      4,      4,      6,      9,
			-9,     -6,     -4,     -4,     -4,     -2,     0,      0,
			0,      0,      -2,     -4,     -4,     -4,     -6,     -9,
			13,     11,     9,      9,      6,      4,      0,      0,
			0,      0,      4,      6,      9,      9,      11,     13,
			-13,    -11,    -9,     -9,     -6,     -4,     0,      0,
			0,      0,      -4,     -6,     -9,     -9,     -11,    -13,
			27,     22,     18,     18,     13,     9,      0,      0,
			0,      0,      9,      13,     18,     18,     22,     27,
			-27,    -22,    -18,    -18,    -13,    -9,     0,      0,
			0,      0,      -9,     -13,    -18,    -18,    -22,    -27,
			55,     46,     37,     37,     27,     18,     0,      0,
			0,      0,      18,     27,     37,     37,     46,     55,
			-55,    -46,    -37,    -37,    -27,    -18,    0,      0,
			0,      0,      -18,    -27,    -37,    -37,    -46,    -55,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			2,      2,      2,      2,      0,      0,      0,      0,
			0,      0,      0,      0,      2,      2,      2,      2,
			-2,     -2,     -2,     -2,     0,      0,      0,      0,
			0,      0,      0,      0,      -2,     -2,     -2,     -2,
			4,      4,      2,      2,      2,      0,      0,      0,
			0,      0,      0,      2,      2,      2,      4,      4,
			-4,     -4,     -2,     -2,     -2,     0,      0,      0,
			0,      0,      0,      -2,     -2,     -2,     -4,     -4,
			6,      6,      4,      4,      2,      2,      0,      0,
			0,      0,      2,      2,      4,      4,      6,      6,
			-6,     -6,     -4,     -4,     -2,     -2,     0,      0,
			0,      0,      -2,     -2,     -4,     -4,     -6,     -6,
			9,      6,      4,      4,      4,      2,      0,      0,
			0,      0,      2,      4,      4,      4,      6,      9,
			-9,     -6,     -4,     -4,     -4,     -2,     0,      0,
			0,      0,      -2,     -4,     -4,     -4,     -6,     -9,
			13,     11,     9,      9,      6,      4,      0,      0,
			0,      0,      4,      6,      9,      9,      11,     13,
			-13,    -11,    -9,     -9,     -6,     -4,     0,      0,
			0,      0,      -4,     -6,     -9,     -9,     -11,    -13,
			28,     23,     19,     19,     13,     9,      0,      0,
			0,      0,      9,      13,     19,     19,     23,     28,
			-28,    -23,    -19,    -19,    -13,    -9,     0,      0,
			0,      0,      -9,     -13,    -19,    -19,    -23,    -28,
			57,     47,     38,     38,     28,     19,     0,      0,
			0,      0,      19,     28,     38,     38,     47,     57,
			-57,    -47,    -38,    -38,    -28,    -19,    0,      0,
			0,      0,      -19,    -28,    -38,    -38,    -47,    -57,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			2,      2,      2,      2,      0,      0,      0,      0,
			0,      0,      0,      0,      2,      2,      2,      2,
			-2,     -2,     -2,     -2,     0,      0,      0,      0,
			0,      0,      0,      0,      -2,     -2,     -2,     -2,
			4,      4,      2,      2,      2,      0,      0,      0,
			0,      0,      0,      2,      2,      2,      4,      4,
			-4,     -4,     -2,     -2,     -2,     0,      0,      0,
			0,      0,      0,      -2,     -2,     -2,     -4,     -4,
			6,      6,      4,      4,      2,      2,      0,      0,
			0,      0,      2,      2,      4,      4,      6,      6,
			-6,     -6,     -4,     -4,     -2,     -2,     0,      0,
			0,      0,      -2,     -2,     -4,     -4,     -6,     -6,
			9,      6,      4,      4,      4,      2,      0,      0,
			0,      0,      2,      4,      4,      4,      6,      9,
			-9,     -6,     -4,     -4,     -4,     -2,     0,      0,
			0,      0,      -2,     -4,     -4,     -4,     -6,     -9,
			13,     11,     9,      9,      6,      4,      0,      0,
			0,      0,      4,      6,      9,      9,      11,     13,
			-13,    -11,    -9,     -9,     -6,     -4,     0,      0,
			0,      0,      -4,     -6,     -9,     -9,     -11,    -13,
			28,     23,     19,     19,     13,     9,      0,      0,
			0,      0,      9,      13,     19,     19,     23,     28,
			-28,    -23,    -19,    -19,    -13,    -9,     0,      0,
			0,      0,      -9,     -13,    -19,    -19,    -23,    -28,
			58,     48,     39,     39,     28,     19,     0,      0,
			0,      0,      19,     28,     39,     39,     48,     58,
			-58,    -48,    -39,    -39,    -28,    -19,    0,      0,
			0,      0,      -19,    -28,    -39,    -39,    -48,    -58,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			2,      2,      2,      2,      0,      0,      0,      0,
			0,      0,      0,      0,      2,      2,      2,      2,
			-2,     -2,     -2,     -2,     0,      0,      0,      0,
			0,      0,      0,      0,      -2,     -2,     -2,     -2,
			5,      5,      2,      2,      2,      0,      0,      0,
			0,      0,      0,      2,      2,      2,      5,      5,
			-5,     -5,     -2,     -2,     -2,     0,      0,      0,
			0,      0,      0,      -2,     -2,     -2,     -5,     -5,
			7,      7,      5,      5,      2,      2,      0,      0,
			0,      0,      2,      2,      5,      5,      7,      7,
			-7,     -7,     -5,     -5,     -2,     -2,     0,      0,
			0,      0,      -2,     -2,     -5,     -5,     -7,     -7,
			10,     7,      5,      5,      5,      2,      0,      0,
			0,      0,      2,      5,      5,      5,      7,      10,
			-10,    -7,     -5,     -5,     -5,     -2,     0,      0,
			0,      0,      -2,     -5,     -5,     -5,     -7,     -10,
			15,     12,     10,     10,     7,      5,      0,      0,
			0,      0,      5,      7,      10,     10,     12,     15,
			-15,    -12,    -10,    -10,    -7,     -5,     0,      0,
			0,      0,      -5,     -7,     -10,    -10,    -12,    -15,
			30,     25,     20,     20,     15,     10,     0,      0,
			0,      0,      10,     15,     20,     20,     25,     30,
			-30,    -25,    -20,    -20,    -15,    -10,    0,      0,
			0,      0,      -10,    -15,    -20,    -20,    -25,    -30,
			60,     50,     40,     40,     30,     20,     0,      0,
			0,      0,      20,     30,     40,     40,     50,     60,
			-60,    -50,    -40,    -40,    -30,    -20,    0,      0,
			0,      0,      -20,    -30,    -40,    -40,    -50,    -60,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			2,      2,      2,      2,      0,      0,      0,      0,
			0,      0,      0,      0,      2,      2,      2,      2,
			-2,     -2,     -2,     -2,     0,      0,      0,      0,
			0,      0,      0,      0,      -2,     -2,     -2,     -2,
			5,      5,      2,      2,      2,      0,      0,      0,
			0,      0,      0,      2,      2,      2,      5,      5,
			-5,     -5,     -2,     -2,     -2,     0,      0,      0,
			0,      0,      0,      -2,     -2,     -2,     -5,     -5,
			7,      7,      5,      5,      2,      2,      0,      0,
			0,      0,      2,      2,      5,      5,      7,      7,
			-7,     -7,     -5,     -5,     -2,     -2,     0,      0,
			0,      0,      -2,     -2,     -5,     -5,     -7,     -7,
			10,     7,      5,      5,      5,      2,      0,      0,
			0,      0,      2,      5,      5,      5,      7,      10,
			-10,    -7,     -5,     -5,     -5,     -2,     0,      0,
			0,      0,      -2,     -5,     -5,     -5,     -7,     -10,
			15,     12,     10,     10,     7,      5,      0,      0,
			0,      0,      5,      7,      10,     10,     12,     15,
			-15,    -12,    -10,    -10,    -7,     -5,     0,      0,
			0,      0,      -5,     -7,     -10,    -10,    -12,    -15,
			30,     25,     20,     20,     15,     10,     0,      0,
			0,      0,      10,     15,     20,     20,     25,     30,
			-30,    -25,    -20,    -20,    -15,    -10,    0,      0,
			0,      0,      -10,    -15,    -20,    -20,    -25,    -30,
			61,     51,     41,     41,     30,     20,     0,      0,
			0,      0,      20,     30,     41,     41,     51,     61,
			-61,    -51,    -41,    -41,    -30,    -20,    0,      0,
			0,      0,      -20,    -30,    -41,    -41,    -51,    -61,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			2,      2,      2,      2,      0,      0,      0,      0,
			0,      0,      0,      0,      2,      2,      2,      2,
			-2,     -2,     -2,     -2,     0,      0,      0,      0,
			0,      0,      0,      0,      -2,     -2,     -2,     -2,
			5,      5,      2,      2,      2,      0,      0,      0,
			0,      0,      0,      2,      2,      2,      5,      5,
			-5,     -5,     -2,     -2,     -2,     0,      0,      0,
			0,      0,      0,      -2,     -2,     -2,     -5,     -5,
			7,      7,      5,      5,      2,      2,      0,      0,
			0,      0,      2,      2,      5,      5,      7,      7,
			-7,     -7,     -5,     -5,     -2,     -2,     0,      0,
			0,      0,      -2,     -2,     -5,     -5,     -7,     -7,
			10,     7,      5,      5,      5,      2,      0,      0,
			0,      0,      2,      5,      5,      5,      7,      10,
			-10,    -7,     -5,     -5,     -5,     -2,     0,      0,
			0,      0,      -2,     -5,     -5,     -5,     -7,     -10,
			15,     12,     10,     10,     7,      5,      0,      0,
			0,      0,      5,      7,      10,     10,     12,     15,
			-15,    -12,    -10,    -10,    -7,     -5,     0,      0,
			0,      0,      -5,     -7,     -10,    -10,    -12,    -15,
			31,     26,     21,     21,     15,     10,     0,      0,
			0,      0,      10,     15,     21,     21,     26,     31,
			-31,    -26,    -21,    -21,    -15,    -10,    0,      0,
			0,      0,      -10,    -15,    -21,    -21,    -26,    -31,
			63,     52,     42,     42,     31,     21,     0,      0,
			0,      0,      21,     31,     42,     42,     52,     63,
			-63,    -52,    -42,    -42,    -31,    -21,    0,      0,
			0,      0,      -21,    -31,    -42,    -42,    -52,    -63,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			2,      2,      2,      2,      0,      0,      0,      0,
			0,      0,      0,      0,      2,      2,      2,      2,
			-2,     -2,     -2,     -2,     0,      0,      0,      0,
			0,      0,      0,      0,      -2,     -2,     -2,     -2,
			5,      5,      2,      2,      2,      0,      0,      0,
			0,      0,      0,      2,      2,      2,      5,      5,
			-5,     -5,     -2,     -2,     -2,     0,      0,      0,
			0,      0,      0,      -2,     -2,     -2,     -5,     -5,
			7,      7,      5,      5,      2,      2,      0,      0,
			0,      0,      2,      2,      5,      5,      7,      7,
			-7,     -7,     -5,     -5,     -2,     -2,     0,      0,
			0,      0,      -2,     -2,     -5,     -5,     -7,     -7,
			10,     7,      5,      5,      5,      2,      0,      0,
			0,      0,      2,      5,      5,      5,      7,      10,
			-10,    -7,     -5,     -5,     -5,     -2,     0,      0,
			0,      0,      -2,     -5,     -5,     -5,     -7,     -10,
			15,     12,     10,     10,     7,      5,      0,      0,
			0,      0,      5,      7,      10,     10,     12,     15,
			-15,    -12,    -10,    -10,    -7,     -5,     0,      0,
			0,      0,      -5,     -7,     -10,    -10,    -12,    -15,
			31,     26,     21,     21,     15,     10,     0,      0,
			0,      0,      10,     15,     21,     21,     26,     31,
			-31,    -26,    -21,    -21,    -15,    -10,    0,      0,
			0,      0,      -10,    -15,    -21,    -21,    -26,    -31,
			64,     53,     43,     43,     31,     21,     0,      0,
			0,      0,      21,     31,     43,     43,     53,     64,
			-64,    -53,    -43,    -43,    -31,    -21,    0,      0,
			0,      0,      -21,    -31,    -43,    -43,    -53,    -64,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			2,      2,      2,      2,      0,      0,      0,      0,
			0,      0,      0,      0,      2,      2,      2,      2,
			-2,     -2,     -2,     -2,     0,      0,      0,      0,
			0,      0,      0,      0,      -2,     -2,     -2,     -2,
			5,      5,      2,      2,      2,      0,      0,      0,
			0,      0,      0,      2,      2,      2,      5,      5,
			-5,     -5,     -2,     -2,     -2,     0,      0,      0,
			0,      0,      0,      -2,     -2,     -2,     -5,     -5,
			7,      7,      5,      5,      2,      2,      0,      0,
			0,      0,      2,      2,      5,      5,      7,      7,
			-7,     -7,     -5,     -5,     -2,     -2,     0,      0,
			0,      0,      -2,     -2,     -5,     -5,     -7,     -7,
			11,     7,      5,      5,      5,      2,      0,      0,
			0,      0,      2,      5,      5,      5,      7,      11,
			-11,    -7,     -5,     -5,     -5,     -2,     0,      0,
			0,      0,      -2,     -5,     -5,     -5,     -7,     -11,
			16,     13,     11,     11,     7,      5,      0,      0,
			0,      0,      5,      7,      11,     11,     13,     16,
			-16,    -13,    -11,    -11,    -7,     -5,     0,      0,
			0,      0,      -5,     -7,     -11,    -11,    -13,    -16,
			33,     27,     22,     22,     16,     11,     0,      0,
			0,      0,      11,     16,     22,     22,     27,     33,
			-33,    -27,    -22,    -22,    -16,    -11,    0,      0,
			0,      0,      -11,    -16,    -22,    -22,    -27,    -33,
			66,     55,     44,     44,     33,     22,     0,      0,
			0,      0,      22,     33,     44,     44,     55,     66,
			-66,    -55,    -44,    -44,    -33,    -22,    0,      0,
			0,      0,      -22,    -33,    -44,    -44,    -55,    -66,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			2,      2,      2,      2,      0,      0,      0,      0,
			0,      0,      0,      0,      2,      2,      2,      2,
			-2,     -2,     -2,     -2,     0,      0,      0,      0,
			0,      0,      0,      0,      -2,     -2,     -2,     -2,
			5,      5,      2,      2,      2,      0,      0,      0,
			0,      0,      0,      2,      2,      2,      5,      5,
			-5,     -5,     -2,     -2,     -2,     0,      0,      0,
			0,      0,      0,      -2,     -2,     -2,     -5,     -5,
			7,      7,      5,      5,      2,      2,      0,      0,
			0,      0,      2,      2,      5,      5,      7,      7,
			-7,     -7,     -5,     -5,     -2,     -2,     0,      0,
			0,      0,      -2,     -2,     -5,     -5,     -7,     -7,
			11,     7,      5,      5,      5,      2,      0,      0,
			0,      0,      2,      5,      5,      5,      7,      11,
			-11,    -7,     -5,     -5,     -5,     -2,     0,      0,
			0,      0,      -2,     -5,     -5,     -5,     -7,     -11,
			16,     13,     11,     11,     7,      5,      0,      0,
			0,      0,      5,      7,      11,     11,     13,     16,
			-16,    -13,    -11,    -11,    -7,     -5,     0,      0,
			0,      0,      -5,     -7,     -11,    -11,    -13,    -16,
			33,     27,     22,     22,     16,     11,     0,      0,
			0,      0,      11,     16,     22,     22,     27,     33,
			-33,    -27,    -22,    -22,    -16,    -11,    0,      0,
			0,      0,      -11,    -16,    -22,    -22,    -27,    -33,
			67,     56,     45,     45,     33,     22,     0,      0,
			0,      0,      22,     33,     45,     45,     56,     67,
			-67,    -56,    -45,    -45,    -33,    -22,    0,      0,
			0,      0,      -22,    -33,    -45,    -45,    -56,    -67,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			2,      2,      2,      2,      0,      0,      0,      0,
			0,      0,      0,      0,      2,      2,      2,      2,
			-2,     -2,     -2,     -2,     0,      0,      0,      0,
			0,      0,      0,      0,      -2,     -2,     -2,     -2,
			5,      5,      2,      2,      2,      0,      0,      0,
			0,      0,      0,      2,      2,      2,      5,      5,
			-5,     -5,     -2,     -2,     -2,     0,      0,      0,
			0,      0,      0,      -2,     -2,     -2,     -5,     -5,
			7,      7,      5,      5,      2,      2,      0,      0,
			0,      0,      2,      2,      5,      5,      7,      7,
			-7,     -7,     -5,     -5,     -2,     -2,     0,      0,
			0,      0,      -2,     -2,     -5,     -5,     -7,     -7,
			11,     7,      5,      5,      5,      2,      0,      0,
			0,      0,      2,      5,      5,      5,      7,      11,
			-11,    -7,     -5,     -5,     -5,     -2,     0,      0,
			0,      0,      -2,     -5,     -5,     -5,     -7,     -11,
			16,     13,     11,     11,     7,      5,      0,      0,
			0,      0,      5,      7,      11,     11,     13,     16,
			-16,    -13,    -11,    -11,    -7,     -5,     0,      0,
			0,      0,      -5,     -7,     -11,    -11,    -13,    -16,
			34,     28,     23,     23,     16,     11,     0,      0,
			0,      0,      11,     16,     23,     23,     28,     34,
			-34,    -28,    -23,    -23,    -16,    -11,    0,      0,
			0,      0,      -11,    -16,    -23,    -23,    -28,    -34,
			69,     57,     46,     46,     34,     23,     0,      0,
			0,      0,      23,     34,     46,     46,     57,     69,
			-69,    -57,    -46,    -46,    -34,    -23,    0,      0,
			0,      0,      -23,    -34,    -46,    -46,    -57,    -69,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			2,      2,      2,      2,      0,      0,      0,      0,
			0,      0,      0,      0,      2,      2,      2,      2,
			-2,     -2,     -2,     -2,     0,      0,      0,      0,
			0,      0,      0,      0,      -2,     -2,     -2,     -2,
			5,      5,      2,      2,      2,      0,      0,      0,
			0,      0,      0,      2,      2,      2,      5,      5,
			-5,     -5,     -2,     -2,     -2,     0,      0,      0,
			0,      0,      0,      -2,     -2,     -2,     -5,     -5,
			7,      7,      5,      5,      2,      2,      0,      0,
			0,      0,      2,      2,      5,      5,      7,      7,
			-7,     -7,     -5,     -5,     -2,     -2,     0,      0,
			0,      0,      -2,     -2,     -5,     -5,     -7,     -7,
			11,     7,      5,      5,      5,      2,      0,      0,
			0,      0,      2,      5,      5,      5,      7,      11,
			-11,    -7,     -5,     -5,     -5,     -2,     0,      0,
			0,      0,      -2,     -5,     -5,     -5,     -7,     -11,
			16,     13,     11,     11,     7,      5,      0,      0,
			0,      0,      5,      7,      11,     11,     13,     16,
			-16,    -13,    -11,    -11,    -7,     -5,     0,      0,
			0,      0,      -5,     -7,     -11,    -11,    -13,    -16,
			34,     28,     23,     23,     16,     11,     0,      0,
			0,      0,      11,     16,     23,     23,     28,     34,
			-34,    -28,    -23,    -23,    -16,    -11,    0,      0,
			0,      0,      -11,    -16,    -23,    -23,    -28,    -34,
			70,     58,     47,     47,     34,     23,     0,      0,
			0,      0,      23,     34,     47,     47,     58,     70,
			-70,    -58,    -47,    -47,    -34,    -23,    0,      0,
			0,      0,      -23,    -34,    -47,    -47,    -58,    -70,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			3,      3,      3,      3,      0,      0,      0,      0,
			0,      0,      0,      0,      3,      3,      3,      3,
			-3,     -3,     -3,     -3,     0,      0,      0,      0,
			0,      0,      0,      0,      -3,     -3,     -3,     -3,
			6,      6,      3,      3,      3,      0,      0,      0,
			0,      0,      0,      3,      3,      3,      6,      6,
			-6,     -6,     -3,     -3,     -3,     0,      0,      0,
			0,      0,      0,      -3,     -3,     -3,     -6,     -6,
			9,      9,      6,      6,      3,      3,      0,      0,
			0,      0,      3,      3,      6,      6,      9,      9,
			-9,     -9,     -6,     -6,     -3,     -3,     0,      0,
			0,      0,      -3,     -3,     -6,     -6,     -9,     -9,
			12,     9,      6,      6,      6,      3,      0,      0,
			0,      0,      3,      6,      6,      6,      9,      12,
			-12,    -9,     -6,     -6,     -6,     -3,     0,      0,
			0,      0,      -3,     -6,     -6,     -6,     -9,     -12,
			18,     15,     12,     12,     9,      6,      0,      0,
			0,      0,      6,      9,      12,     12,     15,     18,
			-18,    -15,    -12,    -12,    -9,     -6,     0,      0,
			0,      0,      -6,     -9,     -12,    -12,    -15,    -18,
			36,     30,     24,     24,     18,     12,     0,      0,
			0,      0,      12,     18,     24,     24,     30,     36,
			-36,    -30,    -24,    -24,    -18,    -12,    0,      0,
			0,      0,      -12,    -18,    -24,    -24,    -30,    -36,
			72,     60,     48,     48,     36,     24,     0,      0,
			0,      0,      24,     36,     48,     48,     60,     72,
			-72,    -60,    -48,    -48,    -36,    -24,    0,      0,
			0,      0,      -24,    -36,    -48,    -48,    -60,    -72,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			3,      3,      3,      3,      0,      0,      0,      0,
			0,      0,      0,      0,      3,      3,      3,      3,
			-3,     -3,     -3,     -3,     0,      0,      0,      0,
			0,      0,      0,      0,      -3,     -3,     -3,     -3,
			6,      6,      3,      3,      3,      0,      0,      0,
			0,      0,      0,      3,      3,      3,      6,      6,
			-6,     -6,     -3,     -3,     -3,     0,      0,      0,
			0,      0,      0,      -3,     -3,     -3,     -6,     -6,
			9,      9,      6,      6,      3,      3,      0,      0,
			0,      0,      3,      3,      6,      6,      9,      9,
			-9,     -9,     -6,     -6,     -3,     -3,     0,      0,
			0,      0,      -3,     -3,     -6,     -6,     -9,     -9,
			12,     9,      6,      6,      6,      3,      0,      0,
			0,      0,      3,      6,      6,      6,      9,      12,
			-12,    -9,     -6,     -6,     -6,     -3,     0,      0,
			0,      0,      -3,     -6,     -6,     -6,     -9,     -12,
			18,     15,     12,     12,     9,      6,      0,      0,
			0,      0,      6,      9,      12,     12,     15,     18,
			-18,    -15,    -12,    -12,    -9,     -6,     0,      0,
			0,      0,      -6,     -9,     -12,    -12,    -15,    -18,
			36,     30,     24,     24,     18,     12,     0,      0,
			0,      0,      12,     18,     24,     24,     30,     36,
			-36,    -30,    -24,    -24,    -18,    -12,    0,      0,
			0,      0,      -12,    -18,    -24,    -24,    -30,    -36,
			73,     61,     49,     49,     36,     24,     0,      0,
			0,      0,      24,     36,     49,     49,     61,     73,
			-73,    -61,    -49,    -49,    -36,    -24,    0,      0,
			0,      0,      -24,    -36,    -49,    -49,    -61,    -73,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			3,      3,      3,      3,      0,      0,      0,      0,
			0,      0,      0,      0,      3,      3,      3,      3,
			-3,     -3,     -3,     -3,     0,      0,      0,      0,
			0,      0,      0,      0,      -3,     -3,     -3,     -3,
			6,      6,      3,      3,      3,      0,      0,      0,
			0,      0,      0,      3,      3,      3,      6,      6,
			-6,     -6,     -3,     -3,     -3,     0,      0,      0,
			0,      0,      0,      -3,     -3,     -3,     -6,     -6,
			9,      9,      6,      6,      3,      3,      0,      0,
			0,      0,      3,      3,      6,      6,      9,      9,
			-9,     -9,     -6,     -6,     -3,     -3,     0,      0,
			0,      0,      -3,     -3,     -6,     -6,     -9,     -9,
			12,     9,      6,      6,      6,      3,      0,      0,
			0,      0,      3,      6,      6,      6,      9,      12,
			-12,    -9,     -6,     -6,     -6,     -3,     0,      0,
			0,      0,      -3,     -6,     -6,     -6,     -9,     -12,
			18,     15,     12,     12,     9,      6,      0,      0,
			0,      0,      6,      9,      12,     12,     15,     18,
			-18,    -15,    -12,    -12,    -9,     -6,     0,      0,
			0,      0,      -6,     -9,     -12,    -12,    -15,    -18,
			37,     31,     25,     25,     18,     12,     0,      0,
			0,      0,      12,     18,     25,     25,     31,     37,
			-37,    -31,    -25,    -25,    -18,    -12,    0,      0,
			0,      0,      -12,    -18,    -25,    -25,    -31,    -37,
			75,     62,     50,     50,     37,     25,     0,      0,
			0,      0,      25,     37,     50,     50,     62,     75,
			-75,    -62,    -50,    -50,    -37,    -25,    0,      0,
			0,      0,      -25,    -37,    -50,    -50,    -62,    -75,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			3,      3,      3,      3,      0,      0,      0,      0,
			0,      0,      0,      0,      3,      3,      3,      3,
			-3,     -3,     -3,     -3,     0,      0,      0,      0,
			0,      0,      0,      0,      -3,     -3,     -3,     -3,
			6,      6,      3,      3,      3,      0,      0,      0,
			0,      0,      0,      3,      3,      3,      6,      6,
			-6,     -6,     -3,     -3,     -3,     0,      0,      0,
			0,      0,      0,      -3,     -3,     -3,     -6,     -6,
			9,      9,      6,      6,      3,      3,      0,      0,
			0,      0,      3,      3,      6,      6,      9,      9,
			-9,     -9,     -6,     -6,     -3,     -3,     0,      0,
			0,      0,      -3,     -3,     -6,     -6,     -9,     -9,
			12,     9,      6,      6,      6,      3,      0,      0,
			0,      0,      3,      6,      6,      6,      9,      12,
			-12,    -9,     -6,     -6,     -6,     -3,     0,      0,
			0,      0,      -3,     -6,     -6,     -6,     -9,     -12,
			18,     15,     12,     12,     9,      6,      0,      0,
			0,      0,      6,      9,      12,     12,     15,     18,
			-18,    -15,    -12,    -12,    -9,     -6,     0,      0,
			0,      0,      -6,     -9,     -12,    -12,    -15,    -18,
			37,     31,     25,     25,     18,     12,     0,      0,
			0,      0,      12,     18,     25,     25,     31,     37,
			-37,    -31,    -25,    -25,    -18,    -12,    0,      0,
			0,      0,      -12,    -18,    -25,    -25,    -31,    -37,
			76,     63,     51,     51,     37,     25,     0,      0,
			0,      0,      25,     37,     51,     51,     63,     76,
			-76,    -63,    -51,    -51,    -37,    -25,    0,      0,
			0,      0,      -25,    -37,    -51,    -51,    -63,    -76,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			3,      3,      3,      3,      0,      0,      0,      0,
			0,      0,      0,      0,      3,      3,      3,      3,
			-3,     -3,     -3,     -3,     0,      0,      0,      0,
			0,      0,      0,      0,      -3,     -3,     -3,     -3,
			6,      6,      3,      3,      3,      0,      0,      0,
			0,      0,      0,      3,      3,      3,      6,      6,
			-6,     -6,     -3,     -3,     -3,     0,      0,      0,
			0,      0,      0,      -3,     -3,     -3,     -6,     -6,
			9,      9,      6,      6,      3,      3,      0,      0,
			0,      0,      3,      3,      6,      6,      9,      9,
			-9,     -9,     -6,     -6,     -3,     -3,     0,      0,
			0,      0,      -3,     -3,     -6,     -6,     -9,     -9,
			13,     9,      6,      6,      6,      3,      0,      0,
			0,      0,      3,      6,      6,      6,      9,      13,
			-13,    -9,     -6,     -6,     -6,     -3,     0,      0,
			0,      0,      -3,     -6,     -6,     -6,     -9,     -13,
			19,     16,     13,     13,     9,      6,      0,      0,
			0,      0,      6,      9,      13,     13,     16,     19,
			-19,    -16,    -13,    -13,    -9,     -6,     0,      0,
			0,      0,      -6,     -9,     -13,    -13,    -16,    -19,
			39,     32,     26,     26,     19,     13,     0,      0,
			0,      0,      13,     19,     26,     26,     32,     39,
			-39,    -32,    -26,    -26,    -19,    -13,    0,      0,
			0,      0,      -13,    -19,    -26,    -26,    -32,    -39,
			78,     65,     52,     52,     39,     26,     0,      0,
			0,      0,      26,     39,     52,     52,     65,     78,
			-78,    -65,    -52,    -52,    -39,    -26,    0,      0,
			0,      0,      -26,    -39,    -52,    -52,    -65,    -78,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			3,      3,      3,      3,      0,      0,      0,      0,
			0,      0,      0,      0,      3,      3,      3,      3,
			-3,     -3,     -3,     -3,     0,      0,      0,      0,
			0,      0,      0,      0,      -3,     -3,     -3,     -3,
			6,      6,      3,      3,      3,      0,      0,      0,
			0,      0,      0,      3,      3,      3,      6,      6,
			-6,     -6,     -3,     -3,     -3,     0,      0,      0,
			0,      0,      0,      -3,     -3,     -3,     -6,     -6,
			9,      9,      6,      6,      3,      3,      0,      0,
			0,      0,      3,      3,      6,      6,      9,      9,
			-9,     -9,     -6,     -6,     -3,     -3,     0,      0,
			0,      0,      -3,     -3,     -6,     -6,     -9,     -9,
			13,     9,      6,      6,      6,      3,      0,      0,
			0,      0,      3,      6,      6,      6,      9,      13,
			-13,    -9,     -6,     -6,     -6,     -3,     0,      0,
			0,      0,      -3,     -6,     -6,     -6,     -9,     -13,
			19,     16,     13,     13,     9,      6,      0,      0,
			0,      0,      6,      9,      13,     13,     16,     19,
			-19,    -16,    -13,    -13,    -9,     -6,     0,      0,
			0,      0,      -6,     -9,     -13,    -13,    -16,    -19,
			39,     32,     26,     26,     19,     13,     0,      0,
			0,      0,      13,     19,     26,     26,     32,     39,
			-39,    -32,    -26,    -26,    -19,    -13,    0,      0,
			0,      0,      -13,    -19,    -26,    -26,    -32,    -39,
			79,     66,     53,     53,     39,     26,     0,      0,
			0,      0,      26,     39,     53,     53,     66,     79,
			-79,    -66,    -53,    -53,    -39,    -26,    0,      0,
			0,      0,      -26,    -39,    -53,    -53,    -66,    -79,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			3,      3,      3,      3,      0,      0,      0,      0,
			0,      0,      0,      0,      3,      3,      3,      3,
			-3,     -3,     -3,     -3,     0,      0,      0,      0,
			0,      0,      0,      0,      -3,     -3,     -3,     -3,
			6,      6,      3,      3,      3,      0,      0,      0,
			0,      0,      0,      3,      3,      3,      6,      6,
			-6,     -6,     -3,     -3,     -3,     0,      0,      0,
			0,      0,      0,      -3,     -3,     -3,     -6,     -6,
			9,      9,      6,      6,      3,      3,      0,      0,
			0,      0,      3,      3,      6,      6,      9,      9,
			-9,     -9,     -6,     -6,     -3,     -3,     0,      0,
			0,      0,      -3,     -3,     -6,     -6,     -9,     -9,
			13,     9,      6,      6,      6,      3,      0,      0,
			0,      0,      3,      6,      6,      6,      9,      13,
			-13,    -9,     -6,     -6,     -6,     -3,     0,      0,
			0,      0,      -3,     -6,     -6,     -6,     -9,     -13,
			19,     16,     13,     13,     9,      6,      0,      0,
			0,      0,      6,      9,      13,     13,     16,     19,
			-19,    -16,    -13,    -13,    -9,     -6,     0,      0,
			0,      0,      -6,     -9,     -13,    -13,    -16,    -19,
			40,     33,     27,     27,     19,     13,     0,      0,
			0,      0,      13,     19,     27,     27,     33,     40,
			-40,    -33,    -27,    -27,    -19,    -13,    0,      0,
			0,      0,      -13,    -19,    -27,    -27,    -33,    -40,
			81,     67,     54,     54,     40,     27,     0,      0,
			0,      0,      27,     40,     54,     54,     67,     81,
			-81,    -67,    -54,    -54,    -40,    -27,    0,      0,
			0,      0,      -27,    -40,    -54,    -54,    -67,    -81,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			3,      3,      3,      3,      0,      0,      0,      0,
			0,      0,      0,      0,      3,      3,      3,      3,
			-3,     -3,     -3,     -3,     0,      0,      0,      0,
			0,      0,      0,      0,      -3,     -3,     -3,     -3,
			6,      6,      3,      3,      3,      0,      0,      0,
			0,      0,      0,      3,      3,      3,      6,      6,
			-6,     -6,     -3,     -3,     -3,     0,      0,      0,
			0,      0,      0,      -3,     -3,     -3,     -6,     -6,
			9,      9,      6,      6,      3,      3,      0,      0,
			0,      0,      3,      3,      6,      6,      9,      9,
			-9,     -9,     -6,     -6,     -3,     -3,     0,      0,
			0,      0,      -3,     -3,     -6,     -6,     -9,     -9,
			13,     9,      6,      6,      6,      3,      0,      0,
			0,      0,      3,      6,      6,      6,      9,      13,
			-13,    -9,     -6,     -6,     -6,     -3,     0,      0,
			0,      0,      -3,     -6,     -6,     -6,     -9,     -13,
			19,     16,     13,     13,     9,      6,      0,      0,
			0,      0,      6,      9,      13,     13,     16,     19,
			-19,    -16,    -13,    -13,    -9,     -6,     0,      0,
			0,      0,      -6,     -9,     -13,    -13,    -16,    -19,
			40,     33,     27,     27,     19,     13,     0,      0,
			0,      0,      13,     19,     27,     27,     33,     40,
			-40,    -33,    -27,    -27,    -19,    -13,    0,      0,
			0,      0,      -13,    -19,    -27,    -27,    -33,    -40,
			82,     68,     55,     55,     40,     27,     0,      0,
			0,      0,      27,     40,     55,     55,     68,     82,
			-82,    -68,    -55,    -55,    -40,    -27,    0,      0,
			0,      0,      -27,    -40,    -55,    -55,    -68,    -82,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			3,      3,      3,      3,      0,      0,      0,      0,
			0,      0,      0,      0,      3,      3,      3,      3,
			-3,     -3,     -3,     -3,     0,      0,      0,      0,
			0,      0,      0,      0,      -3,     -3,     -3,     -3,
			7,      7,      3,      3,      3,      0,      0,      0,
			0,      0,      0,      3,      3,      3,      7,      7,
			-7,     -7,     -3,     -3,     -3,     0,      0,      0,
			0,      0,      0,      -3,     -3,     -3,     -7,     -7,
			10,     10,     7,      7,      3,      3,      0,      0,
			0,      0,      3,      3,      7,      7,      10,     10,
			-10,    -10,    -7,     -7,     -3,     -3,     0,      0,
			0,      0,      -3,     -3,     -7,     -7,     -10,    -10,
			14,     10,     7,      7,      7,      3,      0,      0,
			0,      0,      3,      7,      7,      7,      10,     14,
			-14,    -10,    -7,     -7,     -7,     -3,     0,      0,
			0,      0,      -3,     -7,     -7,     -7,     -10,    -14,
			21,     17,     14,     14,     10,     7,      0,      0,
			0,      0,      7,      10,     14,     14,     17,     21,
			-21,    -17,    -14,    -14,    -10,    -7,     0,      0,
			0,      0,      -7,     -10,    -14,    -14,    -17,    -21,
			42,     35,     28,     28,     21,     14,     0,      0,
			0,      0,      14,     21,     28,     28,     35,     42,
			-42,    -35,    -28,    -28,    -21,    -14,    0,      0,
			0,      0,      -14,    -21,    -28,    -28,    -35,    -42,
			84,     70,     56,     56,     42,     28,     0,      0,
			0,      0,      28,     42,     56,     56,     70,     84,
			-84,    -70,    -56,    -56,    -42,    -28,    0,      0,
			0,      0,      -28,    -42,    -56,    -56,    -70,    -84,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			3,      3,      3,      3,      0,      0,      0,      0,
			0,      0,      0,      0,      3,      3,      3,      3,
			-3,     -3,     -3,     -3,     0,      0,      0,      0,
			0,      0,      0,      0,      -3,     -3,     -3,     -3,
			7,      7,      3,      3,      3,      0,      0,      0,
			0,      0,      0,      3,      3,      3,      7,      7,
			-7,     -7,     -3,     -3,     -3,     0,      0,      0,
			0,      0,      0,      -3,     -3,     -3,     -7,     -7,
			10,     10,     7,      7,      3,      3,      0,      0,
			0,      0,      3,      3,      7,      7,      10,     10,
			-10,    -10,    -7,     -7,     -3,     -3,     0,      0,
			0,      0,      -3,     -3,     -7,     -7,     -10,    -10,
			14,     10,     7,      7,      7,      3,      0,      0,
			0,      0,      3,      7,      7,      7,      10,     14,
			-14,    -10,    -7,     -7,     -7,     -3,     0,      0,
			0,      0,      -3,     -7,     -7,     -7,     -10,    -14,
			21,     17,     14,     14,     10,     7,      0,      0,
			0,      0,      7,      10,     14,     14,     17,     21,
			-21,    -17,    -14,    -14,    -10,    -7,     0,      0,
			0,      0,      -7,     -10,    -14,    -14,    -17,    -21,
			42,     35,     28,     28,     21,     14,     0,      0,
			0,      0,      14,     21,     28,     28,     35,     42,
			-42,    -35,    -28,    -28,    -21,    -14,    0,      0,
			0,      0,      -14,    -21,    -28,    -28,    -35,    -42,
			85,     71,     57,     57,     42,     28,     0,      0,
			0,      0,      28,     42,     57,     57,     71,     85,
			-85,    -71,    -57,    -57,    -42,    -28,    0,      0,
			0,      0,      -28,    -42,    -57,    -57,    -71,    -85,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			3,      3,      3,      3,      0,      0,      0,      0,
			0,      0,      0,      0,      3,      3,      3,      3,
			-3,     -3,     -3,     -3,     0,      0,      0,      0,
			0,      0,      0,      0,      -3,     -3,     -3,     -3,
			7,      7,      3,      3,      3,      0,      0,      0,
			0,      0,      0,      3,      3,      3,      7,      7,
			-7,     -7,     -3,     -3,     -3,     0,      0,      0,
			0,      0,      0,      -3,     -3,     -3,     -7,     -7,
			10,     10,     7,      7,      3,      3,      0,      0,
			0,      0,      3,      3,      7,      7,      10,     10,
			-10,    -10,    -7,     -7,     -3,     -3,     0,      0,
			0,      0,      -3,     -3,     -7,     -7,     -10,    -10,
			14,     10,     7,      7,      7,      3,      0,      0,
			0,      0,      3,      7,      7,      7,      10,     14,
			-14,    -10,    -7,     -7,     -7,     -3,     0,      0,
			0,      0,      -3,     -7,     -7,     -7,     -10,    -14,
			21,     17,     14,     14,     10,     7,      0,      0,
			0,      0,      7,      10,     14,     14,     17,     21,
			-21,    -17,    -14,    -14,    -10,    -7,     0,      0,
			0,      0,      -7,     -10,    -14,    -14,    -17,    -21,
			43,     36,     29,     29,     21,     14,     0,      0,
			0,      0,      14,     21,     29,     29,     36,     43,
			-43,    -36,    -29,    -29,    -21,    -14,    0,      0,
			0,      0,      -14,    -21,    -29,    -29,    -36,    -43,
			87,     72,     58,     58,     43,     29,     0,      0,
			0,      0,      29,     43,     58,     58,     72,     87,
			-87,    -72,    -58,    -58,    -43,    -29,    0,      0,
			0,      0,      -29,    -43,    -58,    -58,    -72,    -87,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			3,      3,      3,      3,      0,      0,      0,      0,
			0,      0,      0,      0,      3,      3,      3,      3,
			-3,     -3,     -3,     -3,     0,      0,      0,      0,
			0,      0,      0,      0,      -3,     -3,     -3,     -3,
			7,      7,      3,      3,      3,      0,      0,      0,
			0,      0,      0,      3,      3,      3,      7,      7,
			-7,     -7,     -3,     -3,     -3,     0,      0,      0,
			0,      0,      0,      -3,     -3,     -3,     -7,     -7,
			10,     10,     7,      7,      3,      3,      0,      0,
			0,      0,      3,      3,      7,      7,      10,     10,
			-10,    -10,    -7,     -7,     -3,     -3,     0,      0,
			0,      0,      -3,     -3,     -7,     -7,     -10,    -10,
			14,     10,     7,      7,      7,      3,      0,      0,
			0,      0,      3,      7,      7,      7,      10,     14,
			-14,    -10,    -7,     -7,     -7,     -3,     0,      0,
			0,      0,      -3,     -7,     -7,     -7,     -10,    -14,
			21,     17,     14,     14,     10,     7,      0,      0,
			0,      0,      7,      10,     14,     14,     17,     21,
			-21,    -17,    -14,    -14,    -10,    -7,     0,      0,
			0,      0,      -7,     -10,    -14,    -14,    -17,    -21,
			43,     36,     29,     29,     21,     14,     0,      0,
			0,      0,      14,     21,     29,     29,     36,     43,
			-43,    -36,    -29,    -29,    -21,    -14,    0,      0,
			0,      0,      -14,    -21,    -29,    -29,    -36,    -43,
			88,     73,     59,     59,     43,     29,     0,      0,
			0,      0,      29,     43,     59,     59,     73,     88,
			-88,    -73,    -59,    -59,    -43,    -29,    0,      0,
			0,      0,      -29,    -43,    -59,    -59,    -73,    -88,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			3,      3,      3,      3,      0,      0,      0,      0,
			0,      0,      0,      0,      3,      3,      3,      3,
			-3,     -3,     -3,     -3,     0,      0,      0,      0,
			0,      0,      0,      0,      -3,     -3,     -3,     -3,
			7,      7,      3,      3,      3,      0,      0,      0,
			0,      0,      0,      3,      3,      3,      7,      7,
			-7,     -7,     -3,     -3,     -3,     0,      0,      0,
			0,      0,      0,      -3,     -3,     -3,     -7,     -7,
			10,     10,     7,      7,      3,      3,      0,      0,
			0,      0,      3,      3,      7,      7,      10,     10,
			-10,    -10,    -7,     -7,     -3,     -3,     0,      0,
			0,      0,      -3,     -3,     -7,     -7,     -10,    -10,
			15,     10,     7,      7,      7,      3,      0,      0,
			0,      0,      3,      7,      7,      7,      10,     15,
			-15,    -10,    -7,     -7,     -7,     -3,     0,      0,
			0,      0,      -3,     -7,     -7,     -7,     -10,    -15,
			22,     18,     15,     15,     10,     7,      0,      0,
			0,      0,      7,      10,     15,     15,     18,     22,
			-22,    -18,    -15,    -15,    -10,    -7,     0,      0,
			0,      0,      -7,     -10,    -15,    -15,    -18,    -22,
			45,     37,     30,     30,     22,     15,     0,      0,
			0,      0,      15,     22,     30,     30,     37,     45,
			-45,    -37,    -30,    -30,    -22,    -15,    0,      0,
			0,      0,      -15,    -22,    -30,    -30,    -37,    -45,
			90,     75,     60,     60,     45,     30,     0,      0,
			0,      0,      30,     45,     60,     60,     75,     90,
			-90,    -75,    -60,    -60,    -45,    -30,    0,      0,
			0,      0,      -30,    -45,    -60,    -60,    -75,    -90,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			3,      3,      3,      3,      0,      0,      0,      0,
			0,      0,      0,      0,      3,      3,      3,      3,
			-3,     -3,     -3,     -3,     0,      0,      0,      0,
			0,      0,      0,      0,      -3,     -3,     -3,     -3,
			7,      7,      3,      3,      3,      0,      0,      0,
			0,      0,      0,      3,      3,      3,      7,      7,
			-7,     -7,     -3,     -3,     -3,     0,      0,      0,
			0,      0,      0,      -3,     -3,     -3,     -7,     -7,
			10,     10,     7,      7,      3,      3,      0,      0,
			0,      0,      3,      3,      7,      7,      10,     10,
			-10,    -10,    -7,     -7,     -3,     -3,     0,      0,
			0,      0,      -3,     -3,     -7,     -7,     -10,    -10,
			15,     10,     7,      7,      7,      3,      0,      0,
			0,      0,      3,      7,      7,      7,      10,     15,
			-15,    -10,    -7,     -7,     -7,     -3,     0,      0,
			0,      0,      -3,     -7,     -7,     -7,     -10,    -15,
			22,     18,     15,     15,     10,     7,      0,      0,
			0,      0,      7,      10,     15,     15,     18,     22,
			-22,    -18,    -15,    -15,    -10,    -7,     0,      0,
			0,      0,      -7,     -10,    -15,    -15,    -18,    -22,
			45,     37,     30,     30,     22,     15,     0,      0,
			0,      0,      15,     22,     30,     30,     37,     45,
			-45,    -37,    -30,    -30,    -22,    -15,    0,      0,
			0,      0,      -15,    -22,    -30,    -30,    -37,    -45,
			91,     76,     61,     61,     45,     30,     0,      0,
			0,      0,      30,     45,     61,     61,     76,     91,
			-91,    -76,    -61,    -61,    -45,    -30,    0,      0,
			0,      0,      -30,    -45,    -61,    -61,    -76,    -91,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			3,      3,      3,      3,      0,      0,      0,      0,
			0,      0,      0,      0,      3,      3,      3,      3,
			-3,     -3,     -3,     -3,     0,      0,      0,      0,
			0,      0,      0,      0,      -3,     -3,     -3,     -3,
			7,      7,      3,      3,      3,      0,      0,      0,
			0,      0,      0,      3,      3,      3,      7,      7,
			-7,     -7,     -3,     -3,     -3,     0,      0,      0,
			0,      0,      0,      -3,     -3,     -3,     -7,     -7,
			10,     10,     7,      7,      3,      3,      0,      0,
			0,      0,      3,      3,      7,      7,      10,     10,
			-10,    -10,    -7,     -7,     -3,     -3,     0,      0,
			0,      0,      -3,     -3,     -7,     -7,     -10,    -10,
			15,     10,     7,      7,      7,      3,      0,      0,
			0,      0,      3,      7,      7,      7,      10,     15,
			-15,    -10,    -7,     -7,     -7,     -3,     0,      0,
			0,      0,      -3,     -7,     -7,     -7,     -10,    -15,
			22,     18,     15,     15,     10,     7,      0,      0,
			0,      0,      7,      10,     15,     15,     18,     22,
			-22,    -18,    -15,    -15,    -10,    -7,     0,      0,
			0,      0,      -7,     -10,    -15,    -15,    -18,    -22,
			46,     38,     31,     31,     22,     15,     0,      0,
			0,      0,      15,     22,     31,     31,     38,     46,
			-46,    -38,    -31,    -31,    -22,    -15,    0,      0,
			0,      0,      -15,    -22,    -31,    -31,    -38,    -46,
			93,     77,     62,     62,     46,     31,     0,      0,
			0,      0,      31,     46,     62,     62,     77,     93,
			-93,    -77,    -62,    -62,    -46,    -31,    0,      0,
			0,      0,      -31,    -46,    -62,    -62,    -77,    -93,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			3,      3,      3,      3,      0,      0,      0,      0,
			0,      0,      0,      0,      3,      3,      3,      3,
			-3,     -3,     -3,     -3,     0,      0,      0,      0,
			0,      0,      0,      0,      -3,     -3,     -3,     -3,
			7,      7,      3,      3,      3,      0,      0,      0,
			0,      0,      0,      3,      3,      3,      7,      7,
			-7,     -7,     -3,     -3,     -3,     0,      0,      0,
			0,      0,      0,      -3,     -3,     -3,     -7,     -7,
			10,     10,     7,      7,      3,      3,      0,      0,
			0,      0,      3,      3,      7,      7,      10,     10,
			-10,    -10,    -7,     -7,     -3,     -3,     0,      0,
			0,      0,      -3,     -3,     -7,     -7,     -10,    -10,
			15,     10,     7,      7,      7,      3,      0,      0,
			0,      0,      3,      7,      7,      7,      10,     15,
			-15,    -10,    -7,     -7,     -7,     -3,     0,      0,
			0,      0,      -3,     -7,     -7,     -7,     -10,    -15,
			22,     18,     15,     15,     10,     7,      0,      0,
			0,      0,      7,      10,     15,     15,     18,     22,
			-22,    -18,    -15,    -15,    -10,    -7,     0,      0,
			0,      0,      -7,     -10,    -15,    -15,    -18,    -22,
			46,     38,     31,     31,     22,     15,     0,      0,
			0,      0,      15,     22,     31,     31,     38,     46,
			-46,    -38,    -31,    -31,    -22,    -15,    0,      0,
			0,      0,      -15,    -22,    -31,    -31,    -38,    -46,
			94,     78,     63,     63,     46,     31,     0,      0,
			0,      0,      31,     46,     63,     63,     78,     94,
			-94,    -78,    -63,    -63,    -46,    -31,    0,      0,
			0,      0,      -31,    -46,    -63,    -63,    -78,    -94,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			4,      4,      4,      4,      0,      0,      0,      0,
			0,      0,      0,      0,      4,      4,      4,      4,
			-4,     -4,     -4,     -4,     0,      0,      0,      0,
			0,      0,      0,      0,      -4,     -4,     -4,     -4,
			8,      8,      4,      4,      4,      0,      0,      0,
			0,      0,      0,      4,      4,      4,      8,      8,
			-8,     -8,     -4,     -4,     -4,     0,      0,      0,
			0,      0,      0,      -4,     -4,     -4,     -8,     -8,
			12,     12,     8,      8,      4,      4,      0,      0,
			0,      0,      4,      4,      8,      8,      12,     12,
			-12,    -12,    -8,     -8,     -4,     -4,     0,      0,
			0,      0,      -4,     -4,     -8,     -8,     -12,    -12,
			16,     12,     8,      8,      8,      4,      0,      0,
			0,      0,      4,      8,      8,      8,      12,     16,
			-16,    -12,    -8,     -8,     -8,     -4,     0,      0,
			0,      0,      -4,     -8,     -8,     -8,     -12,    -16,
			24,     20,     16,     16,     12,     8,      0,      0,
			0,      0,      8,      12,     16,     16,     20,     24,
			-24,    -20,    -16,    -16,    -12,    -8,     0,      0,
			0,      0,      -8,     -12,    -16,    -16,    -20,    -24,
			48,     40,     32,     32,     24,     16,     0,      0,
			0,      0,      16,     24,     32,     32,     40,     48,
			-48,    -40,    -32,    -32,    -24,    -16,    0,      0,
			0,      0,      -16,    -24,    -32,    -32,    -40,    -48,
			96,     80,     64,     64,     48,     32,     0,      0,
			0,      0,      32,     48,     64,     64,     80,     96,
			-96,    -80,    -64,    -64,    -48,    -32,    0,      0,
			0,      0,      -32,    -48,    -64,    -64,    -80,    -96,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			4,      4,      4,      4,      0,      0,      0,      0,
			0,      0,      0,      0,      4,      4,      4,      4,
			-4,     -4,     -4,     -4,     0,      0,      0,      0,
			0,      0,      0,      0,      -4,     -4,     -4,     -4,
			8,      8,      4,      4,      4,      0,      0,      0,
			0,      0,      0,      4,      4,      4,      8,      8,
			-8,     -8,     -4,     -4,     -4,     0,      0,      0,
			0,      0,      0,      -4,     -4,     -4,     -8,     -8,
			12,     12,     8,      8,      4,      4,      0,      0,
			0,      0,      4,      4,      8,      8,      12,     12,
			-12,    -12,    -8,     -8,     -4,     -4,     0,      0,
			0,      0,      -4,     -4,     -8,     -8,     -12,    -12,
			16,     12,     8,      8,      8,      4,      0,      0,
			0,      0,      4,      8,      8,      8,      12,     16,
			-16,    -12,    -8,     -8,     -8,     -4,     0,      0,
			0,      0,      -4,     -8,     -8,     -8,     -12,    -16,
			24,     20,     16,     16,     12,     8,      0,      0,
			0,      0,      8,      12,     16,     16,     20,     24,
			-24,    -20,    -16,    -16,    -12,    -8,     0,      0,
			0,      0,      -8,     -12,    -16,    -16,    -20,    -24,
			48,     40,     32,     32,     24,     16,     0,      0,
			0,      0,      16,     24,     32,     32,     40,     48,
			-48,    -40,    -32,    -32,    -24,    -16,    0,      0,
			0,      0,      -16,    -24,    -32,    -32,    -40,    -48,
			97,     81,     65,     65,     48,     32,     0,      0,
			0,      0,      32,     48,     65,     65,     81,     97,
			-97,    -81,    -65,    -65,    -48,    -32,    0,      0,
			0,      0,      -32,    -48,    -65,    -65,    -81,    -97,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			4,      4,      4,      4,      0,      0,      0,      0,
			0,      0,      0,      0,      4,      4,      4,      4,
			-4,     -4,     -4,     -4,     0,      0,      0,      0,
			0,      0,      0,      0,      -4,     -4,     -4,     -4,
			8,      8,      4,      4,      4,      0,      0,      0,
			0,      0,      0,      4,      4,      4,      8,      8,
			-8,     -8,     -4,     -4,     -4,     0,      0,      0,
			0,      0,      0,      -4,     -4,     -4,     -8,     -8,
			12,     12,     8,      8,      4,      4,      0,      0,
			0,      0,      4,      4,      8,      8,      12,     12,
			-12,    -12,    -8,     -8,     -4,     -4,     0,      0,
			0,      0,      -4,     -4,     -8,     -8,     -12,    -12,
			16,     12,     8,      8,      8,      4,      0,      0,
			0,      0,      4,      8,      8,      8,      12,     16,
			-16,    -12,    -8,     -8,     -8,     -4,     0,      0,
			0,      0,      -4,     -8,     -8,     -8,     -12,    -16,
			24,     20,     16,     16,     12,     8,      0,      0,
			0,      0,      8,      12,     16,     16,     20,     24,
			-24,    -20,    -16,    -16,    -12,    -8,     0,      0,
			0,      0,      -8,     -12,    -16,    -16,    -20,    -24,
			49,     41,     33,     33,     24,     16,     0,      0,
			0,      0,      16,     24,     33,     33,     41,     49,
			-49,    -41,    -33,    -33,    -24,    -16,    0,      0,
			0,      0,      -16,    -24,    -33,    -33,    -41,    -49,
			99,     82,     66,     66,     49,     33,     0,      0,
			0,      0,      33,     49,     66,     66,     82,     99,
			-99,    -82,    -66,    -66,    -49,    -33,    0,      0,
			0,      0,      -33,    -49,    -66,    -66,    -82,    -99,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			4,      4,      4,      4,      0,      0,      0,      0,
			0,      0,      0,      0,      4,      4,      4,      4,
			-4,     -4,     -4,     -4,     0,      0,      0,      0,
			0,      0,      0,      0,      -4,     -4,     -4,     -4,
			8,      8,      4,      4,      4,      0,      0,      0,
			0,      0,      0,      4,      4,      4,      8,      8,
			-8,     -8,     -4,     -4,     -4,     0,      0,      0,
			0,      0,      0,      -4,     -4,     -4,     -8,     -8,
			12,     12,     8,      8,      4,      4,      0,      0,
			0,      0,      4,      4,      8,      8,      12,     12,
			-12,    -12,    -8,     -8,     -4,     -4,     0,      0,
			0,      0,      -4,     -4,     -8,     -8,     -12,    -12,
			16,     12,     8,      8,      8,      4,      0,      0,
			0,      0,      4,      8,      8,      8,      12,     16,
			-16,    -12,    -8,     -8,     -8,     -4,     0,      0,
			0,      0,      -4,     -8,     -8,     -8,     -12,    -16,
			24,     20,     16,     16,     12,     8,      0,      0,
			0,      0,      8,      12,     16,     16,     20,     24,
			-24,    -20,    -16,    -16,    -12,    -8,     0,      0,
			0,      0,      -8,     -12,    -16,    -16,    -20,    -24,
			49,     41,     33,     33,     24,     16,     0,      0,
			0,      0,      16,     24,     33,     33,     41,     49,
			-49,    -41,    -33,    -33,    -24,    -16,    0,      0,
			0,      0,      -16,    -24,    -33,    -33,    -41,    -49,
			100,    83,     67,     67,     49,     33,     0,      0,
			0,      0,      33,     49,     67,     67,     83,     100,
			-100,   -83,    -67,    -67,    -49,    -33,    0,      0,
			0,      0,      -33,    -49,    -67,    -67,    -83,    -100,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			4,      4,      4,      4,      0,      0,      0,      0,
			0,      0,      0,      0,      4,      4,      4,      4,
			-4,     -4,     -4,     -4,     0,      0,      0,      0,
			0,      0,      0,      0,      -4,     -4,     -4,     -4,
			8,      8,      4,      4,      4,      0,      0,      0,
			0,      0,      0,      4,      4,      4,      8,      8,
			-8,     -8,     -4,     -4,     -4,     0,      0,      0,
			0,      0,      0,      -4,     -4,     -4,     -8,     -8,
			12,     12,     8,      8,      4,      4,      0,      0,
			0,      0,      4,      4,      8,      8,      12,     12,
			-12,    -12,    -8,     -8,     -4,     -4,     0,      0,
			0,      0,      -4,     -4,     -8,     -8,     -12,    -12,
			17,     12,     8,      8,      8,      4,      0,      0,
			0,      0,      4,      8,      8,      8,      12,     17,
			-17,    -12,    -8,     -8,     -8,     -4,     0,      0,
			0,      0,      -4,     -8,     -8,     -8,     -12,    -17,
			25,     21,     17,     17,     12,     8,      0,      0,
			0,      0,      8,      12,     17,     17,     21,     25,
			-25,    -21,    -17,    -17,    -12,    -8,     0,      0,
			0,      0,      -8,     -12,    -17,    -17,    -21,    -25,
			51,     42,     34,     34,     25,     17,     0,      0,
			0,      0,      17,     25,     34,     34,     42,     51,
			-51,    -42,    -34,    -34,    -25,    -17,    0,      0,
			0,      0,      -17,    -25,    -34,    -34,    -42,    -51,
			102,    85,     68,     68,     51,     34,     0,      0,
			0,      0,      34,     51,     68,     68,     85,     102,
			-102,   -85,    -68,    -68,    -51,    -34,    0,      0,
			0,      0,      -34,    -51,    -68,    -68,    -85,    -102,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			4,      4,      4,      4,      0,      0,      0,      0,
			0,      0,      0,      0,      4,      4,      4,      4,
			-4,     -4,     -4,     -4,     0,      0,      0,      0,
			0,      0,      0,      0,      -4,     -4,     -4,     -4,
			8,      8,      4,      4,      4,      0,      0,      0,
			0,      0,      0,      4,      4,      4,      8,      8,
			-8,     -8,     -4,     -4,     -4,     0,      0,      0,
			0,      0,      0,      -4,     -4,     -4,     -8,     -8,
			12,     12,     8,      8,      4,      4,      0,      0,
			0,      0,      4,      4,      8,      8,      12,     12,
			-12,    -12,    -8,     -8,     -4,     -4,     0,      0,
			0,      0,      -4,     -4,     -8,     -8,     -12,    -12,
			17,     12,     8,      8,      8,      4,      0,      0,
			0,      0,      4,      8,      8,      8,      12,     17,
			-17,    -12,    -8,     -8,     -8,     -4,     0,      0,
			0,      0,      -4,     -8,     -8,     -8,     -12,    -17,
			25,     21,     17,     17,     12,     8,      0,      0,
			0,      0,      8,      12,     17,     17,     21,     25,
			-25,    -21,    -17,    -17,    -12,    -8,     0,      0,
			0,      0,      -8,     -12,    -17,    -17,    -21,    -25,
			51,     42,     34,     34,     25,     17,     0,      0,
			0,      0,      17,     25,     34,     34,     42,     51,
			-51,    -42,    -34,    -34,    -25,    -17,    0,      0,
			0,      0,      -17,    -25,    -34,    -34,    -42,    -51,
			103,    86,     69,     69,     51,     34,     0,      0,
			0,      0,      34,     51,     69,     69,     86,     103,
			-103,   -86,    -69,    -69,    -51,    -34,    0,      0,
			0,      0,      -34,    -51,    -69,    -69,    -86,    -103,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			4,      4,      4,      4,      0,      0,      0,      0,
			0,      0,      0,      0,      4,      4,      4,      4,
			-4,     -4,     -4,     -4,     0,      0,      0,      0,
			0,      0,      0,      0,      -4,     -4,     -4,     -4,
			8,      8,      4,      4,      4,      0,      0,      0,
			0,      0,      0,      4,      4,      4,      8,      8,
			-8,     -8,     -4,     -4,     -4,     0,      0,      0,
			0,      0,      0,      -4,     -4,     -4,     -8,     -8,
			12,     12,     8,      8,      4,      4,      0,      0,
			0,      0,      4,      4,      8,      8,      12,     12,
			-12,    -12,    -8,     -8,     -4,     -4,     0,      0,
			0,      0,      -4,     -4,     -8,     -8,     -12,    -12,
			17,     12,     8,      8,      8,      4,      0,      0,
			0,      0,      4,      8,      8,      8,      12,     17,
			-17,    -12,    -8,     -8,     -8,     -4,     0,      0,
			0,      0,      -4,     -8,     -8,     -8,     -12,    -17,
			25,     21,     17,     17,     12,     8,      0,      0,
			0,      0,      8,      12,     17,     17,     21,     25,
			-25,    -21,    -17,    -17,    -12,    -8,     0,      0,
			0,      0,      -8,     -12,    -17,    -17,    -21,    -25,
			52,     43,     35,     35,     25,     17,     0,      0,
			0,      0,      17,     25,     35,     35,     43,     52,
			-52,    -43,    -35,    -35,    -25,    -17,    0,      0,
			0,      0,      -17,    -25,    -35,    -35,    -43,    -52,
			105,    87,     70,     70,     52,     35,     0,      0,
			0,      0,      35,     52,     70,     70,     87,     105,
			-105,   -87,    -70,    -70,    -52,    -35,    0,      0,
			0,      0,      -35,    -52,    -70,    -70,    -87,    -105,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			4,      4,      4,      4,      0,      0,      0,      0,
			0,      0,      0,      0,      4,      4,      4,      4,
			-4,     -4,     -4,     -4,     0,      0,      0,      0,
			0,      0,      0,      0,      -4,     -4,     -4,     -4,
			8,      8,      4,      4,      4,      0,      0,      0,
			0,      0,      0,      4,      4,      4,      8,      8,
			-8,     -8,     -4,     -4,     -4,     0,      0,      0,
			0,      0,      0,      -4,     -4,     -4,     -8,     -8,
			12,     12,     8,      8,      4,      4,      0,      0,
			0,      0,      4,      4,      8,      8,      12,     12,
			-12,    -12,    -8,     -8,     -4,     -4,     0,      0,
			0,      0,      -4,     -4,     -8,     -8,     -12,    -12,
			17,     12,     8,      8,      8,      4,      0,      0,
			0,      0,      4,      8,      8,      8,      12,     17,
			-17,    -12,    -8,     -8,     -8,     -4,     0,      0,
			0,      0,      -4,     -8,     -8,     -8,     -12,    -17,
			25,     21,     17,     17,     12,     8,      0,      0,
			0,      0,      8,      12,     17,     17,     21,     25,
			-25,    -21,    -17,    -17,    -12,    -8,     0,      0,
			0,      0,      -8,     -12,    -17,    -17,    -21,    -25,
			52,     43,     35,     35,     25,     17,     0,      0,
			0,      0,      17,     25,     35,     35,     43,     52,
			-52,    -43,    -35,    -35,    -25,    -17,    0,      0,
			0,      0,      -17,    -25,    -35,    -35,    -43,    -52,
			106,    88,     71,     71,     52,     35,     0,      0,
			0,      0,      35,     52,     71,     71,     88,     106,
			-106,   -88,    -71,    -71,    -52,    -35,    0,      0,
			0,      0,      -35,    -52,    -71,    -71,    -88,    -106,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			4,      4,      4,      4,      0,      0,      0,      0,
			0,      0,      0,      0,      4,      4,      4,      4,
			-4,     -4,     -4,     -4,     0,      0,      0,      0,
			0,      0,      0,      0,      -4,     -4,     -4,     -4,
			9,      9,      4,      4,      4,      0,      0,      0,
			0,      0,      0,      4,      4,      4,      9,      9,
			-9,     -9,     -4,     -4,     -4,     0,      0,      0,
			0,      0,      0,      -4,     -4,     -4,     -9,     -9,
			13,     13,     9,      9,      4,      4,      0,      0,
			0,      0,      4,      4,      9,      9,      13,     13,
			-13,    -13,    -9,     -9,     -4,     -4,     0,      0,
			0,      0,      -4,     -4,     -9,     -9,     -13,    -13,
			18,     13,     9,      9,      9,      4,      0,      0,
			0,      0,      4,      9,      9,      9,      13,     18,
			-18,    -13,    -9,     -9,     -9,     -4,     0,      0,
			0,      0,      -4,     -9,     -9,     -9,     -13,    -18,
			27,     22,     18,     18,     13,     9,      0,      0,
			0,      0,      9,      13,     18,     18,     22,     27,
			-27,    -22,    -18,    -18,    -13,    -9,     0,      0,
			0,      0,      -9,     -13,    -18,    -18,    -22,    -27,
			54,     45,     36,     36,     27,     18,     0,      0,
			0,      0,      18,     27,     36,     36,     45,     54,
			-54,    -45,    -36,    -36,    -27,    -18,    0,      0,
			0,      0,      -18,    -27,    -36,    -36,    -45,    -54,
			108,    90,     72,     72,     54,     36,     0,      0,
			0,      0,      36,     54,     72,     72,     90,     108,
			-108,   -90,    -72,    -72,    -54,    -36,    0,      0,
			0,      0,      -36,    -54,    -72,    -72,    -90,    -108,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			4,      4,      4,      4,      0,      0,      0,      0,
			0,      0,      0,      0,      4,      4,      4,      4,
			-4,     -4,     -4,     -4,     0,      0,      0,      0,
			0,      0,      0,      0,      -4,     -4,     -4,     -4,
			9,      9,      4,      4,      4,      0,      0,      0,
			0,      0,      0,      4,      4,      4,      9,      9,
			-9,     -9,     -4,     -4,     -4,     0,      0,      0,
			0,      0,      0,      -4,     -4,     -4,     -9,     -9,
			13,     13,     9,      9,      4,      4,      0,      0,
			0,      0,      4,      4,      9,      9,      13,     13,
			-13,    -13,    -9,     -9,     -4,     -4,     0,      0,
			0,      0,      -4,     -4,     -9,     -9,     -13,    -13,
			18,     13,     9,      9,      9,      4,      0,      0,
			0,      0,      4,      9,      9,      9,      13,     18,
			-18,    -13,    -9,     -9,     -9,     -4,     0,      0,
			0,      0,      -4,     -9,     -9,     -9,     -13,    -18,
			27,     22,     18,     18,     13,     9,      0,      0,
			0,      0,      9,      13,     18,     18,     22,     27,
			-27,    -22,    -18,    -18,    -13,    -9,     0,      0,
			0,      0,      -9,     -13,    -18,    -18,    -22,    -27,
			54,     45,     36,     36,     27,     18,     0,      0,
			0,      0,      18,     27,     36,     36,     45,     54,
			-54,    -45,    -36,    -36,    -27,    -18,    0,      0,
			0,      0,      -18,    -27,    -36,    -36,    -45,    -54,
			109,    91,     73,     73,     54,     36,     0,      0,
			0,      0,      36,     54,     73,     73,     91,     109,
			-109,   -91,    -73,    -73,    -54,    -36,    0,      0,
			0,      0,      -36,    -54,    -73,    -73,    -91,    -109,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			4,      4,      4,      4,      0,      0,      0,      0,
			0,      0,      0,      0,      4,      4,      4,      4,
			-4,     -4,     -4,     -4,     0,      0,      0,      0,
			0,      0,      0,      0,      -4,     -4,     -4,     -4,
			9,      9,      4,      4,      4,      0,      0,      0,
			0,      0,      0,      4,      4,      4,      9,      9,
			-9,     -9,     -4,     -4,     -4,     0,      0,      0,
			0,      0,      0,      -4,     -4,     -4,     -9,     -9,
			13,     13,     9,      9,      4,      4,      0,      0,
			0,      0,      4,      4,      9,      9,      13,     13,
			-13,    -13,    -9,     -9,     -4,     -4,     0,      0,
			0,      0,      -4,     -4,     -9,     -9,     -13,    -13,
			18,     13,     9,      9,      9,      4,      0,      0,
			0,      0,      4,      9,      9,      9,      13,     18,
			-18,    -13,    -9,     -9,     -9,     -4,     0,      0,
			0,      0,      -4,     -9,     -9,     -9,     -13,    -18,
			27,     22,     18,     18,     13,     9,      0,      0,
			0,      0,      9,      13,     18,     18,     22,     27,
			-27,    -22,    -18,    -18,    -13,    -9,     0,      0,
			0,      0,      -9,     -13,    -18,    -18,    -22,    -27,
			55,     46,     37,     37,     27,     18,     0,      0,
			0,      0,      18,     27,     37,     37,     46,     55,
			-55,    -46,    -37,    -37,    -27,    -18,    0,      0,
			0,      0,      -18,    -27,    -37,    -37,    -46,    -55,
			111,    92,     74,     74,     55,     37,     0,      0,
			0,      0,      37,     55,     74,     74,     92,     111,
			-111,   -92,    -74,    -74,    -55,    -37,    0,      0,
			0,      0,      -37,    -55,    -74,    -74,    -92,    -111,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			4,      4,      4,      4,      0,      0,      0,      0,
			0,      0,      0,      0,      4,      4,      4,      4,
			-4,     -4,     -4,     -4,     0,      0,      0,      0,
			0,      0,      0,      0,      -4,     -4,     -4,     -4,
			9,      9,      4,      4,      4,      0,      0,      0,
			0,      0,      0,      4,      4,      4,      9,      9,
			-9,     -9,     -4,     -4,     -4,     0,      0,      0,
			0,      0,      0,      -4,     -4,     -4,     -9,     -9,
			13,     13,     9,      9,      4,      4,      0,      0,
			0,      0,      4,      4,      9,      9,      13,     13,
			-13,    -13,    -9,     -9,     -4,     -4,     0,      0,
			0,      0,      -4,     -4,     -9,     -9,     -13,    -13,
			18,     13,     9,      9,      9,      4,      0,      0,
			0,      0,      4,      9,      9,      9,      13,     18,
			-18,    -13,    -9,     -9,     -9,     -4,     0,      0,
			0,      0,      -4,     -9,     -9,     -9,     -13,    -18,
			27,     22,     18,     18,     13,     9,      0,      0,
			0,      0,      9,      13,     18,     18,     22,     27,
			-27,    -22,    -18,    -18,    -13,    -9,     0,      0,
			0,      0,      -9,     -13,    -18,    -18,    -22,    -27,
			55,     46,     37,     37,     27,     18,     0,      0,
			0,      0,      18,     27,     37,     37,     46,     55,
			-55,    -46,    -37,    -37,    -27,    -18,    0,      0,
			0,      0,      -18,    -27,    -37,    -37,    -46,    -55,
			112,    93,     75,     75,     55,     37,     0,      0,
			0,      0,      37,     55,     75,     75,     93,     112,
			-112,   -93,    -75,    -75,    -55,    -37,    0,      0,
			0,      0,      -37,    -55,    -75,    -75,    -93,    -112,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			4,      4,      4,      4,      0,      0,      0,      0,
			0,      0,      0,      0,      4,      4,      4,      4,
			-4,     -4,     -4,     -4,     0,      0,      0,      0,
			0,      0,      0,      0,      -4,     -4,     -4,     -4,
			9,      9,      4,      4,      4,      0,      0,      0,
			0,      0,      0,      4,      4,      4,      9,      9,
			-9,     -9,     -4,     -4,     -4,     0,      0,      0,
			0,      0,      0,      -4,     -4,     -4,     -9,     -9,
			13,     13,     9,      9,      4,      4,      0,      0,
			0,      0,      4,      4,      9,      9,      13,     13,
			-13,    -13,    -9,     -9,     -4,     -4,     0,      0,
			0,      0,      -4,     -4,     -9,     -9,     -13,    -13,
			19,     13,     9,      9,      9,      4,      0,      0,
			0,      0,      4,      9,      9,      9,      13,     19,
			-19,    -13,    -9,     -9,     -9,     -4,     0,      0,
			0,      0,      -4,     -9,     -9,     -9,     -13,    -19,
			28,     23,     19,     19,     13,     9,      0,      0,
			0,      0,      9,      13,     19,     19,     23,     28,
			-28,    -23,    -19,    -19,    -13,    -9,     0,      0,
			0,      0,      -9,     -13,    -19,    -19,    -23,    -28,
			57,     47,     38,     38,     28,     19,     0,      0,
			0,      0,      19,     28,     38,     38,     47,     57,
			-57,    -47,    -38,    -38,    -28,    -19,    0,      0,
			0,      0,      -19,    -28,    -38,    -38,    -47,    -57,
			114,    95,     76,     76,     57,     38,     0,      0,
			0,      0,      38,     57,     76,     76,     95,     114,
			-114,   -95,    -76,    -76,    -57,    -38,    0,      0,
			0,      0,      -38,    -57,    -76,    -76,    -95,    -114,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			4,      4,      4,      4,      0,      0,      0,      0,
			0,      0,      0,      0,      4,      4,      4,      4,
			-4,     -4,     -4,     -4,     0,      0,      0,      0,
			0,      0,      0,      0,      -4,     -4,     -4,     -4,
			9,      9,      4,      4,      4,      0,      0,      0,
			0,      0,      0,      4,      4,      4,      9,      9,
			-9,     -9,     -4,     -4,     -4,     0,      0,      0,
			0,      0,      0,      -4,     -4,     -4,     -9,     -9,
			13,     13,     9,      9,      4,      4,      0,      0,
			0,      0,      4,      4,      9,      9,      13,     13,
			-13,    -13,    -9,     -9,     -4,     -4,     0,      0,
			0,      0,      -4,     -4,     -9,     -9,     -13,    -13,
			19,     13,     9,      9,      9,      4,      0,      0,
			0,      0,      4,      9,      9,      9,      13,     19,
			-19,    -13,    -9,     -9,     -9,     -4,     0,      0,
			0,      0,      -4,     -9,     -9,     -9,     -13,    -19,
			28,     23,     19,     19,     13,     9,      0,      0,
			0,      0,      9,      13,     19,     19,     23,     28,
			-28,    -23,    -19,    -19,    -13,    -9,     0,      0,
			0,      0,      -9,     -13,    -19,    -19,    -23,    -28,
			57,     47,     38,     38,     28,     19,     0,      0,
			0,      0,      19,     28,     38,     38,     47,     57,
			-57,    -47,    -38,    -38,    -28,    -19,    0,      0,
			0,      0,      -19,    -28,    -38,    -38,    -47,    -57,
			115,    96,     77,     77,     57,     38,     0,      0,
			0,      0,      38,     57,     77,     77,     96,     115,
			-115,   -96,    -77,    -77,    -57,    -38,    0,      0,
			0,      0,      -38,    -57,    -77,    -77,    -96,    -115,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			4,      4,      4,      4,      0,      0,      0,      0,
			0,      0,      0,      0,      4,      4,      4,      4,
			-4,     -4,     -4,     -4,     0,      0,      0,      0,
			0,      0,      0,      0,      -4,     -4,     -4,     -4,
			9,      9,      4,      4,      4,      0,      0,      0,
			0,      0,      0,      4,      4,      4,      9,      9,
			-9,     -9,     -4,     -4,     -4,     0,      0,      0,
			0,      0,      0,      -4,     -4,     -4,     -9,     -9,
			13,     13,     9,      9,      4,      4,      0,      0,
			0,      0,      4,      4,      9,      9,      13,     13,
			-13,    -13,    -9,     -9,     -4,     -4,     0,      0,
			0,      0,      -4,     -4,     -9,     -9,     -13,    -13,
			19,     13,     9,      9,      9,      4,      0,      0,
			0,      0,      4,      9,      9,      9,      13,     19,
			-19,    -13,    -9,     -9,     -9,     -4,     0,      0,
			0,      0,      -4,     -9,     -9,     -9,     -13,    -19,
			28,     23,     19,     19,     13,     9,      0,      0,
			0,      0,      9,      13,     19,     19,     23,     28,
			-28,    -23,    -19,    -19,    -13,    -9,     0,      0,
			0,      0,      -9,     -13,    -19,    -19,    -23,    -28,
			58,     48,     39,     39,     28,     19,     0,      0,
			0,      0,      19,     28,     39,     39,     48,     58,
			-58,    -48,    -39,    -39,    -28,    -19,    0,      0,
			0,      0,      -19,    -28,    -39,    -39,    -48,    -58,
			117,    97,     78,     78,     58,     39,     0,      0,
			0,      0,      39,     58,     78,     78,     97,     117,
			-117,   -97,    -78,    -78,    -58,    -39,    0,      0,
			0,      0,      -39,    -58,    -78,    -78,    -97,    -117,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			4,      4,      4,      4,      0,      0,      0,      0,
			0,      0,      0,      0,      4,      4,      4,      4,
			-4,     -4,     -4,     -4,     0,      0,      0,      0,
			0,      0,      0,      0,      -4,     -4,     -4,     -4,
			9,      9,      4,      4,      4,      0,      0,      0,
			0,      0,      0,      4,      4,      4,      9,      9,
			-9,     -9,     -4,     -4,     -4,     0,      0,      0,
			0,      0,      0,      -4,     -4,     -4,     -9,     -9,
			13,     13,     9,      9,      4,      4,      0,      0,
			0,      0,      4,      4,      9,      9,      13,     13,
			-13,    -13,    -9,     -9,     -4,     -4,     0,      0,
			0,      0,      -4,     -4,     -9,     -9,     -13,    -13,
			19,     13,     9,      9,      9,      4,      0,      0,
			0,      0,      4,      9,      9,      9,      13,     19,
			-19,    -13,    -9,     -9,     -9,     -4,     0,      0,
			0,      0,      -4,     -9,     -9,     -9,     -13,    -19,
			28,     23,     19,     19,     13,     9,      0,      0,
			0,      0,      9,      13,     19,     19,     23,     28,
			-28,    -23,    -19,    -19,    -13,    -9,     0,      0,
			0,      0,      -9,     -13,    -19,    -19,    -23,    -28,
			58,     48,     39,     39,     28,     19,     0,      0,
			0,      0,      19,     28,     39,     39,     48,     58,
			-58,    -48,    -39,    -39,    -28,    -19,    0,      0,
			0,      0,      -19,    -28,    -39,    -39,    -48,    -58,
			118,    98,     79,     79,     58,     39,     0,      0,
			0,      0,      39,     58,     79,     79,     98,     118,
			-118,   -98,    -79,    -79,    -58,    -39,    0,      0,
			0,      0,      -39,    -58,    -79,    -79,    -98,    -118,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			5,      5,      5,      5,      0,      0,      0,      0,
			0,      0,      0,      0,      5,      5,      5,      5,
			-5,     -5,     -5,     -5,     0,      0,      0,      0,
			0,      0,      0,      0,      -5,     -5,     -5,     -5,
			10,     10,     5,      5,      5,      0,      0,      0,
			0,      0,      0,      5,      5,      5,      10,     10,
			-10,    -10,    -5,     -5,     -5,     0,      0,      0,
			0,      0,      0,      -5,     -5,     -5,     -10,    -10,
			15,     15,     10,     10,     5,      5,      0,      0,
			0,      0,      5,      5,      10,     10,     15,     15,
			-15,    -15,    -10,    -10,    -5,     -5,     0,      0,
			0,      0,      -5,     -5,     -10,    -10,    -15,    -15,
			20,     15,     10,     10,     10,     5,      0,      0,
			0,      0,      5,      10,     10,     10,     15,     20,
			-20,    -15,    -10,    -10,    -10,    -5,     0,      0,
			0,      0,      -5,     -10,    -10,    -10,    -15,    -20,
			30,     25,     20,     20,     15,     10,     0,      0,
			0,      0,      10,     15,     20,     20,     25,     30,
			-30,    -25,    -20,    -20,    -15,    -10,    0,      0,
			0,      0,      -10,    -15,    -20,    -20,    -25,    -30,
			60,     50,     40,     40,     30,     20,     0,      0,
			0,      0,      20,     30,     40,     40,     50,     60,
			-60,    -50,    -40,    -40,    -30,    -20,    0,      0,
			0,      0,      -20,    -30,    -40,    -40,    -50,    -60,
			120,    100,    80,     80,     60,     40,     0,      0,
			0,      0,      40,     60,     80,     80,     100,    120,
			-120,   -100,   -80,    -80,    -60,    -40,    0,      0,
			0,      0,      -40,    -60,    -80,    -80,    -100,   -120,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			5,      5,      5,      5,      0,      0,      0,      0,
			0,      0,      0,      0,      5,      5,      5,      5,
			-5,     -5,     -5,     -5,     0,      0,      0,      0,
			0,      0,      0,      0,      -5,     -5,     -5,     -5,
			10,     10,     5,      5,      5,      0,      0,      0,
			0,      0,      0,      5,      5,      5,      10,     10,
			-10,    -10,    -5,     -5,     -5,     0,      0,      0,
			0,      0,      0,      -5,     -5,     -5,     -10,    -10,
			15,     15,     10,     10,     5,      5,      0,      0,
			0,      0,      5,      5,      10,     10,     15,     15,
			-15,    -15,    -10,    -10,    -5,     -5,     0,      0,
			0,      0,      -5,     -5,     -10,    -10,    -15,    -15,
			20,     15,     10,     10,     10,     5,      0,      0,
			0,      0,      5,      10,     10,     10,     15,     20,
			-20,    -15,    -10,    -10,    -10,    -5,     0,      0,
			0,      0,      -5,     -10,    -10,    -10,    -15,    -20,
			30,     25,     20,     20,     15,     10,     0,      0,
			0,      0,      10,     15,     20,     20,     25,     30,
			-30,    -25,    -20,    -20,    -15,    -10,    0,      0,
			0,      0,      -10,    -15,    -20,    -20,    -25,    -30,
			60,     50,     40,     40,     30,     20,     0,      0,
			0,      0,      20,     30,     40,     40,     50,     60,
			-60,    -50,    -40,    -40,    -30,    -20,    0,      0,
			0,      0,      -20,    -30,    -40,    -40,    -50,    -60,
			121,    101,    81,     81,     60,     40,     0,      0,
			0,      0,      40,     60,     81,     81,     101,    121,
			-121,   -101,   -81,    -81,    -60,    -40,    0,      0,
			0,      0,      -40,    -60,    -81,    -81,    -101,   -121,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			5,      5,      5,      5,      0,      0,      0,      0,
			0,      0,      0,      0,      5,      5,      5,      5,
			-5,     -5,     -5,     -5,     0,      0,      0,      0,
			0,      0,      0,      0,      -5,     -5,     -5,     -5,
			10,     10,     5,      5,      5,      0,      0,      0,
			0,      0,      0,      5,      5,      5,      10,     10,
			-10,    -10,    -5,     -5,     -5,     0,      0,      0,
			0,      0,      0,      -5,     -5,     -5,     -10,    -10,
			15,     15,     10,     10,     5,      5,      0,      0,
			0,      0,      5,      5,      10,     10,     15,     15,
			-15,    -15,    -10,    -10,    -5,     -5,     0,      0,
			0,      0,      -5,     -5,     -10,    -10,    -15,    -15,
			20,     15,     10,     10,     10,     5,      0,      0,
			0,      0,      5,      10,     10,     10,     15,     20,
			-20,    -15,    -10,    -10,    -10,    -5,     0,      0,
			0,      0,      -5,     -10,    -10,    -10,    -15,    -20,
			30,     25,     20,     20,     15,     10,     0,      0,
			0,      0,      10,     15,     20,     20,     25,     30,
			-30,    -25,    -20,    -20,    -15,    -10,    0,      0,
			0,      0,      -10,    -15,    -20,    -20,    -25,    -30,
			61,     51,     41,     41,     30,     20,     0,      0,
			0,      0,      20,     30,     41,     41,     51,     61,
			-61,    -51,    -41,    -41,    -30,    -20,    0,      0,
			0,      0,      -20,    -30,    -41,    -41,    -51,    -61,
			123,    102,    82,     82,     61,     41,     0,      0,
			0,      0,      41,     61,     82,     82,     102,    123,
			-123,   -102,   -82,    -82,    -61,    -41,    0,      0,
			0,      0,      -41,    -61,    -82,    -82,    -102,   -123,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			5,      5,      5,      5,      0,      0,      0,      0,
			0,      0,      0,      0,      5,      5,      5,      5,
			-5,     -5,     -5,     -5,     0,      0,      0,      0,
			0,      0,      0,      0,      -5,     -5,     -5,     -5,
			10,     10,     5,      5,      5,      0,      0,      0,
			0,      0,      0,      5,      5,      5,      10,     10,
			-10,    -10,    -5,     -5,     -5,     0,      0,      0,
			0,      0,      0,      -5,     -5,     -5,     -10,    -10,
			15,     15,     10,     10,     5,      5,      0,      0,
			0,      0,      5,      5,      10,     10,     15,     15,
			-15,    -15,    -10,    -10,    -5,     -5,     0,      0,
			0,      0,      -5,     -5,     -10,    -10,    -15,    -15,
			20,     15,     10,     10,     10,     5,      0,      0,
			0,      0,      5,      10,     10,     10,     15,     20,
			-20,    -15,    -10,    -10,    -10,    -5,     0,      0,
			0,      0,      -5,     -10,    -10,    -10,    -15,    -20,
			30,     25,     20,     20,     15,     10,     0,      0,
			0,      0,      10,     15,     20,     20,     25,     30,
			-30,    -25,    -20,    -20,    -15,    -10,    0,      0,
			0,      0,      -10,    -15,    -20,    -20,    -25,    -30,
			61,     51,     41,     41,     30,     20,     0,      0,
			0,      0,      20,     30,     41,     41,     51,     61,
			-61,    -51,    -41,    -41,    -30,    -20,    0,      0,
			0,      0,      -20,    -30,    -41,    -41,    -51,    -61,
			124,    103,    83,     83,     61,     41,     0,      0,
			0,      0,      41,     61,     83,     83,     103,    124,
			-124,   -103,   -83,    -83,    -61,    -41,    0,      0,
			0,      0,      -41,    -61,    -83,    -83,    -103,   -124,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			5,      5,      5,      5,      0,      0,      0,      0,
			0,      0,      0,      0,      5,      5,      5,      5,
			-5,     -5,     -5,     -5,     0,      0,      0,      0,
			0,      0,      0,      0,      -5,     -5,     -5,     -5,
			10,     10,     5,      5,      5,      0,      0,      0,
			0,      0,      0,      5,      5,      5,      10,     10,
			-10,    -10,    -5,     -5,     -5,     0,      0,      0,
			0,      0,      0,      -5,     -5,     -5,     -10,    -10,
			15,     15,     10,     10,     5,      5,      0,      0,
			0,      0,      5,      5,      10,     10,     15,     15,
			-15,    -15,    -10,    -10,    -5,     -5,     0,      0,
			0,      0,      -5,     -5,     -10,    -10,    -15,    -15,
			21,     15,     10,     10,     10,     5,      0,      0,
			0,      0,      5,      10,     10,     10,     15,     21,
			-21,    -15,    -10,    -10,    -10,    -5,     0,      0,
			0,      0,      -5,     -10,    -10,    -10,    -15,    -21,
			31,     26,     21,     21,     15,     10,     0,      0,
			0,      0,      10,     15,     21,     21,     26,     31,
			-31,    -26,    -21,    -21,    -15,    -10,    0,      0,
			0,      0,      -10,    -15,    -21,    -21,    -26,    -31,
			63,     52,     42,     42,     31,     21,     0,      0,
			0,      0,      21,     31,     42,     42,     52,     63,
			-63,    -52,    -42,    -42,    -31,    -21,    0,      0,
			0,      0,      -21,    -31,    -42,    -42,    -52,    -63,
			126,    105,    84,     84,     63,     42,     0,      0,
			0,      0,      42,     63,     84,     84,     105,    126,
			-126,   -105,   -84,    -84,    -63,    -42,    0,      0,
			0,      0,      -42,    -63,    -84,    -84,    -105,   -126,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			5,      5,      5,      5,      0,      0,      0,      0,
			0,      0,      0,      0,      5,      5,      5,      5,
			-5,     -5,     -5,     -5,     0,      0,      0,      0,
			0,      0,      0,      0,      -5,     -5,     -5,     -5,
			10,     10,     5,      5,      5,      0,      0,      0,
			0,      0,      0,      5,      5,      5,      10,     10,
			-10,    -10,    -5,     -5,     -5,     0,      0,      0,
			0,      0,      0,      -5,     -5,     -5,     -10,    -10,
			15,     15,     10,     10,     5,      5,      0,      0,
			0,      0,      5,      5,      10,     10,     15,     15,
			-15,    -15,    -10,    -10,    -5,     -5,     0,      0,
			0,      0,      -5,     -5,     -10,    -10,    -15,    -15,
			21,     15,     10,     10,     10,     5,      0,      0,
			0,      0,      5,      10,     10,     10,     15,     21,
			-21,    -15,    -10,    -10,    -10,    -5,     0,      0,
			0,      0,      -5,     -10,    -10,    -10,    -15,    -21,
			31,     26,     21,     21,     15,     10,     0,      0,
			0,      0,      10,     15,     21,     21,     26,     31,
			-31,    -26,    -21,    -21,    -15,    -10,    0,      0,
			0,      0,      -10,    -15,    -21,    -21,    -26,    -31,
			63,     52,     42,     42,     31,     21,     0,      0,
			0,      0,      21,     31,     42,     42,     52,     63,
			-63,    -52,    -42,    -42,    -31,    -21,    0,      0,
			0,      0,      -21,    -31,    -42,    -42,    -52,    -63,
			127,    106,    85,     85,     63,     42,     0,      0,
			0,      0,      42,     63,     85,     85,     106,    127,
			-127,   -106,   -85,    -85,    -63,    -42,    0,      0,
			0,      0,      -42,    -63,    -85,    -85,    -106,   -127,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			5,      5,      5,      5,      0,      0,      0,      0,
			0,      0,      0,      0,      5,      5,      5,      5,
			-5,     -5,     -5,     -5,     0,      0,      0,      0,
			0,      0,      0,      0,      -5,     -5,     -5,     -5,
			10,     10,     5,      5,      5,      0,      0,      0,
			0,      0,      0,      5,      5,      5,      10,     10,
			-10,    -10,    -5,     -5,     -5,     0,      0,      0,
			0,      0,      0,      -5,     -5,     -5,     -10,    -10,
			15,     15,     10,     10,     5,      5,      0,      0,
			0,      0,      5,      5,      10,     10,     15,     15,
			-15,    -15,    -10,    -10,    -5,     -5,     0,      0,
			0,      0,      -5,     -5,     -10,    -10,    -15,    -15,
			21,     15,     10,     10,     10,     5,      0,      0,
			0,      0,      5,      10,     10,     10,     15,     21,
			-21,    -15,    -10,    -10,    -10,    -5,     0,      0,
			0,      0,      -5,     -10,    -10,    -10,    -15,    -21,
			31,     26,     21,     21,     15,     10,     0,      0,
			0,      0,      10,     15,     21,     21,     26,     31,
			-31,    -26,    -21,    -21,    -15,    -10,    0,      0,
			0,      0,      -10,    -15,    -21,    -21,    -26,    -31,
			64,     53,     43,     43,     31,     21,     0,      0,
			0,      0,      21,     31,     43,     43,     53,     64,
			-64,    -53,    -43,    -43,    -31,    -21,    0,      0,
			0,      0,      -21,    -31,    -43,    -43,    -53,    -64,
			129,    107,    86,     86,     64,     43,     0,      0,
			0,      0,      43,     64,     86,     86,     107,    129,
			-129,   -107,   -86,    -86,    -64,    -43,    0,      0,
			0,      0,      -43,    -64,    -86,    -86,    -107,   -129,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			5,      5,      5,      5,      0,      0,      0,      0,
			0,      0,      0,      0,      5,      5,      5,      5,
			-5,     -5,     -5,     -5,     0,      0,      0,      0,
			0,      0,      0,      0,      -5,     -5,     -5,     -5,
			10,     10,     5,      5,      5,      0,      0,      0,
			0,      0,      0,      5,      5,      5,      10,     10,
			-10,    -10,    -5,     -5,     -5,     0,      0,      0,
			0,      0,      0,      -5,     -5,     -5,     -10,    -10,
			15,     15,     10,     10,     5,      5,      0,      0,
			0,      0,      5,      5,      10,     10,     15,     15,
			-15,    -15,    -10,    -10,    -5,     -5,     0,      0,
			0,      0,      -5,     -5,     -10,    -10,    -15,    -15,
			21,     15,     10,     10,     10,     5,      0,      0,
			0,      0,      5,      10,     10,     10,     15,     21,
			-21,    -15,    -10,    -10,    -10,    -5,     0,      0,
			0,      0,      -5,     -10,    -10,    -10,    -15,    -21,
			31,     26,     21,     21,     15,     10,     0,      0,
			0,      0,      10,     15,     21,     21,     26,     31,
			-31,    -26,    -21,    -21,    -15,    -10,    0,      0,
			0,      0,      -10,    -15,    -21,    -21,    -26,    -31,
			64,     53,     43,     43,     31,     21,     0,      0,
			0,      0,      21,     31,     43,     43,     53,     64,
			-64,    -53,    -43,    -43,    -31,    -21,    0,      0,
			0,      0,      -21,    -31,    -43,    -43,    -53,    -64,
			130,    108,    87,     87,     64,     43,     0,      0,
			0,      0,      43,     64,     87,     87,     108,    130,
			-130,   -108,   -87,    -87,    -64,    -43,    0,      0,
			0,      0,      -43,    -64,    -87,    -87,    -108,   -130,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			5,      5,      5,      5,      0,      0,      0,      0,
			0,      0,      0,      0,      5,      5,      5,      5,
			-5,     -5,     -5,     -5,     0,      0,      0,      0,
			0,      0,      0,      0,      -5,     -5,     -5,     -5,
			11,     11,     5,      5,      5,      0,      0,      0,
			0,      0,      0,      5,      5,      5,      11,     11,
			-11,    -11,    -5,     -5,     -5,     0,      0,      0,
			0,      0,      0,      -5,     -5,     -5,     -11,    -11,
			16,     16,     11,     11,     5,      5,      0,      0,
			0,      0,      5,      5,      11,     11,     16,     16,
			-16,    -16,    -11,    -11,    -5,     -5,     0,      0,
			0,      0,      -5,     -5,     -11,    -11,    -16,    -16,
			22,     16,     11,     11,     11,     5,      0,      0,
			0,      0,      5,      11,     11,     11,     16,     22,
			-22,    -16,    -11,    -11,    -11,    -5,     0,      0,
			0,      0,      -5,     -11,    -11,    -11,    -16,    -22,
			33,     27,     22,     22,     16,     11,     0,      0,
			0,      0,      11,     16,     22,     22,     27,     33,
			-33,    -27,    -22,    -22,    -16,    -11,    0,      0,
			0,      0,      -11,    -16,    -22,    -22,    -27,    -33,
			66,     55,     44,     44,     33,     22,     0,      0,
			0,      0,      22,     33,     44,     44,     55,     66,
			-66,    -55,    -44,    -44,    -33,    -22,    0,      0,
			0,      0,      -22,    -33,    -44,    -44,    -55,    -66,
			132,    110,    88,     88,     66,     44,     0,      0,
			0,      0,      44,     66,     88,     88,     110,    132,
			-132,   -110,   -88,    -88,    -66,    -44,    0,      0,
			0,      0,      -44,    -66,    -88,    -88,    -110,   -132,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			5,      5,      5,      5,      0,      0,      0,      0,
			0,      0,      0,      0,      5,      5,      5,      5,
			-5,     -5,     -5,     -5,     0,      0,      0,      0,
			0,      0,      0,      0,      -5,     -5,     -5,     -5,
			11,     11,     5,      5,      5,      0,      0,      0,
			0,      0,      0,      5,      5,      5,      11,     11,
			-11,    -11,    -5,     -5,     -5,     0,      0,      0,
			0,      0,      0,      -5,     -5,     -5,     -11,    -11,
			16,     16,     11,     11,     5,      5,      0,      0,
			0,      0,      5,      5,      11,     11,     16,     16,
			-16,    -16,    -11,    -11,    -5,     -5,     0,      0,
			0,      0,      -5,     -5,     -11,    -11,    -16,    -16,
			22,     16,     11,     11,     11,     5,      0,      0,
			0,      0,      5,      11,     11,     11,     16,     22,
			-22,    -16,    -11,    -11,    -11,    -5,     0,      0,
			0,      0,      -5,     -11,    -11,    -11,    -16,    -22,
			33,     27,     22,     22,     16,     11,     0,      0,
			0,      0,      11,     16,     22,     22,     27,     33,
			-33,    -27,    -22,    -22,    -16,    -11,    0,      0,
			0,      0,      -11,    -16,    -22,    -22,    -27,    -33,
			66,     55,     44,     44,     33,     22,     0,      0,
			0,      0,      22,     33,     44,     44,     55,     66,
			-66,    -55,    -44,    -44,    -33,    -22,    0,      0,
			0,      0,      -22,    -33,    -44,    -44,    -55,    -66,
			133,    111,    89,     89,     66,     44,     0,      0,
			0,      0,      44,     66,     89,     89,     111,    133,
			-133,   -111,   -89,    -89,    -66,    -44,    0,      0,
			0,      0,      -44,    -66,    -89,    -89,    -111,   -133,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			5,      5,      5,      5,      0,      0,      0,      0,
			0,      0,      0,      0,      5,      5,      5,      5,
			-5,     -5,     -5,     -5,     0,      0,      0,      0,
			0,      0,      0,      0,      -5,     -5,     -5,     -5,
			11,     11,     5,      5,      5,      0,      0,      0,
			0,      0,      0,      5,      5,      5,      11,     11,
			-11,    -11,    -5,     -5,     -5,     0,      0,      0,
			0,      0,      0,      -5,     -5,     -5,     -11,    -11,
			16,     16,     11,     11,     5,      5,      0,      0,
			0,      0,      5,      5,      11,     11,     16,     16,
			-16,    -16,    -11,    -11,    -5,     -5,     0,      0,
			0,      0,      -5,     -5,     -11,    -11,    -16,    -16,
			22,     16,     11,     11,     11,     5,      0,      0,
			0,      0,      5,      11,     11,     11,     16,     22,
			-22,    -16,    -11,    -11,    -11,    -5,     0,      0,
			0,      0,      -5,     -11,    -11,    -11,    -16,    -22,
			33,     27,     22,     22,     16,     11,     0,      0,
			0,      0,      11,     16,     22,     22,     27,     33,
			-33,    -27,    -22,    -22,    -16,    -11,    0,      0,
			0,      0,      -11,    -16,    -22,    -22,    -27,    -33,
			67,     56,     45,     45,     33,     22,     0,      0,
			0,      0,      22,     33,     45,     45,     56,     67,
			-67,    -56,    -45,    -45,    -33,    -22,    0,      0,
			0,      0,      -22,    -33,    -45,    -45,    -56,    -67,
			135,    112,    90,     90,     67,     45,     0,      0,
			0,      0,      45,     67,     90,     90,     112,    135,
			-135,   -112,   -90,    -90,    -67,    -45,    0,      0,
			0,      0,      -45,    -67,    -90,    -90,    -112,   -135,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			5,      5,      5,      5,      0,      0,      0,      0,
			0,      0,      0,      0,      5,      5,      5,      5,
			-5,     -5,     -5,     -5,     0,      0,      0,      0,
			0,      0,      0,      0,      -5,     -5,     -5,     -5,
			11,     11,     5,      5,      5,      0,      0,      0,
			0,      0,      0,      5,      5,      5,      11,     11,
			-11,    -11,    -5,     -5,     -5,     0,      0,      0,
			0,      0,      0,      -5,     -5,     -5,     -11,    -11,
			16,     16,     11,     11,     5,      5,      0,      0,
			0,      0,      5,      5,      11,     11,     16,     16,
			-16,    -16,    -11,    -11,    -5,     -5,     0,      0,
			0,      0,      -5,     -5,     -11,    -11,    -16,    -16,
			22,     16,     11,     11,     11,     5,      0,      0,
			0,      0,      5,      11,     11,     11,     16,     22,
			-22,    -16,    -11,    -11,    -11,    -5,     0,      0,
			0,      0,      -5,     -11,    -11,    -11,    -16,    -22,
			33,     27,     22,     22,     16,     11,     0,      0,
			0,      0,      11,     16,     22,     22,     27,     33,
			-33,    -27,    -22,    -22,    -16,    -11,    0,      0,
			0,      0,      -11,    -16,    -22,    -22,    -27,    -33,
			67,     56,     45,     45,     33,     22,     0,      0,
			0,      0,      22,     33,     45,     45,     56,     67,
			-67,    -56,    -45,    -45,    -33,    -22,    0,      0,
			0,      0,      -22,    -33,    -45,    -45,    -56,    -67,
			136,    113,    91,     91,     67,     45,     0,      0,
			0,      0,      45,     67,     91,     91,     113,    136,
			-136,   -113,   -91,    -91,    -67,    -45,    0,      0,
			0,      0,      -45,    -67,    -91,    -91,    -113,   -136,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			5,      5,      5,      5,      0,      0,      0,      0,
			0,      0,      0,      0,      5,      5,      5,      5,
			-5,     -5,     -5,     -5,     0,      0,      0,      0,
			0,      0,      0,      0,      -5,     -5,     -5,     -5,
			11,     11,     5,      5,      5,      0,      0,      0,
			0,      0,      0,      5,      5,      5,      11,     11,
			-11,    -11,    -5,     -5,     -5,     0,      0,      0,
			0,      0,      0,      -5,     -5,     -5,     -11,    -11,
			16,     16,     11,     11,     5,      5,      0,      0,
			0,      0,      5,      5,      11,     11,     16,     16,
			-16,    -16,    -11,    -11,    -5,     -5,     0,      0,
			0,      0,      -5,     -5,     -11,    -11,    -16,    -16,
			23,     16,     11,     11,     11,     5,      0,      0,
			0,      0,      5,      11,     11,     11,     16,     23,
			-23,    -16,    -11,    -11,    -11,    -5,     0,      0,
			0,      0,      -5,     -11,    -11,    -11,    -16,    -23,
			34,     28,     23,     23,     16,     11,     0,      0,
			0,      0,      11,     16,     23,     23,     28,     34,
			-34,    -28,    -23,    -23,    -16,    -11,    0,      0,
			0,      0,      -11,    -16,    -23,    -23,    -28,    -34,
			69,     57,     46,     46,     34,     23,     0,      0,
			0,      0,      23,     34,     46,     46,     57,     69,
			-69,    -57,    -46,    -46,    -34,    -23,    0,      0,
			0,      0,      -23,    -34,    -46,    -46,    -57,    -69,
			138,    115,    92,     92,     69,     46,     0,      0,
			0,      0,      46,     69,     92,     92,     115,    138,
			-138,   -115,   -92,    -92,    -69,    -46,    0,      0,
			0,      0,      -46,    -69,    -92,    -92,    -115,   -138,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			5,      5,      5,      5,      0,      0,      0,      0,
			0,      0,      0,      0,      5,      5,      5,      5,
			-5,     -5,     -5,     -5,     0,      0,      0,      0,
			0,      0,      0,      0,      -5,     -5,     -5,     -5,
			11,     11,     5,      5,      5,      0,      0,      0,
			0,      0,      0,      5,      5,      5,      11,     11,
			-11,    -11,    -5,     -5,     -5,     0,      0,      0,
			0,      0,      0,      -5,     -5,     -5,     -11,    -11,
			16,     16,     11,     11,     5,      5,      0,      0,
			0,      0,      5,      5,      11,     11,     16,     16,
			-16,    -16,    -11,    -11,    -5,     -5,     0,      0,
			0,      0,      -5,     -5,     -11,    -11,    -16,    -16,
			23,     16,     11,     11,     11,     5,      0,      0,
			0,      0,      5,      11,     11,     11,     16,     23,
			-23,    -16,    -11,    -11,    -11,    -5,     0,      0,
			0,      0,      -5,     -11,    -11,    -11,    -16,    -23,
			34,     28,     23,     23,     16,     11,     0,      0,
			0,      0,      11,     16,     23,     23,     28,     34,
			-34,    -28,    -23,    -23,    -16,    -11,    0,      0,
			0,      0,      -11,    -16,    -23,    -23,    -28,    -34,
			69,     57,     46,     46,     34,     23,     0,      0,
			0,      0,      23,     34,     46,     46,     57,     69,
			-69,    -57,    -46,    -46,    -34,    -23,    0,      0,
			0,      0,      -23,    -34,    -46,    -46,    -57,    -69,
			139,    116,    93,     93,     69,     46,     0,      0,
			0,      0,      46,     69,     93,     93,     116,    139,
			-139,   -116,   -93,    -93,    -69,    -46,    0,      0,
			0,      0,      -46,    -69,    -93,    -93,    -116,   -139,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			5,      5,      5,      5,      0,      0,      0,      0,
			0,      0,      0,      0,      5,      5,      5,      5,
			-5,     -5,     -5,     -5,     0,      0,      0,      0,
			0,      0,      0,      0,      -5,     -5,     -5,     -5,
			11,     11,     5,      5,      5,      0,      0,      0,
			0,      0,      0,      5,      5,      5,      11,     11,
			-11,    -11,    -5,     -5,     -5,     0,      0,      0,
			0,      0,      0,      -5,     -5,     -5,     -11,    -11,
			16,     16,     11,     11,     5,      5,      0,      0,
			0,      0,      5,      5,      11,     11,     16,     16,
			-16,    -16,    -11,    -11,    -5,     -5,     0,      0,
			0,      0,      -5,     -5,     -11,    -11,    -16,    -16,
			23,     16,     11,     11,     11,     5,      0,      0,
			0,      0,      5,      11,     11,     11,     16,     23,
			-23,    -16,    -11,    -11,    -11,    -5,     0,      0,
			0,      0,      -5,     -11,    -11,    -11,    -16,    -23,
			34,     28,     23,     23,     16,     11,     0,      0,
			0,      0,      11,     16,     23,     23,     28,     34,
			-34,    -28,    -23,    -23,    -16,    -11,    0,      0,
			0,      0,      -11,    -16,    -23,    -23,    -28,    -34,
			70,     58,     47,     47,     34,     23,     0,      0,
			0,      0,      23,     34,     47,     47,     58,     70,
			-70,    -58,    -47,    -47,    -34,    -23,    0,      0,
			0,      0,      -23,    -34,    -47,    -47,    -58,    -70,
			141,    117,    94,     94,     70,     47,     0,      0,
			0,      0,      47,     70,     94,     94,     117,    141,
			-141,   -117,   -94,    -94,    -70,    -47,    0,      0,
			0,      0,      -47,    -70,    -94,    -94,    -117,   -141,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			5,      5,      5,      5,      0,      0,      0,      0,
			0,      0,      0,      0,      5,      5,      5,      5,
			-5,     -5,     -5,     -5,     0,      0,      0,      0,
			0,      0,      0,      0,      -5,     -5,     -5,     -5,
			11,     11,     5,      5,      5,      0,      0,      0,
			0,      0,      0,      5,      5,      5,      11,     11,
			-11,    -11,    -5,     -5,     -5,     0,      0,      0,
			0,      0,      0,      -5,     -5,     -5,     -11,    -11,
			16,     16,     11,     11,     5,      5,      0,      0,
			0,      0,      5,      5,      11,     11,     16,     16,
			-16,    -16,    -11,    -11,    -5,     -5,     0,      0,
			0,      0,      -5,     -5,     -11,    -11,    -16,    -16,
			23,     16,     11,     11,     11,     5,      0,      0,
			0,      0,      5,      11,     11,     11,     16,     23,
			-23,    -16,    -11,    -11,    -11,    -5,     0,      0,
			0,      0,      -5,     -11,    -11,    -11,    -16,    -23,
			34,     28,     23,     23,     16,     11,     0,      0,
			0,      0,      11,     16,     23,     23,     28,     34,
			-34,    -28,    -23,    -23,    -16,    -11,    0,      0,
			0,      0,      -11,    -16,    -23,    -23,    -28,    -34,
			70,     58,     47,     47,     34,     23,     0,      0,
			0,      0,      23,     34,     47,     47,     58,     70,
			-70,    -58,    -47,    -47,    -34,    -23,    0,      0,
			0,      0,      -23,    -34,    -47,    -47,    -58,    -70,
			142,    118,    95,     95,     70,     47,     0,      0,
			0,      0,      47,     70,     95,     95,     118,    142,
			-142,   -118,   -95,    -95,    -70,    -47,    0,      0,
			0,      0,      -47,    -70,    -95,    -95,    -118,   -142,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			6,      6,      6,      6,      0,      0,      0,      0,
			0,      0,      0,      0,      6,      6,      6,      6,
			-6,     -6,     -6,     -6,     0,      0,      0,      0,
			0,      0,      0,      0,      -6,     -6,     -6,     -6,
			12,     12,     6,      6,      6,      0,      0,      0,
			0,      0,      0,      6,      6,      6,      12,     12,
			-12,    -12,    -6,     -6,     -6,     0,      0,      0,
			0,      0,      0,      -6,     -6,     -6,     -12,    -12,
			18,     18,     12,     12,     6,      6,      0,      0,
			0,      0,      6,      6,      12,     12,     18,     18,
			-18,    -18,    -12,    -12,    -6,     -6,     0,      0,
			0,      0,      -6,     -6,     -12,    -12,    -18,    -18,
			24,     18,     12,     12,     12,     6,      0,      0,
			0,      0,      6,      12,     12,     12,     18,     24,
			-24,    -18,    -12,    -12,    -12,    -6,     0,      0,
			0,      0,      -6,     -12,    -12,    -12,    -18,    -24,
			36,     30,     24,     24,     18,     12,     0,      0,
			0,      0,      12,     18,     24,     24,     30,     36,
			-36,    -30,    -24,    -24,    -18,    -12,    0,      0,
			0,      0,      -12,    -18,    -24,    -24,    -30,    -36,
			72,     60,     48,     48,     36,     24,     0,      0,
			0,      0,      24,     36,     48,     48,     60,     72,
			-72,    -60,    -48,    -48,    -36,    -24,    0,      0,
			0,      0,      -24,    -36,    -48,    -48,    -60,    -72,
			144,    120,    96,     96,     72,     48,     0,      0,
			0,      0,      48,     72,     96,     96,     120,    144,
			-144,   -120,   -96,    -96,    -72,    -48,    0,      0,
			0,      0,      -48,    -72,    -96,    -96,    -120,   -144,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			6,      6,      6,      6,      0,      0,      0,      0,
			0,      0,      0,      0,      6,      6,      6,      6,
			-6,     -6,     -6,     -6,     0,      0,      0,      0,
			0,      0,      0,      0,      -6,     -6,     -6,     -6,
			12,     12,     6,      6,      6,      0,      0,      0,
			0,      0,      0,      6,      6,      6,      12,     12,
			-12,    -12,    -6,     -6,     -6,     0,      0,      0,
			0,      0,      0,      -6,     -6,     -6,     -12,    -12,
			18,     18,     12,     12,     6,      6,      0,      0,
			0,      0,      6,      6,      12,     12,     18,     18,
			-18,    -18,    -12,    -12,    -6,     -6,     0,      0,
			0,      0,      -6,     -6,     -12,    -12,    -18,    -18,
			24,     18,     12,     12,     12,     6,      0,      0,
			0,      0,      6,      12,     12,     12,     18,     24,
			-24,    -18,    -12,    -12,    -12,    -6,     0,      0,
			0,      0,      -6,     -12,    -12,    -12,    -18,    -24,
			36,     30,     24,     24,     18,     12,     0,      0,
			0,      0,      12,     18,     24,     24,     30,     36,
			-36,    -30,    -24,    -24,    -18,    -12,    0,      0,
			0,      0,      -12,    -18,    -24,    -24,    -30,    -36,
			72,     60,     48,     48,     36,     24,     0,      0,
			0,      0,      24,     36,     48,     48,     60,     72,
			-72,    -60,    -48,    -48,    -36,    -24,    0,      0,
			0,      0,      -24,    -36,    -48,    -48,    -60,    -72,
			145,    121,    97,     97,     72,     48,     0,      0,
			0,      0,      48,     72,     97,     97,     121,    145,
			-145,   -121,   -97,    -97,    -72,    -48,    0,      0,
			0,      0,      -48,    -72,    -97,    -97,    -121,   -145,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			6,      6,      6,      6,      0,      0,      0,      0,
			0,      0,      0,      0,      6,      6,      6,      6,
			-6,     -6,     -6,     -6,     0,      0,      0,      0,
			0,      0,      0,      0,      -6,     -6,     -6,     -6,
			12,     12,     6,      6,      6,      0,      0,      0,
			0,      0,      0,      6,      6,      6,      12,     12,
			-12,    -12,    -6,     -6,     -6,     0,      0,      0,
			0,      0,      0,      -6,     -6,     -6,     -12,    -12,
			18,     18,     12,     12,     6,      6,      0,      0,
			0,      0,      6,      6,      12,     12,     18,     18,
			-18,    -18,    -12,    -12,    -6,     -6,     0,      0,
			0,      0,      -6,     -6,     -12,    -12,    -18,    -18,
			24,     18,     12,     12,     12,     6,      0,      0,
			0,      0,      6,      12,     12,     12,     18,     24,
			-24,    -18,    -12,    -12,    -12,    -6,     0,      0,
			0,      0,      -6,     -12,    -12,    -12,    -18,    -24,
			36,     30,     24,     24,     18,     12,     0,      0,
			0,      0,      12,     18,     24,     24,     30,     36,
			-36,    -30,    -24,    -24,    -18,    -12,    0,      0,
			0,      0,      -12,    -18,    -24,    -24,    -30,    -36,
			73,     61,     49,     49,     36,     24,     0,      0,
			0,      0,      24,     36,     49,     49,     61,     73,
			-73,    -61,    -49,    -49,    -36,    -24,    0,      0,
			0,      0,      -24,    -36,    -49,    -49,    -61,    -73,
			147,    122,    98,     98,     73,     49,     0,      0,
			0,      0,      49,     73,     98,     98,     122,    147,
			-147,   -122,   -98,    -98,    -73,    -49,    0,      0,
			0,      0,      -49,    -73,    -98,    -98,    -122,   -147,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			6,      6,      6,      6,      0,      0,      0,      0,
			0,      0,      0,      0,      6,      6,      6,      6,
			-6,     -6,     -6,     -6,     0,      0,      0,      0,
			0,      0,      0,      0,      -6,     -6,     -6,     -6,
			12,     12,     6,      6,      6,      0,      0,      0,
			0,      0,      0,      6,      6,      6,      12,     12,
			-12,    -12,    -6,     -6,     -6,     0,      0,      0,
			0,      0,      0,      -6,     -6,     -6,     -12,    -12,
			18,     18,     12,     12,     6,      6,      0,      0,
			0,      0,      6,      6,      12,     12,     18,     18,
			-18,    -18,    -12,    -12,    -6,     -6,     0,      0,
			0,      0,      -6,     -6,     -12,    -12,    -18,    -18,
			24,     18,     12,     12,     12,     6,      0,      0,
			0,      0,      6,      12,     12,     12,     18,     24,
			-24,    -18,    -12,    -12,    -12,    -6,     0,      0,
			0,      0,      -6,     -12,    -12,    -12,    -18,    -24,
			36,     30,     24,     24,     18,     12,     0,      0,
			0,      0,      12,     18,     24,     24,     30,     36,
			-36,    -30,    -24,    -24,    -18,    -12,    0,      0,
			0,      0,      -12,    -18,    -24,    -24,    -30,    -36,
			73,     61,     49,     49,     36,     24,     0,      0,
			0,      0,      24,     36,     49,     49,     61,     73,
			-73,    -61,    -49,    -49,    -36,    -24,    0,      0,
			0,      0,      -24,    -36,    -49,    -49,    -61,    -73,
			148,    123,    99,     99,     73,     49,     0,      0,
			0,      0,      49,     73,     99,     99,     123,    148,
			-148,   -123,   -99,    -99,    -73,    -49,    0,      0,
			0,      0,      -49,    -73,    -99,    -99,    -123,   -148,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			6,      6,      6,      6,      0,      0,      0,      0,
			0,      0,      0,      0,      6,      6,      6,      6,
			-6,     -6,     -6,     -6,     0,      0,      0,      0,
			0,      0,      0,      0,      -6,     -6,     -6,     -6,
			12,     12,     6,      6,      6,      0,      0,      0,
			0,      0,      0,      6,      6,      6,      12,     12,
			-12,    -12,    -6,     -6,     -6,     0,      0,      0,
			0,      0,      0,      -6,     -6,     -6,     -12,    -12,
			18,     18,     12,     12,     6,      6,      0,      0,
			0,      0,      6,      6,      12,     12,     18,     18,
			-18,    -18,    -12,    -12,    -6,     -6,     0,      0,
			0,      0,      -6,     -6,     -12,    -12,    -18,    -18,
			25,     18,     12,     12,     12,     6,      0,      0,
			0,      0,      6,      12,     12,     12,     18,     25,
			-25,    -18,    -12,    -12,    -12,    -6,     0,      0,
			0,      0,      -6,     -12,    -12,    -12,    -18,    -25,
			37,     31,     25,     25,     18,     12,     0,      0,
			0,      0,      12,     18,     25,     25,     31,     37,
			-37,    -31,    -25,    -25,    -18,    -12,    0,      0,
			0,      0,      -12,    -18,    -25,    -25,    -31,    -37,
			75,     62,     50,     50,     37,     25,     0,      0,
			0,      0,      25,     37,     50,     50,     62,     75,
			-75,    -62,    -50,    -50,    -37,    -25,    0,      0,
			0,      0,      -25,    -37,    -50,    -50,    -62,    -75,
			150,    125,    100,    100,    75,     50,     0,      0,
			0,      0,      50,     75,     100,    100,    125,    150,
			-150,   -125,   -100,   -100,   -75,    -50,    0,      0,
			0,      0,      -50,    -75,    -100,   -100,   -125,   -150,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			6,      6,      6,      6,      0,      0,      0,      0,
			0,      0,      0,      0,      6,      6,      6,      6,
			-6,     -6,     -6,     -6,     0,      0,      0,      0,
			0,      0,      0,      0,      -6,     -6,     -6,     -6,
			12,     12,     6,      6,      6,      0,      0,      0,
			0,      0,      0,      6,      6,      6,      12,     12,
			-12,    -12,    -6,     -6,     -6,     0,      0,      0,
			0,      0,      0,      -6,     -6,     -6,     -12,    -12,
			18,     18,     12,     12,     6,      6,      0,      0,
			0,      0,      6,      6,      12,     12,     18,     18,
			-18,    -18,    -12,    -12,    -6,     -6,     0,      0,
			0,      0,      -6,     -6,     -12,    -12,    -18,    -18,
			25,     18,     12,     12,     12,     6,      0,      0,
			0,      0,      6,      12,     12,     12,     18,     25,
			-25,    -18,    -12,    -12,    -12,    -6,     0,      0,
			0,      0,      -6,     -12,    -12,    -12,    -18,    -25,
			37,     31,     25,     25,     18,     12,     0,      0,
			0,      0,      12,     18,     25,     25,     31,     37,
			-37,    -31,    -25,    -25,    -18,    -12,    0,      0,
			0,      0,      -12,    -18,    -25,    -25,    -31,    -37,
			75,     62,     50,     50,     37,     25,     0,      0,
			0,      0,      25,     37,     50,     50,     62,     75,
			-75,    -62,    -50,    -50,    -37,    -25,    0,      0,
			0,      0,      -25,    -37,    -50,    -50,    -62,    -75,
			151,    126,    101,    101,    75,     50,     0,      0,
			0,      0,      50,     75,     101,    101,    126,    151,
			-151,   -126,   -101,   -101,   -75,    -50,    0,      0,
			0,      0,      -50,    -75,    -101,   -101,   -126,   -151,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			6,      6,      6,      6,      0,      0,      0,      0,
			0,      0,      0,      0,      6,      6,      6,      6,
			-6,     -6,     -6,     -6,     0,      0,      0,      0,
			0,      0,      0,      0,      -6,     -6,     -6,     -6,
			12,     12,     6,      6,      6,      0,      0,      0,
			0,      0,      0,      6,      6,      6,      12,     12,
			-12,    -12,    -6,     -6,     -6,     0,      0,      0,
			0,      0,      0,      -6,     -6,     -6,     -12,    -12,
			18,     18,     12,     12,     6,      6,      0,      0,
			0,      0,      6,      6,      12,     12,     18,     18,
			-18,    -18,    -12,    -12,    -6,     -6,     0,      0,
			0,      0,      -6,     -6,     -12,    -12,    -18,    -18,
			25,     18,     12,     12,     12,     6,      0,      0,
			0,      0,      6,      12,     12,     12,     18,     25,
			-25,    -18,    -12,    -12,    -12,    -6,     0,      0,
			0,      0,      -6,     -12,    -12,    -12,    -18,    -25,
			37,     31,     25,     25,     18,     12,     0,      0,
			0,      0,      12,     18,     25,     25,     31,     37,
			-37,    -31,    -25,    -25,    -18,    -12,    0,      0,
			0,      0,      -12,    -18,    -25,    -25,    -31,    -37,
			76,     63,     51,     51,     37,     25,     0,      0,
			0,      0,      25,     37,     51,     51,     63,     76,
			-76,    -63,    -51,    -51,    -37,    -25,    0,      0,
			0,      0,      -25,    -37,    -51,    -51,    -63,    -76,
			153,    127,    102,    102,    76,     51,     0,      0,
			0,      0,      51,     76,     102,    102,    127,    153,
			-153,   -127,   -102,   -102,   -76,    -51,    0,      0,
			0,      0,      -51,    -76,    -102,   -102,   -127,   -153,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			6,      6,      6,      6,      0,      0,      0,      0,
			0,      0,      0,      0,      6,      6,      6,      6,
			-6,     -6,     -6,     -6,     0,      0,      0,      0,
			0,      0,      0,      0,      -6,     -6,     -6,     -6,
			12,     12,     6,      6,      6,      0,      0,      0,
			0,      0,      0,      6,      6,      6,      12,     12,
			-12,    -12,    -6,     -6,     -6,     0,      0,      0,
			0,      0,      0,      -6,     -6,     -6,     -12,    -12,
			18,     18,     12,     12,     6,      6,      0,      0,
			0,      0,      6,      6,      12,     12,     18,     18,
			-18,    -18,    -12,    -12,    -6,     -6,     0,      0,
			0,      0,      -6,     -6,     -12,    -12,    -18,    -18,
			25,     18,     12,     12,     12,     6,      0,      0,
			0,      0,      6,      12,     12,     12,     18,     25,
			-25,    -18,    -12,    -12,    -12,    -6,     0,      0,
			0,      0,      -6,     -12,    -12,    -12,    -18,    -25,
			37,     31,     25,     25,     18,     12,     0,      0,
			0,      0,      12,     18,     25,     25,     31,     37,
			-37,    -31,    -25,    -25,    -18,    -12,    0,      0,
			0,      0,      -12,    -18,    -25,    -25,    -31,    -37,
			76,     63,     51,     51,     37,     25,     0,      0,
			0,      0,      25,     37,     51,     51,     63,     76,
			-76,    -63,    -51,    -51,    -37,    -25,    0,      0,
			0,      0,      -25,    -37,    -51,    -51,    -63,    -76,
			154,    128,    103,    103,    76,     51,     0,      0,
			0,      0,      51,     76,     103,    103,    128,    154,
			-154,   -128,   -103,   -103,   -76,    -51,    0,      0,
			0,      0,      -51,    -76,    -103,   -103,   -128,   -154,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			6,      6,      6,      6,      0,      0,      0,      0,
			0,      0,      0,      0,      6,      6,      6,      6,
			-6,     -6,     -6,     -6,     0,      0,      0,      0,
			0,      0,      0,      0,      -6,     -6,     -6,     -6,
			13,     13,     6,      6,      6,      0,      0,      0,
			0,      0,      0,      6,      6,      6,      13,     13,
			-13,    -13,    -6,     -6,     -6,     0,      0,      0,
			0,      0,      0,      -6,     -6,     -6,     -13,    -13,
			19,     19,     13,     13,     6,      6,      0,      0,
			0,      0,      6,      6,      13,     13,     19,     19,
			-19,    -19,    -13,    -13,    -6,     -6,     0,      0,
			0,      0,      -6,     -6,     -13,    -13,    -19,    -19,
			26,     19,     13,     13,     13,     6,      0,      0,
			0,      0,      6,      13,     13,     13,     19,     26,
			-26,    -19,    -13,    -13,    -13,    -6,     0,      0,
			0,      0,      -6,     -13,    -13,    -13,    -19,    -26,
			39,     32,     26,     26,     19,     13,     0,      0,
			0,      0,      13,     19,     26,     26,     32,     39,
			-39,    -32,    -26,    -26,    -19,    -13,    0,      0,
			0,      0,      -13,    -19,    -26,    -26,    -32,    -39,
			78,     65,     52,     52,     39,     26,     0,      0,
			0,      0,      26,     39,     52,     52,     65,     78,
			-78,    -65,    -52,    -52,    -39,    -26,    0,      0,
			0,      0,      -26,    -39,    -52,    -52,    -65,    -78,
			156,    130,    104,    104,    78,     52,     0,      0,
			0,      0,      52,     78,     104,    104,    130,    156,
			-156,   -130,   -104,   -104,   -78,    -52,    0,      0,
			0,      0,      -52,    -78,    -104,   -104,   -130,   -156,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			6,      6,      6,      6,      0,      0,      0,      0,
			0,      0,      0,      0,      6,      6,      6,      6,
			-6,     -6,     -6,     -6,     0,      0,      0,      0,
			0,      0,      0,      0,      -6,     -6,     -6,     -6,
			13,     13,     6,      6,      6,      0,      0,      0,
			0,      0,      0,      6,      6,      6,      13,     13,
			-13,    -13,    -6,     -6,     -6,     0,      0,      0,
			0,      0,      0,      -6,     -6,     -6,     -13,    -13,
			19,     19,     13,     13,     6,      6,      0,      0,
			0,      0,      6,      6,      13,     13,     19,     19,
			-19,    -19,    -13,    -13,    -6,     -6,     0,      0,
			0,      0,      -6,     -6,     -13,    -13,    -19,    -19,
			26,     19,     13,     13,     13,     6,      0,      0,
			0,      0,      6,      13,     13,     13,     19,     26,
			-26,    -19,    -13,    -13,    -13,    -6,     0,      0,
			0,      0,      -6,     -13,    -13,    -13,    -19,    -26,
			39,     32,     26,     26,     19,     13,     0,      0,
			0,      0,      13,     19,     26,     26,     32,     39,
			-39,    -32,    -26,    -26,    -19,    -13,    0,      0,
			0,      0,      -13,    -19,    -26,    -26,    -32,    -39,
			78,     65,     52,     52,     39,     26,     0,      0,
			0,      0,      26,     39,     52,     52,     65,     78,
			-78,    -65,    -52,    -52,    -39,    -26,    0,      0,
			0,      0,      -26,    -39,    -52,    -52,    -65,    -78,
			157,    131,    105,    105,    78,     52,     0,      0,
			0,      0,      52,     78,     105,    105,    131,    157,
			-157,   -131,   -105,   -105,   -78,    -52,    0,      0,
			0,      0,      -52,    -78,    -105,   -105,   -131,   -157,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			6,      6,      6,      6,      0,      0,      0,      0,
			0,      0,      0,      0,      6,      6,      6,      6,
			-6,     -6,     -6,     -6,     0,      0,      0,      0,
			0,      0,      0,      0,      -6,     -6,     -6,     -6,
			13,     13,     6,      6,      6,      0,      0,      0,
			0,      0,      0,      6,      6,      6,      13,     13,
			-13,    -13,    -6,     -6,     -6,     0,      0,      0,
			0,      0,      0,      -6,     -6,     -6,     -13,    -13,
			19,     19,     13,     13,     6,      6,      0,      0,
			0,      0,      6,      6,      13,     13,     19,     19,
			-19,    -19,    -13,    -13,    -6,     -6,     0,      0,
			0,      0,      -6,     -6,     -13,    -13,    -19,    -19,
			26,     19,     13,     13,     13,     6,      0,      0,
			0,      0,      6,      13,     13,     13,     19,     26,
			-26,    -19,    -13,    -13,    -13,    -6,     0,      0,
			0,      0,      -6,     -13,    -13,    -13,    -19,    -26,
			39,     32,     26,     26,     19,     13,     0,      0,
			0,      0,      13,     19,     26,     26,     32,     39,
			-39,    -32,    -26,    -26,    -19,    -13,    0,      0,
			0,      0,      -13,    -19,    -26,    -26,    -32,    -39,
			79,     66,     53,     53,     39,     26,     0,      0,
			0,      0,      26,     39,     53,     53,     66,     79,
			-79,    -66,    -53,    -53,    -39,    -26,    0,      0,
			0,      0,      -26,    -39,    -53,    -53,    -66,    -79,
			159,    132,    106,    106,    79,     53,     0,      0,
			0,      0,      53,     79,     106,    106,    132,    159,
			-159,   -132,   -106,   -106,   -79,    -53,    0,      0,
			0,      0,      -53,    -79,    -106,   -106,   -132,   -159,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			6,      6,      6,      6,      0,      0,      0,      0,
			0,      0,      0,      0,      6,      6,      6,      6,
			-6,     -6,     -6,     -6,     0,      0,      0,      0,
			0,      0,      0,      0,      -6,     -6,     -6,     -6,
			13,     13,     6,      6,      6,      0,      0,      0,
			0,      0,      0,      6,      6,      6,      13,     13,
			-13,    -13,    -6,     -6,     -6,     0,      0,      0,
			0,      0,      0,      -6,     -6,     -6,     -13,    -13,
			19,     19,     13,     13,     6,      6,      0,      0,
			0,      0,      6,      6,      13,     13,     19,     19,
			-19,    -19,    -13,    -13,    -6,     -6,     0,      0,
			0,      0,      -6,     -6,     -13,    -13,    -19,    -19,
			26,     19,     13,     13,     13,     6,      0,      0,
			0,      0,      6,      13,     13,     13,     19,     26,
			-26,    -19,    -13,    -13,    -13,    -6,     0,      0,
			0,      0,      -6,     -13,    -13,    -13,    -19,    -26,
			39,     32,     26,     26,     19,     13,     0,      0,
			0,      0,      13,     19,     26,     26,     32,     39,
			-39,    -32,    -26,    -26,    -19,    -13,    0,      0,
			0,      0,      -13,    -19,    -26,    -26,    -32,    -39,
			79,     66,     53,     53,     39,     26,     0,      0,
			0,      0,      26,     39,     53,     53,     66,     79,
			-79,    -66,    -53,    -53,    -39,    -26,    0,      0,
			0,      0,      -26,    -39,    -53,    -53,    -66,    -79,
			160,    133,    107,    107,    79,     53,     0,      0,
			0,      0,      53,     79,     107,    107,    133,    160,
			-160,   -133,   -107,   -107,   -79,    -53,    0,      0,
			0,      0,      -53,    -79,    -107,   -107,   -133,   -160,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			6,      6,      6,      6,      0,      0,      0,      0,
			0,      0,      0,      0,      6,      6,      6,      6,
			-6,     -6,     -6,     -6,     0,      0,      0,      0,
			0,      0,      0,      0,      -6,     -6,     -6,     -6,
			13,     13,     6,      6,      6,      0,      0,      0,
			0,      0,      0,      6,      6,      6,      13,     13,
			-13,    -13,    -6,     -6,     -6,     0,      0,      0,
			0,      0,      0,      -6,     -6,     -6,     -13,    -13,
			19,     19,     13,     13,     6,      6,      0,      0,
			0,      0,      6,      6,      13,     13,     19,     19,
			-19,    -19,    -13,    -13,    -6,     -6,     0,      0,
			0,      0,      -6,     -6,     -13,    -13,    -19,    -19,
			27,     19,     13,     13,     13,     6,      0,      0,
			0,      0,      6,      13,     13,     13,     19,     27,
			-27,    -19,    -13,    -13,    -13,    -6,     0,      0,
			0,      0,      -6,     -13,    -13,    -13,    -19,    -27,
			40,     33,     27,     27,     19,     13,     0,      0,
			0,      0,      13,     19,     27,     27,     33,     40,
			-40,    -33,    -27,    -27,    -19,    -13,    0,      0,
			0,      0,      -13,    -19,    -27,    -27,    -33,    -40,
			81,     67,     54,     54,     40,     27,     0,      0,
			0,      0,      27,     40,     54,     54,     67,     81,
			-81,    -67,    -54,    -54,    -40,    -27,    0,      0,
			0,      0,      -27,    -40,    -54,    -54,    -67,    -81,
			162,    135,    108,    108,    81,     54,     0,      0,
			0,      0,      54,     81,     108,    108,    135,    162,
			-162,   -135,   -108,   -108,   -81,    -54,    0,      0,
			0,      0,      -54,    -81,    -108,   -108,   -135,   -162,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			6,      6,      6,      6,      0,      0,      0,      0,
			0,      0,      0,      0,      6,      6,      6,      6,
			-6,     -6,     -6,     -6,     0,      0,      0,      0,
			0,      0,      0,      0,      -6,     -6,     -6,     -6,
			13,     13,     6,      6,      6,      0,      0,      0,
			0,      0,      0,      6,      6,      6,      13,     13,
			-13,    -13,    -6,     -6,     -6,     0,      0,      0,
			0,      0,      0,      -6,     -6,     -6,     -13,    -13,
			19,     19,     13,     13,     6,      6,      0,      0,
			0,      0,      6,      6,      13,     13,     19,     19,
			-19,    -19,    -13,    -13,    -6,     -6,     0,      0,
			0,      0,      -6,     -6,     -13,    -13,    -19,    -19,
			27,     19,     13,     13,     13,     6,      0,      0,
			0,      0,      6,      13,     13,     13,     19,     27,
			-27,    -19,    -13,    -13,    -13,    -6,     0,      0,
			0,      0,      -6,     -13,    -13,    -13,    -19,    -27,
			40,     33,     27,     27,     19,     13,     0,      0,
			0,      0,      13,     19,     27,     27,     33,     40,
			-40,    -33,    -27,    -27,    -19,    -13,    0,      0,
			0,      0,      -13,    -19,    -27,    -27,    -33,    -40,
			81,     67,     54,     54,     40,     27,     0,      0,
			0,      0,      27,     40,     54,     54,     67,     81,
			-81,    -67,    -54,    -54,    -40,    -27,    0,      0,
			0,      0,      -27,    -40,    -54,    -54,    -67,    -81,
			163,    136,    109,    109,    81,     54,     0,      0,
			0,      0,      54,     81,     109,    109,    136,    163,
			-163,   -136,   -109,   -109,   -81,    -54,    0,      0,
			0,      0,      -54,    -81,    -109,   -109,   -136,   -163,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			6,      6,      6,      6,      0,      0,      0,      0,
			0,      0,      0,      0,      6,      6,      6,      6,
			-6,     -6,     -6,     -6,     0,      0,      0,      0,
			0,      0,      0,      0,      -6,     -6,     -6,     -6,
			13,     13,     6,      6,      6,      0,      0,      0,
			0,      0,      0,      6,      6,      6,      13,     13,
			-13,    -13,    -6,     -6,     -6,     0,      0,      0,
			0,      0,      0,      -6,     -6,     -6,     -13,    -13,
			19,     19,     13,     13,     6,      6,      0,      0,
			0,      0,      6,      6,      13,     13,     19,     19,
			-19,    -19,    -13,    -13,    -6,     -6,     0,      0,
			0,      0,      -6,     -6,     -13,    -13,    -19,    -19,
			27,     19,     13,     13,     13,     6,      0,      0,
			0,      0,      6,      13,     13,     13,     19,     27,
			-27,    -19,    -13,    -13,    -13,    -6,     0,      0,
			0,      0,      -6,     -13,    -13,    -13,    -19,    -27,
			40,     33,     27,     27,     19,     13,     0,      0,
			0,      0,      13,     19,     27,     27,     33,     40,
			-40,    -33,    -27,    -27,    -19,    -13,    0,      0,
			0,      0,      -13,    -19,    -27,    -27,    -33,    -40,
			82,     68,     55,     55,     40,     27,     0,      0,
			0,      0,      27,     40,     55,     55,     68,     82,
			-82,    -68,    -55,    -55,    -40,    -27,    0,      0,
			0,      0,      -27,    -40,    -55,    -55,    -68,    -82,
			165,    137,    110,    110,    82,     55,     0,      0,
			0,      0,      55,     82,     110,    110,    137,    165,
			-165,   -137,   -110,   -110,   -82,    -55,    0,      0,
			0,      0,      -55,    -82,    -110,   -110,   -137,   -165,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			6,      6,      6,      6,      0,      0,      0,      0,
			0,      0,      0,      0,      6,      6,      6,      6,
			-6,     -6,     -6,     -6,     0,      0,      0,      0,
			0,      0,      0,      0,      -6,     -6,     -6,     -6,
			13,     13,     6,      6,      6,      0,      0,      0,
			0,      0,      0,      6,      6,      6,      13,     13,
			-13,    -13,    -6,     -6,     -6,     0,      0,      0,
			0,      0,      0,      -6,     -6,     -6,     -13,    -13,
			19,     19,     13,     13,     6,      6,      0,      0,
			0,      0,      6,      6,      13,     13,     19,     19,
			-19,    -19,    -13,    -13,    -6,     -6,     0,      0,
			0,      0,      -6,     -6,     -13,    -13,    -19,    -19,
			27,     19,     13,     13,     13,     6,      0,      0,
			0,      0,      6,      13,     13,     13,     19,     27,
			-27,    -19,    -13,    -13,    -13,    -6,     0,      0,
			0,      0,      -6,     -13,    -13,    -13,    -19,    -27,
			40,     33,     27,     27,     19,     13,     0,      0,
			0,      0,      13,     19,     27,     27,     33,     40,
			-40,    -33,    -27,    -27,    -19,    -13,    0,      0,
			0,      0,      -13,    -19,    -27,    -27,    -33,    -40,
			82,     68,     55,     55,     40,     27,     0,      0,
			0,      0,      27,     40,     55,     55,     68,     82,
			-82,    -68,    -55,    -55,    -40,    -27,    0,      0,
			0,      0,      -27,    -40,    -55,    -55,    -68,    -82,
			166,    138,    111,    111,    82,     55,     0,      0,
			0,      0,      55,     82,     111,    111,    138,    166,
			-166,   -138,   -111,   -111,   -82,    -55,    0,      0,
			0,      0,      -55,    -82,    -111,   -111,   -138,   -166,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			7,      7,      7,      7,      0,      0,      0,      0,
			0,      0,      0,      0,      7,      7,      7,      7,
			-7,     -7,     -7,     -7,     0,      0,      0,      0,
			0,      0,      0,      0,      -7,     -7,     -7,     -7,
			14,     14,     7,      7,      7,      0,      0,      0,
			0,      0,      0,      7,      7,      7,      14,     14,
			-14,    -14,    -7,     -7,     -7,     0,      0,      0,
			0,      0,      0,      -7,     -7,     -7,     -14,    -14,
			21,     21,     14,     14,     7,      7,      0,      0,
			0,      0,      7,      7,      14,     14,     21,     21,
			-21,    -21,    -14,    -14,    -7,     -7,     0,      0,
			0,      0,      -7,     -7,     -14,    -14,    -21,    -21,
			28,     21,     14,     14,     14,     7,      0,      0,
			0,      0,      7,      14,     14,     14,     21,     28,
			-28,    -21,    -14,    -14,    -14,    -7,     0,      0,
			0,      0,      -7,     -14,    -14,    -14,    -21,    -28,
			42,     35,     28,     28,     21,     14,     0,      0,
			0,      0,      14,     21,     28,     28,     35,     42,
			-42,    -35,    -28,    -28,    -21,    -14,    0,      0,
			0,      0,      -14,    -21,    -28,    -28,    -35,    -42,
			84,     70,     56,     56,     42,     28,     0,      0,
			0,      0,      28,     42,     56,     56,     70,     84,
			-84,    -70,    -56,    -56,    -42,    -28,    0,      0,
			0,      0,      -28,    -42,    -56,    -56,    -70,    -84,
			168,    140,    112,    112,    84,     56,     0,      0,
			0,      0,      56,     84,     112,    112,    140,    168,
			-168,   -140,   -112,   -112,   -84,    -56,    0,      0,
			0,      0,      -56,    -84,    -112,   -112,   -140,   -168,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			7,      7,      7,      7,      0,      0,      0,      0,
			0,      0,      0,      0,      7,      7,      7,      7,
			-7,     -7,     -7,     -7,     0,      0,      0,      0,
			0,      0,      0,      0,      -7,     -7,     -7,     -7,
			14,     14,     7,      7,      7,      0,      0,      0,
			0,      0,      0,      7,      7,      7,      14,     14,
			-14,    -14,    -7,     -7,     -7,     0,      0,      0,
			0,      0,      0,      -7,     -7,     -7,     -14,    -14,
			21,     21,     14,     14,     7,      7,      0,      0,
			0,      0,      7,      7,      14,     14,     21,     21,
			-21,    -21,    -14,    -14,    -7,     -7,     0,      0,
			0,      0,      -7,     -7,     -14,    -14,    -21,    -21,
			28,     21,     14,     14,     14,     7,      0,      0,
			0,      0,      7,      14,     14,     14,     21,     28,
			-28,    -21,    -14,    -14,    -14,    -7,     0,      0,
			0,      0,      -7,     -14,    -14,    -14,    -21,    -28,
			42,     35,     28,     28,     21,     14,     0,      0,
			0,      0,      14,     21,     28,     28,     35,     42,
			-42,    -35,    -28,    -28,    -21,    -14,    0,      0,
			0,      0,      -14,    -21,    -28,    -28,    -35,    -42,
			84,     70,     56,     56,     42,     28,     0,      0,
			0,      0,      28,     42,     56,     56,     70,     84,
			-84,    -70,    -56,    -56,    -42,    -28,    0,      0,
			0,      0,      -28,    -42,    -56,    -56,    -70,    -84,
			169,    141,    113,    113,    84,     56,     0,      0,
			0,      0,      56,     84,     113,    113,    141,    169,
			-169,   -141,   -113,   -113,   -84,    -56,    0,      0,
			0,      0,      -56,    -84,    -113,   -113,   -141,   -169,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			7,      7,      7,      7,      0,      0,      0,      0,
			0,      0,      0,      0,      7,      7,      7,      7,
			-7,     -7,     -7,     -7,     0,      0,      0,      0,
			0,      0,      0,      0,      -7,     -7,     -7,     -7,
			14,     14,     7,      7,      7,      0,      0,      0,
			0,      0,      0,      7,      7,      7,      14,     14,
			-14,    -14,    -7,     -7,     -7,     0,      0,      0,
			0,      0,      0,      -7,     -7,     -7,     -14,    -14,
			21,     21,     14,     14,     7,      7,      0,      0,
			0,      0,      7,      7,      14,     14,     21,     21,
			-21,    -21,    -14,    -14,    -7,     -7,     0,      0,
			0,      0,      -7,     -7,     -14,    -14,    -21,    -21,
			28,     21,     14,     14,     14,     7,      0,      0,
			0,      0,      7,      14,     14,     14,     21,     28,
			-28,    -21,    -14,    -14,    -14,    -7,     0,      0,
			0,      0,      -7,     -14,    -14,    -14,    -21,    -28,
			42,     35,     28,     28,     21,     14,     0,      0,
			0,      0,      14,     21,     28,     28,     35,     42,
			-42,    -35,    -28,    -28,    -21,    -14,    0,      0,
			0,      0,      -14,    -21,    -28,    -28,    -35,    -42,
			85,     71,     57,     57,     42,     28,     0,      0,
			0,      0,      28,     42,     57,     57,     71,     85,
			-85,    -71,    -57,    -57,    -42,    -28,    0,      0,
			0,      0,      -28,    -42,    -57,    -57,    -71,    -85,
			171,    142,    114,    114,    85,     57,     0,      0,
			0,      0,      57,     85,     114,    114,    142,    171,
			-171,   -142,   -114,   -114,   -85,    -57,    0,      0,
			0,      0,      -57,    -85,    -114,   -114,   -142,   -171,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			7,      7,      7,      7,      0,      0,      0,      0,
			0,      0,      0,      0,      7,      7,      7,      7,
			-7,     -7,     -7,     -7,     0,      0,      0,      0,
			0,      0,      0,      0,      -7,     -7,     -7,     -7,
			14,     14,     7,      7,      7,      0,      0,      0,
			0,      0,      0,      7,      7,      7,      14,     14,
			-14,    -14,    -7,     -7,     -7,     0,      0,      0,
			0,      0,      0,      -7,     -7,     -7,     -14,    -14,
			21,     21,     14,     14,     7,      7,      0,      0,
			0,      0,      7,      7,      14,     14,     21,     21,
			-21,    -21,    -14,    -14,    -7,     -7,     0,      0,
			0,      0,      -7,     -7,     -14,    -14,    -21,    -21,
			28,     21,     14,     14,     14,     7,      0,      0,
			0,      0,      7,      14,     14,     14,     21,     28,
			-28,    -21,    -14,    -14,    -14,    -7,     0,      0,
			0,      0,      -7,     -14,    -14,    -14,    -21,    -28,
			42,     35,     28,     28,     21,     14,     0,      0,
			0,      0,      14,     21,     28,     28,     35,     42,
			-42,    -35,    -28,    -28,    -21,    -14,    0,      0,
			0,      0,      -14,    -21,    -28,    -28,    -35,    -42,
			85,     71,     57,     57,     42,     28,     0,      0,
			0,      0,      28,     42,     57,     57,     71,     85,
			-85,    -71,    -57,    -57,    -42,    -28,    0,      0,
			0,      0,      -28,    -42,    -57,    -57,    -71,    -85,
			172,    143,    115,    115,    85,     57,     0,      0,
			0,      0,      57,     85,     115,    115,    143,    172,
			-172,   -143,   -115,   -115,   -85,    -57,    0,      0,
			0,      0,      -57,    -85,    -115,   -115,   -143,   -172,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			7,      7,      7,      7,      0,      0,      0,      0,
			0,      0,      0,      0,      7,      7,      7,      7,
			-7,     -7,     -7,     -7,     0,      0,      0,      0,
			0,      0,      0,      0,      -7,     -7,     -7,     -7,
			14,     14,     7,      7,      7,      0,      0,      0,
			0,      0,      0,      7,      7,      7,      14,     14,
			-14,    -14,    -7,     -7,     -7,     0,      0,      0,
			0,      0,      0,      -7,     -7,     -7,     -14,    -14,
			21,     21,     14,     14,     7,      7,      0,      0,
			0,      0,      7,      7,      14,     14,     21,     21,
			-21,    -21,    -14,    -14,    -7,     -7,     0,      0,
			0,      0,      -7,     -7,     -14,    -14,    -21,    -21,
			29,     21,     14,     14,     14,     7,      0,      0,
			0,      0,      7,      14,     14,     14,     21,     29,
			-29,    -21,    -14,    -14,    -14,    -7,     0,      0,
			0,      0,      -7,     -14,    -14,    -14,    -21,    -29,
			43,     36,     29,     29,     21,     14,     0,      0,
			0,      0,      14,     21,     29,     29,     36,     43,
			-43,    -36,    -29,    -29,    -21,    -14,    0,      0,
			0,      0,      -14,    -21,    -29,    -29,    -36,    -43,
			87,     72,     58,     58,     43,     29,     0,      0,
			0,      0,      29,     43,     58,     58,     72,     87,
			-87,    -72,    -58,    -58,    -43,    -29,    0,      0,
			0,      0,      -29,    -43,    -58,    -58,    -72,    -87,
			174,    145,    116,    116,    87,     58,     0,      0,
			0,      0,      58,     87,     116,    116,    145,    174,
			-174,   -145,   -116,   -116,   -87,    -58,    0,      0,
			0,      0,      -58,    -87,    -116,   -116,   -145,   -174,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			7,      7,      7,      7,      0,      0,      0,      0,
			0,      0,      0,      0,      7,      7,      7,      7,
			-7,     -7,     -7,     -7,     0,      0,      0,      0,
			0,      0,      0,      0,      -7,     -7,     -7,     -7,
			14,     14,     7,      7,      7,      0,      0,      0,
			0,      0,      0,      7,      7,      7,      14,     14,
			-14,    -14,    -7,     -7,     -7,     0,      0,      0,
			0,      0,      0,      -7,     -7,     -7,     -14,    -14,
			21,     21,     14,     14,     7,      7,      0,      0,
			0,      0,      7,      7,      14,     14,     21,     21,
			-21,    -21,    -14,    -14,    -7,     -7,     0,      0,
			0,      0,      -7,     -7,     -14,    -14,    -21,    -21,
			29,     21,     14,     14,     14,     7,      0,      0,
			0,      0,      7,      14,     14,     14,     21,     29,
			-29,    -21,    -14,    -14,    -14,    -7,     0,      0,
			0,      0,      -7,     -14,    -14,    -14,    -21,    -29,
			43,     36,     29,     29,     21,     14,     0,      0,
			0,      0,      14,     21,     29,     29,     36,     43,
			-43,    -36,    -29,    -29,    -21,    -14,    0,      0,
			0,      0,      -14,    -21,    -29,    -29,    -36,    -43,
			87,     72,     58,     58,     43,     29,     0,      0,
			0,      0,      29,     43,     58,     58,     72,     87,
			-87,    -72,    -58,    -58,    -43,    -29,    0,      0,
			0,      0,      -29,    -43,    -58,    -58,    -72,    -87,
			175,    146,    117,    117,    87,     58,     0,      0,
			0,      0,      58,     87,     117,    117,    146,    175,
			-175,   -146,   -117,   -117,   -87,    -58,    0,      0,
			0,      0,      -58,    -87,    -117,   -117,   -146,   -175,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			7,      7,      7,      7,      0,      0,      0,      0,
			0,      0,      0,      0,      7,      7,      7,      7,
			-7,     -7,     -7,     -7,     0,      0,      0,      0,
			0,      0,      0,      0,      -7,     -7,     -7,     -7,
			14,     14,     7,      7,      7,      0,      0,      0,
			0,      0,      0,      7,      7,      7,      14,     14,
			-14,    -14,    -7,     -7,     -7,     0,      0,      0,
			0,      0,      0,      -7,     -7,     -7,     -14,    -14,
			21,     21,     14,     14,     7,      7,      0,      0,
			0,      0,      7,      7,      14,     14,     21,     21,
			-21,    -21,    -14,    -14,    -7,     -7,     0,      0,
			0,      0,      -7,     -7,     -14,    -14,    -21,    -21,
			29,     21,     14,     14,     14,     7,      0,      0,
			0,      0,      7,      14,     14,     14,     21,     29,
			-29,    -21,    -14,    -14,    -14,    -7,     0,      0,
			0,      0,      -7,     -14,    -14,    -14,    -21,    -29,
			43,     36,     29,     29,     21,     14,     0,      0,
			0,      0,      14,     21,     29,     29,     36,     43,
			-43,    -36,    -29,    -29,    -21,    -14,    0,      0,
			0,      0,      -14,    -21,    -29,    -29,    -36,    -43,
			88,     73,     59,     59,     43,     29,     0,      0,
			0,      0,      29,     43,     59,     59,     73,     88,
			-88,    -73,    -59,    -59,    -43,    -29,    0,      0,
			0,      0,      -29,    -43,    -59,    -59,    -73,    -88,
			177,    147,    118,    118,    88,     59,     0,      0,
			0,      0,      59,     88,     118,    118,    147,    177,
			-177,   -147,   -118,   -118,   -88,    -59,    0,      0,
			0,      0,      -59,    -88,    -118,   -118,   -147,   -177,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			7,      7,      7,      7,      0,      0,      0,      0,
			0,      0,      0,      0,      7,      7,      7,      7,
			-7,     -7,     -7,     -7,     0,      0,      0,      0,
			0,      0,      0,      0,      -7,     -7,     -7,     -7,
			14,     14,     7,      7,      7,      0,      0,      0,
			0,      0,      0,      7,      7,      7,      14,     14,
			-14,    -14,    -7,     -7,     -7,     0,      0,      0,
			0,      0,      0,      -7,     -7,     -7,     -14,    -14,
			21,     21,     14,     14,     7,      7,      0,      0,
			0,      0,      7,      7,      14,     14,     21,     21,
			-21,    -21,    -14,    -14,    -7,     -7,     0,      0,
			0,      0,      -7,     -7,     -14,    -14,    -21,    -21,
			29,     21,     14,     14,     14,     7,      0,      0,
			0,      0,      7,      14,     14,     14,     21,     29,
			-29,    -21,    -14,    -14,    -14,    -7,     0,      0,
			0,      0,      -7,     -14,    -14,    -14,    -21,    -29,
			43,     36,     29,     29,     21,     14,     0,      0,
			0,      0,      14,     21,     29,     29,     36,     43,
			-43,    -36,    -29,    -29,    -21,    -14,    0,      0,
			0,      0,      -14,    -21,    -29,    -29,    -36,    -43,
			88,     73,     59,     59,     43,     29,     0,      0,
			0,      0,      29,     43,     59,     59,     73,     88,
			-88,    -73,    -59,    -59,    -43,    -29,    0,      0,
			0,      0,      -29,    -43,    -59,    -59,    -73,    -88,
			178,    148,    119,    119,    88,     59,     0,      0,
			0,      0,      59,     88,     119,    119,    148,    178,
			-178,   -148,   -119,   -119,   -88,    -59,    0,      0,
			0,      0,      -59,    -88,    -119,   -119,   -148,   -178,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			7,      7,      7,      7,      0,      0,      0,      0,
			0,      0,      0,      0,      7,      7,      7,      7,
			-7,     -7,     -7,     -7,     0,      0,      0,      0,
			0,      0,      0,      0,      -7,     -7,     -7,     -7,
			15,     15,     7,      7,      7,      0,      0,      0,
			0,      0,      0,      7,      7,      7,      15,     15,
			-15,    -15,    -7,     -7,     -7,     0,      0,      0,
			0,      0,      0,      -7,     -7,     -7,     -15,    -15,
			22,     22,     15,     15,     7,      7,      0,      0,
			0,      0,      7,      7,      15,     15,     22,     22,
			-22,    -22,    -15,    -15,    -7,     -7,     0,      0,
			0,      0,      -7,     -7,     -15,    -15,    -22,    -22,
			30,     22,     15,     15,     15,     7,      0,      0,
			0,      0,      7,      15,     15,     15,     22,     30,
			-30,    -22,    -15,    -15,    -15,    -7,     0,      0,
			0,      0,      -7,     -15,    -15,    -15,    -22,    -30,
			45,     37,     30,     30,     22,     15,     0,      0,
			0,      0,      15,     22,     30,     30,     37,     45,
			-45,    -37,    -30,    -30,    -22,    -15,    0,      0,
			0,      0,      -15,    -22,    -30,    -30,    -37,    -45,
			90,     75,     60,     60,     45,     30,     0,      0,
			0,      0,      30,     45,     60,     60,     75,     90,
			-90,    -75,    -60,    -60,    -45,    -30,    0,      0,
			0,      0,      -30,    -45,    -60,    -60,    -75,    -90,
			180,    150,    120,    120,    90,     60,     0,      0,
			0,      0,      60,     90,     120,    120,    150,    180,
			-180,   -150,   -120,   -120,   -90,    -60,    0,      0,
			0,      0,      -60,    -90,    -120,   -120,   -150,   -180,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			7,      7,      7,      7,      0,      0,      0,      0,
			0,      0,      0,      0,      7,      7,      7,      7,
			-7,     -7,     -7,     -7,     0,      0,      0,      0,
			0,      0,      0,      0,      -7,     -7,     -7,     -7,
			15,     15,     7,      7,      7,      0,      0,      0,
			0,      0,      0,      7,      7,      7,      15,     15,
			-15,    -15,    -7,     -7,     -7,     0,      0,      0,
			0,      0,      0,      -7,     -7,     -7,     -15,    -15,
			22,     22,     15,     15,     7,      7,      0,      0,
			0,      0,      7,      7,      15,     15,     22,     22,
			-22,    -22,    -15,    -15,    -7,     -7,     0,      0,
			0,      0,      -7,     -7,     -15,    -15,    -22,    -22,
			30,     22,     15,     15,     15,     7,      0,      0,
			0,      0,      7,      15,     15,     15,     22,     30,
			-30,    -22,    -15,    -15,    -15,    -7,     0,      0,
			0,      0,      -7,     -15,    -15,    -15,    -22,    -30,
			45,     37,     30,     30,     22,     15,     0,      0,
			0,      0,      15,     22,     30,     30,     37,     45,
			-45,    -37,    -30,    -30,    -22,    -15,    0,      0,
			0,      0,      -15,    -22,    -30,    -30,    -37,    -45,
			90,     75,     60,     60,     45,     30,     0,      0,
			0,      0,      30,     45,     60,     60,     75,     90,
			-90,    -75,    -60,    -60,    -45,    -30,    0,      0,
			0,      0,      -30,    -45,    -60,    -60,    -75,    -90,
			181,    151,    121,    121,    90,     60,     0,      0,
			0,      0,      60,     90,     121,    121,    151,    181,
			-181,   -151,   -121,   -121,   -90,    -60,    0,      0,
			0,      0,      -60,    -90,    -121,   -121,   -151,   -181,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			7,      7,      7,      7,      0,      0,      0,      0,
			0,      0,      0,      0,      7,      7,      7,      7,
			-7,     -7,     -7,     -7,     0,      0,      0,      0,
			0,      0,      0,      0,      -7,     -7,     -7,     -7,
			15,     15,     7,      7,      7,      0,      0,      0,
			0,      0,      0,      7,      7,      7,      15,     15,
			-15,    -15,    -7,     -7,     -7,     0,      0,      0,
			0,      0,      0,      -7,     -7,     -7,     -15,    -15,
			22,     22,     15,     15,     7,      7,      0,      0,
			0,      0,      7,      7,      15,     15,     22,     22,
			-22,    -22,    -15,    -15,    -7,     -7,     0,      0,
			0,      0,      -7,     -7,     -15,    -15,    -22,    -22,
			30,     22,     15,     15,     15,     7,      0,      0,
			0,      0,      7,      15,     15,     15,     22,     30,
			-30,    -22,    -15,    -15,    -15,    -7,     0,      0,
			0,      0,      -7,     -15,    -15,    -15,    -22,    -30,
			45,     37,     30,     30,     22,     15,     0,      0,
			0,      0,      15,     22,     30,     30,     37,     45,
			-45,    -37,    -30,    -30,    -22,    -15,    0,      0,
			0,      0,      -15,    -22,    -30,    -30,    -37,    -45,
			91,     76,     61,     61,     45,     30,     0,      0,
			0,      0,      30,     45,     61,     61,     76,     91,
			-91,    -76,    -61,    -61,    -45,    -30,    0,      0,
			0,      0,      -30,    -45,    -61,    -61,    -76,    -91,
			183,    152,    122,    122,    91,     61,     0,      0,
			0,      0,      61,     91,     122,    122,    152,    183,
			-183,   -152,   -122,   -122,   -91,    -61,    0,      0,
			0,      0,      -61,    -91,    -122,   -122,   -152,   -183,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			7,      7,      7,      7,      0,      0,      0,      0,
			0,      0,      0,      0,      7,      7,      7,      7,
			-7,     -7,     -7,     -7,     0,      0,      0,      0,
			0,      0,      0,      0,      -7,     -7,     -7,     -7,
			15,     15,     7,      7,      7,      0,      0,      0,
			0,      0,      0,      7,      7,      7,      15,     15,
			-15,    -15,    -7,     -7,     -7,     0,      0,      0,
			0,      0,      0,      -7,     -7,     -7,     -15,    -15,
			22,     22,     15,     15,     7,      7,      0,      0,
			0,      0,      7,      7,      15,     15,     22,     22,
			-22,    -22,    -15,    -15,    -7,     -7,     0,      0,
			0,      0,      -7,     -7,     -15,    -15,    -22,    -22,
			30,     22,     15,     15,     15,     7,      0,      0,
			0,      0,      7,      15,     15,     15,     22,     30,
			-30,    -22,    -15,    -15,    -15,    -7,     0,      0,
			0,      0,      -7,     -15,    -15,    -15,    -22,    -30,
			45,     37,     30,     30,     22,     15,     0,      0,
			0,      0,      15,     22,     30,     30,     37,     45,
			-45,    -37,    -30,    -30,    -22,    -15,    0,      0,
			0,      0,      -15,    -22,    -30,    -30,    -37,    -45,
			91,     76,     61,     61,     45,     30,     0,      0,
			0,      0,      30,     45,     61,     61,     76,     91,
			-91,    -76,    -61,    -61,    -45,    -30,    0,      0,
			0,      0,      -30,    -45,    -61,    -61,    -76,    -91,
			184,    153,    123,    123,    91,     61,     0,      0,
			0,      0,      61,     91,     123,    123,    153,    184,
			-184,   -153,   -123,   -123,   -91,    -61,    0,      0,
			0,      0,      -61,    -91,    -123,   -123,   -153,   -184,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			7,      7,      7,      7,      0,      0,      0,      0,
			0,      0,      0,      0,      7,      7,      7,      7,
			-7,     -7,     -7,     -7,     0,      0,      0,      0,
			0,      0,      0,      0,      -7,     -7,     -7,     -7,
			15,     15,     7,      7,      7,      0,      0,      0,
			0,      0,      0,      7,      7,      7,      15,     15,
			-15,    -15,    -7,     -7,     -7,     0,      0,      0,
			0,      0,      0,      -7,     -7,     -7,     -15,    -15,
			22,     22,     15,     15,     7,      7,      0,      0,
			0,      0,      7,      7,      15,     15,     22,     22,
			-22,    -22,    -15,    -15,    -7,     -7,     0,      0,
			0,      0,      -7,     -7,     -15,    -15,    -22,    -22,
			31,     22,     15,     15,     15,     7,      0,      0,
			0,      0,      7,      15,     15,     15,     22,     31,
			-31,    -22,    -15,    -15,    -15,    -7,     0,      0,
			0,      0,      -7,     -15,    -15,    -15,    -22,    -31,
			46,     38,     31,     31,     22,     15,     0,      0,
			0,      0,      15,     22,     31,     31,     38,     46,
			-46,    -38,    -31,    -31,    -22,    -15,    0,      0,
			0,      0,      -15,    -22,    -31,    -31,    -38,    -46,
			93,     77,     62,     62,     46,     31,     0,      0,
			0,      0,      31,     46,     62,     62,     77,     93,
			-93,    -77,    -62,    -62,    -46,    -31,    0,      0,
			0,      0,      -31,    -46,    -62,    -62,    -77,    -93,
			186,    155,    124,    124,    93,     62,     0,      0,
			0,      0,      62,     93,     124,    124,    155,    186,
			-186,   -155,   -124,   -124,   -93,    -62,    0,      0,
			0,      0,      -62,    -93,    -124,   -124,   -155,   -186,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			7,      7,      7,      7,      0,      0,      0,      0,
			0,      0,      0,      0,      7,      7,      7,      7,
			-7,     -7,     -7,     -7,     0,      0,      0,      0,
			0,      0,      0,      0,      -7,     -7,     -7,     -7,
			15,     15,     7,      7,      7,      0,      0,      0,
			0,      0,      0,      7,      7,      7,      15,     15,
			-15,    -15,    -7,     -7,     -7,     0,      0,      0,
			0,      0,      0,      -7,     -7,     -7,     -15,    -15,
			22,     22,     15,     15,     7,      7,      0,      0,
			0,      0,      7,      7,      15,     15,     22,     22,
			-22,    -22,    -15,    -15,    -7,     -7,     0,      0,
			0,      0,      -7,     -7,     -15,    -15,    -22,    -22,
			31,     22,     15,     15,     15,     7,      0,      0,
			0,      0,      7,      15,     15,     15,     22,     31,
			-31,    -22,    -15,    -15,    -15,    -7,     0,      0,
			0,      0,      -7,     -15,    -15,    -15,    -22,    -31,
			46,     38,     31,     31,     22,     15,     0,      0,
			0,      0,      15,     22,     31,     31,     38,     46,
			-46,    -38,    -31,    -31,    -22,    -15,    0,      0,
			0,      0,      -15,    -22,    -31,    -31,    -38,    -46,
			93,     77,     62,     62,     46,     31,     0,      0,
			0,      0,      31,     46,     62,     62,     77,     93,
			-93,    -77,    -62,    -62,    -46,    -31,    0,      0,
			0,      0,      -31,    -46,    -62,    -62,    -77,    -93,
			187,    156,    125,    125,    93,     62,     0,      0,
			0,      0,      62,     93,     125,    125,    156,    187,
			-187,   -156,   -125,   -125,   -93,    -62,    0,      0,
			0,      0,      -62,    -93,    -125,   -125,   -156,   -187,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			7,      7,      7,      7,      0,      0,      0,      0,
			0,      0,      0,      0,      7,      7,      7,      7,
			-7,     -7,     -7,     -7,     0,      0,      0,      0,
			0,      0,      0,      0,      -7,     -7,     -7,     -7,
			15,     15,     7,      7,      7,      0,      0,      0,
			0,      0,      0,      7,      7,      7,      15,     15,
			-15,    -15,    -7,     -7,     -7,     0,      0,      0,
			0,      0,      0,      -7,     -7,     -7,     -15,    -15,
			22,     22,     15,     15,     7,      7,      0,      0,
			0,      0,      7,      7,      15,     15,     22,     22,
			-22,    -22,    -15,    -15,    -7,     -7,     0,      0,
			0,      0,      -7,     -7,     -15,    -15,    -22,    -22,
			31,     22,     15,     15,     15,     7,      0,      0,
			0,      0,      7,      15,     15,     15,     22,     31,
			-31,    -22,    -15,    -15,    -15,    -7,     0,      0,
			0,      0,      -7,     -15,    -15,    -15,    -22,    -31,
			46,     38,     31,     31,     22,     15,     0,      0,
			0,      0,      15,     22,     31,     31,     38,     46,
			-46,    -38,    -31,    -31,    -22,    -15,    0,      0,
			0,      0,      -15,    -22,    -31,    -31,    -38,    -46,
			94,     78,     63,     63,     46,     31,     0,      0,
			0,      0,      31,     46,     63,     63,     78,     94,
			-94,    -78,    -63,    -63,    -46,    -31,    0,      0,
			0,      0,      -31,    -46,    -63,    -63,    -78,    -94,
			189,    157,    126,    126,    94,     63,     0,      0,
			0,      0,      63,     94,     126,    126,    157,    189,
			-189,   -157,   -126,   -126,   -94,    -63,    0,      0,
			0,      0,      -63,    -94,    -126,   -126,   -157,   -189,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			0,      0,      0,      0,      0,      0,      0,      0,
			7,      7,      7,      7,      0,      0,      0,      0,
			0,      0,      0,      0,      7,      7,      7,      7,
			-7,     -7,     -7,     -7,     0,      0,      0,      0,
			0,      0,      0,      0,      -7,     -7,     -7,     -7,
			15,     15,     7,      7,      7,      0,      0,      0,
			0,      0,      0,      7,      7,      7,      15,     15,
			-15,    -15,    -7,     -7,     -7,     0,      0,      0,
			0,      0,      0,      -7,     -7,     -7,     -15,    -15,
			22,     22,     15,     15,     7,      7,      0,      0,
			0,      0,      7,      7,      15,     15,     22,     22,
			-22,    -22,    -15,    -15,    -7,     -7,     0,      0,
			0,      0,      -7,     -7,     -15,    -15,    -22,    -22,
			31,     22,     15,     15,     15,     7,      0,      0,
			0,      0,      7,      15,     15,     15,     22,     31,
			-31,    -22,    -15,    -15,    -15,    -7,     0,      0,
			0,      0,      -7,     -15,    -15,    -15,    -22,    -31,
			46,     38,     31,     31,     22,     15,     0,      0,
			0,      0,      15,     22,     31,     31,     38,     46,
			-46,    -38,    -31,    -31,    -22,    -15,    0,      0,
			0,      0,      -15,    -22,    -31,    -31,    -38,    -46,
			94,     78,     63,     63,     46,     31,     0,      0,
			0,      0,      31,     46,     63,     63,     78,     94,
			-94,    -78,    -63,    -63,    -46,    -31,    0,      0,
			0,      0,      -31,    -46,    -63,    -63,    -78,    -94,
			190,    158,    127,    127,    94,     63,     0,      0,
			0,      0,      63,     94,     127,    127,    158,    190,
			-190,   -158,   -127,   -127,   -94,    -63,    0,      0,
			0,      0,      -63,    -94,    -127,   -127,   -158,   -190,
};

/* register number to channel number , slot offset */
#define OPN_CHAN(N) (N&3)
#define OPN_SLOT(N) ((N>>2)&3)

/* slot number */
#define SLOT1 0
#define SLOT2 2
#define SLOT3 1
#define SLOT4 3

/* struct describing a single operator (SLOT) */
typedef struct
{
  INT32   *DT;        /* detune          :dt_tab[DT]      */
  UINT8   KSR;        /* key scale rate  :3-KSR           */
  UINT32  ar;         /* attack rate                      */
  UINT32  d1r;        /* decay rate                       */
  UINT32  d2r;        /* sustain rate                     */
  UINT32  rr;         /* release rate                     */
  UINT8   ksr;        /* key scale rate  :kcode>>(3-KSR)  */
  UINT32  mul;        /* multiple        :ML_TABLE[ML]    */

  /* Phase Generator */
  UINT32  phase;      /* phase counter */
  INT32   Incr;       /* phase step */

  /* Envelope Generator */
  UINT8   state;      /* phase type */
  UINT32  tl;         /* total level: TL << 3 */
  INT32   volume;     /* envelope counter */
  UINT32  sl;         /* sustain level:sl_table[SL] */
  UINT32  vol_out;    /* current output from EG circuit (without AM from LFO) */

  UINT8  eg_sh_ar;    /*  (attack state)  */
  UINT8  eg_sel_ar;   /*  (attack state)  */
  UINT8  eg_sh_d1r;   /*  (decay state)   */
  UINT8  eg_sel_d1r;  /*  (decay state)   */
  UINT8  eg_sh_d2r;   /*  (sustain state) */
  UINT8  eg_sel_d2r;  /*  (sustain state) */
  UINT8  eg_sh_rr;    /*  (release state) */
  UINT8  eg_sel_rr;   /*  (release state) */

  UINT8  ssg;         /* SSG-EG waveform  */
  UINT8  ssgn;        /* SSG-EG negated output  */

  UINT8  key;         /* 0=last key was KEY OFF, 1=KEY ON */

  /* LFO */
  UINT32  AMmask;     /* AM enable flag */

} FM_SLOT;

typedef struct
{
  FM_SLOT  SLOT[4];     /* four SLOTs (operators) */

  UINT8   ALGO;         /* algorithm */
  UINT8   FB;           /* feedback shift */
  INT32   op1_out[2];   /* op1 output for feedback */

  INT32   *connect1;    /* SLOT1 output pointer */
  INT32   *connect3;    /* SLOT3 output pointer */
  INT32   *connect2;    /* SLOT2 output pointer */
  INT32   *connect4;    /* SLOT4 output pointer */

  INT32   *mem_connect; /* where to put the delayed sample (MEM) */
  INT32   mem_value;    /* delayed sample (MEM) value */

  INT32   pms;          /* channel PMS */
  UINT8   ams;          /* channel AMS */

  UINT32  fc;           /* fnum,blk */
  UINT8   kcode;        /* key code */
  UINT32  block_fnum;   /* blk/fnum value (for LFO PM calculations) */
} FM_CH;


typedef struct
{
  UINT16  address;        /* address register     */
  UINT8   status;         /* status flag          */
  UINT32  mode;           /* mode  CSM / 3SLOT    */
  UINT8   fn_h;           /* freq latch           */
  INT32   TA;             /* timer a value        */
  INT32   TAL;            /* timer a base         */
  INT32   TAC;            /* timer a counter      */
  INT32   TB;             /* timer b value        */
  INT32   TBL;            /* timer b base         */
  INT32   TBC;            /* timer b counter      */
  INT32   dt_tab[8][32];  /* DeTune table         */

} FM_ST;


/***********************************************************/
/* OPN unit                                                */
/***********************************************************/

/* OPN 3slot struct */
typedef struct
{
  UINT32  fc[3];          /* fnum3,blk3: calculated */
  UINT8   fn_h;           /* freq3 latch */
  UINT8   kcode[3];       /* key code */
  UINT32  block_fnum[3];  /* current fnum value for this slot (can be different betweeen slots of one channel in 3slot mode) */
  UINT8   key_csm;        /* CSM mode Key-ON flag */

} FM_3SLOT;

/* OPN/A/B common state */
typedef struct
{
  FM_ST  ST;                  /* general state */
  FM_3SLOT SL3;               /* 3 slot mode state */
  unsigned int pan[6*2];      /* fm channels output masks (0xffffffff = enable) */

  /* EG */
  UINT32  eg_cnt;             /* global envelope generator counter */
  UINT32  eg_timer;           /* global envelope generator counter works at frequency = chipclock/144/3 */

  /* LFO */
  UINT8   lfo_cnt;            /* current LFO phase (out of 128) */
  UINT32  lfo_timer;          /* current LFO phase runs at LFO frequency */
  UINT32  lfo_timer_overflow; /* LFO timer overflows every N samples (depends on LFO frequency) */
  UINT32  LFO_AM;             /* current LFO AM step */
  UINT32  LFO_PM;             /* current LFO PM step */

} FM_OPN;

/***********************************************************/
/* YM2612 chip                                                */
/***********************************************************/
typedef struct
{
  FM_CH   CH[6];  /* channel state */
  UINT8   dacen;  /* DAC mode  */
  INT32   dacout; /* DAC output */
  FM_OPN  OPN;    /* OPN state */

} YM2612;

/* emulated chip */
static YM2612 ym2612;

/* current chip state */
static INT32  m2,c1,c2;   /* Phase Modulation input for operators 2,3,4 */
static INT32  mem;        /* one sample delay memory */
static INT32  out_fm[8];  /* outputs of working channels */
static UINT32 bitmask;    /* working channels output bitmasking (DAC quantization) */


INLINE void FM_KEYON(FM_CH *CH , int s )
{
  FM_SLOT *SLOT = &CH->SLOT[s];

  if (!SLOT->key && !ym2612.OPN.SL3.key_csm)
  {
    /* restart Phase Generator */
    SLOT->phase = 0;

    /* reset SSG-EG inversion flag */
    SLOT->ssgn = 0;

    if ((SLOT->ar + SLOT->ksr) < 94 /*32+62*/)
    {
      SLOT->state = (SLOT->volume <= MIN_ATT_INDEX) ? ((SLOT->sl == MIN_ATT_INDEX) ? EG_SUS : EG_DEC) : EG_ATT;
    }
    else
    {
      /* force attenuation level to 0 */
      SLOT->volume = MIN_ATT_INDEX;

      /* directly switch to Decay (or Sustain) */
      SLOT->state = (SLOT->sl == MIN_ATT_INDEX) ? EG_SUS : EG_DEC;
    }

    /* recalculate EG output */
    if ((SLOT->ssg&0x08) && (SLOT->ssgn ^ (SLOT->ssg&0x04)))
      SLOT->vol_out = ((UINT32)(0x200 - SLOT->volume) & MAX_ATT_INDEX) + SLOT->tl;
    else
      SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
  }

  SLOT->key = 1;
}

INLINE void FM_KEYOFF(FM_CH *CH , int s )
{
  FM_SLOT *SLOT = &CH->SLOT[s];

  if (SLOT->key && !ym2612.OPN.SL3.key_csm)
  {
    if (SLOT->state>EG_REL)
    {
      SLOT->state = EG_REL; /* phase -> Release */

      /* SSG-EG specific update */
      if (SLOT->ssg&0x08)
      {
        /* convert EG attenuation level */
        if (SLOT->ssgn ^ (SLOT->ssg&0x04))
          SLOT->volume = (0x200 - SLOT->volume);

        /* force EG attenuation level */
        if (SLOT->volume >= 0x200)
        {
          SLOT->volume = MAX_ATT_INDEX;
          SLOT->state  = EG_OFF;
        }

        /* recalculate EG output */
        SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
      }
    }
  }

  SLOT->key = 0;
}

INLINE void FM_KEYON_CSM(FM_CH *CH , int s )
{
  FM_SLOT *SLOT = &CH->SLOT[s];

  if (!SLOT->key && !ym2612.OPN.SL3.key_csm)
  {
    /* restart Phase Generator */
    SLOT->phase = 0;

    /* reset SSG-EG inversion flag */
    SLOT->ssgn = 0;

    if ((SLOT->ar + SLOT->ksr) < 94 /*32+62*/)
    {
      SLOT->state = (SLOT->volume <= MIN_ATT_INDEX) ? ((SLOT->sl == MIN_ATT_INDEX) ? EG_SUS : EG_DEC) : EG_ATT;
    }
    else
    {
      /* force attenuation level to 0 */
      SLOT->volume = MIN_ATT_INDEX;

      /* directly switch to Decay (or Sustain) */
      SLOT->state = (SLOT->sl == MIN_ATT_INDEX) ? EG_SUS : EG_DEC;
    }

    /* recalculate EG output */
    if ((SLOT->ssg&0x08) && (SLOT->ssgn ^ (SLOT->ssg&0x04)))
      SLOT->vol_out = ((UINT32)(0x200 - SLOT->volume) & MAX_ATT_INDEX) + SLOT->tl;
    else
      SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
  }
}

INLINE void FM_KEYOFF_CSM(FM_CH *CH , int s )
{
  FM_SLOT *SLOT = &CH->SLOT[s];
  if (!SLOT->key)
  {
    if (SLOT->state>EG_REL)
    {
      SLOT->state = EG_REL; /* phase -> Release */

      /* SSG-EG specific update */
      if (SLOT->ssg&0x08)
      {
        /* convert EG attenuation level */
        if (SLOT->ssgn ^ (SLOT->ssg&0x04))
          SLOT->volume = (0x200 - SLOT->volume);

        /* force EG attenuation level */
        if (SLOT->volume >= 0x200)
        {
          SLOT->volume = MAX_ATT_INDEX;
          SLOT->state  = EG_OFF;
        }

        /* recalculate EG output */
        SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
      }
    }
  }
}

/* CSM Key Controll */
INLINE void CSMKeyControll(FM_CH *CH)
{
  /* all key ON (verified by Nemesis on real hardware) */
  FM_KEYON_CSM(CH,SLOT1);
  FM_KEYON_CSM(CH,SLOT2);
  FM_KEYON_CSM(CH,SLOT3);
  FM_KEYON_CSM(CH,SLOT4);
  ym2612.OPN.SL3.key_csm = 1;
}

INLINE void INTERNAL_TIMER_A()
{
  if (ym2612.OPN.ST.mode & 0x01)
  {
    ym2612.OPN.ST.TAC--;
    if (ym2612.OPN.ST.TAC <= 0)
    {
      /* set status (if enabled) */
      if (ym2612.OPN.ST.mode & 0x04)
        ym2612.OPN.ST.status |= 0x01;

      /* reload the counter */
      ym2612.OPN.ST.TAC = ym2612.OPN.ST.TAL;

      /* CSM mode auto key on */
      if ((ym2612.OPN.ST.mode & 0xC0) == 0x80)
        CSMKeyControll(&ym2612.CH[2]);
    }
  }
}

INLINE void INTERNAL_TIMER_B(int step)
{
  if (ym2612.OPN.ST.mode & 0x02)
  {
    ym2612.OPN.ST.TBC-=step;
    if (ym2612.OPN.ST.TBC <= 0)
    {
      /* set status (if enabled) */
      if (ym2612.OPN.ST.mode & 0x08)
        ym2612.OPN.ST.status |= 0x02;

      /* reload the counter */
      if (ym2612.OPN.ST.TBL)
        ym2612.OPN.ST.TBC += ym2612.OPN.ST.TBL;
      else
        ym2612.OPN.ST.TBC = ym2612.OPN.ST.TBL;
    }
  }
}

/* OPN Mode Register Write */
INLINE void set_timers(int v )
{
  /* b7 = CSM MODE */
  /* b6 = 3 slot mode */
  /* b5 = reset b */
  /* b4 = reset a */
  /* b3 = timer enable b */
  /* b2 = timer enable a */
  /* b1 = load b */
  /* b0 = load a */

  if ((ym2612.OPN.ST.mode ^ v) & 0xC0)
  {
    /* phase increment need to be recalculated */
	ym2612.CH[2].SLOT[SLOT1].Incr=-1;

    /* CSM mode disabled and CSM key ON active*/
    if (((v & 0xC0) != 0x80) && ym2612.OPN.SL3.key_csm)
    {
      /* CSM Mode Key OFF (verified by Nemesis on real hardware) */
      FM_KEYOFF_CSM(&ym2612.CH[2],SLOT1);
      FM_KEYOFF_CSM(&ym2612.CH[2],SLOT2);
      FM_KEYOFF_CSM(&ym2612.CH[2],SLOT3);
      FM_KEYOFF_CSM(&ym2612.CH[2],SLOT4);
      ym2612.OPN.SL3.key_csm = 0;
    }
  }

  /* reload Timers */
  if ((v&1) && !(ym2612.OPN.ST.mode&1))
    ym2612.OPN.ST.TAC = ym2612.OPN.ST.TAL;
  if ((v&2) && !(ym2612.OPN.ST.mode&2))
    ym2612.OPN.ST.TBC = ym2612.OPN.ST.TBL;

  /* reset Timers flags */
  ym2612.OPN.ST.status &= (~v >> 4);

  ym2612.OPN.ST.mode = v;
}

/* set algorithm connection */
INLINE void setup_connection( FM_CH *CH, int ch )
{
  INT32 *carrier = &out_fm[ch];

  INT32 **om1 = &CH->connect1;
  INT32 **om2 = &CH->connect3;
  INT32 **oc1 = &CH->connect2;

  INT32 **memc = &CH->mem_connect;

  switch( CH->ALGO ){
    case 0:
      /* M1---C1---MEM---M2---C2---OUT */
      *om1 = &c1;
      *oc1 = &mem;
      *om2 = &c2;
      *memc= &m2;
      break;
    case 1:
      /* M1------+-MEM---M2---C2---OUT */
      /*      C1-+                     */
      *om1 = &mem;
      *oc1 = &mem;
      *om2 = &c2;
      *memc= &m2;
      break;
    case 2:
      /* M1-----------------+-C2---OUT */
      /*      C1---MEM---M2-+          */
      *om1 = &c2;
      *oc1 = &mem;
      *om2 = &c2;
      *memc= &m2;
      break;
    case 3:
      /* M1---C1---MEM------+-C2---OUT */
      /*                 M2-+          */
      *om1 = &c1;
      *oc1 = &mem;
      *om2 = &c2;
      *memc= &c2;
      break;
    case 4:
      /* M1---C1-+-OUT */
      /* M2---C2-+     */
      /* MEM: not used */
      *om1 = &c1;
      *oc1 = carrier;
      *om2 = &c2;
      *memc= &mem;  /* store it anywhere where it will not be used */
      break;
    case 5:
      /*    +----C1----+     */
      /* M1-+-MEM---M2-+-OUT */
      /*    +----C2----+     */
      *om1 = 0;  /* special mark */
      *oc1 = carrier;
      *om2 = carrier;
      *memc= &m2;
      break;
    case 6:
      /* M1---C1-+     */
      /*      M2-+-OUT */
      /*      C2-+     */
      /* MEM: not used */
      *om1 = &c1;
      *oc1 = carrier;
      *om2 = carrier;
      *memc= &mem;  /* store it anywhere where it will not be used */
      break;
    case 7:
      /* M1-+     */
      /* C1-+-OUT */
      /* M2-+     */
      /* C2-+     */
      /* MEM: not used*/
      *om1 = carrier;
      *oc1 = carrier;
      *om2 = carrier;
      *memc= &mem;  /* store it anywhere where it will not be used */
      break;
  }

  CH->connect4 = carrier;
}

/* set detune & multiple */
INLINE void set_det_mul(FM_CH *CH,FM_SLOT *SLOT,int v)
{
  SLOT->mul = (v&0x0f)? (v&0x0f)*2 : 1;
  SLOT->DT  = ym2612.OPN.ST.dt_tab[(v>>4)&7];
  CH->SLOT[SLOT1].Incr=-1;
}

/* set total level */
INLINE void set_tl(FM_SLOT *SLOT , int v)
{
  SLOT->tl = (v&0x7f)<<(ENV_BITS-7); /* 7bit TL */

  /* recalculate EG output */
  if ((SLOT->ssg&0x08) && (SLOT->ssgn ^ (SLOT->ssg&0x04)) && (SLOT->state > EG_REL))
    SLOT->vol_out = ((UINT32)(0x200 - SLOT->volume) & MAX_ATT_INDEX) + SLOT->tl;
  else
    SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
}

/* set attack rate & key scale  */
INLINE void set_ar_ksr(FM_CH *CH,FM_SLOT *SLOT,int v)
{
  UINT8 old_KSR = SLOT->KSR;

  SLOT->ar = (v&0x1f) ? 32 + ((v&0x1f)<<1) : 0;

  SLOT->KSR = 3-(v>>6);
  if (SLOT->KSR != old_KSR)
  {
    CH->SLOT[SLOT1].Incr=-1;
  }

  /* Even if it seems unnecessary to do it here, it could happen that KSR and KC  */
  /* are modified but the resulted SLOT->ksr value (kc >> SLOT->KSR) remains unchanged. */
  /* In such case, Attack Rate would not be recalculated by "refresh_fc_eg_slot". */
  /* This actually fixes the intro of "The Adventures of Batman & Robin" (Eke-Eke)         */
  if ((SLOT->ar + SLOT->ksr) < (32+62))
  {
    SLOT->eg_sh_ar  = eg_rate_shift [SLOT->ar  + SLOT->ksr ];
    SLOT->eg_sel_ar = eg_rate_select[SLOT->ar  + SLOT->ksr ];
  }
  else
  {
    /* verified by Nemesis on real hardware (Attack phase is blocked) */
    SLOT->eg_sh_ar  = 0;
    SLOT->eg_sel_ar = 18*RATE_STEPS;
  }
 }

/* set decay rate */
INLINE void set_dr(FM_SLOT *SLOT,int v)
{
  SLOT->d1r = (v&0x1f) ? 32 + ((v&0x1f)<<1) : 0;

  SLOT->eg_sh_d1r = eg_rate_shift [SLOT->d1r + SLOT->ksr];
  SLOT->eg_sel_d1r= eg_rate_select[SLOT->d1r + SLOT->ksr];

}

/* set sustain rate */
INLINE void set_sr(FM_SLOT *SLOT,int v)
{
  SLOT->d2r = (v&0x1f) ? 32 + ((v&0x1f)<<1) : 0;

  SLOT->eg_sh_d2r = eg_rate_shift [SLOT->d2r + SLOT->ksr];
  SLOT->eg_sel_d2r= eg_rate_select[SLOT->d2r + SLOT->ksr];
}

/* set release rate */
INLINE void set_sl_rr(FM_SLOT *SLOT,int v)
{
  SLOT->sl = sl_table[ v>>4 ];

  /* check EG state changes */
  if ((SLOT->state == EG_DEC) && (SLOT->volume >= (INT32)(SLOT->sl)))
    SLOT->state = EG_SUS;

  SLOT->rr  = 34 + ((v&0x0f)<<2);

  SLOT->eg_sh_rr  = eg_rate_shift [SLOT->rr  + SLOT->ksr];
  SLOT->eg_sel_rr = eg_rate_select[SLOT->rr  + SLOT->ksr];
}

/* advance LFO to next sample */
INLINE void advance_lfo()
{
  if (ym2612.OPN.lfo_timer_overflow)   /* LFO enabled ? */
  {
    /* increment LFO timer (every samples) */
    ym2612.OPN.lfo_timer ++;

    /* when LFO is enabled, one level will last for 108, 77, 71, 67, 62, 44, 8 or 5 samples */
    if (ym2612.OPN.lfo_timer >= ym2612.OPN.lfo_timer_overflow)
    {
      ym2612.OPN.lfo_timer = 0;

      /* There are 128 LFO steps */
      ym2612.OPN.lfo_cnt = ( ym2612.OPN.lfo_cnt + 1 ) & 127;

      /* triangle (inverted) */
      /* AM: from 126 to 0 step -2, 0 to 126 step +2 */
      if (ym2612.OPN.lfo_cnt<64)
        ym2612.OPN.LFO_AM = (ym2612.OPN.lfo_cnt ^ 63) << 1;
      else
        ym2612.OPN.LFO_AM = (ym2612.OPN.lfo_cnt & 63) << 1;

      /* PM works with 4 times slower clock */
      ym2612.OPN.LFO_PM = ym2612.OPN.lfo_cnt >> 2;
    }
  }
}


INLINE void advance_eg_channels(FM_CH *CH, unsigned int eg_cnt)
{
  unsigned int i = 6; /* six channels */
  unsigned int j;
  FM_SLOT *SLOT;

  do
  {
    SLOT = &CH->SLOT[SLOT1];
    j = 4; /* four operators per channel */
    do
    {
      switch(SLOT->state)
      {
        case EG_ATT:    /* attack phase */
        {
          if (!(eg_cnt & ((1<<SLOT->eg_sh_ar)-1)))
          {
            /* update attenuation level */
            SLOT->volume += (~SLOT->volume * (eg_inc[SLOT->eg_sel_ar + ((eg_cnt>>SLOT->eg_sh_ar)&7)]))>>4;

            /* check phase transition*/
            if (SLOT->volume <= MIN_ATT_INDEX)
            {
              SLOT->volume = MIN_ATT_INDEX;
              SLOT->state = (SLOT->sl == MIN_ATT_INDEX) ? EG_SUS : EG_DEC; /* special case where SL=0 */
            }

            /* recalculate EG output */
            if ((SLOT->ssg&0x08) && (SLOT->ssgn ^ (SLOT->ssg&0x04)))  /* SSG-EG Output Inversion */
              SLOT->vol_out = ((UINT32)(0x200 - SLOT->volume) & MAX_ATT_INDEX) + SLOT->tl;
            else
              SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
          }
          break;
        }

        case EG_DEC:  /* decay phase */
        {
          if (!(eg_cnt & ((1<<SLOT->eg_sh_d1r)-1)))
          {
            /* SSG EG type */
            if (SLOT->ssg&0x08)
            {
              /* update attenuation level */
              if (SLOT->volume < 0x200)
              {
                SLOT->volume += 4 * eg_inc[SLOT->eg_sel_d1r + ((eg_cnt>>SLOT->eg_sh_d1r)&7)];

                /* recalculate EG output */
                if (SLOT->ssgn ^ (SLOT->ssg&0x04))   /* SSG-EG Output Inversion */
                  SLOT->vol_out = ((UINT32)(0x200 - SLOT->volume) & MAX_ATT_INDEX) + SLOT->tl;
                else
                  SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
              }
            }
            else
            {
              /* update attenuation level */
              SLOT->volume += eg_inc[SLOT->eg_sel_d1r + ((eg_cnt>>SLOT->eg_sh_d1r)&7)];

              /* recalculate EG output */
              SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
            }

            /* check phase transition*/
            if (SLOT->volume >= (INT32)(SLOT->sl))
              SLOT->state = EG_SUS;
          }
          break;
        }

        case EG_SUS:  /* sustain phase */
        {
          if (!(eg_cnt & ((1<<SLOT->eg_sh_d2r)-1)))
          {
            /* SSG EG type */
            if (SLOT->ssg&0x08)
            {
              /* update attenuation level */
              if (SLOT->volume < 0x200)
              {
                SLOT->volume += 4 * eg_inc[SLOT->eg_sel_d2r + ((eg_cnt>>SLOT->eg_sh_d2r)&7)];

                /* recalculate EG output */
                if (SLOT->ssgn ^ (SLOT->ssg&0x04))   /* SSG-EG Output Inversion */
                  SLOT->vol_out = ((UINT32)(0x200 - SLOT->volume) & MAX_ATT_INDEX) + SLOT->tl;
                else
                  SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
              }
            }
            else
            {
              /* update attenuation level */
              SLOT->volume += eg_inc[SLOT->eg_sel_d2r + ((eg_cnt>>SLOT->eg_sh_d2r)&7)];

              /* check phase transition*/
              if ( SLOT->volume >= MAX_ATT_INDEX )
                SLOT->volume = MAX_ATT_INDEX;
                /* do not change SLOT->state (verified on real chip) */

              /* recalculate EG output */
              SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
            }
          }
          break;
        }

        case EG_REL:  /* release phase */
        {
         if (!(eg_cnt & ((1<<SLOT->eg_sh_rr)-1)))
          {
             /* SSG EG type */
            if (SLOT->ssg&0x08)
            {
              /* update attenuation level */
              if (SLOT->volume < 0x200)
                SLOT->volume += 4 * eg_inc[SLOT->eg_sel_rr + ((eg_cnt>>SLOT->eg_sh_rr)&7)];

              /* check phase transition */
              if (SLOT->volume >= 0x200)
              {
                SLOT->volume = MAX_ATT_INDEX;
                SLOT->state = EG_OFF;
              }
            }
            else
            {
              /* update attenuation level */
              SLOT->volume += eg_inc[SLOT->eg_sel_rr + ((eg_cnt>>SLOT->eg_sh_rr)&7)];

              /* check phase transition*/
              if (SLOT->volume >= MAX_ATT_INDEX)
              {
                SLOT->volume = MAX_ATT_INDEX;
                SLOT->state = EG_OFF;
              }
            }

            /* recalculate EG output */
            SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;

          }
          break;
        }
      }

      /* next slot */
      SLOT++;
    } while (--j);

    /* next channel */
    CH++;
  } while (--i);
}

/* SSG-EG update process */
/* The behavior is based upon Nemesis tests on real hardware */
/* This is actually executed before each samples */
INLINE void update_ssg_eg_channels(FM_CH *CH)
{
  unsigned int i = 6; /* six channels */
  unsigned int j;
  FM_SLOT *SLOT;

  do
  {
    j = 4; /* four operators per channel */
    SLOT = &CH->SLOT[SLOT1];

    do
    {
      /* detect SSG-EG transition */
      /* this is not required during release phase as the attenuation has been forced to MAX and output invert flag is not used */
      /* if an Attack Phase is programmed, inversion can occur on each sample */
      if ((SLOT->ssg & 0x08) && (SLOT->volume >= 0x200) && (SLOT->state > EG_REL))
      {
        if (SLOT->ssg & 0x01)  /* bit 0 = hold SSG-EG */
        {
          /* set inversion flag */
          if (SLOT->ssg & 0x02)
            SLOT->ssgn = 4;

          /* force attenuation level during decay phases */
          if ((SLOT->state != EG_ATT) && !(SLOT->ssgn ^ (SLOT->ssg & 0x04)))
            SLOT->volume  = MAX_ATT_INDEX;
        }
        else  /* loop SSG-EG */
        {
          /* toggle output inversion flag or reset Phase Generator */
          if (SLOT->ssg & 0x02)
            SLOT->ssgn ^= 4;
          else
            SLOT->phase = 0;

          /* same as Key ON */
          if (SLOT->state != EG_ATT)
          {
            if ((SLOT->ar + SLOT->ksr) < 94 /*32+62*/)
            {
              SLOT->state = (SLOT->volume <= MIN_ATT_INDEX) ? ((SLOT->sl == MIN_ATT_INDEX) ? EG_SUS : EG_DEC) : EG_ATT;
            }
            else
            {
              /* Attack Rate is maximal: directly switch to Decay or Substain */
              SLOT->volume = MIN_ATT_INDEX;
              SLOT->state = (SLOT->sl == MIN_ATT_INDEX) ? EG_SUS : EG_DEC;
            }
          }
        }

        /* recalculate EG output */
        if (SLOT->ssgn ^ (SLOT->ssg&0x04))
          SLOT->vol_out = ((UINT32)(0x200 - SLOT->volume) & MAX_ATT_INDEX) + SLOT->tl;
        else
          SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
      }

      /* next slot */
      SLOT++;
    } while (--j);

    /* next channel */
    CH++;
  } while (--i);
}

INLINE void update_phase_lfo_slot(FM_SLOT *SLOT, INT32 pms, UINT32 block_fnum)
{
  INT32 lfo_fn_table_index_offset = lfo_pm_table[(((block_fnum & 0x7f0) >> 4) << 8) + pms + ym2612.OPN.LFO_PM];

  if (lfo_fn_table_index_offset)  /* LFO phase modulation active */
  {
    UINT8 blk;
    unsigned int kc, fc;

    /* there are 2048 FNUMs that can be generated using FNUM/BLK registers
          but LFO works with one more bit of a precision so we really need 4096 elements */
    block_fnum = block_fnum*2 + lfo_fn_table_index_offset;
    blk = (block_fnum&0x7000) >> 12;
    block_fnum = block_fnum & 0xfff;

    /* keyscale code */
    kc = (blk<<2) | opn_fktable[block_fnum >> 8];

    /* (frequency) phase increment counter */
    fc = (((block_fnum << 5) >> (7 - blk)) + SLOT->DT[kc]) & DT_MASK;

    /* update phase */
    SLOT->phase += (fc * SLOT->mul) >> 1;
  }
  else  /* LFO phase modulation  = zero */
  {
    SLOT->phase += SLOT->Incr;
  }
}

INLINE void update_phase_lfo_channel(FM_CH *CH)
{
  UINT32 block_fnum = CH->block_fnum;

  INT32 lfo_fn_table_index_offset = lfo_pm_table[(((block_fnum & 0x7f0) >> 4) << 8) + CH->pms + ym2612.OPN.LFO_PM];

  if (lfo_fn_table_index_offset)  /* LFO phase modulation active */
  {
    UINT8 blk;
    unsigned int kc, fc, finc;

    /* there are 2048 FNUMs that can be generated using FNUM/BLK registers
          but LFO works with one more bit of a precision so we really need 4096 elements */
    block_fnum = block_fnum*2 + lfo_fn_table_index_offset;
    blk = (block_fnum&0x7000) >> 12;
    block_fnum = block_fnum & 0xfff;

    /* keyscale code */
    kc = (blk<<2) | opn_fktable[block_fnum >> 8];

    /* (frequency) phase increment counter */
    fc = (block_fnum << 5) >> (7 - blk);

    /* apply DETUNE & MUL operator specific values */
    finc = (fc + CH->SLOT[SLOT1].DT[kc]) & DT_MASK;
    CH->SLOT[SLOT1].phase += (finc*CH->SLOT[SLOT1].mul) >> 1;

    finc = (fc + CH->SLOT[SLOT2].DT[kc]) & DT_MASK;
    CH->SLOT[SLOT2].phase += (finc*CH->SLOT[SLOT2].mul) >> 1;

    finc = (fc + CH->SLOT[SLOT3].DT[kc]) & DT_MASK;
    CH->SLOT[SLOT3].phase += (finc*CH->SLOT[SLOT3].mul) >> 1;

    finc = (fc + CH->SLOT[SLOT4].DT[kc]) & DT_MASK;
    CH->SLOT[SLOT4].phase += (finc*CH->SLOT[SLOT4].mul) >> 1;
  }
  else  /* LFO phase modulation  = zero */
  {
    CH->SLOT[SLOT1].phase += CH->SLOT[SLOT1].Incr;
    CH->SLOT[SLOT2].phase += CH->SLOT[SLOT2].Incr;
    CH->SLOT[SLOT3].phase += CH->SLOT[SLOT3].Incr;
    CH->SLOT[SLOT4].phase += CH->SLOT[SLOT4].Incr;
  }
}

/* update phase increment and envelope generator */
INLINE void refresh_fc_eg_slot(FM_SLOT *SLOT , UINT32 fc , UINT8 kc )
{
  /* add detune value */
  fc += SLOT->DT[kc];

  /* (frequency) phase overflow (credits to Nemesis) */
  fc &= DT_MASK;

  /* (frequency) phase increment counter */
  SLOT->Incr = (fc * SLOT->mul) >> 1;

  /* ksr */
  kc = kc >> SLOT->KSR;

  if( SLOT->ksr != kc )
  {
    SLOT->ksr = kc;

    /* recalculate envelope generator rates */
    if ((SLOT->ar + kc) < (32+62))
    {
      SLOT->eg_sh_ar  = eg_rate_shift [SLOT->ar  + kc ];
      SLOT->eg_sel_ar = eg_rate_select[SLOT->ar  + kc ];
    }
    else
    {
      /* verified by Nemesis on real hardware (Attack phase is blocked) */
      SLOT->eg_sh_ar  = 0;
      SLOT->eg_sel_ar = 18*RATE_STEPS;
    }

    SLOT->eg_sh_d1r = eg_rate_shift [SLOT->d1r + kc];
    SLOT->eg_sel_d1r= eg_rate_select[SLOT->d1r + kc];

    SLOT->eg_sh_d2r = eg_rate_shift [SLOT->d2r + kc];
    SLOT->eg_sel_d2r= eg_rate_select[SLOT->d2r + kc];

    SLOT->eg_sh_rr  = eg_rate_shift [SLOT->rr  + kc];
    SLOT->eg_sel_rr = eg_rate_select[SLOT->rr  + kc];
  }
}

/* update phase increment counters */
INLINE void refresh_fc_eg_chan(FM_CH *CH )
{
  if( CH->SLOT[SLOT1].Incr==-1)
  {
    int fc = CH->fc;
    int kc = CH->kcode;
    refresh_fc_eg_slot(&CH->SLOT[SLOT1] , fc , kc );
    refresh_fc_eg_slot(&CH->SLOT[SLOT2] , fc , kc );
    refresh_fc_eg_slot(&CH->SLOT[SLOT3] , fc , kc );
    refresh_fc_eg_slot(&CH->SLOT[SLOT4] , fc , kc );
  }
}

#define volume_calc(OP) ((OP)->vol_out + (AM & (OP)->AMmask))

INLINE signed int op_calc(UINT32 phase, unsigned int env, unsigned int pm)
{
  UINT32 p = (env<<3) + sin_tab[ ( (phase >> SIN_BITS) + (pm >> 1) ) & SIN_MASK ];

  if (p >= TL_TAB_LEN)
    return 0;
  return tl_tab[p];
}

INLINE signed int op_calc1(UINT32 phase, unsigned int env, unsigned int pm)
{
  UINT32 p = (env<<3) + sin_tab[ ( (phase + pm ) >> SIN_BITS ) & SIN_MASK ];

  if (p >= TL_TAB_LEN)
    return 0;
  return tl_tab[p];
}

INLINE void chan_calc(FM_CH *CH, int num)
{
  do
  {
    UINT32 AM = ym2612.OPN.LFO_AM >> CH->ams;
    unsigned int eg_out = volume_calc(&CH->SLOT[SLOT1]);

    m2 = c1 = c2 = mem = 0;

    *CH->mem_connect = CH->mem_value;  /* restore delayed sample (MEM) value to m2 or c2 */
    {
      INT32 out = CH->op1_out[0] + CH->op1_out[1];
      CH->op1_out[0] = CH->op1_out[1];

      if( !CH->connect1 ){
        /* algorithm 5  */
        mem = c1 = c2 = CH->op1_out[0];
      }else{
        /* other algorithms */
        *CH->connect1 += CH->op1_out[0];
      }

      CH->op1_out[1] = 0;
      if( eg_out < ENV_QUIET )  /* SLOT 1 */
      {
        if (!CH->FB)
          out=0;

        CH->op1_out[1] = op_calc1(CH->SLOT[SLOT1].phase, eg_out, (out<<CH->FB) );
      }
    }

    eg_out = volume_calc(&CH->SLOT[SLOT3]);
    if( eg_out < ENV_QUIET )    /* SLOT 3 */
      *CH->connect3 += op_calc(CH->SLOT[SLOT3].phase, eg_out, m2);

    eg_out = volume_calc(&CH->SLOT[SLOT2]);
    if( eg_out < ENV_QUIET )    /* SLOT 2 */
    *CH->connect2 += op_calc(CH->SLOT[SLOT2].phase, eg_out, c1);

    eg_out = volume_calc(&CH->SLOT[SLOT4]);
    if( eg_out < ENV_QUIET )    /* SLOT 4 */
      *CH->connect4 += op_calc(CH->SLOT[SLOT4].phase, eg_out, c2);


    /* store current MEM */
    CH->mem_value = mem;

    /* update phase counters AFTER output calculations */
    if(CH->pms)
    {
      /* add support for 3 slot mode */
      if ((ym2612.OPN.ST.mode & 0xC0) && (CH == &ym2612.CH[2]))
      {
        update_phase_lfo_slot(&CH->SLOT[SLOT1], CH->pms, ym2612.OPN.SL3.block_fnum[1]);
        update_phase_lfo_slot(&CH->SLOT[SLOT2], CH->pms, ym2612.OPN.SL3.block_fnum[2]);
        update_phase_lfo_slot(&CH->SLOT[SLOT3], CH->pms, ym2612.OPN.SL3.block_fnum[0]);
        update_phase_lfo_slot(&CH->SLOT[SLOT4], CH->pms, CH->block_fnum);
      }
      else
      {
        update_phase_lfo_channel(CH);
      }
    }
    else  /* no LFO phase modulation */
    {
      CH->SLOT[SLOT1].phase += CH->SLOT[SLOT1].Incr;
      CH->SLOT[SLOT2].phase += CH->SLOT[SLOT2].Incr;
      CH->SLOT[SLOT3].phase += CH->SLOT[SLOT3].Incr;
      CH->SLOT[SLOT4].phase += CH->SLOT[SLOT4].Incr;
    }

    /* next channel */
    CH++;
  } while (--num);
}

/* write a OPN mode register 0x20-0x2f */
INLINE void OPNWriteMode(int r, int v)
{
  UINT8 c;
  FM_CH *CH;

  switch(r){
    case 0x21:  /* Test */
      break;

    case 0x22:  /* LFO FREQ (YM2608/YM2610/YM2610B/ym2612) */
      if (v&8) /* LFO enabled ? */
      {
        ym2612.OPN.lfo_timer_overflow = lfo_samples_per_step[v&7];
      }
      else
      {
        /* hold LFO waveform in reset state */
        ym2612.OPN.lfo_timer_overflow = 0;
        ym2612.OPN.lfo_timer = 0;
        ym2612.OPN.lfo_cnt = 0;
        ym2612.OPN.LFO_PM = 0;
        ym2612.OPN.LFO_AM = 126;
      }
      break;
    case 0x24:  /* timer A High 8*/
      ym2612.OPN.ST.TA = (ym2612.OPN.ST.TA & 0x03)|(((int)v)<<2);
      ym2612.OPN.ST.TAL = 1024 - ym2612.OPN.ST.TA;
      break;
    case 0x25:  /* timer A Low 2*/
      ym2612.OPN.ST.TA = (ym2612.OPN.ST.TA & 0x3fc)|(v&3);
      ym2612.OPN.ST.TAL = 1024 - ym2612.OPN.ST.TA;
      break;
    case 0x26:  /* timer B */
      ym2612.OPN.ST.TB = v;
      ym2612.OPN.ST.TBL = (256 - v) << 4;
      break;
    case 0x27:  /* mode, timer control */
      set_timers(v);
      break;
    case 0x28:  /* key on / off */
      c = v & 0x03;
      if( c == 3 ) break;
      if (v&0x04) c+=3; /* CH 4-6 */
      CH = &ym2612.CH[c];

      if (v&0x10) FM_KEYON(CH,SLOT1); else FM_KEYOFF(CH,SLOT1);
      if (v&0x20) FM_KEYON(CH,SLOT2); else FM_KEYOFF(CH,SLOT2);
      if (v&0x40) FM_KEYON(CH,SLOT3); else FM_KEYOFF(CH,SLOT3);
      if (v&0x80) FM_KEYON(CH,SLOT4); else FM_KEYOFF(CH,SLOT4);
      break;
  }
}

/* write a OPN register (0x30-0xff) */
INLINE void OPNWriteReg(int r, int v)
{
  FM_CH *CH;
  FM_SLOT *SLOT;

  UINT8 c = OPN_CHAN(r);

  if (c == 3) return; /* 0xX3,0xX7,0xXB,0xXF */

  if (r >= 0x100) c+=3;

  CH = &ym2612.CH[c];

  SLOT = &(CH->SLOT[OPN_SLOT(r)]);

  switch( r & 0xf0 ) {
    case 0x30:  /* DET , MUL */
      set_det_mul(CH,SLOT,v);
      break;

    case 0x40:  /* TL */
      set_tl(SLOT,v);
      break;

    case 0x50:  /* KS, AR */
      set_ar_ksr(CH,SLOT,v);
      break;

    case 0x60:  /* bit7 = AM ENABLE, DR */
      set_dr(SLOT,v);
      SLOT->AMmask = (v&0x80) ? ~0 : 0;
      break;

    case 0x70:  /*     SR */
      set_sr(SLOT,v);
      break;

    case 0x80:  /* SL, RR */
      set_sl_rr(SLOT,v);
      break;

    case 0x90:  /* SSG-EG */
      SLOT->ssg  = v&0x0f;

      /* recalculate EG output */
      if (SLOT->state > EG_REL)
      {
        if ((SLOT->ssg&0x08) && (SLOT->ssgn ^ (SLOT->ssg&0x04)))
          SLOT->vol_out = ((UINT32)(0x200 - SLOT->volume) & MAX_ATT_INDEX) + SLOT->tl;
        else
          SLOT->vol_out = (UINT32)SLOT->volume + SLOT->tl;
      }

      /* SSG-EG envelope shapes :

      E AtAlH
      1 0 0 0  \\\\

      1 0 0 1  \___

      1 0 1 0  \/\/
                ___
      1 0 1 1  \

      1 1 0 0  ////
                ___
      1 1 0 1  /

      1 1 1 0  /\/\

      1 1 1 1  /___


      E = SSG-EG enable


      The shapes are generated using Attack, Decay and Sustain phases.

      Each single character in the diagrams above represents this whole
      sequence:

      - when KEY-ON = 1, normal Attack phase is generated (*without* any
        difference when compared to normal mode),

      - later, when envelope level reaches minimum level (max volume),
        the EG switches to Decay phase (which works with bigger steps
        when compared to normal mode - see below),

      - later when envelope level passes the SL level,
        the EG swithes to Sustain phase (which works with bigger steps
        when compared to normal mode - see below),

      - finally when envelope level reaches maximum level (min volume),
        the EG switches to Attack phase again (depends on actual waveform).

      Important is that when switch to Attack phase occurs, the phase counter
      of that operator will be zeroed-out (as in normal KEY-ON) but not always.
      (I havent found the rule for that - perhaps only when the output level is low)

      The difference (when compared to normal Envelope Generator mode) is
      that the resolution in Decay and Sustain phases is 4 times lower;
      this results in only 256 steps instead of normal 1024.
      In other words:
      when SSG-EG is disabled, the step inside of the EG is one,
      when SSG-EG is enabled, the step is four (in Decay and Sustain phases).

      Times between the level changes are the same in both modes.


      Important:
      Decay 1 Level (so called SL) is compared to actual SSG-EG output, so
      it is the same in both SSG and no-SSG modes, with this exception:

      when the SSG-EG is enabled and is generating raising levels
      (when the EG output is inverted) the SL will be found at wrong level !!!
      For example, when SL=02:
        0 -6 = -6dB in non-inverted EG output
        96-6 = -90dB in inverted EG output
      Which means that EG compares its level to SL as usual, and that the
      output is simply inverted afterall.


      The Yamaha's manuals say that AR should be set to 0x1f (max speed).
      That is not necessary, but then EG will be generating Attack phase.

      */


      break;

    case 0xa0:
      switch( OPN_SLOT(r) ){
        case 0:    /* 0xa0-0xa2 : FNUM1 */
        {
          UINT32 fn = (((UINT32)((ym2612.OPN.ST.fn_h)&7))<<8) + v;
          UINT8 blk = ym2612.OPN.ST.fn_h>>3;
          /* keyscale code */
          CH->kcode = (blk<<2) | opn_fktable[fn >> 7];
          /* phase increment counter */
          CH->fc = (fn << 6) >> (7 - blk);

          /* store fnum in clear form for LFO PM calculations */
          CH->block_fnum = (blk<<11) | fn;

          CH->SLOT[SLOT1].Incr=-1;
          break;
        }
        case 1:    /* 0xa4-0xa6 : FNUM2,BLK */
          ym2612.OPN.ST.fn_h = v&0x3f;
          break;
        case 2:    /* 0xa8-0xaa : 3CH FNUM1 */
          if(r < 0x100)
          {
            UINT32 fn = (((UINT32)(ym2612.OPN.SL3.fn_h&7))<<8) + v;
            UINT8 blk = ym2612.OPN.SL3.fn_h>>3;
            /* keyscale code */
            ym2612.OPN.SL3.kcode[c]= (blk<<2) | opn_fktable[fn >> 7];
            /* phase increment counter */
            ym2612.OPN.SL3.fc[c] = (fn << 6) >> (7 - blk);
            ym2612.OPN.SL3.block_fnum[c] = (blk<<11) | fn;
            ym2612.CH[2].SLOT[SLOT1].Incr=-1;
          }
          break;
        case 3:    /* 0xac-0xae : 3CH FNUM2,BLK */
          if(r < 0x100)
            ym2612.OPN.SL3.fn_h = v&0x3f;
          break;
      }
      break;

    case 0xb0:
      switch( OPN_SLOT(r) ){
        case 0:    /* 0xb0-0xb2 : FB,ALGO */
        {
          CH->ALGO = v&7;
          CH->FB   = (v>>3)&7;
          setup_connection( CH, c );
          break;
        }
        case 1:    /* 0xb4-0xb6 : L , R , AMS , PMS */
          /* b0-2 PMS */
          CH->pms = (v & 7) * 32; /* CH->pms = PM depth * 32 (index in lfo_pm_table) */

          /* b4-5 AMS */
          CH->ams = lfo_ams_depth_shift[(v>>4) & 0x03];

          /* PAN :  b7 = L, b6 = R */
          ym2612.OPN.pan[ c*2   ] = (v & 0x80) ? bitmask : 0;
          ym2612.OPN.pan[ c*2+1 ] = (v & 0x40) ? bitmask : 0;
          break;
      }
      break;
  }
}

static void reset_channels(FM_CH *CH , int num )
{
  int c,s;

  for( c = 0 ; c < num ; c++ )
  {
    CH[c].mem_value   = 0;
    CH[c].op1_out[0]  = 0;
    CH[c].op1_out[1]  = 0;
    for(s = 0 ; s < 4 ; s++ )
    {
      CH[c].SLOT[s].Incr    = -1;
      CH[c].SLOT[s].key     = 0;
      CH[c].SLOT[s].phase   = 0;
      CH[c].SLOT[s].ssgn    = 0;
      CH[c].SLOT[s].state   = EG_OFF;
      CH[c].SLOT[s].volume  = MAX_ATT_INDEX;
      CH[c].SLOT[s].vol_out = MAX_ATT_INDEX;
    }
  }
}

/* initialize generic tables */
static void init_tables(void)
{
  signed int d,i,x;
  signed int n;
  double o,m;

#if 0
  /* build Linear Power Table */
  for (x=0; x<TL_RES_LEN; x++)
  {
    m = (1<<16) / pow(2,(x+1) * (ENV_STEP/4.0) / 8.0);
    m = floor(m);

    /* we never reach (1<<16) here due to the (x+1) */
    /* result fits within 16 bits at maximum */

    n = (int)m; /* 16 bits here */
    n >>= 4;    /* 12 bits here */
    if (n&1)    /* round to nearest */
      n = (n>>1)+1;
    else
      n = n>>1;
                /* 11 bits here (rounded) */
    n <<= 2;    /* 13 bits here (as in real chip) */

    /* 14 bits (with sign bit) */
    tl_tab[ x*2 + 0 ] = n;
    tl_tab[ x*2 + 1 ] = -tl_tab[ x*2 + 0 ];

    /* one entry in the 'Power' table use the following format, xxxxxyyyyyyyys with:            */
    /*        s = sign bit                                                                      */
    /* yyyyyyyy = 8-bits decimal part (0-TL_RES_LEN)                                            */
    /* xxxxx    = 5-bits integer 'shift' value (0-31) but, since Power table output is 13 bits, */
    /*            any value above 13 (included) would be discarded.                             */
    for (i=1; i<13; i++)
    {
      tl_tab[ x*2+0 + i*2*TL_RES_LEN ] =  tl_tab[ x*2+0 ]>>i;
      tl_tab[ x*2+1 + i*2*TL_RES_LEN ] = -tl_tab[ x*2+0 + i*2*TL_RES_LEN ];
    }
  }

  /* build Logarithmic Sinus table */
  for (i=0; i<SIN_LEN; i++)
  {
    /* non-standard sinus */
    m = sin( ((i*2)+1) * M_PI / SIN_LEN ); /* checked against the real chip */
    /* we never reach zero here due to ((i*2)+1) */

    if (m>0.0)
      o = 8*log(1.0/m)/log(2);  /* convert to 'decibels' */
    else
      o = 8*log(-1.0/m)/log(2);  /* convert to 'decibels' */

    o = o / (ENV_STEP/4);

    n = (int)(2.0*o);
    if (n&1)            /* round to nearest */
      n = (n>>1)+1;
    else
      n = n>>1;

    /* 13-bits (8.5) value is formatted for above 'Power' table */
    sin_tab[ i ] = n*2 + (m>=0.0? 0: 1 );
  }

  /* build LFO PM modulation table */
  for(i = 0; i < 8; i++) /* 8 PM depths */
  {
    UINT8 fnum;
    for (fnum=0; fnum<128; fnum++) /* 7 bits meaningful of F-NUMBER */
    {
      UINT8 value;
      UINT8 step;
      UINT32 offset_depth = i;
      UINT32 offset_fnum_bit;
      UINT32 bit_tmp;

      for (step=0; step<8; step++)
      {
        value = 0;
        for (bit_tmp=0; bit_tmp<7; bit_tmp++) /* 7 bits */
        {
          if (fnum & (1<<bit_tmp)) /* only if bit "bit_tmp" is set */
          {
            offset_fnum_bit = bit_tmp * 8;
            value += lfo_pm_output[offset_fnum_bit + offset_depth][step];
          }
        }
        /* 32 steps for LFO PM (sinus) */
        lfo_pm_table[(fnum*32*8) + (i*32) + step   + 0] = value;
        lfo_pm_table[(fnum*32*8) + (i*32) +(step^7)+ 8] = value;
        lfo_pm_table[(fnum*32*8) + (i*32) + step   +16] = -value;
        lfo_pm_table[(fnum*32*8) + (i*32) +(step^7)+24] = -value;
      }
    }
  }
#endif


  /* build DETUNE table */
  for (d = 0;d <= 3;d++)
  {
    for (i = 0;i <= 31;i++)
    {
      ym2612.OPN.ST.dt_tab[d][i]   = (INT32) dt_tab[d*32 + i];
      ym2612.OPN.ST.dt_tab[d+4][i] = -ym2612.OPN.ST.dt_tab[d][i];
    }
  }

}

/* initialize ym2612 emulator */
void YM2612Init(void)
{
  memset(&ym2612,0,sizeof(YM2612));
  init_tables();
}

/* reset OPN registers */
void YM2612ResetChip(void)
{
  int i;

  ym2612.OPN.eg_timer     = 0;
  ym2612.OPN.eg_cnt       = 0;

  ym2612.OPN.lfo_timer_overflow = 0;
  ym2612.OPN.lfo_timer          = 0;
  ym2612.OPN.lfo_cnt            = 0;
  ym2612.OPN.LFO_AM             = 126;
  ym2612.OPN.LFO_PM             = 0;

  ym2612.OPN.ST.TAC       = 0;
  ym2612.OPN.ST.TBC       = 0;

  ym2612.OPN.SL3.key_csm  = 0;

  ym2612.dacen            = 0;
  ym2612.dacout           = 0;

  set_timers(0x30);
  ym2612.OPN.ST.TB = 0;
  ym2612.OPN.ST.TBL = 256 << 4;
  ym2612.OPN.ST.TA = 0;
  ym2612.OPN.ST.TAL = 1024;

  reset_channels(&ym2612.CH[0] , 6 );

  for(i = 0xb6 ; i >= 0xb4 ; i-- )
  {
    OPNWriteReg(i      ,0xc0);
    OPNWriteReg(i|0x100,0xc0);
  }
  for(i = 0xb2 ; i >= 0x30 ; i-- )
  {
    OPNWriteReg(i      ,0);
    OPNWriteReg(i|0x100,0);
  }
}

/* ym2612 write */
/* n = number  */
/* a = address */
/* v = value   */
void YM2612Write(unsigned int a, unsigned int v)
{
  v &= 0xff;  /* adjust to 8 bit bus */

  switch( a )
  {
    case 0:  /* address port 0 */
      ym2612.OPN.ST.address = v;
      break;

    case 2:  /* address port 1 */
      ym2612.OPN.ST.address = v | 0x100;
      break;

    default:  /* data port */
    {
      int addr = ym2612.OPN.ST.address; /* verified by Nemesis on real YM2612 */
      switch( addr & 0x1f0 )
      {
        case 0x20:  /* 0x20-0x2f Mode */
          switch( addr )
          {
            case 0x2a:  /* DAC data (ym2612) */
              ym2612.dacout = ((int)v - 0x80) << 6; /* convert to 14-bit output */
              break;
            case 0x2b:  /* DAC Sel  (ym2612) */
              /* b7 = dac enable */
              ym2612.dacen = v & 0x80;
              break;
            default:  /* OPN section */
              /* write register */
              OPNWriteMode(addr,v);
          }
          break;
        default:  /* 0x30-0xff OPN section */
          /* write register */
          OPNWriteReg(addr,v);
      }
      break;
    }
  }
}

unsigned int YM2612Read(void)
{
  return ym2612.OPN.ST.status & 0xff;
}

/* Generate samples for ym2612 */
void YM2612Update(char *buffer, int length)
{
  int i;
  int lt,rt;

  /* refresh PG increments and EG rates if required */
  refresh_fc_eg_chan(&ym2612.CH[0]);
  refresh_fc_eg_chan(&ym2612.CH[1]);

  if (!(ym2612.OPN.ST.mode & 0xC0))
  {
	refresh_fc_eg_chan(&ym2612.CH[2]);
  }
  else
  {
    /* 3SLOT MODE (operator order is 0,1,3,2) */
	if(ym2612.CH[2].SLOT[SLOT1].Incr==-1)
    {
      refresh_fc_eg_slot(&ym2612.CH[2].SLOT[SLOT1] , ym2612.OPN.SL3.fc[1] , ym2612.OPN.SL3.kcode[1] );
      refresh_fc_eg_slot(&ym2612.CH[2].SLOT[SLOT2] , ym2612.OPN.SL3.fc[2] , ym2612.OPN.SL3.kcode[2] );
      refresh_fc_eg_slot(&ym2612.CH[2].SLOT[SLOT3] , ym2612.OPN.SL3.fc[0] , ym2612.OPN.SL3.kcode[0] );
      refresh_fc_eg_slot(&ym2612.CH[2].SLOT[SLOT4] , ym2612.CH[2].fc , ym2612.CH[2].kcode );
    }
  }

  refresh_fc_eg_chan(&ym2612.CH[3]);
  refresh_fc_eg_chan(&ym2612.CH[4]);
  refresh_fc_eg_chan(&ym2612.CH[5]);

  /* buffering */
  for(i=0; i < length ; i++)
  {
    /* clear outputs */
    out_fm[0] = 0;
    out_fm[1] = 0;
    out_fm[2] = 0;
    out_fm[3] = 0;
    out_fm[4] = 0;
    out_fm[5] = 0;

    /* update SSG-EG output */
    update_ssg_eg_channels(&ym2612.CH[0]);

    /* calculate FM */
    if (!ym2612.dacen)
    {
      chan_calc(&ym2612.CH[0],6);
    }
    else
    {
      /* DAC Mode */
      out_fm[5] = ym2612.dacout;
      chan_calc(&ym2612.CH[0],5);
    }

    /* advance LFO */
    advance_lfo();

    /* advance envelope generator */
    ym2612.OPN.eg_timer ++;

    /* EG is updated every 3 samples */
    if (ym2612.OPN.eg_timer >= 3)
    {
      ym2612.OPN.eg_timer = 0;
      ym2612.OPN.eg_cnt++;
      advance_eg_channels(&ym2612.CH[0], ym2612.OPN.eg_cnt);
    }

    /* 14-bit accumulator channels outputs (range is -8192;+8192) */
    if (out_fm[0] > 8192) out_fm[0] = 8192;
    else if (out_fm[0] < -8192) out_fm[0] = -8192;
    if (out_fm[1] > 8192) out_fm[1] = 8192;
    else if (out_fm[1] < -8192) out_fm[1] = -8192;
    if (out_fm[2] > 8192) out_fm[2] = 8192;
    else if (out_fm[2] < -8192) out_fm[2] = -8192;
    if (out_fm[3] > 8192) out_fm[3] = 8192;
    else if (out_fm[3] < -8192) out_fm[3] = -8192;
    if (out_fm[4] > 8192) out_fm[4] = 8192;
    else if (out_fm[4] < -8192) out_fm[4] = -8192;
    if (out_fm[5] > 8192) out_fm[5] = 8192;
    else if (out_fm[5] < -8192) out_fm[5] = -8192;

    /* stereo DAC channels outputs mixing */
    lt  = ((out_fm[0]) & ym2612.OPN.pan[0]);
    rt  = ((out_fm[0]) & ym2612.OPN.pan[1]);
    lt += ((out_fm[1]) & ym2612.OPN.pan[2]);
    rt += ((out_fm[1]) & ym2612.OPN.pan[3]);
    lt += ((out_fm[2]) & ym2612.OPN.pan[4]);
    rt += ((out_fm[2]) & ym2612.OPN.pan[5]);
    lt += ((out_fm[3]) & ym2612.OPN.pan[6]);
    rt += ((out_fm[3]) & ym2612.OPN.pan[7]);
    lt += ((out_fm[4]) & ym2612.OPN.pan[8]);
    rt += ((out_fm[4]) & ym2612.OPN.pan[9]);
    lt += ((out_fm[5]) & ym2612.OPN.pan[10]);
    rt += ((out_fm[5]) & ym2612.OPN.pan[11]);

    /* buffering */
    *buffer++ = 0;
    *buffer++ = (lt >> 7) + 0x80;
    *buffer++ = 0;
    *buffer++ = (rt >> 7) + 0x80;

    /* CSM mode: if CSM Key ON has occured, CSM Key OFF need to be sent       */
    /* only if Timer A does not overflow again (i.e CSM Key ON not set again) */
    ym2612.OPN.SL3.key_csm <<= 1;

    /* timer A control */
    INTERNAL_TIMER_A();

    /* CSM Mode Key ON still disabled */
    if (ym2612.OPN.SL3.key_csm & 2)
    {
      /* CSM Mode Key OFF (verified by Nemesis on real hardware) */
      FM_KEYOFF_CSM(&ym2612.CH[2],SLOT1);
      FM_KEYOFF_CSM(&ym2612.CH[2],SLOT2);
      FM_KEYOFF_CSM(&ym2612.CH[2],SLOT3);
      FM_KEYOFF_CSM(&ym2612.CH[2],SLOT4);
      ym2612.OPN.SL3.key_csm = 0;
    }
  }

  /* timer B control */
  INTERNAL_TIMER_B(length);
}

void YM2612Config(unsigned char dac_bits)
{
  int i;

  /* DAC precision (normally 9-bit on real hardware, implemented through simple 14-bit channel output bitmasking) */
  bitmask = ~((1 << (TL_BITS - dac_bits)) - 1);

  /* update L/R panning bitmasks */
  for (i=0; i<2*6; i++)
  {
    if (ym2612.OPN.pan[i])
    {
      ym2612.OPN.pan[i] = bitmask;
    }
  }
}

