#include "Arduino.h"
#include <SPI.h>
#include <Ethernet.h>
#include <LiquidCrystal.h>

// Humedad con sensor al aire
int SIN_HUMEDAD = 1022;
// Humedad maxima del sensor
int MAX_HUMEDAD = 0;

// Se definen 2 niveles de humedad
int HUMEDAD_MEDIA = 50;
int HUMEDAD_BAJA = 25;

// Se definen los pulsadores
int PIN_BOTON_APAGAR = 6;
int PIN_BOTON_NIVEL = 7;

// Se define el LED
int PIN_LED = 8;

// Se define el LCD
int RS = A1;
int E = A2;
int D4 = A3;
int D5 = A4;
int D6 = A5;
int D7 = 2;

// Se inicializa la libreria LiquidCrystal con los pins utilizados en la interface
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

// Se define el cliente Ethernet y el servidor SMTP
// MAC address de Arduino Ethernet W5100
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// Nombre de dominio del servidor SMTP
char server[] = "mail.smtp2go.com";
// IP address de la tarjeta de red de Arduino Ethernet W5100
IPAddress ip(192, 168, 1, 50);
// IP address del router
IPAddress gateway(192, 168, 1, 1);
// Mascara de la subred
IPAddress subnet(255, 255, 255, 0);
// Instanciacion del cliente que se utilizara para enviar el email
EthernetClient client;

void setup() {

  // Inicializacion del modulo Serial
  Serial.begin(9600);
  while(!Serial) {
    ;  
  };

  // Inicializacion de los Pulsadores
  pinMode(PIN_BOTON_APAGAR, INPUT);
  pinMode(PIN_BOTON_NIVEL, INPUT);

  // Inicializacion del LED
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);

  // Inicializacion del LCD
  lcd.begin(16, 2);

  // Se define el texto de la primera fila del LCD
  lcd.setCursor(0,0);
  lcd.print("Nivel de Humedad");

  // Se intenta configurar Ethernet mediante DHCP
  if(Ethernet.begin(mac) == 0) {
    Serial.println("No se pudo configurar Ethernet mediante DHCP.");
    // Se lo configura con la IP por defecto
    Ethernet.begin(mac, ip, gateway, subnet);
  }

  Serial.println("Sistema pronto!");
}

bool notificacionHabilitada = false;
bool notificado = false;
int nivelHumedad = 0;
int nivelAceptable = HUMEDAD_MEDIA;

void loop() {
  
  // Se verifica si existe un cambio en la habilitacion de notificaciones
  verificarHabilitacionDeNotificaciones();

  // Se verifica si existe un cambio en el nivel aceptable de humedad
  verificarNivelDeHumedadAceptable();
  
  // Se lee el dato provisto por el sensor de humedad  
  nivelHumedad = analogRead(A0);
  Serial.println(nivelHumedad);

  // Se realiza un porcentaje utilizando el máximo y el mínimo de humedad permitido
  nivelHumedad = map(nivelHumedad, MAX_HUMEDAD, SIN_HUMEDAD, 100, 0);

  // Se mantiene el nivel de humedad dentro de rango
  if(nivelHumedad > 100)
    nivelHumedad = 100;
  else if(nivelHumedad < 0)
    nivelHumedad = 0;

  // Se muestra en pantalla el nivel de humedad en porcentaje
  lcd.setCursor(0,1);
  lcd.print(nivelHumedad);
  lcd.print("%  ");
  Serial.println(nivelHumedad);
  
  // Si el nivel de humedad es menor al nivel aceptable
  // y si no está notificado, se envia email
  if (notificacionHabilitada && !notificado && (nivelHumedad < nivelAceptable)) {
    //apagarLCD();
    
    if (enviarMail()) {
      Serial.println("Notificacion Deshabilitada.");
      notificado = true;  
    }
  }

  // Si el nivel de humedad es superior al nivel aceptable
  // y ya fue notificado el cliente, se resetea la notificacion
  if (notificado && nivelHumedad >= nivelAceptable) {
    Serial.println("Notificacion Habilitada.");
    notificado = false;  
  }

  // Se realizar el monitoreo cada 5 segundos
  delay(1000);
}

/* Se verifica si existe un cambio en la habilitacion de notificaciones */

