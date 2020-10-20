/**
  ******************************************************************************
  * @file    fingerprint.c
  * @author  Calvin Ageneau
  * @version V1.0
  * @date    26-January-2017
  * @brief   Fingerprint source
  ******************************************************************************
*/

#include "config.h"

#if USE_FINGERPRINT

#include "fingerprint.h"

#define FINGERPRINT_OK 0x00
#define FINGERPRINT_PACKETRECIEVEERR 0x01
#define FINGERPRINT_NOFINGER 0x02
#define FINGERPRINT_IMAGEFAIL 0x03
#define FINGERPRINT_IMAGEMESS 0x06
#define FINGERPRINT_FEATUREFAIL 0x07
#define FINGERPRINT_NOMATCH 0x08
#define FINGERPRINT_NOTFOUND 0x09
#define FINGERPRINT_ENROLLMISMATCH 0x0A
#define FINGERPRINT_BADLOCATION 0x0B
#define FINGERPRINT_DBRANGEFAIL 0x0C
#define FINGERPRINT_UPLOADFEATUREFAIL 0x0D
#define FINGERPRINT_PACKETRESPONSEFAIL 0x0E
#define FINGERPRINT_UPLOADFAIL 0x0F
#define FINGERPRINT_DELETEFAIL 0x10
#define FINGERPRINT_DBCLEARFAIL 0x11
#define FINGERPRINT_PASSFAIL 0x13
#define FINGERPRINT_INVALIDIMAGE 0x15
#define FINGERPRINT_FLASHERR 0x18
#define FINGERPRINT_INVALIDREG 0x1A
#define FINGERPRINT_ADDRCODE 0x20
#define FINGERPRINT_PASSVERIFY 0x21

#define FINGERPRINT_STARTCODE 0xEF01

#define FINGERPRINT_COMMANDPACKET 0x1
#define FINGERPRINT_DATAPACKET 0x2
#define FINGERPRINT_ACKPACKET 0x7
#define FINGERPRINT_ENDDATAPACKET 0x8

#define FINGERPRINT_TIMEOUT 0xFF
#define FINGERPRINT_BADPACKET 0xFE

#define FINGERPRINT_GETIMAGE 0x01
#define FINGERPRINT_IMAGE2TZ 0x02
#define FINGERPRINT_REGMODEL 0x05
#define FINGERPRINT_STORE 0x06
#define FINGERPRINT_DELETE 0x0C
#define FINGERPRINT_EMPTY 0x0D
#define FINGERPRINT_VERIFYPASSWORD 0x13
#define FINGERPRINT_HISPEEDSEARCH 0x1B
#define FINGERPRINT_TEMPLATECOUNT 0x1D

void fingerprint_Init(unsigned short baud) {
	thePassword = MDP_ACCES_CAPTEUR;
	theAddress = ADRESSE_CAPTEUR;

	HAL_Delay(1000);

	//Initialisation de l'UART1 à la vitesse de 57600 bauds/secondes PA9 : Tx  | PA10 : Rx.
	UART_init(UART1_ID,baud);

	//"Indique que les printf sortent vers le périphérique UART1."
	SYS_set_std_usart(UART1_ID, UART1_ID, UART1_ID);
}

int getFingerprintIDez() {
  unsigned char p = getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = image2Tz(1);
  if (p != FINGERPRINT_OK)  return -1;

  p = fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  return fingerID;
}

bool_e verifyPassword(void) {
  unsigned char packet[] = {FINGERPRINT_VERIFYPASSWORD,
                      (thePassword >> 24), (thePassword >> 16),
                      (thePassword >> 8), thePassword};
  writePacket(theAddress, FINGERPRINT_COMMANDPACKET, 7, packet);
  unsigned char len = getReply(packet);

  if ((len == 1) && (packet[0] == FINGERPRINT_ACKPACKET) && (packet[1] == FINGERPRINT_OK))
    return TRUE;

  return FALSE;
}

unsigned char getImage(void) {
  unsigned char packet[] = {FINGERPRINT_GETIMAGE};
  writePacket(theAddress, FINGERPRINT_COMMANDPACKET, 3, packet);
  unsigned char len = getReply(packet);

  if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
   return -1;
  return packet[1];
}

unsigned char image2Tz(unsigned char slot) {
  unsigned char packet[] = {FINGERPRINT_IMAGE2TZ, slot};
  writePacket(theAddress, FINGERPRINT_COMMANDPACKET, sizeof(packet)+2, packet);
  unsigned char len = getReply(packet);

  if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
   return -1;
  return packet[1];
}

