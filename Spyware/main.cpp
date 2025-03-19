#include "client.h"
#include <csignal>
#include "startup.h"

Client* client = nullptr;


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	EditRegistry();
	client = new Client();

	delete client;

	return 0;
}