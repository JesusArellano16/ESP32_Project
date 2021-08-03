 

#include <WiFi.h>
#include <HTTPClient.h>
//Aquí se definen los datos de siempre
#define SSID "INFINITUME69D_2.4"
#define PASS "M9HaSA3SQP"
#define MIFOCO1 1
#define MIFOCO2 2
#define MISFOCOSAQUI 2
const char* pagina="https://appdevops.000webhostapp.com/ConsultaESP32.php";
String textoPag="";


// Que el pin sea el 16 (GPIO16)
const int ledPin_1 = 16;  // 16 corresponds to GPIO16

// Variables de PWM del foco con la cosa de la cosa del dimmer
const int freq_1 = 5000;
const int ledChannel_1 = 0;
const int resolution_1 = 8;

int dutyCycle_1=40; 
int valor_1=40;




void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
InitWiFi(); //Esta es una función para establecer 
//la comunicación con el router que me da internet
ledcSetup(ledChannel_1, freq_1, resolution_1);
ledcAttachPin(ledPin_1, ledChannel_1);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  initHTTP();
  //while(1);
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
    
  //Serial.print("Tu IP es: "); 
  //Serial.println(WiFi.localIP());
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
    if(datos[i][0]==MIFOCO1 || datos[i][0]==MIFOCO2){
      misFocos[j][0]=datos[i][1];//este será el estado de encendido o apagado
      misFocos[j][1]=datos[i][2];//este será la intensidad
      j++;
      }
   }
   
  //aquí ya prende los focos
  int nombresFocos[]={MIFOCO1,MIFOCO2};
  for(i=0;i<MISFOCOSAQUI;i++){
    //si el foco está encendido
      switch(nombresFocos[i]){
        
        case MIFOCO1://enciende el foco 1
        Serial.println("El foco: "+String(i)+", Está en estado "+String(misFocos[i][0]));
        dutyCycle_1=misFocos[0][1];
        dutyCycle_1=(dutyCycle_1>100)?100:dutyCycle_1;
        dutyCycle_1=(dutyCycle_1<0)?0:dutyCycle_1;
        valor_1 = misFocos[0][0]*dutyCycle_1*255/100;
        ledcWrite(ledChannel_1, valor_1);
        
        break;
        case MIFOCO2:
        Serial.println("El foco: "+String(i)+", Está en estado "+String(misFocos[i][0]));
        
        
        break;
          }
      
  }
}
