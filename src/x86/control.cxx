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

// Steuerung des Roboters

// Aufgaben:
// Realisierung der Tastaturabfrage
// Gelenkwinkel des Roboters setzen
// neue Armstellung berechnen
// Sichtbarmachung der neuen Armstellung
// Berechnung der neuen Roboterkoordinaten
// Sichtbarmachung der neuen Roboterposition
// Rotationsfuss aus/einfahren
// Selektionsbehandlung von Objekten im Haupt-Window
// Formveraenderung des Target-Draggers im nicht aktiven Zustand 
// Formveraenderung des Target-Draggers im aktiven Zustand 
// Auslesen und abspeichern des "gedraggerten" Zieles
// Selektionsbehandlung von Terminal-Objekten
// Runterfahren eines Screens im Terminal-Windows
// Hochfahren eines Screens im Terminal-Windows
// Betaetigen einer Taste im Terminal-Window
// Ausgabe am News-Screen im Terminal-Windows
// Laden eines Labyrinthes

// Header-Files

#include "main.h"
#include "objects.h"
#include "control.h"
#include "ability.h"


// Globale Variablen

// Koordinaten des Roboters
Point_3D Robot_Koord;
// aktuelle Richtung des Roboters
double Robot_Direction;
// Grundstellung der Gelenke 
double Theta_Start[5];
// Winkelwerte der Gelenke
double Theta[5];
// neue Winkelwerte der Gelenke
double New_Theta[5];
// Rotationsrichtung der Gelenke
int Joint_Rotation_Direction[5];
// Anzahl der Schritte um vom IST-Gelenkwinkel zum SOLL-Gelenkwinkel zu gelangen
int Joint_Steps[5];
// Gelenknamen den Rotationsknoten zuweisen
char* Gelenk_Name[5];
// Timervariable fuer die automatische Bewegung des Roboterfusses
SoTimerSensor *Robot_Foot_InOut;
// Interruptvariable zur Realisierung der automatischen Bewegung der Robotergelenke von einem
//  IST-Wert zu einem SOLL-Wert
SoTimerSensor *To_New_Joints;
// Variable um die Arbeit des Tools nach erreichen eines vorgegebenen Punktes festzulegen
//		TOOL_NOT_WORK				Tool macht nichts mehr
//		TOOL_TAKE_TARGETOBJEKT		Tool nimmt das Zielobjekt auf und geht in die Grundstellung
//		TOOL_TAKEOFF_TargetOBJEKT	Tool legt das Zielobjekt vor sich ab
int What_Tool_Doit;
// Objekt am Tool ?
bool Tool_Busy;
// Erkennung ob Roboter in Drehposition
bool Robot_Rot;
// Hoehe des Roboter-Fusses
double FootLeg_High;
// Anzeige der automatischen Bewegung des Roboter-Fusses 
bool Robot_Foot_Move;
// Anzeige der automatischen Bewegung der Roboterarme
bool Robot_Arm_Move;
// Variablen zum hochfahren von Screens
SoTimerSensor *ScreenMoveUp1;
SoTranslation *Screen_Trans_Up1;
SoSeparator *Screen_Sep_Up1;
bool MovingUpScreen1;
float Screen_Finish_Point_Up1;
SoTimerSensor *ScreenMoveUp2;
SoTranslation *Screen_Trans_Up2;
SoSeparator *Screen_Sep_Up2;
bool MovingUpScreen2;
float Screen_Finish_Point_Up2;
// Variablen zum runterfahren von Screens
SoTimerSensor *ScreenMoveDown1;
SoTranslation *Screen_Trans_Down1;
SoSeparator *Screen_Sep_Down1;
bool MovingDownScreen1;
float Screen_Finish_Point_Down1;
SoTimerSensor *ScreenMoveDown2;
SoTranslation *Screen_Trans_Down2;
SoSeparator *Screen_Sep_Down2;
bool MovingDownScreen2;
float Screen_Finish_Point_Down2;
// Variable zum betaetigen einer Taste
SoTimerSensor *PressButton;
SoBaseColor *Button_Color;
SoTranslation *Button_Trans;
bool KeyPressed;
// Variablen zur Erkennung der Menuesituation
// Menueschichten:
//		NOLAYER 0
//		MANUAL 1
//		MANUAL_JOINT 2
//		AUTO 3
//		AUTO_ROBOT 4
//		AUTO_JOINT 5
//		LOAD 6
int MenuLayer;
// Variable zur Erkennung der gewaehlten Aktion: !!! IMMER NUR 1 BIT GESETZT !!!
//		Bit 0	MANUAL-JOINT
//		Bit 1	MANUAL-ROBOT
//		Bit 2	AUTO-JOINT
//		Bit 3	AUTO-ROBOT
int Aktion;
// Variable zur weiteren Abstufung der Aktion
int Aktion_Number;
// Variable unterscheidet 1=Manuell und 2=Automatisch
int Bewegungsart;
// Variable fuer den Labyrinth-Filenamen
char Filename[Max_Filename_Length];
int Filename_Length;
// Variable zur Erkennung ob auf die Roboter-Kamera umgeschalten ist
bool Robot_View;
// Variablen zur Speicherung der Kamera-Position
SbVec3f Old_View_Camera_Pos;
SbRotation Old_View_Camera_Ori;


// Funktionen

// Funktion zur Realisierung der Tastaturabfrage
void Key_Pressed(void *,SoEventCallback *evCallback) {
	const SoEvent *event=evCallback->getEvent();
	int Objekt_Nummer,Objekt_Bereich;
	//
	// Keine Tastaturabfrage bei der automatischen Bewegung
	// des Roboterfusses, der Roboterarme oder des Roboters
	//
	// Ausser Stoppen aller Automatischen-Such-Timer !!!
	if (SO_KEY_PRESS_EVENT(event,SPACE)) Stop_All_Search_Timer();
	// und Kamera-Position umschalten !!!
	if (SO_KEY_PRESS_EVENT(event,V)) Change_Camera_View();
	if (SO_KEY_PRESS_EVENT(event,E)) Program_Exit();
	// und Beendigung des Programmes !!!
	//
	if ((Robot_Foot_Move==FALSE)&&(Robot_Auto_Move==FALSE)&&(Robot_Arm_Move==FALSE)) {
		// Manuelle Bewegung der Gelenke realisieren
		if ((Aktion==1)&&((SO_KEY_PRESS_EVENT(event,LEFT_ARROW))||(SO_KEY_PRESS_EVENT(event,RIGHT_ARROW)))) {
			// Index erstellen
			int index=Aktion_Number-1;
			// Richtungsabfrage des entsprechenden Gelenkes
			if (SO_KEY_PRESS_EVENT(event,LEFT_ARROW)) {
				double Winkelwert=Theta[index]-Step_Joint;
				// Gelenkwinkel zwischen 0 ... 2*PI
				if (Winkelwert<0) Winkelwert=2*M_PI+Winkelwert;
				if (Test_Calculate_Arm_Parameters(index,Winkelwert)) {
					// neuen Gelenkwert zuweisen
					Theta[index]=Winkelwert;
					Set_Joints();
				}
			}
			if (SO_KEY_PRESS_EVENT(event,RIGHT_ARROW)) {
				double Winkelwert=Theta[index]+Step_Joint;
				// Gelenkwinkel zwischen 0 ... 2*PI
				if (Winkelwert>2*M_PI) Winkelwert=0.0+(Winkelwert-2*M_PI);
				if (Test_Calculate_Arm_Parameters(index,Winkelwert)) {
					// neuen Gelenkwert zuweisen
					Theta[index]=Winkelwert;
					Set_Joints();
				}
			}
		}
		// Manuelle Bewegung des Roboters realisieren
		if ((Aktion==2)&&(SO_KEY_PRESS_EVENT(event,LEFT_ARROW))) {
			if (Robot_Rot==TRUE) {
				Robot_Direction=Robot_Direction+Step_Robot_Rot;
				// Robot_Direction zwischen 0 ... 2*PI
				if (Robot_Direction>2*M_PI) Robot_Direction=0.0+(Robot_Direction-2*M_PI);
			}
			Calculate_Motion_Robot(0,Objekt_Nummer,Objekt_Bereich);
		}
		if ((Aktion==2)&&(SO_KEY_PRESS_EVENT(event,RIGHT_ARROW))) {
			if (Robot_Rot==TRUE) {
				Robot_Direction=Robot_Direction-Step_Robot_Rot;
				// Robot_Direction zwischen 0 ... 2*PI
				if (Robot_Direction<0.0) Robot_Direction=2*M_PI+Robot_Direction;
			}
			Calculate_Motion_Robot(ROTATION,Objekt_Nummer,Objekt_Bereich);
		}
		if ((Aktion==2)&&(SO_KEY_PRESS_EVENT(event,UP_ARROW))) {
			Calculate_Motion_Robot(TRANSLATION_FORWARD,Objekt_Nummer,Objekt_Bereich);
		}
		if ((Aktion==2)&&(SO_KEY_PRESS_EVENT(event,DOWN_ARROW))) {
			Calculate_Motion_Robot(TRANSLATION_BACK,Objekt_Nummer,Objekt_Bereich);
		}
		// Abfragen zur Steuerung des Roboters mittels der Tastatur
		// Bewegungsart:
		//		NO_MOVEMENT = keine Bewegung
		//		MANUAL_MOVEMENT = Manuelle Bewegung
		//		AUTO_MOVEMENT = automatische Bewegung
		if (SO_KEY_PRESS_EVENT(event,M)) {
			Bewegungsart=MANUAL_MOVEMENT;
			Aktion=0;
			Aktion_Number=0;
		}
		if (SO_KEY_PRESS_EVENT(event,A)) {
			Bewegungsart=AUTO_MOVEMENT;
			Aktion=0;
			Aktion_Number=0;
		}
		if (SO_KEY_PRESS_EVENT(event,L)) {
			Bewegungsart=NO_MOVEMENT;
			Aktion=0;
			Aktion_Number=0;
			char File[Max_Filename_Length];
			cout << "Geben Sie bitte das zu ladende Labyrinth-File ein:";
			cin >> File;
			cout << "File=" << File << endl << flush;
			Filename[0]='\0';
			strcat(Filename,File);
			if (Load_Labyrinth()==TRUE) cout << endl << "Labyrinth geladen !!!" << endl;
			else cout << endl << "Labyrinth nicht geladen !!!" << endl;
		}
		if (SO_KEY_PRESS_EVENT(event,X)) Reset_Robot_Joints();
		if ((Bewegungsart==MANUAL_MOVEMENT)&&(SO_KEY_PRESS_EVENT(event,R))) Aktion=2;
		if ((Bewegungsart==MANUAL_MOVEMENT)&&(SO_KEY_PRESS_EVENT(event,J))) Aktion=1;
		if ((Bewegungsart==AUTO_MOVEMENT)&&(SO_KEY_PRESS_EVENT(event,R))) Aktion=8;
		if ((Bewegungsart==AUTO_MOVEMENT)&&(SO_KEY_PRESS_EVENT(event,J))) Aktion=4;
		if ((Aktion==1)&&(SO_KEY_PRESS_EVENT(event,NUMBER_1))) Aktion_Number=1;
		if ((Aktion==1)&&(SO_KEY_PRESS_EVENT(event,NUMBER_2))) Aktion_Number=2;
		if ((Aktion==1)&&(SO_KEY_PRESS_EVENT(event,NUMBER_3))) Aktion_Number=3;
		if ((Aktion==1)&&(SO_KEY_PRESS_EVENT(event,NUMBER_4))) Aktion_Number=4;
		if ((Aktion==1)&&(SO_KEY_PRESS_EVENT(event,NUMBER_5))) Aktion_Number=5;
		if ((Aktion==8)&&(SO_KEY_PRESS_EVENT(event,NUMBER_1))) Init_Labyrinth_Search(SIMPLE_METHODE);
		if ((Aktion==8)&&(SO_KEY_PRESS_EVENT(event,NUMBER_2))) Init_Labyrinth_Search(MODULO_METHODE);
		if ((Aktion==8)&&(SO_KEY_PRESS_EVENT(event,NUMBER_3))) Init_Labyrinth_Search(PLEDGE_METHODE);
		if ((Aktion==8)&&(SO_KEY_PRESS_EVENT(event,NUMBER_4))) Init_Target_Search(BUG_METHODE);
		if ((Aktion==8)&&(SO_KEY_PRESS_EVENT(event,NUMBER_5))) Init_Target_Search(CORNER_METHODE);
		if ((Aktion==4)&&(SO_KEY_PRESS_EVENT(event,NUMBER_1))) {
			if (Tool_Busy==FALSE) {
				// Tool soll nach erreichen des Zielobjektes dieses aufheben
				What_Tool_Doit=TOOL_TAKE_TARGETOBJEKT;
				if (!Robot_TakeONOFF_TargetObjekt()) cout << "Zielobjekt kann nicht aufgenommen werden !!!" << endl;
			}
			else cout << "Tool hat schon ein Objekt aufgenommen werden !!!" << endl;
		}
		if ((Aktion==4)&&(SO_KEY_PRESS_EVENT(event,NUMBER_2))) {
			if (Tool_Busy==TRUE) {
				// Tool soll nach erreichen des Zielobjektes dieses ablegen
				What_Tool_Doit=TOOL_TAKEOFF_TARGETOBJEKT;
				if (!Robot_TakeONOFF_TargetObjekt()) cout << "Zielobjekt kann nicht abgelegt werden !!!" << endl;
			}
			else cout << "Tool hat kein Objekt aufgenommen !!!" << endl;
		}
	}
}

