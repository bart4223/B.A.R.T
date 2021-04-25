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

// Steuer-Header-File

// Aufgaben:
//
// Roboteraktionen definieren
// Gelenknamen des Roboters definieren
// Knotennamen zur Translation bzw. Rotation des Roboters definieren
// Armnamen des Roboters definieren
// Fussnamen des Roboters definieren
// Definition des Toolsystems
// Definition des Roboter-Dragger-Punktes
// Schrittweite der Gelenke definieren
// Schrittweite des Roboters definieren
// Drehweite des Roboters definieren
// Geschwindigkeit der Bewegung des Fusses definieren
// Geschwindigkeit der Bewegung der Gelenke
// Durchmesser des Ziels definieren
// Definition eines Faktors
// Definition des Namens fuer die Ziel-Translation
// Definition des Namens fuer das Ziel-System
// Definition des Namens fuer die Ziel-Form
// Definition des Namens des Ziel-Draggers
// Definition des Namens des Roboter-Draggers
// Definition der Target-Dragger-Files
// Definition der Robot-Dragger-Files
// Definition der Arbeit des Tools nach erreichen eines vorgegebenen Punktes
// Definition zur Laenge des Filenamens
// Definition zum bewegen von Screens im Terminal
// Ausgabetextdefinitionen
// Definition der Terminal-Screen-Files
// Definition der Menueebenen
// Definition der Knoten um Screens von unten/oben einzuschieben
// Tastendefinition des Hauptmenues
// Tastendefinition des Manual/Automenues
// Tastendefinition des Manual-Joint-Menues
// Tastendefinition des Auto-Robot-Menues
// Tastendefinition des Auto-Joint-Menues
// Tastendefinition des Load-Menus
// Definition der Terminal-Screen-Ausgabezeilen
// Definition von Bewegungsarten
//
// Grundstellung der Gelenke 
// Variable fuer Winkelwerte
// Variable fuer neue Winkelwerte der Gelenke
// Variable fuer die Rotationsrichtung der Gelenke
// Variable fuer die Anzahl der Schritte um vom IST-Gelenkwinkel zum SOLL-Gelenkwinkel zu gelangen
// Variablen fuer Roboter-Koordinaten
// Variable fuer Roboter-Richtung
// Variable zur Erkennung ob Roboter in Drehposition
// Feld zur Zuweisung der Gelenknamen zu den Rotationsknoten
// Variable fuer Hoehe des Roboter-Fusses
// Variable zur Anzeige der automatischen Bewegung des Roboter-Fusses
// Variable zur Anzeige der automatischen Bewegung der Roboterarme
// Timervariable fuer die automatische Bewegung des Roboterfusses
// Timervariable fuer die automatische Bewegung der Robotergelenke
//  von einem IST-Wert zu einem SOLL-Wert
// Variable um die Arbeit des Tools nach erreichen eines vorgegebenen Punktes festzulegen
// Objekt am Tool ?
// Variablen zum hochfahren von Screens
// Variablen zum runterfahren von Screens
// Variable zum betaetigen einer Taste
// Variablen zur Erkennung der Menuesituation
// Variable zur Erkennung der gewaehlten Aktion
// Variable zur weiteren Abstufung der Aktion
// Variable zur Unterscheidung der Bewegungsart
// Variable fuer den Labyrinth-Filenamen
// Variable zur Erkennung ob auf die Roboter-Kamera umgeschalten ist
// Variablen zur Speicherung der Kamera-Position
//
// Tastatur-Interrupt-Routine
// Funktion zum setzen der Gelenkwinkel des Roboters
// Funktion zum vorhergehenden testen der neuen Armstellung
// Funktion zum berechnen der neuen Armstellung
// Funktion zur Sichtbarmachung der neuen Armstellung
// Funktion zur Berechnung der neuen Roboterkoordinaten
// Funktion zur Sichtbarmachung der neuen Roboterposition
// Interrupt-Funktion fuer den Roboterfuss
// Funktion zur Selektionsbehandlung von Objekten im Haupt-Window
// Funktion zur Formveraenderung des Robot-Draggers im nicht aktiven Zustand 
// Funktion zur Formveraenderung des Robot-Draggers im aktiven Zustand
// Funktion zum Auslesen der Robot-Dragger Position und Wiederherstellung des Robotersystems 
// Funktion zur Formveraenderung des Target-Draggers im nicht aktiven Zustand 
// Funktion zur Formveraenderung des Target-Draggers im aktiven Zustand 
// Funktion zum Auslesen und abspeichern des "gedraggerten" Zieles
// Funktion zur Formveraenderung des Robot-Draggers im nicht aktiven Zustand 
// Funktion zur Formveraenderung des Robot-Draggers im aktiven Zustand 
// Funktion zum Auslesen und abspeichern des "gedraggerten" Roboters
// Funktion zur Selektionsbehandlung von Terminal-Objekten
// Funktion zum runterfahren eines Screens im Terminal-Windows
// Funktion zum hochfahren eines Screens im Terminal-Windows
// Funktion zum betaetigen einer Taste im Terminal-Window
// Ausgabe am News-Screen im Terminal-Windows
// Funktion zum Laden eines Labyrinthes
// Funktion zum Stoppen aller Automatischen-Such-Timer
// Interrupt-Funktion zur Realisierung der automatischen Bewegung der Robotergelenke von einem
//  IST-Wert zu einem SOLL-Wert 
// Funktion zum Start der automatischen bewegen der Robotergelenke in die Ausgansposition
// Funktion zum Start der automatischen bewegen der Robotergelenke zum Zielobjekt und dessen Aufnahme/Ablage
// Funktion zum umschalten der Kamera-Position
// Funktion zur Beendigung des Programmes


