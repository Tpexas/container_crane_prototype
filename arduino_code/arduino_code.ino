#include <AccelStepper.h>

//pinai
#define stepPinG 32 //PUL G rankenos
#define dirPinG 33 //DIR G rankenos
#define stepPinY 34 //PUL Y lynas
#define dirPinY 35 //DIR Y lynas
#define stepPinX 36 //PUL X ratukai
#define dirPinX 37 //DIR X ratukai
int endstopL = 38; //Left endstop
int endstopR = 39; //Right endstop
int VRx = A0; //joystickas
int VRy = A1;
int SW = 7;

int xPosition = 0;
int yPosition = 0;
int SW_state = 0;
int mapX = 0;
int mapY = 0;
int buvo_joystick = 0;
int joystickPositionToSpeed = 2.56;
int paiimtas = 1;
int ilgis = 0;
int stopMotors = 0;

long x_kordinates = 0;
long y_kordinates = 0;


#define Xlimitas 30000 //- i prieki
#define Ylimitas 10000 //-10000 i apacia
#define RankenosLimitas 600

bool paimtas = false;
bool spausdinti = true;

AccelStepper stepperX(1, stepPinX,dirPinX);
AccelStepper stepperG(1, stepPinG,dirPinG);
AccelStepper stepperY(1, stepPinY,dirPinY);


void setup()
{  
  Serial.begin(9600); 
  pinMode(endstopL, INPUT);
  pinMode(endstopR, INPUT);
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP);
  
    stepperX.setMaxSpeed(1000);
    stepperX.setSpeed(500);
    stepperX.setAcceleration(500);

    stepperY.setMaxSpeed(600);
    stepperY.setSpeed(400);
    stepperY.setAcceleration(200);

    stepperG.setMaxSpeed(300);
    stepperG.setAcceleration(300);

    Nusinulinti();

    Serial.println("Pasirinkite valdyma \n Valdymas komandomis = C, joystiku = J");
}

void Vaziuoti(long xkoord)
{
  Serial.println("Vaziuoja i: ");
  Serial.println(xkoord);
  stepperX.moveTo(xkoord);
  while(true)
  {
      if(stepperX.distanceToGo() != 0)
      {
        stepperX.run();
          if((stepperX.currentPosition() > 300 || stepperX.currentPosition() < -300)  && (digitalRead(endstopL) == 1 || digitalRead(endstopR) == 1))
          {
            stepperX.setCurrentPosition(0);
            break;
          }
      }
      else
      {
        break;
      }
  }
}

void Sukti_trosa(long ykoord)
{
  Serial.println("Sukamas trosas: ");
  Serial.println(ykoord);
   stepperY.moveTo(ykoord);
     while(true)
     {
       if(stepperY.distanceToGo() != 0)
       {
        stepperY.run();
       }
       else
       {
        stepperY.setCurrentPosition(0);
        break;
       }
     }
}

void Sukti_rankenas(long gkoord)
{
  Serial.println("Sukasi rankenos: ");
  Serial.println(gkoord);
  stepperG.moveTo(gkoord);
    while(true)
    {
      if(stepperG.distanceToGo() != 0)
      {
      stepperG.run();
      }
      else
      {
        stepperG.setCurrentPosition(0);
        break;
      }
    }
}

void Nusinulinti()
{
  Serial.println("Pradedamas Y nulinimas");
  Sukti_trosa(Ylimitas);
    if(digitalRead(endstopR) != 1)
    {
    Serial.println("Pradedamas X nulinimas");
    Vaziuoti(Xlimitas);
    }
}
void loop()
{
  if (Serial.available() > 0)
  {
    char y = Serial.read();
    while(y == 'C')
    {
       if(spausdinti==true)
       {
        Serial.println("Valdymas komandomis");
        Serial.println("Vaziuoti X asyje = V, sukti trosa = S, paimti rankenomis = P, atleisti rankenas = A, pakeisti valdyma = B");
       }
        spausdinti = false;
        char x = Serial.read();
        if(x == 'V')
        {
          x_kordinates = Serial.parseFloat();
          Vaziuoti(x_kordinates);
          stepperX.setCurrentPosition(0);
          exit;
        }
        if(x == 'S')
        {
          y_kordinates = Serial.parseFloat();
          Sukti_trosa(y_kordinates);
          stepperY.setCurrentPosition(0);
          exit;
        }
        if(x == 'A')
        {
          Sukti_rankenas(RankenosLimitas);
          stepperY.setCurrentPosition(0);
          exit;
        }
        if(x == 'P')
        {
          Sukti_rankenas(-RankenosLimitas);
          stepperY.setCurrentPosition(0);
          exit;
        }
        if(x == 'B')
        {
          Serial.println("Valdymas komandomis = C, joystiku = J");
          spausdinti = true;
          break;
        }
    }

    while(y == 'J')
    {
      if(spausdinti==true)
       {
        Serial.println("Valdymas  joystiku \n Pakeisti valdyma = B");
       }
        spausdinti = false;
        
          xPosition = analogRead(VRx);
          yPosition = analogRead(VRy);
          SW_state = digitalRead(SW);
          mapX = map(xPosition, 0, 1023, -512, 512);
          mapY = map(yPosition, 0, 1023, -512, 512);
 
  // --- Joystick funkcijos ----------------------------------------------
          if ((mapX > 50) && (digitalRead(endstopR) != 1)) {
            xPosition = analogRead(VRx);
            mapX = map(xPosition, 0, 1023, -512, 512);
            stepperX.setSpeed(abs(mapX * joystickPositionToSpeed));
            buvo_joystick = 1;
          }
          if ((mapX < -50) && (digitalRead(endstopL) != 1)) {
            xPosition = analogRead(VRx);
            mapX = map(xPosition, 0, 1023, -512, 512);
            stepperX.setSpeed((mapX * joystickPositionToSpeed));
            buvo_joystick = 1;
          }
          if (mapY < -50) {
            yPosition = analogRead(VRy);
            mapY = map(yPosition, 0, 1023, -512, 512);
            stepperY.setSpeed(abs(mapY * joystickPositionToSpeed));
            buvo_joystick = 1;
          }
          if (mapY > 50) {
            yPosition = analogRead(VRy);
            mapY = map(yPosition, 0, 1023, -512, 512);
            stepperY.setSpeed(-(mapY * joystickPositionToSpeed));
            buvo_joystick = 1;
          }
          if (((SW_state) != 1) && (paiimtas == 1)) {
            stepperG.moveTo(0);
            while (SW_state == 0) {
              SW_state = digitalRead(SW);
            }
            paiimtas = 0;
            buvo_joystick  = 1;
          }
          if ((SW_state != 1) && (paiimtas == 0)) {
            stepperG.moveTo(RankenosLimitas);
            while (SW_state == 0) {
              SW_state = digitalRead(SW);
            }
            paiimtas = 1;
            buvo_joystick = 1;
          }
            if ( (stopMotors != 1)) {
              stepperX.run();
            }
            stepperY.run();
            stepperG.run();
            if (buvo_joystick == 1) {
              stepperX.runSpeed();
              stepperY.runSpeed();
              stepperX.setSpeed(0);
              stepperY.setSpeed(0);
            }
            char x = Serial.read();
            if(x == 'B')
             {
              Serial.println("Valdymas komandomis = C, joystiku = J");
              spausdinti = true;
              stepperX.setCurrentPosition(0);
              stepperY.setCurrentPosition(0);
              stepperG.setCurrentPosition(0);
              break;
             }
    }
  }
}
