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

// Faehigkeiten des Roboters spezifizieren

// Aufgaben:
//
// Variable fuer die Objekt-Datenbank
// Timer-Variable fuer die Auswegsuche
// Timer-Variable fuer die Zielsuche
// Signalisierung der automatischen Bewegung
// Zielpunkt-Variable
// Variablen fuer den Objekt-Kontakt
// Variable fuer die Koordinaten eines Eckpunktes
// Variablen zur Ermittlung der Totalen-Rotation
// Variablen fuer die Ermittlung der naechsten Position zum Ziel
// Grenzen von Gelenk 2,3 und 4 zuweisen
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
// Funktion zur Berechnung des Euklidschen Abstand zwischen zwei Punkten (2D)
// Funktion zur Initialisierung einer automatischen Zielsuche
// Funktion zur Realisierung der Bug-Methoden zur Zielsuche
//  in einem Labyrinth
// Funktion zur Realisierung der Corner-Methoden zur Zielsuche
//  in einem Labyrinth


// Header-Files

#include "main.h"
#include "objects.h"
#include "control.h"
#include "ability.h"


// Globale Variablen

// Szenen-Datenbank definieren
Objects Szene_Objects;

// Timer fuer verschiedene Loesungsalgorithmus zur 
// automatischen Auswegsuche aus einem eingelesenen Labyrinth
SoTimerSensor *Search_Way_Out;

// Timer zur Realisierung der Bug-Methode zur Zielsuche
//  in einem Labyrinth
SoTimerSensor *BugSearch_Target;

// Timer zur Realisierung der Corner-Methode zur Zielsuche
//  in einem Labyrinth
SoTimerSensor *CornerSearch_Target;

// Signalisierung der automatische Bewegung des Roboters
// TRUE automatische Bewegung
// FALSE keine automatische Bewegung
bool Robot_Auto_Move;

// Automatischer Suchmodus
// SIMPLE_METHODE
// MODULO_METHODE
// PLEDGE_METHODE
// BUG_METHODE
// CORNER_METHODE
int Auto_Search_Modus;

// Roboter-Zielpunkt
Point_3D Robot_Target;

// alte Roboter-Koordinaten
Point_3D Old_Robot_Koord;

// neue Roboter-Richtung
double Robot_New_Direction;

// Totale Richtungsaenderung
double Total_Rotation;

// neue Totale Richtungsaenderung
double New_Total_Rotation;

// Anzahl der Schritte vom IST zum SOLL Winkel
int Steps_of_Rotations;

// Drehrichtung des Roboters um neue Bewegungrichtung zu erreichen
int Robot_Rotation_Direction;

// Roboter hat ersten Kontakt mit Objekt
bool First_Kontakt;

// Roboter an der Objekt-Huelle
// FALSE Roboter nicht an einer Objekt-Huelle entlang
// TRUE Roboter an einer Objekt-Huelle entlang
bool Object_Hull_Kontakt;

// Roboter an der Objekt-Huelle i
int Object_Hull_Number;

// Gebiet in de der naechste Wendepunkt liegt
int Turn_Area;

// Roboter Wendepunkte um an der Objekt-Huelle zu bleiben
Point_3D Turn_Point;

// Koordinaten eines Eckpunktes
Point_3D Corner;

// Position des Roboter wo er ein Objekt beruehrt
Point_3D Kontakt_Robot_Position;

// Roboterposition des bisherigen kleinsten Abstand: Roboter-Ziel 
Point_3D Closest_Target_Position;

// Punkt in der Ebene der am naechsten zum Ziel liegt
// FALSE gehe nicht zu diesem Punkt
// TRUE gehe zu diesem Punkt
bool Go_To_Closest_Point;

// Weglaenge zwischen Beruehrungspunkt und aktueller Roboterposition
double Kontakt_Current_Distance;

// Weglaenge zwischen Beruehrungspunkt und dem Ziel naheliegensten Punkt
double Kontakt_Closest_Distance;

// Laufrichtung zur Umrundung eines Objektes
// LEFT links herum (rechte "Hand" an Objekt)
// RIGHT rechts herum (linke "Hand" an Objekt)
int Umrundung;

// muss der Roboter zuerst aus der Ecke herausgehen
// TRUE  Ja er muss
// FALSE Nein er muss nicht
bool Robot_Go_Out_Of_Corner;


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
bool Objekt_Kontakt(Point_3D Koord_Robot,int& Object_Number,int& Object_Area) {	
	bool Test=FALSE;
	int Objekt_Typ,Nummer,Bereich;
	Point_3D Cube_Min,Cube_Max,Objekt_MiddlePoint,Koord_Robot_Org;
	double Objekt_Rot,Objekt_Radius,Objekt_BottomRadius;
	double Abstand;
	// getestete Objekt-Nummer auf 0 setzen
	Nummer=0;
	// Original-Roboterkoordinaten setzen
	Koord_Robot_Org.X=Koord_Robot.X;
	Koord_Robot_Org.Y=Koord_Robot.Y;
	// Zeiger auf erstes Element in der Datenbank setzen
	Szene_Objects.Reset_Current_Object();
	while ((Szene_Objects.Read_Current_Object_Typ(Objekt_Typ)!=FALSE)&&(Test!=TRUE)) {
		// Original Roboter-Koordinaten wiederherstellen
		Koord_Robot.X=Koord_Robot_Org.X;
		Koord_Robot.Y=Koord_Robot_Org.Y;
		// getestete Objekt-Nummer erhoehen
		Nummer++;
		if (Objekt_Typ==CUBE) {
			// Kollisions-Test mit einem Cube-Objekt
			// Ermittlung der Wuerfel-Daten
			Szene_Objects.Read_Current_Cube(Cube_Min,Cube_Max,Objekt_MiddlePoint,Objekt_Rot);
			// Anpassen der Roboterkoordinaten der Cube-Rotation
			if (Objekt_Rot!=0) Rotation_Koord(Objekt_Rot,Objekt_MiddlePoint,Koord_Robot);
			// Feststellung in welcher Cube-Umgebung sich
			// der Roboter befindet
			Bereich=0;
			if (Koord_Robot.X<Cube_Min.X) Bereich=Bereich|1;
			if (Koord_Robot.X>Cube_Max.X) Bereich=Bereich|2;
			if (Koord_Robot.Y<Cube_Min.Y) Bereich=Bereich|4;
			if (Koord_Robot.Y>Cube_Max.Y) Bereich=Bereich|8;
			// Test ob im Bereich eine Kollision vorliegt
			// Falls Kollision Stellung der Wand merken und
			// Bereich an der die Wand beruehrt wird
			switch (Bereich) {
				case 0: Test=TRUE;
						break;
				case 1: if (Koord_Robot.X>Cube_Min.X-Robot_Movement_Radius) Test=TRUE;
						break;
				case 2: if (Koord_Robot.X<Cube_Max.X+Robot_Movement_Radius) Test=TRUE;
						break;
				case 4: if (Koord_Robot.Y>Cube_Min.Y-Robot_Movement_Radius) Test=TRUE;
						break;
				case 8: if (Koord_Robot.Y<Cube_Max.Y+Robot_Movement_Radius) Test=TRUE;
						break;
				case 5: // Euklidscher Abstand von Roboter-Mittelpunkt und Cube-Eckpunkt P1
						Abstand=Calculate_Point_Distance(Koord_Robot,Cube_Min);
						if (Abstand<Robot_Movement_Radius) Test=TRUE;
						break;
				case 6: // Euklidscher Abstand von Roboter-Mittelpunkt und Cube-Eckpunkt P4
						Point_3D Point_P4;
						Point_P4.X=Cube_Max.X;
						Point_P4.Y=Cube_Min.Y;
						Abstand=Calculate_Point_Distance(Koord_Robot,Point_P4);
						if (Abstand<Robot_Movement_Radius) Test=TRUE;
						break;
				case 9: // Euklidscher Abstand von Roboter-Mittelpunkt und Cube-Eckpunkt P2
						Point_3D Point_P2;
						Point_P2.X=Cube_Min.X;
						Point_P2.Y=Cube_Max.Y;
						Abstand=Calculate_Point_Distance(Koord_Robot,Point_P2);
						if (Abstand<Robot_Movement_Radius) Test=TRUE;
						break;
				case 10: // Euklidscher Abstand von Roboter-Mittelpunkt und Cube-Eckpunkt P3
						 Abstand=Calculate_Point_Distance(Koord_Robot,Cube_Max);
						 if (Abstand<Robot_Movement_Radius) Test=TRUE;
						 break;
			}
		}
		if (Objekt_Typ==CYLINDER) {
			// Kollisions-Test mit einem Cylinder-Objekt
			// Ermittlung der Zylinder-Daten
			Szene_Objects.Read_Current_Cylinder(Objekt_MiddlePoint,Objekt_Radius);
			// Feststellung in welcher Cylinder-Umgebung sich
			// der Roboter befindet
			Bereich=0;
			if (Koord_Robot.X<Objekt_MiddlePoint.X) Bereich=Bereich|1;
			if (Koord_Robot.X>Objekt_MiddlePoint.X) Bereich=Bereich|2;
			if (Koord_Robot.Y<Objekt_MiddlePoint.Y) Bereich=Bereich|4;
			if (Koord_Robot.Y>Objekt_MiddlePoint.Y) Bereich=Bereich|8;
			// Euklidschen Abstand vom Roboter- zum Zylindermittelpunkt berechnen
			Abstand=Calculate_Point_Distance(Koord_Robot,Objekt_MiddlePoint);
			// Test ob Kollision mit Zylinder vorliegt
			if (Abstand<(Robot_Movement_Radius+Objekt_Radius)) Test=TRUE;
		}
		if (Objekt_Typ==CONE) {
			// Kollisions-Test mit einem Cone-Objekt
			// Ermittlung der Kegel-Daten
			Szene_Objects.Read_Current_Cone(Objekt_MiddlePoint,Objekt_BottomRadius);
			// Feststellung in welcher Cone-Umgebung sich
			// der Roboter befindet
			Bereich=0;
			if (Koord_Robot.X<Objekt_MiddlePoint.X) Bereich=Bereich|1;
			if (Koord_Robot.X>Objekt_MiddlePoint.X) Bereich=Bereich|2;
			if (Koord_Robot.Y<Objekt_MiddlePoint.Y) Bereich=Bereich|4;
			if (Koord_Robot.Y>Objekt_MiddlePoint.Y) Bereich=Bereich|8;
			// Euklidschen Abstand vom Roboter- zum Kegelfussmittelpunkt berechnen
			Abstand=Calculate_Point_Distance(Koord_Robot,Objekt_MiddlePoint);
			// Test ob Kollision mit Kegel vorliegt
			if (Abstand<(Robot_Movement_Radius+Objekt_BottomRadius)) Test=TRUE;
		}
		// Bereich des interessierenden Objektes speichern falls
		// nicht noch eine Kollision auftritt
		if (Nummer==Object_Number) Object_Area=Bereich;
	}
	if (Test==FALSE) { 
		if (Object_Number==0) {
			Object_Number=NO_OBJEKT;
			Object_Area=NO_OBJEKT_AREA;
		}
	}
	else {
		Object_Number=Nummer;
		Object_Area=Bereich;
		Print_News_Screen(NOTEXT,OLDTEXT,"OBJECT CONTACT");
		//cout << "Kontakt mit:" << endl;
		//cout << "Object_Number=" << Object_Number << endl;
		//cout << "Object_Area=" << Object_Area << endl;
		//cout << endl;
	}
	return Test;
}

