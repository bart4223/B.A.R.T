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

// Objects-Funktionen-File

// Aufgaben:
//
// Lesen des Szenerie-Graphen und Erstellung
// einer Objektdatenbank in Form einer einfachen Liste
// ueber die in der Szene enthaltenen geometrischen Objekte
//
// Loeschen der kompletten Objektdatenbank 
//
// Auslesen von Daten ueber die in der Liste enthaltenen Objekte

// Header-File

#include "objects.h"


// Public-Funktionen

// Konstruktor
Objects::Objects(void) {
	// Wurzel erzeugen
	Root_Object_Head = new Object_Head;
	// Wurzel dient nur zur Ermittlung ob ein Objekt in Datenbank
	Root_Object_Head->Next_Object_Head=NULL;
	Root_Object_Head->Typ_Struct=NULL;
	Root_Object_Head->SameCorner_Struct=NULL;
	// Zeiger des aktuell zu lesenden Objekt
	Current_Read_Object=NULL;
}

// Destruktor
Objects::~Objects(void) {
	Delete_List();
	delete Root_Object_Head;
}

// Objektdatenbank einer Szene komplett loeschen
void Objects::Delete_List(void) {
	Object_Head *List_Pointer,*Next_List_Pointer;
	List_Pointer=Root_Object_Head->Next_Object_Head;
	// Wurzel dient nur zur Ermittlung ob ein Objekt in Datenbank
	Root_Object_Head->Next_Object_Head=NULL;
	// Zeiger des aktuell zu lesenden Objekt
	Current_Read_Object=NULL;
	while (List_Pointer!=NULL) {
		Next_List_Pointer=List_Pointer->Next_Object_Head;
		// Daten der Objekte loeschen
		if (List_Pointer->Object_Typ==CUBE) {
			Cube_Struct* Cube;
			Cube=(Cube_Struct*)List_Pointer->Typ_Struct;
			delete Cube;
		}
		if (List_Pointer->Object_Typ==CYLINDER) {
			Cylinder_Struct* Cylinder;
			Cylinder=(Cylinder_Struct*)List_Pointer->Typ_Struct;
			delete Cylinder;
		}
		if (List_Pointer->Object_Typ==CONE) {
			Cone_Struct* Cone;
			Cone=(Cone_Struct*)List_Pointer->Typ_Struct;
			delete Cone;
		}
		// Same-Corner-Struktur loeschen
		Same_Corner *Same_Corner_Struct,*Next_Same_Corner_Struct;
		Same_Corner_Struct=List_Pointer->SameCorner_Struct;
		while (Same_Corner_Struct!=NULL) {
			Next_Same_Corner_Struct=Same_Corner_Struct->Next_Same_Corner;
			delete Same_Corner_Struct;
			Same_Corner_Struct=Next_Same_Corner_Struct;
		}
		// Datenkopf loeschen
		delete List_Pointer;
		List_Pointer=Next_List_Pointer;
	}
}

