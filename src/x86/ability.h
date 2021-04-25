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

// Ability-Header-File

// Aufgaben:
//
// Definition der Namen fuer Objekt-Kennzeichnung
// Definition der Robotergeschwindigkeit
// Definition der Namen von Auswegsuchalgorithmen
// Definition der Namen von Zielsuchalgorithmen
// Definition der Namen von Orientierungen
// Konstruktionsmerkmale des Roboters definieren
// Definition der Startwinkelwerte der Robotergelenke
// Definition der Grenzen des Roboterarms
// Definition der Grenzen der Roboter-Gelenke 2,3 und 4
// Bewegungsradius vom Roboter definieren
// Definition des Faktors fuer die Erkennung ob Ziel erreicht
//
// Variable fuer die Objekt-Datenbank
// Timer-Variable fuer die Auswegsuche
// Timer-Variable fuer die Zielsuche
// Signalisierung der automatischen Bewegung
// Zielpunkt-Koordinaten
// Variable fuer die alten Roboterkoordinaten
// Variablen fuer den Objekt-Kontakt
// Variable fuer die Koordinaten eines Eckpunktes
// Variablen zur Ermittlung der Totalen-Rotation
// Variablen fuer die Ermittlung der naechsten Position zum Ziel
// Variable zur Umrundungslaufrichtung eines Objektes
//
// Funktion zur Erkennung einer Kollision
// Funktion zur Rotation von Punkt-Koordinaten um einen Mittelpunkt
// Funktionen zur Initilisierung verschiedener automatischen 
//  Auswegsuchen aus einem eingelesenen Labyrinth
// Loesungsalgorithmus zur automatischen Auswegsuche
// Funktion zur Bestimmung eines Wendepunktes
// Funktion zur Bestimmung der Drehrichtung um SOLL- in den IST-Winkel
//  zu ueberfuehren
// Funktion zur Bestimmung des minimalen Abstandes zwischen zwei Winkeln
// Funktion zur Berechnung von Rest = Wert mod Modul
// Funktion zur Initialisierung von automatischen Zielsuchen
// Interrupt-Funktion zur Realisierung der Bug-Methode zur Zielsuche
//  in einem Labyrinth
// Interrupt-Funktion zur Realisierung der Corner-Methode zur Zielsuche
//  in einem Labyrinth
// Funktion zur Initialisierung der homogenen Point-Matrix
// Funktion zum berechnen des Tool-Point-IKP


// Definitionen

// Namen fuer Objekt-Kennzeichnung
#define NO_OBJEKT -1
#define NO_OBJEKT_AREA -1

// Geschwindigkeit der Automatischen Bewegung des Roboters
#define Time_Move_Robot 0.03

// Namen der automatischen Auswegsuchmethoden aus einem Labyrinth
#define SIMPLE_METHODE 1
#define MODULO_METHODE 2
#define PLEDGE_METHODE 3

// Namen der automatischen Zielsuchmethoden in einem Labyrinth
#define BUG_METHODE 4
#define CORNER_METHODE 5

// Orientierung
#define RIGHT -1
#define LEFT +1

// Definition von Konstruktionsdaten des Roboters
#define L2 42
#define L22 pow(L2,2)
#define L3 44
#define L32 pow(L3,2)
#define d1 32
#define d2  6
#define d3 20
#define Line_b 24
#define Tau 24
#define Tau2 pow(Tau,2)

// Definition der Startwinkelwerte der Robotergelenke
#define Theta1_Start 0.0
#define Theta2_Start 0.0
#define Theta3_Start 0.0
#define Theta4_Start M_PI/2
#define Theta5_Start 0.0

// Definition der Grenzen des Roboterarms
#define Min_Line_a 4.1
#define Max_Line_a 42

// Definition der Grenzen der Roboter-Gelenke 2 und 4
#define Min_Theta21 0.0
#define Max_Theta21 M_PI/2
#define Min_Theta22 5.8
#define Max_Theta22 2*M_PI
#define Min_Theta41 0.0
#define Max_Theta41 2.75
#define Min_Theta42 3.53
#define Max_Theta42 2*M_PI

// Definition des Bewegungsradius vom Roboter
#define Robot_Movement_Radius 75

// Definition des Faktors fuer die Erkennung ob Ziel erreicht
#define Target_Faktor 1.1


// Globale Variablen

// Szenen-Datenbank definieren
extern Objects Szene_Objects;

