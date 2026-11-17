#include <SPI.h>
#include <MFRC522.h>
 
#define RST_PIN   9
#define SS_PIN    10
#define LED 4

MFRC522 rc522(SS_PIN, RST_PIN);

int i=0;
int value = 0;
int down = 0;

void setup(){
  Serial.begin(9600);
  SPI.begin();
  rc522.PCD_Init();
  
  pinMode(LED, OUTPUT);
  pinMode(8, INPUT);
}

void loop(){
  value = digitalRead(8);
  if(value == 1){
    down = 1;
  }

  if(down == 0){
    digitalWrite(LED, LOW);
  }else if(down == 1){
    digitalWrite(LED, HIGH);
  }

  if ( !rc522.PICC_IsNewCardPresent() || !rc522.PICC_ReadCardSerial() ) { 
    //카드 또는 ID 가 읽히지 않으면 return을 통해 다시 시작하게 됩니다.
    delay(500);
    return;
  }
  
  

  Serial.print("Card UID:");
  
  for (byte i = 0; i < 4; i++) {
    Serial.print(rc522.uid.uidByte[i]);
    Serial.print(" ");
  }
  Serial.println(" ");

  if(rc522.uid.uidByte[0]==0x03 && rc522.uid.uidByte[1]==0xFD && rc522.uid.uidByte[2]==0x6C 
    && rc522.uid.uidByte[3]==0xA9) {  // 여기에 CARD UID 를 자신의 카드에 맞는 값으로 변경해주세요
    
    Serial.println("박세현 승객 탑승, 130번 버스");
    digitalWrite(LED, HIGH);
    delay(500);
  }
  else{
    Serial.println("<< WARNING !!! >>  This card is not registered");
    delay(500);
  }

}