// Einlesen der Szene und Erstellung der Datenbank
// Eingabe: Knoten an dem die Objekte abgelegt sind im Szenegraphen
void Objects::Read_Szene_Objects(SoSeparator* Anfang_Szene) {
	// allgemeine Variablen setzen
	int index=0;
	int i=0;
	float Mx,My,Mz,x,y,r,Phi;
	// Zeiger fuer Objekt-Datenbank
	Object_Head* Current_Object_Head;
	Object_Head* Old_Object_Head;
	// Objekterkennung
	bool ALL_Cubes=FALSE;
	bool ALL_Cylinders=FALSE;
	bool ALL_Cones=FALSE;
	// Pfad setzen
	SoPath* Weg;
	// Pfadliste der Objekte definieren
	SoPathList Translation_Wege;
	SoPathList Rotation_Wege;
	SoPathList Cube_Wege;
	SoPathList Cylinder_Wege;
	SoPathList Cone_Wege;
	// Translation des Objektes 
	SoTranslation *Trans_Node;
	SbVec3f Trans;
	// Rotation des Objektes 
	SoRotation *Rot_Node;
	SbRotation Rot;
	// Objekt-Quader
	SoCube *Cube_Node;
	// Objekt-Cylinder
	SoCylinder *Cylinder_Node;
	// Objekt-Kegel
	SoCone *Cone_Node;
	// zu suchende Knotentypen erzeugen
	SoTranslation *Translation = new SoTranslation;
	SoRotation *Rotation = new SoRotation;
	SoCube *Cube = new SoCube;
	SoCylinder *Cylinder = new SoCylinder;
	SoCone *Cone = new SoCone;

	// Suchfunktion aktivieren
	SoSearchAction mySearchAction;
	// Finden aller Translation-Pfade im Szeneriegraphen
	mySearchAction.setType(Translation->getTypeId());
	mySearchAction.setInterest(SoSearchAction::ALL);
	mySearchAction.apply(Anfang_Szene);
	Translation_Wege=mySearchAction.getPaths();
	// Finden aller Rotation-Pfade im Szeneriegraphen
	mySearchAction.setType(Rotation->getTypeId());
	mySearchAction.setInterest(SoSearchAction::ALL);
	mySearchAction.apply(Anfang_Szene);
	Rotation_Wege=mySearchAction.getPaths();
	// Finden aller Cube-Pfade im Szeneriegraphen
	mySearchAction.setType(Cube->getTypeId());
	mySearchAction.setInterest(SoSearchAction::ALL);
	mySearchAction.apply(Anfang_Szene);
	Cube_Wege=mySearchAction.getPaths();
	// Finden aller Cylinder-Pfade im Szeneriegraphen
	mySearchAction.setType(Cylinder->getTypeId());
	mySearchAction.setInterest(SoSearchAction::ALL);
	mySearchAction.apply(Anfang_Szene);
	Cylinder_Wege=mySearchAction.getPaths();
	// Finden aller Cone-Pfade im Szeneriegraphen
	mySearchAction.setType(Cone->getTypeId());
	mySearchAction.setInterest(SoSearchAction::ALL);
	mySearchAction.apply(Anfang_Szene);
	Cone_Wege=mySearchAction.getPaths();
	
	// Datenbank erstellen
	// Zeiger auf 0.tes Objekt setzen
	Old_Object_Head=Root_Object_Head;
	while (Translation_Wege[i]!=NULL)
	{
		Current_Object_Head = new Object_Head;
		// Struktur der gleichen Ecke mit anderen Objekten auf NULL setzen
		Current_Object_Head->SameCorner_Struct=NULL;
		// Translation des Objektes auslesen
		Weg=Translation_Wege[i];
		Trans_Node=(SoTranslation*)Weg->getTail();
		Trans=Trans_Node->translation.getValue();
		Trans.getValue(Mx,My,Mz);
		// Rotation des Objektes auslesen
		Weg=Rotation_Wege[i];
		Rot_Node=(SoRotation*)Weg->getTail();
		Rot=Rot_Node->rotation.getValue();
		Rot.getValue(Trans,Phi);
		// Form des Objektes ermitteln
		if (ALL_Cubes==FALSE) {
			if (Cube_Wege[i]!=NULL) {
				Weg=Cube_Wege[i];
				Cube_Node=(SoCube*)Weg->getTail();
				// Cube Parameter auslesen
				x=Cube_Node->width.getValue();
				y=Cube_Node->height.getValue();
				// Objekt in Datenbank einbauen
				// Objekttyp Cube
				Current_Object_Head->Object_Typ=CUBE;
				// neue Wuerfel-Struktur reservieren
				Cube_Struct* Typ_Cube = new Cube_Struct;
				// Zeiger auf Wuerfel-Struktur setzen
				Current_Object_Head->Typ_Struct=Typ_Cube;
				// Datenbank des Wuerfels anlegen
				// P1 des Wuerfels speichern
				Typ_Cube->Min_Point.X=(double)(Mx-(x/2));
				Typ_Cube->Min_Point.Y=(double)(My-(y/2));
				// P3 des Wuerfels speichern
				Typ_Cube->Max_Point.X=(double)(Mx+(x/2));
				Typ_Cube->Max_Point.Y=(double)(My+(y/2));
				// Drehung des Wuerfels speichern
				Typ_Cube->Phi=(double)Phi;
				// Mittelpunkt des Wuerfels speichern
				Typ_Cube->Middle_Point.X=(double)Mx;
				Typ_Cube->Middle_Point.Y=(double)My;
			}
			else {
			ALL_Cubes=TRUE;
			index=i;
			}
		}
		if ((ALL_Cubes==TRUE)&&(ALL_Cylinders==FALSE)) {
			if (Cylinder_Wege[i-index]!=NULL) {
				Weg=Cylinder_Wege[i-index];
				Cylinder_Node=(SoCylinder*)Weg->getTail();
				// Zylinder Parameter auslesen
				r=Cylinder_Node->radius.getValue();
				// Objekt in Datenbank einbauen
				// Objekttyp Zylinder
				Current_Object_Head->Object_Typ=CYLINDER;
				// neue Zylinder-Struktur reservieren
				Cylinder_Struct* Typ_Cylinder = new Cylinder_Struct;
				// Zeiger auf Zylinder-Struktur setzen
				Current_Object_Head->Typ_Struct=Typ_Cylinder;
				// Datenbank des Zylinders anlegen
				// Mittelpunkt des Wuerfels speichern
				Typ_Cylinder->Middle_Point.X=(double)Mx;
				Typ_Cylinder->Middle_Point.Y=(double)My;
				// Radius des Zylinders speichern
				Typ_Cylinder->Radius=(double)r;
			}
			else {
			ALL_Cylinders=TRUE;
			index=i;
			}
		}
		if ((ALL_Cubes==TRUE)&&(ALL_Cylinders==TRUE)&&(ALL_Cones==FALSE)) {
			if (Cone_Wege[i-index]!=NULL) {
				Weg=Cone_Wege[i-index];
				Cone_Node=(SoCone*)Weg->getTail();
				// Kegel Parameter auslesen
				r=Cone_Node->bottomRadius.getValue();
				// Objekt in Datenbank einbauen
				// Objekttyp Kegel
				Current_Object_Head->Object_Typ=CONE;
				// neue Zylinder-Struktur reservieren
				Cone_Struct* Typ_Cone = new Cone_Struct;
				// Zeiger auf Zylinder-Struktur setzen
				Current_Object_Head->Typ_Struct=Typ_Cone;
				// Datenbank des Zylinders anlegen
				// Mittelpunkt des Wuerfels speichern
				Typ_Cone->Middle_Point.X=(double)Mx;
				Typ_Cone->Middle_Point.Y=(double)My;
				// Radius des Zylinders speichern
				Typ_Cone->BottomRadius=(double)r;
			}
			else {
			ALL_Cones=TRUE;
			index=i;
			}
		}
		// Zeiger auf gerade erstelltes Objekt setzen
		Old_Object_Head->Next_Object_Head=Current_Object_Head;
		Old_Object_Head=Current_Object_Head;
		// naechstes Objekt
		i++;
	}
	// Ende der Objekt-Datenbank
	Old_Object_Head->Next_Object_Head=NULL;
	// Datenbank auf erstes Element setzen
	Reset_Current_Object();
	// Objekte mit gleichen Eckpunkten aufspuehren und in Datenbank ablegen
	Find_Same_Corner();
}

