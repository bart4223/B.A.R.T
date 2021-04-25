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

// Objects-Header-File

// Aufgaben:
//
// eine Szenerie besteht aus verschiedene geometrischen Objekten
// Objects stellt eine Datenstruktur zur Verfuegung
// um diese Objekte zu verwalten
// Objekt-Typen: Quader, Zylinder, Kegel
//
// Ziel: ein autonomer Roboter kann mit Hilfe dieser
//       Strukur eine Kollisionserkennung realisieren u.v.m


// Header-Files

// Header-Files der Suchoperationen in einem Inventor-Szenegraphen
#include <Inventor/actions/SoAction.h>
#include <Inventor/SoLists.h>
#include <Inventor/SoType.h>
#include <Inventor/actions/SoSearchAction.h>

// Header-Files der Transformationen in einem Inventor-Szenegraphen
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoTranslation.h>

// Header-Files der Primitive in einem Inventor-Szenegraphen
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCylinder.h>

// sonstige Header-Files
#include <Inventor/nodes/SoSeparator.h>

// allgemeine Header-Files
#include <iostream.h>


// Definitionen

// Definition von Objektnamen
#define CUBE 0
#define CYLINDER 1
#define CONE 2


// Globale Strukturen definieren

// 3D-Punkt
struct Point_3D {
	 double X,Y,Z;
};


// Objekt-Klasse definieren
class Objects {
	public:
		// Konstruktor
		Objects(void);

		// Destruktor
		~Objects(void);

		// Objektdatenbank einer Szene komplett loeschen
		void Delete_List(void);

		// Objektdatenbank der Szene erstellen
		// Eingabe: Knoten an dem die Objekte abgelegt sind im Szenegraphen
		void Read_Szene_Objects(SoSeparator*);

		// Zeiger auf Listenanfang setzen
		void Reset_Current_Object(void);

		// Aktuellen Objekt-Typ auslesen
		// Ausgabe: 0 CUBE 
		//		1 CYLINDER 
		//		2 CONE 
		// Rueckgabe:
		// True ein Objekt ist in der Liste
		// False kein Objekt ist in der Liste
		bool Read_Current_Object_Typ(int&);

		// Falls der Zeiger in der Liste auf ein Quader zeigt
		// auslesen der Quader-Paramter
		// Ausgabe: Mittelpunkt
		//		Min-Point
		//		Max-Point
		//		Rotation des Quaders
		void Read_Current_Cube(Point_3D&,Point_3D&,Point_3D&,double&);

		// Falls der Zeiger in der Liste auf ein Zylinder zeigt
		// auslesen der Zylinder-Paramter
		// Ausgabe: Mittelpunkt
		//		Radius
		void Read_Current_Cylinder(Point_3D&,double&);

		// Falls der Zeiger in der Liste auf ein Kegel zeigt
		// auslesen der Kegel-Paramter
		// Ausgabe: Mittelpunkt
		//		Radius
		void Read_Current_Cone(Point_3D&,double&);

		// Auslesen des Objekt-Types des Objektes mit der Nummer i
		// Eingabe: Nummer i
		// Rueckgabe: 0 CUBE 
		//		  1 CYLINDER 
		//		  2 CONE 
		int Read_Object_Typ(int);

		// Objekt-Rotationwinkels des Objektes i auslesen
		// Eingabe: Nummer i
		// Rueckgabe: Rotationswinkel
		double Read_Object_Rotation(int);

		// Min/Max-Punkte des Objektes i auslesen
		// Eingabe: Nummer i
		// Ausgabe: Min-Point
		//		Max-Point
		void Read_Object_MinMax_Point(int,Point_3D&,Point_3D&);

		// Auslesen der Objekt-Radius des Objektes i
		// bei Cube ist der Radius=0
		// Eingabe: Nummer i
		// Rueckgabe: Objekt-Radius
		double Read_Object_Radius(int);

		// Auslesen des Objekt-Mittelpunktes des Objektes i
		// Eingabe: Nummer i
		// Rueckgabe: Objekt-Mittelpunkt
		Point_3D Read_Object_MiddelPoint(int);

		// Auslesen ob gemeinsame Eckpunkte von jeweils zwei Quadern gibt
		// Eingabe: Nummer des 1. Quader-Objektes
		//			Eckpunkt des 1. Quader-Objektes
		// Rueckgabe: Nummer des 2. Quader-Objektes welches das erste im eingegebenen Eckpunkt berueht
		//		       falls Nummer=0 dann kein Quader mit gleichem Eckpunkt
		int Read_Same_Corner(int,Point_3D);

		// Drehung eines Punktes um einen Mittelpunkt
		//  Drehung erfolgt um den Wert Phi
		// Eingabe: Winkel Phi
		//			Mittelpunkt
		// Eingabe/Ausgabe: Original-Punktkoordinaten/Verdrehte Punktkoordinaten 
		void Rotation_Point(double,Point_3D,Point_3D&);


	private:


		// Definitionen

		#define ABWEICHUNG 0.1


		// Variablen

		// Struktur zu Verwaltung von Quader Parametern
		struct Cube_Struct {
			Point_3D Min_Point;
			Point_3D Max_Point;
			double Phi;
			Point_3D Middle_Point;
		};

		// Struktur zu Verwaltung von Zylinder Parametern
		struct Cylinder_Struct {
			Point_3D Middle_Point;
			double Radius;
		};

		// Struktur zu Verwaltung von Kegel Parametern
		struct Cone_Struct {
			Point_3D Middle_Point;
			double BottomRadius;
		};

		// Struktur zur Ermittlung von Eckpunkten eines Objektes, welche sich
		// mit einem anderem ueberlagern
		struct Same_Corner {
			Point_3D Corner_Point;
			int Object_Number;
			Same_Corner* Next_Same_Corner;
		};

		// Struktur der Liste von Objekten
		struct Object_Head {
			// CUBE
			// CYLINDER
			// CONE
			int Object_Typ;
			void* Typ_Struct;
			Same_Corner* SameCorner_Struct;
			Object_Head* Next_Object_Head;
		};

		// Zeiger auf die Wurzel der Liste
		Object_Head *Root_Object_Head;
		// Zeiger auf aktuelles Listenelement 
		Object_Head *Current_Read_Object;


		// Funktionen

		// Funktion zum Aufspuehren von sich ueberlagernden Ecke und
		//  Abspeicherung in die Struktur der Liste von Objekten
		void Find_Same_Corner(void);

		// Ermittlung der realen Eckpunkt-Koordinaten aller Quader
		// Eingabe: Zeiger auf einen Objekt-Kopf
		// Ausgabe: Koordinaten der Eckpunkte
		// Rueckgabe: TRUE es war ein Quader
		//		  FALSE es war kein Quader
		bool Calculate_Corner_Points(Object_Head*,Point_3D[]);

		// Euklidschen Abstand zwischen zwei Punkten (2D) ermitteln
		//  Z-Achse wird nicht betrachtet
		// Eingabe: Punkt A
		//		Punkt B
		// Rueckgabe: Euklidscher Abstand
		double Calculate_Point_Distance(Point_3D,Point_3D);
};
