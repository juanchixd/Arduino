#define BLYNK_PRINT Serial
#define pin_relay_bomba 12 //Reemplazar 14 con el pin correspondiente a utilizar
#define pin_sensor_humedad A0 //Reemplazar A0 con el pin correspondiente a utilizar 
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
int sensorhumedad;
char auth[] = "Aca va el codigo que te da blynk de autenticacion";
char ssid[] = "Nombre de la Wi-Fi";
char pass[] = "Contraseña de la Wi-Fi";
BlynkTimer timer;

void myTimerEvent() {
  sensorhumedad = analogRead(pin_sensor_humedad);
  if (sensorhumedad < 512){ //Corregir el valor y el signo segun corresponda a la humedad adecuada y como mida el sensor
    digitalWrite(pin_relay_bomba, HIGH);
    Serial.println("Bomba de agua encendida");
  }
  else{
    digitalWrite(pin_relay_bomba, LOW);
    Serial.println("Bomba de agua apagada");
  }
  Blynk.virtualWrite(V5, sensorhumedad); //Esta linea envia al pin virtual V5 los datos del sensor a la app
}
void setup() {
  Serial.begin(19200);
  Blynk.begin(auth, ssid, pass);
  pinMode(pin_relay_bomba, OUTPUT);
  digitalWrite(pin_relay_bomba, LOW);
  timer.setInterval(1000L, myTimerEvent);
}

void loop() {
  Blynk.run();
  timer.run();
}
