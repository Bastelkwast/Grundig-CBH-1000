#include <Arduino.h>
/*
   Version 5!
   Hier habe ich eine Schalterauswertung für das Funkgerät von Grundig CBH 1000 gebaut.
   Kanäle sind auf 80 begrenzt.
   Kanal und Frequenz werden auf dem OLED ausgegeben.
   Scan Funktion eingebaut

   Anschlussplan:
   Wahlschalter:
   Braun  ->  10
   rot    ->  11
   Orange ->  12
   Gelb   ->  A0
   Grün   ->  A1
   Blau   ->  A2
   Lila   ->  GND

   Sender Stumschaltung ->  13
   Sensor Scan          ->  A6

   PLL:
   Pins 2,3,4,5,6,7,8,9

   The pins für I2C sind in der Wire-library definiert.
   On an arduino Nano:       A4(SDA), A5(SCL)
*/

#include <EEPROM.h>
#include <Wire.h>
#include "SSD1306Ascii.h" // SSD1306ASCII von Bill Greiman
#include "SSD1306AsciiWire.h"
#define I2C_ADDRESS 0x3C

SSD1306AsciiWire oled;

int kanal = 1;
int sw_pos = 1;
int sw_pos_temp = 1;
float freq_ist = 26.965;

// Ausgang zur PLL
int bit_pll[8] = {2, 3, 4, 5, 6, 7, 8, 9};

// Bits für die Byte Auswertung
boolean bit0 = 0;
boolean bit1 = 0;
boolean bit2 = 0;
boolean bit3 = 0;
boolean bit4 = 0;
boolean bit5 = 0;
boolean bit6 = 0;
boolean bit7 = 0;

// Pins vom Schaltermodul
const int Pin1 = 10; // Braun
const int Pin2 = 11; // Rot
const int Pin3 = 12; // Orange
const int Pin4 = A0; // Gelb
const int Pin5 = A1; // Grün
const int Pin6 = A2; // Blau // Stummschaltung für den Sender vom Schalter
// Pin 7 = Masse // Lila
const int stumm_TX = 13; // Ausgang Stummschaltung für den Sender

// Für den Scannerteil
const int scan_sw = A3;   // Schalter für den Scanmodus
const int scan_sens = A6; // Sensoranschluss für den Scanmodus
int schwellwert = 500;    // Schwellwert an dem der Suchlauf umschaltet
int time_high = 5000;     // Zeit beim Suchlauf beim Empfang einen Senders
int time_low = 50;        // Zeit beim Suchlauf ohne Empfang

// Zustand der Eingangspins
boolean Input1 = 0;
boolean Input2 = 0;
boolean Input3 = 0;
boolean Input4 = 0;
boolean Input5 = 0;
boolean Input6 = 0; // Impulsgeber
boolean pos_true = true;

