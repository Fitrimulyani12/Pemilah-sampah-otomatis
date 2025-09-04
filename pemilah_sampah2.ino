#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>  // Untuk membuat format JSON
// LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo, servoPintu;

// LED
#define LED_MERAH_UNORGANIK 36
#define LED_KUNING_UNORGANIK 38
#define LED_HIJAU_UNORGANIK 40
#define LED_MERAH_ORGANIK 42
#define LED_KUNING_ORGANIK 44
#define LED_HIJAU_ORGANIK 46

// Ultrasonic
#define TRIG_PIN_UNORGANIK 4
#define ECHO_PIN_UNORGANIK 2
#define TRIG_PIN_ORGANIK 30
#define ECHO_PIN_ORGANIK 31
#define TRIG_PIN_PINTU 10
#define ECHO_PIN_PINTU 8

// Sensor
#define SENSOR_INDUCTIVE 9
#define SENSOR_CAPACITIVE 11
#define SENSOR_INFRARED 15

// Buzzer
const int BUZZER_ORGANIK = 41;
const int BUZZER_UNORGANIK = 43;

// Servo Angles
const int Servo1_Open = 0;
const int Servo_Close = 90;
const int Servo2_Open = 180;
const int Servo3_Open = 150;
const int Servo3_Close = 0;

String Volume_Organik;
String Volume_Unorganik;

unsigned long duration_organik, cm_organik;
unsigned long duration_unorganik, cm_unorganik;
unsigned long duration_pintu, cm_pintu;

bool organikkosong = false, unorganikkosong = false;

// Fungsi Menampilkan Pesan ke LCD
void displayMessage(String line1, String line2) {
 lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);  // Komunikasi ke NodeMCU

  // Inisialisasi pin
  pinMode(BUZZER_ORGANIK, OUTPUT);
  pinMode(BUZZER_UNORGANIK, OUTPUT);


  pinMode(LED_MERAH_ORGANIK, OUTPUT);
  pinMode(LED_KUNING_ORGANIK, OUTPUT);
  pinMode(LED_HIJAU_ORGANIK, OUTPUT);

  pinMode(LED_MERAH_UNORGANIK, OUTPUT);
  pinMode(LED_KUNING_UNORGANIK, OUTPUT);
  pinMode(LED_HIJAU_UNORGANIK, OUTPUT);

  pinMode(TRIG_PIN_ORGANIK, OUTPUT);
  pinMode(ECHO_PIN_ORGANIK, INPUT);

  pinMode(TRIG_PIN_UNORGANIK, OUTPUT);
  pinMode(ECHO_PIN_UNORGANIK, INPUT);

  pinMode(TRIG_PIN_PINTU, OUTPUT);
  pinMode(ECHO_PIN_PINTU, INPUT);

  pinMode(SENSOR_INDUCTIVE, INPUT);
  pinMode(SENSOR_CAPACITIVE, INPUT);
  pinMode(SENSOR_INFRARED, INPUT);

//  servoOrganik.attach(7);
//  servoOrganik.write(Servo1_Close);
  servo.attach(5);
  servo.write(Servo_Close);
  servoPintu.attach(3);
  servoPintu.write(Servo3_Close);

  lcd.init();
  lcd.backlight();
  displayMessage("Sistem Pemilah", "Sampah Otomatis");
  delay(2000);
}

void cek_tinggi_sampah_organik() {
  digitalWrite(TRIG_PIN_ORGANIK, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN_ORGANIK, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN_ORGANIK, LOW);

  duration_organik = pulseIn(ECHO_PIN_ORGANIK, HIGH);
  cm_organik = duration_organik * 0.034 / 2;

  Serial.print(" jarak organik   ");
  Serial.println(cm_organik);

  if (cm_organik <= 7) {
    Volume_Organik = "Sampah Organik Penuh";
    Serial.println(Volume_Organik);
    displayMessage("Sampah Organik", "P E N U H");
    digitalWrite(LED_MERAH_ORGANIK, HIGH);
    digitalWrite(LED_HIJAU_ORGANIK, LOW);
    digitalWrite(LED_KUNING_ORGANIK, LOW);
    // tone(BUZZER_ORGANIK, 1000);
    organikkosong = false;
  } else if (cm_organik >= 10 && cm_organik < 19) {
    Volume_Organik = "Sampah Organik Hampir  Penuh";
    digitalWrite(LED_KUNING_ORGANIK, HIGH);
    digitalWrite(LED_MERAH_ORGANIK, LOW);
    digitalWrite(LED_HIJAU_ORGANIK, LOW);
    organikkosong = false;
  } else {
    organikkosong = true;
    Volume_Organik = "Sampah Organik Kosong";
     //lcd.clear();
    //displayMessage("Sistem Pemilah", "Sampah Otomatis");
    digitalWrite(LED_MERAH_ORGANIK, LOW);
    digitalWrite(LED_KUNING_ORGANIK, LOW);
    digitalWrite(LED_HIJAU_ORGANIK, HIGH);
    noTone(BUZZER_ORGANIK);
  }
}

