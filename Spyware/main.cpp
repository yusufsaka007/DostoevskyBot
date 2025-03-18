#include "client.h"
#include <csignal>
#include "decrypt.h"

Client* client = nullptr;

// SIGINT handler
BOOL WINAPI console_handler(DWORD signal) {
	if (signal == CTRL_C_EVENT) {
		std::cout << "Ctrl-C received" << std::endl;
		delete client;
		exit(signal);
	}
	return TRUE;
}

int main() {
	// Set the console handler. TEST PURPOSES WHEN DEBUGGING
	if (!SetConsoleCtrlHandler(console_handler, TRUE)) {
		std::cerr << "Error: Could not set control handler" << std::endl;
		return 1;
	}

	client = new Client();
	
	delete client;


	return 0;
}