// Hier Werden die Frequenzen dem Kanal zugeordnet
float freq_Set[81] = {
    00.000,
    /*Kanal 1*/ 26.965,
    /*Kanal 2*/ 27.975,
    /*Kanal 3*/ 27.985,
    /*Kanal 4*/ 27.005,
    /*Kanal 5*/ 27.015,
    /*Kanal 6*/ 27.025,
    /*Kanal 7*/ 27.035,
    /*Kanal 8*/ 27.055,
    /*Kanal 9*/ 27.065,
    /*Kanal 10*/ 27.075,
    /*Kanal 11*/ 27.085,
    /*Kanal 12*/ 27.105,
    /*Kanal 13*/ 27.115,
    /*Kanal 14*/ 27.125,
    /*Kanal 15*/ 27.135,
    /*Kanal 16*/ 27.155,
    /*Kanal 17*/ 27.165,
    /*Kanal 18*/ 27.175,
    /*Kanal 19*/ 27.185,
    /*Kanal 20*/ 27.205,
    /*Kanal 21*/ 27.215,
    /*Kanal 22*/ 27.225,
    /*Kanal 23*/ 27.255,
    /*Kanal 24*/ 27.235,
    /*Kanal 25*/ 27.245,
    /*Kanal 26*/ 27.265,
    /*Kanal 27*/ 27.275,
    /*Kanal 28*/ 27.285,
    /*Kanal 29*/ 27.295,
    /*Kanal 30*/ 27.305,
    /*Kanal 31*/ 27.315,
    /*Kanal 32*/ 27.325,
    /*Kanal 33*/ 27.335,
    /*Kanal 34*/ 27.345,
    /*Kanal 35*/ 27.355,
    /*Kanal 36*/ 27.365,
    /*Kanal 37*/ 27.375,
    /*Kanal 38*/ 27.385,
    /*Kanal 39*/ 27.395,
    /*Kanal 40*/ 27.405,
    /*Kanal 41*/ 26.565,
    /*Kanal 42*/ 26.575,
    /*Kanal 43*/ 26.585,
    /*Kanal 44*/ 26.595,
    /*Kanal 45*/ 26.605,
    /*Kanal 46*/ 26.615,
    /*Kanal 47*/ 26.625,
    /*Kanal 48*/ 26.635,
    /*Kanal 49*/ 26.645,
    /*Kanal 50*/ 26.655,
    /*Kanal 51*/ 26.665,
    /*Kanal 52*/ 26.675,
    /*Kanal 53*/ 26.685,
    /*Kanal 54*/ 26.695,
    /*Kanal 55*/ 26.705,
    /*Kanal 56*/ 26.715,
    /*Kanal 57*/ 26.725,
    /*Kanal 58*/ 26.735,
    /*Kanal 59*/ 26.745,
    /*Kanal 60*/ 26.755,
    /*Kanal 61*/ 26.765,
    /*Kanal 62*/ 26.775,
    /*Kanal 63*/ 26.785,
    /*Kanal 64*/ 26.795,
    /*Kanal 65*/ 26.805,
    /*Kanal 66*/ 26.815,
    /*Kanal 67*/ 26.825,
    /*Kanal 68*/ 26.835,
    /*Kanal 69*/ 26.845,
    /*Kanal 70*/ 26.855,
    /*Kanal 71*/ 26.865,
    /*Kanal 72*/ 26.875,
    /*Kanal 73*/ 26.885,
    /*Kanal 74*/ 26.895,
    /*Kanal 75*/ 26.905,
    /*Kanal 76*/ 26.915,
    /*Kanal 77*/ 26.925,
    /*Kanal 78*/ 26.935,
    /*Kanal 79*/ 26.945,
    /*Kanal 80*/ 26.955,
};

// Hier wird der Binärcode für die PLL dem Kanal zugeordnet
byte pll_set[81] = {
    B00000000,
    /*Kanal 1*/ B10000000,
    /*Kanal 2*/ B10000001,
    /*Kanal 3*/ B10000010,
    /*Kanal 4*/ B10000100,
    /*Kanal 5*/ B10000101,
    /*Kanal 6*/ B10000110,
    /*Kanal 7*/ B10000111,
    /*Kanal 8*/ B10001001,
    /*Kanal 9*/ B10001010,
    /*Kanal 10*/ B10001011,
    /*Kanal 11*/ B10001100,
    /*Kanal 12*/ B10001110,
    /*Kanal 13*/ B10001111,
    /*Kanal 14*/ B10010000,
    /*Kanal 15*/ B10010001,
    /*Kanal 16*/ B10010011,
    /*Kanal 17*/ B10010100,
    /*Kanal 18*/ B10010101,
    /*Kanal 19*/ B10010110,
    /*Kanal 20*/ B10011000,
    /*Kanal 21*/ B10011001,
    /*Kanal 22*/ B10011010,
    /*Kanal 23*/ B10011101,
    /*Kanal 24*/ B10011011,
    /*Kanal 25*/ B10011100,
    /*Kanal 26*/ B10011110,
    /*Kanal 27*/ B10011111,
    /*Kanal 28*/ B10100000,
    /*Kanal 29*/ B10100001,
    /*Kanal 30*/ B10100010,
    /*Kanal 31*/ B10100011,
    /*Kanal 32*/ B10100100,
    /*Kanal 33*/ B10100101,
    /*Kanal 34*/ B10100110,
    /*Kanal 35*/ B10100111,
    /*Kanal 36*/ B10101000,
    /*Kanal 37*/ B10101001,
    /*Kanal 38*/ B10101010,
    /*Kanal 39*/ B10101011,
    /*Kanal 40*/ B10101100,
    /*Kanal 41*/ B01011000,
    /*Kanal 42*/ B01011001,
    /*Kanal 43*/ B01011010,
    /*Kanal 44*/ B01011011,
    /*Kanal 45*/ B01011100,
    /*Kanal 46*/ B01011101,
    /*Kanal 47*/ B01011110,
    /*Kanal 48*/ B01011111,
    /*Kanal 49*/ B01100000,
    /*Kanal 50*/ B01100001,
    /*Kanal 51*/ B01100010,
    /*Kanal 52*/ B01100011,
    /*Kanal 53*/ B01100100,
    /*Kanal 54*/ B01100101,
    /*Kanal 55*/ B01100110,
    /*Kanal 56*/ B01100111,
    /*Kanal 57*/ B01101000,
    /*Kanal 58*/ B01101001,
    /*Kanal 59*/ B01101010,
    /*Kanal 60*/ B01101011,
    /*Kanal 61*/ B01101100,
    /*Kanal 62*/ B01101101,
    /*Kanal 63*/ B01101110,
    /*Kanal 64*/ B01101111,
    /*Kanal 65*/ B01110000,
    /*Kanal 66*/ B01110001,
    /*Kanal 67*/ B01110010,
    /*Kanal 68*/ B01110011,
    /*Kanal 69*/ B01110100,
    /*Kanal 70*/ B01110101,
    /*Kanal 71*/ B01110110,
    /*Kanal 72*/ B01110111,
    /*Kanal 73*/ B01111000,
    /*Kanal 74*/ B01111001,
    /*Kanal 75*/ B01111010,
    /*Kanal 76*/ B01111011,
    /*Kanal 77*/ B01111100,
    /*Kanal 78*/ B01111101,
    /*Kanal 79*/ B01111110,
    /*Kanal 80*/ B01111111,

};

