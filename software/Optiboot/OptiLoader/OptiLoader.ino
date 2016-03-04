// optiLoader.ino
//
// this sketch allows an Arduino to program Optiboot onto any other
// Arduino-like device containing ATmega8, ATmega168, or ATmega328
// microcontroller chips.
//
// Copyright (c) 2011, 2015 by Bill Westfield ("WestfW")

//-------------------------------------------------------------------------------------
// "MIT Open Source Software License":
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in the
// Software without restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
// and to permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//-------------------------------------------------------------------------------------

//
// this sketch allows an Arduino to program Optiboot onto any other
// Arduino-like device containing ATmega8, ATmega168, or ATmega328
// microcontroller chips.
//
// It is based on AVRISP
//
// Designed to connect to a generic programming cable,
// using the following pins:
// 10: slave reset
// 11: MOSI
// 12: MISO
// 13: SCK
//  9: Power to external chip.
//     This is a little questionable, since the power it is legal to draw
//     from a PIC pin is pretty close to the power consumption of an AVR
//     chip being programmed.  But it permits the target to be entirely
//     powered down for safe reconnection of the programmer to additional
//     targets, and it seems to work for most Arduinos.  If the target board
//     contains additional circuitry and is expected to draw more than 40mA,
//     connect the target power to a stronger source of +5V.  Do not use pin
//     9 to power more complex Arduino boards that draw more than 40mA, such
//     as the Arduino Uno Ethernet !
//
// If the aim is to reprogram the bootloader in one Arduino using another
// Arudino as the programmer, you can just use jumpers between the connectors
// on the Arduino board.  In this case, connect:
// Pin 13 to Pin 13
// Pin 12 to Pin 12
// Pin 11 to Pin 11
// Pin 10 (of "programmer") to RESET (of "target" (on the "power" connector))
// +5V to +5V and GND to GND.  Only the "programmer" board should be powered
//     by USB or external power.
//
// ----------------------------------------------------------------------

// The following credits are from AVRISP.  It turns out that there isn't
// a lot of AVRISP left in this sketch, but probably if AVRISP had never
// existed,  this sketch would not have been written.
//
// October 2009 by David A. Mellis
// - Added support for the read signature command
// 
// February 2009 by Randall Bohn
// - Added support for writing to EEPROM (what took so long?)
// Windows users should consider WinAVR's avrdude instead of the
// avrdude included with Arduino software.
//
// January 2008 by Randall Bohn
// - Thanks to Amplificar for helping me with the STK500 protocol
// - The AVRISP/STK500 (mk I) protocol is used in the arduino bootloader
// - The SPI functions herein were developed for the AVR910_ARD programmer 
// - More information at http://code.google.com/p/mega-isp


#include <avr/pgmspace.h>
#include "optiLoader.h"

char Arduino_preprocessor_hint;

// Pins to target
#define SCK     13
#define MISO    12
#define MOSI    11
#define RESET   10
#define POWER    9 // inverted for P-Chn MosFet
#define LED      8
#define SWITCH   7 // switch to start programming

#define SPI_SPEED_SLOW ((1<<SPR1)|(1<<SPR0)) // 125kHz clk=Fcpu/128
#define SPI_SPEED_FAST ((0<<SPR1)|(1<<SPR0)) // 1MHz   clk=Fcpu/16
#define SPI_SPEED_MASK ((1<<SPR1)|(1<<SPR0))

// STK Definitions; we can still use these as return codes
#define STK_OK 0x10
#define STK_FAILED 0x11

// Useful message printing definitions
#define fp(string) flashprint(PSTR(string));
#define debug(string) // flashprint(PSTR(string));
#define error(string) flashprint(PSTR(string)); perror=1;

// Forward references
void pulse(int pin, int times);
void read_image(const image_t *ip);

// Global Variables
extern const image_t PROGMEM image_328p, image_328pb;

const image_t * images[] = //Table of defined images
{
  &image_328p , &image_328pb, 0
};

