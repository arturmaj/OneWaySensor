/*
Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.
*/

/**
* Example RF Radio Ping Pair
*
* This is an example of how to use the RF24 class.  Write this sketch to two different nodes,
* connect the role_pin to ground on one.  The ping node sends the current time to the pong node,
* which responds by sending the value back.  The ping node can then see how long the whole cycle
* took.
*/

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT11
#define ONOFFPIN 6
DHT dht(DHTPIN, DHTTYPE);

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9, 10);

//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = {
	0xF0F0F0F0E1LL, 0xF0F0F0F0D3LL
};
int lasttemp = 0;
int tmp = 1;
bool on = false;
const unsigned long postingInterval = 120000;
unsigned long lastRequest = 0;
void setup(void) {

	//
	// Print preamble
	//

	Serial.begin(9600);
	//printf_begin();
	//printf("\n\rRF24/examples/pingpair/\n\r");
	pinMode(ONOFFPIN, OUTPUT);
	//
	// Setup and configure rf radio
	//

	radio.begin();

	// optionally, increase the delay between retries & # of retries
	radio.setRetries(15, 15);

	// optionally, reduce the payload size.  seems to
	// improve reliability
	radio.setPayloadSize(sizeof(int));
	radio.setPALevel(RF24_PA_HIGH);
	//radio.setDataRate(RF24_250KBPS);
	//
	// Open pipes to other nodes for communication
	//

	// This simple sketch opens two pipes for these two nodes to communicate
	// back and forth.
	// Open 'our' pipe for writing
	// Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)


	radio.openWritingPipe(pipes[1]);
	radio.openReadingPipe(1, pipes[0]);


	//
	// Start listening
	//

	radio.startListening();

	//
	// Dump the configuration of the rf unit for debugging
	//

	radio.printDetails();
	dht.begin();
	lastRequest = millis();
}

void SendData(void){
	Serial.println("probuje wyslac...");


	// Send the final one back.
	
	radio.stopListening();
	bool ok = radio.write(&tmp, sizeof(int));
	if (ok){
		Serial.print("Wyslalem: ");
		Serial.println(tmp);
	}
	radio.startListening();
	delay(200);


}
void loop() {
	long now = millis();
	int h = dht.readHumidity();
	int t = dht.readTemperature();
	tmp= t * 100 + h;
	//Serial.println(now - lastRequest);
	if (now - lastRequest >= postingInterval) {
		for (int x = 0; x < 1; x++){
			SendData();
		}
		lastRequest = now;
	}
	if (lasttemp != t){
		SendData();
		lasttemp = t;
	}
}
// vim:cin:ai:sts=2 sw=2 ft=cpp

