// SerialIO.cpp

#include "SerialIO.hpp"
#include <fcntl.h> // for open()
#include <unistd.h> // for write(), read(), and close()
#include <termios.h> // for tcgetattr(), termios struct, cfsetispeed(), cfsetospeed()
#include <errno.h> // for errno
#include <chrono> // for getSteadyClockTimestampMs().count()
#include <iostream>

using namespace serial;

static const float WARMUP_DURATION = 200.0;

SerialIO::SerialIO() {
	fileDesc = SERIAL_UNAVAILABLE;
	// Add a null character at the end for c strings
	buffer = (char*) calloc(MAX_LENGTH + 1, sizeof(char));
}

SerialIO::~SerialIO() {
	free(buffer);
	if (fileDesc != SERIAL_UNAVAILABLE) close(fileDesc);
}

/**
 * Opens a connection to the serial port defined. Adapted from
 * https://github.com/kleydon/Mac-SerialPort-Cpp
 *
 * @param portPath The path to the port, like "/dev/cu.usbmodem144201".
 * @param rate The rate for the port. This is typically 9600.
 */
void SerialIO::openPort(const char* portPath, int rate) {
	if (fileDesc != SERIAL_UNAVAILABLE) close(fileDesc);

	// Open the port with read/write, non-controlling, make it non-blocking
	// even if nothing is coming from the port yet.
	fileDesc = open(portPath, (O_RDWR | O_NOCTTY | O_NDELAY));
	if (fileDesc == SERIAL_UNAVAILABLE) {
		throw SerialException("openPort() failed to open serial port");
	}

	// Hold settings for the terminal interface
	struct termios options;
	tcgetattr(fileDesc, &options);
	cfsetispeed(&options, rate);
	cfsetospeed(&options, rate);

	// Configure other settings. Source:
	// https://github.com/Marzac/rs232/blob/master/rs232-linux.c
	options.c_iflag &= ~(INLCR | ICRNL);
	options.c_iflag |= IGNPAR | IGNBRK;
	options.c_oflag &= ~(OPOST | ONLCR | OCRNL);
	options.c_cflag &= ~(PARENB | PARODD | CSTOPB | CSIZE | CRTSCTS);
	options.c_cflag |= CLOCAL | CREAD | CS8;
	options.c_lflag &= ~(ICANON | ISIG | ECHO);
	options.c_cc[VTIME] = 1;
	options.c_cc[VMIN] = 0;

	if (tcsetattr(fileDesc, TCSANOW, &options) < 0) {
		throw SerialException("Failed to set attributes for serial port in openPort()");
	}
}

std::chrono::milliseconds getTime() {
	using namespace std::chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
}

void SerialIO::warmUp() {
	std::chrono::milliseconds startTime = getTime();
	while (getTime().count() - startTime.count() < WARMUP_DURATION) {
		readLine();
	}
}

std::string SerialIO::readLine() {
	if (fileDesc == SERIAL_UNAVAILABLE) {
		throw SerialException("Cannot read line from a serial port that has not been opened. Call openPort() first");
	}

	ssize_t bufferStart = bufferStop;
	bool foundNewLine = false;
	std::string line = "";

	// Keep reading until we have a line
	while (!foundNewLine) {
		// Find the \n character
		while (bufferStop < bufferLen && buffer[bufferStop] != '\n') {
			bufferStop++;
		}
	
		foundNewLine = (buffer[bufferStop] == '\n');
	
		buffer[bufferStop] = '\0'; // null terminated
		char* lineCStr = &buffer[bufferStart];
		std::string nextPart(lineCStr);
		line += nextPart;
	
		// Move pointer forward for next readLine()
		bufferStop++;

		// If we did not find \n, read next line
		if (!foundNewLine) {
			bufferLen = read(fileDesc, buffer, MAX_LENGTH);
			bufferStart = 0, bufferStop = 0;
	
			if (bufferLen < 0) {
				throw SerialException(strerror(errno));
			}
		}
	}

	return line;
}
