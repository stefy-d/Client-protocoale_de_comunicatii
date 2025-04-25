<div align="left">

# **Client**
### Manea Ștefania-Delia
</div>
<br>

### **Descriere generală:** 
**Această aplicație client permite interacțiunea cu un server pentru diverse
operațiuni legate de utilizatori și gestionarea unei biblioteci de cărți. Aplicația 
utilizează socket-uri pentru comunicare și JSON pentru schimbul de date.**

### **Funcționalități:**
1. **Register:**
Înregistrează un nou utilizator trimițând o cerere *POST* cu numele de
utilizator și parola în format *JSON* către server.
2. **Login:** Autentifică un utilizator existent trimițând o cerere *POST* cu
numele de utilizator și parola. La autentificarea cu succes, primește și stochează
cookie-uri pentru gestionarea sesiunii.
3. **Enter library:** Accesează biblioteca trimițând o cerere *GET* cu
cookie-urile stocate. La succes, primește și stochează un token *JWT* pentru
cereri autentificate ulterioare.
4. **Get books:** Primește lista de cărți din bibliotecă trimițând o cerere
*GET* cu token-ul *JWT* primit anterior.
5. **Get book:** Primește detaliile unei cărți trimițând o cerere *GET* cu
token-ul *JW*T primit anterior și id-ul cărții citit de la tastatură.
6. **Add book:** Adaugă o carte în bibliotecă trimițând o cerere *POST* cu token-ul
*JWT* primit anterior și detaliile cărții în format *JSON* citite de la tastatură.
7. **Delete book:** Șterge o carte din bibliotecă trimițând o cerere *DELETE* cu
token-ul *JWT* primit anterior și id-ul cărții citit de la tastatură.
8. **Logout:** Deconectează utilizatorul trimițând o cerere *GET* cu cookie-urile
stocate. La succes, șterge cookie-urile și token-ul *JWT*.

### **Flow-ul aplicației:**

Bucla principală a programului citește comenzi de la utilizator și execută funcțiile 
corespunzătoare pentru fiecare comandă *(register, login, enter_library, get_books, 
get_book, add_book, delete_book, logout, exit)*. Aplicația rămâne în buclă până la 
comanda **exit**. 
+ Pentru comenzile, **login, enter_library și logout** se folosesc cookie-uri, 
pentru a verifica dacă utilizatorul este autentificat. Dacă utilizatorul este 
autentificat, se folosește un token *JWT* pentru a permite accesul la utilizarea 
celorlalte comenzi. La *login* sunt extrase cookie-urile din header-ul răspunsului 
și la *enter_library* este extras token-ul *JWT* din conținutul răspunsului.
+ Pentru comenzile **get_books, get_book, add_book și delete_book** se folosește 
un token *JWT* pentru a demonstra că există permisiuni pentru executarea acestor 
comenzi. La *add_book* se citesc de la tastatură detaliile cărții și se construiește 
un *JSON* cu acestea, după ce se verifică dacă s-a primit un număr pentru *page_count*, 
folosind funcția **valid_page_count**.
Pentru fiecare comandă, se deschide conexiunea, se construiește request-ul 
*HTTP* care este trimis către server (*send_to_server*) și se primește un 
răspuns(*receive_from_server*), pe baza căruia se afișează un mesaj corespunzător. 
La final, se închide conexiunea.

### **Funcții pentru comunicarea cu serverul:**
1. **open_connection:** Această funcție creează un socket, configurează adresa 
serverului și încearcă să se conecteze la acesta. Dacă conexiunea eșuează, afișează 
un mesaj de eroare și termină execuția programului.
2. **close_connection:** Aceasta închide conexiunea la server prin închiderea socket-ului specificat.
3. **send_to_server:** Această funcție trimite un request *HTTP* la server,folosind 
un loop pentru a se asigura că tot conținutul mesajului este transmis și verifică dacă 
acesta a fost trimis cu succes. Dacă nu, afișează un mesaj de eroare și închide conexiunea.
4. **receive_from_server:** Primește răspunsul de la server și construiește răspunsul 
complet. Aceasta primește date de la server și le adaugă într-un buffer, căutând 
header-ul pentru a determina lungimea conținutului și citind restul datelor 
până când toate sunt primite. 
+ Utilizează funcția **find_substring** pentru a căuta un șir de caractere într-un alt șir de caractere.

### **Funcții pentru construirea request-urilor:**
1. **register_and_login:** Construiește un request *HTTP* de tip *POST* pentru 
înregistrare și autentificare, folosind un *JSON* cu numele de utilizator și parola. Returnează mesajul construit. 
2. **enter_library:** Construiește un request *HTTP* de tip *GET* pentru a accesa 
biblioteca. Include cookie-urile pentru autentificare. Returnează mesajul construit.
3. **get_books:** Construiește un request *HTTP* de tip *GET* pentru a primi lista 
de cărți din bibliotecă. Include token-ul *JWT* pentru autentificare. Returnează mesajul construit. 
4. **get_book:** Construiește un request *HTTP* de tip *GET* pentru a primi detaliile 
unei cărți din bibliotecă. Include token-ul *JWT* și id-ul cărții. Returnează mesajul construit.
5. **add_book:** Construiește un request *HTTP* de tip *POST* pentru a adăuga o carte în 
bibliotecă. Include token-ul *JWT* și detaliile cărții, folosind un *JSON*. Returnează mesajul construit.
6. **delete_book:** Construiește un request *HTTP* de tip *DELETE* pentru a șterge 
o carte din bibliotecă. Include token-ul *JWT* și id-ul cărții. Returnează mesajul construit.
7. **logout:** Construiește un request *HTTP* de tip *GET* pentru a deconecta utilizatorul. 
Include cookie-urile pentru autentificare. Returnează mesajul construit.

### **Biblioteca nlohmann/json:**
+ Am folosit această bibliotecă pentru a putea folosi obiecte de tip *JSON* în C++. 
Aceasta oferă avantajul de a putea manipula ușor datele în format *JSON* și de a 
le transforma în obiecte C++ și invers.
