[![LinkedIn](https://img.shields.io/badge/LinkedIn-Simone%20Mattioli-blue?style=for-the-badge&logo=linkedin)](www.linkedin.com/in/simonemattioli2003)
[![YouTube – Simo's Diary](https://img.shields.io/badge/YouTube-Simo's%20Diary-red?style=for-the-badge&logo=youtube)](https://www.youtube.com/@SimosDiary2003)
[![YouTube – Mountain Fauna](https://img.shields.io/badge/Mountain%20Fauna-YT-green?style=for-the-badge&logo=youtube)](https://www.youtube.com/@mountainfaunalover)
[![Instagram Personal](https://img.shields.io/badge/@simo___one-Instagram-purple?style=for-the-badge&logo=instagram)](https://www.instagram.com/simo___one/)
[![Instagram Fauna](https://img.shields.io/badge/@mountainfaunalover-Wildlife%20IG-orange?style=for-the-badge&logo=instagram)](https://www.instagram.com/mountainfaunalover/)

# Corso di Programmazione e Sicurezza delle Reti

> Repository personale di **Simone Mattioli** per il corso _Programmazione e Sicurezza delle Reti_  
> Università degli Studi di Verona · Docente **Davide Quaglia**

---

## Indice
- [Corso di Programmazione e Sicurezza delle Reti](#corso-di-programmazione-e-sicurezza-delle-reti)
  - [Indice](#indice)
  - [Obiettivi del repository](#obiettivi-del-repository)
  - [Requisiti](#requisiti)
    - [Compilazione rapida](#compilazione-rapida)
  - [Struttura delle cartelle](#struttura-delle-cartelle)
  - [Guida rapida](#guida-rapida)
    - [Compilare gli esempi UDP/TCP](#compilare-gli-esempi-udptcp)
    - [Esempio di web‑server HTTP](#esempio-di-webserver-http)
    - [Chat WebSocket](#chat-websocket)
    - [Web‑services REST](#webservices-rest)
    - [Esercitazione MQTT](#esercitazione-mqtt)
    - [Analisi del traffico con Wireshark](#analisi-del-traffico-con-wireshark)
  - [Documentazione teorica](#documentazione-teorica)
  - [TODO](#todo)
  - [Contribuire](#contribuire)
  - [Licenza](#licenza)
  - [Crediti](#crediti)
  - [Link Utili](#link-utili)


---

## Obiettivi del repository
Questo progetto raccoglie **codice sorgente**, **slide**, **appunti** e **tracce di rete** utilizzati durante il corso, con l’obiettivo di:

* Fornire esempi pratici di programmazione di rete in **C**, **Java**, **HTML/JS**.
* Documentare procedure di laboratorio (Wireshark, MQTT, WebSocket, REST).
* Conservare materiali didattici e appunti personali per il ripasso d’esame.

## Requisiti
| Software | Versione minima | Note |
|----------|-----------------|------|
| GCC / Clang | 9.0 | Su Windows consigliato **Cygwin** o **WSL** |
| Make | – | Alcuni esempi includono `Makefile` |
| Wireshark | 4.x | Per l’analisi del traffico |
| Mosquitto | 2.x | Solo per l’esercitazione MQTT |
| Python 3 | 3.10 | Script di supporto opzionali |

> Consulta `Impostazione-PC-applicazioni-socket.pdf` per istruzioni dettagliate su installazione ambiente C su Linux, macOS e Windows.

### Compilazione rapida
```bash
# Esempio UDP server/client
gcc network.c serverUDP.c -o serverUDP -lpthread
gcc network.c clientUDP.c -o clientUDP -lpthread

# Esempio TCP server/client
gcc network.c serverTCP.c -o serverTCP -lpthread
gcc network.c clientTCP.c -o clientTCP -lpthread
```

## Struttura delle cartelle
| Cartella | Descrizione sintetica |
|----------|-----------------------|
| **DOMANDE ESAME/** | Domande esami orali precedenti. |
| **FILE FORNITI DAL PROF/** | File STOCK dal moodle del corso ( senza soluzioni ). |
| **MIEI ESERCIZI/** | Soluzioni personali agli esercizi di laboratorio. |
| **PER ESAME/** | Soluzioni personali + COMMENTI ( Materiale da usare per l'esame ) |
| **APPUNTI GOODNOTES/** | Appunti manoscritti in PDF esportati da GoodNotes. -> COMMING SOON! ( 2,3 days). |
| **LUCIDI/** | Slide del corso in PDF. |
| **.vscode/** | Task e configurazioni per Visual Studio Code. |

## Guida rapida

### Compilare gli esempi UDP/TCP
1. Apri **due terminali**.
2. Nel primo compila ed esegui il server:  
   ```bash
   ./serverUDP      # o ./serverTCP
   ```
3. Nel secondo compila ed esegui il client:  
   ```bash
   ./clientUDP      # o ./clientTCP
   ```
4. Apri **Wireshark**, filtra per porta o protocollo e osserva i pacchetti.

### Esempio di web‑server HTTP
```bash
gcc serverHTTP.c -o serverHTTP -lpthread
./serverHTTP 8000
xdg-open "http://127.0.0.1:8000/"
```

### Chat WebSocket
1. Avvia il server (`websocket_server.c` o equivalente).  
2. Apri `client.html` in un browser moderno.  
3. Digita un messaggio per broadcast a tutti i client connessi.

### Web‑services REST
```bash
# Avvia il server
gcc serverHTTP-REST.c -o rest_server -lpthread
./rest_server 8080

# Chiamata di test (curl):
curl "http://127.0.0.1:8080/api/somma?x=5&y=7"
```

### Esercitazione MQTT
```bash
mosquitto -v              # Avvia il broker
./publisher               # Pubblica valori
./subscriber              # Ascolta e stampa
```

### Analisi del traffico con Wireshark
* Apri il file `.pcapng` corrispondente.
* Usa **Follow TCP/UDP Stream** per ricostruire il dialogo.
* Confronta numeri di sequenza, flag, handshake e ritrasmissioni.

## Documentazione teorica
Nella cartella `LUCIDI/` sono disponibili le slide ufficiali (socket, HTTP, WebSocket, REST, MQTT, architettura TCP/IP).  
Gli appunti personali si trovano in `APPUNTI GOODNOTES/`, mentre `2_ripasso-reti.pdf` fornisce un riassunto rapido dei concetti di rete in vista dell’esame.

## TODO
- [ ] Aggiungere `Makefile` e script CI con **GitHub Actions**.  
- [ ] Automatizzare test di integrazione per i servizi REST.  
- [ ] Trascrivere appunti GoodNotes in Markdown.

## Contribuire
Le _pull‑request_ sono benvenute!  
Per mantenere coerenza:

1. Usa branch descrittivi (`feature/…`, `fix/…`).  
2. Segui la convenzione di stile **clang‑format** fornita in `.clang-format` (work‑in‑progress).  
3. Aggiorna la documentazione quando modifichi o aggiungi funzionalità.

## Licenza
Materiale fornito **solo per uso didattico** (licenza in definizione).  
Contatta l’autore prima di redistribuire in pubblico.

---

## Crediti
- **Simone Mattioli** – Studente di Informatica, Università degli Studi di Verona | Anno corso 2024/2025
- **Davide Quaglia** – Docente del corso ( e pure molto bravo ), Università degli Studi di Verona

## Link Utili
- **[Corso Moodle](https://moodledidattica.univr.it/course/view.php?id=20206)**
- **[Panopto per video Registrazioni](https://univr.cloud.panopto.eu/Panopto/Pages/Viewer.aspx?id=cc809fae-424b-40f2-b7a3-b2fa00ae8042&instance=MoodleDidattica)**
- **[Scrivimi su Linkedin](https://www.linkedin.com/in/simonemattioli2003/)