// Timer fuer verschiedene Loesungsalgorithmus zur 
// automatischen Auswegsuche aus einem eingelesenen Labyrinth
extern SoTimerSensor *Search_Way_Out;

// Timer zur Realisierung der Bug-Methode zur Zielsuche
//  in einem Labyrinth
extern SoTimerSensor *BugSearch_Target;

// Timer zur Realisierung der Corner-Methode zur Zielsuche
//  in einem Labyrinth
extern SoTimerSensor *CornerSearch_Target;

// Signalisierung der automatische Bewegung des Roboters
// TRUE automatische Bewegung
// FALSE keine automatische Bewegung
extern bool Robot_Auto_Move;

// Automatischer Suchmodus
// SIMPLE_METHODE
// MODULO_METHODE
// PLEDGE_METHODE
// BUG_METHODE
// CORNER_METHODE
extern int Auto_Search_Modus;

// Roboter-Zielpunkt
extern Point_3D Robot_Target;

// alte Roboter-Position
extern Point_3D Old_Robot_Koord;

// neue Roboter-Richtung
extern double Robot_New_Direction;

// Totale Richtungsaenderung
extern double Total_Rotation;

// neue Totale Richtungsaenderung
extern double New_Total_Rotation;

// Anzahl der Schritte vom IST zum SOLL Winkel
extern int Steps_of_Rotations;

// Drehrichtung des Roboters um neue Bewegungrichtung zu erreichen
extern int Robot_Rotation_Direction;

// Roboter hat ersten Kontakt mit Objekt
extern bool First_Kontakt;

// Roboter an der Objekt-Huelle
// FALSE Roboter nicht an einer Objekt-Huelle entlang
// TRUE Roboter an einer Objekt-Huelle entlang
extern bool Object_Hull_Kontakt;

// Roboter an der Objekt-Huelle i
extern int Object_Hull_Number;

// Gebiet in de der naechste Wendepunkt liegt
extern int Turn_Area;

// Roboter Wendepunkte um an der Objekt-Huelle zu bleiben
extern Point_3D Turn_Point;

// Koordinaten eines Eckpunktes
extern Point_3D Corner;

// Position des Roboter wo er ein Objekt beruehrt
extern Point_3D Kontakt_Robot_Position;

// Roboterposition des bisherigen kleinsten Abstand: Roboter-Ziel 
extern Point_3D Closest_Target_Position;

// Punkt in der Ebene der am naechsten zum Ziel liegt
// FALSE gehe nicht zu diesem Punkt
// TRUE gehe zu diesem Punkt
extern bool Go_To_Closest_Point;

// Weglaenge zwischen Beruehrungspunkt und aktueller Roboterposition
extern double Kontakt_Current_Distance;

// Weglaenge zwischen Beruehrungspunkt und dem Ziel naheliegensten Punkt
extern double Kontakt_Closest_Distance;

// Laufrichtung zur Umrundung eines Objektes
// LEFT links herum (rechte "Hand" an Objekt)
// RIGHT rechts herum (linke "Hand" an Objekt)
extern int Umrundung;

// muss der Roboter zuerst aus der Ecke herausgehen ?
// TRUE  Ja er muss
// FALSE Nein er muss nicht
extern bool Robot_Go_Out_Of_Corner;


// Funktionen

// Funktion zur Erkennung ob der Roboter ein Objekt beruehrt hat
// Eingabe: aktuelle Koordinaten des Roboters
//          Eingabe: Nummer eines Objektes i (1...)
//			Ausgabe: Bereich in dem sich der Roboter aktuell von diesem
//                   Objekt i befindet
//					 !!! falls ein anderes Objekt j beruehrt wird ist die 
//					 !!! Ausgabe wie bei Eingabe der Objekt-Nummer 0
//			Eingabe: Objekt-Nummer 0 bedeutet falls ein Objekt beruehrt wird			
//			Ausgabe: Nummer des Objektes welches beruehrt wurde sonst NO_OBJEKT
//					 Bereich wo Objekt beruehrt wurde sonst NO_OBJEKT_AREA
// Return: TRUE Objekt beruehrt
//		   FALSE kein Objekt beruehrt
extern bool Objekt_Kontakt(Point_3D,int&,int&);

// Drehung eines Punktes um einen Mittelpunkt um den Winkel Phi
// Eingabe: Drehwinkel Phi
//			Mittelpunkt-Koordinaten
// Eingabe/Ausgabe: Punkt-Koordinaten 
// Rueckgabe: Richtung Chi in dem der Punkt vom Mittelpunkt liegt (0...2*PI)
extern double  Rotation_Koord(double,Point_3D,Point_3D&);

