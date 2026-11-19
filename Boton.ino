#include <Thread.h>

const int pinBoton = 2;

volatile bool pulsado = false;
volatile unsigned long tiempoRebote = 0;

bool mismaPulsacion = false;

unsigned long tiempoInicial = 0;
unsigned long tiempoPulsacion = 0;

Thread hiloBoton;

void ISR_BOTON() {
  if (millis() - tiempoRebote >= 50) {
    tiempoRebote = millis();

    if (digitalRead(pinBoton) == LOW) {
      pulsado = true;
    }
    else {
      pulsado = false;
    }
  }
}

// Todo lo que antes estaba en loop()
void tareaBoton() {
  if (pulsado) {
    if (mismaPulsacion == false) {
      tiempoInicial = millis();
      mismaPulsacion = true;
    }
    Serial.println(millis() - tiempoInicial);
  }

  if (!pulsado && mismaPulsacion == true) {
    tiempoPulsacion = millis() - tiempoInicial;
    mismaPulsacion = false;

    if (tiempoPulsacion >= 2000 && tiempoPulsacion <= 3000) {
      Serial.println("Pulsacion de reinicio");
    }
    else if (tiempoPulsacion >= 5000) {
      Serial.println("Entrada de Admin");
    }
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(pinBoton, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinBoton), ISR_BOTON, CHANGE);

  hiloBoton.onRun(tareaBoton);
  hiloBoton.setInterval(100);   // se ejecuta cada 100 ms
}

void loop() {
  if (hiloBoton.shouldRun()) {
    hiloBoton.run();
  }
}
