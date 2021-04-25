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

// Hauptprogramm

// Aufgaben:
//
// Selektionsevent in den Haupt-Szenegraph einfuegen
// Selektionsevent in den Terminal-Szenegraph einfuegen
// Tastaturevent in Haupt-Szenegraph einfuegen
// Erstellung des Grundgeruestes des Haupt-Szenegraphen
// Laden des Roboters
// Laden des Terminals
// Darstellung des Haupt-Szeneriegraphen
// Darstellung des Terminal-Szeneriegraphen
//
// Hauptinitialisirungsroutine
// Funktion zum Laden eines IV-Files
// Funktion zum Erzeugen von Zufallszahlen


// Header-Files

#include "main.h"
#include "objects.h"
#include "control.h"
#include "ability.h"


// Variablen

// Variable fuer das Window
Widget Main_Window;

// Variable zur Speicherung der Haupt-Fenster-Kamera
SoCamera *Main_Camera;

// Variable zur Speicherung der aktuellen Zufallszahl
double Current_Random_Number;

// Timer fuer den Zufallszahlengenerator
SoTimerSensor *RandomNumber;


// Hauptprogramm
void main (int, char **argv) {
	SoSeparator *Screen_Sep,*Screen;
	// Initialisierung des Haupt-Fensters
	Main_Window=SoXt::init(argv[0]);
	if (Main_Window == NULL) {
         cout << " Kein Haupt-Fenster oeffenbar !!! " << endl;
         exit (1);
	}

	// Wurzel des Haupt-Szenegraphen erzeugen
	SoSeparator *Main_Root = new SoSeparator;
	Main_Root->ref();
	// Haupt-Wurzel einen Namen geben
	Main_Root->setName(Haupt_Wurzelknoten);

	// Wurzel des Terminal-Szenegraphen erzeugen
	SoSeparator *Terminal_Root = new SoSeparator;
	Terminal_Root->ref();
	// Terminal-Wurzel einen Namen geben
	Terminal_Root->setName(Terminal_Wurzelknoten);

	// Haupt-Selection-Wurzel erzeugen
	SoSelection *Main_Sel_Root=new SoSelection;
	Main_Sel_Root->ref();
	Main_Sel_Root->policy=SoSelection::SINGLE;
	Main_Sel_Root->addSelectionCallback(Main_SelectionCB);
	// Haupt-Selection-Wurzel einen Namen geben
	Main_Sel_Root->setName(MainSelRoot);
 
	// Terminal-Selection-Wurzel erzeugen
	SoSelection *Term_Sel_Root=new SoSelection;
	Term_Sel_Root->ref();
	Term_Sel_Root->policy=SoSelection::SINGLE;
	Term_Sel_Root->addSelectionCallback(Terminal_SelectionCB);
	// Terminal-Selection-Wurzel einen Namen geben
	Term_Sel_Root->setName(TermSelRoot);
	
	// Haupt-Wurzel einfuegen
	Main_Sel_Root->addChild(Main_Root);

	// Terminal-Wurzel einfuegen
	Term_Sel_Root->addChild(Terminal_Root);

	// Haupt-Initialisierungsroutine
	main_init();

	// Definition und Einbinden der Tastenabfrage-Routine
	SoEventCallback *evcb=new SoEventCallback;
	evcb->addEventCallback(SoKeyboardEvent::getClassTypeId(),Key_Pressed,NULL);
	Main_Root->addChild(evcb);

	// Einlesen der Grundstruktur des Terminals
	SoSeparator *Terminal=Read_File(TERMINAL_Root);
	Terminal_Root->addChild(Terminal);
	// Einlesen des Manual-Screens
	Screen_Sep=(SoSeparator*)SoNode::getByName(Manual_Screen_Sep);
	Screen=Read_File(MANUAL_Screen_File);
	Screen_Sep->addChild(Screen);
	// Einlesen des Manual-Joint-Screens
	Screen_Sep=(SoSeparator*)SoNode::getByName(Manual_Joint_Screen_Sep);
	Screen=Read_File(MANUAL_JOINT_Screen_File);
	Screen_Sep->addChild(Screen);
	// Einlesen des Auto-Joint-Screens
	Screen_Sep=(SoSeparator*)SoNode::getByName(Auto_Joint_Screen_Sep);
	Screen=Read_File(AUTO_JOINT_Screen_File);
	Screen_Sep->addChild(Screen);
	// Einlesen des Auto-Robot-Screens
	Screen_Sep=(SoSeparator*)SoNode::getByName(Auto_Robot_Screen_Sep);
	Screen=Read_File(AUTO_ROBOT_Screen_File);
	Screen_Sep->addChild(Screen);
	// Einlesen des Load-Screens
	Screen_Sep=(SoSeparator*)SoNode::getByName(Load_Screen_Sep);
	Screen=Read_File(LOAD_Screen_File);
	Screen_Sep->addChild(Screen);

	// Ausrichtung des Koordinatensystems im Haupt-Fenster
	SoGroup *Koordsystem = new SoGroup;
	Main_Root->addChild(Koordsystem);
	SoRotation *X_Rot = new SoRotation;
	X_Rot->rotation.setValue(SbVec3f(-1.0,0.0,0.0),float(M_PI/2));
	Koordsystem->addChild(X_Rot);
	SoRotation *Z_Rot = new SoRotation;
	Z_Rot->rotation.setValue(SbVec3f(0.0,0.0,-1.0),float(M_PI/2));
	Koordsystem->addChild(Z_Rot);

	// Erzeugung der Grundszeneriem im Haupt-Fenster
	SoSeparator *Grundstruktur = new SoSeparator;
	Main_Root->addChild(Grundstruktur);
	SoBaseColor *Color_Bodenplatte = new SoBaseColor;
	Color_Bodenplatte->rgb.setValue(GRUEN);
	Grundstruktur->addChild(Color_Bodenplatte);
	SoTexture2 *Textur_Bodenplatte = new SoTexture2;
	Textur_Bodenplatte->filename.setValue("bluerock.rgb");
	Grundstruktur->addChild(Textur_Bodenplatte);
	SoTranslation *Boden_Trans = new SoTranslation;
	Boden_Trans->translation.setValue(0.0,0.0,-1.0);
	Grundstruktur->addChild(Boden_Trans);
	SoCube *Bodenplatte = new SoCube;
	Bodenplatte->depth.setValue(Z_Raum);
	Bodenplatte->width.setValue(Y_Raum);
	Bodenplatte->height.setValue(X_Raum);
	Grundstruktur->addChild(Bodenplatte);

	// Einfuegen einer Szenerie-Separators im Hauptfenster
	SoSeparator *Szenesystem = new SoSeparator;
	// Knoten einen Namen geben
	Szenesystem->setName(SzenerieSeparator);
	Main_Root->addChild(Szenesystem);

	// Erzeugung des Robotersystems im Hauptfenster
	SoSeparator *Robotsystem = new SoSeparator;
	// Knoten einen Namen geben
	Robotsystem->setName(RoboterSeparator);
	Main_Root->addChild(Robotsystem);
	SoTranslation *Robot_Trans = new SoTranslation;
	Robot_Trans->translation.setValue(Robot_Koord.X,Robot_Koord.Y,0.0);
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

	// Erzeugung des Ziel-Dragger-Systems im Hauptfenster
	// SoTranslate2Dragger erzeugen
	SoTranslate2Dragger *Target_Dragger = new SoTranslate2Dragger;
	Target_Dragger->setName(TargetDragger);
	Target_Dragger->translation.setValue(X_Raum/2,Y_Raum/2,0.0);
	// Target-Dragger aktivieren
	Target_Dragger->isActive.setValue(TRUE);
	// Geometrie des Target-Draggers festlegen
	Target_Dragger->setPart("translator",Build_Target());
	Target_Dragger->setPart("translatorActive",Build_Target_Aktive());
	// Dragger-Callback-Funktion initialisieren
	Target_Dragger->addMotionCallback(Read_Target_Dragger_Pos,Target_Dragger);
	// Dragger in Szene einfuegen
	Main_Root->addChild(Target_Dragger);	

	// Erzeugung des Zielobjektsystems im Hauptfenster
	SoSeparator *Targetsystem = new SoSeparator;
	// Knoten Namen geben
	Targetsystem->setName(ZielSystem);
	Main_Root->addChild(Targetsystem);
	// Farbe des Zielobjektes erzeugen
	SoBaseColor *Target_Color = new SoBaseColor;
	Target_Color->rgb.setValue(BLAU);
	Targetsystem->addChild(Target_Color);
	// Translation des Zielobjektes erzeugen
	SoTranslation *Target_Trans = new SoTranslation;
	// Knoten Namen geben
	Target_Trans->setName(ZielTranslation);
	Targetsystem->addChild(Target_Trans);

	// Haupt-Fenster anschalten und Haupt-Scenegraphen einbinden
	SoXtExaminerViewer *Main_Viewer = new SoXtExaminerViewer(Main_Window);
	Main_Viewer->setSize(SbVec2s(1000,600));	
	Main_Viewer->setSceneGraph(Main_Sel_Root);
	Main_Viewer->setTitle("Modellierung eines Beweglichen Autonomen RoboTers   B.A.R.T");
	Main_Viewer->setIconTitle("MAIN-B.A.R.T");
	// Lichtstaerke im Main-Viewer veranedern
	SoDirectionalLight* Main_Viewer_Light;
	Main_Viewer_Light=Main_Viewer->getHeadlight();
	Main_Viewer_Light->intensity.setValue(2);
	// Haupt-Fenster-Kamera initialisieren
	Main_Camera=Main_Viewer->getCamera();
	
	/*
	// Terminal-Fenster anschalten und Terminal-Scenegraphen einbinden
	SoXtPlaneViewer *Terminal_Viewer = new SoXtPlaneViewer;
	Terminal_Viewer->setSize(SbVec2s(800,300));
	Terminal_Viewer->setSceneGraph(Term_Sel_Root);
	Terminal_Viewer->setTitle("B.A.R.T's Terminal");
	Terminal_Viewer->setIconTitle("TERM-B.A.R.T");
	
	// Kamera des Terminal-Fensters einstellen
	SoCamera *Term_Camera;
	Term_Camera=Terminal_Viewer->getCamera();
	SbVec3f Term_Camera_Pos;
	Term_Camera_Pos=Term_Camera->position.getValue();
	float X,Y,Z;
	Term_Camera_Pos.getValue(X,Y,Z);
	Term_Camera_Pos.setValue(X,Y,Z/2.5);
	Term_Camera->position.setValue(Term_Camera_Pos);
	*/

	Main_Viewer->show();
	SoXt::show(Main_Window);
	//Terminal_Viewer->show();

	// Hauptschleife
	SoXt::mainLoop();
}


