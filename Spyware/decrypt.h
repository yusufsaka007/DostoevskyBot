#ifndef DECRYPT_H
#define DECRYPT_H

#include <iostream>
#include <string>
#include <cstring>

std::string decrypt_ip(const char* __ip) {
	int keySub = 0x0A;
	int keyXor = 0x0C;
	int len = std::strlen(__ip);
	std::string result;

    for (int i = 0; i < len; i++) {
        result += static_cast<char>((__ip[i] ^ keyXor) - keySub);
    }

    return result;
}

int decrypt_port(const int __port) {
    int key = 0x1234;
    int port = __port ^ key;
    port -= 0x0A;
    return port;
}

#endif // !DECRYPT_H