// Definitionen

// Definition von Roboteraktionen
#define ROTATION 0
#define TRANSLATION_FORWARD 1
#define TRANSLATION_BACK 2

// Definition von Gelenkbezeichnungen
#define Gelenk1_Name "Gelenk_Theta1"
#define Gelenk2_Name "Gelenk_Theta2"
#define Gelenk3_Name "Gelenk_Theta3"
#define Gelenk4_Name "Gelenk_Theta4"
#define Gelenk5_Name "Gelenk_Theta5"

// Definition des Knotennamens zur Translation des Roboters
#define RoboterTranslation "Robot_Trans"

// Definition des Knotennamens zur Rotation des Roboters
#define RoboterRotation "Robot_Rotation"

// Definition von Armbezeichnungen
#define Arm1_Rot "Arm1_Rot"
#define Arm2_Length "Arm2_Length"
#define Arm2_Trans "Arm2_Trans"

// Definition von Fussbezeichnungen
#define Fuss_Trans "Fuss_Trans"
#define Fussbein_Trans "Fussbein_Trans"
#define Fussbein_Groesse "Fussbein"

// Definition des Toolsystems
#define ToolSystem "TOOL_SYSTEM"

// Definition des Roboter-Dragger-Punktes
#define DragRobot "ROBOT_DRAGGER"

// Schrittweite der Gelenke 
#define Step_Joint 0.05

// Schrittweite des Roboters
#define Step_Robot_Trans 2

// Drehweite des Roboters
#define Step_Robot_Rot 0.05

// Geschwindigkeit der Bewegung des Fusses
#define Time_Move_Foot 0.03

// Geschwindigkeit der Bewegung der Gelenke
#define Time_Move_Joint 0.03

// Definition des Ziel-Radius
#define Ziel_Radius 5

// Definition eines Faktors
#define Faktor 3 

// Definition des Namens fuer die Ziel-Translation
#define ZielTranslation "Target_Translation"

// Definition des Namens fuer das Ziel-System
#define ZielSystem "Target_Separator"

// Definition des Namens fuer die Ziel-Form
#define Ziel "Target_Form"

// Definition des Namens des Ziel-Draggers
#define TargetDragger "TARGET_DRAGGER"

// Definition des Namens des Roboter-Draggers
#define RobotDragger "ROBOT_DRAGGER"

// Definition der Target-Dragger-Files
#define TARGET_Dragger_File "target_dragger.iv"

// Definition der Robot-Dragger-Files
#define ROBOT_Dragger_File "robot_dragger.iv"

