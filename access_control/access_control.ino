#include <SPI.h>
#include <MFRC522.h>
 
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

#define LEN_UID 9
String validUID[] = {
    " 79 3A DB A2", 
    " 17 E6 58 0F", //CAIQUE
    " 87 5E EF 32", //BOMBÃO
    " 3D 01 49 9C", //EDUARDO
    " 5B 5C 64 39", //GG
    " 33 F1 EB DF", //SERGIÂO
    " 62 0D D7 D3", //THIAGO
    " 6B 42 76 39", //VELHO
    " A3 A6 50 DF" //DANIELZINHO
  };

#define PORT_TRIGGER 9
#define LED 8
 
void setup() 
{
  Serial.begin( 9600 );   // Inicia a serial
  SPI.begin();      // Inicia  SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522

  pinMode( PORT_TRIGGER, OUTPUT );
  pinMode( LED, OUTPUT );
  digitalWrite( PORT_TRIGGER, LOW );

  digitalWrite( LED, HIGH );
  delay( 300 );
  digitalWrite( LED, LOW );
  
  Serial.println( "Aproxime o seu cartao do leitor..." );
  Serial.println();
}

int isValidUID( String uid ){
  for ( int i = 0; i < LEN_UID; i++ )
     if ( validUID[i] == uid ) return 1;

  return 0;
}

void notValidUID(){
  for( int i = 0; i < 2; i++ ){
    digitalWrite( LED, HIGH );
    delay ( 300 );
    digitalWrite( LED, LOW );
    delay ( 300 );
  }
  
}

void openTheDoor(){
  Serial.println( "Porta Aberta" );
  digitalWrite( PORT_TRIGGER, HIGH );
  digitalWrite( LED, HIGH );
  delay( 1000 );
}

void closeTheDoor(){
  digitalWrite( PORT_TRIGGER, LOW );
  digitalWrite( LED, LOW );
}

String readUID(){
  Serial.print( "UID da tag :" );
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid.concat( String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ") );
    uid.concat( String(mfrc522.uid.uidByte[i], HEX) );
  }

  uid.toUpperCase();
  return uid;
}
 
void loop() 
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) return;
  
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) return;
  
  String uid  = readUID();
  Serial.println( uid );
  if ( isValidUID( uid ) ) openTheDoor();
  else notValidUID();
  closeTheDoor();
} 
