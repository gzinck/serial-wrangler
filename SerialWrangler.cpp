//
// Graeme Zinck, 2020
//
// SerialWrangler.cpp for processing and retransmitting serial data
// from an arduino over websockets.
//

#include <string>
#include <iostream>
#include <json/value.h>
#include "SerialIO.hpp"
#include "Server.hpp"

static const int SERVE_PORT = 4567;

void getData(std::string portPath, server::Server* server) {
	serial::SerialIO io;
	io.openPort(portPath.c_str(), 9600);
	io.warmUp();

	while (true) {
		Json::Value val;
		int i = 0;
		std::string line = io.readLine();
		std::string::size_type start = 0;
		std::string::size_type end = line.find(' ');
		while (end != std::string::npos) {
			val["Force" + std::to_string(i++)] = line.substr(start, end - start);
			start = end + 1;
			end = line.find(' ', start);
		}

		if (start < line.length()) {
			val["Force" + std::to_string(i++)] = line.substr(start);
		}

		*server << val;
	}
}

int main(int argc, char* argv[]) {
	std::string portPath;
	if (argc > 1) portPath = argv[1];
	else {
		std::cout << "Usage: SerialWrangler [serial port path] [websocket port (optional, default: 4567)]" << std::endl;
		return -1;
	}

	// Set up the server to share our data
	int servePort;
	if (argc > 2) servePort = atoi(argv[2]);
	else servePort = SERVE_PORT;

	server::Server serv;
	websocketpp::lib::thread t(websocketpp::lib::bind(&getData, portPath, &serv));

	serv.run(SERVE_PORT);

	t.join();

	return 0;
}
