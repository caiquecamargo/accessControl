#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>

#define LED_ON HIGH
#define LED_OFF LOW

#define OPEN HIGH
#define CLOSE LOW

#define redLed 8
#define door 7

byte storedCard[4];
byte readCard[4];
byte masterCard[4];

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

void systemInit()
{
  Serial.println("Sistema iniciado");
  for (uint8_t i = 0; i < 3; i++)
  {
    digitalWrite(redLed, LED_ON);
    delay(100);
    digitalWrite(redLed, LED_ON);
    delay(100);
  }
}

void printReadCard()
{
  Serial.print(">>>>>> Cartão lido com sucesso com o endereço: ");
  Serial.print(readCard[0]);
  Serial.print(readCard[1]);
  Serial.print(readCard[2]);
  Serial.println(readCard[3]);
}

void printStoredCard()
{
  Serial.print(">>>>>> Cartão encontrado com sucesso com o endereço: ");
  Serial.print(storedCard[0]);
  Serial.print(storedCard[1]);
  Serial.print(storedCard[2]);
  Serial.println(storedCard[3]);
}

void printMasterCard()
{
  Serial.print(">>>>>> Cartão encontrado com sucesso com o endereço: ");
  Serial.print(masterCard[0]);
  Serial.print(masterCard[1]);
  Serial.print(masterCard[2]);
  Serial.println(masterCard[3]);
}

void flashLed(uint32_t time, uint8_t quantity, uint8_t chosenLed)
{
  for (uint8_t i = 0; i < quantity; i++)
  {
    digitalWrite(chosenLed, LED_ON);
    delay(time);
    digitalWrite(chosenLed, LED_OFF);
    delay(time);
  }
}

void notAMasterCard()
{
  Serial.println(">>>>>> Não é um cartão master.");
  flashLed(100, 3, redLed);
}

void operationError(String error)
{
  Serial.print(">>>>>> Erro ao realizar operação. ");
  Serial.println(error);
  flashLed(200, 3, redLed);
}

void successDelete()
{
  Serial.println(">>>>>> Cartão deletado com sucesso.");
  flashLed(100, 6, redLed);
}

void successWrite()
{
  Serial.println(">>>>>> Cartão gravado com sucesso.");
  flashLed(200, 3, redLed);
}

void accessGranted()
{
  Serial.println(">>>>>> Acesso garantido.");
  digitalWrite(door, OPEN);
  digitalWrite(redLed, LED_ON);
  delay(1000);
  digitalWrite(door, CLOSE);
  digitalWrite(redLed, LED_ON);
}

void accessDenied()
{
  Serial.println(">>>>>> Acesso negado.");
  digitalWrite(door, CLOSE);
  flashLed(100, 3, redLed);
}

bool compareTwoIDs(byte a[], byte b[])
{
  for (uint8_t i = 0; i < 4; i++)
    if (a[i] != b[i])
      return false;

  return true;
}

uint8_t successRead()
{
  if (!mfrc522.PICC_IsNewCardPresent())
    return 0;

  if (!mfrc522.PICC_ReadCardSerial())
    return 0;

  for (uint8_t i = 0; i < 4; i++)
    readCard[i] = mfrc522.uid.uidByte[i];

  printReadCard();

  mfrc522.PICC_HaltA();

  return 1;
}

bool isMaster(byte id[])
{
  return compareTwoIDs(id, masterCard);
}

void readID(uint8_t position)
{
  uint8_t start = (position * 4) + 2;
  for (uint8_t i = 0; i < 4; i++)
    storedCard[i] = EEPROM.read(start + i);
}

void printStore()
{
  Serial.print(">>>>>>>>> Número mágico: ");
  Serial.println(EEPROM.read(1));
  Serial.print(">>>>>>>>> Quantidade de cartões cadastrados: ");
  Serial.println(EEPROM.read(0));
  for (uint8_t i = 0; i < EEPROM.read(0); i++)
  {
    readID(i);
    Serial.print(">>>>>>>>> ");
    Serial.print(i);
    Serial.print(" => ");
    Serial.print(storedCard[0]);
    Serial.print(storedCard[1]);
    Serial.print(storedCard[2]);
    Serial.println(storedCard[3]);
  }
}

