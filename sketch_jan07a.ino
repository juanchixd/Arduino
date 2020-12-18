#define SOL_ADC A0     // Panel solar, voltaje del divisor resistivo conectado al pin A0 
#define BAT_ADC A1    // Bateria, volataje del divisor resistivo conectado al pin A1
#define AVG_NUM 10    // número de iteraciones de la rutina adc para promediar las lecturas de adc
#define BAT_MIN 10  // minimo voltaje de la bateria para salida 12V
#define BAT_MAX 15  // maximo voltaje de la bateria para salida 12V
#define BULK_CH_SP 14.4 // punto de maximo de la bateria
#define FLOAT_CH_SP 13.8  // tension de flote de la bateria
#define LVD 11          // tension de desconexion por baja tension
#define PIR_PIN 7         // pin-7 usado para el sensor pir
#define REF_PIN 4         // pin-4 usado para controlar el reflector con sensor pir
#define PWM_PIN 3         // pin-3 usado para controlar la carga a traves del MOSFET //la frecuencia por defecto es 490.20Hz
#define LOAD_PIN 2       // pin-2 usado para controlar la salida
//--------------------------------------------------------------------------------------------------------------------------
///////////////////////DECLARACION DE VARIABLES GLOBALES//////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------------------------------
float solar_volt = 0;
float bat_volt = 0;
float bulk_charge_sp = 0;
float float_charge_sp = 0;
float charge_status = 0;
float load_status = 0;
float error = 0;
float Ep = 0;
int duty = 0;
float lvd;
float msec = 0;
float last_msec = 0;
float elasped_msec = 0;
float elasped_time = 0;
int val = 0;
int pirstate = 0;

//******************************************************* PROGRAMA PRINCIPAL ************************************************
void setup()
{
  Serial.begin(9600);
  pinMode(PWM_PIN, OUTPUT);
  pinMode(LOAD_PIN, OUTPUT);
  pinMode(REF_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  digitalWrite(PWM_PIN, LOW); // por defecto para que inicie apagada la carga
  digitalWrite(LOAD_PIN, LOW); // por defecto para que inicie apagada la carga
}

void loop()
{
  read_data();             // leer datos de los pines
  charge_cycle();         // pwm carga de bateria
  power();                // calcular potencia
  load_control();         // controlar salida
  print_data();            // imprimir datos
}
//************************************************************ PROGRAMA FIN *************************************************
//------------------------------------------------------------------------------------------------------
////////////////// LEER VALORES Y SACAR CUENTAS (SOLAR VOLTAGE, BATERIA VOLTAGE)////////////////
//------------------------------------------------------------------------------------------------------
int read_adc(int adc_parameter) {
  int sum = 0;
  int sample ;
  for (int i = 0; i < AVG_NUM; i++) {
    sample = analogRead(adc_parameter);    // leer pin de entrada
    sum += sample;                        // guardar lectura
    delayMicroseconds(50);              // esperar 50 microsegundos
  }
  return (sum / AVG_NUM);               // divide sum by AVG_NUM to get average and return it
}
//-------------------------------------------------------------------------------------------------------------
////////////////////////////////////LEER DATOS//////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------------------
void read_data(void) {
  //5V = ADC value 1024 => 1 ADC value = (5/1024)Volt= 0.0048828Volt
  // Vout=Vin*R2/(R1+R2) => Vin = Vout*(R1+R2)/R2   R1=100 and R2=20
  solar_volt = read_adc(SOL_ADC) * 0.00488 * (120 / 20);
  bat_volt   = read_adc(BAT_ADC) * 0.00488 * (120 / 20);
  val = digitalRead(PIR_PIN);
}
//------------------------------------------------------------------------------------------------------------
/////////////////////////////////CALCULO DE POTENCIA //////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------------
void power(void) {
  msec = millis();
  elasped_msec = msec - last_msec; // calcula el tiempo desde la ultima vez que llame a la funcion
  elasped_time = elasped_msec / 1000.0; // 1sec=1000 msec
  last_msec = msec; // guarda el 'ahora' para la proxima vez
}

//------------------------------------------------------------------------------------------------------------
/////////////////////////////////PRINT DATA IN SERIAL MONITOR/////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------------
void print_data(void)
{
  delay(10000);
  Serial.print("Tension del panel solar: ");
  Serial.print(solar_volt);
  Serial.println("V");
  Serial.print("Tension de la bateria: ");
  Serial.print(bat_volt);
  Serial.println("V");
  Serial.print("Punto de carga:");
  Serial.println(bulk_charge_sp);
  Serial.print("Estado de carga :");
  if (charge_status == 1)
  {
    Serial.println("99%");
    Serial.println("Cargando");
  }
  else if (charge_status == 2)
  {
    Serial.print(Ep);
    Serial.println("%");
    Serial.println("Punto de flote");
  }
  else
  {
    Serial.println("0%");
    Serial.println("No cargando");
  }
  if (load_status == 1)
  {
    Serial.println("Salida conectada");
  }
  else
  {
    Serial.println("Salida desconectada");
  }

  Serial.println("***************************");
}
//--------------------------------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////PWM CARGA A 500 HZ //////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------------------------------------
void charge_cycle(void)
{
  if (solar_volt > bat_volt)
  {


    if (bat_volt <= FLOAT_CH_SP) // carga empieza
    {
      charge_status = 1; // indica el estado de carga
      duty = 252.45;
      analogWrite(PWM_PIN, duty); // 99 % de carga // carga rapida


    }
    else if (bat_volt > FLOAT_CH_SP)
    {
      charge_status = 2; // indica estado de flote
      error  = (BULK_CH_SP - bat_volt);      // reduce la frecuencia cuando se acerca al punto de carga maximo
      Ep = error * 100 ; //Ep= error* Kp // Assume  Kp=100

      if (Ep < 0)
      {
        Ep = 0;
      }
      else if (Ep > 100)
      {
        Ep = 100;
      }
      else if (Ep > 0 && Ep <= 100) // regulando
      {
        duty = (Ep * 255) / 100;
      }
      analogWrite(PWM_PIN, duty);
    }
  }
  else
  {
    charge_status = 0; // indica que no esta cargando
    duty = 0;
    analogWrite(PWM_PIN, duty);
  }
}
//----------------------------------------------------------------------------------------------------------------------
/////////////////////////////////////////////CONTROL DE SALIDA/////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------------------------------------

void load_control()
{
  if (solar_volt < 5  ) // verifica que la tension del panel sea menor, indicando que es de noche
  {
    if (bat_volt > lvd) // verifica la tension de la bateria
    {
      load_status = 1;
      digitalWrite(LOAD_PIN, HIGH); // enciende la salida
      if (val == HIGH)
      {
        digitalWrite(REF_PIN, HIGH);
      }
      else if (val == LOW)
      {
         digitalWrite(REF_PIN, LOW);
      }
    }
    else if (bat_volt < lvd)
    {
      load_status = 0;
      digitalWrite(LOAD_PIN, LOW); //apaga la salida
    }
  }
  else // apaga la salida durante el dia
  {
    load_status = 0;
    digitalWrite(LOAD_PIN, LOW);
  }
}
