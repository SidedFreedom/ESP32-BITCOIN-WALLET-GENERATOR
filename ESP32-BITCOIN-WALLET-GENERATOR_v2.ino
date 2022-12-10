//source: https://github.com/SidedFreedom/ESP32-BITCOIN-WALLET-GENERATOR

//esp32 only
//Many thanks to Limor Fried/LadyAda, ricmoo, stepansnigirev and my anonymous friend for their information and help

//Obviously you need to install libraries like ricmoo's qrcode, uBitcoin, SdFat (not the forks), and various Adafruit libraries for this to work


#include "bootloader_random.h"
#include <Bitcoin.h>
#include <qrcoderm.h> //CRITICAL see instructions for renaming files here or QR code will not work https://github.com/ricmoo/QRCode/issues/35#issuecomment-1179311130
#include <SPI.h>
#include <SdFat.h>
SdFat SD;
File myFile;

float ONE;
float TWO;
float THREE;
float FOUR;

#include "Adafruit_Thermal.h" //CRITICAL edit this file to increase the heat of the printer. My settings: setHeatConfig(uint8_t dots=11, uint8_t time=200, uint8_t interval=240)

Adafruit_Thermal printer(&Serial1);

#include "Adafruit_ThinkInk.h"
#define SD_CS       14   // Reference https://forums.adafruit.com/viewtopic.php?f=22&t=171897
#define EPD_DC      33 // 
#define EPD_CS      15  // 
#define EPD_BUSY    -1  // 
#define SRAM_CS     32  // 
#define EPD_RESET   -1  // 

#define BUTTONA 27
#define BUTTONB 12
#define BUTTONC 13

ThinkInk_290_Grayscale4_T5 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);
#define DEBUG

#define COLOR1 EPD_BLACK
#define COLOR2 EPD_LIGHT


//////////////////////////////////////////////////////////
int calc_buffer_size(QRCode *qrcode, int scale) {

  int size = qrcode->version * 4 + 17;
  return ((size * scale + 7) / 8) * size * scale;
}
byte x;
byte y;
QRCode qrcode;
uint8_t blockValue = qrcode_getModule(&qrcode, x, y);

void buffer_qr(QRCode *qrcode, uint8_t buffer[], int scale)
{
  int size = qrcode->version * 4 + 17;
  int gridByteSize = (size * scale + 7) / 8;
  int gridSize = size * scale;

  int pixelNum=0, byteNum, x, y, b, i;

  for(byteNum=y=0; y<gridSize; y++) { // Each row...
    for(x=0; x<gridByteSize; x++) { // Each 8-pixel block within row...
      b = 0;
      for(i=0; i < 8; i++) {// each bit in byte
        b = b << 1;
        b += qrcode_getModule(qrcode, (x * 8 + i) / scale, y / scale);
      }
      buffer[byteNum] = b;
      byteNum++;
    }
  }
}
//////////////////////////////////////////////////////////////////////////////


PrivateKey pk;
PublicKey pubk;
bool no_wallet_created_so_far = true;
String pkstr = "", pkwif = "", pubkstr = "";
const char* PRIVATE;
const char* PUBLIC;
const char* PHR;

const char* P1;
const char* P2;
const char* P3;
const char* P4;

String PB1;
String PB2;
String PB3;
String PB4;


const char* PBL1;
const char* PBL2;
const char* PBL3;
const char* PBL4;
const char* XXPUB;

String pu1;
String pu2;
String pu3;
String pu4;
String phra;
String phrase;
String ROOOT;
String finger;
String BWxpub;
String pubx;

int RAND1;
int RANDO;



uint8_t generate_random_8bit()
{

    
    RANDO = esp_random();
//    ONE = esp_random();
//    TWO = esp_random();
//    THREE = esp_random();
//    FOUR = esp_random();
//
//    RAND1 = random(1,8);
//    if (RAND1 == 1)
//    {RANDO = int(ONE/TWO);};
//    if (RAND1 == 2)
//    {RANDO = int(ONE*TWO);};
//    if (RAND1 == 3)
//    {RANDO = int(TWO/FOUR);};
//    if (RAND1 == 4)
//    {RANDO = int(THREE+ONE);};
//    if (RAND1 == 5)
//    {RANDO = int(THREE*ONE);};
//    if (RAND1 == 6)
//    {RANDO = int(TWO+ONE);};
//    if (RAND1 == 7)
//    {RANDO = int(FOUR+TWO);};
//    if (RAND1 == 8)
//    {RANDO = int(TWO-THREE);};
//    
    //add in EMF entropy
    return (uint8_t)RANDO;
}

void testdrawtext(const char *text, uint16_t color) {
  display.setCursor(0, 0);
  display.setTextColor(color);
  display.setTextWrap(true);
  display.print(text);
}

