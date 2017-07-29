#include "board.h"
#include "efm8_usb.h"

void hid_sendBadgeINReport()
{
	badgeHIDINReport_t report;

	report.reportID = 1;
	report.encoderStatus = hidEncoderStatus & ~ENCODER_DIRTY;
	report.powerFlags = powerFlags;
	report.vbat = battVoltage;

	USBD_Write(EP3IN, (uint8_t *)&report, sizeof(report), true);
}
