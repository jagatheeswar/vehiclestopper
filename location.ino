#include <SoftwareSerial.h>


SoftwareSerial SIM900(2,3);

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
  Serial.begin(9600);
  SIM900.begin(9600);
  delay(3000);

  responce = SIM900_send("ATE1");  //enable echo if not enabled by default
  Serial.print ("Responce:");
  delay(1000);

  responce = SIM900_send("AT+CGATT=1"); //set the sim 800 in gprs mode
  Serial.print ("Responce:");
  delay(1000);

  responce = SIM900_send("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\" "); ///activate bearer profile
  Serial.print ("Responce:"); Serial.println ("Responce:");
  delay(2000);

responce = SIM900_send("AT+SAPBR=3,1,\"APN\",\"RCMNET\" "); //set vpn options 
  Serial.print ("Responce:"); Serial.println ("Responce:");
  delay(2000);

 responce = SIM900_send("AT+SAPBR=1,1"); //open bearer profile
  Serial.print ("Responce:"); Serial.println ("Responce:");
  delay(2000);

 responce = SIM900_send("AT+SAPBR=2,1"); //get the ip adress of the bearer profile
  Serial.print ("Responce:"); Serial.println ("Responce:");
  delay(1000);
}

void prepare_message()
{
  int first_comma = responce.indexOf(','); //find the position of 1st comma
  int second_comma = responce.indexOf(',', first_comma+1); //pos of 2nd comma

  int third_comma = responce.indexOf(',', second_comma+1); //3rd comma

  for(int i=first_comma+1; i<second_comma; i++){
    Longitude = Longitude + responce.charAt(i);
  }

  for(int i=second_comma+1; i<third_comma; i++)
    Latitude = Latitude + responce.charAt(i);

  Serial.println(Latitude); Serial.println(Longitude);
  Link = Link + Latitude + "," +Longitude;
  Serial.println(Link);
  
}

String incoming = "";

void loop() {
  if (SIM900.available())
  {
    char a = SIM900.read();
    Serial.write(a);
    incoming = incoming + String(a);
    if (a==13)
    incoming = "";
    incoming.trim();

    if (incoming=="RING")
    {
      Serial.println ("sending sms"); delay(1000);
      responce = SIM900_send("ATH");
      delay(1000);
      responce = SIM900_send("ATE0");
      delay (1000);

      responce = ""; Latitude = "";
      SIM900.println("AT+CLBS=1,1");
      delay(5000);
        while (SIM900.available()){
          char letter = SIM900.read();
          responce = responce + String(letter);
          
        }
      Serial.print("Result Obtained as:");

      prepare_message(); delay(1000);

      SIM900.println("AT+CMGF=1"); //set the module in sms mode
      delay(1000);

      SIM900.println("AT+CMGS=\"6381698218\"");
      delay(1000);

SIM900.println(Link);
      delay(1000);

      SIM900.println((char)26);//ascii code of ctrl+z
    delay(1000);
    }
    
  }
  if (Serial.available())
  {
    SIM900.write(Serial.read());
  }

}