// Drehung eines Punktes um einen Mittelpunkt um den Winkel Phi
// Eingabe: Drehwinkel Phi
//			Mittelpunkt-Koordinaten
// Eingabe/Ausgabe: Punkt-Koordinaten 
// Rueckgabe: Richtung Chi in dem der Punkt vom Mittelpunkt liegt (0...2*PI)
double Rotation_Koord(double Drehwinkel,Point_3D Mittelpunkt, Point_3D& Punkt) { 
	double Zaehler,Nenner,Chi,Abstand;
	// Euklidschen Abstand vom Roboter- zum Cube-Mittelpunkt berechnen
	Abstand=Calculate_Point_Distance(Punkt,Mittelpunkt);
	Zaehler=Punkt.Y-Mittelpunkt.Y;
	Nenner=Punkt.X-Mittelpunkt.X;
	// Realisierung der Atan2(Zaehler,Nenner)-Funktion
	if ((Zaehler>0)&&(Nenner>0)) Chi=atan(Zaehler/Nenner);
	if ((Zaehler<0)&&(Nenner>0)) Chi=2*M_PI+atan(Zaehler/Nenner);
	if ((Zaehler>0)&&(Nenner==0)) Chi=M_PI/2;
	if ((Zaehler==0)&&(Nenner==0)) Chi=0.0;
	if ((Zaehler<0)&&(Nenner==0)) Chi=1.5*M_PI;
	if (Nenner<0) Chi=atan(Zaehler/Nenner)+M_PI;
	Chi=Chi-Drehwinkel;
	Punkt.X=Mittelpunkt.X+Abstand*cos(Chi);
	Punkt.Y=Mittelpunkt.Y+Abstand*sin(Chi);
	return Chi;
}

// Funktion zur Initialisierung einer automatischen
// Auswegsuche aus einem vorher eingelesenen Labyrinth
// Eingabe: Loesungsalgorithmus
//			SIMPLE_METHODE
//			MODULO_METHODE
//			PLEDGE_METHODE
void Init_Labyrinth_Search(int Proceed) {	
	Robot_Auto_Move=TRUE;
	// Richtung belassen
	Robot_New_Direction=Robot_Direction;
	// Totale Drehung auf 0.0 setzen
	Total_Rotation=0.0;
	// Richtung der Umrundung eines Objektes festlegen
	Umrundung=RIGHT; 
	if (Proceed==SIMPLE_METHODE) {
		// Simpler Loesungsalgorithmus
		//cout << "Simple Suchmethode laeuft !!!" << endl;
		Auto_Search_Modus=SIMPLE_METHODE;
	}
	if (Proceed==MODULO_METHODE) {
		// 2.ter Loesungsalgorithmus
		//cout << "2.te Suchmethode laeuft !!!" << endl;
		Auto_Search_Modus=MODULO_METHODE;
	}
	if (Proceed==PLEDGE_METHODE) {
		// Pledge Loesungsalgorithmus
		//cout << "Pledge Suchmethode laeuft !!!" << endl;
		Auto_Search_Modus=PLEDGE_METHODE;
	}
	// kein Huellenkontakt
	Object_Hull_Kontakt=FALSE;
	// Timer-Intervall setzen
	Search_Way_Out->setInterval(Time_Move_Robot);
	// Timer starten
	Search_Way_Out->schedule();
	// Robotergelenke in die Ausgangsposition bringen
	Reset_Robot_Joints();
}

