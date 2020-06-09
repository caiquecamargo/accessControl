#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
 
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

#define LEN_UID 2
String *validUID;

#define PORT_TRIGGER 9
 
void setup() 
{
  Serial.begin( 9600 );   // Inicia a serial
  SPI.begin();      // Inicia  SPI bus
  
  mfrc522.PCD_Init();   // Inicia MFRC522
  createValidUIDTable();

  pinMode( PORT_TRIGGER, OUTPUT );
  digitalWrite( PORT_TRIGGER, LOW );
  
  Serial.println( "Aproxime o seu cartao do leitor..." );
  Serial.println();
}

void createValidUIDTable(){
  String validUID_temp[] = {
    "79 3A DB A2",
    "79 3A DB A3"
  };
  validUID = validUID_temp;
}

int isValidUID( String uid ){
  for ( int i = 0; i < LEN_UID; i++ ){
     if ( validUID[i] == uid ) return 1;
  }

  return 0;
}

void openTheDoor(){
  Serial.println( "Porta Aberta" );
  digitalWrite( PORT_TRIGGER, HIGH );
}

void closeTheDoor(){
  digitalWrite( PORT_TRIGGER, LOW );
}
 
void loop() 
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  
  //Mostra UID na serial
  Serial.print( "UID da tag :" );
  String uid= "";
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     uid.concat( String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ") );
     //Serial.print(i);
     //Serial.print(" ");
     //Serial.println(conteudo);
     uid.concat( String(mfrc522.uid.uidByte[i], HEX) );
     //Serial.print(i);
     //Serial.print(" ");
     //Serial.println(conteudo);
  }
  uid.toUpperCase();
  Serial.println( uid );
  if ( isValidUID( uid ) ){
    openTheDoor();
    delay(1000);
    closeTheDoor();
  }
} 