// Aktuellen Objekt-Typ auslesen
// Ausgabe: 0 CUBE 
//		1 CYLINDER 
//		2 CONE 
// Rueckgabe:
// True ein Objekt ist in der Liste
// False kein Objekt ist in der Liste
bool Objects::Read_Current_Object_Typ(int& Objekt_Typ) {
	bool Test=FALSE;
	if (Current_Read_Object!=NULL) {
		Test=TRUE;
		Objekt_Typ=Current_Read_Object->Object_Typ;
	}
	return Test;
}

// Zeiger auf Listenanfang setzen
void Objects::Reset_Current_Object(void) {
	Current_Read_Object=Root_Object_Head->Next_Object_Head;
}

// Falls der Zeiger in der Liste auf ein Quader zeigt
// auslesen der Quader-Parameter
// Ausgabe: Mittelpunkt
//		Min-Point
//		Max-Point
//		Rotation des Quaders
void Objects::Read_Current_Cube(Point_3D& Min,Point_3D& Max,Point_3D& MiddlePoint,double& Phi) {
	Cube_Struct* Cube;
	Cube=(Cube_Struct*)Current_Read_Object->Typ_Struct;
	// P1 und P3 des Wuerfels auslesen
	Min=Cube->Min_Point;
	Max=Cube->Max_Point;
	// Drehung des Wuerfels auslesen
	Phi=Cube->Phi;
	// Mittelpunkt des Wuerfels auslesen
	MiddlePoint=Cube->Middle_Point;
	// auf aktuelles Objekt setzen
	Current_Read_Object=Current_Read_Object->Next_Object_Head;
}