// Interrupt-Funktion zur Realisierung verschiedener automatischen
// Methoden zur Auswegsuche aus einem Labyrinth
void Robot_Search_Way_Out(void*,SoSensor*) {
	double Abstand;
	int Robot_Aktion,Objekt_Bereich,Objekt_Nummer;
	bool Kontakt;
	// Wendung des Roboters um an Objekt-Huelle zu bleiben
	// FALSE noch nicht durchgefuehrt
	// TRUE durchgefuehrt
	bool Robot_Turn;
	// Abstand des Roboters vom Ziel ermitteln
	Abstand=Calculate_Point_Distance(Robot_Koord,Robot_Target);
	if ((Robot_Foot_Move!=TRUE)&&(Abstand>=Robot_Movement_Radius+Target_Faktor*Ziel_Radius)) {
		// Roboter weiter zum Ausweg fuehren
		// Variablen initialisieren
		Kontakt=FALSE;
		Robot_Aktion=0;
		Objekt_Bereich=NO_OBJEKT_AREA;
		Objekt_Nummer=0;
		

		//cout << "Robot_Direction=" << Robot_Direction << endl;
		//cout << "Robot_New_Direction=" << Robot_New_Direction << endl << endl;
		

		// in aktuelle Laufrichtung drehen ?
		if (Robot_New_Direction==Robot_Direction) {
			// Laufrichtung erreicht 
			// Laufe nun an der Objekt-Huelle entlang oder entlang der
			// eingeschlagenen Lauf-Richtung
			if (Object_Hull_Kontakt==FALSE) {
				// Laufe in eingeschlagener Laufrichtung
				Robot_Aktion=TRANSLATION_FORWARD;
				Kontakt=Calculate_Motion_Robot(Robot_Aktion,Objekt_Nummer,Objekt_Bereich);
				// Roboter beim fahren in Startrichtung auf ein Objekt gestossen 
				if (Kontakt==TRUE) {
					// Roboter an der Objekt-Huelle entlangschicken
					Object_Hull_Kontakt=TRUE;
					Object_Hull_Number=Objekt_Nummer;
					// Roboter hat ersten Kontakt mit dem Objekt
					First_Kontakt=TRUE;
					// neue Richtung und anstehenden Wendepunkt ermitteln
					Calculate_Turn_Point(Robot_Koord,Object_Hull_Number,Objekt_Bereich,First_Kontakt,Umrundung,Robot_New_Direction,Turn_Area,Turn_Point,Corner);
					// Rotationsrichtung ermitteln um neue Bewegungsrichtung zu erreichen
					// und die dazu benoetigten Schritte
					Robot_Rotation_Direction=Calculate_Rotation_Direction(Robot_Direction,Robot_New_Direction,Step_Robot_Rot,Steps_of_Rotations);			
					if (Auto_Search_Modus!=SIMPLE_METHODE) Calculate_Total_Rotation();
				}
			}
			else {
				Robot_Turn=FALSE;
				// Aktuellen Objektbereich ermitteln
				Objekt_Nummer=Object_Hull_Number;
				Kontakt=Objekt_Kontakt(Robot_Koord,Objekt_Nummer,Objekt_Bereich);
				// Koordinaten des Roboters um den Wert 
				// der Objektrotation rotieren
				// Original Roboterkoordinaten kopieren
				Point_3D Robot_Koord_Test=Robot_Koord;
				// Ermittlung ob aktuelles Objekt gedreht ist
				double Phi=Szene_Objects.Read_Object_Rotation(Objekt_Nummer);
				if (Phi!=0) {
					// Mittelpunkt des zu umlaufenden Objektes auslesen
					Point_3D Mittelpunkt=Szene_Objects.Read_Object_MiddelPoint(Objekt_Nummer);
					// Koordinaten anpassen
					Rotation_Koord(Phi,Mittelpunkt,Robot_Koord_Test);
				}
				// Wendepunkt schon erreicht ?
				if ((Objekt_Bereich==Turn_Area)&&(Turn_Area==6)&&(Robot_Koord_Test.X>=Turn_Point.X)) Robot_Turn=TRUE;
				if ((Objekt_Bereich==Turn_Area)&&(Turn_Area==10)&&(Robot_Koord_Test.Y>=Turn_Point.Y)) Robot_Turn=TRUE;
				if ((Objekt_Bereich==Turn_Area)&&(Turn_Area==9)&&(Robot_Koord_Test.X<=Turn_Point.X)) Robot_Turn=TRUE;
				if ((Objekt_Bereich==Turn_Area)&&(Turn_Area==5)&&(Robot_Koord_Test.Y<=Turn_Point.Y)) Robot_Turn=TRUE;
				if (Robot_Turn==TRUE) {
					// Test ob Ecke mit einem anderem Objekt gleich ist
					int New_Object_Hull_Number=Szene_Objects.Read_Same_Corner(Object_Hull_Number,Corner);
					if (New_Object_Hull_Number!=0) {
						Object_Hull_Number=New_Object_Hull_Number;
						// Uebergang zu einem neuen Objekt
						Objekt_Nummer=Object_Hull_Number;
						// aktuellen Objekt-Bereich des neuen Objektes ermitteln
						Objekt_Kontakt(Robot_Koord,Objekt_Nummer,Objekt_Bereich);
					}
					// neue Richtung und anstehenden Wendepunkt ermitteln
					Calculate_Turn_Point(Robot_Koord,Objekt_Nummer,Objekt_Bereich,FALSE,Umrundung,Robot_New_Direction,Turn_Area,Turn_Point,Corner);
					// Rotationsrichtung ermitteln um neue Bewegungsrichtung zu erreichen
					// und die dazu benoetigten Schritte
					Robot_Rotation_Direction=Calculate_Rotation_Direction(Robot_Direction,Robot_New_Direction,Step_Robot_Rot,Steps_of_Rotations);			
					if (Auto_Search_Modus!=SIMPLE_METHODE) Calculate_Total_Rotation();
				}
				else {
					// Laufe geradeaus an Objekt-Huelle entlang 
					Robot_Aktion=TRANSLATION_FORWARD;
					// erster Kontakt mit dem Objekt ist vorbei
					First_Kontakt=FALSE;
					Objekt_Nummer=Object_Hull_Number;
					// Test ob ein neues Objekt beruehrt wird
					Kontakt=Calculate_Motion_Robot(Robot_Aktion,Objekt_Nummer,Objekt_Bereich);
					// wenn nicht ist Objekt_Bereich aktueller Bereich in dem
					// sich der Roboter von dem zu umfahrenden Objektes befindet
					if (Kontakt==TRUE) {
						// Roboter beim fahren entlang der Objekt-Huelle auf ein Objekt gestossen 
						// Roboter an neuer Objekt-Huelle entlangschicken
						Object_Hull_Kontakt=TRUE;
						Object_Hull_Number=Objekt_Nummer;
						// Roboter hat ersten Kontakt mit dem Objekt
						First_Kontakt=TRUE;
						// neue Richtung und anstehenden Wendepunkt ermitteln
						Calculate_Turn_Point(Robot_Koord,Objekt_Nummer,Objekt_Bereich,First_Kontakt,Umrundung,Robot_New_Direction,Turn_Area,Turn_Point,Corner);
						// Rotationsrichtung ermitteln um neue Bewegungsrichtung zu erreichen
						// und die dazu benoetigten Schritte
						Robot_Rotation_Direction=Calculate_Rotation_Direction(Robot_Direction,Robot_New_Direction,Step_Robot_Rot,Steps_of_Rotations);			
						if (Auto_Search_Modus!=SIMPLE_METHODE) Calculate_Total_Rotation();
					}
				}
			}
		}
		else {
			// Roboter muss nun noch gedreht werden !!!
			Robot_Aktion=ROTATION;
			if (Steps_of_Rotations!=0) {
				// Schritte bis zur neuen Richtung um eins vermindern
				Steps_of_Rotations--;
				// Roboter in neue Richtung drehen
				if (Robot_Rotation_Direction==RIGHT) {
					// in Uhrzeigersinn drehen
					Robot_Direction=Robot_Direction-Step_Robot_Rot;
				}
				else {		
					// gegen Uhrzeigersinn drehen
					Robot_Direction=Robot_Direction+Step_Robot_Rot;
				}					
			}
			// neue Laufrichtung erreicht
			else {
				// Roboter-Richtung auf exakte Lauf-Richtung setzen
				Robot_Direction=Robot_New_Direction;
				// Roboter-Fuss wieder einfahren
				Kontakt=Calculate_Motion_Robot(TRANSLATION_FORWARD,Objekt_Nummer,Objekt_Bereich);
				// Modulo Methode
				if (Auto_Search_Modus==MODULO_METHODE) {
					// Totale-Rotation auf exakten Wert setzen
					Total_Rotation=New_Total_Rotation;
					double Rest=Calculate_Modulo(Total_Rotation,2*M_PI);
					if (Rest==0.0) {
						Print_News_Screen(NOTEXT,OLDTEXT,NOTEXT);
						// von Objekt loesen
						Object_Hull_Kontakt=FALSE;
					}
				}
				// Pledge Methode
				if (Auto_Search_Modus==PLEDGE_METHODE) {
					// Totale-Rotation auf exakten Wert setzen
					Total_Rotation=New_Total_Rotation;
					if (Total_Rotation==0.0) {
						Print_News_Screen(NOTEXT,OLDTEXT,NOTEXT);
						// von Objekt loesen
						Object_Hull_Kontakt=FALSE;
					}
				}
			}
			// Robot_Direction zwischen 0 ... 2*PI
			if (Robot_Direction<0.0) Robot_Direction=2*M_PI+Robot_Direction;
			if (Robot_Direction>2*M_PI) Robot_Direction=0.0+(Robot_Direction-2*M_PI);
			// Sichtbarmachung der Roboterdrehung
			Kontakt=Calculate_Motion_Robot(Robot_Aktion,Objekt_Nummer,Objekt_Bereich);	
		}
	}
	else {
		if (Abstand<Robot_Movement_Radius+Target_Faktor*Ziel_Radius) {
			// Roboter am Ausgang
			// Timer stoppen
			Search_Way_Out->unschedule();
			// Stoppen der Auswegsuche
			Robot_Auto_Move=FALSE;
			Print_News_Screen(NOTEXT,"ROBOT ESCAPE",NOTEXT);
			//cout << "Ausgang erreicht !!!" << endl;	
		}
	}
}

