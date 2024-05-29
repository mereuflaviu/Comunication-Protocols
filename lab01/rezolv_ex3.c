#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

struct Packet
{
  char payload[100];
  int sum;
  int size;
};


int main()
{
  FILE *f = fopen("ex3.txt", "rb");
  int bytes_count = 0;
  struct Packet *p = (struct Packet *) malloc(sizeof(struct Packet));
  if(f == NULL)
  {
    fprintf(stderr, "eroare la deschidere\n");
    exit(-1);
  }

  while(1)
  {

  bytes_count=fread(p, sizeof(*p), 1, f);
  
  if(bytes_count <= 0)
  {
    break;
  }
  printf("payload = %s sum = %d size = %d\n", p->payload, p->sum, p->size);
  }
 

  fclose(f);
  return 0;
}