// Falls der Zeiger in der Liste auf ein Zylinder zeigt
// auslesen der Zylinder-Parameter
// Ausgabe: Mittelpunkt
//		Radius
void Objects::Read_Current_Cylinder(Point_3D& MiddlePoint,double& Radius) {
	Cylinder_Struct* Cylinder;
	Cylinder=(Cylinder_Struct*)Current_Read_Object->Typ_Struct;
	// Radius des Zylinders auslesen
	Radius=Cylinder->Radius;
	// Mittelpunkt des Zylinders auslesen
	MiddlePoint=Cylinder->Middle_Point;
	// auf aktuelles Objekt setzen
	Current_Read_Object=Current_Read_Object->Next_Object_Head;
}


// Falls der Zeiger in der Liste auf ein Kegel zeigt
// auslesen der Kegel-Parameter
// Ausgabe: Mittelpunkt
//		Radius
void Objects::Read_Current_Cone(Point_3D& MiddlePoint,double& Radius) {
	Cone_Struct* Cone;
	Cone=(Cone_Struct*)Current_Read_Object->Typ_Struct;
	// Radius des Zylinders auslesen
	Radius=Cone->BottomRadius;
	// Mittelpunkt des Zylinders auslesen
	MiddlePoint=Cone->Middle_Point;
	// auf aktuelles Objekt setzen
	Current_Read_Object=Current_Read_Object->Next_Object_Head;
}

// Auslesen des Objekt-Types des Objektes der Nummer i
// Eingabe: Nummer i
// Rueckgabe: 0 CUBE 
//		  1 CYLINDER 
//		  2 CONE 
int Objects::Read_Object_Typ(int Object_Number) {
	int i=1;
	Object_Head *List_Pointer,*Next_List_Pointer;
	List_Pointer=Root_Object_Head->Next_Object_Head;
	while (i!=Object_Number) {
		i++;
		Next_List_Pointer=List_Pointer->Next_Object_Head;
		List_Pointer=Next_List_Pointer;
	}
	Current_Read_Object=List_Pointer;
	return List_Pointer->Object_Typ;
}

// Objekt-Rotationwinkels des Objektes i auslesen
// Eingabe: Nummer i
// Rueckgabe: Rotationswinkel
double Objects::Read_Object_Rotation(int Object_Number) {
	int Typ;
	double Phi;
	Typ=Read_Object_Typ(Object_Number);
	if (Typ==CUBE) {
		Cube_Struct* Cube;
		Cube=(Cube_Struct*)Current_Read_Object->Typ_Struct;
		Phi=Cube->Phi;
	}
	else Phi=0.0;
	return Phi;
}

