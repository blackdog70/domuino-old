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
#define SIMPLE_PARAMETER ((simple_parameter*)CONFIG_DATA->value)
#define OUT_PARAMETERS ((out_parameters*)CONFIG_DATA->value)
#define HUB_DATA ((payload_hub*)payload)
#define SWITCH_DATA ((payload_switch*)payload)
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
	static const unsigned char MEM = 0x7c;
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
	timeout():timer(millis()), value(0) {};
};

typedef uint8_t (*ifuncptr)();

struct data_channel {
	void* data;
	uint8_t size;
	timeout* t;
	uint8_t retry;
	ifuncptr function;
};

// FIXME: Suddividere strutture tra QUERY e ANSWER

/*
 * PARAMETERS
 */
struct simple_parameter {
	char value;
};

struct out_parameters {
	char pin;
	char value;
	char negate;
};

/*
 * INCOMING PAYLOAD
 */
struct payload_config : base_payload {
	uint8_t parameter;
	char *value;
	payload_config():base_payload(QUERIES::CONFIG) {};
};

struct payload_hub : base_payload {
	uint8_t node_id;
	payload_hub():base_payload(QUERIES::HUB) {};
};

struct payload_mem : base_payload {
	uint16_t memory;
	payload_mem():base_payload(ANSWERS::MEM) {};
};

struct payload_start : base_payload {
	uint8_t node_id;
	payload_start():base_payload(QUERIES::START) {};
};

struct payload_hbt : base_payload {
	payload_hbt():base_payload(QUERIES::HBT) {};
};

struct payload_lux : base_payload {
	int value;
	payload_lux():base_payload(QUERIES::LUX) {};
};

struct payload_pir : base_payload {
	char state;
	payload_pir():base_payload(QUERIES::PIR) {};
};

struct payload_switch : base_payload {
	char pin;
	char value;
	payload_switch():base_payload(QUERIES::SWITCH) {};
};

//add your function definitions for the project domuwin here
#define NUM_EMS 2
#define NUM_IO 2

struct payload_inps : base_payload {
	uint8_t state[NUM_IO];
	payload_inps():base_payload(QUERIES::SWITCH) {};
};

struct payload_outs : base_payload {
	uint8_t state[NUM_IO];
	payload_outs():base_payload(QUERIES::BINARY_OUT) {};
};

struct payload_ems : base_payload {
	double value[NUM_EMS];
	payload_ems():base_payload(QUERIES::EMS) {};
};

class Domuino: public DomuNet {
public:
	Domuino(uint8_t node_id, uint32_t baudrate);
	virtual ~Domuino() {};
	void run();
	void begin();
	void channel(void* data, uint8_t size, timeout* t, ifuncptr fn);

protected:
	uint8_t hub_id;
	timeout hb_timeout;
	uint8_t channel_id;
	data_channel channels[MAX_CHANNELS];
	payload_hbt hbt;

	virtual uint8_t parse_packet(void* payload);

private:
	void update_hub(data_channel* data);
//	void push_out_alive();
};

#endif /* DOMUINO_H_ */
