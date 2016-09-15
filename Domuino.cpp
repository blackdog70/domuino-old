/*
 * Domuino.cpp
 *
 *  Created on: May 25, 2016
 *      Author: sebastiano
 */

#include "Domuino.h"
#include <FreeMemory.h>
#include <avr/io.h>
#include <avr/wdt.h>

Domuino::Domuino(unsigned char node_id) : MM485(node_id) {
	hub_id = 1;
	hb_enabled = 1;
	hb_timeout.timer = millis();
	hb_timeout.value = HB_TIMING;
}

void Domuino::begin() {
	pinMode(en485, OUTPUT);
	rs485.begin(BAUDRATE);
	rs485.flush();
#ifdef DEBUG
	Serial.println("start");
#endif
}

uint8_t Domuino::parse_packet(void* payload) {
	uint8_t size = MM485::parse_packet(payload);
	switch (PAYLOAD_CODE) {
		case QUERIES::PING:
			PAYLOAD_CODE = ANSWERS::PONG;
			size = 1;
			break;
		case QUERIES::RESET:
//			wdt_enable(WDTO_30MS);
//			while(1) {}
			break;
		case QUERIES::HUB: {
				hub_id = HUB_DATA.node_id;
				PAYLOAD_CODE = ACK;
				size = 1;
			}
			break;
		case QUERIES::CONFIG: {
				switch(CONFIG_DATA.parameter) {
					// CONFIG HEARTBIT FREQUENCY
					case PARAMETERS::HBT:
						if(CONFIG_DATA.value >= 1)
							hb_timeout.value = CONFIG_DATA.value * 1000UL;
						PAYLOAD_CODE = ACK;
						size = 1;
						break;
				}
			}
			break;
		case QUERIES::MEM: {
				t_payload<payload_mem> *mem = (t_payload<payload_mem>*)payload;
				mem->code = ANSWERS::MEM;
				mem->data.memory = freeMemory();
				size = sizeof(t_payload<payload_mem>);
			}
			break;
	}
	return size;
}

uint8_t Domuino::run() {
	uint8_t ready = MM485::run();
	if(ready && hb_enabled) {
#ifdef DEBUG
		Serial.println("Send HBT");
#endif
		t_payload<uint8_t> hbt;
		hbt.code = QUERIES::HBT;
		update_hub(&hbt, sizeof(hbt), &hb_timeout);
	}
	return ready;
}

// TODO: Need optimization
void Domuino::update_hub(void* data, uint8_t size, timeout* t) {
	int enable = 1;
	if (t != NULL)
		enable = (millis() - t->timer) > t->value;
	if(enable) {
		if (t != NULL)
			t->timer = millis();
		send(hub_id, data, size);
	}
}