// Min/Max-Punkte des Objektes i auslesen
// Eingabe: Nummer i
// Ausgabe: Min-Point
//		Max-Point
void Objects::Read_Object_MinMax_Point(int Object_Number,Point_3D& Min,Point_3D& Max) {
	int Typ;
	Typ=Read_Object_Typ(Object_Number);
	if (Typ==CUBE) {
		Cube_Struct* Cube;
		Cube=(Cube_Struct*)Current_Read_Object->Typ_Struct;
		// P1 und P3 des Wuerfels auslesen
		Min=Cube->Min_Point;
		Max=Cube->Max_Point;
	}
	if (Typ==CYLINDER) {
		Cylinder_Struct* Cylinder;
		Cylinder=(Cylinder_Struct*)Current_Read_Object->Typ_Struct;
		// Mittelpunkt des Zylinders auslesen
		Min=Cylinder->Middle_Point;
		Max=Min;
	}
	if (Typ==CONE) {
		Cone_Struct* Cone;
		Cone=(Cone_Struct*)Current_Read_Object->Typ_Struct;
		// Mittelpunkt des Kegels auslesen
		Min=Cone->Middle_Point;
		Max=Min;
	}
}

// Auslesen der Objekt-Radius des Objektes i
// bei Cube ist der Radius=0
// Eingabe: Nummer i
// Rueckgabe: Objekt-Radius
double Objects::Read_Object_Radius(int Object_Number) {
	int Typ;
	double Radius;
	Typ=Read_Object_Typ(Object_Number);
	if (Typ==CUBE) Radius=0.0;
	if (Typ==CYLINDER) {
		Cylinder_Struct* Cylinder;
		Cylinder=(Cylinder_Struct*)Current_Read_Object->Typ_Struct;
		// Radius des Zylinders auslesen
		Radius=Cylinder->Radius;
	}
	if (Typ==CONE) {
		Cone_Struct* Cone;
		Cone=(Cone_Struct*)Current_Read_Object->Typ_Struct;
		// Boden-Radius des Kegels auslesen
		Radius=Cone->BottomRadius;
	}
	return Radius;
}

// Auslesen des Objekt-Mittelpunktes des Objektes i
// Eingabe: Nummer i
// Rueckgabe: Objekt-Mittelpunkt
Point_3D Objects::Read_Object_MiddelPoint(int Object_Number) {
	int Typ;
	Point_3D Object_MiddelPoint;
	Typ=Read_Object_Typ(Object_Number);
	// Auslesen des Objekt-Mittelpunktes
	if (Typ==CUBE) {
		Cube_Struct* Cube;
		Cube=(Cube_Struct*)Current_Read_Object->Typ_Struct;
		Object_MiddelPoint=Cube->Middle_Point;
	}
	if (Typ==CYLINDER) {
		Cylinder_Struct* Cylinder;
		Cylinder=(Cylinder_Struct*)Current_Read_Object->Typ_Struct;
		// Mittelpunkt des Zylinders auslesen
		Object_MiddelPoint=Cylinder->Middle_Point;
	}
	if (Typ==CONE) {
		Cone_Struct* Cone;
		Cone=(Cone_Struct*)Current_Read_Object->Typ_Struct;
		// Mittelpunkt des Kegels auslesen
		Object_MiddelPoint=Cone->Middle_Point;
	}
	return Object_MiddelPoint;
}