// Funktion zum setzen der Gelenkwinkel des Roboters
void Set_Joints(void) {
	// Knoten zur Gelenkmanipulation
	SoRotation *Gelenk;
	for (int i=0;i<5;i++) {
		// Knoten zur Manipulation des Gelenkes ermitteln
		Gelenk=(SoRotation*)SoNode::getByName(Gelenk_Name[i]);
		Gelenk->rotation.setValue(SbVec3f(0.0,0.0,1.0),Theta[i]);
		// Aufruf um Kraftarme anzupassen	
		Motion_Arm1u2();
	}
}

// Funktion zum vorhergehenden testen der neuen Armstellung
// Eingabe:
//		Nummer des Gelenkwinkels
//		neuer Wert des Gelenkwinkels
// Rueckgabe:
//		TRUE Armstellung moeglich
//		FALSE Armstellung nicht moeglich
bool Test_Calculate_Arm_Parameters(int Index,double Winkelwert) {
	double Eta,Line_a;
	bool Gelenkwinkel_OK=TRUE;
	// Test des Gelenkes 2
	if (Index==1) {
		if (((Min_Theta21<=Winkelwert)&&(Winkelwert<=Max_Theta21))||((Min_Theta22<=Winkelwert)&&(Winkelwert<=Max_Theta22))) {}
		else Gelenkwinkel_OK=FALSE;
	}
	// Test des Gelenkes 3
	if (Index==2) {
		Calculate_Arm_Parameters(Winkelwert,Eta,Line_a);
		if ((Min_Line_a<=Line_a)&&(Line_a<=Max_Line_a)) {}
		else Gelenkwinkel_OK=FALSE;
	}
	// Test des Gelenkes 4
	if (Index==3) {
		if (((Min_Theta41<=Winkelwert)&&(Winkelwert<=Max_Theta41))||((Min_Theta42<=Winkelwert)&&(Winkelwert<=Max_Theta42))) {}
		else Gelenkwinkel_OK=FALSE;
	}
	return Gelenkwinkel_OK;
}

// Funktion zur Berechnung der neuen Armstellung
// Eingabe:
//		Gelenkwinkel 3
// Ausgabe: 
//		Winkel Eta
//		Strecke Line_a
void Calculate_Arm_Parameters(double Theta3,double &Eta,double &Line_a) {
	double Gamma;
	Gamma=M_PI/2+Theta3;
	Line_a=sqrt(Tau2+L32-2*Tau*L3*cos(Gamma));
	Eta=acos((L32-Tau2-pow(Line_a,2))/(-2*Line_a*Tau));
	Eta=M_PI/2-Eta;
	Line_a=Line_a-Line_b;	
}

// Funktion zur Sichtbarmachung der neuen Armstellung
void Motion_Arm1u2(void) {
	double Eta,Line_a;
	Calculate_Arm_Parameters(Theta[2],Eta,Line_a);
	// Knoten zur Manipulation der Arme 1 & 2 finden
	SoCube *Motion_Line_a_Size;
	Motion_Line_a_Size=(SoCube*)SoNode::getByName(Arm2_Length);
	SoTranslation *Motion_Line_a_Trans;
	Motion_Line_a_Trans=(SoTranslation*)SoNode::getByName(Arm2_Trans);
	SoRotation *Motion_Eta_Rot;
	Motion_Eta_Rot=(SoRotation*)SoNode::getByName(Arm1_Rot);
	// Manipulation durchfuehren
	Motion_Line_a_Size->depth.setValue(Line_a);
	Motion_Line_a_Trans->translation.setValue(0,0,-Line_a/2-Line_b/2);
	Motion_Eta_Rot->rotation.setValue(SbVec3f(0.0,-1.0,0.0),Eta);
}

// Funktion zur Berechnung der neuen Koordinaten des Roboters
//
// Eingabe: Robot_Aktion
//			Zur Erkennung der Bewegung die der Roboter ausfuehren soll
//			ROTATION
//			TRANSLATION_FORWARD			
//			TRANSLATION_BACK
//          Eingabe: Nummer eines Objektes i (1...)
//			Ausgabe: Bereich in dem sich der Roboter von diesem
//                   Objekt i befindet
//					 !!! falls ein anderes Objekt j beruehrt wird ist die 
//					 !!! Ausgabe wie bei Eingabe der Objekt-Nummer 0
//			Eingabe: Objekt-Nummer 0 bedeutet falls ein Objekt beruehrt wird			
//			Ausgabe: Nummer des Objektes welches beruehrt wurde sonst NO_OBJEKT
//					 Bereich wo Objekt beruehrt wurde sonst NO_OBJEKT_AREA
// Rueckgabe:
// FALSE kein Objekt beruehrt
// TRUE Objekt beruehrt		
bool Calculate_Motion_Robot(int Robot_Aktion,int& Objekt_Nummer,int& Objekt_Bereich) {
	// Robot_Rot_Aktion: Drehbewegung des Roboters ?
	// FALSE Roboter soll nicht gedreht werden
	// TRUE Roboter soll gedreht werden
	bool Robot_Rot_Aktion=FALSE;
	// Kontakt: Objekt beruehrt ?
	// FALSE kein Objekt beruehrt
	// TRUE Objekt beruehrt
	bool Kontakt=FALSE;
	Point_3D Robot_Koord_Test=Robot_Koord;
	if (Robot_Aktion==ROTATION) {
		Kontakt=Objekt_Kontakt(Robot_Koord_Test,Objekt_Nummer,Objekt_Bereich);
		Robot_Rot_Aktion=TRUE;
	}
	if ((Robot_Aktion==TRANSLATION_FORWARD)&&(Robot_Rot==FALSE)) {
		Robot_Koord_Test.X=Robot_Koord.X+Step_Robot_Trans*cos(Robot_Direction);
		Robot_Koord_Test.Y=Robot_Koord.Y+Step_Robot_Trans*sin(Robot_Direction);
		Kontakt=Objekt_Kontakt(Robot_Koord_Test,Objekt_Nummer,Objekt_Bereich);
		if (Kontakt==FALSE) Robot_Koord=Robot_Koord_Test;
	}
	if ((Robot_Aktion==TRANSLATION_BACK)&&(Robot_Rot==FALSE)) {
		Robot_Koord_Test.X=Robot_Koord.X-Step_Robot_Trans*cos(Robot_Direction);
		Robot_Koord_Test.Y=Robot_Koord.Y-Step_Robot_Trans*sin(Robot_Direction);
		Kontakt=Objekt_Kontakt(Robot_Koord_Test,Objekt_Nummer,Objekt_Bereich);
		if (Kontakt==FALSE) Robot_Koord=Robot_Koord_Test;
	}
	Motion_Robot(Robot_Rot_Aktion);
	return Kontakt;
}