/*
 * Table of "Aliases."  Chips that are effectively the same as chips
 * that we have a bootloader for.  These work by simply overriding the
 * signature read with the signature of the chip we "know."
 */
const alias_t aliases[] =
{
  { "atmega328",   0x9514, 0x950F }  /* Treat 328 same as 328P */
  //{ "atmega328p",  0x950F, 0x9514 },  /* Treat 328P same as 328 */
  //{ "atmega328pb", 0x9516, 0x9514 },  /* Treat 328PB same as 328 */
};

int pmode=0;
int perror=0;
// address for reading and writing, set by 'U' command
int here;

uint16_t target_type = 0;   /* type of target_cpu */
uint16_t target_startaddr;
uint8_t target_pagesize;       /* Page size for flash programming (bytes) */
uint8_t *buff;

const image_t *target_flashptr;          /* pointer to target info in flash */
uint8_t target_code[512];        /* The whole code */

void setup (void) {
  end_pmode();
  pinMode(LED, OUTPUT);       /* set LED pin as output */
  digitalWrite(LED, LOW);     /* LED off */
  pinMode(SWITCH, INPUT);     /* set SWITCH pin as input */
  digitalWrite(SWITCH, HIGH); /* SWITCH pull-up on */

  Serial.begin(115200);       /* Initialize serial for status msgs */
  fp("\nOptiLoader Bootstrap Programmer (" __DATE__ ")\n\n");
}

void loop (void) {
  pulse(LED, 3);
  if (target_poweron()) {   /* Turn on target power */
    perror = 0;
    do {
      if (!target_identify())     /* Figure out what kind of CPU */
        break;
      if (!target_findimage())    /* look for an image */
        break;
      if (!target_progfuses())    /* get fuses ready to program */
        break;
      //SPI speed up after fuse programming
      SPCR = (SPCR & ~SPI_SPEED_MASK) | SPI_SPEED_FAST;
      if (!target_program())      /* Program the image */
        break;
      (void) target_normfuses();  /* reset fuses to normal mode */
    } 
    while (0);
  }
  else {
    Serial.println();
  }
  target_poweroff();      /* turn power off */

  if (perror == 0)
    digitalWrite(LED, HIGH);  /* LED on */
  else
    digitalWrite(LED, LOW);   /* LED off */

  while(Serial.read() != -1); /* clear buffer */
  fp("\nType 'G' or hit RESET for next chip.\n")

#ifdef SWITCH
  int sw_state = digitalRead(SWITCH);
#endif
  while (1)
  {
    if (Serial.read() == 'G')
    {
      break;
    }
#ifdef SWITCH
    if(sw_state != digitalRead(SWITCH))
    {
      /*
      digitalWrite(LED, LOW);
      fp("\nWaiting for 2nd press...\n")
      delay(500);
      sw_state = digitalRead(SWITCH);
      while (sw_state == digitalRead(SWITCH));
      */
      break;
    }
#endif
    if (perror != 0)
    {
      pulse(LED, 3);
    }
  }
}

/*
 * Low level support functions
 */

/*
 * flashprint
 * print a text string direct from flash memory to Serial
 */
void flashprint (const char p[])
{
  uint8_t c;
  while (0 != (c = pgm_read_byte(p++))) {
    Serial.write(c);
  }
}

/*
 * hexton
 * Turn a Hex digit (0..9, A..F) into the equivalent binary value (0-16)
 */
uint8_t hexton (uint8_t h)
{
  if (h >= '0' && h <= '9')
    return(h - '0');
  if (h >= 'A' && h <= 'F')
    return((h - 'A') + 10);
  error("Bad hex digit!");
  return(0);
}

/*
 * pulse
 * turn a pin on and off a few times; indicates life via LED
 */
#define PTIME 30
void pulse (int pin, int times) {
  do {
    digitalWrite(pin, HIGH);
    delay(PTIME);
    digitalWrite(pin, LOW);
    delay(PTIME);
  } 
  while (times--);
}