// Auslesen ob gemeinsame Eckpunkte von jeweils zwei Quadern gibt
// Eingabe: Nummer des 1. Quader-Objektes
//			Eckpunkt des 1. Quader-Objektes
// Rueckgabe: Nummer des 2. Quader-Objektes welches das erste im eingegebenen Eckpunkt berueht
//		       falls Nummer=0 dann kein Quader mit gleichem Eckpunkt
int Objects::Read_Same_Corner(int Object_Number,Point_3D Corner) {
	int Typ,Touch_Object_Number=0;
	Same_Corner *Same_Corner_Struct;
	Typ=Read_Object_Typ(Object_Number);
	Same_Corner_Struct=Current_Read_Object->SameCorner_Struct;
	while (Same_Corner_Struct!=NULL) {
		Point_3D Test_Corner;
		Test_Corner=Same_Corner_Struct->Corner_Point;
		if ((Test_Corner.X==Corner.X)&&(Test_Corner.Y==Corner.Y)) {
			Touch_Object_Number=Same_Corner_Struct->Object_Number;
			Same_Corner_Struct=NULL;
		}
		else Same_Corner_Struct=Same_Corner_Struct->Next_Same_Corner;
	}
	return Touch_Object_Number;
}

// Drehung eines Punktes um einen Mittelpunkt
//  Drehung erfolgt um den Wert Phi
// Eingabe: Winkel Phi
//			Mittelpunkt
// Eingabe/Ausgabe: Original-Punktkoordinaten/Verdrehte Punktkoordinaten 
void Objects::Rotation_Point(double Winkel,Point_3D Mittelpunkt, Point_3D& Punkt) {
	Point_3D Vektor;
	Vektor=Punkt;
	Vektor.X=(Punkt.X-Mittelpunkt.X)*cos(Winkel)-(Punkt.Y-Mittelpunkt.Y)*sin(Winkel)+Mittelpunkt.X;
	Vektor.Y=(Punkt.X-Mittelpunkt.X)*sin(Winkel)+(Punkt.Y-Mittelpunkt.Y)*cos(Winkel)+Mittelpunkt.Y;
	Punkt=Vektor;
}


// Private Funktionen

