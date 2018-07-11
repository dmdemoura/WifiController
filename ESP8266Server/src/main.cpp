#include <ESP8266WiFi.h>

//Wifi Status
enum CommState { offline, waiting, connected };

CommState commState = offline;
WiFiClient client;
WiFiServer server(5050);

//Maps button numbers to pins numbers
const int buttonPinMap[8] =
{
    2,
    5,
    13,
    4,
    0,
    12,
    3,
    14,
};

//Keeps the current button state, used for detecting key presses and releases
bool buttonState[8] = {};

void setup()
{
    Serial.begin(9600);
    Serial.println();

    //Creates an wifi access point
    bool wifiOnline = WiFi.softAP("WifiController11", "projeto11");

    if (wifiOnline)
    {
        Serial.println("Wifi online.");
        //Starts a TCP server
        server.begin();
        //Disables the server from waiting until there is a significant amount of data to send
        server.setNoDelay(true);
        commState = waiting;
    }
    else
    {
        Serial.println("Wifi setup failed!");
    }

    //Enables the needed pins with the internal pullup resitor enabled
    pinMode(5, INPUT_PULLUP);
    pinMode(4, INPUT_PULLUP);
    pinMode(0, INPUT_PULLUP);
    pinMode(2, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
    pinMode(12, INPUT_PULLUP);
    pinMode(13, INPUT_PULLUP);
    pinMode(3, INPUT_PULLUP);
}
void loop()
{
    //Waiting for client
    if (commState == waiting)
    {
        //Checks if there's a client trying to connect
        client = server.available();
        if (client)
        {
            Serial.println("Got client");
            commState = connected;
        }
    }
    //Connected to client
    else if (commState == connected)
    {
        //For all buttons, check if state has changed
        for (int i = 0; i < 8; i++)
        {
            //Only send events if state has changed
            if (!digitalRead(buttonPinMap[i]) != buttonState[i]) //Pins are HIGH when released
            {
                buttonState[i] = !buttonState[i];       //Updates button state

                //Send button event
                client.write((uint8_t) i);              //Button ID
                client.write((uint8_t) buttonState[i]); //Button state

                Serial.printf("Button %d: %d\n", i, buttonState[i]);
            }
        }
    }
    //Returns control to the "operating system", so the wifi and networking stack doesn't lock up.
    yield();
}