/*
 * spi_wait
 * wait for SPI transfer to complete
 */
void spi_wait (void) {
  debug("spi_wait");
  do {
  } 
  while (!(SPSR & (1 << SPIF)));
}

/*
 * spi_send
 * send a byte via SPI, wait for the transfer.
 */
uint8_t spi_send (uint8_t b) {
  uint8_t reply;
  SPDR=b;
  spi_wait();
  reply = SPDR;
  return reply;
}

/*
 * Functions specific to ISP programming of an AVR
 */

unsigned long spi_transaction (uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
  uint8_t n, m;
  spi_send(a); 
  n=spi_send(b);
  //if (n != a) error = -1;
  m=spi_send(c);
  return 0xFFFFFF & ((((uint32_t)n)<<16)+(m<<8) + spi_send(d));
}

void spi_poll_ready (void) {
  delay(5);
  do{
  }
  while((spi_transaction(0xF0, 0x00, 0x00, 0x00) & 0x1) != 0);
}

uint16_t start_pmode () {
  uint16_t result;
  uint8_t x;

  // initialize the AVR SPI peripheral
  pinMode(MISO, INPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT);
  SPCR = (1<<SPE)|(1<<MSTR)|SPI_SPEED_SLOW;
  x=SPSR;
  x=SPDR;

  // following delays may not work on all targets...
  pinMode(RESET, OUTPUT);
  digitalWrite(RESET, HIGH);
  delay(20);
  digitalWrite(RESET, LOW);
  delay(20);
  debug("...spi_transaction");
  result = spi_transaction(0xAC, 0x53, 0x00, 0x00);
  debug("...Done");
  pmode = 1;
  return result;
}

void end_pmode (void) {
  SPCR = 0;                 /* reset SPI */
  digitalWrite(MISO, LOW);    /* Make sure pullups are off too */
  pinMode(MISO, INPUT);
  digitalWrite(MOSI, LOW);
  pinMode(MOSI, INPUT);
  digitalWrite(SCK, LOW);
  pinMode(SCK, INPUT);
  digitalWrite(RESET, LOW);
  pinMode(RESET, INPUT);
  pmode = 0;
}

/*
 * read_image
 *
 * Read an intel hex image from a string in pgm memory.
 * We assume that the image does not exceed the 512 bytes that we have
 * allowed for it to have.  that would be bad.
 * Also read other data from the image, such as fuse and protecttion byte
 * values during programming, and for after we're done.
 */
void read_image (const image_t *ip)
{
  uint16_t len, totlen=0, addr;
  const char *hextext = &ip->image_hexcode[0];
  target_startaddr = 0;
  target_pagesize = pgm_read_byte(&ip->image_pagesize);
  uint8_t b, cksum = 0;

  while (1) {
    if (pgm_read_byte(hextext++) != ':') {
      error("No colon");
      break;
    }
    len = hexton(pgm_read_byte(hextext++));
    len = (len<<4) + hexton(pgm_read_byte(hextext++));
    cksum = len;

    b = hexton(pgm_read_byte(hextext++)); /* record type */
    b = (b<<4) + hexton(pgm_read_byte(hextext++));
    cksum += b;
    addr = b;
    b = hexton(pgm_read_byte(hextext++)); /* record type */
    b = (b<<4) + hexton(pgm_read_byte(hextext++));
    cksum += b;
    addr = (addr << 8) + b;
    if (target_startaddr == 0) {
      target_startaddr = addr;
      fp("  Start address at ");
      Serial.println(addr, HEX);
    } 
    else if (addr == 0) {
      break;
    }

    b = hexton(pgm_read_byte(hextext++)); /* record type */
    b = (b<<4) + hexton(pgm_read_byte(hextext++));
    cksum += b;

    for (uint8_t i=0; i < len; i++) {
      b = hexton(pgm_read_byte(hextext++));
      b = (b<<4) + hexton(pgm_read_byte(hextext++));
      if (addr - target_startaddr >= sizeof(target_code)) {
        error("\nCode extends beyond allowed range");
        break;
      }
      target_code[addr++ - target_startaddr] = b;
      cksum += b;
#if VERBOSE
      Serial.print(b, HEX);
      Serial.write(' ');
#endif
      totlen++;
      if (totlen >= sizeof(target_code)) {
        error("\nToo much code");
        break;
      }
    }
    b = hexton(pgm_read_byte(hextext++)); /* checksum */
    b = (b<<4) + hexton(pgm_read_byte(hextext++));
    cksum += b;
    if (cksum != 0) {
      error("\nBad checksum: ");
      Serial.print(cksum, HEX);
    }
    if (pgm_read_byte(hextext++) != '\n') {
      error("\nNo end of line");
      break;
    }
#if VERBOSE
    Serial.println();
#endif
  }
  fp("  Total bytes read: ");
  Serial.println(totlen);
}

