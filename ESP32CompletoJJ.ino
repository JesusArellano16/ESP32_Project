#include <WiFi.h>                                                                       //Librería para conectarse a Wi-Fi
#include <HTTPClient.h>                                                                 //Librería para conexión http
#include <RBDdimmer.h>                                                                  //Librería para el dimmer

//Aquí se definen los datos de siempre
#define SSID "Salahause_2.4Gnormal"                                                     //Nombre de la red
#define PASS "JJrb2002pp"                                                               //Contraseña de la red
#define MIFOCO1 5                                                                       //Foco en la base de datos
#define MIFOCO2 6                                                                       //Foco en la base de datos
#define RELAY 2
#define MISFOCOSAQUI 3                                                                  //Cantidad de focos en la base de datos
#define Led 2
#define RELAYPIN 15
const char* pagina="https://appdevops.000webhostapp.com/ConsultaESP32.php";             //Archivo php que ayuda a obtener información
String textoPag="";                                                                     //Texto que se recibe de la página

const int ledPin_1 = 16;                                                              // 16 corresponde a GPIO16 y es igual al MIFOCO1

// Variables de PWM del foco con la cosa de la cosa del dimmer
const int frecuencia = 5000;                                                                // frecuencia en Hz
const int canalPWM = 0;                                                             
const int resolucion = 8;                                                             // 8 bits de resolución

//Variables del dimmer AC
const int pinCruceCero = 22;
const int acdPin = 18;
dimmerLamp acd(acdPin, pinCruceCero);

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
pinMode(Led, OUTPUT);
InitWiFi(); //Esta es una función para establecer 
//la comunicación con el router que me da internet
ledcSetup(canalPWM, frecuencia, resolucion);
ledcAttachPin(ledPin_1, canalPWM);
acd.begin(NORMAL_MODE, ON);
pinMode(RELAYPIN, OUTPUT);
}

void loop() {
  initHTTP();
}

void InitWiFi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);
  Serial.print("Conectando a ");
  Serial.println(SSID);
  int tim =0;
  while(WiFi.status()!=WL_CONNECTED){
    Serial.print(".");
    digitalWrite(Led, 0);
    delay(50);
    tim++;
    if (tim == 100)
    {
      ESP.restart();
    }
    }
  if(WiFi.status()==WL_CONNECTED){
    Serial.println("Conexion exitosa");
    digitalWrite(Led, 1);
    }
}


void initHTTP(){
  if(WiFi.status()==WL_CONNECTED){
    textoPag=getRequest(pagina);
    Serial.print("Lo que obtenemos de la página ");
    Serial.print(pagina);
    Serial.print(" es lo siguiente:\n");
    Serial.println(textoPag);
    /////////////////////////////////////////////////////////////////////////////////////////
    //aquí nos aseguramos de que los espacios en el texto se vayan
    if(textoPag!="..."&& textoPag!=""){
      String baguette=textoPag;
      baguette.trim();
      
      //las posiciones de los primeros datos
      int posIni=baguette.indexOf(':');
      int posFin=baguette.indexOf('>');
      int posUltimoSigno=baguette.lastIndexOf('>');
      int longitud=baguette.length();
      
      //obteniendo el número de focos
      int numFocos=baguette.substring(posUltimoSigno+1,longitud).toInt();
      String cadenas[numFocos];
      int datos[numFocos][3];
      int contador,i;
      for (contador=0;contador<numFocos;contador++){
        //se come la primera parte del baguette, que es el primer foco
        cadenas[contador]=baguette.substring(posIni+1,posFin); //el formato aquí es nn-nn-nn ya no hay "< ID:" ni ">"
        Serial.println(cadenas[contador]);
        baguette=baguette.substring(posFin+1,longitud);
        for (i=0;i<3;i++){
          //tomaré las variables que ya tenía prestadas, porque no las voy a volver a usar
          
          longitud=cadenas[contador].length();
          posFin=cadenas[contador].indexOf('-');
          
          datos[contador][i]=cadenas[contador].substring(0,posFin).toInt();
          cadenas[contador]=cadenas[contador].substring(posFin+1,longitud);
          }
        posIni=baguette.indexOf(':');
        posFin=baguette.indexOf('>');
        longitud=baguette.length();
        }//fin del for


        //AQUI DEBO INSERTAR LA PARTE DE LOS FOCOS, LA FUNCIONALIDAD
        for(contador=0;contador<numFocos;contador++){
          //impresion="";
          for(i=0;i<3;i++){
            
            Serial.print(String(datos[contador][i])+", ");
            }
            Serial.println("");
          }
         ///////////DEBO RECORDAR QUITAR LO DE ARRIBA LUEGO
         prendeFocos(datos,numFocos);
  
     }else{
      Serial.println("Hubo un error");
      }
    }//fin del if not connected
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
     if (payload.indexOf("mysqli")>0){
      return "...";
      }
     return payload;
  }

  
void prendeFocos(int datos[][3],int numFocos){
  int misFocos[MISFOCOSAQUI][2],i=0,j=0;
  //Aqui obtiene los focos de la matriz de datos
  for(i=0;i<numFocos;i++){
    if(datos[i][0]==RELAY || datos[i][0]==MIFOCO1 || datos[i][0]==MIFOCO2){
      misFocos[j][0]=datos[i][1];//este será el estado de encendido o apagado
      misFocos[j][1]=datos[i][2];//este será la intensidad
      j++;
      }
   }
   
  //aquí ya prende los focos
  int nombresFocos[]={RELAY,MIFOCO1,MIFOCO2};
  for(i=0;i<MISFOCOSAQUI;i++){
    //si el foco está encendido
      switch(nombresFocos[i]){
        case RELAY:
        Serial.println("El relay: "+String(nombresFocos[0])+", Está en estado "+String(misFocos[i][0]));
        digitalWrite(RELAYPIN, !misFocos[i][0]);
        break;
        case MIFOCO1://enciende el foco 1
        Serial.println("El foco: "+String(nombresFocos[1])+", Está en estado "+String(misFocos[i][0])+", Con intensidad = "+String(misFocos[i][1]));
        ledcWrite(canalPWM,misFocos[1][0]*map(misFocos[1][1],0,100,0,255));      
        break;
        case MIFOCO2:
        Serial.println("El foco: "+String(nombresFocos[2])+", Está en estado "+String(misFocos[i][0])+", Con intensidad = "+String(misFocos[i][1]));
        //ledcWrite(canalPWM2,misFocos[1][0]*map(misFocos[1][1],0,100,0,255));    //Si es led con pwm
        if(misFocos[2][0]==1){acd.setPower(map(misFocos[2][1],0,100,16,95));}
        else{acd.setPower(15);}
        break;
      }
  }
}
