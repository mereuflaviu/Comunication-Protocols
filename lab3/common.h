#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "link_emulator/lib.h"

uint8_t simple_csum(uint8_t *buf, size_t len);

uint32_t crc32(uint8_t *buf, size_t len);

/* Layer 3 header */
struct l3_msg_hdr {
	uint16_t len;
	uint32_t sum;
};

/* Layer 3 frame */
struct l3_msg {
	struct l3_msg_hdr hdr;
	/* Data */
	/* MTU = 1500 => we can send 1500 - header size bytes of data */
	char payload[1500 - sizeof(struct l3_msg_hdr)];
};