// Betrachtung der Total_Rotation
void Calculate_Total_Rotation(void) {
	double Abstand,Total_Abstand;
	// Abstand zwischen IST/SOLL-Winkel ermitteln
	Abstand=Calculate_Rotation_Distance(Robot_Direction,Robot_New_Direction);
	if (Auto_Search_Modus==MODULO_METHODE) {
		Total_Abstand=Calculate_Modulo(Total_Rotation,2*M_PI);
		// Fuer Modulo Methode geringsten Abstand: zwischen Total_Rotation und Anzahl*2*M_PI ermitteln
		if (Robot_Rotation_Direction==LEFT) Total_Abstand=2*M_PI-Total_Abstand;
	}
	if (Auto_Search_Modus==PLEDGE_METHODE) {
		// Fuer Pledge Methode: Distanze von Total_Direction und 0.0
		if (Total_Rotation<0.0) Total_Abstand=Total_Rotation*-1;
		else Total_Abstand=Total_Rotation;
	}
	if ((Total_Abstand<Abstand)&&(First_Kontakt==FALSE)) {
		// neue Richtung des Roboters begrenzen
		if (Robot_Rotation_Direction==RIGHT) Robot_New_Direction=Robot_Direction-Total_Abstand;
		else Robot_New_Direction=Robot_Direction+Total_Abstand;
		// Rotationsrichtung ermitteln um neue Bewegungsrichtung zu erreichen
		// und die dazu benoetigten Schritte
		Robot_Rotation_Direction=Calculate_Rotation_Direction(Robot_Direction,Robot_New_Direction,Step_Robot_Rot,Steps_of_Rotations); 
		// Abstand zwischen IST/SOLL-Winkel ermitteln
		Abstand=Calculate_Rotation_Distance(Robot_Direction,Robot_New_Direction);
	}
	// neue Totale Richtungsaenderung ermitteln
	New_Total_Rotation=Total_Rotation+Abstand*Robot_Rotation_Direction;
}

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
void Calculate_Turn_Point(Point_3D Current_Point,int Objekt_Nummer,int Objekt_Bereich,bool Erster_Kontakt,int Umfahrung,double& Richtung,int& Wende_Bereich,Point_3D& Wendepunkt,Point_3D& Eckpunkt) {
	double Objekt_Rotation,Objekt_Radius,Abstand;
	int Objekt_Typ;
	Point_3D Min_Point,Max_Point;
	// von der Objekt-Nummer wird der aktuelle Objekt-Typ ermittelt
	Objekt_Typ=Szene_Objects.Read_Object_Typ(Objekt_Nummer);
	// Ermittlung ob aktuelles Objekt gedreht ist
	// Quader gedreht ?
	if (Objekt_Typ==CUBE) Objekt_Rotation=Szene_Objects.Read_Object_Rotation(Objekt_Nummer);
	// Zylinder und Kegel sind nicht in Z-Achse verdreht
	else Objekt_Rotation=0.0;
	// Ermittlung des Min/Max-Pointes des Objektes
	Szene_Objects.Read_Object_MinMax_Point(Objekt_Nummer,Min_Point,Max_Point);
	// Abstand von Objekt ermitteln
	if ((Objekt_Typ==CYLINDER)||(Objekt_Typ==CONE)) {
		Objekt_Radius=Szene_Objects.Read_Object_Radius(Objekt_Nummer);
		Abstand=Robot_Movement_Radius+Objekt_Radius;
	}
	else Abstand=Robot_Movement_Radius;
	// neue Bewegungsrichtung und anstehenden Wendepunkt ermitteln
	if (Objekt_Bereich==1) {
		if (Umfahrung==RIGHT) {
			Richtung=Objekt_Rotation+1.5*M_PI;
			Wendepunkt.X=Current_Point.X;
			Wendepunkt.Y=Min_Point.Y-Abstand;
			Wende_Bereich=5;
			// Eckpunkt ermitteln
			Eckpunkt.X=Min_Point.X;
			Eckpunkt.Y=Min_Point.Y;
		}
		else {
			Richtung=Objekt_Rotation+M_PI/2;
			Wendepunkt.X=Current_Point.X;
			Wendepunkt.Y=Max_Point.Y+Abstand;
			Wende_Bereich=9;
			// Eckpunkt ermitteln
			Eckpunkt.X=Min_Point.X;
			Eckpunkt.Y=Max_Point.Y;
		}
	}
	if (Objekt_Bereich==2) {
		if (Umfahrung==RIGHT) {
			Richtung=Objekt_Rotation+M_PI/2;
			Wendepunkt.X=Current_Point.X;
			Wendepunkt.Y=Max_Point.Y+Abstand;
			Wende_Bereich=10;
			// Eckpunkt ermitteln
			Eckpunkt.X=Max_Point.X;
			Eckpunkt.Y=Max_Point.Y;
		}
		else {
			Richtung=Objekt_Rotation+1.5*M_PI;
			Wendepunkt.X=Current_Point.X;
			Wendepunkt.Y=Min_Point.Y-Abstand;
			Wende_Bereich=6;
			// Eckpunkt ermitteln
			Eckpunkt.X=Max_Point.X;
			Eckpunkt.Y=Min_Point.Y;
		}
	}
	if (Objekt_Bereich==4) {
		if (Umfahrung==RIGHT) {
			Richtung=Objekt_Rotation;
			Wendepunkt.X=Max_Point.X+Abstand;
			Wendepunkt.Y=Current_Point.Y;
			Wende_Bereich=6;
			// Eckpunkt ermitteln
			Eckpunkt.X=Max_Point.X;
			Eckpunkt.Y=Min_Point.Y;
		}
		else {
			Richtung=Objekt_Rotation+M_PI;
			Wendepunkt.X=Min_Point.X-Abstand;
			Wendepunkt.Y=Current_Point.Y;
			Wende_Bereich=5;
			// Eckpunkt ermitteln
			Eckpunkt.X=Min_Point.X;
			Eckpunkt.Y=Min_Point.Y;
		}
	}
	if (Objekt_Bereich==8) {
		if (Umfahrung==RIGHT) {
			Richtung=Objekt_Rotation+M_PI;
			Wendepunkt.X=Min_Point.X-Abstand;
			Wendepunkt.Y=Current_Point.Y;
			Wende_Bereich=9;
			// Eckpunkt ermitteln
			Eckpunkt.X=Min_Point.X;
			Eckpunkt.Y=Max_Point.Y;
		}
		else {
			Richtung=Objekt_Rotation;
			Wendepunkt.X=Max_Point.X+Abstand;
			Wendepunkt.Y=Current_Point.Y;
			Wende_Bereich=10;
			// Eckpunkt ermitteln
			Eckpunkt.X=Max_Point.X;
			Eckpunkt.Y=Max_Point.Y;
		}
	}
	if (Erster_Kontakt==TRUE) {
		if (Objekt_Bereich==5) {
			if (Umfahrung==RIGHT) {
				Richtung=Objekt_Rotation+1.5*M_PI;
				Wendepunkt.X=Current_Point.X;
				Wendepunkt.Y=Min_Point.Y-Abstand;
				Wende_Bereich=5;
				// Eckpunkt ermitteln
				Eckpunkt.X=Min_Point.X;
				Eckpunkt.Y=Min_Point.Y;
			}
			else {
				Richtung=Objekt_Rotation+M_PI;
				Wendepunkt.X=Min_Point.X-Abstand;
				Wendepunkt.Y=Current_Point.Y;
				Wende_Bereich=5;
				// Eckpunkt ermitteln
				Eckpunkt.X=Min_Point.X;
				Eckpunkt.Y=Min_Point.Y;
			}
		}
		if (Objekt_Bereich==6) {
			if (Umfahrung==RIGHT) {
				Richtung=Objekt_Rotation;
				Wendepunkt.X=Max_Point.X+Abstand;
				Wendepunkt.Y=Current_Point.Y;
				Wende_Bereich=6;
				// Eckpunkt ermitteln
				Eckpunkt.X=Max_Point.X;
				Eckpunkt.Y=Min_Point.Y;
			}
			else {
				Richtung=Objekt_Rotation+1.5*M_PI;
				Wendepunkt.X=Current_Point.X;
				Wendepunkt.Y=Min_Point.Y-Abstand;
				Wende_Bereich=6;
				// Eckpunkt ermitteln
				Eckpunkt.X=Max_Point.X;
				Eckpunkt.Y=Min_Point.Y;
			}
		}
		if (Objekt_Bereich==9) {
			if (Umfahrung==RIGHT) {
				Richtung=Objekt_Rotation+M_PI;
				Wendepunkt.X=Min_Point.X-Abstand;
				Wendepunkt.Y=Current_Point.Y;
				Wende_Bereich=9;
				// Eckpunkt ermitteln
				Eckpunkt.X=Min_Point.X;
				Eckpunkt.Y=Max_Point.Y;
			}
			else {
				Richtung=Objekt_Rotation+M_PI/2;
				Wendepunkt.X=Current_Point.X;
				Wendepunkt.Y=Max_Point.Y+Abstand;
				Wende_Bereich=9;
				// Eckpunkt ermitteln
				Eckpunkt.X=Min_Point.X;
				Eckpunkt.Y=Max_Point.Y;
			}
		}
		if (Objekt_Bereich==10) {
			if (Umfahrung==RIGHT) {
				Richtung=Objekt_Rotation+M_PI/2;
				Wendepunkt.X=Current_Point.X;
				Wendepunkt.Y=Max_Point.Y+Abstand;
				Wende_Bereich=10;
				// Eckpunkt ermitteln
				Eckpunkt.X=Max_Point.X;
				Eckpunkt.Y=Max_Point.Y;
			}
			else {
				Richtung=Objekt_Rotation;
				Wendepunkt.X=Max_Point.X+Abstand;
				Wendepunkt.Y=Current_Point.Y;
				Wende_Bereich=10;
				// Eckpunkt ermitteln
				Eckpunkt.X=Max_Point.X;
				Eckpunkt.Y=Max_Point.Y;
			}
		}
	}
	else {
		if (Objekt_Bereich==9) {
			if (Umfahrung==RIGHT) {
				Richtung=Objekt_Rotation+1.5*M_PI;
				Wendepunkt.X=Current_Point.X;
				Wendepunkt.Y=Min_Point.Y-Abstand;
				Wende_Bereich=5;
				// Eckpunkt ermitteln
				Eckpunkt.X=Min_Point.X;
				Eckpunkt.Y=Min_Point.Y;
			}
			else {
				Richtung=Objekt_Rotation;
				Wendepunkt.X=Max_Point.X+Abstand;
				Wendepunkt.Y=Current_Point.Y;
				Wende_Bereich=10;
				// Eckpunkt ermitteln
				Eckpunkt.X=Max_Point.X;
				Eckpunkt.Y=Max_Point.Y;
			}
		}
		if (Objekt_Bereich==6) {
			if (Umfahrung==RIGHT) {
				Richtung=Objekt_Rotation+M_PI/2;
				Wendepunkt.X=Current_Point.X;
				Wendepunkt.Y=Max_Point.Y+Abstand;
				Wende_Bereich=10;
				// Eckpunkt ermitteln
				Eckpunkt.X=Max_Point.X;
				Eckpunkt.Y=Max_Point.Y;
			}
			else {
				Richtung=Objekt_Rotation+M_PI;
				Wendepunkt.X=Min_Point.X-Abstand;
				Wendepunkt.Y=Current_Point.Y;
				Wende_Bereich=5;
				// Eckpunkt ermitteln
				Eckpunkt.X=Min_Point.X;
				Eckpunkt.Y=Min_Point.Y;
			}
		}
		if (Objekt_Bereich==5) {
			if (Umfahrung==RIGHT) {
				Richtung=Objekt_Rotation;
				Wendepunkt.X=Max_Point.X+Abstand;
				Wendepunkt.Y=Current_Point.Y;
				Wende_Bereich=6;
				// Eckpunkt ermitteln
				Eckpunkt.X=Max_Point.X;
				Eckpunkt.Y=Min_Point.Y;
			}
			else {
				Richtung=Objekt_Rotation+M_PI/2;
				Wendepunkt.X=Current_Point.X;
				Wendepunkt.Y=Max_Point.Y+Abstand;
				Wende_Bereich=9;
				// Eckpunkt ermitteln
				Eckpunkt.X=Min_Point.X;
				Eckpunkt.Y=Max_Point.Y;
			}
		}
		if (Objekt_Bereich==10) {
			if (Umfahrung==RIGHT) {
				Richtung=Objekt_Rotation+M_PI;
				Wendepunkt.X=Min_Point.X-Abstand;
				Wendepunkt.Y=Current_Point.Y;
				Wende_Bereich=9;
				// Eckpunkt ermitteln
				Eckpunkt.X=Min_Point.X;
				Eckpunkt.Y=Max_Point.Y;
			}
			else {
				Richtung=Objekt_Rotation+1.5*M_PI;
				Wendepunkt.X=Current_Point.X;
				Wendepunkt.Y=Min_Point.Y-Abstand;
				Wende_Bereich=6;
				// Eckpunkt ermitteln
				Eckpunkt.X=Max_Point.X;
				Eckpunkt.Y=Min_Point.Y;
			}
		}
	}
	// Eckpunktekoordinaten berechnen
	if (Objekt_Rotation!=0.0) {
		Point_3D Objekt_Mittelpunkt=Szene_Objects.Read_Object_MiddelPoint(Objekt_Nummer);
		Szene_Objects.Rotation_Point(Objekt_Rotation,Objekt_Mittelpunkt,Eckpunkt);
	}
}

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
double Calculate_Rotation_Direction (double Ist_Winkel, double Soll_Winkel,double Schrittweite,int& Steps) {
	double Direction,Abstand;
	if (Ist_Winkel>Soll_Winkel) {
		if ((Ist_Winkel-Soll_Winkel)<(2*M_PI-Ist_Winkel+Soll_Winkel)) Direction=-1;
		else Direction=LEFT;
	}
	else {
		if ((Soll_Winkel-Ist_Winkel)<(2*M_PI-Soll_Winkel+Ist_Winkel)) Direction=+1;
		else Direction=RIGHT;
	}
	// Abstand zwischen IST/SOLL-Winkel ermitteln
	Abstand=Calculate_Rotation_Distance(Ist_Winkel,Soll_Winkel);
	// Drehschritte ermitteln
	Steps=(int)(Abstand/Schrittweite);
	return Direction;
}

