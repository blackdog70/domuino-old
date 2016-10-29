/*
 * Domuino.h
 *
 *  Created on: May 25, 2016
 *      Author: sebastiano
 */

#ifndef DOMUINO_H_
#define DOMUINO_H_

#include <domunet.h>

#define HB_TIMING 10000UL  // milliseconds frequency for heartbeat
#define PAYLOAD_CODE ((base_payload*)payload)->code
#define PAYLOAD_SIZE sizeof(base_payload)
#define CONFIG_DATA ((payload_config*)payload)
#define HUB_DATA ((payload_hub*)payload)
#define MAX_CHANNELS 3
#define MAX_RETRY 3

// PARAMETERS AND ANSWERS HAS CODE <= 0x7f
// QUERIES HAS CODE > 0x7f

class PARAMETERS {
public:
	// SYSTEM
	static const unsigned char ERR = 0x7f;
	static const unsigned char ACK = 0x7e;
	static const unsigned char PONG = 0x7d;
	static const unsigned char RAM = 0x7c;
	static const unsigned char HBT = 0x7b;
	static const unsigned char START = 0x7a;
	// DEVICE
	static const unsigned char SWITCH = 0x01;
	static const unsigned char LIGHT = 0x02;
	static const unsigned char BINARY_OUT = 0x03;
	static const unsigned char EMS = 0x04;
	static const unsigned char DHT = 0x05;
	static const unsigned char PIR = 0x06;
	static const unsigned char LUX = 0x07;
};

class ANSWERS: public PARAMETERS {};

class QUERIES {
public:
	// SYSTEM
	static const unsigned char START = 0x80;
	static const unsigned char PING = 0x81;
	static const unsigned char RESET = 0x82;
	static const unsigned char CONFIG = 0x88;
	static const unsigned char HUB = 0x89;
	static const unsigned char MEM = 0x90;
	static const unsigned char HBT = 0x9f;
	// DEVICE
	static const unsigned char DHT = 0xA0;
	static const unsigned char EMS = 0xA1;
	static const unsigned char BINARY_OUT = 0xA2;
	static const unsigned char SWITCH = 0xA3;
	static const unsigned char LIGHT = 0xA4;
	static const unsigned char PIR = 0xA5;
	static const unsigned char LUX = 0xA6;
};

struct timeout {
	unsigned long timer;
	unsigned long value;
};

struct data_channel {
	void* data;
	uint8_t size;
	timeout* t;
	uint8_t retry;
	uint8_t sending;
};

struct payload_config : base_payload {
	uint8_t parameter;
	char value;
};

struct payload_hub : base_payload {
	uint8_t node_id;
};

struct payload_mem : base_payload {
	uint16_t memory;
};

struct payload_start : payload_hub {};

class Domuino: public DomuNet {
public:
	Domuino(uint8_t node_id, uint32_t baudrate);
	virtual ~Domuino() {};
	void run();
	void begin();

protected:
	uint8_t hub_id;
	timeout hb_timeout;
	uint8_t channel_id;
	data_channel channels[MAX_CHANNELS];
	base_payload hbt;

	uint8_t parse_packet(void* payload);
	data_channel* channel(void* data, uint8_t size, timeout* t);
	void refresh_channel(data_channel* data);

private:
	void update_hub(data_channel* data);
//	void push_out_alive();
};

#endif /* DOMUINO_H_ */