/*
 * target_findimage
 *
 * given target_type loaded with the relevant part of the device signature,
 * search the hex images that we have programmed in flash, looking for one
 * that matches.
 */
boolean target_findimage ()
{
  const image_t *ip;
  fp("Searching for image...\n");
  // Search through our table of chip aliases first
  for (uint8_t i=0; i < sizeof(aliases)/sizeof(aliases[0]); i++) {
    const alias_t *a = &aliases[i];
    if (a->real_chipsig == target_type) {
      fp("  Compatible bootloader for ");
      Serial.println(a->alias_chipname);
      target_type = a->alias_chipsig;  /* Overwrite chip signature */
      break;
    }
  }
  // Search through our table of self-contained images.
  for (uint8_t i=0; i < sizeof(images)/sizeof(images[0]); i++) {
    target_flashptr = ip = images[i];
    if (ip && (pgm_read_word(&ip->image_chipsig) == target_type)) {
      fp("  Found \"");
      flashprint(&ip->image_name[0]);
      fp("\" for ");
      flashprint(&ip->image_chipname[0]);
      fp("\n");
      read_image(ip);
      return true;
    }
  }
  fp(" Not Found\n");
  return(false);
}

/*
 * target_identify
 * read the signature bytes (if possible) and check whether it's
 * a legal value (atmega8, atmega168, atmega328)
 */
boolean target_identify ()
{
  boolean result;
  target_type = 0;
  fp("\nReading signature:");
  target_type = read_signature();
  if (target_type == 0 || target_type == 0xFFFF) {
    error(" Bad value: ");
    result = false;
  } 
  else {
    result = true;
  }
  Serial.println(target_type, HEX);
  if (target_type == 0) {
    fp("  (no target attached?)\n");
  }
  return result;
}

/*
 * target_progfuses
 * given initialized target image data, re-program the fuses to allow
 * the optiboot image to be programmed.
 */
boolean target_progfuses ()
{
  uint8_t f;
  fp("\nSetting fuses for programming");

  f = pgm_read_byte(&target_flashptr->image_progfuses[FUSE_PROT]);
  if (f) {
    fp("\n  Lock: ");
    Serial.print(f, HEX);
    fp(" ");
    Serial.print(spi_transaction(0xAC, 0xE0, 0x00, f), HEX);
    spi_poll_ready();
  }
  f = pgm_read_byte(&target_flashptr->image_progfuses[FUSE_LOW]);
  if (f) {
    fp("  Low: ");
    Serial.print(f, HEX);
    fp(" ");
    Serial.print(spi_transaction(0xAC, 0xA0, 0x00, f), HEX);
    spi_poll_ready();
  }
  f = pgm_read_byte(&target_flashptr->image_progfuses[FUSE_HIGH]);
  if (f) {
    fp("  High: ");
    Serial.print(f, HEX);
    fp(" ");
    Serial.print(spi_transaction(0xAC, 0xA8, 0x00, f), HEX);
    spi_poll_ready();
  }
  f = pgm_read_byte(&target_flashptr->image_progfuses[FUSE_EXT]);
  if (f) {
    fp("  Ext: ");
    Serial.print(f, HEX);
    fp(" ");
    Serial.print(spi_transaction(0xAC, 0xA4, 0x00, f), HEX);
    spi_poll_ready();
  }
  Serial.println();

  return true;
}

