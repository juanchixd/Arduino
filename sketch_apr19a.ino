// Boton_luz_salon.ino
// Al pulsar el botón en el móvil se activará un relé que en
// este caso encenderá una luz vía Internet.
 
#include <ESP8266WiFi.h>    // ESP8266WiFi.h entre < y >
#include <BlynkSimpleEsp8266.h> // BlynkSimpleEsp8266.h entre < y >
 
//Su authtoken generado por la aplicacion Blynk
char auth[] = "eb9074962b404df381c4a608661def48"; // auth del e-mail
 
//Datos para la conexion de Red Wifi.
char ssid[] = "Empanadas de carne 2.4GHz"; //Nombre de la red WIFI
char pass[] = "otravezsopa"; //contraseña de la red WIFI
 
void setup()
{
Serial.begin(115200);
Blynk.begin(auth, ssid, pass);
}
 
void loop()
{
Blynk.run();
}
