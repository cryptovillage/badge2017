#include "board.h"

uint16_t battVoltage;
uint8_t powerFlags;
uint8_t backlightBrightness;
uint16_t dtrLowTime;
uint16_t esp32FlashTime;
uint16_t lastBattADCTime;
uint16_t lastUARTTxTime;
uint16_t lastHIDTime;
uint16_t lastChargeAnimTime;
uint8_t chargeAnimCycle;

const uint8_t animCycleBrightness[] = {
	1, 2, 4, 8, 16, 32, 16, 8, 4, 2
};

void power_init()
{
	battVoltage = 0;
	powerFlags = 0;
	backlightBrightness = 0xff;
	dtrLowTime = 0;
	esp32FlashTime = 0;
	lastBattADCTime = 0;
	lastUARTTxTime = 0;
	lastHIDTime = 0;
	lastChargeAnimTime = 0;
	chargeAnimCycle = 0;

	// Check power switch comparator in case the initial rising edge is missed
	SFRPAGE = 0;
	if (CMP0CN0 & (1 << 6)) {
		powerFlags = POWER_FLAG_PHYSICAL_SWITCH_ON | POWER_FLAG_POWER_FLAGS_CHANGED;
	}
}

void power_update()
{
	uint16_t diff;

	IE_EA = 0;

	if (!(powerFlags & POWER_FLAG_VIRTUAL_SWITCH_ON)) {
		if (lastHIDTime || lastUARTTxTime) {
			powerFlags |= POWER_FLAG_VIRTUAL_SWITCH_ON | POWER_FLAG_POWER_FLAGS_CHANGED;
		}
	}

	if (powerFlags & POWER_FLAG_POWER_FLAGS_CHANGED) {
		powerFlags &= ~POWER_FLAG_POWER_FLAGS_CHANGED;
		if (powerFlags & (POWER_FLAG_PHYSICAL_SWITCH_ON | POWER_FLAG_VIRTUAL_SWITCH_ON)) {
			powerFlags |= POWER_FLAG_POWERED_ON;
			// Switch to low current (100mA) charging
			BC_EN_2 = 0; // Disable high current
			BC_EN_1 = 1; // Enable low current
			leds_init();
			ledFlags |= LED_FLAG_UPDATE_LEDS;
		} else {
			powerFlags &= ~POWER_FLAG_POWERED_ON;
			// Switch to high current (500mA) charging
			BC_EN_1 = 0; // Disable low current
			BC_EN_2 = 1; // Enable high current
			lastChargeAnimTime = _cur_ms;
			chargeAnimCycle = 0;
			leds_init();
			ledFlags |= LED_FLAG_UPDATE_LEDS;
		}
	}

	if (uartFlags & UART_FLAGS_UPDATED) {
		if (!(uartFlags & UART_FLAGS_DTR)) {
			dtrLowTime = _cur_ms;
		} else { // DTR is set
			if ((uartFlags & UART_FLAGS_RTS) && dtrLowTime) {
				// Check for a signal to begin reflashing the ESP32
				diff = msDiff(dtrLowTime, _cur_ms);
				if (diff > 80 && diff < 1600) {
					powerFlags |= POWER_FLAG_VIRTUAL_SWITCH_ON | POWER_FLAG_POWER_FLAGS_CHANGED;
					esp32FlashTime = _cur_ms;
					ESP32_EN = 0;
					SFRPAGE = 0;
					PCA0CPH0 = 0;
				}
			}
			dtrLowTime = 0;
		}
		uartFlags &= ~UART_FLAGS_UPDATED;
	}

	IE_EA = 1;

	if (esp32FlashTime) {
		diff = msDiff(esp32FlashTime, _cur_ms);
		if (diff > 300) {
			ESP32_BOOT = ESP32_BOOT_IDLE;
			esp32FlashTime = 0;
		} else if (diff > 200) {
			ESP32_EN = 1;
			SFRPAGE = 0;
			PCA0CPH0 = backlightBrightness;
		} else if (diff > 100) {
			ESP32_BOOT = ESP32_BOOT_ACTIVE;
			IE_EA = 0;
			powerFlags |= POWER_FLAG_ESP32_IN_BOOTLOADER;
			IE_EA = 1;
		}
	} else if (powerFlags & POWER_FLAG_ESP32_IN_BOOTLOADER) {
		diff = msDiff(lastUARTTxTime, _cur_ms);
		if (diff > 5100) {
			IE_EA = 0;
			powerFlags &= ~POWER_FLAG_ESP32_IN_BOOTLOADER;
			IE_EA = 1;
		} else if (diff > 5000) {
			ESP32_EN = 0;
			SFRPAGE = 0;
			PCA0CPH0 = 0;
		}
	} else {
		if (powerFlags & POWER_FLAG_POWERED_ON) {
			ESP32_EN = 1;
			SFRPAGE = 0;
			PCA0CPH0 = backlightBrightness;
		} else {
			ESP32_EN = 0;
			SFRPAGE = 0;
			PCA0CPH0 = 0;
		}
	}

	if (!(powerFlags & POWER_FLAG_POWERED_ON)) {
		if (msDiff(lastChargeAnimTime, _cur_ms) >= 100) {
			if (!CSTAT) { // Charging
				leds_set_charging_pattern(animCycleBrightness[chargeAnimCycle++]);
			} else {
				leds_set_charged_pattern(animCycleBrightness[chargeAnimCycle++]);
			}
			if (chargeAnimCycle >= sizeof(animCycleBrightness)) {
				chargeAnimCycle = 0;
			}
			lastChargeAnimTime = _cur_ms;
		}
	}

	// Read the battery voltage every second
	if (msDiff(lastBattADCTime, _cur_ms) >= 1000) {
		SFRPAGE = 0;
		ADC0CN0_ADBUSY = 1;
	}
}

