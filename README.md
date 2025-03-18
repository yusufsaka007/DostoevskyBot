# DostoevskyBot

DostoevskyBot is an automated spyware tool that utilizes a Telegram bot to accept specific commands from the user and receive screenshots from a Windows system. This project is still in development and should not be used for illegal activities or any form of unethical monitoring.

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Disclaimer](#disclaimer)

## Introduction
DostoevskyBot is a project aimed at creating a Python server and a Telegram bot that interact to provide remote monitoring capabilities. The primary functionality includes accepting commands via Telegram and capturing screenshots from a Windows machine using C++.

## Features
- **Python Server**: Handles communication between the Telegram bot and the Windows system.
- **Telegram Bot**: Accepts specific commands from the user.
- **Screenshot Capture**: Captures screenshots from the Windows system using C++.

## Installation
To set up DostoevskyBot, follow these steps:

1. Clone the repository:
    ```sh
    git clone https://github.com/yusufsaka007/DostoevskyBot.git
    cd DostoevskyBot
    ```

2. Install the required Python packages:
    ```sh
    pip3 install python-telegram-bot
    ```

3. Set up the Telegram bot:
    - Create a new Telegram bot and obtain the API token from [BotFather](https://core.telegram.org/bots#botfather).
    - Create a `token.txt` file and add your API token:

4. Build the C++ Spyware:
    - This Spywware will use an encrypted IP and Port. You can use the encrypt.py to get the encrypted IP and Port. 
    Change manually:
    ```sh
    python3 encrypt.py <ip> <port>
    ```
    - Change automatically (Searches for client.h and write the file with the appropriate values):
    ```sh
    python3 encrypt.py <ip> <port> -c
    ```
    - Ensure the compiled executable is accessible to the Python server.

6. Add your telegram Id to user_ids.txt. You can find out your Id by using [userinfobot](https://telegram.me/userinfobot)

## Usage
1. Start the Python server:
    ```sh
    python3 trigger.py <ip> <port>
    ```

2. Interact with the Telegram bot using the predefined commands to capture screenshots and perform other monitoring tasks.

## Contributing
We welcome contributions to enhance the functionality and security of DostoevskyBot. Please fork the repository and create a pull request with your improvements. I would like to add my own user GUI for mobile applications. If you would like to contribute to this project by building a simple user interface you may contact me.

- **@yusufsaka007 on X.com**
- **yusufsaka5481@gmail.com**
  
## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Disclaimer
**DostoevskyBot is intended for educational and ethical purposes only.**

- **Do not use this tool for illegal activities or unauthorized monitoring.**
- The developers are not responsible for any misuse or damage caused by this tool.
- This project is still in development, and its functionality may change over time.

By using DostoevskyBot, you agree to comply with all applicable laws and regulations.