// Funktion zur Sichtbarmachung der neuen Roboterposition
// Eingabe: FALSE Roboter soll nicht gedreht werden
//			TRUE Roboter soll gedreht werden
void Motion_Robot(bool Robot_Rot_Aktion) {
	// Zeiger auf Knoten zur Translation des Roboters ermitteln
	SoTranslation *Robot_Trans;
	Robot_Trans=(SoTranslation*)SoNode::getByName(RoboterTranslation);
	// Zeiger auf Knoten zur Rotation des Roboters ermitteln
	SoRotation *Robot_Rotation;
	Robot_Rotation=(SoRotation*)SoNode::getByName(RoboterRotation);
	// Translation des Roboters
	if (Robot_Rot_Aktion==FALSE) {
		if (Robot_Rot==TRUE) {
			// Roboterfuss erst einfahren
			Robot_Foot_InOut->setInterval(Time_Move_Foot);
			// Roboterfuss wird eingefahren
			Robot_Foot_Move=TRUE;
			Robot_Foot_InOut->schedule();
		}
		else {
			// Translation des Roboters
			Robot_Trans->translation.setValue(Robot_Koord.X,Robot_Koord.Y,0.0);
			// Roboter-Kamera der Positions-oder Richtungsaenderung des Roboters anpassen
			if (Robot_View==TRUE) {
				Main_Camera->position.setValue(SbVec3f(Robot_Koord.Y,70,Robot_Koord.X));
				Main_Camera->orientation.setValue(SbVec3f(0.0,1.0,0.0),(float)Robot_Direction-M_PI);
			}
		}
	}
	// Rotation des Roboters
	if (Robot_Rot_Aktion==TRUE) {
		if (Robot_Rot==FALSE) {
			// Roboterfuss erst ausfahren
			Robot_Foot_InOut->setInterval(Time_Move_Foot);
			// Roboterfuss wird ausgefahren
			Robot_Foot_Move=TRUE;
			Robot_Foot_InOut->schedule();
		}
		else {
			Robot_Rotation->rotation.setValue(SbVec3f(0.0,0.0,1.0),Robot_Direction-M_PI/2);		
			// Roboter-Kamera der Positions-oder Richtungsaenderung des Roboters anpassen
			if (Robot_View==TRUE) {
				Main_Camera->position.setValue(SbVec3f(Robot_Koord.Y,70+FootLeg_High-11,Robot_Koord.X));
				Main_Camera->orientation.setValue(SbVec3f(0.0,1.0,0.0),(float)Robot_Direction-M_PI);
			}
		}
	}
}	

// Interupt-Funktion zum aus/einfahren des Rotationsfusses
void Robotfoot_drive(void*,SoSensor*) {
	// Zeiger auf Knoten zur Translation des Roboters ermitteln
	SoTranslation *Robot_Trans;
	Robot_Trans=(SoTranslation*)SoNode::getByName(RoboterTranslation);
	// Zeiger auf Knoten zur Manipulation des Roboters ermitteln
	SoTranslation *Foot_Trans;
	Foot_Trans=(SoTranslation*)SoNode::getByName(Fuss_Trans);
	SoCylinder *FootLeg_Height;
	FootLeg_Height=(SoCylinder*)SoNode::getByName(Fussbein_Groesse);
	SoTranslation *FootLeg_Trans;
	FootLeg_Trans=(SoTranslation*)SoNode::getByName(Fussbein_Trans);
	// Roboterfuss auf Boden absetzen bzw. Roboterfuss einfahren 
	Foot_Trans->translation.setValue(0.0,-FootLeg_High-2,0.0);
	FootLeg_Height->height.setValue(FootLeg_High);
	FootLeg_Trans->translation.setValue(0.0,(FootLeg_High/2+2)*-1,0.0);
	if (Robot_Rot==FALSE) {
		// Roboter anheben
		if (FootLeg_High>11) {
			Robot_Trans->translation.setValue(Robot_Koord.X,Robot_Koord.Y,FootLeg_High-11);
			// Roboter-Kamera der Positionsaenderung des Roboters anpassen
			if (Robot_View==TRUE) {
				Main_Camera->position.setValue(SbVec3f(Robot_Koord.Y,70+FootLeg_High-11,Robot_Koord.X));
				Main_Camera->orientation.setValue(SbVec3f(0.0,1.0,0.0),(float)Robot_Direction-M_PI);
			}
		}
		if (FootLeg_High==20) {
			// Roboterfuss komplett ausgefahren
			Robot_Foot_InOut->unschedule();
			Robot_Foot_Move=FALSE;
			// Fuss ausgefahren
			Robot_Rot=TRUE;
		}
		else FootLeg_High++;
	}
	else {
		// Roboter absenken
		if (FootLeg_High>10) {
			Robot_Trans->translation.setValue(Robot_Koord.X,Robot_Koord.Y,FootLeg_High-11);
			// Roboter-Kamera der Positionsaenderung des Roboters anpassen
			if (Robot_View==TRUE) {
				Main_Camera->position.setValue(SbVec3f(Robot_Koord.Y,70+FootLeg_High-11,Robot_Koord.X));
				Main_Camera->orientation.setValue(SbVec3f(0.0,1.0,0.0),(float)Robot_Direction-M_PI);
			}
		}
		if (FootLeg_High==4) {
			// Roboterfuss komplett eingefahren
			Robot_Foot_InOut->unschedule();
			Robot_Foot_Move=FALSE;
			// Fuss eingefahren
			Robot_Rot=FALSE;		
		}
		else FootLeg_High--;
	}
}

// Funktion zur Selektionsbehandlung von Objekten im Haupt-Window
void Main_SelectionCB(void *,SoPath *Selection_Path) {
	SoNode *Knoten=Selection_Path->getTail();
	if ((Knoten->getName()==DragRobot)&&(Tool_Busy==FALSE)&&(Robot_Rot==FALSE)) {
		SoSeparator *Roboter_Sep;
		Roboter_Sep=(SoSeparator*)SoNode::getByName(RoboterSeparator);
		// Roboter aus Szenegraphen loeschen
		Roboter_Sep->removeAllChildren();
		// Haupt-Wurzelknoten finden
		SoSeparator *Main_Root;
		Main_Root=(SoSeparator*)SoNode::getByName(Haupt_Wurzelknoten);
		// Robot-Dragger erzeugen
		SoTranslate2Dragger *Robot_Dragger = new SoTranslate2Dragger;
		Robot_Dragger->setName(RobotDragger);
		Robot_Dragger->translation.setValue(Robot_Koord.X,Robot_Koord.Y,0.0);
		// Robot-Dragger aktivieren
		Robot_Dragger->isActive.setValue(TRUE);
		// Geometrie des Target-Draggers festlegen
		Robot_Dragger->setPart("translator",Build_Robot());
		Robot_Dragger->setPart("translatorActive",Build_Robot_Aktive());
		// Dragger-Callback-Funktion initialisieren
		Robot_Dragger->addFinishCallback(Read_Robot_Dragger_Pos,Robot_Dragger);
		// Dragger in Szene einfuegen
		Main_Root->addChild(Robot_Dragger);
	}
	// Deselection aller Knoten im Haupt-Window
	SoSelection *Wurzel_Sel;
	Wurzel_Sel=(SoSelection*)SoNode::getByName(MainSelRoot);
	Wurzel_Sel->deselectAll();
}

// Funktion zur Formveraenderung des Roboter-Draggers im nicht aktiven Zustand 
SoSeparator *Build_Robot(void) {
	SoSeparator *Robot_Dragger_Sep = new SoSeparator;
	Robot_Dragger_Sep->ref();
	// Rotation des Roboters realisieren
	SoRotation *Robot_Dragger_Rot = new SoRotation;
	Robot_Dragger_Rot->rotation.setValue(SbVec3f(0.0,0.0,1.0),Robot_Direction-M_PI/2);
	Robot_Dragger_Sep->addChild(Robot_Dragger_Rot);
	// Laden der Dragger-Form
	SoSeparator *Sep=Read_File(ROBOT_Dragger_File);
	if (Sep==NULL) Print_News_Screen("NO ROBOT DRAGGER",NOTEXT,NOTEXT);
	else Robot_Dragger_Sep->addChild(Sep);
	return (Robot_Dragger_Sep);
}

// Funktion zur Formveraenderung des Roboter-Draggers im aktiven Zustand 
SoSeparator *Build_Robot_Aktive(void) {
	SoSeparator *Robot_Aktive = new SoSeparator;
	Robot_Aktive->ref();
	Robot_Aktive->addChild(Build_Robot());
	return(Robot_Aktive);
}

// Funktion zum Auslesen der Robot-Dragger Position und Wiederherstellung des Robotersystems
void Read_Robot_Dragger_Pos(void *,SoDragger *Robot_Dragger_Help) {
	SbVec3f Point;
	float X,Y,Z;
	// Translation des Robot-Draggers auslesen
	SoTranslate2Dragger *Robot_Dragger=(SoTranslate2Dragger*)Robot_Dragger_Help;
	Point=Robot_Dragger->translation.getValue();
	// Roboterkoordinaten auslesen und speichern
	Point.getValue(X,Y,Z);
	Robot_Koord.X=(double)X;
	Robot_Koord.Y=(double)Y;
	Z=0.0;
	// Haupt-Wurzelknoten finden
	SoSeparator *Main_Root;
	Main_Root=(SoSeparator*)SoNode::getByName(Haupt_Wurzelknoten);
	// Robot-Dragger loeschen
	Main_Root->removeChild(Robot_Dragger);
	// Roboter wieder in Szene einfuegen
	SoSeparator *Robotsystem;
	Robotsystem=(SoSeparator*)SoNode::getByName(RoboterSeparator);
	SoTranslation *Robot_Trans = new SoTranslation;
	Robot_Trans->translation.setValue(X,Y,Z);
	// Knoten Namen geben
	Robot_Trans->setName(RoboterTranslation);
	Robotsystem->addChild(Robot_Trans);
	SoRotation *Robot_Rotation = new SoRotation;
	Robot_Rotation->rotation.setValue(SbVec3f(0.0,0.0,1.0),Robot_Direction-M_PI/2);
	// Knoten Namen geben
	Robot_Rotation->setName(RoboterRotation);
	Robotsystem->addChild(Robot_Rotation);
	// Einlesen des Inventoraustauschfiles von B.A.R.T
	SoSeparator *Robot=Read_File(Roboter_File);
	if (Robot==NULL) {
		cout << "File: " << Roboter_File << " kann nicht geoeffnet werden" << endl;
		cout << ", ist nicht vorhanden oder " << endl;
		cout << "ist nicht korrekt !!!" << endl;
	}
	else Robotsystem->addChild(Robot);
}

