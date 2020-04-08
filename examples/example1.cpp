//
// Created by Andrei Balasescu on 07/04/2020.
//

#include <Arduino.h>

#include "MqttClient/MqttClient.h"

#define WIFI_SSID "WIFI-SSID"
#define WIFI_PASSWORD "WIFI-PASSWORD"
#define MQTT_IP_ADDRESS "YOUR-MQTT-BROKER-IP"
#define MQTT_PORT 1883

MqttClient::Client client;

void handleMqttConnected();
void handleMqttDisconnected(MqttClient::Reason reason);
void handleMqttMessageReceived(const String& topic, const String& payload);

void setup() {

	Serial.begin(115200);

	Serial.print("Connecting to WiFi ");

	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

	while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
		delay(1000);

		Serial.print('.');
	}

	IPAddress ipAddress;
	ipAddress.fromString(MQTT_IP_ADDRESS);

	client.setServer(ipAddress, MQTT_PORT);
	client.setClientName("Example 1");
	client.setWill("will/topic", "willPayload");

	client.setConnectedCallback(handleMqttConnected);
	client.setDisconnectedCallback(handleMqttDisconnected);
	client.setMessageReceivedCallback(handleMqttMessageReceived);

	Serial.println("Connecting to MQTT");

	client.beginConnect();
}

void loop() {

	client.loop();
}

void handleMqttConnected() {

	Serial.println("MQTT connected.");

	client.subscribe("example/topic");
	client.publish("example/topic", "example payload");
}

void handleMqttDisconnected(MqttClient::Reason reason) {

	Serial.println("Disconnected with reason ");
	Serial.println((int)reason);
}

void handleMqttMessageReceived(const String& topic, const String& payload) {

	Serial.println("Message received");

	String finalMessage = topic;
	finalMessage += ": ";
	finalMessage += payload;

	Serial.println(finalMessage);
}