// Funktionen

// Haupt-Initialisierungsroutine
void main_init(void) {
	// Namen der Robotergelenke
	Gelenk_Name[0]=Gelenk1_Name;
	Gelenk_Name[1]=Gelenk2_Name;
	Gelenk_Name[2]=Gelenk3_Name;
	Gelenk_Name[3]=Gelenk4_Name;
	Gelenk_Name[4]=Gelenk5_Name;

	// Ausgangswinkel der Robotergelenke
	Theta[0]=Theta1_Start;
	Theta[1]=Theta2_Start;
	Theta[2]=Theta3_Start;
	Theta[3]=Theta4_Start;
	Theta[4]=Theta5_Start;
	// Grundstellung der Gelenke 
	Theta_Start[0]=Theta1_Start;
	Theta_Start[1]=Theta2_Start;
	Theta_Start[2]=Theta3_Start;
	Theta_Start[3]=Theta4_Start;
	Theta_Start[4]=Theta5_Start;

	// Ausgangspositionen des Roboters
	Robot_Koord.X=0;
	Robot_Koord.Y=0;

	// Ausgangsrichtung des Roboters
	Robot_Direction=M_PI/2;

	// Ausgangsposition des Ziels
	Robot_Target.X=X_Raum/2;
	Robot_Target.Y=Y_Raum/2;

	// Roboter nicht in Drehposition
	Robot_Rot=FALSE;

	// Timerfunktionen fuer Roboterfuss initialisieren
	Robot_Foot_InOut = new SoTimerSensor(Robotfoot_drive,NULL);

	// Laenge des Roboterfusses
	FootLeg_High=4;

	// Roboterfuss bewegt sich zu diesem Zeitpunkt nicht
	Robot_Foot_Move=FALSE;

	// Automatische Bewegung des Roboters nicht gestartet
	Robot_Auto_Move=FALSE;

	// Roboterarme bewegen sich nicht
	Robot_Arm_Move=FALSE;

	// kein Objekt am Tool "befestigt"
	Tool_Busy=FALSE;

	// Filename setzen
	Filename[0]='\0';

	// Menuebene 0
	MenuLayer=NOLAYER;

	// Aktionen
	Aktion=0;
	Aktion_Number=0;	

	// Taste gedrueckt ?
	KeyPressed=FALSE;

	// Hoch/Runterbewegung eines Screens
	// TRUE aktiv
	// FALSE nicht aktiv
	MovingDownScreen1=FALSE;
	MovingDownScreen2=FALSE;
	MovingUpScreen1=FALSE;
	MovingUpScreen2=FALSE;

	// nicht auf Roboter-Kamera umgeschaltet
	Robot_View=FALSE;

	// Interrupt-Funktion zum betaetigen einer Taste im Terminal-Window
	PressButton = new SoTimerSensor(Press_Button,NULL);
	PressButton->setInterval(Time_Press_Button);
	
	// Interrupt-Funktion zum Ausfahren eines Screens im Terminal-Window
	ScreenMoveDown1 = new SoTimerSensor(Screen_Move_Down1,NULL);
	ScreenMoveDown1->setInterval(Time_Move_Screen);	
	// weitere Interrupt-Funktion zum Ausfahren eines Screens im Terminal-Window
	ScreenMoveDown2 = new SoTimerSensor(Screen_Move_Down2,NULL);
	ScreenMoveDown2->setInterval(Time_Move_Screen);
	
	// Interrupt-Funktion zum Einfahren eines Screens im Terminal-Window
	ScreenMoveUp1 = new SoTimerSensor(Screen_Move_Up1,NULL);
	ScreenMoveUp1->setInterval(Time_Move_Screen);
	// weitere Interrupt-Funktion zum Einfahren eines Screens im Terminal-Window
	ScreenMoveUp2 = new SoTimerSensor(Screen_Move_Up2,NULL);
	ScreenMoveUp2->setInterval(Time_Move_Screen);
	
	// Interrupt-Funktion zur Erzeugung von Zufallszahlen
	RandomNumber = new SoTimerSensor(RandomNumber_Generator,NULL);
	// Zufallszahlengenartor initialisieren und starten
	RandomNumber->setInterval(RandomNumberCreateTime);
	RandomNumber->schedule();

	// Interrupt-Funktion zur Realisierung verschiedener automatischen
	// Methoden zur Auswegsuche aus einem Labyrinth
	Search_Way_Out = new SoTimerSensor(Robot_Search_Way_Out,NULL);
	// Interrupt-Funktion zur Realisierung der Bug-Methode
	//  zur Zielsuche in einem Labyrinth
	BugSearch_Target = new SoTimerSensor(Robot_BugSearch_Target,NULL);
	// Interrupt-Funktion zur Realisierung der Corner-Methode
	//  zur Zielsuche in einem Labyrinth
	CornerSearch_Target = new SoTimerSensor(Robot_CornerSearch_Target,NULL);
	// Interrupt-Funktion zur Realisierung der automatischen Bewegung der
	//  Roboterarme in eine vorgegebene Stellung
	To_New_Joints =new SoTimerSensor(Move_To_New_Joints,NULL);
}

// Routine zum Einlesen eines IV-Files
SoSeparator* Read_File(const char *File) {
	// File oeffnen
	SoInput mySceneInput;
	if (!mySceneInput.openFile(File)) return NULL;
	// Einlesen des geoeffneten Files
	SoSeparator *myGraph = SoDB::readAll(&mySceneInput);
	if (myGraph == NULL) return NULL;
	mySceneInput.closeFile();
	return myGraph;	
}

// Zufallszahlengenerator
// erzeugt Zufallszahlen zwischen 0 und 1
void RandomNumber_Generator(void*,SoSensor*) {
	double Zahl;
	Zahl=rand();
	Current_Random_Number=Zahl/32767;
}
