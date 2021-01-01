//
// Graeme Zinck, 2020
//
// SerialWrangler.cpp for processing and retransmitting serial data
// from an arduino over websockets.
//

#include <string>
#include <iostream>
#include "SerialIO.hpp"
#include "Server.hpp"

static const int SERVE_PORT = 4567;

void getData(std::string portPath, server::Server* server) {
	serial::SerialIO io;
	io.openPort(portPath.c_str(), 9600);
	io.warmUp();
	while (true) *server << io.readLine();
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