// Ermittlung des Abstandes zwischen einem gegebenem Winkel und einem
// anderen gegebenem Winkel
// Eingabe: Ist-Winkel
//			Soll-Winkel
// Ausgabe: minimaler Abstand zwischen den Winkeln
double Calculate_Rotation_Distance (double Ist_Winkel, double Soll_Winkel) {
	double Abstand;
	if (Ist_Winkel>Soll_Winkel) {
		if ((Ist_Winkel-Soll_Winkel)<(2*M_PI-Ist_Winkel+Soll_Winkel)) Abstand=Ist_Winkel-Soll_Winkel;
		else Abstand=2*M_PI-Ist_Winkel+Soll_Winkel;
	}
	else {
		if ((Soll_Winkel-Ist_Winkel)<(2*M_PI-Soll_Winkel+Ist_Winkel)) Abstand=Soll_Winkel-Ist_Winkel;
		else Abstand=2*M_PI-Soll_Winkel+Ist_Winkel;
	}
	return Abstand;
}

// Berechnung: Rest = Wert mod Modul
// Eingabe: Wert
//			Modul
// Rueckgabe: Rest
double Calculate_Modulo(double Wert,double Modul) {
	if (Wert>=0.0) {
		while (Wert>=0.0) {
			Wert=Wert-Modul;
		}
		Wert=Wert+Modul;
	}
	else {
		while (Wert<0.0) {
			Wert=Wert+Modul;
		}
	}
	return Wert;
}

// Funktion zur Initialisierung einer automatischen
// Zielsuche in einem vorher eingelesenem Labyrinth
// Eingabe: Suchalgorithmus
//          BUG_METHODE
//		CORNER_METHODE
void Init_Target_Search(int Proceed) {
	Robot_Auto_Move=TRUE;
	// Original Roboterzielkoordinaten sichern
	Point_3D Robot_Target_Help=Robot_Target;
	// Zielrichtung ermitteln	
	Robot_New_Direction=Rotation_Koord(0.0,Robot_Koord,Robot_Target_Help);
	// Rotationsrichtung ermitteln
	Robot_Rotation_Direction=Calculate_Rotation_Direction(Robot_Direction,Robot_New_Direction,Step_Robot_Rot,Steps_of_Rotations);
	// kein Huellenkontakt
	Object_Hull_Kontakt=FALSE;
	// Richtung der Umrundung eines Objektes per Zufall ermitteln
	if (Current_Random_Number<0.5) Umrundung=RIGHT;
	else Umrundung=LEFT;
	if (Proceed==BUG_METHODE) {
		// Bug Loesungsalgorithmus
		// Distanz vom Beruehrungspunkt eines Objektes bis zur aktuellen Roboterposition
		Kontakt_Current_Distance=0.0;
		// Distanz vom Beruehrungspunkt bis zum Punkt der dem Ziel am naechsten ist
		Kontakt_Closest_Distance=0.0;
		//cout << "Bug Suchmethode laeuft !!!" << endl;
		Auto_Search_Modus=BUG_METHODE;
		Go_To_Closest_Point=FALSE;
		// Timer-Intervall setzen
		BugSearch_Target->setInterval(Time_Move_Robot);
		// Timer starten
		BugSearch_Target->schedule();
	}
	if (Proceed==CORNER_METHODE) {
		// CORNER Loesungsalgorithmus
		//cout << "Corner Suchmethode laeuft !!!" << endl;
		Auto_Search_Modus=CORNER_METHODE;
		CornerSearch_Target->setInterval(Time_Move_Robot);
		// Timer starten
		CornerSearch_Target->schedule();
	}
	// Robotergelenke in die Ausgangsposition bringen
	Reset_Robot_Joints();
}