/*
 * target_program
 * Actually program the image into the target chip
 */
boolean target_program ()
{
  int l;        /* actual length */

  fp("\nProgramming bootloader: ");
  here = target_startaddr>>1;     /* word address */
  buff = target_code;
  l = 512;
  Serial.print(l, DEC);
  fp(" bytes at 0x");
  Serial.println(here, HEX);

  spi_transaction(0xAC, 0x80, 0, 0);  /* chip erase */
  delay(200);
  spi_poll_ready();
  if (write_flash(l) != STK_OK) {
    error("\nFlash Write Failed");
    return false;
  }
  return true;
}

/*
 * target_normfuses
 * reprogram the fuses to the state they should be in for bootloader
 * based programming
 */
boolean target_normfuses ()
{
  uint8_t f;
  fp("\nRestoring normal fuses");

  f = pgm_read_byte(&target_flashptr->image_normfuses[FUSE_PROT]);
  if (f) {
    fp("\n  Lock: ");
    Serial.print(f, HEX);
    fp(" ");
    Serial.print(spi_transaction(0xAC, 0xE0, 0x00, f), HEX);
    spi_poll_ready();
  }
  f = pgm_read_byte(&target_flashptr->image_normfuses[FUSE_LOW]);
  if (f) {
    fp("  Low: ");
    Serial.print(f, HEX);
    fp(" ");
    Serial.print(spi_transaction(0xAC, 0xA0, 0x00, f), HEX);
    spi_poll_ready();
  }
  f = pgm_read_byte(&target_flashptr->image_normfuses[FUSE_HIGH]);
  if (f) {
    fp("  High: ");
    Serial.print(f, HEX);
    fp(" ");
    Serial.print(spi_transaction(0xAC, 0xA8, 0x00, f), HEX);
    spi_poll_ready();
  }
  f = pgm_read_byte(&target_flashptr->image_normfuses[FUSE_EXT]);
  if (f) {
    fp("  Ext: ");
    Serial.print(f, HEX);
    fp(" ");
    Serial.print(spi_transaction(0xAC, 0xA4, 0x00, f), HEX);
    spi_poll_ready();
  }
  Serial.println();
  return true;
}

/*
 * target_poweron
 * Turn on power to the target chip (assuming that it is powered through
 * the relevant IO pin of THIS arduino.)
 */
boolean target_poweron ()
{
  uint16_t result;

  fp("Target power ON! ...");
  digitalWrite(POWER, LOW);
  pinMode(POWER, OUTPUT);
  //digitalWrite(POWER, HIGH);
  delay(100);
  digitalWrite(RESET, LOW);  // reset it right away
  pinMode(RESET, OUTPUT);
  delay(5);
  // Check if the target is pulling RESET HIGH by reverting to input
  pinMode(RESET, INPUT);
  delay(1);
  if (digitalRead(RESET) != HIGH) {
    error("\nNo RESET pullup detected! - no target?");
    return false;
  }

  delay(100);
  fp("\nStarting Program Mode");
  result = start_pmode();
  if ((result & 0xFF00) != 0x5300) {
    error(" - Failed, result = 0x");
    Serial.println(result, HEX);
    return false;
  }
  fp(" [OK]\n");
  return true;
}

boolean target_poweroff ()
{
  end_pmode();
  digitalWrite(POWER, HIGH);
  //digitalWrite(POWER, LOW);
  //pinMode(POWER, INPUT);
  fp("\nTarget power OFF!\n");
  return true;
}

