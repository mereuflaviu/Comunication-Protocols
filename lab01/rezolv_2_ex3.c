#include <unistd.h> /* pentru open(), exit() */
#include <fcntl.h> /* O_RDWR */
#include <stdio.h> /* perror() */
#include <errno.h>
#include <stdlib.h>

void fatal(char * mesaj_eroare)
{
    perror(mesaj_eroare);
    exit(0);
}

int main(void)
{
    int source_fd, dest_fd;
    int bytes_count;
    char buf[1024];

    /* Open primeste ca argumente path-ul catre un fisier
       si un flag, in cazul acesta flag-ul speicica ca urmeaza
       doar sa citim din fisier.
    */
    source_fd = open("ex3.txt", O_RDONLY);
    /* O_CREAT este un flag special care specifica faptul ca daca
       fisierul nu exista, vom creea unul noi cu permisiunile 0644
    */
    // dest_fd = open("/path/to/destination", O_WRONLY | O_CREAT, 0644);
    
    /* Daca open returneaza o valoare mai mica de 0, atunci inseamna
       ca avem o eroare */
    if (source_fd < 0)
        fatal("Nu pot deschide un fisier");
        
    /* Fisierele sunt acuma identificate prin cele 2 file descriptors,
       source_fd si dest_fd 
    */
    
    /* Cu ajutorul functie read citim din fisier, in cazul de fata
       citim din fisierul identificat prin source_fd date
       si le punem in buf. Citim maxim sizeof(buf), adica 1024 */
    while ((bytes_count = read(source_fd, buf, sizeof(buf)))) {
    
        /* read returneaza numarul de bytes cititi */

        if (bytes_count < 0)
            fatal("Eroare la citire");
            
         /* read muta si ceea ce numim cursorul, daca de exemplu
           fisierul are 2048 de bytes, primul apel de read va muta
           cursorul pe pozitia 1024, o citire ulterioara va returna
           date de la pozitia 1024 in sus. lseek este o functie speciala
           pentru a interactiona cu cursorul
         */
           
        /* write este similar cu read */
        bytes_count = write(1, buf, bytes_count);
        if (bytes_count < 0)
            fatal("Eroare la scriere");
    }
    
    /* Este good practice sa eliberam file descriptors */
    close(source_fd);
    close(145);
    close(78755);

    return 0;
}