// Ermittlung der Schalterposition des Wahlschalters
void check_pos()
{
  Input1 = digitalRead(Pin1);
  Input2 = digitalRead(Pin2);
  Input3 = digitalRead(Pin3);
  Input4 = digitalRead(Pin4);
  Input5 = digitalRead(Pin5);

  // Position 1
  if (Input1 == 0 && Input2 == 0 && Input3 == 0 && Input4 == 0 && Input5 == 1)
  {
    sw_pos = 1;
  }
  // Position 2
  if (Input1 == 1 && Input2 == 1 && Input3 == 1 && Input4 == 1 && Input5 == 0)
  {
    sw_pos = 2;
  }
  // Position 3
  if (Input1 == 0 && Input2 == 1 && Input3 == 1 && Input4 == 1 && Input5 == 0)
  {
    sw_pos = 3;
  }
  // Position 4
  if (Input1 == 1 && Input2 == 1 && Input3 == 0 && Input4 == 1 && Input5 == 1)
  {
    sw_pos = 4;
  }
  // Position 5
  if (Input1 == 0 && Input2 == 1 && Input3 == 0 && Input4 == 1 && Input5 == 1)
  {
    sw_pos = 5;
  }
  // Position 6
  if (Input1 == 1 && Input2 == 0 && Input3 == 0 && Input4 == 1 && Input5 == 1)
  {
    sw_pos = 6;
  }
  // Position 7
  if (Input1 == 0 && Input2 == 0 && Input3 == 0 && Input4 == 1 && Input5 == 1)
  {
    sw_pos = 7;
  }
  // Position 8
  if (Input1 == 0 && Input2 == 1 && Input3 == 1 && Input4 == 0 && Input5 == 1)
  {
    sw_pos = 8;
  }
  // Position 9
  if (Input1 == 1 && Input2 == 0 && Input3 == 1 && Input4 == 0 && Input5 == 1)
  {
    sw_pos = 9;
  }
  // Position 10
  if (Input1 == 0 && Input2 == 0 && Input3 == 1 && Input4 == 0 && Input5 == 1)
  {
    sw_pos = 10;
  }
  // Position 11
  if (Input1 == 1 && Input2 == 1 && Input3 == 0 && Input4 == 0 && Input5 == 1)
  {
    sw_pos = 11;
  }
  // Position 12
  if (Input1 == 1 && Input2 == 0 && Input3 == 0 && Input4 == 0 && Input5 == 1)
  {
    sw_pos = 12;
  }
}

// Ermittling des gewählten Kanal
void check_kanal()
{

  if (sw_pos == 1)
  {
    if (sw_pos_temp == 2)
    {
      sw_pos_temp = sw_pos;
      kanal--;
    }
    else
    {
      sw_pos_temp = sw_pos;
      kanal++;
    }
  }
  if (sw_pos == 12)
  {
    if (sw_pos_temp == 1)
    {
      sw_pos_temp = sw_pos;
      kanal--;
    }
    else
    {
      sw_pos_temp = sw_pos;
      kanal++;
    }
  }

  if (sw_pos < sw_pos_temp)
  {

    sw_pos_temp = sw_pos;
    kanal--;
  }
  if (sw_pos > sw_pos_temp)
  {

    sw_pos_temp = sw_pos;
    kanal++;
  }
  if (kanal > 80)
  {
    kanal = 1;
  }
  if (kanal < 1)
  {
    kanal = 80;
  }
  EEPROM.write(0, kanal);
}

