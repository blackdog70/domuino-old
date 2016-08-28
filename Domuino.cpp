/*
 * Domuino.cpp
 *
 *  Created on: May 25, 2016
 *      Author: sebastiano
 */

#include "Domuino.h"
#include <FreeMemory.h>
#include <packet.h>
#include <avr/io.h>
#include <avr/wdt.h>

#define BAUDRATE 19200
#define HB_TIMING 10000UL  // milliseconds frequency for heartbeat

Domuino::Domuino(unsigned char node_id) : MM485(node_id) {
	hub_id = 1;
	hb_enabled = 1;
	hb_timeout.timer = millis();
	hb_timeout.value = HB_TIMING;
}

void Domuino::begin() {
#ifdef SOFTWARESERIAL
	pinMode(en485, OUTPUT);
	rs485.begin(BAUDRATE);
	digitalWrite(en485, HIGH);
	rs485.println("start");
	rs485.flush();
	digitalWrite(en485, LOW);
#else
	Serial.begin(BAUDRATE);
	Serial.println("start");
#endif
}

uint8_t Domuino::parse_packet(unsigned char* data, Packet* pkt) {
	uint8_t size = MM485::parse_packet(data, pkt);
	switch (pkt->data[0]) {
		case QUERIES::PING:
			data[0] = ANSWERS::PONG;
			break;
		case QUERIES::RESET:
//			wdt_enable(WDTO_30MS);
//			while(1) {}
			break;
		case QUERIES::HUB:
			hub_id = pkt->data[1];
			break;
		case QUERIES::CONFIG:
			switch(pkt->data[1]) {
				// CONFIG HEARTBIT FREQUENCY
				case PARAMETERS::HBT:
					if(pkt->data[2] >= 1)
						hb_timeout.value = pkt->data[2] * 1000UL;
					break;
				default:
					data[0] = ERR;
					break;
			}
			break;
		case QUERIES::MEM: {
				data[0] = ANSWERS::MEM;
				int fm = freeMemory();
				memcpy((char *)(data + 1), (char*)&fm, sizeof(fm));
				size = 1 + sizeof(fm);
			}
			break;
		default:
			data[0] = ERR;
			break;
	}
	return size;
}

void Domuino::run() {
	MM485::run();
	if(hb_enabled)
		update_hub(QUERIES::HBT, &hb_timeout);
}

// TODO: Need optimization
void Domuino::update_hub(const unsigned char cmd, timeout* t) {
	int enable = 1;
	if (t != NULL)
		enable = (millis() - t->timer) > t->value;
	if(enable) {
		unsigned char msg[1];

		t->timer = millis();
		msg[0] = cmd;
		send(hub_id, msg, sizeof(msg));
	}
}

// TODO: Need optimization
void Domuino::update_hub(const unsigned char cmd, int value1, timeout* t) {
	int enable = 1;
	if (t != NULL)
		enable = (millis() - t->timer) > t->value;
	if(enable) {
		unsigned char msg[1 + sizeof(value1)];

		if (t!=NULL)
			t->timer = millis();
		msg[0] = cmd;
		memcpy((char *)(msg + 1), (char*)&value1, sizeof(value1));
		send(hub_id, msg, sizeof(msg));
	}
}

// TODO: Need optimization
void Domuino::update_hub(const unsigned char cmd, int value1, int value2, timeout* t) {
	int enable = 1;
	if (t != NULL)
		enable = (millis() - t->timer) > t->value;
	if(enable) {
		unsigned char msg[1 + sizeof(value1) + sizeof(value2)];

		if (t!=NULL)
			t->timer = millis();
		msg[0] = cmd;
		memcpy((char *)(msg + 1), (char*)&value1, sizeof(value1));
		memcpy((char *)(msg + 1 + sizeof(value1)), (char*)&value2, sizeof(value2));
		send(hub_id, msg, sizeof(msg));
	}
}

// TODO: Need optimization
void Domuino::update_hub(const unsigned char cmd, float value1, timeout* t) {
	int enable = 1;
	if (t != NULL)
		enable = (millis() - t->timer) > t->value;
	if(enable) {
		unsigned char msg[1 + sizeof(value1)];

		if (t!=NULL)
			t->timer = millis();
		msg[0] = cmd;
		memcpy((char *)(msg + 1), (char*)&value1, sizeof(value1));
		send(hub_id, msg, sizeof(msg));
	}
}