// Funktion zum Aufspuehren von sich ueberlagernden Ecke und
//  Abspeicherung in die Struktur der Liste von Objekten
void Objects::Find_Same_Corner(void) {
	// aktuell zu vergleichendes Objekt
	Object_Head *Current_Object;
	// zu vergleichendes Objekt
	Object_Head *Test_Object;
	// Zeiger auf erstes Objekt setzen
	Current_Object=Root_Object_Head->Next_Object_Head;
	// aktuelle Objektnummer
	int Current_Object_Number=1;
	// zu vergleichende Objektnummer
	int Test_Object_Number;
	// aktuelle Eck-Punkte
	Point_3D Current_Points[4];
	// Test Eck-Punkte
	Point_3D Test_Points[4];
	// Zeiger fuer gleiche Ecken-Struktur
	Same_Corner *Corner_Struct,*Old_Corner_Struct;
	// erste gemeinsame Ecke TRUE=JA/FALSE=NEIN
	bool First_SameCorner;
	// Anzahl der paarweisen gleichen Ecken
	int Count_of_Same_Corner=0;
	while ((Current_Object!=NULL)&&(Current_Object->Object_Typ==CUBE)) {
		// Eckpunktes des aktuellen Objektes bestimmen
		Calculate_Corner_Points(Current_Object,Current_Points);

		//cout << "Point2X=" << Current_Points[1].X << endl;
		//cout << "Point2Y=" << Current_Points[1].Y << endl;

		// Zeiger auf den Listenanfang setzen
		Test_Object=Root_Object_Head->Next_Object_Head;
		// zu vergleichende Objektnummer setzen
		Test_Object_Number=1;	
		// erste gemeinsame Ecke
		First_SameCorner=TRUE;
		while ((Test_Object!=NULL)&&(Test_Object->Object_Typ==CUBE)) {
			// Quader nicht mit sich selber testen
			if (Test_Object_Number!=Current_Object_Number) {
				// Eckpunktes des zu testenden Objektes bestimmen
				Calculate_Corner_Points(Test_Object,Test_Points);
				// Test der Ecken durchfuehren
				for (int u=0;u<4;u++) {
					for (int v=0;v<4;v++) {
						if (((Test_Points[v].X-ABWEICHUNG<=Current_Points[u].X)&&(Current_Points[u].X<=Test_Points[v].X+ABWEICHUNG))&&((Test_Points[v].Y-ABWEICHUNG<=Current_Points[u].Y)&&(Current_Points[u].Y<=Test_Points[v].Y+ABWEICHUNG))) {
							// Anzahl der paarweisen gleichen Ecken um eins erhoehen
							Count_of_Same_Corner++;

							//cout << "Punkt:" << Current_Object_Number << "," << u << "=" << Test_Object_Number << "," << v << endl << flush;
							
							Corner_Struct = new Same_Corner;
							// Struktur in Objektdatenbank einbinden
							if (First_SameCorner==TRUE) {
								First_SameCorner=FALSE;
								Current_Object->SameCorner_Struct=Corner_Struct;
							}
							else Old_Corner_Struct->Next_Same_Corner=Corner_Struct;
							// gemeinsame Eckpunkte
							Corner_Struct->Corner_Point=Current_Points[u];
							// mit Objekt v
							Corner_Struct->Object_Number=Test_Object_Number;
							// naechste gemeinsame Ecke
							Corner_Struct->Next_Same_Corner=NULL;
							Old_Corner_Struct=Corner_Struct;
						}
					}
				}
			}
			// naechstes zu testendes Objekt
			Test_Object=Test_Object->Next_Object_Head;
			Test_Object_Number++;
		}
		// naechstes Objekt
		Current_Object=Current_Object->Next_Object_Head;
		Current_Object_Number++;
	}
	cout << "Anzahl der paarweise gemeinsamen Ecken: " << Count_of_Same_Corner/2 << endl;
}

// Ermittlung der realen Eckpunkt-Koordinaten aller Quader
// Eingabe: Zeiger auf einen Objekt-Kopf
// Ausgabe: Koordinaten der Eckpunkte
// Rueckgabe: TRUE es war ein Quader
//		  FALSE es war kein Quader
bool Objects::Calculate_Corner_Points(Object_Head* Object,Point_3D Points[]) {
	Cube_Struct* Cube;
	Point_3D Middle_Point;
	double Rotation;
	bool Quader=FALSE;
	// Test ob der Zeiger Object wirklich auf eine Quader-Struktur zeigt
	if (Object->Object_Typ==CUBE) {
		Quader=TRUE;
		Cube=(Cube_Struct*)Object->Typ_Struct;
		// Point 1
		Points[0]=Cube->Min_Point;
		// Point 3
		Points[2]=Cube->Max_Point;
		// Point 2
		Points[1].X=Points[0].X;
		Points[1].Y=Points[2].Y;
		// Point 4
		Points[3].X=Points[2].X;
		Points[3].Y=Points[0].Y;
		Rotation=Cube->Phi;
		Middle_Point=Cube->Middle_Point;
		// genaue Lage der Eckpunkte des Quaders ermitteln
		if (Rotation!=0.0) for (int i=0;i<4;i++) Rotation_Point(Rotation,Middle_Point,Points[i]);
	}
	return Quader;
}

// Euklidschen Abstand zwischen zwei Punkten (2D) ermitteln
//  Z-Achse wird nicht betrachtet
// Eingabe: Punkt A
//		Punkt B
// Rueckgabe: Euklidscher Abstand
double Objects::Calculate_Point_Distance(Point_3D Punkt_A,Point_3D Punkt_B) {
	double Abstand=sqrt(pow(Punkt_A.X-Punkt_B.X,2)+pow(Punkt_A.Y-Punkt_B.Y,2));
	return Abstand;
}