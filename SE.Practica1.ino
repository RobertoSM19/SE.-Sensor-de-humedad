#include <LiquidCrystal_I2C.h> //Inclusión de la libreria para la comunicación de la pantalla lcd con comunicación I2C
LiquidCrystal_I2C pantalla(0x27, 16, 2);

//Definiendo los pines
#define PIN_SENSOR A0 //Recibimos señal analogica de el pin por lo que hacemos que la señal sea recibida de un puerto analogico.
#define BUTTON 5
#define RELAY 6
#define PIN_BUZZER 8
//los siguientes tres pines digitales son necesarios para mandar señal a los tres pines del led rgb para controlar el color de dicho led
#define PIN_RED 9
#define PIN_GREEN 10
#define PIN_BLUE 11

//Bandera que determina si el modo automatico esta activo o no.
bool modoAutomatico=false;

//Creación de un array de tipo byte donde contienen los pixeles de un caracter de una gota de agua en la pantalla LCD
byte waterIcon[8] = {
  B00100,
  B01010,
  B01010,
  B01010,
  B01010,
  B10001,
  B10001,
  B01110
};

//Creación de un array de tipo byte donde contienen los pixeles de un caracter de un rayo en la pantalla LCD
byte lightningIcon[8] = {
  B00100,
  B01010,
  B10101,
  B00100,
  B00100,
  B01010,
  B10101,
  B00100
};

