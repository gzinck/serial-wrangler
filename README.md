# SerialWrangler

SerialWrangler is a simple C++ application for taking in input from an
Arduino (or other device) using a serial port and sending the data using
WebSockets.

Right now, ViconWrangler is under development---use at your own risk.
It currently only has the libraries for **macOS** support.

## Compiling and Running SerialWrangler

1. Run `make`.
2. `cd` into `build` and run `./SerialWrangler [SERIAL PORT PATH] [WEBSOCKET PORT]`, where:
	- `[SERIAL PORT PATH]` is the path of the serial port to use. Find the
	  port using `ls /dev/cu.*`. Example path: `/dev/cu.usbmodem142201`.
	- `[WEBSOCKET_PORT]` is the port the Vicon data is transmitted on using
	  websockets. Defaults to 4567.

## Testing SerialWrangler

The easiest way to see what's coming out of SerialWrangler is to use
`websocat`. You can install it with `brew install websocat` on macOS.
Then, while running SerialWrangler in a separate terminal window, run
`websocat ws://localhost:4567` to see the output.
