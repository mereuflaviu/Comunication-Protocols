Client pentru Interacțiunea cu un REST API
Descriere
Acest proiect implementează un client care interacționează cu un REST API prin trimiterea de cereri HTTP de tip POST, GET și DELETE. Clientul acceptă comenzi de la tastatură și trimite tipul de cerere corespunzător în funcție de comandă. Am folosit funcțiile din laborator pentru crearea cererilor și am creat o funcție personalizată pentru generarea unui JSON corespunzător folosind biblioteca PARSON. Fiind vorba despre un client HTTP, conexiunea cu serverul este deschisă și închisă înainte și după fiecare cerere. Funcțiile din laborator au fost utilizate pentru extragerea informațiilor din răspunsul serverului, iar JSON-ul a fost interpretat cu funcțiile din biblioteca PARSON pentru a imprima mesajul de eroare sau de succes.

Comenzi
Clientul acceptă următoarele comenzi de la tastatură:

login: Se loghează în contul tău. Serverul returnează un cookie de sesiune sau un mesaj de eroare dacă username-ul sau parola sunt incorecte. Cererea nu este trimisă dacă username-ul sau parola conțin spații (conform cerințelor).

register: Înregistrează un cont nou. Serverul returnează un mesaj de succes sau un mesaj de eroare dacă username-ul este deja luat sau dacă username-ul sau parola conțin spații (conform cerințelor).

enter_library: Intră în bibliotecă. Serverul returnează un token JWT pentru accesul în bibliotecă în caz de succes, iar în caz de eroare, un mesaj de eroare.

get_books: Obține lista de cărți din bibliotecă. Serverul returnează lista de cărți în caz de succes sau un mesaj de eroare în caz de eroare. Biblioteca PARSON a fost utilizată pentru a crea un array JSON și a-l imprima.

get_book: Obține informații despre o carte specifică din bibliotecă. Serverul returnează informațiile despre carte în caz de succes sau un mesaj de eroare în caz de eroare de acces sau dacă cartea nu există. (ID-ul este verificat să fie un număr).

add_book: Adaugă o carte în bibliotecă. Serverul returnează un mesaj de succes în caz de succes sau un mesaj de eroare în caz de eroare de acces sau dacă datele sunt invalide. (Câmpurile de tip string sunt verificate să nu fie nule și page-count este verificat să fie un număr).

delete_book: Trimite o cerere DELETE pentru a șterge o carte din bibliotecă. Serverul returnează un mesaj de succes în caz de succes sau un mesaj de eroare în caz de eroare de acces sau dacă cartea nu există. (ID-ul este verificat să fie un număr).

logout: Trimite o cerere GET pentru a deloga utilizatorul curent. Cookie-ul de sesiune și token-ul JWT sunt șterse din client.

exit: Închide clientul.

Pentru orice altă comandă, clientul va imprima un mesaj de eroare.

Implementare
Implementarea clientului include mai multe funcții, fiecare gestionând o parte specifică a interacțiunii cu REST API:

remove_newline
Elimină caracterul newline dintr-un string, dacă este prezent.

allocate_data
Alocă memorie pentru stocarea câmpurilor de date utilizate în cereri.

register_user
Gestionează înregistrarea utilizatorului prin solicitarea unui username și a unei parole, validarea acestora și trimiterea unei cereri POST către server.

login_user
Gestionează autentificarea utilizatorului prin solicitarea unui username și a unei parole, validarea acestora, trimiterea unei cereri POST către server și extragerea cookie-ului de sesiune din răspuns.

enter_library
Solicită accesul în bibliotecă prin trimiterea unei cereri GET către server și extragerea token-ului JWT din răspuns.

get_books
Obține lista de cărți din bibliotecă prin trimiterea unei cereri GET către server și interpretarea răspunsului JSON.

get_book
Obține informații despre o carte specifică prin trimiterea unei cereri GET cu ID-ul cărții către server și interpretarea răspunsului JSON.

add_book
Adaugă o nouă carte în bibliotecă prin solicitarea detaliilor cărții, validarea acestora, trimiterea unei cereri POST cu detaliile cărții către server și gestionarea răspunsului.

delete_book
Șterge o carte din bibliotecă prin solicitarea ID-ului cărții, validarea acestuia, trimiterea unei cereri DELETE către server și gestionarea răspunsului.

logout
Deloghează utilizatorul curent prin trimiterea unei cereri GET către server și ștergerea cookie-ului de sesiune și a token-ului JWT.

main
Funcția principală inițializează clientul, procesează comenzile utilizatorului într-un buclă și apelează funcțiile corespunzătoare pe baza comenzilor. De asemenea, gestionează curățarea memoriei înainte de ieșire.

Conexiunea cu serverul este deschisă și închisă înainte și după fiecare cerere pentru a asigura o comunicare corectă.
Clientul folosește biblioteca PARSON pentru a gestiona datele JSON.
Gestionarea erorilor este implementată pentru a acoperi diverse cazuri limită, cum ar fi intrarea invalidă și erorile serverului.

NOTA  
Am folosit valgrind pentru a detecta și a remedia erorile de memorie. Cea mai mare joaca la capitolul asta a fost in enter_library si la add_book. Asta este si cauza pentru care am intarziat. 

Pentru rulare cu valgrind:
 "valgrind --leak-check=full ./client"

Pentru rulare cu checker:
"python3 checker/checker.py ./client"

Pentru rulare cu checker pe user: flaviu si password: test
"python3 checker/checker.py --user "flaviu:test"  ./client"

