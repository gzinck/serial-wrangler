#include "Server.hpp"
#include <json/reader.h>

// Use namespace for the mutex to ensure threads don't all try
// to access the list of connections at the same time.
using websocketpp::lib::mutex;
using websocketpp::lib::lock_guard;

using namespace server;

void Server::onOpen(websocketpp::connection_hdl hdl) {
	lock_guard<mutex> guard(connection_lock);
	connections.insert(hdl);
	// endpoint.send(hdl, "Connection successful!", websocketpp::frame::opcode::text);
	std::cout << "Connection opened" << std::endl;
}

void Server::onMessage(websocketpp::connection_hdl hdl, sock_serv::message_ptr msgPtr) {
//	std::cout << "Message received" << std::endl;
//	std::string msgStr = msgPtr->get_payload();
//
//	std::stringstream sstr(msgStr);
//	Json::Value msg;
//	sstr >> msg;
//
//	*this << messageHandler.handleMessage(msg);
}

void Server::onClose(websocketpp::connection_hdl hdl) {
	lock_guard<mutex> guard(connection_lock);
	connections.erase(hdl);
	std::cout << "Connection closed" << std::endl;
}

void Server::onFail(websocketpp::connection_hdl hdl) {
	lock_guard<mutex> guard(connection_lock);
	connections.erase(hdl);
	std::cout << "Connection failed and closed" << std::endl;
}

Server::Server() {
	// Set up logging
	endpoint.set_error_channels(websocketpp::log::elevel::all);
	endpoint.set_access_channels(websocketpp::log::alevel::all ^ websocketpp::log::alevel::frame_payload);
	
	// Initialize
	endpoint.init_asio();

	using websocketpp::lib::placeholders::_1;
	using websocketpp::lib::placeholders::_2;
	using websocketpp::lib::bind;

	endpoint.set_open_handler(bind(&Server::onOpen, this, _1));
	endpoint.set_message_handler(bind(&Server::onMessage, this, _1, _2));
	endpoint.set_close_handler(bind(&Server::onClose, this, _1));
	endpoint.set_fail_handler(bind(&Server::onFail, this, _1));

	endpoint.set_reuse_addr(true);
}

bool Server::run(int port) {
	try {
		endpoint.listen(port);
	} catch (websocketpp::exception const &e) {
		std::cout << "Error listening on port " << port << ": " << e.what() << std::endl;
		return false;
	}

	websocketpp::lib::error_code ec;
	endpoint.start_accept();
	if (ec) {
		std::cout << "Error starting to accept connections: " << ec.message() << std::endl;
		return false;
	}

	try {
		endpoint.run();
	} catch (websocketpp::exception const &e) {
		std::cout << "Error running server: " << e.what() << std::endl;
		return false;
	}

	return true;
}

const Server& Server::operator << (const Json::Value& msg) {
	Json::StreamWriterBuilder builder;
	builder["commentStyle"] = "None";
	builder["indentation"] = "";
	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	std::ostringstream os;
	writer->write(msg, &os);
	std::string s = os.str();

	lock_guard<mutex> guard(connection_lock);
	for (websocketpp::connection_hdl hdl : connections) {
		try {
			endpoint.send(hdl, s, websocketpp::frame::opcode::text);
		} catch (websocketpp::exception const &e) {
			std::cout << "Error sending message: " << e.what() << std::endl;
		}
	}

	return *this;
}
