#include <SoftwareSerial.h> // Library for using serial communication
SoftwareSerial SIM900(2,3); // Pins 2, 3 are used as used as software serial pins

String incomingData;   // for storing incoming serial data
String message = "";   // A String for storing the message
int relay_pin = 7;    // Initialized a pin for relay module
String Link ="The current Location is https://www.google.com/maps/place/";
String responce ="";
String Longitude ="";
String Latitude ="";

String SIM900_send(String incoming) 
{
  SIM900.println(incoming); delay(100);
  String result = "";
  while(SIM900.available())
  {
    char letter = SIM900.read();
    result = result + String(letter);
  }
  return result;
}



void setup()
{
  Serial.begin(115200); // baudrate for serial monitor
  SIM900.begin(19200); // baudrate for GSM shield

  pinMode(relay_pin, OUTPUT);   // Setting erlay pin as output pin
  digitalWrite(relay_pin, HIGH);  // Making relay pin initailly low

  // set SMS mode to text mode
  SIM900.print("AT+CMGF=1\r");  
  delay(100);
  
  // set gsm module to tp show the output on serial out
  SIM900.print("AT+CNMI=2,2,0,0,0\r"); 
  delay(100);
}

void loop()
{
  //Function for receiving sms
  receive_message();

  // if received command is to turn on bike
  if(incomingData.indexOf("on")>=0)
  {
    digitalWrite(relay_pin, LOW);
    message = "bike is turned ON";
    // Send a sms back to confirm that the relay is turned off
    send_message(message);
  }
  
  // if received command is to turn off relay
  if(incomingData.indexOf("off")>=0)
  {
    digitalWrite(relay_pin, HIGH);
    message = "bike is turned OFF";
    // Send a sms back to confirm that the bike is turned off
    send_message(message);
  }

  if(incomingData.indexOf("Location")>=0)
  {
    digitalWrite(relay_pin, HIGH);
    message = "Led is turned OFF";
    // Send a sms back to confirm that the relay is turned off
    send_message(message);

  Serial.begin(9600);
  SIM900.begin(9600);
  delay(3000);

  responce = SIM900_send("ATE1");  //enable echo if not enabled by default
  delay(1000);

  responce = SIM900_send("AT+CGATT=1"); //set the sim 800 in gprs mode
  delay(1000);

  responce = SIM900_send("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\" "); ///activate bearer profile
  delay(2000);

responce = SIM900_send("AT+SAPBR=3,1,\"APN\",\"RCMNET\" "); //set vpn options 
  delay(2000);

 responce = SIM900_send("AT+SAPBR=1,1"); //open bearer profile
  delay(2000);

 responce = SIM900_send("AT+SAPBR=2,1"); //get the ip adress of the bearer profile
  delay(1000);

     Serial.println ("sending sms"); delay(1000);
      responce = SIM900_send("ATH");
      delay(1000);
      responce = SIM900_send("ATE0");
      delay (1000);

      responce = ""; Latitude = "";
      SIM900.println("AT+CLBS=1,1");  //long,lat
      delay(5000);
        while (SIM900.available()){
          char letter = SIM900.read();
          responce = responce + String(letter);
          
        }
      Serial.print("Result Obtained as:");
      prepare_message(); delay(1000);
  } 
}

void receive_message()
{
  if (SIM900.available() > 0)
  {
    incomingData = SIM900.readString(); // Get the data from the serial port.
    Serial.print(incomingData); 
    delay(10); 
  }
}

void send_message(String message)
{
  SIM900.println("AT+CMGF=1");    //Set the GSM Module in Text Mode
  delay(100);  
  SIM900.println("AT+CMGS=\"+916381698218\""); // Replace it with your mobile number
  delay(100);
  SIM900.println(message);   // The SMS text you want to send
  delay(100);
  SIM900.println((char)26);  // ASCII code of CTRL+Z
  delay(100);
  SIM900.println();
  delay(1000);  
}
void prepare_message()
{
  int first_comma = responce.indexOf(','); //find the position of 1st comma
  int second_comma = responce.indexOf(',', first_comma+1); //pos of 2nd comma

  int third_comma = responce.indexOf(',', second_comma+1); //3rd comma

  for(int i=first_comma+1; i<second_comma; i++)
    Longitude = Longitude + responce.charAt(i);

  for(int i=second_comma+1; i<third_comma; i++)
    Latitude = Latitude + responce.charAt(i);

  Link = Link + Latitude + "," +Longitude;
  send_message(Link);
}