// Definition der Arbeit des Tools nach erreichen eines vorgegebenen Punktes
#define TOOL_NOT_WORK 0
#define TOOL_TAKE_TARGETOBJEKT 1
#define TOOL_TAKEOFF_TARGETOBJEKT 2

// Definition der maximalen Laenge des Filenamens
#define Max_Filename_Length 10
// Definition der minimalen Laenge des Filenamens
#define Min_Filename_Length 4

// Definition zum bewegen von Screens im Terminal
#define Time_Move_Screen 0.04
#define Time_Press_Button 0.04
#define Time_Hold_Button 0.5
#define Step_Move_Screen 6.0
#define Up_Screen 59.0
#define Down_Screen -59.0
#define Full_Screen 0.0

// Ausgabetextdefinitionen
#define NOTEXT ""
#define OLDTEXT "$$$$$$$$$$"

// Definition der Terminal-Screen-Files
#define MANUAL_Screen_File "screen_manual.iv"
#define MANUAL_JOINT_Screen_File "screen_manual_joint.iv"
#define AUTO_Screen_File "screen_auto.iv"
#define AUTO_ROBOT_Screen_File "screen_auto_robot.iv"
#define AUTO_JOINT_Screen_File "screen_auto_joint.iv"
#define LOAD_Screen_File "screen_load.iv"

// Definition der Menueebenen
#define NOLAYER 0
#define MANUAL 1
#define MANUAL_JOINT 2
#define AUTO 3
#define AUTO_ROBOT 4
#define AUTO_JOINT 5
#define LOAD 6

// Definition der Knoten um Screens von unten/oben einzuschieben
#define Manual_Screen_Sep "MANUAL_SCREEN_SEP"
#define Manual_Screen_Trans "MANUAL_SCREEN_TRANS"
#define Manual_Joint_Screen_Sep "MANUAL_JOINT_SCREEN_SEP"
#define Manual_Joint_Screen_Trans "MANUAL_JOINT_SCREEN_TRANS"
#define Auto_Joint_Screen_Sep "AUTO_JOINT_SCREEN_SEP"
#define Auto_Joint_Screen_Trans "AUTO_JOINT_SCREEN_TRANS"
#define Auto_Robot_Screen_Sep "AUTO_ROBOT_SCREEN_SEP"
#define Auto_Robot_Screen_Trans "AUTO_ROBOT_SCREEN_TRANS"
#define Load_Screen_Sep "LOAD_SCREEN_SEP"
#define Load_Screen_Trans "LOAD_SCREEN_TRANS"

// Tastendefinition des Hauptmenues
#define MANUAL_Taste "MANUAL_KEY"
#define MANUAL_Taste_Farbe "MANUAL_KEY_COLOR"
#define MANUAL_Taste_Trans "MANUAL_KEY_TRANS"

#define AUTO_Taste "AUTO_KEY"
#define AUTO_Taste_Farbe "AUTO_KEY_COLOR"
#define AUTO_Taste_Trans "AUTO_KEY_TRANS"

#define LOAD_Taste "LOAD_KEY"
#define LOAD_Taste_Farbe "LOAD_KEY_COLOR"
#define LOAD_Taste_Trans "LOAD_KEY_TRANS"

#define BACK_Taste "BACK_KEY"
#define BACK_Taste_Farbe "BACK_KEY_COLOR"
#define BACK_Taste_Trans "BACK_KEY_TRANS"

#define VIEW_Taste "VIEW_KEY"
#define VIEW_Taste_Farbe "VIEW_KEY_COLOR"
#define VIEW_Taste_Trans "VIEW_KEY_TRANS"

#define STOP_Taste "STOP_KEY"
#define STOP_Taste_Farbe "STOP_KEY_COLOR"
#define STOP_Taste_Trans "STOP_KEY_TRANS"

#define EXIT_Taste "EXIT_KEY"
#define EXIT_Taste_Farbe "EXIT_KEY_COLOR"
#define EXIT_Taste_Trans "EXIT_KEY_TRANS"

