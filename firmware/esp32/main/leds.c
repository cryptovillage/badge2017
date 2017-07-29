#include "leds.h"
#include "i2c.h"
#include "lwip/udp.h"

uint8_t ledBuf[32];
LEDFrame *ledFrames = (LEDFrame *)ledBuf;

void led_udp_pkt_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, ip_addr_t *addr, uint16_t port)
{
	if (p && p->len >= 3) {
		uint8_t i;

		for (i = 0; i < 8; i++) {
			ledFrames[i].ones = 7;
			ledFrames[i].brightness = 31;
			ledFrames[i].blue = *(uint8_t *)(p->payload + 2);
			ledFrames[i].green = *(uint8_t *)(p->payload + 1);
			ledFrames[i].red = *(uint8_t *)(p->payload + 0);
		}

		i2c_send_led_cmd(ledBuf);
	}

	if (p) {
		pbuf_free(p);
	}
}

void led_init_udp()
{
    struct udp_pcb *u = udp_new();
    udp_bind(u, IP_ADDR_ANY, 28500);
    udp_recv(u, led_udp_pkt_callback, NULL);
}
