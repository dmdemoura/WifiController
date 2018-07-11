//Compile with -lX11 -lXtst

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
//Libraries for networking
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
//X11 libraries
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

const char* DefaultPort = "5050";
//Button IDs, sent by controller
enum Buttons
{
    Power = 0,
    Left  = 1,
    Right = 2,
    Down  = 3,
    Up    = 4,
    A     = 5,
    One   = 6,
    Two   = 7
};
//Key Codes, used for X server
//Use xev command to find
enum Keys
{
    Enter      = 36,
    ArrowUp    = 111,
    ArrowLeft  = 113,
    ArrowRight = 114,
    ArrowDown  = 116,
    KeyZ       = 52,
    KeyX       = 53,
    KeyC       = 54
};
//Maps button ids to key codes
const KeyCode buttonKeyMap[8] =
{
    [Power] = Enter,
    [Left]  = ArrowLeft,
    [Right] = ArrowRight,
    [Down]  = ArrowDown,
    [Up]    = ArrowUp,
    [A]     = KeyC,
    [One]   = KeyZ,
    [Two]   = KeyX
};
//Fills a addrinfo struct from an IP address and a port
struct addrinfo* getHostAddr(char *address, char* port)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints)); //Set the struct to zeroes
    hints.ai_family = AF_UNSPEC; //IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; //TCP
    hints.ai_flags = AI_PASSIVE; //Auto fill my IP
    
    int status;
    struct addrinfo *servinfo;//Results go here
    if ((status = getaddrinfo(address, port, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
    return servinfo;
}
//Opens a socket with an addrinfo
int openSocket(struct addrinfo *servinfo)
{
    int sockfd;
    sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    char on = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    return sockfd;
}

int main(int argc, char** argv)
{
	//Checks if argument count is valid
    if (argc != 2 && argc != 3)
    {
        printf("Usage: wifiController IP [PORT]\n");
        return 1;
    }
    char* ip = argv[1];
    char* port;
	//Uses the default port number if not supplied
    if (argc == 3)
        port = argv[2];
    else
        port = (char*) DefaultPort;

	//Setups a socket
    struct addrinfo *servinfo = getHostAddr(ip, port);
    int socket = openSocket(servinfo);
    freeaddrinfo(servinfo);

    //Tries to connect to controller
    if (connect(socket, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        printf("error: %d\n", errno);
        return -1;
    }

    //Tries to open the default display
    Display* display = XOpenDisplay(NULL);  //NULL is the default display
    if (!display)
    {
        printf("Couldn't open display\n");
        return 1;
    }

    while (true)
    {
        printf("Waiting.. \n");

        //Receive button events
        uint8_t button;
        recv(socket, &button, sizeof(button), 0);

        uint8_t state;
        recv(socket, &state, sizeof(state), 0);

        printf("Button %d: %d\n\n", button, state);
        
        //Use the XTest library to simulate a key event
        XTestFakeKeyEvent(display, buttonKeyMap[button], state, 0);
        //Send the simulated events
        XFlush(display);
    }

    return 0;
}