// Funktion zur Formveraenderung des Ziel-Draggers im nicht aktiven Zustand 
SoSeparator *Build_Target(void) {
	SoSeparator *Target_Dragger_Sep = new SoSeparator;
	Target_Dragger_Sep->ref();
	// Laden der Dragger-Form
	SoSeparator *Sep=Read_File(TARGET_Dragger_File);
	if (Sep==NULL) Print_News_Screen("NO TARGET DRAGGER",NOTEXT,NOTEXT);
	else Target_Dragger_Sep=Sep;
	return (Target_Dragger_Sep);
}

// Funktion zur Formveraenderung des Ziel-Draggers im aktiven Zustand 
SoSeparator *Build_Target_Aktive(void) {
	SoSeparator *Target_Aktive = new SoSeparator;
	Target_Aktive->ref();
	Target_Aktive->addChild(Build_Target());
	return(Target_Aktive);
}

// Funktion zum Auslesen der Ziel-Dragger Position
void Read_Target_Dragger_Pos(void *,SoDragger * Target_Dragger_Help) {
	SbVec3f Point;
	float X,Y,Z;
	// Translation des Target-Draggers auslesen
	SoTranslate2Dragger *Target_Dragger=(SoTranslate2Dragger*)Target_Dragger_Help;
	Point=Target_Dragger->translation.getValue();
	// Zielpunkt auslesen und speichern
	Point.getValue(X,Y,Z);
	Robot_Target.X=(double)X;
	Robot_Target.Y=(double)Y;
	Z=0.0;
}


// Ausgabe am News-Screen im Terminal-Window
// Eingabe:
//	Alarm 	Text
//	Aktion 	Text
// 	Info 	Text
void Print_News_Screen(char Alarm[],char Aktion[],char Info[]) {
	SoText3 *Text;
	// Eingabe-Zeichenfolge=OLDTEXT -> alter Text bleibt stehen
	if (strcmp(Alarm,OLDTEXT)==0);
	else {
		// Ausgabe in der Alarm-Zeile
		Text=(SoText3*)SoNode::getByName(ALARM_Zeile);
		Text->string.setValue(Alarm);
	}
	if (strcmp(Aktion,OLDTEXT)==0); 
	else {
		// Ausgabe in der Aktion-Zeile
		Text=(SoText3*)SoNode::getByName(AKTION_Zeile);
		Text->string.setValue(Aktion);
	}
	if (strcmp(Info,OLDTEXT)==0); 
	else {
		// Ausgabe in der Info-Zeile
		Text=(SoText3*)SoNode::getByName(INFO_Zeile);
		Text->string.setValue(Info);
	}
}

// Funktion zum betaetigen einer Taste im Terminal-Window
void Press_Button(void*,SoSensor*) {
	SbVec3f Point;
	float X,Y,Z;
	// Translation der Taste auslesen
	Point=Button_Trans->translation.getValue();
	Point.getValue(X,Y,Z);
	if (KeyPressed==FALSE) {
		// Taste gedrueckt
		// Farbe der Taste setzen
		Button_Color->rgb.setValue(GRUEN);
		// Taste gedrueckt
		Z--;
		KeyPressed=TRUE;
		// Taste halten
		PressButton->setInterval(Time_Hold_Button);
	}
	else {
		// Taste loslassen und Zeit wieder auf Ausgangsituation setzen
		PressButton->setInterval(Time_Press_Button);
		// Timer abschalten
		PressButton->unschedule();
		// Farbe der Taste setzen
		Button_Color->rgb.setValue(WEISS);
		// Taste loslassen
		Z++;
		KeyPressed=FALSE;
	}
	Button_Trans->translation.setValue(X,Y,Z);
}

// Funktion zum runterfahren eines Screens im Terminal-Window
void Screen_Move_Down1(void*,SoSensor*) {
	SbVec3f Point;
	float X,Y,Z;
	// Translation des jeweiligen Screens auslesen
	Point=Screen_Trans_Down1->translation.getValue();
	Point.getValue(X,Y,Z);
	Y=Y-Step_Move_Screen;
	if (Y<Screen_Finish_Point_Down1) {
		// Screen auf exakte jeweilige Endposition setzen
		Y=Screen_Finish_Point_Down1;
		// Timer abschalten
		ScreenMoveDown1->unschedule();
		// Screen wird nicht mehr nach unten bewegt
		MovingDownScreen1=FALSE;
	}
	// Translation des jeweiligen Screens setzen
	Point.setValue(X,Y,Z);
	Screen_Trans_Down1->translation.setValue(Point);
}

// weitere Funktion zum runterfahren eines Screens im Terminal-Window
void Screen_Move_Down2(void*,SoSensor*) {
	SbVec3f Point;
	float X,Y,Z;
	// Translation des jeweiligen Screens auslesen
	Point=Screen_Trans_Down2->translation.getValue();
	Point.getValue(X,Y,Z);
	Y=Y-Step_Move_Screen;
	if (Y<Screen_Finish_Point_Down2) {
		// Screen auf exakte jeweilige Endposition setzen
		Y=Screen_Finish_Point_Down2;
		// Timer abschalten
		ScreenMoveDown2->unschedule();
		// Screen wird nicht mehr nach unten bewegt
		MovingDownScreen2=FALSE;
	}
	// Translation des jeweiligen Screens setzen
	Point.setValue(X,Y,Z);
	Screen_Trans_Down2->translation.setValue(Point);
}

// Funktion zum hochfahren eines Screens im Terminal-Window
void Screen_Move_Up1(void*,SoSensor*) {
	SbVec3f Point;
	float X,Y,Z;
	// Translation des jeweiligen Screens auslesen
	Point=Screen_Trans_Up1->translation.getValue();
	Point.getValue(X,Y,Z);
	Y=Y+Step_Move_Screen;
	if (Y>Screen_Finish_Point_Up1) {
		// Screen auf exakte jeweilige Endposition setzen
		Y=Screen_Finish_Point_Up1;
		// Timer abschalten
		ScreenMoveUp1->unschedule();
		// Screen wird nicht mehr nach oben bewegt
		MovingUpScreen1=FALSE;
	}
	// Translation des jeweiligen Screens setzen
	Point.setValue(X,Y,Z);
	Screen_Trans_Up1->translation.setValue(Point);
}

// weitere Funktion zum hochfahren eines Screens im Terminal-Window
void Screen_Move_Up2(void*,SoSensor*) {
	SbVec3f Point;
	float X,Y,Z;
	// Translation des jeweiligen Screens auslesen
	Point=Screen_Trans_Up2->translation.getValue();
	Point.getValue(X,Y,Z);
	Y=Y+Step_Move_Screen;
	if (Y>Screen_Finish_Point_Up2) {
		// Screen auf exakte jeweilige Endposition setzen
		Y=Screen_Finish_Point_Up2;
		// Timer abschalten
		ScreenMoveUp2->unschedule();
		// Screen wird nicht mehr nach oben bewegt
		MovingUpScreen2=FALSE;
	}
	// Translation des jeweiligen Screens setzen
	Point.setValue(X,Y,Z);
	Screen_Trans_Up2->translation.setValue(Point);
}

