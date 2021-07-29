#include <WiFi.h>
#include <HTTPClient.h>
//Aquí se definen los datos de internet
#define SSID ""
#define PASS ""
//la página a la que nos queremos conectar
const char* pagina="https://website1063.000webhostapp.com/proyectoElectronicaConsultaDB.php";
String cadena="";
// Que el pin sea el 16 (GPIO16)
const int ledPin = 16;  // 16 corresponds to GPIO16

// Variables de PWM
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

int dutyCycle=40; 
int valor=40;


void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
InitWiFi(); //Esta es una función para establecer 
//la comunicación con el router que me da internet
ledcSetup(ledChannel, freq, resolution);
  dutyCycle=50;
  valor=50;
  // attach the channel to the GPIO to be controlled
  //aquí le dice al canal 0, que obligue al led 16 a
  //sacar todo lo que él le diga
  ledcAttachPin(ledPin, ledChannel);



}

void loop() {
  // put your main code here, to run repeatedly:
  //valor=10;
  initHTTP();//esto conecta a la página deseada
}

void InitWiFi(){
    WiFi.mode(WIFI_STA);
  //WIFI_STA Permite CONECTARSE A una red
  //WIFI_AP Convierte en router o punto de acceso a la tarjeta
  //WIFI_MODE_APSTA Permite hacer las dos cosas al mismo tiempo
  WiFi.begin(SSID, PASS);
  Serial.print("Conectando a ");
  Serial.println(SSID);
  while(WiFi.status()!=WL_CONNECTED){
    Serial.print(".");
    delay(50);
    }

  if(WiFi.status()==WL_CONNECTED){
    Serial.println("Conexion exitosa");
    }
  Serial.println("");
  Serial.print("Tu IP es: "); 
  Serial.println(WiFi.localIP());
}


void initHTTP(){
  if(WiFi.status()==WL_CONNECTED){
    cadena=getRequest(pagina);
    Serial.print("Lo que obtenemos de la página ");
    Serial.print(pagina);
    Serial.print(" es lo siguiente:\n");
    Serial.println(cadena);
    //Aquí extraemos el número
    if(cadena!="..."&& cadena!=""){
      Serial.println("Entro");
      int posPts=cadena.indexOf(':');
      int posEnter=cadena.length();
      dutyCycle=cadena.substring(posPts+1,posEnter).toInt();
      dutyCycle=(dutyCycle>100)?100:dutyCycle;
      dutyCycle=(dutyCycle<0)?0:dutyCycle;
      valor = dutyCycle*255/100;
      ledcWrite(ledChannel, valor);
      }else{
        Serial.println("EL ERROR");
        
       }
     //ledcWrite(ledChannel, valor);
     delay(200);
     
    
    }
  }



String getRequest(const char* serverName){
  HTTPClient http;
  http.begin(serverName); //establece comunicación con el servidor
  int httpResponseCode=http.GET();

  String payload="...";
  if (httpResponseCode>0){//si el código de respuesta es 
    //mayor a cero, sí se conectó
    
    Serial.print("HTTP Response code: "); //imprime el código de respuesta
    Serial.println(httpResponseCode);
    payload=http.getString();
    }
    else{//no se pudo
       Serial.print("Error code: ");
       Serial.println(httpResponseCode);
      }
     http.end();
     if(payload.indexOf("mysqli")>0)
     {
      return "...";
      }
     return payload;
  }