// Funktion zur Initialisierung einer automatischen
// Auswegsuche aus einem vorher eingelesenen Labyrinth
// Eingabe: Loesungsalgorithmus
//			SIMPLE_METHODE
//			MODULO_METHODE
//			PLEDGE_METHODE
extern void Init_Labyrinth_Search(int);

// Interrupt-Funktion zur Realisierung verschiedener automatischer
// Methoden zur Auswegsuche aus einem Labyrinth
extern void Robot_Search_Way_Out(void*,SoSensor*);

// Betrachtung der Total_Rotation
extern void Calculate_Total_Rotation(void); 

// Ermittlung des Wendepunktes um an der Objekt-Huelle zu bleiben
// Eingaben: Aktuelle Position des Roboters
//			 Objekt-Nummer des zu umfahrenden Objektes
//			 aktueller Objekt-Bereich in dem sich der Roboter befindet
//			 erster Kontakt mit dem Roboter (TRUE/FALSE)
//			 Richtung der Umfahrung des Objektes (LEFT/RIGHT)
// Ausgaben: neue Bewegungsrichtung des Roboters
//			 naechster Wendebereich
//			 naechster Wendepunkt des Roboters
//			 Eckpunkt-Koordinaten an dem der Roboter wendet
extern void Calculate_Turn_Point(Point_3D,int,int,bool,int,double&,int&,Point_3D&,Point_3D&);

// Ermittlung der Rotationsrichtung um den IST-Winkel in einen
// SOLL-Winkel zu ueberfuehren und die Anzahl der benoetigten
// Schritte um dies zu tun
// Eingabe: Ist-Winkel
//			Soll-Winkel
//			Schrittweite
// Ausgabe: Anzahl der Schritte
// Rueckgabe: Rotationsrichtung
//			  RIGHT in Uhrzeigersinn
//			  LEFT gegen Uhrzeigersinn
extern double Calculate_Rotation_Direction(double,double,double,int&); 

// Ermittlung des Abstandes zwischen einem gegebenem Winkel und einem
// anderen gegebenem Winkel
// Eingabe: Ist-Winkel
//			Soll-Winkel
// Ausgabe: minimaler Abstand zwischen den Winkeln
extern double Calculate_Rotation_Distance (double,double);

// Berechnung: Rest = Wert mod Modul
// Eingabe: Wert
//			Modul
// Ausgabe: Ganzer Anteil
// Rueckgabe: Rest
extern double Calculate_Modulo(double,double);

// Funktion zur Initialisierung einer automatischen
// Zielsuche in einem vorher eingelesenem Labyrinth
// Eingabe: Suchalgorithmus
//          BUG_METHODE
//			CORNER_METHODE
extern void Init_Target_Search(int);

// Interrupt-Funktion zur Realisierung der Bug-Methode zur Zielsuche
//  in einem Labyrinth
extern void Robot_BugSearch_Target(void*,SoSensor*);

// Euklidschen Abstand zwischen zwei Punkten (2D) ermitteln
// Eingabe: Punkt A
//			Punkt B
// Rueckgabe: Euklidscher Abstand
extern double Calculate_Point_Distance(Point_3D,Point_3D);

// Interrupt-Funktion zur Realisierung der Corner-Methode zur Zielsuche
//  in einem Labyrinth
extern void Robot_CornerSearch_Target(void*,SoSensor*);

// Funktion zur Berechnung der Tool-Koordinaten im Weltkoordinatensystem
// Eingabe: im letzten Koordinatensystem um den Wert auf der Z-Achse
//			 verschoben
extern SbVec3f Calculate_Tool_Koord(double);

// Funktion zur Initialisierung der homogenen Point-Matrix
// Eingabe:
//		Punkt der durch Roboterarme erreicht werden soll
// Ausgabegabe:
//		homogene Punkt-Matrix 
extern void Set_Point_Matrix(Point_3D,double*);

// Funktion zum berechnen des Tool-Point-IKP
//  berechnete Winkelwerte werden falls loesbar in dem globalen Feld New_Theta gespeichert
// Eingabe
//		Punkt der durch Roboterarme erreicht werden soll
// Rueckgabe:
//		TRUE IKP loesbar
//		FALSE IKP nicht loesbar
extern bool Calculate_Tool_Point_IKP(double *Point_Matrix);