// Funktion zur Selektionsbehandlung von Terminal-Objekten
void Terminal_SelectionCB(void *,SoPath *Selection_Path) {
	SoNode *Knoten=Selection_Path->getTail();
	// bei Bewegung des Screens wird keine weitere Selektion beachtet
	if ((MovingDownScreen1==FALSE)&&(MovingDownScreen2==FALSE)&&(MovingUpScreen1==FALSE)&&(MovingUpScreen2==FALSE)&&(KeyPressed==FALSE)) {
		// Behandlung bei der Selektion des Zielobjektes
		if (Knoten->getName()==MANUAL_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(MANUAL_Taste_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(MANUAL_Taste_Trans);
			PressButton->schedule();
			// auszufuehrende Aktion
			Aktion=0;
			Aktion_Number=0;
			switch (MenuLayer) {
				case NOLAYER:
					// Ausgabe auf den News-Screen
					Print_News_Screen(NOTEXT,"MANUAL",NOTEXT);			
					// Manual-Screen von oben einschieben
					MovingDownScreen1=TRUE;
					Screen_Finish_Point_Down1=Full_Screen;
					Screen_Trans_Down1=(SoTranslation*)SoNode::getByName(Manual_Screen_Trans);
					ScreenMoveDown1->schedule();
					// neue Menueebene
					MenuLayer=MANUAL;
					break;
				case MANUAL:
					// NOP
					break;
				case MANUAL_JOINT:
					// NOP
					break;
				case AUTO:
					// Ausgabe auf den News-Screen
					Print_News_Screen(NOTEXT,"MANUAL",NOTEXT);
					// neue Menueebene
					MenuLayer=MANUAL;
					break;
				case AUTO_ROBOT:
					// NOP
					break;
				case AUTO_JOINT:
					// NOP
					break;
				case LOAD:
					// Ausgabe auf den News-Screen loeschen
					Print_News_Screen(NOTEXT,NOTEXT,NOTEXT);
					// Load-Screen nach unten rausschieben
					MovingDownScreen1=TRUE;
					Screen_Finish_Point_Down1=-59;
					Screen_Trans_Down1=(SoTranslation*)SoNode::getByName(Load_Screen_Trans);
					ScreenMoveDown1->schedule();
					// Ausgabe auf den News-Screen
					Print_News_Screen(NOTEXT,"MANUAL",NOTEXT);
					// Manual-Screen von oben einschieben
					MovingDownScreen2=TRUE;
					Screen_Finish_Point_Down2=Full_Screen;
					Screen_Trans_Down2=(SoTranslation*)SoNode::getByName(Manual_Screen_Trans);
					ScreenMoveDown2->schedule();
					// neue Menueebene
					MenuLayer=MANUAL;
					break;
			}
		}
		if (Knoten->getName()==AUTO_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(AUTO_Taste_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(AUTO_Taste_Trans);
			PressButton->schedule();
			// auszufuehrende Aktion
			Aktion=0;
			Aktion_Number=0;
			switch (MenuLayer) {
				case NOLAYER:
					// Ausgabe auf den News-Screen
					Print_News_Screen(NOTEXT,"AUTOMATIC",NOTEXT);	
					// Auto-Screen=Manual-Screen von oben einschieben
					MovingDownScreen1=TRUE;
					Screen_Finish_Point_Down1=Full_Screen;
					Screen_Trans_Down1=(SoTranslation*)SoNode::getByName(Manual_Screen_Trans);
					ScreenMoveDown1->schedule();
					// neue Menueebene
					MenuLayer=AUTO;
					break;
				case MANUAL:
					// Ausgabe auf den News-Screen
					Print_News_Screen(NOTEXT,"AUTOMATIC",NOTEXT);
					// neue Menueebene
					MenuLayer=AUTO;	
					break;
				case MANUAL_JOINT:
					// NOP	
					break;
				case AUTO:
					// NOP
					break;
				case AUTO_ROBOT:
					// NOP	
					break;
				case AUTO_JOINT:
					// NOP	
					break;
				case LOAD:
					// Ausgabe auf den News-Screen loeschen
					Print_News_Screen(NOTEXT,NOTEXT,NOTEXT);
					// Load-Screen nach unten rausschieben
					MovingDownScreen1=TRUE;
					Screen_Finish_Point_Down1=Down_Screen;
					Screen_Trans_Down1=(SoTranslation*)SoNode::getByName(Load_Screen_Trans);
					ScreenMoveDown1->schedule();
					// Ausgabe auf den News-Screen loeschen
					Print_News_Screen(NOTEXT,"AUTOMATIC",NOTEXT);
					// Auto-Screen=Manual-Screen von oben einschieben
					MovingDownScreen2=TRUE;
					Screen_Finish_Point_Down2=Full_Screen;
					Screen_Trans_Down2=(SoTranslation*)SoNode::getByName(Manual_Screen_Trans);
					ScreenMoveDown2->schedule();
					// neue Menueebene
					MenuLayer=AUTO;	
					break;
			}
		}
		if (Knoten->getName()==LOAD_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(LOAD_Taste_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(LOAD_Taste_Trans);
			PressButton->schedule();
			// auszufuehrende Aktion
			Aktion=0;
			Aktion_Number=0;
			switch (MenuLayer) {
				case NOLAYER:
					// Filename initialisieren
					Filename[0]='\0';
					strcat(Filename,"laby");
					Filename_Length=Min_Filename_Length-1;
					// Ausgabe auf den News-Screen
					Print_News_Screen(NOTEXT,"File:",Filename);	
					// Load-Screen von oben einschieben
					MovingUpScreen1=TRUE;
					Screen_Finish_Point_Up1=Full_Screen;
					Screen_Trans_Up1=(SoTranslation*)SoNode::getByName(Load_Screen_Trans);
					ScreenMoveUp1->schedule();
					// neue Menueebene
					MenuLayer=LOAD;
					break;
				case MANUAL:
					// Filename initialisieren
					Filename[0]='\0';
					strcat(Filename,"laby");
					Filename_Length=Min_Filename_Length-1;
					// Ausgabe auf den News-Screen
					Print_News_Screen(NOTEXT,"File:",Filename);	
					// Manual-Screen nach oben rausschieben
					MovingUpScreen1=TRUE;
					Screen_Finish_Point_Up1=Up_Screen;
					Screen_Trans_Up1=(SoTranslation*)SoNode::getByName(Manual_Screen_Trans);
					ScreenMoveUp1->schedule();
					// Load-Screen von unten einschieben
					MovingUpScreen2=TRUE;
					Screen_Finish_Point_Up2=Full_Screen;
					Screen_Trans_Up2=(SoTranslation*)SoNode::getByName(Load_Screen_Trans);
					ScreenMoveUp2->schedule(); 
					// neue Menueebene
					MenuLayer=LOAD;
					break;
				case MANUAL_JOINT:
					// NOP
					break;
				case AUTO:
					// Filename initialisieren
					Filename[0]='\0';
					strcat(Filename,"laby");
					Filename_Length=Min_Filename_Length-1;
					// Ausgabe auf den News-Screen
					Print_News_Screen(NOTEXT,"File:",Filename);	
					// Auto-Screen=Manual-Screen nach oben rausschieben
					MovingUpScreen1=TRUE;
					Screen_Finish_Point_Up1=Up_Screen;
					Screen_Trans_Up1=(SoTranslation*)SoNode::getByName(Manual_Screen_Trans);
					ScreenMoveUp1->schedule();
					// Load-Screen von unten einschieben
					MovingUpScreen2=TRUE;
					Screen_Finish_Point_Up2=Full_Screen;
					Screen_Trans_Up2=(SoTranslation*)SoNode::getByName(Load_Screen_Trans);
					ScreenMoveUp2->schedule();
					// neue Menueebene
					MenuLayer=LOAD;
					break;
				case AUTO_ROBOT:
					// NOP
					break;
				case AUTO_JOINT:
					// NOP
					break;
				case LOAD:
					// Filename initialisieren
					Filename[0]='\0';
					strcat(Filename,"laby");
					Filename_Length=Min_Filename_Length-1;
					// Ausgabe auf den News-Screen
					Print_News_Screen(NOTEXT,"File:",Filename);
					break;
			}
		}
		if (Knoten->getName()==BACK_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(BACK_Taste_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(BACK_Taste_Trans);
			PressButton->schedule();
			switch (MenuLayer) {
				case NOLAYER:
					// NOP
					break;
				case MANUAL:
					// Ausgabe auf den News-Screen loeschen
					Print_News_Screen(NOTEXT,NOTEXT,NOTEXT);
					// Manuell-Screen nach oben rausschieben
					MovingUpScreen1=TRUE;
					Screen_Finish_Point_Up1=Up_Screen;
					Screen_Trans_Up1=(SoTranslation*)SoNode::getByName(Manual_Screen_Trans);
					ScreenMoveUp1->schedule();
					// neue Menueebene
					MenuLayer=NOLAYER;
					break;
				case MANUAL_JOINT:
					// Manuell-Joint-Screen nach unten rausschieben
					MovingDownScreen1=TRUE;
					Screen_Finish_Point_Down1=Down_Screen;
					Screen_Trans_Down1=(SoTranslation*)SoNode::getByName(Manual_Joint_Screen_Trans);
					ScreenMoveDown1->schedule();
					// Manual-Screen von oben reinschieben
					MovingDownScreen2=TRUE;
					Screen_Finish_Point_Down2=Full_Screen;
					Screen_Trans_Down2=(SoTranslation*)SoNode::getByName(Manual_Screen_Trans);
					ScreenMoveDown2->schedule();
					// neue Menueebene
					MenuLayer=MANUAL;
					break;
				case AUTO:
					// Ausgabe auf den News-Screen loeschen
					Print_News_Screen(NOTEXT,NOTEXT,NOTEXT);
					// Auto-Screen=Manual-Screen nach oben rausschieben
					MovingUpScreen1=TRUE;
					Screen_Finish_Point_Up1=Up_Screen;
					Screen_Trans_Up1=(SoTranslation*)SoNode::getByName(Manual_Screen_Trans);
					ScreenMoveUp1->schedule();
					// neue Menueebene
					MenuLayer=NOLAYER;
					break;
				case AUTO_ROBOT:
					// Auto-Robot-Screen nach unten rausschieben
					MovingDownScreen1=TRUE;
					Screen_Finish_Point_Down1=Down_Screen;
					Screen_Trans_Down1=(SoTranslation*)SoNode::getByName(Auto_Robot_Screen_Trans);
					ScreenMoveDown1->schedule();
					// Auto-Screen=Manual-Screen von oben reinschieben
					MovingDownScreen2=TRUE;
					Screen_Finish_Point_Down2=Full_Screen;
					Screen_Trans_Down2=(SoTranslation*)SoNode::getByName(Manual_Screen_Trans);
					ScreenMoveDown2->schedule();
					// neue Menueebene
					MenuLayer=AUTO;
					// Stop-Search-Timer();
					break;
				case AUTO_JOINT:
					// Auto-Joint-Screen nach unten rausschieben
					MovingDownScreen1=TRUE;
					Screen_Finish_Point_Down1=Down_Screen;
					Screen_Trans_Down1=(SoTranslation*)SoNode::getByName(Auto_Joint_Screen_Trans);
					ScreenMoveDown1->schedule();
					// Auto-Screen=Manual-Screen von oben reinschieben
					MovingDownScreen2=TRUE;
					Screen_Finish_Point_Down2=Full_Screen;
					Screen_Trans_Down2=(SoTranslation*)SoNode::getByName(Manual_Screen_Trans);
					ScreenMoveDown2->schedule();
					// neue Menueebene
					MenuLayer=AUTO;
					// Stop_Search_Timer();
					break;
				case LOAD:
					// Ausgabe auf den News-Screen loeschen
					Print_News_Screen(NOTEXT,NOTEXT,NOTEXT);	
					// Load-Screen nach unten rausschieben
					MovingDownScreen1=TRUE;
					Screen_Finish_Point_Down1=Down_Screen;
					Screen_Trans_Down1=(SoTranslation*)SoNode::getByName(Load_Screen_Trans);
					ScreenMoveDown1->schedule();
					// neue Menueebene
					MenuLayer=NOLAYER;
					break;
			}
		}
		if (Knoten->getName()==VIEW_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(VIEW_Taste_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(VIEW_Taste_Trans);
			PressButton->schedule();
			// Kamera umschalten
			Change_Camera_View();
		}
		if (Knoten->getName()==STOP_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(STOP_Taste_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(STOP_Taste_Trans);
			PressButton->schedule();
			switch (MenuLayer) {
				case NOLAYER:
					// NOP
					break;
				case MANUAL:
					//NOP
					break;
				case MANUAL_JOINT:
					// NOP
					break;
				case AUTO:
					//NOP
					break;
				case AUTO_ROBOT:
					Stop_All_Search_Timer();
					Print_News_Screen(NOTEXT,NOTEXT,NOTEXT);
					break;
				case AUTO_JOINT:
					// Roboterarm zum Ziel bewegen anhalten
					break;
				case LOAD:
					//NOP
					break;
			}
		}
		if (Knoten->getName()==EXIT_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(EXIT_Taste_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(EXIT_Taste_Trans);
			PressButton->schedule();
			Program_Exit();
		}
		if (Knoten->getName()==ROBOT_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(ROBOT_Taste_Text_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(ROBOT_Taste_Trans);
			PressButton->schedule();
			switch (MenuLayer) {
				case MANUAL:
					Print_News_Screen(NOTEXT,"MANUAL ROBOT","USE CURSOR");
					// auszufuehrende Aktion
					Aktion=2;
					Aktion_Number=1;
					break;
				case AUTO:
					Print_News_Screen(NOTEXT,"AUTO ROBOT",NOTEXT);
					// Auto-Screen=Manual-Screen nach oben rausschieben
					MovingUpScreen1=TRUE;
					Screen_Finish_Point_Up1=Up_Screen;
					Screen_Trans_Up1=(SoTranslation*)SoNode::getByName(Manual_Screen_Trans);
					ScreenMoveUp1->schedule(); 
					// Auto-Robot-Screen von unten einschieben
					MovingUpScreen2=TRUE;
					Screen_Finish_Point_Up2=Full_Screen;
					Screen_Trans_Up2=(SoTranslation*)SoNode::getByName(Auto_Robot_Screen_Trans);
					ScreenMoveUp2->schedule();
					// auszufuehrende Aktion
					Aktion=8;
					Aktion_Number=0;
					// neue Menueebene
					MenuLayer=AUTO_ROBOT;
					break;
			}
		}
		if (Knoten->getName()==JOINT_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(JOINT_Taste_Text_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(JOINT_Taste_Trans);
			PressButton->schedule();
			switch (MenuLayer) {
				case MANUAL:
					Print_News_Screen(NOTEXT,"MANUAL JOINT",NOTEXT);
					// Manual-Screen nach oben rausschieben
					MovingUpScreen1=TRUE;
					Screen_Finish_Point_Up1=Up_Screen;
					Screen_Trans_Up1=(SoTranslation*)SoNode::getByName(Manual_Screen_Trans);
					ScreenMoveUp1->schedule(); 
					// Manual-Joint-Screen von unten einschieben
					MovingUpScreen2=TRUE;
					Screen_Finish_Point_Up2=Full_Screen;
					Screen_Trans_Up2=(SoTranslation*)SoNode::getByName(Manual_Joint_Screen_Trans);
					ScreenMoveUp2->schedule();
					// auszufuehrende Aktion
					Aktion=1;
					// neue Menueebene
					MenuLayer=MANUAL_JOINT;
					break;
				case AUTO:
					Print_News_Screen(NOTEXT,"AUTO JOINT",NOTEXT);
					// Auto-Screen=Manual-Screen nach oben rausschieben
					MovingUpScreen1=TRUE;
					Screen_Finish_Point_Up1=Up_Screen;
					Screen_Trans_Up1=(SoTranslation*)SoNode::getByName(Manual_Screen_Trans);
					ScreenMoveUp1->schedule();
					// Auto-Joint-Screen von unten einschieben
					MovingUpScreen2=TRUE;
					Screen_Finish_Point_Up2=Full_Screen;
					Screen_Trans_Up2=(SoTranslation*)SoNode::getByName(Auto_Joint_Screen_Trans);
					ScreenMoveUp2->schedule();
					// auszufuehrende Aktion
					Aktion=4;
					// neue Menueebene
					MenuLayer=AUTO_JOINT;
					break;
			}
		}
		// Nur manuelle Armbewegung wenn automatische nicht mehr laeuft
		if (Robot_Arm_Move==FALSE) {
			if (Knoten->getName()==JOINT1_Taste) {
				// Taste druecken
				Button_Color=(SoBaseColor*)SoNode::getByName(JOINT1_Taste_Text_Farbe);
				Button_Trans=(SoTranslation*)SoNode::getByName(JOINT1_Taste_Trans);
				PressButton->schedule();
				Print_News_Screen(NOTEXT,"JOINT 1","USE CURSOR");
				Aktion_Number=1;
			}
			if (Knoten->getName()==JOINT2_Taste) {
				// Taste druecken
				Button_Color=(SoBaseColor*)SoNode::getByName(JOINT2_Taste_Text_Farbe);
				Button_Trans=(SoTranslation*)SoNode::getByName(JOINT2_Taste_Trans);
				PressButton->schedule();
				Print_News_Screen(NOTEXT,"JOINT 2","USE CURSOR");
				Aktion_Number=2;
			}
			if (Knoten->getName()==JOINT3_Taste) {
				// Taste druecken
				Button_Color=(SoBaseColor*)SoNode::getByName(JOINT3_Taste_Text_Farbe);
				Button_Trans=(SoTranslation*)SoNode::getByName(JOINT3_Taste_Trans);
				PressButton->schedule();
				Print_News_Screen(NOTEXT,"JOINT 3","USE CURSOR");
				Aktion_Number=3;
			}
			if (Knoten->getName()==JOINT4_Taste) {
				// Taste druecken
				Button_Color=(SoBaseColor*)SoNode::getByName(JOINT4_Taste_Text_Farbe);
				Button_Trans=(SoTranslation*)SoNode::getByName(JOINT4_Taste_Trans);
				PressButton->schedule();
				Print_News_Screen(NOTEXT,"JOINT 4","USE CURSOR");
				Aktion_Number=4;
			}
			if (Knoten->getName()==JOINT5_Taste) {
				// Taste druecken
				Button_Color=(SoBaseColor*)SoNode::getByName(JOINT5_Taste_Text_Farbe);
				Button_Trans=(SoTranslation*)SoNode::getByName(JOINT5_Taste_Trans);
				PressButton->schedule();
				Print_News_Screen(NOTEXT,"JOINT 5","USE CURSOR");
				Aktion_Number=5;
			}
		}
		// Nur neuen Such-Algorithmus starten wenn kein anderer mehr laeuft !!!
		if (Robot_Auto_Move==FALSE) {
			if (Knoten->getName()==SIMPLE_Taste) {
				// Taste druecken
				Button_Color=(SoBaseColor*)SoNode::getByName(SIMPLE_Taste_Text_Farbe);
				Button_Trans=(SoTranslation*)SoNode::getByName(SIMPLE_Taste_Trans);
				PressButton->schedule();
				Print_News_Screen(NOTEXT,"SIMPLE SEARCH RUN",NOTEXT);
				Init_Labyrinth_Search(SIMPLE_METHODE);
			}
			if (Knoten->getName()==MODULO_Taste) {
				// Taste druecken
				Button_Color=(SoBaseColor*)SoNode::getByName(MODULO_Taste_Text_Farbe);
				Button_Trans=(SoTranslation*)SoNode::getByName(MODULO_Taste_Trans);
				PressButton->schedule();
				Print_News_Screen(NOTEXT,"MODULO SEARCH RUN",NOTEXT);
				Init_Labyrinth_Search(MODULO_METHODE);
			}
			if (Knoten->getName()==PLEDGE_Taste) {
				// Taste druecken
				Button_Color=(SoBaseColor*)SoNode::getByName(PLEDGE_Taste_Text_Farbe);
				Button_Trans=(SoTranslation*)SoNode::getByName(PLEDGE_Taste_Trans);
				PressButton->schedule();
				Print_News_Screen(NOTEXT,"PLEDGE SEARCH RUN",NOTEXT);
				Init_Labyrinth_Search(PLEDGE_METHODE);
			}
			if (Knoten->getName()==BUG_Taste) {
				// Taste druecken
				Button_Color=(SoBaseColor*)SoNode::getByName(BUG_Taste_Text_Farbe);
				Button_Trans=(SoTranslation*)SoNode::getByName(BUG_Taste_Trans);
				PressButton->schedule();
				Print_News_Screen(NOTEXT,"BUG SEARCH RUN",NOTEXT);
				Init_Target_Search(BUG_METHODE);
			}
			if (Knoten->getName()==CORNER_Taste) {
				// Taste druecken
				Button_Color=(SoBaseColor*)SoNode::getByName(CORNER_Taste_Text_Farbe);
				Button_Trans=(SoTranslation*)SoNode::getByName(CORNER_Taste_Trans);
				PressButton->schedule();
				Print_News_Screen(NOTEXT,"CORNER SEARCH RUN",NOTEXT);
				Init_Target_Search(CORNER_METHODE);
			}
		}
		// Nur neue automatische Armbewegung wenn keine andere automatische Armbewegung mehr laeuft
		if (Robot_Arm_Move==FALSE) {
			if (Knoten->getName()==PICKUP_Taste) {
				// Taste druecken
				Button_Color=(SoBaseColor*)SoNode::getByName(PICKUP_Taste_Text_Farbe);
				Button_Trans=(SoTranslation*)SoNode::getByName(PICKUP_Taste_Trans);
				PressButton->schedule();
				Print_News_Screen(NOTEXT,OLDTEXT,NOTEXT);
				// Start der Aufnahme des Zielobjektes
				if (Tool_Busy==FALSE) {
					// Tool soll nach erreichen des Zielobjektes dieses aufheben
					What_Tool_Doit=TOOL_TAKE_TARGETOBJEKT;
					if (!Robot_TakeONOFF_TargetObjekt()) Print_News_Screen("CANT PICKUP OBJECT",OLDTEXT,NOTEXT);
				}
				else Print_News_Screen("TOOL BUSY",OLDTEXT,NOTEXT);
			}
			if (Knoten->getName()==TAKEOFF_Taste) {
				// Taste druecken
				Button_Color=(SoBaseColor*)SoNode::getByName(TAKEOFF_Taste_Text_Farbe);
				Button_Trans=(SoTranslation*)SoNode::getByName(TAKEOFF_Taste_Trans);
				PressButton->schedule();
				Print_News_Screen(NOTEXT,OLDTEXT,NOTEXT);
				// Start der Ablage des Zielobjektes
				if (Tool_Busy==TRUE) {
					// Tool soll nach erreichen des Zielobjektes dieses ablegen
					What_Tool_Doit=TOOL_TAKEOFF_TARGETOBJEKT;
					if (!Robot_TakeONOFF_TargetObjekt()) Print_News_Screen("CANT TAKEOFF OBJECT",OLDTEXT,NOTEXT);
				}
				else Print_News_Screen("NO OBJECT ON TOOL",OLDTEXT,NOTEXT);
			}
			if (Knoten->getName()==STARTPOS_Taste) {
				// Taste druecken
				Button_Color=(SoBaseColor*)SoNode::getByName(STARTPOS_Taste_Text_Farbe);
				Button_Trans=(SoTranslation*)SoNode::getByName(STARTPOS_Taste_Trans);
				PressButton->schedule();
				Print_News_Screen(NOTEXT,OLDTEXT,NOTEXT);
				// Start der automatischen Gelenkbewegung in die Ausgangsposition
				Reset_Robot_Joints();
			}
		}
		if (Knoten->getName()==NUMBER1_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(NUMBER1_Taste_Text_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(NUMBER1_Taste_Trans);
			PressButton->schedule();
			if (Filename_Length!=Max_Filename_Length-1) {
				strcat(Filename,"1");
				Filename_Length++;
			}
			// Ausgabe auf den News-Screen
			Print_News_Screen(NOTEXT,"File:",Filename);
		}
		if (Knoten->getName()==NUMBER2_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(NUMBER2_Taste_Text_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(NUMBER2_Taste_Trans);
			PressButton->schedule();
			if (Filename_Length!=Max_Filename_Length-1) {
				strcat(Filename,"2");
				Filename_Length++;
			}
			// Ausgabe auf den News-Screen
			Print_News_Screen(NOTEXT,"File:",Filename);
		}
		if (Knoten->getName()==NUMBER3_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(NUMBER3_Taste_Text_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(NUMBER3_Taste_Trans);
			PressButton->schedule();
			if (Filename_Length!=Max_Filename_Length-1) {
				strcat(Filename,"3");
				Filename_Length++;
			}
			// Ausgabe auf den News-Screen
			Print_News_Screen(NOTEXT,"File:",Filename);
		}
		if (Knoten->getName()==NUMBER4_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(NUMBER4_Taste_Text_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(NUMBER4_Taste_Trans);
			PressButton->schedule();
			if (Filename_Length!=Max_Filename_Length-1) {
				strcat(Filename,"4");
				Filename_Length++;
			}
			// Ausgabe auf den News-Screen
			Print_News_Screen(NOTEXT,"File:",Filename);
		}
		if (Knoten->getName()==NUMBER5_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(NUMBER5_Taste_Text_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(NUMBER5_Taste_Trans);
			PressButton->schedule();
			if (Filename_Length!=Max_Filename_Length-1) {
				strcat(Filename,"5");
				Filename_Length++;
			}
			// Ausgabe auf den News-Screen
			Print_News_Screen(NOTEXT,"File:",Filename);
		}
		if (Knoten->getName()==NUMBER6_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(NUMBER6_Taste_Text_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(NUMBER6_Taste_Trans);
			PressButton->schedule();
			if (Filename_Length!=Max_Filename_Length-1) {
				strcat(Filename,"6");
				Filename_Length++;
			}
			// Ausgabe auf den News-Screen
			Print_News_Screen(NOTEXT,"File:",Filename);
		}
		if (Knoten->getName()==NUMBER7_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(NUMBER7_Taste_Text_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(NUMBER7_Taste_Trans);
			PressButton->schedule();
			if (Filename_Length!=Max_Filename_Length-1) {
				strcat(Filename,"7");
				Filename_Length++;
			}
			// Ausgabe auf den News-Screen
			Print_News_Screen(NOTEXT,"File:",Filename);
		}
		if (Knoten->getName()==NUMBER8_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(NUMBER8_Taste_Text_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(NUMBER8_Taste_Trans);
			PressButton->schedule();
			if (Filename_Length!=Max_Filename_Length-1) {
				strcat(Filename,"8");
				Filename_Length++;
			}
			// Ausgabe auf den News-Screen
			Print_News_Screen(NOTEXT,"File:",Filename);
		}
		if (Knoten->getName()==NUMBER9_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(NUMBER9_Taste_Text_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(NUMBER9_Taste_Trans);
			PressButton->schedule();
			if (Filename_Length!=Max_Filename_Length-1) {
				strcat(Filename,"9");
				Filename_Length++;
			}
			// Ausgabe auf den News-Screen
			Print_News_Screen(NOTEXT,"File:",Filename);
		}
		if (Knoten->getName()==NUMBER0_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(NUMBER0_Taste_Text_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(NUMBER0_Taste_Trans);
			PressButton->schedule();
			if (Filename_Length!=Max_Filename_Length-1) {
				strcat(Filename,"0");
				Filename_Length++;
			}
			// Ausgabe auf den News-Screen
			Print_News_Screen(NOTEXT,"File:",Filename);
		}
		if (Knoten->getName()==PLUS_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(PLUS_Taste_Text_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(PLUS_Taste_Trans);
			PressButton->schedule();
			if (Filename_Length!=Max_Filename_Length-1) {
				strcat(Filename,"+");
				Filename_Length++;
			}
			// Ausgabe auf den News-Screen
			Print_News_Screen(NOTEXT,"File:",Filename);
		}
		if (Knoten->getName()==DELETE_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(DELETE_Taste_Text_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(DELETE_Taste_Trans);
			PressButton->schedule();
			if (Filename_Length!=Min_Filename_Length-1) {
				Filename[Filename_Length]='\0';
				Filename_Length--;
			}
			// Ausgabe auf den News-Screen
			Print_News_Screen(NOTEXT,"File:",Filename);
		}
		if (Knoten->getName()==ENTER_Taste) {
			// Taste druecken
			Button_Color=(SoBaseColor*)SoNode::getByName(ENTER_Taste_Text_Farbe);
			Button_Trans=(SoTranslation*)SoNode::getByName(ENTER_Taste_Trans);
			PressButton->schedule();
			if (Load_Labyrinth()==TRUE) {
				Print_News_Screen(NOTEXT,"FILE LOADED",NOTEXT);
				// Filename neu initialisieren
				Filename[0]='\0';
				strcat(Filename,"laby");
				Filename_Length=Min_Filename_Length-1;
			}
			else {
				// Filename neu initialisieren
				Filename[0]='\0';
				strcat(Filename,"laby");
				Filename_Length=Min_Filename_Length-1;
				// Ausgabe auf den News-Screen
				Print_News_Screen("FILE NOT LOADED",NOTEXT,NOTEXT);
			}
		}
	}
	// Deselection aller Knoten im Terminal-Window
	SoSelection *Wurzel_Sel;
	Wurzel_Sel=(SoSelection*)SoNode::getByName(TermSelRoot);
	Wurzel_Sel->deselectAll();

	//cout << "MenuLayer=" << MenuLayer << endl << flush;
	//cout << "Aktion=" << Aktion << endl;
	//cout << "Aktion_Number=" << Aktion_Number << endl;

}

// Funktion zum Laden eines Labyrinthes
// Rueckgabe:
// TRUE File geladen
// FALSE File nicht geladen
bool Load_Labyrinth(void) {
	int Typ;
	bool Test;
	char Szenen_Filename[Max_Filename_Length];
	strcpy(Szenen_Filename,Filename);
	// Filenamen aufbereiten
	strcat(Szenen_Filename,".iv");
	// Separatorknoten des Szenensystems finden
	SoSeparator *Szenesystem;
	Szenesystem=(SoSeparator*)SoNode::getByName(SzenerieSeparator);
	// Zeiger in der Objekt-Liste auf den Anfang setzen
	Szene_Objects.Reset_Current_Object();
	if (Szene_Objects.Read_Current_Object_Typ(Typ)!=FALSE) {
		// Altes Labyrinth aus dem Haupt-Viewer loeschen
		Szenesystem->removeAllChildren();
		// Altes Labyrinth aus der Datenbank loeschen
		Szene_Objects.Delete_List();
	}
	SoSeparator *Szene=Read_File(Szenen_Filename);
	if (Szene==NULL) Test=FALSE;
	else {
		// Einlesen des Labyrinthes
		Szenesystem->addChild(Szene);
		// Erstellung der Objektdatenbank fuer das Labyrinth
		Szene_Objects.Read_Szene_Objects(Szene);
		Test=TRUE;
	}
	return (Test);
}

// Funktion zum Stoppen aller Automatischen-Such-Timer
void Stop_All_Search_Timer(void) {
	// Stoppen der automatischen Such-Timer
	if (Auto_Search_Modus<=PLEDGE_METHODE) {
		// Timer stoppen von Auswegsuche
		Search_Way_Out->unschedule();
	}
	if (Auto_Search_Modus==BUG_METHODE) {
		// Timer stoppen von Bug-Zielsuche
		BugSearch_Target->unschedule();
	}
	if (Auto_Search_Modus==CORNER_METHODE) {
		// Timer stoppen von Corner-Zielsuche
		CornerSearch_Target->unschedule();
	}
	// Stoppen der automatischen Suche
	Robot_Auto_Move=FALSE;
}

// Interrupt-Funktion zur Realisierung der automatischen Bewegung der Robotergelenke von einem
//  IST-Wert zu einem SOLL-Wert
void Move_To_New_Joints(void*,SoSensor*) {
	int Moved=0;
	for (int i=0;i<5;i++) {
		if (Joint_Steps[i]==0) {
			if (Theta[i]!=New_Theta[i]) Theta[i]=New_Theta[i];
			Moved++;
		}
		if (Joint_Steps[i]>0) {
			Theta[i]=Theta[i]+Step_Robot_Rot*Joint_Rotation_Direction[i];
			// Test ob Gelenkwinkelwerte zwischen 0 ... 2*M_PI liegen
			if (Theta[i]<0) Theta[i]=2*M_PI+Theta[i];
			if (Theta[i]>2*M_PI) Theta[i]=0.0+Theta[i]-2*M_PI;
			Joint_Steps[i]=Joint_Steps[i]-1;
		}
	}
	if (Moved==5) {
		// Interrupt-Funktion zur automatischen Bewegung der Robotergelenke stoppen
		To_New_Joints->unschedule();
		// keine automatischen Bewegung der Roboterarme mehr
		Robot_Arm_Move=FALSE;
	}
	if ((Moved==5)&&(What_Tool_Doit==TOOL_TAKE_TARGETOBJEKT)) {	
		// Zielobjekt aus Szene loeschen
		SoSeparator *Target_Sep;
		Target_Sep=(SoSeparator*)SoNode::getByName(ZielSystem);
		SoSphere *Target_Form;
		Target_Form=(SoSphere*)SoNode::getByName(Ziel);
		Target_Sep->removeChild(Target_Form);
		// Zielobjekt an Tool "heften"
		SoSeparator *Tool_Sep;
		Tool_Sep=(SoSeparator*)SoNode::getByName(ToolSystem);
		SoSphere *Tool_Objekt = new SoSphere;
		Tool_Objekt->radius.setValue(Ziel_Radius);
		SoTranslation *Tool_Objekt_Trans = new SoTranslation;
		Tool_Objekt_Trans->translation.setValue(0.0,0.0,Ziel_Radius);
		SoBaseColor *Tool_Objekt_Farbe = new SoBaseColor;
		Tool_Objekt_Farbe->rgb.setValue(BLAU);
		Tool_Sep->addChild(Tool_Objekt_Farbe);
		Tool_Sep->addChild(Tool_Objekt_Trans);
		Tool_Sep->addChild(Tool_Objekt);
		Reset_Robot_Joints();
		// ein Objekt befindet sich am Tool
		Tool_Busy=TRUE;
	}
	if ((Moved==5)&&(What_Tool_Doit==TOOL_TAKEOFF_TARGETOBJEKT)) {
		SbVec3f Point;
		// Zielobjekt am Tool "loeschen"
		SoSeparator *Tool_Sep;
		Tool_Sep=(SoSeparator*)SoNode::getByName(ToolSystem);
		Tool_Sep->removeAllChildren();
		Reset_Robot_Joints();
		// kein Objekt befindet sich am Tool
		Tool_Busy=FALSE;
	}
	Set_Joints();
}

// Funktion zum Start des automatischen bewegens der Robotergelenke in die Ausgansposition
void Reset_Robot_Joints(void) {
	// Tool soll nach erreichen der vorgegebenen Winkelposition nichts mehr tun
	What_Tool_Doit=TOOL_NOT_WORK;
	for (int i=0;i<5;i++) {
		New_Theta[i]=Theta_Start[i];
		// Drehrichtung der einzelnen Gelenke ermitteln
		Joint_Rotation_Direction[i]=Calculate_Rotation_Direction(Theta[i],New_Theta[i],Step_Joint,Joint_Steps[i]);
	}
	// automatischen Bewegung der Roboterarme
	Robot_Arm_Move=TRUE;
	// Interrupt-Funktion zur automatischen Bewegung der Robotergelenke in die Grundstellung starten
	To_New_Joints->setInterval(Time_Move_Joint);
	To_New_Joints->schedule();
}

// Funktion zum Start der Aufnahme/Ablage eines ZielObjektes (Kugelform)
// Rueckgabe:
//		TRUE Zielobjekt aufgenommen/abgelegt
//		FALSE Zielobjekt nicht aufgenommen/abgelegt
bool Robot_TakeONOFF_TargetObjekt(void) {
	double Punkt_Matrix[16];
	bool Aufgabe_Erledigt;
	if (What_Tool_Doit==TOOL_TAKE_TARGETOBJEKT) {
		Robot_Target.Z=2*Ziel_Radius;
		// Koordinaten und Matrix des Zielobjektes festlegen
		Set_Point_Matrix(Robot_Target,Punkt_Matrix);
	}
	if (What_Tool_Doit==TOOL_TAKEOFF_TARGETOBJEKT) {
		Point_3D TakeOff_Point;
		TakeOff_Point.X=Robot_Koord.X+Robot_Movement_Radius*cos(Robot_Direction);
		TakeOff_Point.Y=Robot_Koord.Y+Robot_Movement_Radius*sin(Robot_Direction);
		TakeOff_Point.Z=2*Ziel_Radius;
		Set_Point_Matrix(TakeOff_Point,Punkt_Matrix);
	}
	Aufgabe_Erledigt=Calculate_Tool_Point_IKP(Punkt_Matrix);
	if (Aufgabe_Erledigt) {
		if (What_Tool_Doit==TOOL_TAKE_TARGETOBJEKT) {
			// Zielobjekt in Szene einfuegen um es aufzunehmen
			SoSphere *Target_Form = new SoSphere;
			Target_Form->radius.setValue(Ziel_Radius);
			Target_Form->setName(Ziel);
			SoSeparator *Target_Sep;
			Target_Sep=(SoSeparator*)SoNode::getByName(ZielSystem);
			// Translation des Ziels setzen
			SoTranslation *Target_Trans;
			Target_Trans=(SoTranslation*)SoNode::getByName(ZielTranslation);
			Target_Trans->translation.setValue(Robot_Target.X,Robot_Target.Y,Ziel_Radius);
			// Kugel in Szene einfuegen
			Target_Sep->addChild(Target_Form);
		}
		// Ziel-Dragger in Laengsrichtung des Roboters versetzen 
		SoTranslate2Dragger *Target_Dragger;
		Target_Dragger=(SoTranslate2Dragger*)SoNode::getByName(TargetDragger);
		SbVec3f Target_Dragger_Point;
		float X,Y,Z;
		X=Robot_Koord.X+2*Robot_Movement_Radius*cos(Robot_Direction);
		Y=Robot_Koord.Y+2*Robot_Movement_Radius*sin(Robot_Direction);
		Z=0.0;
		Target_Dragger_Point.setValue(X,Y,Z);
		Target_Dragger->translation.setValue(Target_Dragger_Point);
		// neue Zielposition festlegen
		Robot_Target.X=X;
		Robot_Target.Y=Y;
		Robot_Target.Z=Z;
		// Drehrichtung der einzelnen Gelenke ermitteln
		for (int i=0;i<5;i++) Joint_Rotation_Direction[i]=Calculate_Rotation_Direction(Theta[i],New_Theta[i],Step_Joint,Joint_Steps[i]);
		// automatischen Bewegung der Roboterarme
		Robot_Arm_Move=TRUE;
		// Interrupt-Funktion zur automatischen Bewegung des Tools zum Zielobjekt starten
		To_New_Joints->setInterval(Time_Move_Joint);
		To_New_Joints->schedule();
	}
	return Aufgabe_Erledigt;
}

// Funktion zum umschalten der Kamera-Position
void Change_Camera_View(void) {
	SbVec3f Kamera_Pos;
	if (Robot_View==TRUE) {
		// alte Kameraposition und Richtung merken
		Kamera_Pos=Old_View_Camera_Pos;
		Main_Camera->orientation.setValue(Old_View_Camera_Ori);
		Robot_View=FALSE;
	}
	else {
		// alte Kameraposition und Richtung wiederherstellen
		Old_View_Camera_Pos=Main_Camera->position.getValue();
		Old_View_Camera_Ori=Main_Camera->orientation.getValue();
		// Kamera in der Mitte von Link 1 anbringen
		Kamera_Pos.setValue(Robot_Koord.Y,70,Robot_Koord.X);
		Main_Camera->orientation.setValue(SbVec3f(0.0,1.0,0.0),(float)Robot_Direction-M_PI);
		Robot_View=TRUE;
	}
	Main_Camera->position.setValue(Kamera_Pos);
}

// Funktion zur Beendigung des Programmes
void Program_Exit(void) {
	// Stoppen aller automatischen Such-Timer
	Stop_All_Search_Timer();
	// Stoppen der Terminal-Timer
	if (ScreenMoveDown1->isScheduled()==TRUE) ScreenMoveDown1->unschedule();
	if (ScreenMoveDown2->isScheduled()==TRUE) ScreenMoveDown2->unschedule();
	if (ScreenMoveUp1->isScheduled()==TRUE) ScreenMoveUp1->unschedule();
	if (ScreenMoveUp2->isScheduled()==TRUE) ScreenMoveUp2->unschedule();
	if (PressButton->isScheduled()==TRUE) PressButton->unschedule();
	// Deselection aller Knoten im Haupt-Window
	SoSelection *Wurzel_Sel;
	Wurzel_Sel=(SoSelection*)SoNode::getByName(MainSelRoot);
	Wurzel_Sel->deselectAll();
	// Deselection aller Knoten im Terminal-Window
	Wurzel_Sel=(SoSelection*)SoNode::getByName(TermSelRoot);
	Wurzel_Sel->deselectAll();
	// Loeschen des Terminal-Windows
	SoXt::hide(Main_Window);
	// Programm beenden
	exit(0);
}