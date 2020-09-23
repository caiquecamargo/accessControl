#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

#define LEN_UID 46
String validUID[] = {
    /*TAGS DO BOMBÃO
    " E7 F8 63 7B", " 57 F2 80 7A", " 17 DB 6F 7B", " 97 40 8A 7B", " 97 FF 66 7B",
    " F7 EC 7A 7A", " 67 B7 78 7A", " E7 50 6A 7B", " 27 AC 70 7A", " 47 6B 7D 7B",
    " 27 D2 52 7A", " 67 CE 66 7B", " 97 E8 66 7B", " A7 8A 7C 7A", " 47 35 71 7B",
    " 17 8C 8B 7B", " D7 13 70 7A", " 97 F4 63 7B", " E7 C6 78 7A", " 27 F6 6E 7B",
    " 67 D9 54 7B", " 97 9F 52 7B", " C7 D9 80 7B", " 57 FA 64 7B", " F7 46 66 7B",
    " 67 CD 50 7B", " B7 84 66 7B", " 67 18 7B 7A", " E7 2A 73 7B", " 67 55 85 7B",
    " D7 6D 76 7A", " 47 1B 7B 7B", " D7 A8 73 7A", " 27 A7 80 7B", " A7 E9 80 7A",*/
    " E7 2A 7B 7B", " 57 27 7B 7B", " F7 A1 50 7A", " 97 0A 74 7A", " C7 7A 6C 7B",
    " A7 F8 82 7A", " 37 4F 7E 7B", " D7 9D 80 7B", " 47 38 6A 7B", " 67 2B 73 7B",
    " D7 25 81 7A", " B7 F5 82 7A", " 07 2B 6A 7B", " 67 5F 4F 7B", " D7 98 70 7A",
    /**/
    //BS CORRETIVA
    " 17 E6 58 0F", //CAIQUE
    " 87 5E EF 32", //BOMBÃO
    " 63 C5 22 E0", //LUQUITA
    " 72 45 D5 D3", //KOZIOL
    " 62 0D D7 D3", //THIAGO
    " 6B 42 76 39", //VELHO
    " 83 FA F5 DF", //DIGUERA
    " 22 7B D7 D3", //MARIO
    " 35 48 3F AD", //PRATES
    " 17 E6 58 0F",
    " 17 E6 58 0F",
    " 17 E6 58 0F",

    //BS PREVENTIVA
    " 3D 01 49 9C", //EDUARDO
    " 5B 5C 64 39", //GG
    " 33 F1 EB DF", //SERGIÂO
    " 27 FE 6D 0F", //MONTOIA
    " 9A D2 E9 41", //JAILSON
    " 37 1A 30 0F", //MICHEL
    " 63 CD 55 DF", //RAONI
    " D3 95 10 E0", //FINUS
    " D3 75 4A DF", //MURILO
    " A3 A6 50 DF", //DANIELZINHO
    " 3A EB 15 24", //MASSAO
    " AA EB 29 18", //BETINHO
    " 43 5E 45 E0", //JAMILE
    " 17 E6 58 0F",
    " 17 E6 58 0F",
    " 17 E6 58 0F",
    " 17 E6 58 0F",
    " 17 E6 58 0F",

    " 43 9E 1F E0" //NELSON
};

#define PORT_TRIGGER 7
#define LED 8

void setup()
{
  //Serial.begin( 9600 );   // Inicia a serial
  SPI.begin();        // Inicia  SPI bus
  mfrc522.PCD_Init(); // Inicia MFRC522

  pinMode(PORT_TRIGGER, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(PORT_TRIGGER, LOW);

  digitalWrite(LED, HIGH);
  delay(300);
  digitalWrite(LED, LOW);

  //Serial.println( "Aproxime o seu cartao do leitor..." );
  //Serial.println();
}

void printUID()
{
  for (int i = 0; i < LEN_UID; i++)
  {
    Serial.print(i);
    Serial.print(" ");
    Serial.println(validUID[i]);
  }
}

int isValidUID(String uid)
{
  //printUID();
  for (int i = 0; i < LEN_UID; i++)
    if (validUID[i] == uid)
      return 1;

  return 0;
}

void notValidUID()
{
  for (int i = 0; i < 2; i++)
  {
    digitalWrite(LED, HIGH);
    delay(300);
    digitalWrite(LED, LOW);
    delay(300);
  }
}

void openTheDoor()
{
  //Serial.println( "Porta Aberta" );
  digitalWrite(PORT_TRIGGER, HIGH);
  digitalWrite(LED, HIGH);
  delay(1000);
}

void closeTheDoor()
{
  digitalWrite(PORT_TRIGGER, LOW);
  digitalWrite(LED, LOW);
}

String readUID()
{
  //Serial.print( "UID da TAG:" );
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    uid.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    uid.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  uid.toUpperCase();
  return uid;
}

void loop()
{
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent())
    return;

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
    return;

  String uid = readUID();
  //Serial.println( uid );
  if (isValidUID(uid))
    openTheDoor();
  else
    notValidUID();
  closeTheDoor();
}