int countdown = 250; //2 minutes

/////////////////////////////////////////////////////////////////////////////////////////////
// generate private key and public address
void generate_wallet()
{
  const uint8_t pklen = 32;
    uint8_t pkbytes[pklen];
    
    bootloader_random_enable();
    for (uint8_t i = 0; i < pklen; i++)
    {
        pkbytes[i] = generate_random_8bit();
    }
    bootloader_random_disable();

    pkstr = "";
    for (uint8_t i = 0; i < pklen; i++)
    {
        if (pkbytes[i] <= 0x0f)
        {
            pkstr += "0";
        }
        pkstr += String(pkbytes[i], HEX);
    }
  
  
  String entropy = pkstr;
  phrase = generateMnemonic(12, entropy);
  Serial.println(phrase);

phra = String("Private Recovery Phrase:\n\n")+phrase;
PHR = phra.c_str();

// using default empty password
HDPrivateKey root(phrase, "");
ROOOT = root;
finger = root.fingerprint();
HDPrivateKey account = root.derive("m/84'/0'/0'");
Serial.println(root);
Serial.println(account);
Serial.println(account.xpub());


HDPublicKey xpub = account.xpub();
pubx = account.xpub();
Serial.println(pubx);
BWxpub = "["+finger+"/84h/0h/0h"+"]"+pubx;
XXPUB = BWxpub.c_str();
Serial.println(XXPUB);

HDPublicKey pub1;
HDPublicKey pub2;
HDPublicKey pub3;
HDPublicKey pub4;

pub1 = xpub.child(0).child(1);
Serial.println(pub1.address());

pub2 = xpub.child(0).child(2);
Serial.println(pub2.address());

pub3 = xpub.child(0).child(3);
Serial.println(pub3.address());

pub4 = xpub.child(0).child(0);
Serial.println(pub4.address());

pu1 = String("Public Address 1:\n\n")+String(pub1.address());
pu2 = String("Public Address 2:\n\n")+String(pub2.address());
pu3 = String("Public Address 3:\n\n")+String(pub3.address());
pu4 = String("Public Address 0:\n\n")+String(pub4.address());

P1 = pu1.c_str();
P2 = pu2.c_str();
P3 = pu3.c_str();
P4 = pu4.c_str();

PB1 = pub1.address();
PBL1 = PB1.c_str();

PB2 = pub2.address();
PBL2 = PB2.c_str();

PB3 = pub3.address();
PBL3 = PB3.c_str();

PB4 = pub4.address();
PBL4 = PB4.c_str();

////////////eInk text print//////////////////
  display.begin(THINKINK_MONO);

  display.clearBuffer();
  display.setTextSize(1);
  testdrawtext("IN CHRIST ALONE WE TRUST\n\nLIBERTOPIA BITCOIN WALLET GENERATOR v2.1\n\n\n\n\nPlease wait 1 minute for options to recall \naddresses & recovery phrase as generator \nattempts to print\n\n\nWill also attempt to write wallet info to SD card", COLOR1);
  display.display();

/////////////qrcode initialization/////////////////
QRCode qrcode;
uint8_t qrVersion = 5;
uint8_t qrErrorLevel = ECC_MEDIUM;
byte qrcodeBytes[qrcode_getBufferSize(qrVersion)];

  
////////////////////////////////////////
//////// XPUB /////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////
printer.justify('L');
printer.println(F("XPUB: "));
printer.feed(1);
printer.printf(XXPUB);
printer.feed(1);
printer.printf("---------------------------");
printer.feed(1);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////




  
////////////////////////////////////////
//////// PUBLIC ADDRESS 1 /////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////

qrcode_initText(&qrcode, qrcodeBytes, qrVersion, qrErrorLevel, PBL1);

printer.justify('L');
printer.println(F("IN CHRIST ALONE WE TRUST"));
printer.feed(1);
printer.justify('L');
printer.println(F("Public Address 1: "));
printer.feed(1);
printer.printf(PBL1);

////////////////////////////////////////////////////////////////////
// try scale=7, try lower values if you run out of memory
uint8_t scale=5;
int buffer_size = calc_buffer_size(&qrcode, scale);
int width = ((qrcode.version * 4 + 17) * scale);
uint8_t pixels[buffer_size];

// for each qr code use this code to print it - use the same pixel buffer to save memory
buffer_qr(&qrcode, pixels, scale);
printer.feed(1);
printer.printBitmap(width, width, pixels);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////
//////// PUBLIC ADDRESS 2/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////

  qrcode_initText(&qrcode, qrcodeBytes, qrVersion, qrErrorLevel, PBL2);


printer.feed(3);
//printer.justify('L');
//printer.println(F("IN CHRIST ALONE WE TRUST"));
//
//printer.feed(1);
printer.justify('L');
printer.println(F("Public Address 2: "));
printer.feed(1);
printer.printf(PBL2);

//////////////////////////////////////////////////////////////////

// try scale=5, try lower values if you run out of memory
scale=5;
buffer_size = calc_buffer_size(&qrcode, scale);
width = ((qrcode.version * 4 + 17) * scale);
pixels[buffer_size];

// for each qr code use this code to print it - use the same pixel buffer to save memory
buffer_qr(&qrcode, pixels, scale);
printer.feed(1);
printer.printBitmap(width, width, pixels);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////
//////// PUBLIC ADDRESS 3 /////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////

  qrcode_initText(&qrcode, qrcodeBytes, qrVersion, qrErrorLevel, PBL3);


printer.feed(3);
//printer.justify('L');
//printer.println(F("IN CHRIST ALONE WE TRUST"));
//
//printer.feed(1);
printer.justify('L');
printer.println(F("Public Address 3: "));
printer.feed(1);
printer.printf(PBL3);

//////////////////////////////////////////////////////////////////

// try scale=5, try lower values if you run out of memory
scale=5;
buffer_size = calc_buffer_size(&qrcode, scale);
width = ((qrcode.version * 4 + 17) * scale);
pixels[buffer_size];

// for each qr code use this code to print it - use the same pixel buffer to save memory
buffer_qr(&qrcode, pixels, scale);
printer.feed(1);
printer.printBitmap(width, width, pixels);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////




printer.feed(1);
printer.justify('L');
printer.printf("---------------------------");
printer.feed(1);
printer.printf(PHR);
printer.feed(1);
  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////BLUE WALLET FOOTER
printer.justify('L');
printer.printf("---------------------------");
printer.feed(1);
printer.printf("Import Recovery Phrase to");
printer.feed(1);
printer.printf("BlueWallet for ease of use:");
printer.feed(1);
printer.printf("https://bluewallet.io");
printer.feed(1);
qrcode_initText(&qrcode, qrcodeBytes, qrVersion, qrErrorLevel, "https://bluewallet.io/");
printer.feed(1);
printer.printf("Step 1) Go to create new wallet; click but don't create yet");
printer.feed(1);
printer.printf("Step 2) Scroll down & click");
printer.feed(1);
printer.printf("      IMPORT");
printer.feed(1);
printer.printf("Step 3) Enter recovery phrase & continue. Enjoy!");
//////////////////////////////////////////////////////////////////

// try scale=5, try lower values if you run out of memory
scale=5;
buffer_size = calc_buffer_size(&qrcode, scale);
width = ((qrcode.version * 4 + 17) * scale);
pixels[buffer_size];

// for each qr code use this code to print it - use the same pixel buffer to save memory
buffer_qr(&qrcode, pixels, scale);
printer.feed(1);
printer.printBitmap(width, width, pixels);
printer.feed(3);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  
  display.clearBuffer();
  display.setTextSize(1);
  testdrawtext("IN CHRIST ALONE WE TRUST\n\nLIBERTOPIA BITCOIN WALLET GENERATOR v2.1\n\n\nPress button near SD slot to generate wallet.\n\n1) Press Top Left Button for Address 0 \n\n2) Press Top Middle Button for XPUB \n\n3) Press Top Right Button for Recovery Phrase \n& Optional SD Card Backup\n\n\n If SD Fat card present, it now has wallet info.", COLOR1);
  display.display();
  
  }



