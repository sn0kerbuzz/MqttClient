# MqttClient

A simple way to handle publish/subscribe to an Mqtt broker.

##Features

1. **Easy handle client connection.** This library includes an internal loop method that handles the connection. The client
exposes 2 callbacks to notify connection state changes. For more details check this
[example](https://github.com/sn0kerbuzz/MqttClient/blob/master/examples/example1.cpp).
 
2. **Unique callback for each subscribe topic.** When subscribing to a topic you can
specify a unique callback for that topic and when a message is received only that
callback will be called, thus enabling you to do different things when a message is
published without filtering the topic each time. The library manages that for you.

##Dependencies
This library depends on [PubSubClient](https://github.com/knolleary/pubsubclient).
