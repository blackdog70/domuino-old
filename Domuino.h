/*
 * Domuino.h
 *
 *  Created on: May 25, 2016
 *      Author: sebastiano
 */

#ifndef DOMUINO_H_
#define DOMUINO_H_

#include <mm485.h>

struct timeout {
	unsigned long timer;
	unsigned long value;
};

class PARAMETERS {
public:
	// SYSTEM
	static const unsigned char HBT = 0x7d;
	static const unsigned char MEM = 0x7e;
	static const unsigned char PONG = 0x7f;
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

class Domuino: public MM485 {
public:
	Domuino(unsigned char node_id);
	virtual ~Domuino() {};
	void run();
	void begin();

protected:
	uint8_t hub_id;
	timeout hb_timeout;
	int hb_enabled;

	uint8_t parse_packet(unsigned char* data, Packet* pkt);
	void update_hub(const unsigned char cmd, timeout* t);
	void update_hub(const unsigned char cmd, int value1, timeout* t);
	void update_hub(const unsigned char cmd, int value1, int value2, timeout* t);
	void update_hub(const unsigned char cmd, float value1, timeout* t);

private:
	void push_out_alive();
};

#endif /* DOMUINO_H_ */