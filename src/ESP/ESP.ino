
            
#include <ESP8266WiFi.h>        //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>        //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>
#include <PubSubClient.h>

#define DEBUG                   //Se descomentar esta linha vai habilitar a 'impressÃ£o' na porta serial

//Coloque os valores padrÃµes aqui, porÃ©m na interface eles poderÃ£o ser substituÃ­dos.
#define servidor_mqtt             "m12.cloudmqtt.com"  //URL do servidor MQTT
#define servidor_mqtt_porta       "12913"  //Porta do servidor (a mesma deve ser informada na variÃ¡vel abaixo)
#define servidor_mqtt_usuario     "jzzvmmrz"  //UsuÃ¡rio
#define servidor_mqtt_senha       "JjgVV2dVIaTR"  //Senha
#define mqtt_topico_sub           "pote/racao/id101/alimentar"    //TÃ³pico para subscrever o comando a ser dado no pino declarado abaixo
#define mqtt_topico_pub           "pote/racao/id101/atividades" 
#define TAMANHO 300

WiFiClient espClient;                                 //InstÃ¢ncia do WiFiClient
PubSubClient client(espClient);                       //Passando a instÃ¢ncia do WiFiClient para a instÃ¢ncia do PubSubClient


//FunÃ§Ã£o para imprimir na porta serial
void imprimirSerial(bool linha, String mensagem){
  #ifdef DEBUG
    if(linha){
      Serial.println(mensagem);
    }else{
      Serial.print(mensagem);
    }
  #endif
}



//FunÃ§Ã£o que reconecta ao servidor MQTT
void reconectar() {
  //Repete atÃ© conectar
  while (!client.connected()) {
    imprimirSerial(false, "Tentando conectar ao servidor MQTT...");
    
    //Tentativa de conectar. Se o MQTT precisa de autenticaÃ§Ã£o, serÃ¡ chamada a funÃ§Ã£o com autenticaÃ§Ã£o, caso contrÃ¡rio, chama a sem autenticaÃ§Ã£o. 
    bool conectado = strlen(servidor_mqtt_usuario) > 0 ?
                     client.connect("ESP8266Client", servidor_mqtt_usuario, servidor_mqtt_senha) :
                     client.connect("ESP8266Client");

    if(conectado) {
      imprimirSerial(true, "Conectado!");
      //Subscreve para monitorar os comandos recebidos
      client.subscribe(mqtt_topico_sub, 1); //QoS 1
    } else {
      imprimirSerial(false, "Falhou ao tentar conectar. Codigo: ");
      imprimirSerial(false, String(client.state()).c_str());
      imprimirSerial(true, " tentando novamente em 5 segundos");
      //Aguarda 5 segundos para tentar novamente
      delay(5000);
    }
  }
}



//FunÃ§Ã£o que serÃ¡ chamada ao receber mensagem do servidor MQTT
void retorno(char* topico, byte* mensagem, unsigned int tamanho) {
  //Convertendo a mensagem recebida para string
  mensagem[tamanho] = '\0';
  String strMensagem = String((char*)mensagem);
  strMensagem.toLowerCase();
  //float f = s.toFloat();

  imprimirSerial(true, strMensagem);

}





void publica() {
  if (!client.connected()) {
    imprimirSerial(true, "MQTT desconectado! Tentando reconectar...");
    reconectar();
  }
  
  client.loop();


if (Serial.available() > 0) {
//Lê o texto disponível na porta serial:
char texto[TAMANHO];
Serial.readBytesUntil('\n', texto, TAMANHO);
//Grava o texto recebido como JSON
StaticJsonBuffer<TAMANHO> jsonBuffer;
JsonObject& json = jsonBuffer.parseObject(texto);
if(json.success() && json.containsKey("atividade")) {

int atividades_conv =0;
int qtde_aberturas_conv =0;

atividades_conv = json["atividade"];
int fechado_conv = json["fechado"] ;
qtde_aberturas_conv = json["qtde_abertura"];
int tempo_abertura_conv  = json["tempo_abertura"];

Serial.println(atividades_conv);
Serial.println(fechado_conv);
Serial.println(qtde_aberturas_conv);
Serial.println(tempo_abertura_conv);


String json_pub = "{\"atividades\":\"" + String(atividades_conv)+ "\",""\"fechado\":\"" + String(fechado_conv)+ "\",""\"qtde_aberturas\":\"" + String(qtde_aberturas_conv)+ "\",""\"tempo_abertura\":\"" + String(tempo_abertura_conv)+ "\"}";
char message_buff[100];
json_pub.toCharArray(message_buff, json_pub.length()+1);
  imprimirSerial(true, "Fazendo a publicacao...");
  client.publish(mqtt_topico_pub, message_buff);
}

  
  
}

}




//FunÃ§Ã£o inicial (serÃ¡ executado SOMENTE quando ligar o ESP)
void setup(){
  #ifdef DEBUG
    Serial.begin(9600);
  #endif
  imprimirSerial(true, "...");

 WiFiManager wifiManager;
 
  if (!wifiManager.autoConnect("AutoConnectAP", "senha123")) {
    imprimirSerial(true, "Falha ao conectar. Excedeu o tempo limite para conexao.");
    delay(3000);
  }

  //Se chegou atÃ© aqui Ã© porque conectou na WiFi!
  imprimirSerial(true, "Conectado!! :)");

  

  imprimirSerial(false, "IP: ");
  imprimirSerial(true, WiFi.localIP().toString());

  //Informando ao client do PubSub a url do servidor e a porta.
  int portaInt = atoi(servidor_mqtt_porta);
  client.setServer(servidor_mqtt, portaInt);
  client.setCallback(retorno);

}

//FunÃ§Ã£o de repetiÃ§Ã£o (serÃ¡ executado INFINITAMENTE atÃ© o ESP ser desligado)
void loop() {
  if (!client.connected()) {
    reconectar();
  }

  publica();

client.loop();

  

}
