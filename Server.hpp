// Server.h
#include <string.h>
#include <iostream>
#include <set>
#include <json/writer.h>
#include <exception>

// Include websocketpp
// The ASIO_STANDALONE define is necessary to use the standalone version of Asio.
#define ASIO_STANDALONE
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace server {
	typedef websocketpp::server<websocketpp::config::asio> sock_serv;
	typedef std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> con_list;

	class Server {
	public:
		Server();
		virtual ~Server() {};
		/**
		 * Runs the server on the provided port. Note that this operation
		 * is blocking; consider running this on a separate thread if you
		 * need background processes.
		 *
		 * @param port The port number.
		 * @returns True iff no errors occurred.
		 */
		bool run(int port);
		/**
		 * Sends a JSON string to all subscribed entities.
		 */
		const Server& operator << (const Json::Value&);
	private:
		/**
		 * Callback when a new connection to the server is opened.
		 */
		void onOpen(websocketpp::connection_hdl hdl);
		/**
		 * Callback when a message is received by the server.
		 */
		void onMessage(websocketpp::connection_hdl, sock_serv::message_ptr);
		/**
		 * Callback when a connection to the server is closed.
		 */
		void onClose(websocketpp::connection_hdl hdl);
		/**
		 * Callback when a connection to the server fails.
		 */
		void onFail(websocketpp::connection_hdl hdl);

		/**
		 * The set of connections attached to the server.
		 */
		con_list connections;
		/**
		 * The websocketpp endpoint used for sending/receiving messages.
		 */
		sock_serv endpoint;

		/**
		 * A mutex to ensure safe access to the set of connections attached
		 * to the server.
		 */
		websocketpp::lib::mutex connection_lock;
	};

	class ServerException : public std::exception {
	public:
		ServerException(const std::string& message) : msg(message) {};
		~ServerException() {};
		const char* what() const throw() { return msg.c_str(); }
	private:
		std::string msg;
	};
}
