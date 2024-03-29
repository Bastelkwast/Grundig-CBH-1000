# Grundig-CBH-1000

Hier beschreibe ich den Bau eine Ansteuerung für das Funkgerät von Grundig CBH-1000.

Da ich das Funkgerät auf 80 Kanäle umbauen wollte habe ich mir vorgenommen selbst eine Schaltung dafür zu bauen.

Ich wollte den Drehwahlschalter gerne behalten. Deshalb habe ich die Logic dahinter ermittelt und eine Auswertung geschrieben, so das ich jetzt darüber 80 Kanäle damit einstellen kann.

Wenn man den Drehwahlschalter über Kanal 80 dreht, fängt er wieder bei Kanal 1 an und umgekehrt.

Habe dann die Frequenzen und die zugehörige PLL Ansteuerung rausgesucht und auch daraus eine Logic geschrieben das die PLL entsprechend des Kanal und der Frequenz ansteuert.

Für die Anzeige habe ich mich für ein OLED Display entschieden das mir jetzt den eingestellten Kanal und die dazugehörige Frequenz anzeigt.

Einen Scan Modus habe ich auch mit eingebaut so das ich auch die Kanäle abscannen kann.

Als Herzstück dient ein Arduino Nano mit einem Atmega 328P.

Der Arduino ist für die ganze Logic, Ansteuerung der PLL, Stummschaltung des Sender/Empfänger, Ansteuerung des OLED und dessen Stromversorgung zuständig.

Das Display ist ein OLED SSD1306

Die Anschlüsse für den Arduino sind im Code näher beschrieben.

Somit sollte jeder der sich schon mal mit einem Arduino beschäftigt hat in der Lage sein die Schaltung nachzubauen.

Benötigtes Material:

- Arduino Nano mit dem Atmega 328P Prozessor
- OLED Display SSD1306
- Arduino Nano Extension Board
- Kleinmaterial wie Kabel, Schalter usw.

Viel Spaß beim Nachbau.

Ich habe eine Version des Code für die Arduino IDE und eine für VSC (Visual Studio Code) bereitgestellt.
