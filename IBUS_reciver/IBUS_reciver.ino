/*
	IBUS_revicer is part of the IBUS Transmitter project.

	This part is used with an ESP8266 board like ESP-02 or
	ESP-01 as an IBUS receiver to work with a large amount
	of fly controllers board.

	Devloped by Lancelot H. (Azuxul)
	Version 1.0 (06/08/2018)
*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <HardwareSerial.h>

#define RECIVE_CHANNELS 12
#define RECIVE_PORT 1081

uint16_t const ibusDataSize = 32;
IPAddress const ip = IPAddress(192, 168, 1, 1);
IPAddress const mask = IPAddress(255, 255, 255, 0);

uint16_t data[RECIVE_CHANNELS] = {};
WiFiUDP server;

void setup() {

	Serial.begin(115200);

	WiFi.softAPConfig(ip, ip, mask);
	WiFi.softAP("IBUS Transmiter");

	server.begin(RECIVE_PORT);

	for (byte i = 0; i < RECIVE_CHANNELS; i++) {
		data[i] = 1500;
	}
}

void loop() {

	reciveData(data);
	sendIBUS(data);
	delay(5);
}

void sendIBUS(uint16_t dataRaw[]) {

	uint8_t data[ibusDataSize];

	uint16_t checkSum = 0xFFFF;

	data[0] = ibusDataSize;
	data[1] = 0x40;

	for (uint8_t i = 0; i < 14; i++) {
		uint16_t value = i >= RECIVE_CHANNELS ? 1500 : dataRaw[i];

		uint8_t n = 2 + 2 * i;

		data[n] = value & 0xFF;
		data[n + 1] = value >> 8;
		checkSum -= data[n];
		checkSum -= data[n + 1];
	}

	checkSum -= 0x40;
	checkSum -= 0x20;


	data[30] = checkSum & 0xFF;
	data[31] = checkSum >> 8;

	for (uint8_t i = 0; i < ibusDataSize; i++) {
		Serial.write(data[i]);
	}
}

// Need an array of the size of the recive channels
void reciveData(uint16_t data[]) {


		byte const bufferSize = RECIVE_CHANNELS * 2;
		byte buffer[bufferSize] = {};
		byte reciveCont = 0;

		while (server.parsePacket() && server.read(buffer, bufferSize) && reciveCont < 15) {
			reciveCont++;
			for (byte i = 0; i < RECIVE_CHANNELS; i++) { 
				byte index = i * 2;
				data[i] = (uint16_t(buffer[index]) << 8) | buffer[index + 1];
				//Serial.println(data[i]);
			}
		}
	
}
