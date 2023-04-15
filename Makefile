all: clear_bin
	mkdir -p ./bin                              # Create folder for binaries
	gcc client.c -o ./bin/client -Wall &        # Compile client
	gcc server.c -o ./bin/server -Wall -pthread # Compile server
	wait                                        # Wait for both client and server to compile

clear_bin:
	rm -rf ./bin				    # Remove binaries