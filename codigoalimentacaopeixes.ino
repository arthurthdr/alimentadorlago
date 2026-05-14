#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_MAX31865.h>
#include <Servo.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// ======================
// CONFIGURAÇÕES DE REDE
// ======================
const char* ssid = "...";
const char* password = "zoio2540";

// ======================
// DEFINIÇÃO DE PINOS
// ======================
#define PIN_BOIA_NIVEL 14
#define TRIG_PIN 2
#define ECHO_PIN 0
#define PIN_FS300A 4
#define SERVO_PIN 5

// ======================
// OBJETOS GLOBAIS
// ======================
ESP8266WebServer server(80);
Servo servo;
WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000);

// ======================
// HORÁRIOS DA ALIMENTAÇÃO
// ======================
int hora1 = 8;
int minuto1 = 0;

int hora2 = 18;
int minuto2 = 0;

// ======================
// CONTROLE DIÁRIO
// ======================
bool alimentou1 = false;
bool alimentou2 = false;

String ultimaAlimentacao = "--:--";

// ======================
// VARIÁVEIS DOS SENSORES
// ======================
float temperatura = 0;
int nivel = 0;
float fluxo = 0;
float vazao = 0;

// ======================
// FUNÇÃO ALIMENTADOR
// ======================
void liberarRacao() {

  Serial.println("Liberando ração...");

  // PRIMEIRA VOLTA
  servo.write(180);
  delay(1000);

  // PAUSA
  servo.write(90);
  delay(500);

  // SEGUNDA VOLTA
  servo.write(180);
  delay(1000);

  // PARA
  servo.write(90);

  ultimaAlimentacao =
    String(timeClient.getHours()) + ":" +
    (timeClient.getMinutes() < 10 ? "0" : "") +
    String(timeClient.getMinutes());

  Serial.println("Ração liberada!");
}

// ======================
// PÁGINA WEB
// ======================
void paginaPrincipal() {

  String html = R"rawliteral(

<!DOCTYPE html>
<html lang="pt-br">

<head>

<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">

<title>Monitoramento do Lago</title>

<style>

body{
  margin:0;
  font-family:Arial;
  background:#0f172a;
  color:white;
}

.topo{
  background:#1e293b;
  padding:20px;
  text-align:center;
}

.monitoramento{
  padding:30px;
}

.grid{
  display:grid;
  grid-template-columns:repeat(auto-fit,minmax(250px,1fr));
  gap:20px;
}

.card{
  background:#1e293b;
  padding:20px;
  border-radius:20px;
  text-align:center;
}

.card h3{
  margin-bottom:10px;
}

.card p{
  font-size:32px;
  color:#38bdf8;
}

.destaque{
  border:2px solid #38bdf8;
}

footer{
  text-align:center;
  padding:20px;
  margin-top:30px;
}

button{
  background:#38bdf8;
  border:none;
  padding:15px 25px;
  border-radius:10px;
  font-size:18px;
  cursor:pointer;
}

</style>

</head>

<body>

<header class="topo">

<div class="titulo">
<h1>Lago Tecnológico IFPR</h1>
<p>Painel de Monitoramento</p>
</div>

</header>

<section class="monitoramento">

<h2>Dados em Tempo Real</h2>

<div class="grid">

<div class="card">
<h3>🌡 Temperatura</h3>
<p>)rawliteral";

  html += String(temperatura);
  html += R"rawliteral( °C</p>
<span>Faixa ideal: 22°C - 28°C</span>
</div>

<div class="card">
<h3>💧 Nível da Água</h3>
<p>)rawliteral";

  html += String(nivel);
  html += R"rawliteral( %</p>
<span>Status do reservatório</span>
</div>

<div class="card">
<h3>🌊 Fluxo</h3>
<p>)rawliteral";

  html += String(fluxo);
  html += R"rawliteral( L/min</p>
<span>Circulação da bomba</span>
</div>

<div class="card">
<h3>🚰 Vazão</h3>
<p>)rawliteral";

  html += String(vazao);
  html += R"rawliteral( L/min</p>
<span>Entrada/Saída de água</span>
</div>

<div class="card destaque">
<h3>🐟 Última Alimentação</h3>
<p>)rawliteral";

  html += ultimaAlimentacao;

  html += R"rawliteral(</p>
<span>Controle automático</span>

<br><br>

<a href="/alimentar">
<button>Alimentar Agora</button>
</a>

</div>

</div>

</section>

<footer>
IFPR - Assis Chateaubriand
</footer>

</body>
</html>

)rawliteral";

  server.send(200, "text/html", html);
}

// ======================
// ALIMENTAÇÃO MANUAL
// ======================
void alimentarManual() {

  liberarRacao();

  server.sendHeader("Location", "/");
  server.send(303);
}

// ======================
// SETUP
// ======================
void setup() {

  Serial.begin(115200);

  // SERVO
  servo.attach(SERVO_PIN);
  servo.write(90);

  // WIFI
  WiFi.begin(ssid, password);

  Serial.print("Conectando WiFi");

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado!");

  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // NTP
  timeClient.begin();

  // ROTAS WEB
  server.on("/", paginaPrincipal);

  server.on("/alimentar", alimentarManual);

  server.begin();

  Serial.println("Servidor iniciado!");
}

// ======================
// LOOP PRINCIPAL
// ======================
void loop() {

  server.handleClient();

  timeClient.update();

  int horaAtual = timeClient.getHours();
  int minutoAtual = timeClient.getMinutes();

  // ======================
  // PRIMEIRA ALIMENTAÇÃO
  // ======================
  if (horaAtual == hora1 &&
      minutoAtual == minuto1 &&
      !alimentou1) {

    liberarRacao();
    alimentou1 = true;
  }

  // ======================
  // SEGUNDA ALIMENTAÇÃO
  // ======================
  if (horaAtual == hora2 &&
      minutoAtual == minuto2 &&
      !alimentou2) {

    liberarRacao();
    alimentou2 = true;
  }

  // ======================
  // RESET DIÁRIO
  // ======================
  if (horaAtual == 0 &&
      minutoAtual == 0) {

    alimentou1 = false;
    alimentou2 = false;
  }

  delay(1000);
}