//
// Created by Andrei Balasescu on 07/04/2020.
//

#pragma once

#include <functional>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

namespace MqttClient {
	enum class Reason {
		Unknown = 0,
		ConnectionTimeout = 1,
		ConnectionLost = 2,
		ConnectionFailed = 3,
		BadProtocol = 4,
		BadClientId = 5,
		BadCredentials = 6,
		Unavailable = 7,
		Unauthorized = 8
	};

	namespace Callbacks {
		typedef std::function<void()> ConnectedCallback;
		typedef std::function<void(Reason)> DisconnectedCallback;
		typedef std::function<void(String, String)> MessageReceivedCallback;
	}

	class Client {
	public:
		// LIFECYCLE
		Client();

		void loop();

		// GENERAL
		void publish(const String& topic, const String& payload, bool retain = false);
		void subscribe(const String& topic);
		void unsubscribe(const String& topic);

		// CONNECTION
		void beginConnect();
		bool connect();
		void disconnect();

		// GETTERS
		bool isConnected();

		// SETTERS
		void setConnectedCallback(Callbacks::ConnectedCallback onConnected);
		void setDisconnectedCallback(Callbacks::DisconnectedCallback onDisconnected);
		void setMessageReceivedCallback(Callbacks::MessageReceivedCallback onMessageReceived);

		void setClientName(const String& clientName);
		void setServer(const IPAddress& server, unsigned port = 1883);
		void setCredentials(const String& user, const String& password);
		void setWill(const String& willTopic, const String& willPayload);

	private:
		// CALLBACKS
		Callbacks::ConnectedCallback m_onConnected;
		Callbacks::DisconnectedCallback m_onDisconnected;
		Callbacks::MessageReceivedCallback m_onMessageReceived;

		// HANDLERS
		void handleMessageReceived(const char* topic, byte* payloadData, unsigned int length);

		void handleConnectionEstablished();
		void handleConnectionLost();

		// GLOBALS
		WiFiClient m_wifiClient;
		PubSubClient m_client;

		IPAddress m_serverIp;
		unsigned m_port { 1883 };

		String m_clientName;
		String m_user;
		String m_password;

		String m_willTopic;
		String m_willPayload;

		bool m_tryConnection { false };
		bool m_wasConnected { false };

		Reason getDisconnectReason();
	};
}