// Interrupt-Funktion zur Realisierung der Bug-Methode zur Zielsuche
//  in einem Labyrinth
void Robot_BugSearch_Target(void*,SoSensor*) {
	double Abstand;
	int Robot_Aktion,Objekt_Bereich,Objekt_Nummer;
	bool Kontakt;
	// Wendung des Roboters um an Objekt-Huelle zu bleiben
	// FALSE noch nicht durchgefuehrt
	// TRUE durchgefuehrt
	bool Robot_Turn;
	// Abstand des Roboters vom Ziel ermitteln
	Abstand=Calculate_Point_Distance(Robot_Koord,Robot_Target);
	if ((Robot_Foot_Move!=TRUE)&&(Abstand>Robot_Movement_Radius+Target_Faktor*Ziel_Radius)) {
		// Roboter weiter zum Ausweg fuehren
		// Variablen initialisieren
		Kontakt=FALSE;
		Robot_Aktion=0;
		Objekt_Bereich=NO_OBJEKT_AREA;
		Objekt_Nummer=0;

		
		//cout << "Robot_Koord_X=" << Robot_Koord.X << endl;
		//cout << "Robot_Koord_Y=" << Robot_Koord.Y << endl << endl;
		//cout << "Robot_Direction=" << Robot_Direction << endl;
		
		//cout << "Kontakt_Robot_Position_X=" << Kontakt_Robot_Position.X << endl;
		//cout << "Kontakt_Robot_Position_Y=" << Kontakt_Robot_Position.Y << endl << endl;
		//cout << "Kontakt_Current_Distance=" << Kontakt_Current_Distance << endl;
		//cout << "Kontakt_Closest_Distance=" << Kontakt_Closest_Distance << endl;

		// in aktuelle Laufrichtung drehen ?
		if (Robot_Direction==Robot_New_Direction) {
			// Laufrichtung erreicht 
			// Laufe nun an der Objekt-Huelle entlang oder entlang der
			// eingeschlagenen Lauf-Richtung
			if (Object_Hull_Kontakt==FALSE) {
				// Laufe in eingeschlagener Laufrichtung
				Robot_Aktion=TRANSLATION_FORWARD;
				Kontakt=Calculate_Motion_Robot(Robot_Aktion,Objekt_Nummer,Objekt_Bereich);
				// Roboter beim fahren in Startrichtung auf ein Objekt gestossen 
				if (Kontakt==TRUE) {
					// Roboter an der Objekt-Huelle entlangschicken
					Object_Hull_Kontakt=TRUE;
					Object_Hull_Number=Objekt_Nummer;
					// Roboter hat ersten Kontakt mit dem Objekt
					First_Kontakt=TRUE;
					// neue Richtung und anstehenden Wendepunkt ermitteln
					Calculate_Turn_Point(Robot_Koord,Object_Hull_Number,Objekt_Bereich,First_Kontakt,Umrundung,Robot_New_Direction,Turn_Area,Turn_Point,Corner);
					// Rotationsrichtung ermitteln um neue Bewegungsrichtung zu erreichen
					// und die dazu benoetigten Schritte
					Robot_Rotation_Direction=Calculate_Rotation_Direction(Robot_Direction,Robot_New_Direction,Step_Robot_Rot,Steps_of_Rotations);			
					// nur Kantenpunkte werden wieder erreicht bei Erstkontakt !!!
					if ((Objekt_Bereich==1)||(Objekt_Bereich==2)||(Objekt_Bereich==4)||(Objekt_Bereich==8)) {
						// Roboterposition merken wo Objekt beruehrt wurde
						Kontakt_Robot_Position=Robot_Koord;
						// Roboterposition des bisherigen kleinsten Abstand: Roboter-Ziel 
						Closest_Target_Position=Kontakt_Robot_Position;
						// Roboter muss nicht aus der Ecke herausgehen
						Robot_Go_Out_Of_Corner=FALSE;
					}
					else {
						// bei Erst-Kontakt mit einem Objekt in den Bereichen 5,6,9,10
						//  muss der Roboter zuerst aus der Ecke herausgehen
						Robot_Go_Out_Of_Corner=TRUE;
					}
				}
			}
			else {
				Robot_Turn=FALSE;
				// Aktuellen Objektbereich ermitteln
				Objekt_Nummer=Object_Hull_Number;
				Kontakt=Objekt_Kontakt(Robot_Koord,Objekt_Nummer,Objekt_Bereich);
				// Koordinaten des Roboters um den Wert 
				// der Objektrotation rotieren
				//
				// Original Roboterkoordinaten kopieren
				Point_3D Robot_Koord_Test=Robot_Koord;
				// Ermittlung ob aktuelles Objekt gedreht ist
				double Phi=Szene_Objects.Read_Object_Rotation(Objekt_Nummer);
				if (Phi!=0.0) {
					// Mittelpunkt des zu umlaufenden Objektes auslesen
					Point_3D Mittelpunkt=Szene_Objects.Read_Object_MiddelPoint(Objekt_Nummer);
					// Koordinaten anpassen
					Rotation_Koord(Phi,Mittelpunkt,Robot_Koord_Test);
				}
				// Wendepunkt schon erreicht ?
				if (Umrundung==RIGHT) {
					if ((Objekt_Bereich==Turn_Area)&&(Turn_Area==6)&&(Robot_Koord_Test.X>=Turn_Point.X)) Robot_Turn=TRUE;
					if ((Objekt_Bereich==Turn_Area)&&(Turn_Area==10)&&(Robot_Koord_Test.Y>=Turn_Point.Y)) Robot_Turn=TRUE;
					if ((Objekt_Bereich==Turn_Area)&&(Turn_Area==9)&&(Robot_Koord_Test.X<=Turn_Point.X)) Robot_Turn=TRUE;
					if ((Objekt_Bereich==Turn_Area)&&(Turn_Area==5)&&(Robot_Koord_Test.Y<=Turn_Point.Y)) Robot_Turn=TRUE;
				}
				else {
					if ((Objekt_Bereich==Turn_Area)&&(Turn_Area==6)&&(Robot_Koord_Test.Y<=Turn_Point.Y)) Robot_Turn=TRUE;
					if ((Objekt_Bereich==Turn_Area)&&(Turn_Area==10)&&(Robot_Koord_Test.X>=Turn_Point.X)) Robot_Turn=TRUE;
					if ((Objekt_Bereich==Turn_Area)&&(Turn_Area==9)&&(Robot_Koord_Test.Y>=Turn_Point.Y)) Robot_Turn=TRUE;
					if ((Objekt_Bereich==Turn_Area)&&(Turn_Area==5)&&(Robot_Koord_Test.X<=Turn_Point.X)) Robot_Turn=TRUE;
				}
				if (Robot_Turn==TRUE) {
					// erster Kontakt mit dem Objekt ist vorbei
					First_Kontakt=FALSE;
					if (Robot_Go_Out_Of_Corner==TRUE) {
						// Roboterposition merken wo Objekt beruehrt wurde
						Kontakt_Robot_Position=Robot_Koord;
						// Roboterposition des bisherigen kleinsten Abstand: Roboter-Ziel 
						Closest_Target_Position=Kontakt_Robot_Position;
						// Roboter ist aus der Ecke herausgefahren
						Robot_Go_Out_Of_Corner=FALSE;
					}
					// Test ob Ecke mit einem anderem Objekt gleich ist
					int New_Object_Hull_Number=Szene_Objects.Read_Same_Corner(Object_Hull_Number,Corner);
					if (New_Object_Hull_Number!=0) {
						Object_Hull_Number=New_Object_Hull_Number;
						// Uebergang zu einem neuen Objekt
						Objekt_Nummer=Object_Hull_Number;
						// aktuellen Objekt-Bereich des neuen Objektes ermitteln
						Objekt_Kontakt(Robot_Koord,Objekt_Nummer,Objekt_Bereich);
					}
					// neue Richtung und anstehenden Wendepunkt ermitteln
					Calculate_Turn_Point(Robot_Koord,Objekt_Nummer,Objekt_Bereich,FALSE,Umrundung,Robot_New_Direction,Turn_Area,Turn_Point,Corner);
					// Rotationsrichtung ermitteln um neue Bewegungsrichtung zu erreichen
					// und die dazu benoetigten Schritte
					Robot_Rotation_Direction=Calculate_Rotation_Direction(Robot_Direction,Robot_New_Direction,Step_Robot_Rot,Steps_of_Rotations);		
				}
				else {
					// Laufe geradeaus an Objekt-Huelle entlang 
					Robot_Aktion=TRANSLATION_FORWARD;
					// Speicherung der alten Roboterposition
					Old_Robot_Koord=Robot_Koord;
					// Objekt-Nummer des zu umfahrenden Objektes auslesen
					Objekt_Nummer=Object_Hull_Number;
					// Test ob ein neues Objekt beruehrt wird
					Kontakt=Calculate_Motion_Robot(Robot_Aktion,Objekt_Nummer,Objekt_Bereich);
					if ((Go_To_Closest_Point==FALSE)&&(Robot_Go_Out_Of_Corner==FALSE)) {
						// Ermittlung der Laenge des zurueckgelegten Weges
						//  zwischen Beruehrungspunkt und aktuellen Roboter-Koordinaten
						Kontakt_Current_Distance=Kontakt_Current_Distance+Calculate_Point_Distance(Old_Robot_Koord,Robot_Koord);
					}
					if (Kontakt==FALSE) {
						// Nur bei erster Umrundung des Objektes
						if ((Go_To_Closest_Point==FALSE)&&(Robot_Go_Out_Of_Corner==FALSE)) {
							// Test ob Abstand Roboter-Ziel kleiner
							if (Calculate_Point_Distance(Closest_Target_Position,Robot_Target)>=Calculate_Point_Distance(Robot_Koord,Robot_Target)) {
								// Roboterposition des jetzigen kleinsten Abstandes: Roboter-Ziel 
								Closest_Target_Position=Robot_Koord;
								// Weglaenge von Beruehrungspunkt zum naheliegensten Punkt zum Ziel speichern
								Kontakt_Closest_Distance=Kontakt_Current_Distance;
							}
						}
						// Roboter laeuft um das beruehrte Objekt
						if ((Go_To_Closest_Point==FALSE)&&(Robot_Go_Out_Of_Corner==FALSE)) {
							// Roboter wieder in Ausgangsposition ?
							if (Calculate_Point_Distance(Robot_Koord,Kontakt_Robot_Position)<Step_Robot_Trans/2) {
								// Gehe jetzt zum dem Punkt der dem Ziel am naechsten ist
								Go_To_Closest_Point=TRUE;
								// Laufrichtung zur Umrundung des Objektes ermitteln
								if (Kontakt_Closest_Distance>(Kontakt_Current_Distance-Kontakt_Closest_Distance)) {
									// anders herum
									if (Umrundung==RIGHT) Umrundung=LEFT;
									else Umrundung=RIGHT;
									// aktuellen Bereich des Roboters zum beruehrten Objekt ermitteln
									Point_3D Robot_Koord_Test=Robot_Koord;
									Objekt_Nummer=Object_Hull_Number;
									Objekt_Kontakt(Robot_Koord_Test,Objekt_Nummer,Objekt_Bereich);
									// neue Richtung und neuen Wendepunkt ermitteln
									Calculate_Turn_Point(Robot_Koord,Objekt_Nummer,Objekt_Bereich,FALSE,Umrundung,Robot_New_Direction,Turn_Area,Turn_Point,Corner);
									// Rotationsrichtung ermitteln um neue Bewegungsrichtung zu erreichen
									// und die dazu benoetigten Schritte
									Robot_Rotation_Direction=Calculate_Rotation_Direction(Robot_Direction,Robot_New_Direction,Step_Robot_Rot,Steps_of_Rotations);	
								}
								// Weglaengen loeschen
								// Distanz vom Beruehrungspunkt eines Objektes bis zur aktuellen Roboterposition
								Kontakt_Current_Distance=0.0;
								// Distanz vom Beruehrungspunkt bis zum Punkt der dem Ziel am naechsten ist
								Kontakt_Closest_Distance=0.0;
							}
						}
						// Roboter geht zum naheliegensten Punkt zum Ziel
						if (Go_To_Closest_Point==TRUE) {
							// Roboter am naheliegensten Punkt zum Ziel ?
							if (Calculate_Point_Distance(Closest_Target_Position,Robot_Koord)<Step_Robot_Trans/2) {
								// Roboter-Koordinaten auf genau naechsten Punkt zum Ziel setzen
								Robot_Koord=Closest_Target_Position;
								// neue Richtung bestimmen
								// Original Roboterkoordinaten sichern
								Point_3D Robot_Target_Help=Robot_Target;
								// Zielrichtung ermitteln
								Robot_New_Direction=Rotation_Koord(0.0,Robot_Koord,Robot_Target_Help);
								// Rotationsrichtung ermitteln um neue Bewegungsrichtung zu erreichen
								// und die dazu benoetigten Schritte
								Robot_Rotation_Direction=Calculate_Rotation_Direction(Robot_Direction,Robot_New_Direction,Step_Robot_Rot,Steps_of_Rotations);
								Print_News_Screen(NOTEXT,OLDTEXT,NOTEXT);
								// vom Objekt loesen
								Object_Hull_Kontakt=FALSE;
								// zum naechsten Punkt gehen loeschen
								Go_To_Closest_Point=FALSE;
								// Distanz vom Beruehrungspunkt eines Objektes bis zur aktuellen Roboterposition
								Kontakt_Current_Distance=0.0-Step_Robot_Trans;
								// Laufrichtung zur Umrundung per Zufall setzen
								if (Current_Random_Number<0.5) Umrundung=RIGHT;
								else Umrundung=LEFT;
							}
						}
					}
					else  {					
						// Roboter beim fahren entlang der Objekt-Huelle auf ein Objekt gestossen 
						// Roboter an neuer Objekt-Huelle entlangschicken
						Object_Hull_Kontakt=TRUE;
						Object_Hull_Number=Objekt_Nummer;
						// Roboter hat ersten Kontakt mit dem Objekt
						First_Kontakt=TRUE;
						// neue Richtung und anstehenden Wendepunkt ermitteln
						Calculate_Turn_Point(Robot_Koord,Objekt_Nummer,Objekt_Bereich,First_Kontakt,Umrundung,Robot_New_Direction,Turn_Area,Turn_Point,Corner);
						// Rotationsrichtung ermitteln um neue Bewegungsrichtung zu erreichen
						// und die dazu benoetigten Schritte
						Robot_Rotation_Direction=Calculate_Rotation_Direction(Robot_Direction,Robot_New_Direction,Step_Robot_Rot,Steps_of_Rotations);			
					}
				}
			}
		}
		else {
			// Roboter muss noch gedreht werden !!!
			Robot_Aktion=ROTATION;
			if (Steps_of_Rotations!=0) {
				// Schritte bis zur neuen Richtung um eins vermindern
				Steps_of_Rotations--;
				// Roboter in neue Richtung drehen
				if (Robot_Rotation_Direction==RIGHT) {
					// in Uhrzeigersinn drehen
					Robot_Direction=Robot_Direction-Step_Robot_Rot;
				}
				else {		
					// gegen Uhrzeigersinn drehen
					Robot_Direction=Robot_Direction+Step_Robot_Rot;					
				}
			}
			else {
				// Roboter-Richtung auf exakte Lauf-Richtung setzen
				Robot_Direction=Robot_New_Direction;
				// Roboter-Fuss wieder einfahren
				Kontakt=Calculate_Motion_Robot(TRANSLATION_FORWARD,Objekt_Nummer,Objekt_Bereich);
			}
			// Robot_Direction zwischen 0 ... 2*PI
			if (Robot_Direction<0.0) Robot_Direction=2*M_PI+Robot_Direction;
			if (Robot_Direction>2*M_PI) Robot_Direction=0.0+(Robot_Direction-2*M_PI);
			// Sichtbarmachung der Roboterdrehung
			Kontakt=Calculate_Motion_Robot(Robot_Aktion,Objekt_Nummer,Objekt_Bereich);	
		}
	}
	else {
		if (Abstand<Robot_Movement_Radius+Target_Faktor*Ziel_Radius) {
			// Roboter am Ziel
			// Timer stoppen
			BugSearch_Target->unschedule();
			// Stoppen der Zielsuche
			Robot_Auto_Move=FALSE;
			Print_News_Screen(NOTEXT,"TARGET",NOTEXT);
			//cout << "Ziel erreicht !!!" << endl;	
		}
	}
}

