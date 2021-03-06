//
// Created by Andrei Balasescu on 07/04/2020.
//

#include "MqttClient.h"

using namespace std::placeholders;

// LIFECYCLE

MqttClient::Client::Client() :
	m_client (PubSubClient(m_wifiClient))
{
	this->m_client.setCallback(std::bind(&Client::handleMessageReceived, this, _1, _2, _3));
}

void MqttClient::Client::loop() {

	if (!this->m_client.connected()) {
		if (this->m_wasConnected) {
			this->handleConnectionLost();
		}

		if (this->m_tryConnection) {
			this->m_tryConnection = false;

			this->connect();
		}
	}

	this->m_client.loop();
}

// PUBLIC

// GENERAL

void MqttClient::Client::publish(const String& topic, const String& payload, bool retain) {

	this->m_client.publish(topic.c_str(), payload.c_str(), retain);
}

void MqttClient::Client::subscribe(const String& topic) {

	if (topic.isEmpty()) {
		return;
	}

	this->m_client.subscribe(topic.c_str());
}

void MqttClient::Client::subscribe(const String &topic, MqttClient::Callbacks::MessageReceivedCallback callback) {

	if (topic.isEmpty()) {
		return;
	}

	this->subscribe(topic);

	this->m_subscribers[topic] = callback;
}

void MqttClient::Client::unsubscribe(const String& topic) {

	if (topic.isEmpty()) {
		return;
	}

	this->m_client.unsubscribe(topic.c_str());

	if (this->m_subscribers.find(topic) != this->m_subscribers.end()) {
		this->m_subscribers.erase(topic);
	}
}

// CONNECTION

void MqttClient::Client::beginConnect() {

	this->m_tryConnection = true;
}

bool MqttClient::Client::connect() {

	bool hasWill = !this->m_willTopic.isEmpty();

	const char *willTopic = nullptr;
	const char *willPayload = nullptr;

	if (hasWill) {
		willTopic = this->m_willTopic.c_str();
		willPayload = this->m_willPayload.c_str();
	}

	bool connectionEstablished = this->m_client.connect(this->m_clientName.c_str(), this->m_user.c_str(), this->m_password.c_str(), willTopic, 0, this->m_willRetain, willPayload);

	if (connectionEstablished) {
		this->handleConnectionEstablished();
	} else {
		this->handleConnectionLost();
	}

	return connectionEstablished;
}

void MqttClient::Client::disconnect() {

	this->m_client.disconnect();
}

// GETTERS

bool MqttClient::Client::isConnected() {

	return this->m_client.connected();
}

// SETTERS

void MqttClient::Client::setConnectedCallback(Callbacks::ConnectedCallback onConnected) {

	this->m_onConnected = onConnected;
}

void MqttClient::Client::setDisconnectedCallback(Callbacks::DisconnectedCallback onDisconnected) {

	this->m_onDisconnected = onDisconnected;
}

void MqttClient::Client::setMessageReceivedCallback(MqttClient::Callbacks::MessageReceivedCallback onMessageReceived) {

	this->m_onMessageReceived = onMessageReceived;
}

void MqttClient::Client::setClientName(const String& clientName) {

	this->m_clientName = clientName;
}

void MqttClient::Client::setServer(const IPAddress& server, unsigned int port) {

	this->m_serverIp = server;
	this->m_port = port;

	this->m_client.setServer(this->m_serverIp, this->m_port);
}

void MqttClient::Client::setCredentials(const String& user, const String& password) {

	this->m_user = user;
	this->m_password = password;
}

void MqttClient::Client::setWill(const String& willTopic, const String& willPayload, bool retain) {

	this->m_willTopic = willTopic;
	this->m_willPayload = willPayload;
	this->m_willRetain = retain;
}

// PRIVATE

MqttClient::Reason MqttClient::Client::getDisconnectReason() {

	int state = this->m_client.state();

	if (state == MQTT_CONNECTION_TIMEOUT) {
		return MqttClient::Reason::ConnectionTimeout;
	}

	if (state == MQTT_CONNECTION_LOST) {
		return MqttClient::Reason::ConnectionLost;
	}

	if (state == MQTT_CONNECT_FAILED) {
		return MqttClient::Reason::ConnectionFailed;
	}

	if (state == MQTT_CONNECT_BAD_PROTOCOL) {
		return MqttClient::Reason::BadProtocol;
	}

	if (state == MQTT_CONNECT_BAD_CLIENT_ID) {
		return MqttClient::Reason::BadClientId;
	}

	if (state == MQTT_CONNECT_BAD_CREDENTIALS) {
		return MqttClient::Reason::BadCredentials;
	}

	if (state == MQTT_CONNECT_UNAVAILABLE) {
		return MqttClient::Reason::Unavailable;
	}

	if (state == MQTT_CONNECT_UNAUTHORIZED) {
		return MqttClient::Reason::Unauthorized;
	}

	return MqttClient::Reason::Unknown;
}

// HANDLERS

void MqttClient::Client::handleMessageReceived(const char *topic, byte *payloadData, unsigned int length) {

	payloadData[length] = '\0';

	String topicString = String(topic);
	String payloadString = String((char *) payloadData);

	MqttClient::Callbacks::MessageReceivedCallback callback = this->m_subscribers[topicString];

	if (callback) {
		callback(topicString, payloadString);
	} else {
		if (this->m_onMessageReceived) {
			this->m_onMessageReceived(topicString, payloadString);
		}
	}
}

void MqttClient::Client::handleConnectionEstablished() {

	this->m_wasConnected = true;

	if (this->m_onConnected) {
		this->m_onConnected();
	}
}

void MqttClient::Client::handleConnectionLost() {

	this->m_wasConnected = false;

	if (this->m_onDisconnected) {
		Reason reason = this->getDisconnectReason();

		this->m_onDisconnected(reason);
	}
}


