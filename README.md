# WifiController
This is a simple demo of an ESP8266 detecting key presses (and releases) from tactile switches, sending them over the network to a client program on a Linux computer (although, it should work in anything with POSIX sockets and X11), which then simulates keypresses.

## ESP8266 Server
The ESP8266 by the default runs an WiFi Access Point:
```
SSID:     WifiController11
password: projeto11
```
It could be changed without much trouble to connect to an existing network.

It runs a TCP server, by default on port 5050, which the Linux client can connect to. Currently it only supports a single client, and does NOT handle client disconnection. Which means, if you disconnect the client and want to reconnect you have to reset the ESP8266. (Which usually won't cut the computers wifi connection to the ESP.)

The server code uses Arduino libraries for the ESP8266, and was tested using PlatformIO for VS Code. So I would suggest using it, for compiling and uploading, because it downloads the SDKs and needed libraries automatically. It is possible to just open the ESP8266Server folder as a project.

[PlatformIO Getting Started Guide](http://docs.platformio.org/en/latest/ide/vscode.html#ide-vscode)

[ESP8266 Arduino Core Documentation](https://arduino-esp8266.readthedocs.io/en/latest/)

## Linux Client
It is a command line application that connects to the ESP8266 and then listens for key events, using X11's XTest library to fake them in the computer.

Command syntax:
```
./wifiController ESP-IP-Address Port
```
The port is optional, and the default 5050 will be used if one is supplied.

It can be compiled using the supplied makefile, by running:
```
make
```
or by using:
```
gcc main.c -o wifiController -lX11 -lXtst
```
XTest can do quite a lot more than only keypresses.
So if you're interested check out it's documentation.
[XTest Documentation](https://www.x.org/releases/X11R7.7/doc/libXtst/xtestlib.html)
