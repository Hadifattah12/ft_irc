# ft_irc

## 🧠 Description

`ft_irc` is a lightweight IRC (Internet Relay Chat) server written in C++98 as part of the 42 Network curriculum.
This project involves creating a server that mimics the behavior of a real IRC server, enabling multiple clients
to connect, communicate through channels, and use essential IRC commands.

## 🚀 Features

- Handles **multiple simultaneous clients** using `poll()` (non-blocking I/O).
- Clients can:
  - **Authenticate** using a password.
  - Set their **nickname** and **username**.
  - **Join** and **communicate** through IRC channels.
  - **Send and receive private messages**.
- Channel support:
  - Broadcast messages to all users in a channel.
  - Channel **modes** (i, t, k, o, l).
  - **Operators** and **regular users**.
  - Channel commands: `JOIN`, `PART`, `TOPIC`, `INVITE`, `KICK`, `MODE`.

## 🛠️ Installation

```bash
git clone https://github.com/your-username/ft_irc.git
cd ft_irc
make
💡 Usage
Start the server:

./ircserv <port> <password>
port: Port number to listen on (e.g. 6667)

password: The password clients must use to connect

./ircserv 6667 1337pass
🧪 Testing
You can test the server using an IRC client like:

HexChat


Or even simple testing with nc:


nc -C 127.0.0.1 6667

✅ Implemented Commands

Command	Description
PASS	Authenticate with server password
NICK	Set/change nickname
USER	Set username
JOIN	Join a channel
PART	Leave a channel
PRIVMSG	Send a private message to a user or channel
TOPIC	View or change the channel topic
KICK	Kick a user from a channel (operator only)
INVITE	Invite a user to a channel (operator only)
MODE	Change channel modes (operator only)
🧱 Architecture
Server: Manages connections, clients, and channels.

Client: Represents a connected user.

Channel: Manages members, topic, and modes.

Commands: Command parsing and execution.

Uses poll() for non-blocking I/O.

Only one poll() loop is used to manage all operations.

🧼 Code Style
Fully compatible with C++98

No external libraries

Clean, modular, and readable

🧠 Project Constraints
No client implementation.

No server-to-server communication.

No forking or multithreading.

Only one poll() loop allowed.

Fully non-blocking I/O.

Must behave similarly to a real IRC server.