void verificarHabilitacionDeNotificaciones() {
  
  if (digitalRead(PIN_BOTON_APAGAR)) {
    notificacionHabilitada = !notificacionHabilitada;
    if (notificacionHabilitada) {
      digitalWrite(PIN_LED, LOW);
      Serial.println("Notificacion Habilitada.");
    } else {
      digitalWrite(PIN_LED, HIGH);
      Serial.println("Notificacion Deshabilitada.");
    }
  }
}

/* Se verifica si existe un cambio en el nivel aceptable de humedad */

void verificarNivelDeHumedadAceptable() {
  
  if (digitalRead(PIN_BOTON_NIVEL)) {
    if (nivelAceptable == HUMEDAD_MEDIA) {
      nivelAceptable = HUMEDAD_BAJA;
      Serial.println("Nivel aceptable de humedad es hasta un 25%");
    } else {
      nivelAceptable = HUMEDAD_MEDIA;
      Serial.println("Nivel aceptable de humedad es hasta un 50%");
    }
  }
}

/* Envia mail notificando que se alcanzo al 50 pct del nivel de humedad determinado */

byte enviarMail() {
  
  delay(200);
  Serial.println("Conectando..");

  // Se intenta conectar con el servidor SMTP
  if(client.connect(server, 25)) {
    Serial.println("Conexion exitosa!");
  }else {
    Serial.println("Conexion fallida!");
    return 0;
  };

  Serial.println("Enviando correo..");
  
  // Se realizan los comandos del protocolo SMTP
  client.println("ehlo");
  if (!conseguirRespuesta())return 0;
  
  client.println("auth login");
  if (!conseguirRespuesta())return 0;
  
  client.println("Replace for your SMTP2GO account in base64");
  if (!conseguirRespuesta())return 0;
  
  client.println("Replace for your SMTP2GO password in base64");
  if (!conseguirRespuesta())return 0;
  
  client.println("MAIL From: <Replace for your mail>");
  if (!conseguirRespuesta())return 0;
  
  client.println("RCPT TO: <Replace for destination mail>");
  if (!conseguirRespuesta())return 0;
  
  client.println("DATA");
  if (!conseguirRespuesta())return 0;
  
  client.println("From: <Replace for your mail>");
  client.println("TO: <Replace for destination mail>");
  client.println("SUBJECT: Alerta! Se ha alcanzado un nivel de humdedad insuficiente!");
  client.println("Se ha alcanzado un nivel de humdedad insuficiente!");
  client.println(".");
  if (!conseguirRespuesta())return 0;
  Serial.println("Correo enviado!");

  // Se desconecta del servidor de correo
  Serial.println("Desconectando..");
  client.println("QUIT");
  if (!conseguirRespuesta()) return 0;

  // Se detiene el cliente
  client.stop();
  Serial.println("Desconectado!");

  return 1;
};

/* Metodo que consigue la respuesta del servidor de correo */

byte conseguirRespuesta() {

  // Si no se obtiene respuesta, se desconecta el cliente
  if (esperarRespuesta() == 0) {
    client.stop();
    return 0;
  }

  // Se detecta el primer caracter del codigo de respuesta, sin leerlo totalmente
  char respCode = client.peek();

  // Se muestra el codigo de respuesta
  esperarRespuesta();

  // Si el primer caracter es 4, es porque se detecto un problema, entonces se cierra la sesion
  if (respCode >= '4') {
    Serial.println("Se detecto un error, desconectando..");
    client.println("QUIT");
    if (esperarRespuesta()) {
      mostrarRespuesta();
    }
    client.stop();
    Serial.println("Desconectado.");
    return 0;
  }

  return 1;
};

/* Metodo que espera hasta detectar una respuesta o un timeou del servidor de correo */

int esperarRespuesta() {
  
  unsigned long tiempoMax = millis() + 10000;
  while (!client.available()) {
    delay(1);
    // Si no se recibe nada por 10 segundos, se considera timeout
    if (millis() > tiempoMax) {
      Serial.println("Timeout");
      return 0;
    }
  }

  return 1;
};

/* Metodo que muestra las respuestas obtenidas desde el servidor de correo */

void mostrarRespuesta() {
  
  while (client.available()) {
    char thisByte = client.read();
    Serial.write(thisByte);
  }
};