void setup()
{
//
  Serial.begin(115200);  
  Serial1.begin(9600);  // Initialize HardwareSerial for Printer
  delay(100);
  printer.begin();        // Init printer (same regardless of serial type)
  delay(100);
  
//////////////////////////////////////////
  if(!SD.begin(SD_CS, SD_SCK_MHZ(16))){Serial.println("SD did not initialize.");}; //Check for SD Fat format, not Fat32. If formatted correctly, you may need to reseat the SD Card.
//////////////////////////////////////////
  delay(100);
  generate_wallet();
  delay(100);
  pinMode(BUTTONA, INPUT_PULLUP);
  pinMode(BUTTONB, INPUT_PULLUP);
  pinMode(BUTTONC, INPUT_PULLUP);

  SD.remove("PUBLIC.txt");
  myFile = SD.open("PUBLIC.txt", FILE_WRITE);
  myFile.println(PB4);
  myFile.close();
  Serial.println("PUBLIC text written to SD");

  SD.remove("XPUB.txt");
  myFile = SD.open("XPUB.txt", FILE_WRITE);
  myFile.println(pubx);
  myFile.close();
  Serial.println("XPUB text written to SD");

  SD.remove("PHRASE.txt");
  myFile = SD.open("PHRASE.txt", FILE_WRITE);
  myFile.println(phrase);
  myFile.close();
  Serial.println("PHRASE text written to SD");

  SD.remove("ROOT.txt");
  myFile = SD.open("ROOT.txt", FILE_WRITE);
  myFile.println(ROOOT);
  myFile.close();
  Serial.println("ROOT text written to SD");

  Serial.println("Setup complete. Keys written to SD if FAT formatted card is present and SD initializes correctly.");



}