// Tastendefinition des Manual/Automenues
#define ROBOT_Taste "ROBOT_KEY"
#define ROBOT_Taste_Text_Farbe "ROBOT_KEYTYPE_COLOR"
#define ROBOT_Taste_Trans "ROBOT_KEY_TRANS"

#define JOINT_Taste "JOINT_KEY"
#define JOINT_Taste_Text_Farbe "JOINT_KEYTYPE_COLOR"
#define JOINT_Taste_Trans "JOINT_KEY_TRANS"

// Tastendefinition des Manual-Joint-Menues
#define JOINT1_Taste "JOINT1_KEY"
#define JOINT1_Taste_Text_Farbe "JOINT1_KEYTYPE_COLOR"
#define JOINT1_Taste_Trans "JOINT1_KEY_TRANS"

#define JOINT2_Taste "JOINT2_KEY"
#define JOINT2_Taste_Text_Farbe "JOINT2_KEYTYPE_COLOR"
#define JOINT2_Taste_Trans "JOINT2_KEY_TRANS"

#define JOINT3_Taste "JOINT3_KEY"
#define JOINT3_Taste_Text_Farbe "JOINT3_KEYTYPE_COLOR"
#define JOINT3_Taste_Trans "JOINT3_KEY_TRANS"

#define JOINT4_Taste "JOINT4_KEY"
#define JOINT4_Taste_Text_Farbe "JOINT4_KEYTYPE_COLOR"
#define JOINT4_Taste_Trans "JOINT4_KEY_TRANS"

#define JOINT5_Taste "JOINT5_KEY"
#define JOINT5_Taste_Text_Farbe "JOINT5_KEYTYPE_COLOR"
#define JOINT5_Taste_Trans "JOINT5_KEY_TRANS"

// Tastendefinition des Auto-Robot-Menues
#define SIMPLE_Taste "SIMPLE_KEY"
#define SIMPLE_Taste_Text_Farbe "SIMPLE_KEYTYPE_COLOR"
#define SIMPLE_Taste_Trans "SIMPLE_KEY_TRANS"

#define MODULO_Taste "MODULO_KEY"
#define MODULO_Taste_Text_Farbe "MODULO_KEYTYPE_COLOR"
#define MODULO_Taste_Trans "MODULO_KEY_TRANS"

#define PLEDGE_Taste "PLEDGE_KEY"
#define PLEDGE_Taste_Text_Farbe "PLEDGE_KEYTYPE_COLOR"
#define PLEDGE_Taste_Trans "PLEDGE_KEY_TRANS"

#define BUG_Taste "BUG_KEY"
#define BUG_Taste_Text_Farbe "BUG_KEYTYPE_COLOR"
#define BUG_Taste_Trans "BUG_KEY_TRANS"

#define CORNER_Taste "CORNER_KEY"
#define CORNER_Taste_Text_Farbe "CORNER_KEYTYPE_COLOR"
#define CORNER_Taste_Trans "CORNER_KEY_TRANS"

// Tastendefinition des Auto-Joint-Menues
#define PICKUP_Taste "PICKUP_KEY"
#define PICKUP_Taste_Text_Farbe "PICKUP_KEYTYPE_COLOR"
#define PICKUP_Taste_Trans "PICKUP_KEY_TRANS"

#define TAKEOFF_Taste "TAKEOFF_KEY"
#define TAKEOFF_Taste_Text_Farbe "TAKEOFF_KEYTYPE_COLOR"
#define TAKEOFF_Taste_Trans "TAKEOFF_KEY_TRANS"

#define STARTPOS_Taste "STARTPOS_KEY"
#define STARTPOS_Taste_Text_Farbe "STARTPOS_KEYTYPE_COLOR"
#define STARTPOS_Taste_Trans "STARTPOS_KEY_TRANS"

// Tastendefinition des Load-Menus
#define NUMBER1_Taste "NUMBER1_KEY"
#define NUMBER1_Taste_Text_Farbe "NUMBER1_KEYTYPE_COLOR"
#define NUMBER1_Taste_Trans "NUMBER1_KEY_TRANS"

#define NUMBER2_Taste "NUMBER2_KEY"
#define NUMBER2_Taste_Text_Farbe "NUMBER2_KEYTYPE_COLOR"
#define NUMBER2_Taste_Trans "NUMBER2_KEY_TRANS"