void flash (uint8_t hilo, int addr, uint8_t data) {
#if VERBOSE
  Serial.print(data, HEX);
  fp(":");
  Serial.print(spi_transaction(0x40+8*hilo, 
  addr>>8 & 0xFF, 
  addr & 0xFF,
  data), HEX);
  fp(" ");
#else
  (void) spi_transaction(0x40+8*hilo, 
  addr>>8 & 0xFF, 
  addr & 0xFF,
  data);
#endif
}

void commit (int addr) {
  fp("  Commit Page: ");
  Serial.print(addr, HEX);
  fp(":");
  Serial.println(spi_transaction(0x4C, (addr >> 8) & 0xFF, addr & 0xFF, 0), HEX);
  delay(100);
}

//#define _current_page(x) (here & 0xFFFFE0)
int current_page (int addr) {
  if (target_pagesize ==  32) return here & 0xFFFFFFF0;
  if (target_pagesize ==  64) return here & 0xFFFFFFE0;
  if (target_pagesize == 128) return here & 0xFFFFFFC0;
  return here;
}

uint8_t write_flash (int length) {
  if (target_pagesize < 1) return STK_FAILED;
  //if (target_pagesize != 64) return STK_FAILED;
  int page = current_page(here);
  int x = 0;
  while (x < length) {
    if (page != current_page(here)) {
      commit(page);
      page = current_page(here);
    }
    flash(LOW, here, buff[x]);
    flash(HIGH, here, buff[x+1]);
    x+=2;
    here++;
  }
  commit(page);
  return STK_OK;
}

uint16_t read_signature () {
  uint8_t sig_middle = spi_transaction(0x30, 0x00, 0x01, 0x00);
  uint8_t sig_low = spi_transaction(0x30, 0x00, 0x02, 0x00);
  return ((sig_middle << 8) + sig_low);
}

/*
 * Bootload images.
 * These are the intel Hex files produced by the optiboot makefile,
 * with a small amount of automatic editing to turn them into C strings,
 * and a header attched to identify them
 *
 * Emacs keyboard macro:

      4*SPC     ;; self-insert-command
      "     ;; self-insert-command
      C-e     ;; move-end-of-line
      \     ;; self-insert-command
      n"      ;; self-insert-command * 2
      C-n     ;; next-line
      C-a     ;; move-beginning-of-line

 */

const image_t PROGMEM image_328p = {
  { "optiboot_m328p.hex" },
  { "atmega328p" },
    0x950F,              /* 0x1E 0x95 0x16 Signature bytes for 328PB */
  { 0xFF,0xFF,0xD6,0xFD,0 },
  { 0xCF,0,0,0,0 },
    128,
  {
    ":107E00001F92CDB7DEB7112484B714BE982F9D7092\n"
    ":107E100009F0E6D082E08093C00088E18093C10041\n"
    ":107E200086E08093C20080E18093C4008EE0C3D0DE\n"
    ":107E300010928500109284004899FECF489BFECF97\n"
    ":107E400081E0809381004899FECF109281002091BB\n"
    ":107E50008400822F20918500922F089644E0969509\n"
    ":107E600087954A95E1F701978093C40098D08033B5\n"
    ":107E7000E9F7A7D0812C912C13E001E025E0F22E48\n"
    ":107E800031E1E32E8CD0813479F489D0898399D083\n"
    ":107E90008981823811F482E005C0813811F486E0CE\n"
    ":107EA00001C083E075D071C0823411F484E103C055\n"
    ":107EB000853419F485E08DD068C0853549F46FD0DC\n"
    ":107EC000D82E6DD08D2C912C982A880C991C5CC0D2\n"
    ":107ED000863521F484E07DD080E0E4CF843609F05B\n"
    ":107EE00036C05DD05CD0D82E5AD0C82EA12CBB2471\n"
    ":107EF000B39455D0F50181935F01DE12FACF61D0C2\n"
    ":107F0000F5E4CF1201C0FFCFF40117BFE89507B623\n"
    ":107F100000FCFDCFA401A0E0B1E02C911296CD01B0\n"
    ":107F20000197FC01808130E0382BFA01090107BF7D\n"
    ":107F3000E89511244E5F5F4FDA12EFCFF401F7BEE0\n"
    ":107F4000E89507B600FCFDCFE7BEE8951EC0843774\n"
    ":107F500071F425D024D0D82E22D033D05401F5018D\n"
    ":107F600085915F0115D0DA94D110F9CF0EC0853715\n"
    ":107F700039F427D08EE10CD085E90AD08FE092CF7A\n"
    ":107F8000813511F488E017D01CD080E101D07ACF80\n"
    ":107F90009091C00095FFFCCF8093C600089580911A\n"
    ":107FA000C00087FFFCCF8091C00084FD01C0A89570\n"
    ":107FB0008091C6000895E0E6F0E098E19083808328\n"
    ":107FC0000895EDDF803219F088E0F5DFFFCF84E11E\n"
    ":107FD000DFCFCF93C82FE3DFC150E9F7CF91F1CFC7\n"
    ":0C7FE000282E80E0E8DFE0E0FF27099495\n"
    ":0400000300007E007B\n"
    ":00000001FF\n"
  }
};