//Configuración necesaria para inicializar el sistema.
void setup()
{
  Serial.begin(9600); //Inicializamos los pines RX y TX para que puedan ser usados como puerto serial con una velocidad de 9600 baudios por segundo
  pantalla.init(); //Inicializa el modulo adaptador LCD a I2C, esta función internamente configura e inicializa el I2C y el LCD.
  pantalla.backlight(); //Encendemos la luz del fondo de la pantalla LCD.
  pantalla.createChar(0, waterIcon); //Crea un carácter personalizado en este caso una gota de agua para su uso en la pantalla LCD. 
  pantalla.createChar(1, lightningIcon);  //Crea un carácter personalizado en este caso un rayo para su uso en la pantalla LCD. 

  //Los siguientes dos pines son puestos como entrada debido a que reciben algún dato o señal.
  pinMode(PIN_SENSOR,INPUT);
  pinMode(BUTTON,INPUT);
  
  //Los siguientes cinco pines son puestos como salida debido a que son los que nos mandan señal a traves de algun componente como por ejemplo el led RGB.
  pinMode(RELAY,OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
}
 

//Función principal
void loop()
{
  float porcentaje=getSoilmoisture(); //Obtiene el valor retornado de la funcion getSoilmoisture que es el porcentaje de la temperatura de lo que lee el sensor segun una regla de tres.
  
  //Control automatico del riego o trabajo sin modo automatico
  if(modoAutomatico){  //condicional que busca la bandera del modo automatico para determinar que hacer segun su modo.
    (checkHumidity(porcentaje))?drainWater(true):drainWater(false);  //condicional ternario que mandara como parametro un true a la funcion drainwWater en caso de que el valor de la funcion checkHumidity retorne true.
  }else{
    //En caso de que la bandera modoAutomatico sea igual a false llamaran a las siguientes funciones.
    showValue(porcentaje);
    ledIndicator(porcentaje);
  }
  
  if(digitalRead(BUTTON)){  // En caso de que el boton sea presionado disparara los eventos correspondientes
    modoAutomatico=!modoAutomatico; //Cambia la bandera de valor, si es true se vuelve false y viceversa
    if (modoAutomatico) {  //En caso de que la bandera sea true desencadena los eventos correspondientes
      pantalla.clear();  //Limpia todo lo que hay en la pantalla LCD
      digitalWrite(PIN_RED, HIGH); //Manda una señal positiva o un uno logico al pin rojo del led para modificar el color
      digitalWrite(PIN_GREEN, LOW);  //Manda una señal negativa o un cero logico al pin verde del led para modificar el color
      digitalWrite(PIN_BLUE, HIGH);  //Manda una señal positiva o un uno logico al pin azul del led para modificar el color
      //Con las configuraciones anteriores el led RGB se vuelve de color morado indicando que esta en modo automatico.
      playSong(1);  //Manda como parametro un 1 como parametro de la funcion playSong que sera la opcion de cancion que se tocara en el buzzer 
    } else {  //En caso de que la bandera sea false desencadena los eventos correspondientes
      drainWater(false);  //manda como parametro un false a la funcion drainWater y desencadenara los eventos que esten en dicha funcion
      showValue(porcentaje); //Manda como parametro el porcentaje de humedad para que en base a eso desencadene los eventos que esten en dicha funcion
      ledIndicator(porcentaje);  //Manda como paramtro elporcentaje de humedad para que en base a eso desencadene los eventos que esten en dicha funcion
      playSong(2); //Manda como parametro un 2 como parametro de la funcion playSong que sera la opcion de cancion que se tocara en el buzzer 
    }
  }
  
  delay(1000); //Pausamos la ejecucion del programa durante un segundo o 1000 milisegundos.
}

//Función que retorna la humedad en porcentaje
float getSoilmoisture(){
  int valorSensor=analogRead(PIN_SENSOR);  //Obtiene la humedad del sensor en valor analogico
  float porcentaje=map(valorSensor,1023,20,0,100);  //En base a la humedad en valor analogico se hace una regla de tres con el map para que nos de un porcentaje, se le pasan estos parametros debido a que 1023 es el valor maximo en analogico y el minimo 20, la variable porcentaje almacenara un valor de 0 a 100 segun el valor de la variable valorSensor.
  return porcentaje;  //retorna el valor de la variable porcentaje.
}

//Función que muestra el valor analogico y digital de la humedad en la LCD 
void showValue (float porcentaje ){  //Requiere el parametro porcentaje para que funcione la funcion
  pantalla.clear();  //Limpia la pantalla LCD
  pantalla.setCursor(0,0);  //Posiciona el cursor del LCD en la posición indicada por columna y fila(x,y); es decir, establece la ubicación en la que se mostrará posteriormente texto escrito para la pantalla LCD.
  pantalla.print("Humidity: "); //Escribe el texto Humidity: en el LCD
  pantalla.write(byte(0));  //Escribe un caracter en la LCD en este caso el caracter de la gota de agua.
  pantalla.print(porcentaje);  //Escribe el valor de porcentaje en el LCD
  pantalla.print("%");  //Escribe el texto % en el LCD
  pantalla.setCursor(0,1);  //Posiciona el cursor del LCD en la posición indicada por columna y fila(x,y); es decir, establece la ubicación en la que se mostrará posteriormente texto escrito para la pantalla LCD.
  pantalla.print("Analog: "); //Escribe el texto Analog: en el LCD
  pantalla.write(byte(1));  //Escribe un caracter en la LCD en este caso el caracter del trueno.
  pantalla.print(analogRead(PIN_SENSOR));   //Escribe el valor analogico leido del sensor de humedad en el LCD
}

//Función para controlar los colores del led según la humedad
void ledIndicator(float porcentaje){  //Requiere el parametro porcentaje para que funcione la funcion
    //Condicional que trabaja con el valor del porcentaje para encender el led RGB de algun color mandando señales a cada pin del led.
  if(porcentaje==0){  //En este caso el led se hara rojo
    digitalWrite(PIN_RED, HIGH);
    digitalWrite(PIN_GREEN, LOW);
    digitalWrite(PIN_BLUE, LOW);
  }else if (porcentaje > 0 && porcentaje<50) {  //En este caso el led se hara amarillo
    digitalWrite(PIN_RED, HIGH);
    digitalWrite(PIN_GREEN, HIGH);
    digitalWrite(PIN_BLUE, LOW);
  }else if (porcentaje >= 50 && porcentaje<75) { //En este caso el led se hara blanco
    digitalWrite(PIN_RED, HIGH);
    digitalWrite(PIN_GREEN, HIGH);
    digitalWrite(PIN_BLUE, HIGH);
  }else if (porcentaje >= 75 && porcentaje<100) {  //En este caso el led se pondra azul
    digitalWrite(PIN_RED, LOW);
    digitalWrite(PIN_GREEN, LOW);
    digitalWrite(PIN_BLUE, HIGH);
  } else if(porcentaje==100) {  //En este caso el led se pondra verde
    digitalWrite(PIN_RED, LOW);
    digitalWrite(PIN_GREEN, HIGH);
    digitalWrite(PIN_BLUE, LOW);
  }
}

//Función que verifica si la humedad supera un umbral asignado
bool checkHumidity(float porcentaje){  //Requiere el parametro porcentaje para que funcione la funcion
  return porcentaje < 80;  //Retorna true o false en caso de que la condicion se cumpla
}

//Función que reproduce sonidos en el BUZZER según sea la opción.
void playSong(int opcion){  //Requiere el parametro opcion para que funcione la funcion
  if(opcion==1){  //En caso de que opcion sea igual a 1 se reproducira el tono creado
    tone(PIN_BUZZER, 400, 250);  //Genera un tono en el pin del buzzer de 400 hz durante 250 milisegundos
    delay(250);
    tone(PIN_BUZZER, 600, 250);
    delay(250);
  }else if(opcion==2){  //En caso de que opcion sea igual a 2 se reproducira el tono creado
    tone(PIN_BUZZER, 800, 50);
    delay(50);
    tone(PIN_BUZZER, 600, 50);
    delay(50);
    tone(PIN_BUZZER, 400, 50);
    delay(50);
  }else if(opcion==3){  //En caso de que opcion sea igual a 3 se reproducira el tono creado
    tone(PIN_BUZZER, 523, 125);
  delay(125);
  tone(PIN_BUZZER, 587, 125);
  delay(125);
    tone(PIN_BUZZER, 659, 125);
    delay(125);
    tone(PIN_BUZZER, 698, 125);
    delay(125);
    tone(PIN_BUZZER, 783, 125);
    delay(125);
  }
}


//Función para activar o desactivar la bomba de agua.
void drainWater (bool humidityFlag){  //Requiere el parametro humidityFlag para que funcione la funcion
  if(humidityFlag){  //En caso de que la bandera sea tru desencadena los eventos correspondientes
    digitalWrite(RELAY,HIGH);  //Manda una señal alta o un 1 logico al relay lo que hace que se ponga en normalmente cerrado y se active la bomba de agua.
    playSong(3);  //Reproduce la cancion 3 de la funcion playSong
  }else{
    digitalWrite(RELAY,LOW); //Manda una señal baja o un o logico al relay lo que hace que se ponga en normalmente abierto y se  desactive la bomba de agua.
    modoAutomatico=false;  //La bandera modoAutomatico la cambia para que se salga del modo automatioc automaticamente.
  }
}