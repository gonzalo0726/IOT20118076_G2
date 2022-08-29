#include <ESP8266WiFi.h> //LIBRERÍA DEL ESP8266
#include <PubSubClient.h>
#include <Wire.h>

//LIBRERÍAS DEL DHT (SENSOR DE TEMPERATURA Y HUMEDAD)
#include <DHT.h>
#include <DHT_U.h>

const char* ssid = "Caramelo_WiFi"; //RED DE WIFI A CONECTARSE
const char* password = "nIACARAMELO042117"; //CONTRASEÑA DE LA RED DE WIFI
const char* mqtt_server = "127.0.0.1";

WiFiClient espClient; //VARIABLE PARA CONECTARNOS DESDE EL WIFI COMO CLIENTE
PubSubClient client(espClient);

DHT dht(D4, DHT22); //INDICAMOS QUE EL DHT ESTARÁ TRABAJANDO CON LA ENTRADA SERIAL D4
float tempe, hume, hume_suelo; //VARIABLES GLOBALES PARA GUARDAR EL VALOR DE LAS ENTRADAS DIGITALES
byte hsp = A0; //VARIABLE QUE GUARDA LA ENTRADA ANALÓGICA

char dato1[10], dato2[10], dato3[10], dato4[10];

void setup_wifi() {

  delay(10);
  //NOS CONECTAMOS A WIFI
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  //EN CASO QUE NO SE CONECTE
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());

  //ESCRIBIENDO DATOS DE LA RED CONECTADA
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
}

//Funcion para recibir mensajes del MQTT
void callback(char* topic, byte* payload, unsigned int length){
   Serial.print("Message arrived [");
    Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (String(topic) == "boton") {
    if(messageTemp == "true"){
      Serial.println("on");
    }
    else if(messageTemp == "false"){
      Serial.println("off");
      digitalWrite(D7, LOW);
    }
  }
}

//FUNCION PARA CONECTAR
void reconnect() {
  while (!client.connected()) {
    Serial.println("Intentando conexion MQTT");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if(client.connect(clientId.c_str())){
      Serial.println("Conectado");
      client.publish("outTopic", "hello world");
      client.subscribe("temperatura");
      client.subscribe("humedad");
      client.subscribe("humedad_suelo");
    } else{
      Serial.print("Conexion fallida, rc=");
      Serial.print(client.state());
      Serial.println(" Reintentando en 4 segundos");
      delay(4000);
    } 
   }
}




void setup() {
  Serial.begin(115200); //INICIALIZAMOS EL SERIAL
  setup_wifi(); //LLAMAMOS A LA FUNCIÓN PARA CONECTARNOS A WIFI
  while(!Serial){
    ;
  }

  dht.begin(); //INICIALIZAMOS EL DHT

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}


void loop() {
  
  if(!client.connected()){
    reconnect();
  }
  client.loop();
  
  tempe = dht.readTemperature(); //LEER LA TEMPERATURA
  hume = dht.readHumidity(); //LEER LA HUMEDAD
  hume_suelo = analogRead(hsp); //LEER LA HUMEDAD DEL SUELO

  dtostrf(tempe,0,0,dato1); client.publish("temperatura",dato1);
  dtostrf(hume,0,0,dato2); client.publish("humedad",dato2);
  dtostrf(hume_suelo,0,0,dato3); client.publish("humedad_suelo",dato3);
  

  
  delay (2000);
}