#define NUMBER3_Taste "NUMBER3_KEY"
#define NUMBER3_Taste_Text_Farbe "NUMBER3_KEYTYPE_COLOR"
#define NUMBER3_Taste_Trans "NUMBER3_KEY_TRANS"

#define NUMBER4_Taste "NUMBER4_KEY"
#define NUMBER4_Taste_Text_Farbe "NUMBER4_KEYTYPE_COLOR"
#define NUMBER4_Taste_Trans "NUMBER4_KEY_TRANS"

#define NUMBER5_Taste "NUMBER5_KEY"
#define NUMBER5_Taste_Text_Farbe "NUMBER5_KEYTYPE_COLOR"
#define NUMBER5_Taste_Trans "NUMBER5_KEY_TRANS"

#define NUMBER6_Taste "NUMBER6_KEY"
#define NUMBER6_Taste_Text_Farbe "NUMBER6_KEYTYPE_COLOR"
#define NUMBER6_Taste_Trans "NUMBER6_KEY_TRANS"

#define NUMBER7_Taste "NUMBER7_KEY"
#define NUMBER7_Taste_Text_Farbe "NUMBER7_KEYTYPE_COLOR"
#define NUMBER7_Taste_Trans "NUMBER7_KEY_TRANS"

#define NUMBER8_Taste "NUMBER8_KEY"
#define NUMBER8_Taste_Text_Farbe "NUMBER8_KEYTYPE_COLOR"
#define NUMBER8_Taste_Trans "NUMBER8_KEY_TRANS"

#define NUMBER9_Taste "NUMBER9_KEY"
#define NUMBER9_Taste_Text_Farbe "NUMBER9_KEYTYPE_COLOR"
#define NUMBER9_Taste_Trans "NUMBER9_KEY_TRANS"

#define NUMBER0_Taste "NUMBER0_KEY"
#define NUMBER0_Taste_Text_Farbe "NUMBER0_KEYTYPE_COLOR"
#define NUMBER0_Taste_Trans "NUMBER0_KEY_TRANS"

#define PLUS_Taste "PLUS_KEY"
#define PLUS_Taste_Text_Farbe "PLUS_KEYTYPE_COLOR"
#define PLUS_Taste_Trans "PLUS_KEY_TRANS"

#define DELETE_Taste "DELETE_KEY"
#define DELETE_Taste_Text_Farbe "DELETE_KEYTYPE_COLOR"
#define DELETE_Taste_Trans "DELETE_KEY_TRANS"

#define ENTER_Taste "ENTER_KEY"
#define ENTER_Taste_Text_Farbe "ENTER_KEYTYPE_COLOR"
#define ENTER_Taste_Trans "ENTER_KEY_TRANS"

// Definition der Terminal-Screen-Ausgabezeilen
#define AKTION_Zeile "AKTION_LINE"
#define INFO_Zeile "INFO_LINE"
#define ALARM_Zeile "ALARM_LINE"

// Definition von Bewegungsarten
#define NO_MOVEMENT 0
#define MANUAL_MOVEMENT 1
#define AUTO_MOVEMENT 2


// Globale Variablen

// Grundstellung der Gelenke 
extern double Theta_Start[5];

// Winkelwert der Gelenke
extern double Theta[5];

// neue Winkelwerte der Gelenke
extern double New_Theta[5];

// Rotationsrichtung der Gelenke
extern int Joint_Rotation_Direction[5];

// Anzahl der Schritte um vom IST-Gelenkwinkel zum SOLL-Gelenkwinkel zu gelangen
extern int Joint_Steps[5];

// Koordinaten des Roboters
extern Point_3D Robot_Koord;

// aktuelle Richtung des Roboters
extern double Robot_Direction;

// Erkennung ob Roboter in Drehposition
// TRUE Roboter in Drehposition
// FALSE Roboter nicht in Drehposition
extern bool Robot_Rot;

// Gelenknamen den Rotationsknoten zuweisen
extern char* Gelenk_Name[5];

// Hoehe des Roboter-Fusses
extern double FootLeg_High;