//-----------------------------------------------------------------------------
// ADC0EOC_ISR
//-----------------------------------------------------------------------------
//
// ADC0EOC ISR Content goes here. Remember to clear flag bits:
// ADC0CN0::ADINT (Conversion Complete Interrupt Flag)
//
//-----------------------------------------------------------------------------
SI_INTERRUPT (ADC0EOC_ISR, ADC0EOC_IRQn)
{
	uint16_t newVoltage;
	newVoltage = ADC0;
	if (newVoltage != battVoltage) {
		battVoltage = newVoltage;
		//powerFlags |= POWER_FLAG_BATTERY_VOLTAGE_UPDATED;
	}
	powerFlags |= POWER_FLAG_BATTERY_VOLTAGE_UPDATED;
	ADC0CN0_ADINT = 0;
}


//-----------------------------------------------------------------------------
// CMP0_ISR
//-----------------------------------------------------------------------------
//
// CMP0 ISR Content goes here. Remember to clear flag bits:
// CMP0CN0::CPFIF (Comparator Falling-Edge Flag)
// CMP0CN0::CPRIF (Comparator Rising-Edge Flag)
//
//-----------------------------------------------------------------------------

#define CPRIF (1 << 5)
#define CPFIF (1 << 4)

SI_INTERRUPT (CMP0_ISR, CMP0_IRQn)
{
	if (CMP0CN0 & CPRIF) {
		CMP0CN0 &= ~CPRIF;
		powerFlags |= POWER_FLAG_PHYSICAL_SWITCH_ON | POWER_FLAG_POWER_FLAGS_CHANGED;
	}
	if (CMP0CN0 & CPFIF) {
		CMP0CN0 &= ~CPFIF;
		powerFlags &= ~POWER_FLAG_PHYSICAL_SWITCH_ON;
		powerFlags |= POWER_FLAG_POWER_FLAGS_CHANGED;
	}
}
