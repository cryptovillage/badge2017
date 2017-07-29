#include "dac.h"
#include "ym2612.h"
#include "vgm.h"

extern const uint8_t emeraldhill_vgm_start[] asm("_binary_emeraldhill_vgm_start");
extern const uint8_t emeraldhill_vgm_end[]   asm("_binary_emeraldhill_vgm_end");

#define VGM_BUF_SIZE 32
char vgmBuf[VGM_BUF_SIZE * 4];

void genVGMSamples(uint32_t count)
{
	//printf("%d\n", count);
	while (count) {
		if (count > VGM_BUF_SIZE) {
			YM2612Update(vgmBuf, VGM_BUF_SIZE);
			dac_samples(vgmBuf, VGM_BUF_SIZE);
			count -= VGM_BUF_SIZE;
		} else {
			YM2612Update(vgmBuf, count);
			dac_samples(vgmBuf, count);
			return;
		}
	}
}

void playVGM()
{
	const uint8_t *vgmPtr;
	const uint8_t *pcmDataPtr;
	uint32_t tmp;
	YM2612Init();
	YM2612Config(9);
	YM2612ResetChip();
	dac_init();
	//vgmPtr = emeraldhill_vgm_start + *(unsigned int *)(emeraldhill_vgm_start + 0x34) + 0x34;
	vgmPtr = emeraldhill_vgm_start + 0x2f26; // HACK!
	pcmDataPtr = emeraldhill_vgm_start + 0x47;
	for (;;) {
		if ((*vgmPtr & 0xF0) == 0x70) {
			genVGMSamples((*vgmPtr++ & 0xF) + 1);
		} else if ((*vgmPtr & 0xF0) == 0x80) {
			YM2612Write(0, 0x2a);
			YM2612Write(1, *pcmDataPtr++);
			genVGMSamples(*vgmPtr++ & 0xF);
		} else {
			switch (*vgmPtr++) {
			case 0x4f: // PSG
			case 0x50: // PSG
				// Not supported
				vgmPtr++;
				break;
			case 0x52: // YM2612 port 0
				YM2612Write(0, *vgmPtr++);
				YM2612Write(1, *vgmPtr++);
				break;
			case 0x53: // YM2612 port 1
				YM2612Write(2, *vgmPtr++);
				YM2612Write(3, *vgmPtr++);
				break;
			case 0x61:
				tmp = *vgmPtr++;
				tmp |= (*vgmPtr++) << 8;
				genVGMSamples(tmp);
				break;
			case 0x62:
				genVGMSamples(735);
				break;
			case 0x63:
				genVGMSamples(882);
				break;
			case 0x66:
				//printf("LOOPING VGM\n");
				vgmPtr = emeraldhill_vgm_start + 0xb541;
				break;
			case 0xe0:
				tmp = *vgmPtr++;
				tmp |= (*vgmPtr++) << 8;
				tmp |= (*vgmPtr++) << 16;
				tmp |= (*vgmPtr++) << 24;
				pcmDataPtr = emeraldhill_vgm_start + 0x47 + tmp;
				break;
			}
		}
		if (vgmPtr >= emeraldhill_vgm_end) {
			//printf("LOOPING VGM\n");
			vgmPtr = emeraldhill_vgm_start + 0xb541;
		}
	}
}