const image_t PROGMEM image_328pb = {
  { "optiboot_m328pb.hex" },
  { "atmega328pb" },
    0x9516,              /* 0x1E 0x95 0x16 Signature bytes for 328PB */
  { 0xFF,0xFF,0xD6,0xFD,0 },
  { 0xCF,0,0,0,0 },
    128,
  {
    ":107E00001F92CDB7DEB7112484B714BE982F9D7092\n"
    ":107E100009F0E6D082E08093C00088E18093C10041\n"
    ":107E200086E08093C20080E18093C4008EE0C3D0DE\n"
    ":107E300010928500109284004899FECF489BFECF97\n"
    ":107E400081E0809381004899FECF109281002091BB\n"
    ":107E50008400822F20918500922F089644E0969509\n"
    ":107E600087954A95E1F701978093C40098D08033B5\n"
    ":107E7000E9F7A7D0812C912C13E001E025E0F22E48\n"
    ":107E800031E1E32E8CD0813479F489D0898399D083\n"
    ":107E90008981823811F482E005C0813811F486E0CE\n"
    ":107EA00001C083E075D071C0823411F484E103C055\n"
    ":107EB000853419F485E08DD068C0853549F46FD0DC\n"
    ":107EC000D82E6DD08D2C912C982A880C991C5CC0D2\n"
    ":107ED000863521F484E07DD080E0E4CF843609F05B\n"
    ":107EE00036C05DD05CD0D82E5AD0C82EA12CBB2471\n"
    ":107EF000B39455D0F50181935F01DE12FACF61D0C2\n"
    ":107F0000F5E4CF1201C0FFCFF40117BFE89507B623\n"
    ":107F100000FCFDCFA401A0E0B1E02C911296CD01B0\n"
    ":107F20000197FC01808130E0382BFA01090107BF7D\n"
    ":107F3000E89511244E5F5F4FDA12EFCFF401F7BEE0\n"
    ":107F4000E89507B600FCFDCFE7BEE8951EC0843774\n"
    ":107F500071F425D024D0D82E22D033D05401F5018D\n"
    ":107F600085915F0115D0DA94D110F9CF0EC0853715\n"
    ":107F700039F427D08EE10CD085E90AD086E192CF82\n"
    ":107F8000813511F488E017D01CD080E101D07ACF80\n"
    ":107F90009091C00095FFFCCF8093C600089580911A\n"
    ":107FA000C00087FFFCCF8091C00084FD01C0A89570\n"
    ":107FB0008091C6000895E0E6F0E098E19083808328\n"
    ":107FC0000895EDDF803219F088E0F5DFFFCF84E11E\n"
    ":107FD000DFCFCF93C82FE3DFC150E9F7CF91F1CFC7\n"
    ":0C7FE000282E80E0E8DFE0E0FF27099495\n"
    ":0400000300007E007B\n"
    ":00000001FF\n"
  }
};

