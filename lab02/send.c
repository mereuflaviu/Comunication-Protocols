#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "link_emulator/lib.h"
#include <time.h>

/* We don't touch this */
#define HOST "127.0.0.1"
#define PORT 10000

/* Here we have the Frame structure */
#include "common.h"

/* Our unqiue layer 2 ID */
static int ID = 123131;

/* Function which our protocol implementation will provide to the upper layer. */
int send_frame(char *buf, int size)
{

	/* TODO 1.1: Create a new frame. */
	struct Frame frame;
	/* TODO 1.2: Copy the data from buffer to our frame structure */
	strncpy(frame.payload, buf, strlen(buf));
	memset(frame.payload + strlen(buf), '\0',  sizeof(frame.payload) - strlen(buf));

	frame.frame_delim_start[0]=DLE;
  frame.frame_delim_start[1]=STX;
  frame.frame_delim_end[0]=DLE;
  frame.frame_delim_end[1]=ETX;
  /* TODO 2.1: Set the destination and source */
 frame.source=88;
	frame.dest=ID;

	/* TODO 1.3: We can cast the frame to a char *, and iterate through sizeof(struct Frame) bytes
	 calling send_bytes. */

/* Trimite bytes din frame */
for (int i = 0; i < sizeof(struct Frame); i++) {
  printf("[SENDER] se trimite\n");
  send_byte( * ((char *) &frame + i));
}

	/* if all went all right, return 0 */
	return 0;
}

int main(int argc,char** argv){
	// Don't touch this
	init(HOST,PORT);

	/* TODO 1.0: Get some input in a buffer and call send_frame with it */
	char buffer[30];
	printf("[SENDER] Introdu inputul:\n");
  fgets(buffer, 30, stdin);
	send_frame(buffer, sizeof(buffer));

	/* TODO 3.1: Get a timestamp of the current time copy it in the the payload */
  time_t t0 = time(0);
	char buffer1[100];
	memcpy(buffer1, &t0, sizeof(t0));
  send_frame(buffer1, sizeof(buffer1));
	/* TODO 3.0: Update the maximum size of the payload in Frame to 100 (in common.h), send the frame */

	/* TODO 3.0: Update the maximum size of the payload in Frame to 300, send the frame */

	return 0;
}
