#include <SI_EFM8UB1_Register_Enums.h>
#include "efm8_usb.h"
#include "board.h"

uint8_t i2cMBuf[MAX_I2C_PKT_SIZE];
uint8_t i2cSBuf[MAX_I2C_PKT_SIZE];
uint8_t i2cMAddr;
uint8_t i2cMBufPtr;
uint8_t i2cSBufPtr;
uint8_t i2cMBufLen;
uint8_t i2cSBufLen;
uint8_t i2cFlags;

void i2c_enable_slave()
{
	i2cSBufPtr = i2cSBuf;
	i2cSBufLen = sizeof(i2cSBuf);
	// FIXME(supersat): What happens when we inhibit the slave but have EHACK on?
	SFRPAGE = 0;
	SMB0CF &= ~SMB0CF_INH__SLAVE_DISABLED;
}

void i2c_init()
{
	i2cFlags = 0;
	i2cMAddr = 0;
	i2cMBufPtr = 0;
	i2cSBufPtr = 0;
	i2cMBufLen = 0;
	i2cSBufLen = 0;

	i2c_enable_slave();
}

void i2c_update()
{
	uint8_t i;

	if (i2cFlags & I2C_FLAGS_SLAVE_MSG_RECV) {
		if (i2cSBuf[0] == 0xBB) { // BLING BLING
			for (i = 0; i < 32; i++) {
				ledBuf[i] = i2cSBuf[i + 1];
			}
			ledFlags |= LED_FLAG_UPDATE_LEDS;
		} else if (i2cSBuf[0] == 0xDB) { // Display Brightness
			backlightBrightness = i2cSBuf[1];
		} else if (i2cSBuf[0] == 'U') { // Get updates
			// TODO: Fill I2C update buffer
			i2cSBuf[0] = 'V';
			i2cSBuf[1] = i2cEncoderStatus & 0x7F;
			i2cEncoderStatus &= ~ENCODER_DIRTY;
			i2cSBuf[2] = powerFlags;
			i2cSBuf[3] = battVoltage & 0xff;
			i2cSBuf[4] = battVoltage >> 8;
			// TODO(supersat): Copy HID packets
		} else if (i2cSBuf[0] == 'H') { // HID packet
			if (USBD_EpIsBusy(EP3IN)) {
				return; // Don't re-enable I2C slave until we can send the HID msg
			}
			USBD_Write(EP3IN, i2cSBuf + 1, (uint16_t)i2cSBufPtr - (uint16_t)i2cSBuf - 1, false);
		}
		IE_EA = 0;
		i2cFlags &= ~I2C_FLAGS_SLAVE_MSG_RECV;
		i2c_enable_slave();
		IE_EA = 1;
	}
}

void i2c_write(uint8_t addr, uint8_t *buf, uint8_t len)
{
	uint8_t i;
	for (i = 0; i < len; i++) {
		i2cMBuf[i] = buf[i];
	}
	i2cMAddr = addr;
	i2cMBufPtr = 0;
	i2cMBufLen = len;
	SFRPAGE = 0;
	SMB0CN0_STA = 1;
}

// Disables the headphone amp, allowing us to release the SHDN line
// and provide control to the ESP32. Must be called when I2C is idle.
void i2c_initHeadphoneAmp()
{
	uint8_t attemptsLeft = 5;

	i2cMAddr = MAX9723_I2C_ADDR;
	i2cMBufPtr = 0;
	i2cMBufLen = 1;
	i2cMBuf[0] = 0x10;
	SFRPAGE = 0;
	SMB0CN0_STA = 1;
	while (i2cMAddr) {
		idle();

		// Retry transaction if we are no longer the master
		if (!SMB0CN0_MASTER) {
			if (!attemptsLeft--) {
				return;
			}

			i2cMBufPtr = 0;
			SMB0CN0_STA = 1;
		}
	}
}

SI_INTERRUPT (SMBUS0_ISR, SMBUS0_IRQn)
{
	if (SMB0CN0_ARBLOST == 0) {
		if (SMB0CN0_MASTER) {
			if (SMB0CN0_STA) {
				SMB0CN0_STA = 0;
				SMB0CN0_STO = 0;
				SMB0DAT = i2cMAddr;
			} else if (SMB0CN0_ACK) {
				if (i2cMAddr & 1) { // Read address
					if (i2cMBufPtr < i2cMBufLen) {
						i2cMBuf[i2cMBufPtr++] = SMB0DAT;
						if (i2cMBufPtr < i2cMBufLen) {
							SMB0CN0_ACK = 1;
						} else {
							i2cMAddr = 0;
							SMB0CN0_STO = 1;
						}
					} else {
						SMB0CN0_STO = 1;
					}
				} else { // Write address
					if (i2cMBufPtr < i2cMBufLen) {
						SMB0DAT = i2cMBuf[i2cMBufPtr++];
					} else {
						i2cMAddr = 0;
						SMB0CN0_STO = 1;
					}
				}
			} else {
				SMB0CN0_STO = 1;
			}
		} else { // Slave
			if (SMB0CN0_STA) {
				SMB0CN0_STA = 0;
				i2cSBufPtr = 0;
				if (SMB0DAT & 1) { // Read address
					SMB0DAT = i2cSBuf[i2cSBufPtr++];
				}
			} else if (SMB0CN0_STO) {
				i2cFlags |= I2C_FLAGS_SLAVE_MSG_RECV;
				// FIXME(supersat): Is inhibiting slave interrupts what we should do here? Even with EHACK?
				//SMB0CF |= SMB0CF_INH__SLAVE_DISABLED;
				SMB0CN0_STO = 0;
			} else {
				if (SMB0CN0_TXMODE) {
					if (SMB0CN0_ACK && (i2cSBufPtr < i2cSBufLen)) {
						SMB0DAT = i2cSBuf[i2cSBufPtr++];
					}
				} else {
					i2cSBuf[i2cSBufPtr++] = SMB0DAT;
					if (i2cSBufPtr < i2cSBufLen) {
						SMB0CN0_ACK = 1;
					}
				}
			}
		}
	} else { // Lost arbitration
		SMB0CN0_STA = 0;
		SMB0CN0_STO = 0;
		SMB0CN0_ACK = 1;
	}

	SMB0CN0_SI = 0;
}
