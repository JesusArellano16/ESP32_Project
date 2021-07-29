
#include <WiFi.h>
#include <HTTPClient.h>
//Aquí se definen los datos de siempre
#define SSID ""
#define PASS ""
//ESTA ES LA VARIABLE QUE TIENEN QUE CAMBIAR PARA CAMBIAR LA PÁGINA
const char* pagina="https://website1063.000webhostapp.com/proyectoElectronicaConsultaDB.php";
String cadena="";

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
InitWiFi(); //Esta es una función para establecer 
//la comunicación con el router que me da internet

}

void loop() {
  // put your main code here, to run repeatedly:
  
  initHTTP();
  while(1);
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
    Serial.println("Conexion exitosa jojojo");
    }
  Serial.println("");
  Serial.print("Tu IP es: tonta... es broma");
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
     if (payload.indexOf("mysqli")>=0){
      return "...";
      }
     return payload;
  }
