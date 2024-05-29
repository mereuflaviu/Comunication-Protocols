Exerciții

1. Vrem să ne pregătim pentru a trimite date în format binar peste un mediu de transmisie. În fișierul de aici avem un array de structuri de tipul Packet în format binar. Realizați un program în C/C++ care să citească array-ul cu elemente de tip Packet din acest fișier și să afișeze conținutul din payload al fiecărei intrări. Procesul prin care acest fișier a fost creat se numește serializare și îl vom întâlni atunci când vom serializa datele pentru a fi trimise spre rețea. Procesul invers, pe care îl veți implementa, se numește deserializare.

struct Packet {
    char payload[100];
    int sum;
    int size;
};

2. Vom folosi telnet -4 telehack.com pentru a ne conecta la un server TCP. In prompt o să scriem starwars si enter. Acest server trimite un șir de biți către noi, iar telnet îl afișează pe ecran (stdout). Dacă totul a mers bine, ar trebui să vedeți prima scenă din Star Wars în format ASCII.

3. Folosind Wireshark vom analiză traficul generat de către comanda telnet de la exercițiul precedent. Identificați un pachet ce conține bucăți din textul ce apare în terminal.

Note

În exemplele de filtre pe care le-am văzut până acuma erau utilizate doar adrese IP (e.g. ip.addr == 127.0.0.1). Pentru a afla adresa ip a telehack.com vom folosi host telehack.com.

4. Pentru a ne obișnui cu programarea low level, scrieți un utilitar similar cu cat în C/C++. Acesta trebuie să afișeze conținutul unui fișier, linie cu linie, la stdout. Vom folosi API-ul direct peste file descriptors (e.g. read, open).