unsigned char fingerFastSearch(void) {
  fingerID = 0xFFFF;
  confidence = 0xFFFF;
  // high speed search of slot #1 starting at page 0x0000 and page #0x00A3
  unsigned char packet[] = {FINGERPRINT_HISPEEDSEARCH, 0x01, 0x00, 0x00, 0x00, 0xA3};
  writePacket(theAddress, FINGERPRINT_COMMANDPACKET, sizeof(packet)+2, packet);
  unsigned char len = getReply(packet);

  if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
   return -1;

  fingerID = packet[2];
  fingerID <<= 8;
  fingerID |= packet[3];

  confidence = packet[4];
  confidence <<= 8;
  confidence |= packet[5];

  return packet[1];
}

void writePacket(unsigned int addr, unsigned char packettype,
				       unsigned short len, unsigned char *packet) {
#ifdef FINGERPRINT_DEBUG
  printf("---> 0x");
  printf((unsigned char)(FINGERPRINT_STARTCODE >> 8), HEX);
  printf(" 0x");
  printf((unsigned char)FINGERPRINT_STARTCODE, HEX);
  printf(" 0x");
  printf((unsigned char)(addr >> 24), HEX);
  printf(" 0x");
  printf((unsigned char)(addr >> 16), HEX);
  printf(" 0x");
  printf((unsigned char)(addr >> 8), HEX);
  printf(" 0x");
  printf((unsigned char)(addr), HEX);
  printf(" 0x");
  printf((unsigned char)packettype, HEX);
  printf(" 0x");
  printf((unsigned char)(len >> 8), HEX);
  printf(" 0x");
  printf((unsigned char)(len), HEX);
#endif

  printf("%c",(unsigned char)(FINGERPRINT_STARTCODE >> 8));
  printf("%c",(unsigned char)FINGERPRINT_STARTCODE);
  printf("%c",(unsigned char)(addr >> 24));
  printf("%c",(unsigned char)(addr >> 16));
  printf("%c",(unsigned char)(addr >> 8));
  printf("%c",(unsigned char)(addr));
  printf("%c",(unsigned char)packettype);
  printf("%c",(unsigned char)(len >> 8));
  printf("%c",(unsigned char)(len));


  unsigned short sum = (len>>8) + (len&0xFF) + packettype;
  unsigned char i;
  for (i=0; i< len-2; i++) {

    printf("%c",(unsigned char)(packet[i]));

#ifdef FINGERPRINT_DEBUG
    printf(" 0x"); printf(packet[i], HEX);
#endif
    sum += packet[i];
  }
#ifdef FINGERPRINT_DEBUG
  //printf("Checksum = 0x"); printf(sum); printf("\n");
  printf(" 0x"); printf((unsigned char)(sum>>8), HEX);
  printf(" 0x"); printf((unsigned char)(sum), HEX); printf("\n");
#endif
#if ARDUINO >= 100
  mySerial->write((unsigned char)(sum>>8));
  mySerial->write((unsigned char)sum);
#else
  printf("%c",(unsigned char)(sum>>8));
  printf("%c",(unsigned char)sum);
#endif
}


unsigned char getReply(unsigned char packet[]) {

	unsigned short timeout=DEFAULTTIMEOUT;
	unsigned char reply[20], idx;
    unsigned short timer=0;

  idx = 0;
#ifdef FINGERPRINT_DEBUG
  printf("<--- ");
#endif
while (TRUE) {
    while (!UART_data_ready(UART1_ID)) {
    HAL_Delay(1);
      timer++;
      if (timer >= timeout) return FINGERPRINT_TIMEOUT;
    }
    // something to read!
    scanf("%c",&reply[idx]);
#ifdef FINGERPRINT_DEBUG
    printf(" 0x"); printf(reply[idx], HEX);
#endif
    if ((idx == 0) && (reply[0] != (FINGERPRINT_STARTCODE >> 8)))
      continue;
    idx++;

    // check packet!
    if (idx >= 9) {
      if ((reply[0] != (FINGERPRINT_STARTCODE >> 8)) ||
          (reply[1] != (FINGERPRINT_STARTCODE & 0xFF)))
          return FINGERPRINT_BADPACKET;
      unsigned char packettype = reply[6];
      //printf("Packet type"); printf(packettype); printf("\n");
      unsigned short len = reply[7];
      len <<= 8;
      len |= reply[8];
      len -= 2;
      //printf("Packet len"); printf(len); printf("\n");
      if (idx <= (len+10)) continue;
      packet[0] = packettype;
      unsigned char i;
      for (i=0; i<len; i++) {
        packet[1+i] = reply[9+i];
      }
#ifdef FINGERPRINT_DEBUG
      printf("\n");
#endif
      return len;
    }
  }
}

#endif