// Euklidschen Abstand zwischen zwei Punkten (2D) ermitteln
// Eingabe: Punkt A
//			Punkt B
// Rueckgabe: Euklidscher Abstand
double Calculate_Point_Distance(Point_3D Punkt_A,Point_3D Punkt_B) {
	double Abstand=sqrt(pow(Punkt_A.X-Punkt_B.X,2)+pow(Punkt_A.Y-Punkt_B.Y,2));
	return Abstand;
}

// Interrupt-Funktion zur Realisierung der Corner-Methode zur Zielsuche
//  in einem Labyrinth
void Robot_CornerSearch_Target(void*,SoSensor*) {
	double Abstand,Target_Direction;
	int Robot_Aktion,Objekt_Bereich,Objekt_Nummer;
	bool Kontakt;
	// Wendung des Roboters um an Objekt-Huelle zu bleiben
	// FALSE noch nicht durchgefuehrt
	// TRUE durchgefuehrt
	bool Robot_Turn;
	// Abstand des Roboters vom Ziel ermitteln
	Abstand=Calculate_Point_Distance(Robot_Koord,Robot_Target);
	if ((Robot_Foot_Move!=TRUE)&&(Abstand>Robot_Movement_Radius+Target_Faktor*Ziel_Radius)) {
		// Roboter weiter zum Ausweg fuehren
		// Variablen initialisieren
		Kontakt=FALSE;
		Robot_Aktion=0;
		Objekt_Bereich=NO_OBJEKT_AREA;
		Objekt_Nummer=0;

		// in aktuelle Laufrichtung drehen ?
		if (Robot_Direction==Robot_New_Direction) {
			// Laufrichtung erreicht 
			// Laufe nun an der Objekt-Huelle entlang oder entlang der
			// eingeschlagenen Lauf-Richtung
			if (Object_Hull_Kontakt==FALSE) {
				// Laufe in eingeschlagener Laufrichtung
				Robot_Aktion=TRANSLATION_FORWARD;
				Kontakt=Calculate_Motion_Robot(Robot_Aktion,Objekt_Nummer,Objekt_Bereich);
				// Roboter beim fahren in Startrichtung auf ein Objekt gestossen 
				if (Kontakt==TRUE) {
					// Roboter an der Objekt-Huelle entlangschicken
					Object_Hull_Kontakt=TRUE;
					Object_Hull_Number=Objekt_Nummer;
					// Roboter hat ersten Kontakt mit dem Objekt
					First_Kontakt=TRUE;
					// neue Richtung und anstehenden Wendepunkt ermitteln
					Calculate_Turn_Point(Robot_Koord,Object_Hull_Number,Objekt_Bereich,First_Kontakt,Umrundung,Robot_New_Direction,Turn_Area,Turn_Point,Corner);
					// Rotationsrichtung ermitteln um neue Bewegungsrichtung zu erreichen
					// und die dazu benoetigten Schritte
					Robot_Rotation_Direction=Calculate_Rotation_Direction(Robot_Direction,Robot_New_Direction,Step_Robot_Rot,Steps_of_Rotations);			
				}
			}
			else {
				Robot_Turn=FALSE;
				// Aktuellen Objektbereich ermitteln
				Objekt_Nummer=Object_Hull_Number;
				Kontakt=Objekt_Kontakt(Robot_Koord,Objekt_Nummer,Objekt_Bereich);
				// Koordinaten des Roboters um den Wert 
				// der Objektrotation rotieren
				//
				// Original Roboterkoordinaten kopieren
				Point_3D Robot_Koord_Test=Robot_Koord;
				// Ermittlung ob aktuelles Objekt gedreht ist
				double Phi=Szene_Objects.Read_Object_Rotation(Objekt_Nummer);
				if (Phi!=0.0) {
					// Mittelpunkt des zu umlaufenden Objektes auslesen
					Point_3D Mittelpunkt=Szene_Objects.Read_Object_MiddelPoint(Objekt_Nummer);
					// Koordinaten anpassen
					Rotation_Koord(Phi,Mittelpunkt,Robot_Koord_Test);
				}
				// Wendepunkt schon erreicht ?
				if (Umrundung==RIGHT) {
					if ((Objekt_Bereich==Turn_Area)&&(Turn_Area==6)&&(Robot_Koord_Test.X>=Turn_Point.X)) Robot_Turn=TRUE;
					if ((Objekt_Bereich==Turn_Area)&&(Turn_Area==10)&&(Robot_Koord_Test.Y>=Turn_Point.Y)) Robot_Turn=TRUE;
					if ((Objekt_Bereich==Turn_Area)&&(Turn_Area==9)&&(Robot_Koord_Test.X<=Turn_Point.X)) Robot_Turn=TRUE;
					if ((Objekt_Bereich==Turn_Area)&&(Turn_Area==5)&&(Robot_Koord_Test.Y<=Turn_Point.Y)) Robot_Turn=TRUE;
				}
				else {
					if ((Objekt_Bereich==Turn_Area)&&(Turn_Area==6)&&(Robot_Koord_Test.Y<=Turn_Point.Y)) Robot_Turn=TRUE;
					if ((Objekt_Bereich==Turn_Area)&&(Turn_Area==10)&&(Robot_Koord_Test.X>=Turn_Point.X)) Robot_Turn=TRUE;
					if ((Objekt_Bereich==Turn_Area)&&(Turn_Area==9)&&(Robot_Koord_Test.Y>=Turn_Point.Y)) Robot_Turn=TRUE;
					if ((Objekt_Bereich==Turn_Area)&&(Turn_Area==5)&&(Robot_Koord_Test.X<=Turn_Point.X)) Robot_Turn=TRUE;
				}
				if (Robot_Turn==TRUE) {
					// Test ob Ecke mit einem anderem Objekt gleich ist
					int New_Object_Hull_Number=Szene_Objects.Read_Same_Corner(Object_Hull_Number,Corner);
					if (New_Object_Hull_Number!=0) {
						Object_Hull_Number=New_Object_Hull_Number;
						// Uebergang zu einem neuen Objekt
						Objekt_Nummer=Object_Hull_Number;
						// aktuellen Objekt-Bereich des neuen Objektes ermitteln
						Objekt_Kontakt(Robot_Koord,Objekt_Nummer,Objekt_Bereich);
					}
					// neue Richtung und anstehenden Wendepunkt ermitteln
					Calculate_Turn_Point(Robot_Koord,Objekt_Nummer,Objekt_Bereich,FALSE,Umrundung,Robot_New_Direction,Turn_Area,Turn_Point,Corner);
					// Rotationsrichtung ermitteln um neue Bewegungsrichtung zu erreichen
					//	und die dazu benoetigten Schritte
					Robot_Rotation_Direction=Calculate_Rotation_Direction(Robot_Direction,Robot_New_Direction,Step_Robot_Rot,Steps_of_Rotations);	
					// Test ob Zielpunkt erreicht werden kann
					// Original Roboterkoordinaten sichern
					Point_3D Robot_Target_Help=Robot_Target;
					// Richtung in dem der Zielpunkt liegt ermitteln	
					Target_Direction=Rotation_Koord(0.0,Robot_Koord,Robot_Target_Help);
					// Rotationsrichtung ermitteln um in Richtung Ziel zu gelangen
					//	und die dazu benoetigten Schritte
					int Steps_to_Target;
					double Target_Rotation_Direction;
					Target_Rotation_Direction=Calculate_Rotation_Direction(Robot_Direction,Target_Direction,Step_Robot_Rot,Steps_to_Target);		
					
					//if ((Target_Rotation_Direction==Robot_Rotation_Direction)&&(Steps_of_Rotations>=Steps_to_Target)) {
					if (Steps_of_Rotations>=Steps_to_Target) {
						// Richtung zum Ziel einschlagen
						Robot_New_Direction=Target_Direction;
						Robot_Rotation_Direction=Target_Rotation_Direction;
						Steps_of_Rotations=Steps_to_Target;
						Print_News_Screen(NOTEXT,OLDTEXT,NOTEXT);
						// von der Wand loesen
						Object_Hull_Kontakt=FALSE;
						// neue Richtung der Umrundung eines Objektes per Zufall ermitteln
						if (Current_Random_Number<0.5) Umrundung=RIGHT;
						else Umrundung=LEFT;
					}
					// erster Kontakt mit dem Objekt ist vorbei
					First_Kontakt=FALSE;	
				}
				else {
					// Laufe geradeaus an Objekt-Huelle entlang 
					Robot_Aktion=TRANSLATION_FORWARD;
					// Speicherung der alten Roboterposition
					Old_Robot_Koord=Robot_Koord;
					// Objekt-Nummer des zu umfahrenden Objektes auslesen
					Objekt_Nummer=Object_Hull_Number;
					// Test ob ein neues Objekt beruehrt wird
					Kontakt=Calculate_Motion_Robot(Robot_Aktion,Objekt_Nummer,Objekt_Bereich);
					if (Kontakt==TRUE) {					
						// Roboter beim fahren entlang der Objekt-Huelle auf ein Objekt gestossen 
						// Roboter an neuer Objekt-Huelle entlangschicken
						Object_Hull_Kontakt=TRUE;
						Object_Hull_Number=Objekt_Nummer;
						// Roboter hat ersten Kontakt mit dem Objekt
						First_Kontakt=TRUE;
						// neue Richtung und anstehenden Wendepunkt ermitteln
						Calculate_Turn_Point(Robot_Koord,Objekt_Nummer,Objekt_Bereich,First_Kontakt,Umrundung,Robot_New_Direction,Turn_Area,Turn_Point,Corner);
						// Rotationsrichtung ermitteln um neue Bewegungsrichtung zu erreichen
						// und die dazu benoetigten Schritte
						Robot_Rotation_Direction=Calculate_Rotation_Direction(Robot_Direction,Robot_New_Direction,Step_Robot_Rot,Steps_of_Rotations);			
					}
				}
			}
		}
		else {
			// Roboter muss noch gedreht werden !!!
			Robot_Aktion=ROTATION;
			if (Steps_of_Rotations!=0) {
				// Schritte bis zur neuen Richtung um eins vermindern
				Steps_of_Rotations--;
				// Roboter in neue Richtung drehen
				if (Robot_Rotation_Direction==RIGHT) {
					// in Uhrzeigersinn drehen
					Robot_Direction=Robot_Direction-Step_Robot_Rot;
				}
				else {		
					// gegen Uhrzeigersinn drehen
					Robot_Direction=Robot_Direction+Step_Robot_Rot;					
				}
			}
			else {
				// Roboter-Richtung auf exakte Lauf-Richtung setzen
				Robot_Direction=Robot_New_Direction;
				// Roboter-Fuss wieder einfahren
				Kontakt=Calculate_Motion_Robot(TRANSLATION_FORWARD,Objekt_Nummer,Objekt_Bereich);
			}
			// Robot_Direction zwischen 0 ... 2*PI
			if (Robot_Direction<0.0) Robot_Direction=2*M_PI+Robot_Direction;
			if (Robot_Direction>2*M_PI) Robot_Direction=0.0+(Robot_Direction-2*M_PI);
			// Sichtbarmachung der Roboterdrehung
			Kontakt=Calculate_Motion_Robot(Robot_Aktion,Objekt_Nummer,Objekt_Bereich);	
		}
	}
	else {
		if (Abstand<Robot_Movement_Radius+Target_Faktor*Ziel_Radius) {
			// Roboter am Ziel
			// Timer stoppen
			CornerSearch_Target->unschedule();
			// Stoppen der Zielsuche
			Robot_Auto_Move=FALSE;
			Print_News_Screen(NOTEXT,"TARGET",NOTEXT);
			//cout << "Ziel erreicht !!!" << endl;	
		}
	}
}