uint8_t findIDSlot(byte id[])
{
  uint8_t numberOfEntries = EEPROM.read(0);
  uint8_t i;
  for (i = 0; i < numberOfEntries; i++)
  {
    readID(i);
    if (compareTwoIDs(id, storedCard))
      return i;
  }

  return i;
}

bool findID(byte id[])
{
  uint8_t numberOfEntries = EEPROM.read(0);
  for (uint8_t i = 0; i < numberOfEntries; i++)
  {
    readID(i);
    if (compareTwoIDs(id, storedCard))
    {
      printStoredCard();
      return true;
    }
  }

  Serial.println(">>>>>> Cartão não encontrado.");
  return false;
}

void writeID(byte id[])
{
  //STORAGE A CARD ID
  Serial.println(">>>>>> Modo de gravação.");
  uint8_t numberOfEntries = EEPROM.read(0);
  uint8_t start = (numberOfEntries * 4) + 2;

  numberOfEntries++;
  EEPROM.write(0, numberOfEntries);

  for (uint8_t i = 0; i < 4; i++)
    EEPROM.write(start + i, id[i]);

  successWrite();
}

void deleteID(byte id[])
{
  Serial.println(">>>>>> Modo de exclusão.");
  if (isMaster(id))
  {
    //NOT REGISTERED CARD
    operationError("É um cartão master.");
  }
  else
  {
    //DELETING CARD
    uint8_t numberOfEntries = EEPROM.read(0);
    uint8_t slot = findIDSlot(id);
    Serial.print(">>>>>> SLOT encontrado: ");
    Serial.println(slot);

    if (slot < numberOfEntries)
    {
      uint8_t start = (slot * 4) + 2;
      uint8_t looping = ((numberOfEntries - slot) * 4);
      numberOfEntries--;
      EEPROM.write(0, numberOfEntries);

      uint8_t i;

      for (i = 0; i < looping; i++)
        EEPROM.write(start + i, EEPROM.read(start + 4 + i));

      for (uint8_t j = 0; j < 4; j++)
        EEPROM.write(start + i + j, 0);

      successDelete();
    }
  }
}

void accessProgramMode()
{
  Serial.println(">>>>>> Acessado modo de programação.");
  digitalWrite(redLed, LED_ON);
  bool waitingForACard = true;

  while (waitingForACard)
  {
    digitalWrite(redLed, LED_ON);
    if (successRead())
    {
      waitingForACard = false;
      (!findID(readCard)) ? writeID(readCard) : deleteID(readCard);
      printStore();
    }

    delay(200);
    digitalWrite(redLed, LED_OFF);
    delay(200);
  }
}

void setup()
{
  pinMode(redLed, OUTPUT);
  pinMode(door, OUTPUT);

  digitalWrite(redLed, LED_OFF);
  digitalWrite(door, CLOSE);

  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  Serial.println("INIT");

  //TO HARD_RESET ENABLE THIS LINE
  EEPROM.write(1, 0);

  //IF DON'T HAVE A MASTERCARD CADASTRED
  if (EEPROM.read(1) != 143)
  {
    Serial.println("Sem cartão master registrado");
    bool waitingForACard = true;
    digitalWrite(redLed, LED_ON);

    while (waitingForACard)
    {
      if (successRead())
      {
        waitingForACard = false;

        for (uint8_t i = 0; i < 4; i++)
          EEPROM.write(2 + i, readCard[i]);

        EEPROM.write(1, 143);
        EEPROM.write(0, 1);
      }
    }

    digitalWrite(redLed, LED_OFF);
    successWrite();
  }

  for (uint8_t i = 0; i < 4; i++)
    masterCard[i] = EEPROM.read(2 + i);

  printMasterCard();
  systemInit();
}

void loop()
{
  if (successRead())
  {
    if (isMaster(readCard))
      accessProgramMode();
    else
    {
      (findID(readCard)) ? accessGranted() : accessDenied();
    }
  }

  digitalWrite(redLed, LED_OFF);
}