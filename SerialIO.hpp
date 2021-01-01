// SerialIO.hpp

#include <unistd.h> // for ssize_t
#include <string>
#include <exception>

namespace serial {
	static const int SERIAL_UNAVAILABLE = -1;

	class SerialIO {
	public:
		SerialIO();
		~SerialIO();
		void openPort(const char*, int);
		void warmUp();
		std::string readLine();
	private:
		int fileDesc;
		char* buffer;
		ssize_t bufferLen = 0;
		ssize_t bufferStop = 0;
		static const int MAX_LENGTH = 10000;
	};

	class SerialException : public std::exception {
	public:
		SerialException(const std::string& message) : msg(message) {};
		~SerialException() {};
		const char* what() const throw() { return msg.c_str(); }
	private:
		std::string msg;
	};
}
