#include "common.h"
#include <string.h>

uint8_t simple_csum(uint8_t *buf, size_t len) {

  /* TODO 1.1: Implement the simple checksum algorithm */
  /* Ca input primim un buffer char *buf de dimensiune int count */
  struct l3_msg msg;
  uint8_t checksum;
  uint8_t offset=0;

  memcpy(msg.payload, buf, len);

  msg.hdr.sum = 0;

  /* Adăugăm în sum fiecare byte din buffer */
  while (len > 0) {
    msg.hdr.sum += *((uint8_t *) buf);
    *(char *) msg.payload += offset;
    offset += 1;
    len -= 1;
  }

  checksum = msg.hdr.sum % 256;
  // printf("checksum=%d\n", checksumi);
  return checksum;
}

uint32_t crc32(uint8_t *buf, size_t len)
{
  /* TODO 2.1: Implement the CRC 32 algorithm */
  /* Iterate through each byte of buff */
  /* Iterate through each bit */
  /* If the bit is 1, compute the new reminder */
  /* By convention, we negate the crc */

  /* unsigned char *buffer contine payload-ul, len este lungimea acestuia */
  /* Prin conventie crc-ul initial are toti bitii setati pe 1 */
  uint32_t crc = ~0; // 0xffffffff
  const uint32_t POLY = 0xEDB88320;

  /* Parcurgem fiecare byte din buffer */
  while(len--)
  {
    /* crc contine restul impartirii la fiecare etapa */
    /* nu ne intereseaza catul */
    /* adunam urmatorii 8 bytes din buffer */
    crc = crc ^ *buf++;
    for( int bit = 0; bit < 8; bit++ )
    {
      /* 10011 ) 11010110110000 = Bytes of payload
         =Poly   10011,,.,,....
         -----,,.,,....
         10011,.,,....  (operatia de xor cand primul bit e 1)
         10011,.,,....
         -----,.,,....
         00001.,,....  (asta e noua valoare a lui crc) (crc >> 1) ^ POLY
         */
      if( crc & 1 )
        crc = (crc >> 1) ^ POLY;
      else 
        /* 10011 ) 11010110110000 = Bytes of payload
           =Poly   10011,,.,,....
           -----,,.,,....
           10011,.,,....  
           10011,.,,....
           -----,.,,....
           00001.,,....  primul bit e 0, 
           00000.,,....  
           -----.,,....
           00010,,.... am facut shift la dreapta, pentru ca suntem pe **little endian**
           */
        crc = (crc >> 1);
    }
  }
  // Prin conventie, o sa facem flip la toti bitii
  crc = ~crc;
  printf("crc=0x%04hx\n", crc);
  return crc;
}