void loop()
{
  if ( digitalRead(BUTTONA) == LOW )
  {
  Serial.println("ButtonA");
  delay(100);
  display.clearBuffer();
  display.setTextSize(2);
  testdrawtext(P4, COLOR1);
  display.display();
  ///////////////////
   while (countdown >= (0)) 
   {delay(250);
   //try to connect for a period of time
   if (digitalRead(BUTTONA) != LOW) 
  {
    delay(250);
    String countprint = String(countdown);
    Serial.println(countprint);
    --countdown;}
      else {Serial.println("ButtonA"); break;}
   }
    Serial.println("Exit Address A");
    countdown = 250;
  ///////////////////
  display.clearBuffer();
  display.setTextSize(1);
  testdrawtext("IN CHRIST ALONE WE TRUST\n\nLIBERTOPIA BITCOIN WALLET GENERATOR v2.1\n\n\nPress button near SD slot to generate wallet.\n\n1) Press Top Left Button for Address 0 \n\n2) Press Top Middle Button for XPUB \n\n3) Press Top Right Button for Recovery Phrase \n& Optional SD Card Backup", COLOR1);
  display.display();}
 

  if ( digitalRead(BUTTONB) == LOW )
  {Serial.println("ButtonB");
   delay(100);
  display.clearBuffer();
  display.setTextSize(2);
  testdrawtext(XXPUB,COLOR1);
  display.display();
  ///////////////////
   while (countdown >= (0)) 
   {delay(250);
   //try to connect for a period of time
   if (digitalRead(BUTTONB) != LOW) 
  {
    delay(250);
    String countprint = String(countdown);
    Serial.println(countprint);
    --countdown;}
      else {Serial.println("ButtonB"); break;}
   }
    Serial.println("Exit XPUB");
    countdown = 250;
  ///////////////////
  display.clearBuffer();
  display.setTextSize(1);
  testdrawtext("IN CHRIST ALONE WE TRUST\n\nLIBERTOPIA BITCOIN WALLET GENERATOR v2.1\n\n\nPress button near SD slot to generate wallet.\n\n1) Press Top Left Button for Address 0 \n\n2) Press Top Middle Button for XPUB \n\n3) Press Top Right Button for Recovery Phrase \n& Optional SD Card Backup", COLOR1);
  display.display();}

  if ( digitalRead(BUTTONC) == LOW )
  {
  Serial.println("ButtonC");
  if(!SD.begin(SD_CS, SD_SCK_MHZ(16))){Serial.println("SD did not initialize.");};
  SD.remove("PUBLIC.txt");
  myFile = SD.open("PUBLIC.txt", FILE_WRITE);
  myFile.println(PB1);
  myFile.close();
  Serial.println("PUBLIC text written to SD");

  SD.remove("XPUB.txt");
  myFile = SD.open("XPUB.txt", FILE_WRITE);
  myFile.println(pubx);
  myFile.close();
  Serial.println("XPUB text written to SD");

  SD.remove("PHRASE.txt");
  myFile = SD.open("PHRASE.txt", FILE_WRITE);
  myFile.println(phra);
  myFile.close();
  Serial.println("PHRASE text written to SD");
  
  display.clearBuffer();
  display.setTextSize(2);
  testdrawtext(PHR, COLOR1);
  display.display();
  ///////////////////
   while (countdown >= (0)) 
   {delay(250);
   //try to connect for a period of time
   if (digitalRead(BUTTONC) != LOW) 
  {
    delay(250);
    String countprint = String(countdown);
    Serial.println(countprint);
    --countdown;}
      else {Serial.println("ButtonC"); break;}
   }
    Serial.println("Exit Phrase");
    countdown = 250;
  ///////////////////
  display.clearBuffer();
  display.setTextSize(1);
  testdrawtext("IN CHRIST ALONE WE TRUST\n\nLIBERTOPIA BITCOIN WALLET GENERATOR v2.1\n\n\nPress button near SD slot to generate wallet.\n\n1) Press Top Left Button for Address 0 \n\n2) Press Top Middle Button for XPUB \n\n3) Press Top Right Button for Recovery Phrase \n& Optional SD Card Backup\n\n\n If SD Fat card present, it now has wallet info.", COLOR1);
  display.display();
  }


  
}
