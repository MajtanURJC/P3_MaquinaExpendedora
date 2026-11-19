const int pinJoystickX = A0;
const int pinJoystickY = A1;
const int pinJoystickSW = 22;

int valorX = 0;
int valorY = 0;
int valorSW = 0;

void setup() {
  Serial.begin(9600);

  pinMode(pinJoystickSW, INPUT_PULLUP);
}

void loop() {
  valorX = analogRead(pinJoystickX);
  valorY = analogRead(pinJoystickY);
  valorSW = digitalRead(pinJoystickSW);

  Serial.print("Eje X: ");
  Serial.print(valorX);

  Serial.print(" | Eje Y: ");
  Serial.print(valorY);

  Serial.print(" | Pulsador: ");
  
  if (valorSW == LOW) {
    Serial.println("PULSADO");
  } else {
    Serial.println("SIN PULSAR");
  }

  delay(200);
}
