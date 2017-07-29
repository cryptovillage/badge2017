#include <SI_EFM8UB1_Defs.h>
#include "fifo.h"

SI_SBIT(ACCEL_INT, SFR_P0, 0);
SI_SBIT(ENC_A, SFR_P0, 1);
SI_SBIT(ENC_B, SFR_P0, 2);
SI_SBIT(ENC_SW, SFR_P0, 3);
SI_SBIT(ESP32_BOOT, SFR_P1, 3);
SI_SBIT(CSTAT, SFR_P1, 4);
SI_SBIT(ESP32_EN, SFR_P1, 7);
SI_SBIT(MIC_SHDN, SFR_P2, 0);
SI_SBIT(LCD_EN, SFR_P2, 1);
SI_SBIT(BC_EN_1, SFR_P2, 2);
SI_SBIT(BC_EN_2, SFR_P2, 3);

#define ENCODER_CW			(1 << 0)
#define ENCODER_CCW			(1 << 1)
#define ENCODER_DOWN		(1 << 2)
#define ENCODER_UP			(1 << 3)
#define ENCODER_PRESSED		(1 << 4)
#define ENCODER_NEEDS_POLL	(1 << 5)
#define ENCODER_DIRTY		(1 << 7)

extern uint8_t internalEncoderStatus;
extern uint8_t i2cEncoderStatus;
extern uint8_t hidEncoderStatus;
extern void encoder_init();
extern void encoder_updateStatus();
extern void encoder_updateInternalStatus();
extern void encoder_updateExternalStatus(uint8_t *status);

#define	BADGE_HID_REPORT_IN_DIAL_LEFT		3
#define	BADGE_HID_REPORT_IN_DIAL_RIGHT		1
#define	BADGE_HID_REPORT_IN_DIAL_PRESSED	4
#define BADGE_HID_REPORT_OUT_MIC_SHDN		1
#define BADGE_HID_REPORT_OUT_LCD_EN			2
#define BADGE_HID_REPORT_OUT_BC_1			4
#define BADGE_HID_REPORT_OUT_BC_2			8
#define BADGE_HID_REPORT_OUT_ESP32_EN		16

typedef struct {
	uint8_t reportID;
	uint8_t encoderStatus;
	uint8_t powerFlags;
	uint16_t vbat;
} badgeHIDINReport_t;

extern void hid_sendBadgeINReport();

extern serialFifo_t rxFifo;
extern serialFifo_t txFifo;

extern uint32_t uartBPS;
extern uint8_t uartStopBits;
extern uint8_t uartParityType;
extern uint8_t uartDataBits;
extern uint8_t uartFlags;

extern void uart_init();

// These must match the bits used in SET_CONTROL_LINE_STATE
#define UART_FLAGS_DTR						1
#define UART_FLAGS_RTS						2
#define UART_FLAGS_UPDATED					128

extern uint8_t rxHIDBuf[64];
extern uint8_t txBuf[64];

typedef struct {
	uint8_t ones:3;
	uint8_t brightness:5;
	uint8_t blue;
	uint8_t green;
	uint8_t red;
} LEDFrame;

extern uint8_t ledBuf[32];
extern LEDFrame *ledFrames;
extern uint8_t ledFlags;

#define LED_FLAG_UPDATE_LEDS				1

extern void leds_init();
extern void leds_beginTx();
extern uint8_t leds_areBusy();
extern void leds_set_charging_pattern(uint8_t brightness);
extern void leds_set_charged_pattern(uint8_t brightness);

#define	POWER_FLAG_POWERED_ON				1
#define POWER_FLAG_PHYSICAL_SWITCH_ON		2
#define POWER_FLAG_VIRTUAL_SWITCH_ON		4
#define POWER_FLAG_USB_HOST_CONNECTED		8
#define POWER_FLAG_USB_CHARGER_CONNECTED	16
#define POWER_FLAG_ESP32_IN_BOOTLOADER		32
#define POWER_FLAG_BATTERY_VOLTAGE_UPDATED	64
#define POWER_FLAG_POWER_FLAGS_CHANGED		128

extern uint16_t battVoltage;
extern uint8_t powerFlags;
extern uint8_t backlightBrightness;
extern uint16_t lastUARTTxTime;
extern uint16_t lastHIDTime;

extern void power_init();
extern void power_update();

#define MAX_I2C_PKT_SIZE					64

#define MMA8452Q_I2C_ADDR					0x38
#define MAX9723_I2C_ADDR					0x9A
#define ATECC508A_I2C_ADDR					0xC0
#define EFM8_I2C_ADDR						0xC8
#define ESP32_I2C_ADDR						0xCC

#define I2C_FLAGS_SLAVE_MSG_RECV			1
//#define I2C_FLAGS_SLAVE_READ_EXPECTED		2

extern uint8_t i2cFlags;

extern void i2c_init();
extern void i2c_update();
extern void i2c_initHeadphoneAmp();

extern data uint16_t _cur_ms;
extern int16_t msDiff(uint16_t ts1, uint16_t ts2);

// For V2 boards
//#define ESP32_BOOT_IDLE						1
//#define ESP32_BOOT_ACTIVE					0

// For V3 boards
#define ESP32_BOOT_IDLE						0
#define ESP32_BOOT_ACTIVE					1

#define idle() do { \
	PCON0 |= PCON0_IDLE__IDLE; \
	NOP(); \
	NOP(); \
	NOP(); \
} while (0);
