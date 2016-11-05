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

Domuino::Domuino(uint8_t node_id, uint32_t baudrate) : DomuNet(node_id, baudrate) {
	hub_id = 1;
	hb_timeout.value = HB_TIMING;
	channel_id = 0;
}

void Domuino::begin() {
	pinMode(en485, OUTPUT);
	rs485.flush();
	payload_start start;
	start.node_id = node_id;
	send(hub_id, &start, sizeof(base_payload));
}

uint8_t Domuino::parse_packet(void* payload) {
	uint8_t size = 0;
	switch (PAYLOAD_CODE) {
		case QUERIES::PING:
			PAYLOAD_CODE = ANSWERS::PONG;
			size = PAYLOAD_SIZE;
			break;
		case QUERIES::RESET:
//			wdt_enable(WDTO_30MS);
//			while(1) {}
			break;
		case QUERIES::HUB: {
				hub_id = HUB_DATA->node_id;
				PAYLOAD_CODE = ANSWERS::ACK;
				size = PAYLOAD_SIZE;
			}
			break;
		case QUERIES::CONFIG: {
				switch(CONFIG_DATA->parameter) {
					// CONFIG HEARTBIT FREQUENCY
					case PARAMETERS::HBT:
						if(SIMPLE_PARAMETER->value >= 1)
							hb_timeout.value = SIMPLE_PARAMETER->value * 1000UL;
						PAYLOAD_CODE = ANSWERS::ACK;
						size = PAYLOAD_SIZE;
						break;
				}
			}
			break;
		case QUERIES::MEM: {
				payload_mem mem;

				mem.memory = freeMemory();
				size = sizeof(payload_mem);
				memcpy(payload, &mem, size);
			}
			break;
	}
	return size;
}

void Domuino::run() {
	if (receive()) {
		unsigned char payload_[MAX_DATA_SIZE];

		memcpy(payload_, &packet_in.payload, packet_in.core.data_size);
		// Parse a packet and get an answer
		packet_in.core.data_size = parse_packet(&payload_);
		if (packet_in.core.data_size < 1)
			// Don't send an answer if message is unrecognized
			return;
		packet_in.core.dest = packet_in.core.source;
		memcpy(&packet_in.payload, payload_, packet_in.core.data_size);
#ifdef DEBUG
		Serial.println("Payload Out");
		unsigned char *p = (uint8_t*)&(packet_in.payload);
		for(int i=0; i<packet_in.core.data_size; i++) {
			Serial.print(i);
			Serial.print(" :");
			Serial.println(*(p + i), HEX);
		}
#endif
		delay(TX_DELAY);
		write(&packet_in);
	} else
		for (int i = 0; i < channel_id; i++) {
			update_hub(&channels[i]);
		}
}

void Domuino::update_hub(data_channel* data) {
	if ((data == NULL) || (data->t == NULL))
		return;
	if((millis() - data->t->timer) > data->t->value) {
		if (data->function()) {
			if (send(hub_id, data->data, data->size)) {
				data->t->timer = millis();
				data->retry = 0;
				parse_ack(&packet_in);
			}
			else
				if (data->retry++ > MAX_RETRY) {
					data->t->timer = millis();
					data->retry = 0;
				}
		}
	}
}

void Domuino::channel(void* data, uint8_t size, timeout* t, ifuncptr fn) {
	if (channel_id < MAX_CHANNELS && size <= MAX_DATA_SIZE) {
		channels[channel_id].data = data;
		channels[channel_id].size = size;
		channels[channel_id].t = t;
		channels[channel_id].retry = 0;
		channels[channel_id].function = fn;
		channel_id++;
	}
}