// Funktion zur Berechnung der Tool-Koordinaten im Weltkoordinatensystem
// Eingabe: im letzten Koordinatensystem um den Wert auf der Z-Achse
//			 verschoben
SbVec3f Calculate_Tool_Koord(double Offset) {
	SbVec3f Tool;
	// Theta1, Theta2, Theta3 und Theta4 sind mit Offsets behaftet
	// (wegen Darstellung), diese muessen nun dazuaddiert werden
	// und Anpassung an die Roboterdrehung
	double Theta1_=Theta[0]+Robot_Direction;
	double Theta2_=Theta[1]-M_PI/2;
	double Theta3_=Theta[2]+M_PI/2;
	double Theta4_=Theta[3]+M_PI/2;
	// Berechnung der transzendenten Funktionen
	double c1=cos(Theta1_);
	double s1=sin(Theta1_);
	double c2=cos(Theta2_);
	double s2=sin(Theta2_);
	double c23=cos(Theta2_)*cos(Theta3_)-sin(Theta2_)*sin(Theta3_);
	double s23=cos(Theta2_)*sin(Theta3_)+sin(Theta2_)*cos(Theta3_);
	double Theta23=acos(c23);
	double c234=cos(Theta23)*cos(Theta4_)-sin(Theta23)*sin(Theta4_);
	double s234=cos(Theta23)*sin(Theta4_)+sin(Theta23)*cos(Theta4_);
	// Koordinaten des Tools (eventuell mit Offset)
	double x=Robot_Koord.X+((d3+Offset)*c1*s234+c1*(L3*c23+L2*c2));
	double y=Robot_Koord.Y+((d3+Offset)*s1*s234+s1*(L3*c23+L2*c2));
	double z=(d3+Offset)*c234-L3*s23-L2*s2+d1+d2;
	// wenn Fuss ausgefahren ist
	if (FootLeg_High>10) z=z+FootLeg_High-11;
	// Koordinaten des Tools als Vektor zurueckgeben
	Tool.setValue(x,y,z);
	return Tool;
}


// Funktion zur Initialisierung der homogenen Point-Matrix
// Eingabe:
//		Punkt der durch Roboterarme erreicht werden soll
// Ausgabe:
//		homogene Punkt-Matrix 
void Set_Point_Matrix(Point_3D Point,double *Point_Matrix) {
	// 1.Zeile der Matrix
	Point_Matrix[0]=-1;
	Point_Matrix[1]=0;
	Point_Matrix[2]=0;
	Point_Matrix[3]=Point.X-Robot_Koord.X;
	// 2.Zeile der Matrix
	Point_Matrix[4]=0;
	Point_Matrix[5]=1;
	Point_Matrix[6]=0;
	Point_Matrix[7]=Point.Y-Robot_Koord.Y;
	// 3.Zeile der Matrix
	Point_Matrix[8]=0;
	Point_Matrix[9]=0;
	Point_Matrix[10]=-1;
	Point_Matrix[11]=Point.Z;
	// 4.Zeile der Matrix
	Point_Matrix[12]=0;
	Point_Matrix[13]=0;
	Point_Matrix[14]=0;
	Point_Matrix[15]=1;
}

// Funktion zum berechnen des Tool-Point-IKP
//  berechnete Winkelwerte werden falls loesbar in dem globalen Feld New_Theta gespeichert
// Eingabe
//		Punkt der durch Roboterarme erreicht werden soll
// Rueckgabe:
//		TRUE IKP loesbar
//		FALSE IKP nicht loesbar
bool Calculate_Tool_Point_IKP(double *Point_Matrix) {
	bool IKP_Berechnet=FALSE;
	bool IKP_OK=TRUE;
	double Theta_IKP[5];
	// IKP_Matrix-Werte auslesen
	// 1.Zeile der Matrix
	double q11=Point_Matrix[0];
	double q12=Point_Matrix[1];
	double q13=Point_Matrix[2];
	double q14=Point_Matrix[3];
	// 2.Zeile der Matrix
	double q21=Point_Matrix[4];
	double q22=Point_Matrix[5];
	double q23=Point_Matrix[6];
	double q24=Point_Matrix[7];
	// 3.Zeile der Matrix
	double q31=Point_Matrix[8];
	double q32=Point_Matrix[9];
	double q33=Point_Matrix[10];
	double q34=Point_Matrix[11];
	// 4.Zeile der Matrix
	double q41=Point_Matrix[12];
	double q42=Point_Matrix[13];
	double q43=Point_Matrix[14];
	double q44=Point_Matrix[15];
	// neuen Theta1 berechnen
	Theta_IKP[0]=atan2(q24,q14);
	double c1=cos(Theta_IKP[0]);
	double s1=sin(Theta_IKP[0]);
	// neuen Theta234 berechnen
	double Theta234_IKP=atan2(c1*q13+s1*q23,q33);
	double c234=cos(Theta234_IKP);
	double s234=sin(Theta234_IKP);
	// neuen Theta5 berechnen
	Theta_IKP[4]=atan2(c1*q21-s1*q11,c1*q22-s1*q12);
	// neuen Theta3 berechnen
	double x=c1*q14+s1*q24-d3*s234;
	double y=d3*c234+d1+d2-q34;
	double c3=(pow(x,2)+pow(y,2)-L32-L22)/(2*L2*L3);
	// negativen Radikanten ausschliessen
	if ((c3!=0)&&(1-pow(c3,2))>0) {
		// negativer Fall fuer s2 wird nicht benoetigt,
                // weil Theta3 immer > 0 sein muss
		double s3=sqrt(1-pow(c3,2));
		Theta_IKP[2]=atan2(s3,c3);
		// neuen Theta2 berechnen
		double a=L3*c3+L2;
		double b=L3*s3;
		if ((x*a+y*b)!=0) {
			IKP_Berechnet=TRUE;
			Theta_IKP[1]=atan2(y*a-x*b,x*a+y*b);
			Theta_IKP[3]=Theta234_IKP-Theta_IKP[2]-Theta_IKP[1];
		}
	}
	if (IKP_Berechnet==TRUE) {
		// berechnete Winkelwerte anpassen wegen Darstellung
		New_Theta[0]=Theta_IKP[0]-M_PI/2+(M_PI/2-Robot_Direction);
		New_Theta[1]=Theta_IKP[1]+M_PI/2;
		New_Theta[2]=Theta_IKP[2]-M_PI/2;
		New_Theta[3]=Theta_IKP[3]-M_PI/2;
		New_Theta[4]=Theta_IKP[4];
		for (int i=0;i<5;i++) {
			// Test ob neue Gelenkwinkelwerte zwischen 0 ... 2*M_PI liegen
			if (New_Theta[i]<0) New_Theta[i]=2*M_PI+New_Theta[i];
			if (New_Theta[i]>2*M_PI) New_Theta[i]=0.0+New_Theta[i]-2*M_PI;
			// Test ob Gelenkwinkel 2,3 und 4 auch im zulaessigen Bereich sind
			if (!Test_Calculate_Arm_Parameters(i,New_Theta[i])) IKP_OK=FALSE;
		}
	}
	else IKP_OK=FALSE;

	return IKP_OK;
}