// Anzeige der automatischen Bewegung des Roboter-Fusses 
extern bool Robot_Foot_Move;

// Anzeige der automatischen Bewegung der Roboterarme
extern bool Robot_Arm_Move;

// Timervariable fuer die automatische Bewegung des Roboterfusses
extern SoTimerSensor *Robot_Foot_InOut;

/// Interruptvariable zur Realisierung der automatischen Bewegung der Robotergelenke von einem
//  IST-Wert zu einem SOLL-Wert
extern SoTimerSensor *To_New_Joints;

// Variable um die Arbeit des Tools nach erreichen eines vorgegebenen Punktes festzulegen
//		TOOL_NOT_WORK		Tool macht nichts mehr
//		TOOL_TAKE_TARGET	Tool nimmt das Zielobjekt auf und geht in die Grundstellung
//		TOOL_TAKEOFF_Target Tool legt das Zielobjekt vor sich ab
extern int What_Tool_Doit;

// Objekt am Tool ?
extern bool Tool_Busy;

// Variablen zum hochfahren von Screens
extern SoTimerSensor *ScreenMoveUp1;
extern SoTranslation *Screen_Trans_Up1;
extern SoSeparator *Screen_Sep_Up1;
extern bool MovingUpScreen1;
extern float Screen_Finish_Point_Up1;

extern SoTimerSensor *ScreenMoveUp2;
extern SoTranslation *Screen_Trans_Up2;
extern SoSeparator *Screen_Sep_Up2;
extern bool MovingUpScreen2;
extern float Screen_Finish_Point_Up2;

// Variablen zum runterfahren von Screens
extern SoTimerSensor *ScreenMoveDown1;
extern SoTranslation *Screen_Trans_Down1;
extern SoSeparator *Screen_Sep_Down1;
extern bool MovingDownScreen1;
extern float Screen_Finish_Point_Down1;

extern SoTimerSensor *ScreenMoveDown2;
extern SoTranslation *Screen_Trans_Down2;
extern SoSeparator *Screen_Sep_Down2;
extern bool MovingDownScreen2;
extern float Screen_Finish_Point_Down2;

// Variable zum betaetigen einer Taste
extern SoTimerSensor *PressButton;
extern SoBaseColor *Button_Color;
extern SoTranslation *Button_Trans;
extern bool KeyPressed;

// Variablen zur Erkennung der Menuesituation
// Menueschichten:
//		NOLAYER 0
//		MANUAL 1
//		MANUAL_JOINT 2
//		AUTO 3
//		AUTO_ROBOT 4
//		AUTO_JOINT 5
//		LOAD 6
extern int MenuLayer;

// Variable zur Erkennung der gewaehlten Aktion: !!! IMMER NUR 1 BIT GESETZT !!!
//		Bit 0	MANUAL-JOINT
//		Bit 1	MANUAL-ROBOT
//		Bit 2	AUTO-JOINT
//		Bit 3	AUTO-ROBOT
extern int Aktion;

// Variable zur weiteren Abstufung der Aktion
extern int Aktion_Number;

// Variable unterscheidet:
//		NO_MOVEMENT = keine Bewegung
//		MANUAL_MOVEMENT = Manuelle Bewegung
//		AUTO_MOVEMENT = automatische Bewegung
extern int Bewegungsart;

// Variable fuer den Labyrinth-Filenamen
extern char Filename[Max_Filename_Length];
extern int Filename_Length;

// Variable zur Erkennung ob auf die Roboter-Kamera umgeschalten ist
extern bool Robot_View;

// Variablen zur Speicherung der Kamera-Position
extern SbVec3f Old_View_Camera_Pos;
extern SbRotation Old_View_Camera_Ori;

// Funktionen

// Deklaration der Tastatur-Interruptroutine
extern void Key_Pressed(void*,SoEventCallback*);

// Funktion zum setzen der Gelenkwinkel des Roboters
extern void Set_Joints(void);

// Funktion zum vorhergehenden testen der neuen Armstellung
// Eingabe:
//		Nummer des Gelenkwinkels
//		neuer Wert des Gelenkwinkels
// Rueckgabe:
//		TRUE Armstellung moeglich
//		FALSE Armstellung nicht moeglich
extern bool Test_Calculate_Arm_Parameters(int,double);