// Setzen der Ausgangspins für die PLL
void set_bits_pll()
{
  digitalWrite(stumm_TX, HIGH);
  delay(20);
  byte pllbin_ist = pll_set[kanal];

  bit0 = bitRead(pllbin_ist, 0);
  bit1 = bitRead(pllbin_ist, 1);
  bit2 = bitRead(pllbin_ist, 2);
  bit3 = bitRead(pllbin_ist, 3);
  bit4 = bitRead(pllbin_ist, 4);
  bit5 = bitRead(pllbin_ist, 5);
  bit6 = bitRead(pllbin_ist, 6);
  bit7 = bitRead(pllbin_ist, 7);

  digitalWrite(bit_pll[0], bit0);
  digitalWrite(bit_pll[1], bit1);
  digitalWrite(bit_pll[2], bit2);
  digitalWrite(bit_pll[3], bit3);
  digitalWrite(bit_pll[4], bit4);
  digitalWrite(bit_pll[5], bit5);
  digitalWrite(bit_pll[6], bit6);
  digitalWrite(bit_pll[7], bit7);
  delay(20);
  digitalWrite(stumm_TX, LOW);
}

// Seriale Ausgabe des Aktuellen Status.
void print_status()
{

  freq_ist = freq_Set[kanal];

  Serial.print("Kanal: ");
  Serial.print(kanal);
  Serial.print("\t");

  Serial.print("Frequenz= : ");
  Serial.print(freq_ist, 3);
  Serial.print("\t");

  Serial.print(" bits: ");
  Serial.print(bit7);
  Serial.print(bit6);
  Serial.print(bit5);
  Serial.print(bit4);
  Serial.print(bit3);
  Serial.print(bit2);
  Serial.print(bit1);
  Serial.println(bit0);
}

// Ausgabe an das OLED Display
void oled_ausgabe(void)
{
  oled.setFont(fixednums15x31);
  oled.clear();            // Löschen der aktuellen Displayanzeige
  oled.setCursor(45, 0);   // Setzt den Cursor
  oled.println(kanal);     // Text in der ersten Zeile. "Println" sorgt dabei für einen Zeilensprung.
  oled.setCursor(15, 4);   // Setzt den Cursor
  oled.print(freq_ist, 3); // Text in der zweiten Zeile. Da es keine dritte Zeile gibt, wird hier kein Zeilenumsprung benötigt.
}

void setup()
{
  Serial.begin(115200);
  Wire.setClock(400000L);
  oled.begin(&Adafruit128x64, I2C_ADDRESS);

  pinMode(Pin1, INPUT_PULLUP);
  pinMode(Pin2, INPUT_PULLUP);
  pinMode(Pin3, INPUT_PULLUP);
  pinMode(Pin4, INPUT_PULLUP);
  pinMode(Pin5, INPUT_PULLUP);
  pinMode(Pin6, INPUT_PULLUP);
  pinMode(scan_sw, INPUT_PULLUP);
  pinMode(scan_sens, INPUT);
  pinMode(stumm_TX, OUTPUT); // Dient zur Stummschaltung des Senders während der Kanalumschaltung

  for (int i = 0; i < 8; i++)
  {
    pinMode(bit_pll[i], OUTPUT);
  }

  kanal = EEPROM.read(0);
  set_bits_pll();
  print_status();
  oled_ausgabe();
}

void loop()
{

  Input6 = digitalRead(Pin6); //

  while (Input6 == false)
  {
    digitalWrite(stumm_TX, HIGH);
    Input6 = digitalRead(Pin6);
    pos_true = false;
  }

  if (pos_true == false)
  {
    check_pos();
    check_kanal();
    digitalWrite(stumm_TX, LOW);
    set_bits_pll();
    print_status();
    oled_ausgabe();
    pos_true = true;
  }

  if (digitalRead(scan_sw) == 0)
  {
    // digitalWrite(stumm_TX, HIGH);
    if (analogRead(scan_sens) >= schwellwert)
    {
      delay(time_high);
    }
    else
    {
      delay(time_low);
    }
    kanal++;
    if (kanal > 80)
    {
      kanal = 1;
    }
    set_bits_pll();
    print_status();
    oled_ausgabe();
  }
  else
  {
    digitalWrite(stumm_TX, LOW);
  }
}
