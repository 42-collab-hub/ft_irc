# ft_irc
_This project has been created by [mglikenf](https://github.com/mglikenf) and [Gwendaaaaal](https://github.com/Gwendaaaaal) as part of the 42 School curriculum._

## Description
This project implements a fully functional IRC server that follows the IRC protocol (RFC 1459).
This project does not implement server-to-server communication, nor does it require developing its own IRC client. Instead, it instructs using any existing IRC client as reference.
Our client of choice is [irssi](https://irssi.org/).
The server can handle multiple client connections simultaneously.

## Project Requirements
- All I/O operations must be non-blocking
- Communication via TCP/IP
- Client-Server communication must be similar to any official IRC server. However, subject guidelines imply the following set of mandatory commands: `PASS`, `NICK`, `USER`, `JOIN`, `PRIVMSG`, `PART`, `TOPIC`, `KICK`, `INVITE`, `MODE`, `QUIT`

## Technical Requirements
- Language: Everything in C++98 and a set of specific C functions
- Non-blocking I/O using `poll()` for multiplexing
- Compilation flags `-Wall -Wextra -Werror -std=c++98`

## Compilation
```
make
```
### clean
```
make clean   # Remove object files
make fclean  # Remove object files and executables
make re      # Rebuild everything
```
## Usage
### Starting the server
```
./ircserv <port> <password>
# Example: ./ircserv 6667 mypassword
```
### Connecting to server
You can connect using an IRC client (out reference client is **irssi**) or netcat:
```
nc -C localhost 6667
```
Then authenticate:
```
PASS mypassword
NICK mynickname
USER myusername 0 * :Real Name
```

## Resources
- [RFC 1459 - Internet Relay Chat Protocol](https://www.rfc-editor.org/rfc/rfc1459.html)
- [RFS 2812 - IRC Client Protocol](https://www.rfc-editor.org/rfc/rfc2812)
- [Numeric Replies](https://modern.ircdocs.horse/index.html#numerics)