void cek_tinggi_sampah_unorganik() {
  digitalWrite(TRIG_PIN_UNORGANIK, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN_UNORGANIK, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN_UNORGANIK, LOW);

  duration_unorganik = pulseIn(ECHO_PIN_UNORGANIK, HIGH);
  cm_unorganik = duration_unorganik * 0.034 / 2;
  Serial.print(" jarak unorganik  ");
  Serial.println(cm_unorganik);

  if (cm_unorganik <= 7) {
    Volume_Unorganik = "Sampah Unorganik Penuh";
    displayMessage("Sampah Unorganik", "P E N U H");
    Serial.println(Volume_Unorganik);
    digitalWrite(LED_MERAH_UNORGANIK, HIGH);
    digitalWrite(LED_HIJAU_UNORGANIK, LOW);
    digitalWrite(LED_KUNING_UNORGANIK, LOW);
    //tone(BUZZER_UNORGANIK, 1000);
    unorganikkosong = false;
  } else if (cm_unorganik >= 10 && cm_unorganik < 19) {
    Volume_Unorganik = "Sampah Unorganik Hampir Penuh";
    digitalWrite(LED_KUNING_UNORGANIK, HIGH);
    digitalWrite(LED_MERAH_UNORGANIK, LOW);
    digitalWrite(LED_HIJAU_UNORGANIK, LOW);
    unorganikkosong = false;
  } else if (cm_unorganik > 28) {
    unorganikkosong = true;
    Volume_Unorganik = "Sampah Unorganik Kosong";
    //displayMessage("Sistem Pemilah", "Sampah Otomatis");
    digitalWrite(LED_MERAH_UNORGANIK, LOW);
    digitalWrite(LED_KUNING_UNORGANIK, LOW);
    digitalWrite(LED_HIJAU_UNORGANIK, HIGH);
    noTone(BUZZER_UNORGANIK);
  }
}

void cek_sampah() {


  int sensorInductive = digitalRead(SENSOR_INDUCTIVE);
  int sensorCapacitive = digitalRead(SENSOR_CAPACITIVE);
  int sensorInfrared = digitalRead(SENSOR_INFRARED);
  Serial.print(" induktive  ");
  Serial.println(sensorInductive);
  Serial.print(" capasitive  ");
  Serial.println(sensorCapacitive);
  Serial.print(" infrared  ");
  Serial.println(sensorInfrared);
  
   if (sensorInductive == 1 && sensorCapacitive == 0 && sensorInfrared ==1 ) {
      displayMessage("Sistem Pemilah", "Masukan Sampah");
     servo.write(Servo_Close);
  } else if (sensorInductive == 1 && sensorCapacitive == 1 && sensorInfrared == 0 ) {
    displayMessage("Sistem Pemilah", "Sampah Organik");
    //servoOrganik.write(Servo2_Open);
    servo.write(Servo1_Open );
    delay(3000);
    servo.write(Servo_Close);
  } else if (sensorInductive == 1 && sensorCapacitive == 1 && sensorInfrared == 1 ) {
    displayMessage("Sistem Pemilah", "Sampah Organik");
    servo.write(Servo1_Open);
   // servoUnorganik.write(Servo1_Close );
    delay(3000);
    servo.write(Servo_Close);
  } else if (sensorInductive == 0 && sensorCapacitive == 0 && sensorInfrared == 1) {
    displayMessage("Sistem Pemilah", "Sampah Unorganik");
    servo.write(Servo2_Open);
    //servoOrganik.write(Servo2_Close);x
    delay(3000);
   servo.write(Servo_Close);
  } else if (sensorInductive == 1 && sensorCapacitive == 0 && sensorInfrared == 0) {
    displayMessage("Sistem Pemilah", "Sampah Unorganik");
    servo.write(Servo2_Open);
   // servoOrganik.write(Servo2_Close);
    delay(3000);
   servo.write(Servo_Close);
  }
  else if (sensorInductive == 0 && sensorCapacitive == 0 && sensorInfrared == 0) {
    displayMessage("Sistem Pemilah", "Sampah Unorganik");
    servo.write(Servo2_Open);
    delay(3000);
   servo.write(Servo_Close);
    }
}

void cek_orang() {
  digitalWrite(TRIG_PIN_PINTU, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN_PINTU, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN_PINTU, LOW);

  duration_pintu = pulseIn(ECHO_PIN_PINTU, HIGH);
  cm_pintu = duration_pintu * 0.034 / 2;
  Serial.print(" pintu  ");
  Serial.println(cm_pintu);
  if (cm_pintu <= 28 && organikkosong && unorganikkosong) {
    displayMessage(" Silahkan Masukan", "S A M P A H");
    servoPintu.write(Servo3_Open);  // Buka pintu
    delay(1500);
    cek_sampah();
  } else {
    servoPintu.write(Servo3_Close);  // Tutup pintu
  }
}

void kirim_data_ke_nodeMCU() {
  StaticJsonDocument<200> doc;
  doc["volume_organik"] = Volume_Organik;
  doc["volume_unorganik"] = Volume_Unorganik;

  String output;
  serializeJson(doc, output);
  Serial2.println(output);
  Serial.println(output);
}


void loop() {
  cek_orang();
  cek_tinggi_sampah_organik();
  cek_tinggi_sampah_unorganik();
  kirim_data_ke_nodeMCU();
  if (organikkosong && unorganikkosong) {
  displayMessage("Sistem Pemilah", "Sampah Otomatis");
  }
    else if (!organikkosong) {
    displayMessage("Sampah Organik", "P E N U H");
  }
    else if (!unorganikkosong) {
    displayMessage("Sampah Unorganik", "P E N U H");
  }
  delay(1000);
}
