#include "painlessMesh.h"

#define   MESH_PREFIX     "TheBees"
#define   MESH_PASSWORD   "password"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

const int trigPin1 = 33;
const int echoPin1 = 25;

const int trigPin2 = 35;
const int echoPin2 = 32;

int isOccupied = 0;
int newHigh = 0;
int newLow = 0;
int difference = 0;

long duration, distance, BackSensor,FrontSensor;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() {
  String msg = "Hello from node ";
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}


void setup()
{
  Serial.begin (9600);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  
  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
}

void loop() {
  SonarSensor(trigPin1, echoPin1);
  FrontSensor = distance;
  SonarSensor(trigPin2, echoPin2);
  BackSensor = distance;

  //Serial.println(FrontSensor);
  //Serial.println(BackSensor);
  //Serial.println("");
  delay(100);

  difference = FrontSensor - BackSensor;

  if (difference > newHigh)
  {
    newHigh = difference;
  }
  if (difference < newLow)
  {
    newLow = difference;
  }
  
  /*Serial.print("new high = ");
  Serial.println(newHigh);
  Serial.print("new low = ");
  Serial.println(newLow); */
  //Serial.println(difference);
  //delay(100); 

 
  if (difference < -20 )
  {
    isOccupied = 1;
    Serial.println("Forward IN");
    Serial.println("");
    delay(2000);
  }
  else if (difference > 20)
  {
    isOccupied = 1;
    Serial.println("Backward OUT");
    Serial.println("");
    delay(2000);
  }
  if (isOccupied == 1 && ((FrontSensor - BackSensor < 10) || (BackSensor - FrontSensor < 10)))
  {
    isOccupied = 0;
    Serial.println("RESET"); 
    Serial.println("");
  } 


  mesh.update();

}

void SonarSensor(int trigPin,int echoPin)
{
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
duration = pulseIn(echoPin, HIGH);
distance = (duration/2) / 29.1;

}