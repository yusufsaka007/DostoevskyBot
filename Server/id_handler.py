import random
import os

characters = [str(i) for i in range(0,10)] + [chr(i) for i in range(97, 123)]

ID_PATH = "./id_log.txt"
LENGTH = 16
CHARS_LENGTH = len(characters)

def print_id_log():
    if(os.path.exists(ID_PATH) == False):
        with open(ID_PATH, "w") as f:
            f.write("")

    with open(ID_PATH, "r") as f:
        lines = f.readlines()
        print("\nContent of the id.txt\n\n")
        for line in lines:
            print(f"{line.strip().split(':'[0])} ---> {line.strip().split(':')[1]}")
            
def generate_random_id():
    generated_id = ""
    for i in range(LENGTH-1):
        generated_id += characters[random.randint(0, CHARS_LENGTH-1)]
    generated_id += "\0"
    return generated_id

def find_key_to_modify(__id_target_db, __target):
    key_to_modify = None 
    for key, value in __id_target_db.items():
        if value == __target:
            key_to_modify = key
            break
    if key_to_modify is None:  
         return False
    return key_to_modify

def modify_id(__id_target_db, __target):
    key_to_modify = find_key_to_modify(__id_target_db, __target)

    new_id = generate_random_id()
    # Remove the original pair
    __id_target_db.pop(key_to_modify)
    # Add the new pair
    __id_target_db[new_id] = __target
    # Write the changes in to the file
    with open(ID_PATH, "r") as f:
        lines = f.readlines()
    with open(ID_PATH, "w") as f:
        for line in lines:
            line_split = line.strip().split(':')
            if line_split[1] == __target:
                f.write(f"{new_id}:{__target}\n")
            else:
                f.write(line)
    return True

def verify_id(__id, __id_target_db):
    if len(__id) != LENGTH:
        print("Length of the received ID is not correct")
        return False
    if __id not in __id_target_db.keys():
        print("ID is not in the database")
        return False
    return __id_target_db[__id]

def add_id(__id_target_db, __target, __id):
    __id_target_db[__id] = __target
    with open(ID_PATH, "a") as f:
        f.write(f"{__id}:{__target}\n")

def remove_id(all=False, __id_target_db=None, __target=None):
    if all:
        with open(ID_PATH, "w") as f:
            f.write("")
    else:
        if __target is None:
            print("target is not provided")
            return False
        if __target not in __id_target_db.values():
            print("target is not in the database")
            return False
        
        key_to_modify = find_key_to_modify(__id_target_db, __target)
        __id_target_db.pop(key_to_modify)

        with open(ID_PATH, "r") as f:
            lines = f.readlines()
        
        with open(ID_PATH, "w") as f:
            for line in lines:
                line_split = line.strip().split(':')
                if line_split[1] != __target:
                    f.write(line)
    return True

def read_ids(__id_target_db):
    try:
        with open(ID_PATH, "r") as f:
            lines = f.readlines()
            for line in lines:
                line_split = line.strip().split(':')
                __id_target_db[line_split[0]] = line_split[1]
    except FileNotFoundError:
        with open(ID_PATH, "w") as f:
            f.write("")