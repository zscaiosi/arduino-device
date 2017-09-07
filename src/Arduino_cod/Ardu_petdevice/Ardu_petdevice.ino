
#include <ArduinoJson.h>
#include <Servo.h>

#define TAMANHO 200


Servo ServoRacao; 	
int racao = 0;


int racao_servo(int qtde_aberturas_servo, int tempo_abertura_servo ){
	


	
 for (int i=0; i <= qtde_aberturas_servo; i++){
    
    	
ServoRacao.write(90);
delay(tempo_abertura_servo);		
ServoRacao.write(0);


	
	
 };	
 

	
	
}


void Acao(int qtde_aberturas, int tempo_abertura ) {
	
if(qtde_aberturas <= 0 || tempo_abertura <= 0 ) {

Serial.println("ERRO RACAO, tempo de abertura ou numero de aberturas a baixo de 0");
} else {

 int racao = racao_servo(qtde_aberturas,tempo_abertura);

StaticJsonBuffer<TAMANHO> jsonBuffer;
JsonObject& json = jsonBuffer.createObject();
json["atividade"] = 1;
json["fechado"] = true;
json["qtde_abertura"] = qtde_aberturas;
json["tempo_abertura"] = (tempo_abertura)/1000;

json.printTo(Serial); Serial.println();


 
}



	
	
	
	
}




	
	


void setup() {

  // configura o data rate da SoftwareSerial
  Serial.begin(9600);
  Serial.println("iniciando");
  ServoRacao.attach(9);
  
ServoRacao.write(0);

}

void loop() { 

if(Serial.available() > 0 ) {

char texto[TAMANHO];

Serial.readBytesUntil('\n', texto, TAMANHO);
StaticJsonBuffer<TAMANHO> jsonBuffer;

JsonObject& json = jsonBuffer.parseObject(texto);
if(json.success() && json.containsKey("alimentar")) {
	
int qtde_aberturas_MQTT = json["qtde_aberturas"];
int tempo_abertura_MQTT = (atoi(json["tempo_abertura"])*1000);	
	
Acao(qtde_aberturas_MQTT,tempo_abertura_MQTT);

}

delay(1000);
	
	
	
}


}