// Funktion zur Berechnung der neuen Armstellung
// Eingabe:
//		Gelenkwinkel 3
// Ausgabe: 
//		Winkel Eta
//		Strecke Line_a
extern void Calculate_Arm_Parameters(double,double&,double&);

// Funktion zur Sichtbarmachung der neuen Armstellung
extern void Motion_Arm1u2(void);

// Funktion zur Berechnung der neuen Koordinaten des Roboters
//
// Eingabe: Robot_Aktion
//			Zur Erkennung der Bewegung die der Roboter ausfuehren soll
//			0 Rotation links oder rechts
//			1 Translation vor
//			2 Translation zurËck
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
extern bool Calculate_Motion_Robot(int,int&,int&);

// Funktion zur Sichtbarmachung der neuen Roboterposition
// Eingabe: FALSE Roboter nicht drehen
//			TRUE Roboter drehen
extern void Motion_Robot(bool);

// Interupt-Funktion zum ein/ausfahren des Rotationsfusses
extern void Robotfoot_drive(void*,SoSensor*);

// Funktion zur Selektionsbehandlung von Objekten im Haupt-Window
extern void Main_SelectionCB(void *,SoPath *);

// Funktion zur Formveraenderung des Roboter-Draggers im nicht aktiven Zustand 
extern SoSeparator *Build_Robot(void);

// Funktion zur Formveraenderung des Roboter-Draggers im aktiven Zustand 
extern SoSeparator *Build_Robot_Aktive(void);

// Funktion zum Auslesen der Robot-Dragger Position und Wiederherstellung des Robotersystems
extern void Read_Robot_Dragger_Pos(void *,SoDragger *);

// Funktion zur Formveraenderung des Ziel-Draggers im nicht aktiven Zustand 
extern SoSeparator *Build_Target(void);

// Funktion zur Formveraenderung des Ziel-Draggers im aktiven Zustand 
extern SoSeparator *Build_Target_Aktive(void);

// Funktion zum Auslesen der Ziel-Dragger Position
extern void Read_Target_Dragger_Pos(void *,SoDragger *);

// Funktion zur Selektionsbehandlung von Terminal-Objekten
extern void Terminal_SelectionCB(void *,SoPath *);

// Funktion zum runterfahren eines Screens im Terminal-Windows
extern void Screen_Move_Down1(void*,SoSensor*);

// weitere Funktion zum runterfahren eines Screens im Terminal-Windows
extern void Screen_Move_Down2(void*,SoSensor*);

// Funktion zum hochfahren eines Screens im Terminal-Windows
extern void Screen_Move_Up1(void*,SoSensor*);

// weitere Funktion zum hochfahren eines Screens im Terminal-Windows
extern void Screen_Move_Up2(void*,SoSensor*);

// Funktion zum betaetigen einer Taste im Terminal-Window
extern void Press_Button(void*,SoSensor*);

// Ausgabe am News-Screen im Terminal-Windows
extern void Print_News_Screen(char[],char[],char[]);

// Funktion zum Laden eines Labyrinthes
extern bool Load_Labyrinth(void);

// Funktion zum Stoppen aller Automatischen-Such-Timer
extern void Stop_All_Search_Timer(void);

// Interrupt-Funktion zur Realisierung der automatischen Bewegung der Robotergelenke von einem
//  IST-Wert zu einem SOLL-Wert
extern void Move_To_New_Joints(void*,SoSensor*);

// Funktion zum Start des automatischen bewegens der Robotergelenke in die Ausgansposition
extern void Reset_Robot_Joints(void);

// Funktion zum Start der Aufnahme/Ablage eines Zielobjektes
// Rueckgabe:
//		TRUE Zielobjekt aufgenommen/abgelegt
//		FALSE Zielobjekt nicht aufgenommen/abgelegt	
extern bool Robot_TakeONOFF_TargetObjekt(void);

// Funktion zum umschalten der Kamera-Position
extern void Change_Camera_View(void);

// Funktion zur Beendigung des Programmes
extern void Program_Exit(void);