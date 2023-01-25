#include <SparkFunMLX90614.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <Wire.h> //Library for I2C Communication
#include <SparkFunMLX90614.h>  //Library for communication with MLX90614 IR Sensor

//Object responsible for communication with the infrared sensor
IRTherm sensor;

//Variable that will store the read temperature of the Person
float tempPerson;

//Variables for storing the Information about incoming person
String Name = "";
String Address = "";  
String Contact = "";

//Specify digital pin on the Arduino that the positive lead of piezo buzzer is attached.
int piezoPin = D7;

const char* ssid = "Aakash";       //The name of your Local Wifi
const char* password = "mehta0000"; //Password of your Local Wifi
//WiFiClient client;
char server[] = "192.168.1.105";   //eg: 192.168.0.222

WiFiClientSecure client;    

const char* host = "script.google.com";
const int httpsPort = 443;
String GAS_ID = "AKfycbyJXSUMb3PtjiVwNMZt1Y4wHmLJh_pFqTQMgoJZCyLY66_mD-Uak0fNPgx5eO0URz0w"; //--> spreadsheet script ID


void setup()
{
    //Initialise Serial monitor with baud rate of 9600
    Serial.begin(9600);
    Wire.begin(); //Joing I2C bus
    //Initialises Infrared Temperature Sensor
    sensor.begin();
    
    //Select temperature in Fahrenheit
    sensor.setUnit(TEMP_F);    //This sets the unit of temperature read by sensor in Fahrenheit
     
    delay(10); //Gives a delay of 10 ms
    
    // Connect to WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
   
    WiFi.begin(ssid, password);
   
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
   
    // Start the server
    //  server.begin();
    Serial.println("Server started");

    //Prints the Local IP Address
    Serial.println(WiFi.localIP()); //Gets the WiFi shield's IP address and Print the IP address of serial monitor
    Serial.println();
    delay(1000);
  client.setInsecure();
}


void loop()
{
  //Taking the information of the incoming person
 Serial.print("Enter your name: ");
while (Serial.available() == 0)   //Wait for user input 
 {
  
} 
  Name = Serial.readString(); //Reading the Input string for name from Serial port and storing it in the variable Name

  Serial.println(Name);

 Serial.print("Enter your Address: ");
while (Serial.available() == 0)   //Wait for user input 
 {
  
} 
 Address = Serial.readString(); //Taking the address from the user

  Serial.println(Address);

  Serial.print("Enter your Contact: ");  
while (Serial.available() == 0)   //Wait for user input 
 {
  
} 
  Contact = Serial.readString(); //Reading email id from user

  Serial.println(Contact);

  Serial.println("Please place your hand near the temperature sensor!");
  Serial.println();

      delay(4000);
  
  if (sensor.read())
  {
    tempPerson = sensor.object(); //Reading the temperature from the IR Sensor
    
    //Showing the temperature of the person on Monitor
    Serial.print("Your Temperature is: ");
    Serial.println(tempPerson);
    Serial.println();
    
    if(tempPerson >= 85.0)
    {
      //Turning ON the Sound Alarm
      tone(piezoPin, 3000);
      delay(1000);
      noTone(piezoPin);
      delay(1000);
      
      tone(piezoPin, 3000);
      delay(1000);
      noTone(piezoPin);
      delay(1000);

    }
    sendData(tempPerson); //Sending data to database
  }
}

void sendData(float tem) 
{
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  
  //----------------------------------------Connect to Google host
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  //----------------------------------------

  //----------------------------------------Processing data and sending data
  String string_temperature =  String(tem);
  
  String url = "/macros/s/" + GAS_ID + "/exec?temperature=" + string_temperature;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");
  //----------------------------------------

  //----------------------------------------Checking whether the data was sent successfully or not
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266 CI successfull!");
  } else {
    Serial.println("esp8266 CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  //----------------------------------------
} 