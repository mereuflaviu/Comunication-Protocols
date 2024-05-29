#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "link_emulator/lib.h"
#include <time.h>

/* Do not touch these two */
#define HOST "127.0.0.1"
#define PORT 10001

#include "common.h"

/* Our unqiue layer 2 ID */
static int ID = 123131;

/* Function which our protocol implementation will provide to the upper layer. */
int recv_frame(char *buf, int size)
{
	/* TODO 1.1: Call recv_byte() until we receive the frame start
	 * delimitator. This operation makes this function blocking until it
	 * receives a frame. */
	char c1, c2;

c1 = recv_byte();
c2 = recv_byte();

/* Cât timp nu am primit DLE STX citim bytes. Atenție la modul în care salvăm
 * byte-ul precedent. */
while( ((c1 != DLE) && (c2 != STX)) || (c1 == DLE && c2 != STX) || (c1 != DLE && c2 == STX)) {
  c1 = c2;
  c2 = recv_byte();
}

		/* TODO 2.1: The first two 2 * sizeof(int) bytes represent sender and receiver ID */
	for(int i = 0; i < 4; i++) //trec peste inturi
   		recv_byte();
	int dest;

	for(int i = 0; i < 4; i++)
			*((char *) &dest + i)=recv_byte();

	/* TODO 2.2: Check that the frame was sent to me */
	printf("Destinatia este %d\n", dest);

	/* TODO 1.2: Read bytes and copy them to buff until we receive the end of the frame */
	
/* Am primit începutul unui frame: DLE STX */
 int i;
for (i = 0; i < size; i++) {
    char byte = recv_byte();

  /* Dacă am primit un escape */
    if (byte == DLE) {
      byte = recv_byte();
      /* Am primit DLE ETX */
      if (byte == ETX)
        {
    	return i;
			}
    }
    /* Punem în buffer conținutul frame-ului */
    buf[i] = byte;
}
	/* If everything went well return the number of bytes received */
	return i;
}

int main(int argc,char** argv){
	/* Do not touch this */
	init(HOST,PORT);


	/* TODO 1.0: Allocate a buffer and call recv_frame */
	char buffer[30];
	int nr_bytes = recv_frame(buffer, sizeof(buffer));
	for(int i = 0; i < nr_bytes; i++)
		printf("%c", buffer[i]);

   printf("[RECEIVER] am primit %s si nr de octeti %d\n", buffer, nr_bytes);

	/* TODO 3: Measure latency in a while loop for any frame that contains
	 * a timestamp we receive, print frame_size and latency */
  char buffer1[100];
  printf("Se primeste timestampul\n");
  nr_bytes=recv_frame(buffer1, sizeof(buffer1));
  time_t received_timestamp=(time_t) buffer1;
  time_t t1 = time(0);

  printf("[RECEIVER] the timestamp we received=%ld, the frame_size=%ld, the latency=%f\n", received_timestamp, sizeof(buffer1), difftime(t1,received_timestamp));

	printf("[RECEIVER] Finished transmission\n");
	return 0;
}
