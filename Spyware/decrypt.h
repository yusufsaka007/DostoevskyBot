#ifndef DECRYPT_H
#define DECRYPT_H

#include <iostream>
#include <string>
#include <cstring>

std::string cqzcuzswcstfa3r(const char* __ip) { // decrypt ip
	int keySub = 0x0A;
	int keyXor = 0x0C;
	int len = std::strlen(__ip);
	std::string result;

    for (int i = 0; i < len; i++) {
        result += static_cast<char>((__ip[i] ^ keyXor) - keySub);
    }

    return result;
}

int gj7eaodc0g10rbk(const int __port) { // decrypt port
    int key = 0x1234;
    int port = __port ^ key;
    port -= 0x0A;
    return port;
}

#endif // !DECRYPT_H
