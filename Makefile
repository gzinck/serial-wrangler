CC=g++

INCLUDE_DIR=./include
OUT_DIR=./build
OBJ_DIR=./obj

# Include vicon and websocketpp when compiling
CPP_FLAGS=-std=c++11 \
		  -I$(INCLUDE_DIR)/jsoncpp/dist \
		  -I$(INCLUDE_DIR)/websocketpp \
		  -I$(INCLUDE_DIR)/asio/asio/include \

.PHONY: all
all: $(OUT_DIR)/SerialWrangler

$(OUT_DIR)/SerialWrangler: $(OBJ_DIR)/SerialWrangler.o $(OBJ_DIR)/SerialIO.o $(OBJ_DIR)/Server.o $(OBJ_DIR)/jsoncpp.o | $(OUT_DIR)
	$(CC) $(OBJ_DIR)/SerialWrangler.o \
		$(OBJ_DIR)/SerialIO.o \
		$(OBJ_DIR)/Server.o \
		$(OBJ_DIR)/jsoncpp.o \
		$(CPP_FLAGS) \
		-o $(OUT_DIR)/SerialWrangler

$(OBJ_DIR)/SerialWrangler.o: SerialWrangler.cpp SerialIO.hpp Server.hpp | $(OBJ_DIR) $(INCLUDE_DIR)/websocketpp $(INCLUDE_DIR)/asio
	$(CC) SerialWrangler.cpp $(CPP_FLAGS) \
		-c -o $(OBJ_DIR)/SerialWrangler.o

$(OBJ_DIR)/SerialIO.o: SerialIO.cpp SerialIO.hpp | $(OBJ_DIR)
	$(CC) SerialIO.cpp $(CPP_FLAGS) \
		-c -o $(OBJ_DIR)/SerialIO.o

$(OBJ_DIR)/Server.o: Server.cpp Server.hpp | $(OBJ_DIR) $(INCLUDE_DIR)/websocketpp $(INCLUDE_DIR)/jsoncpp $(INCLUDE_DIR)/asio
	$(CC) Server.cpp $(CPP_FLAGS) \
		-c -o $(OBJ_DIR)/Server.o

$(OBJ_DIR)/jsoncpp.o: $(INCLUDE_DIR)/jsoncpp/dist/jsoncpp.cpp | $(INCLUDE_DIR)/jsoncpp $(OBJ_DIR)
	$(CC) $(INCLUDE_DIR)/jsoncpp/dist/jsoncpp.cpp \
		$(CPP_FLAGS) \
		-c -o $(OBJ_DIR)/jsoncpp.o

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

$(INCLUDE_DIR)/jsoncpp: | $(INCLUDE_DIR)
	cd $(INCLUDE_DIR) && git clone https://github.com/open-source-parsers/jsoncpp.git
	cd $(INCLUDE_DIR)/jsoncpp && python amalgamate.py

$(INCLUDE_DIR)/websocketpp: | $(INCLUDE_DIR)
	cd $(INCLUDE_DIR) && git clone https://github.com/zaphoyd/websocketpp.git

$(INCLUDE_DIR)/asio: | $(INCLUDE_DIR)
	cd $(INCLUDE_DIR) && git clone https://github.com/chriskohlhoff/asio.git

$(INCLUDE_DIR):
	mkdir -p $(INCLUDE_DIR)

.PHONY: clean
clean:
	rm -r $(OUT_DIR) $(OBJ_DIR)

