#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_MAX31865.h>
#include <Servo.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h> // Inclusão da biblioteca WebServer para criar um servidor web


// ======================
// CONFIGURAÇÕES DE REDE
// ======================
const char* ssid = "Servicos_Gerais";
const char* password = "ifpr2024";
// const char* ssid = "Memphis";
// const char* password = "brasil123";

// ======================
// DEFINIÇÃO DE PINOS
// ======================
#define MAX31865_CS   15           // Pino D8
#define MAX31865_DI   13           // Pino D7
#define MAX31865_DO   12           // Pino D6
#define MAX31865_CLK  14           // Pino D5
#define TRIG_PIN 2                 // Pino D4 HC-SR04 Trig
#define ECHO_PIN 0                 // Pino D3 HC-SR04 Echo
#define PIN_FS300A 4               // Pino D2 Sensor de fluxo (pulsos)
#define PIN_BOIA_NIVEL 5           // Pino D1 Sensor boia de nível (digital)
#define SERVO_PIN 16               // Pino D0 Servo MG995

// ======================
// OBJETOS GLOBAIS
// ======================
Adafruit_MAX31865 max31865(MAX31865_CS, MAX31865_DI, MAX31865_DO, MAX31865_CLK);
//Adafruit_MAX31865 sensorRTD = Adafruit_MAX31865(MAX31865_CS, MAX31865_DI, MAX31865_DO, MAX31865_CLK);
//sensorRTD.begin(MAX31865_2WIRE);
ESP8266WebServer server(80);

Servo servo;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000); // GMT-3 Brasil

// ======================
// VARIÁVEIS GLOBAIS
// ======================
unsigned long last_measurement = 0;

// Alimentador
const float ALTURA_TOTAL_CM = 30.0;
const float DISTANCIA_MIN_CM = 2.0;
const int SERVO_FEED_ANGLE = 180;
const int SERVO_IDLE_ANGLE = 0;
const int SERVO_FEED_TIME_MS = 2000;
String ultimaAlimentacao = "--:--";
String proximaAlimentacao = "06:00";
bool alimentadoManha = false;
bool alimentadoTarde = false;
const int TEMPO_TOTAL_SEGUNDOS = 40;      // 12 horas = 43200 segundos
const int DELAY_SEGUNDOS = 20;               // Delay de 60 segundos (1 minuto)
const int NUMERO_ITERACOES = TEMPO_TOTAL_SEGUNDOS / DELAY_SEGUNDOS;  // 720 iterações


// ======================
// FUNÇÕES DO ALIMENTADOR
// ======================
void alimentarPeixes(String horario) {
  servo.write(SERVO_FEED_ANGLE);
  delay(SERVO_FEED_TIME_MS);
  servo.write(SERVO_IDLE_ANGLE);
  delay(SERVO_FEED_TIME_MS);
  servo.write(SERVO_FEED_ANGLE);
  delay(SERVO_FEED_TIME_MS);
  servo.write(SERVO_IDLE_ANGLE);
  ultimaAlimentacao = horario;
  if (horario == "06:00") proximaAlimentacao = "18:00";
  else proximaAlimentacao = "06:00";
  Serial.println("\nPeixes alimentados automaticamente às " + horario);
}

// ======================
// SETUP
// ======================
void setup() {


  Serial.begin(115200);

  servo.attach(SERVO_PIN);
  servo.write(SERVO_IDLE_ANGLE);


     WiFi.begin(ssid, password);
     Serial.print("Conectando ao WiFi...");
     while (WiFi.status() != WL_CONNECTED) {
       delay(500);
       Serial.print(".");
     }
     Serial.println("\nWiFi conectado! IP: " + WiFi.localIP().toString());

}

// ======================
// LOOP PRINCIPAL
// ======================
void loop() {
   
  server.handleClient();
  timeClient.update();
      Serial.println("Apos server e time update");
  

  int hora = timeClient.getHours();
  int minuto = timeClient.getMinutes(); 
  char horariolocal[6];
  sprintf(horariolocal, "%02d:%02d", hora, minuto);
  Serial.printf("Horário atual: %s", horariolocal);
  // Comparando o horário para alimentar os peixes
  if (String(horariolocal) == "07:25" ) {

    alimentarPeixes(String(horariolocal));
    

    // Exibe status da alimentação
  Serial.println("===== MONITORAMENTO GERAL =====");
  Serial.printf("Última alimentação: %s | Próxima: %s\n", ultimaAlimentacao.c_str(), proximaAlimentacao.c_str());
  Serial.println("===============================\n");

  // Mensagem final após completar 12 horas
  Serial.println("\n====================================");
  Serial.println("CONTAGEM CONCLUÍDA!");
  Serial.println("12 horas se passaram. Alimentando peixes");
  Serial.println("====================================\n");
  // alimentarPeixes("12:00");
  }

  

  // Delay para atualizar a cada minuto
  delay(30000);  // 60 segundos





    //alimentarPeixes("15:00");
  //   for (int i = 1; i <= NUMERO_ITERACOES; i++) {
  //   int segundos_decorridos = i * DELAY_SEGUNDOS;
    
  //   // Converte para horas, minutos e segundos
  //   int horas = segundos_decorridos / 3600;
  //   int minutos = (segundos_decorridos % 3600) / 60;
  //   int segundos = segundos_decorridos % 60;
    
  //   // Exibe o tempo decorrido formatado
  //   Serial.printf("Iteração %4d: %02d:%02d:%02d decorridos\n", i, horas, minutos, segundos);
    
  //   // Delay de 60 segundos (1 minuto)
  //   delay(DELAY_SEGUNDOS * 1000);  // Converte segundos para milissegundos
  // }
  
  

  Serial.println("\n====================================");
  
  // Aguarda 5 segundos antes de reiniciar a contagem
  delay(5000);
  

  }
///////////
