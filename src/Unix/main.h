// Modellierung eines Beweglichen Autonomen RoboTers
// 
// Aufnahme eines markierten Gegenstandes in unbekannter Umgebung
// und gezielter Ablage
// 
// geschrieben unter Verwendung der Bibliothek Open Inventor
// (C) by NG 1999/2000
//
// erste gueltige Fassung: 01.12.99
// letzte Aenderung: 29.02.00

// Haupt-Header-File

// Aufgaben:
//
// Allgemeine Headerfiles
// Open Inventor Headerfiles laden
//
// Raumgroesse der Szenerie definieren
// Definition des Roboter-Files
// Defintion des Terminal-Root-Files
// Definition des Namens vom Szenerie-Separator
// Definition des Namens vom Roboter-Separator
// Definition des Namens der Main-Root
// Definition des Namens der Term-Root
// Definition des Namens der Main-Sel-Root
// Definition des Namens der Term-Sel-Root
// Definition der Zeit wann eine neue Zufallszahl in
//  Current_Random_Number geschrieben wird
// Definition von Farben
//
// Variable fuer das Window
// Variable zur Speicherung der Haupt-Fenster Kamera-Position
// Variable zur Speicherung der aktuellen Zufallszahl
// Timer fuer den Zufallszahlengenerator
//
// Haupt-Initialisierungsfunktion definieren
// Einleseroutine f∆r IV-Files definieren
// Zufallszahlengenerator starten

// Allgemeine Headerfiles
#include <iostream.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


// Open Inventor Headerfiles

// Header-Files fuer Viewer
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/Xt/viewers/SoXtPlaneViewer.h>

// Header-Files der Eventhaendler
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/nodes/SoEventCallback.h>

// Header-File des Manipulators
#include <Inventor/manips/SoHandleBoxManip.h>

// Header-File des Draggers
#include <Inventor/draggers/SoTranslate2Dragger.h>

// Header-Files des Timers
#include <Inventor/sensors/SoTimerSensor.h>

// Header-Files der Selection
#include <Inventor/nodes/SoSelection.h>

// Header-Files der Transformationen
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoTranslation.h>

// Header-Files der Primitive
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCylinder.h>

// Header-Files von Text
#include <Inventor/nodes/SoText3.h>

// Header-Files von Texturen
#include <Inventor/nodes/SoTexture2.h>

// Header-Files von Farbe
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoBaseColor.h>

// Header-Files fuer Licht und Kamera
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoCamera.h>

// sonstige Header-Files
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoGroup.h>


// Definitionen

// Definition der Raumgroesse der Szenerie
#define X_Raum 1500
#define Y_Raum 1500
#define Z_Raum 2

// Definition des Namen vom Roboter
#define Roboter_File "bart.iv"

// Defintion des Terminal-Root-Files
#define TERMINAL_Root "terminal.iv"

// Definition des Namens vom Szenerie-Separator
#define SzenerieSeparator "Szenen_Separator"

// Definition des Namens vom Roboter-Separator
#define RoboterSeparator "Roboter_Separator"

// Definition des Namens der Main-Root
#define Haupt_Wurzelknoten "Main_Root"

// Definition des Namens der Terminal-Root
#define Terminal_Wurzelknoten "Terminal_Root"

// Definition des Haupt-Selection-Wurzel
#define MainSelRoot "Haupt_Sel_Wurzel"

// Definition des Terminal-Selection-Wurzel
#define TermSelRoot "Term_Sel_Wurzel"

// Definition der Zeit wann eine neue Zufallszahl in
//  Current_Random_Number geschrieben wird
#define RandomNumberCreateTime 1.0

// Definition von Farben
#define ROT 1.0,0.0,0.0
#define GRUEN 0.0,1.0,0.0
#define BLAU 0.0,0.0,1.0
#define WEISS 1.0,1.0,1.0
#define SCHWARZ 0.0,0.0,0.0
#define GRAU 0.8,0.8,0.8


// Variablen

// Variable fuer die Windows
extern Widget Main_Window;

// Variable zur Speicherung der Haupt-Fenster-Kamera
extern SoCamera *Main_Camera;

// Variable zur Speicherung der aktuellen Zufallszahl
extern double Current_Random_Number;

// Timer fuer den Zufallszahlengenerator
extern SoTimerSensor *RandomNumber;


// Funktionen

// Haupt-Initialisierungsroutine
extern void main_init(void);

// Deklaration einer Routine zum Einlesen eines IV-Files
extern SoSeparator* Read_File(const char*);

// Zufallszahlengenerator mit Hilfe einer Interruptfunktion
// erzeugt Zufallszahlen zwischen 0 und 1
extern void RandomNumber_Generator(void*,SoSensor*);
