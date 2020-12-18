int pin_del_sensor = A0; //supongamos que sea el A0 osea el pin analogico 0
int sensor_valor; //variable para almacenar el dato leido
void setup(){
  Serial.begin(9600); // esto es para que muestre en el monitor serial lo leido
}
void loop(){
  sensor_valor = analogRead(pin_del_sensor); //lo que recibo del sensor lo guardo en una variable
  Serial.println(sensor_valor); //muestro en pantalla lo que tiene la variable
  delay(500);  //hago un "delay" o "pausa" para no llenar la pantalla con datos, osea cada 0,5segundos hace un "pausa"
}
