[![LinkedIn](https://img.shields.io/badge/LinkedIn-Simone%20Mattioli-blue?style=for-the-badge&logo=linkedin)](https://www.linkedin.com/in/simonemattioli2003/)
[![YouTube – Simo's Diary](https://img.shields.io/badge/YouTube-Simo's%20Diary-red?style=for-the-badge&logo=youtube)](https://www.youtube.com/@SimosDiary2003)
[![YouTube – Mountain Fauna](https://img.shields.io/badge/Mountain%20Fauna-YT-green?style=for-the-badge&logo=youtube)](https://www.youtube.com/@mountainfaunalover)
[![Instagram Personal](https://img.shields.io/badge/@simo___one-Instagram-purple?style=for-the-badge&logo=instagram)](https://www.instagram.com/simo___one/)
[![Instagram Fauna](https://img.shields.io/badge/@mountainfaunalover-Wildlife%20IG-orange?style=for-the-badge&logo=instagram)](https://www.instagram.com/mountainfaunalover/)

# Corso di Programmazione e Sicurezza delle Reti

> Repository personale di **Simone Mattioli** per il corso _Programmazione e Sicurezza delle Reti_  
> Università degli Studi di Verona · Docente **Prof. Davide Quaglia** · A.A. 2024/2025

---

## 📚 Indice
- [Corso di Programmazione e Sicurezza delle Reti](#corso-di-programmazione-e-sicurezza-delle-reti)
  - [📚 Indice](#-indice)
  - [🎯 Obiettivi del repository](#-obiettivi-del-repository)
  - [⚙️ Requisiti di sistema](#️-requisiti-di-sistema)
    - [🚀 Compilazione rapida](#-compilazione-rapida)
  - [📁 Struttura del repository](#-struttura-del-repository)
    - [🔍 Dettaglio delle sottocartelle principali](#-dettaglio-delle-sottocartelle-principali)
  - [🛠️ Guida rapida all'utilizzo](#️-guida-rapida-allutilizzo)
    - [📡 Socket UDP/TCP](#-socket-udptcp)
    - [🌐 Server HTTP](#-server-http)
    - [💬 Chat WebSocket](#-chat-websocket)
    - [🔄 API REST](#-api-rest)
    - [📨 Protocollo MQTT](#-protocollo-mqtt)
  - [📊 Analisi del traffico con Wireshark](#-analisi-del-traffico-con-wireshark)
  - [📖 Documentazione teorica](#-documentazione-teorica)
    - [📋 Slide del corso](#-slide-del-corso)
    - [📝 Appunti personali](#-appunti-personali)
  - [🔮 Sviluppi futuri (Roadmap)](#-sviluppi-futuri-roadmap)
    - [TODO List](#todo-list)
  - [🤝 Contribuire](#-contribuire)
    - [Linee guida per i contributi](#linee-guida-per-i-contributi)
    - [Processo di contribuzione](#processo-di-contribuzione)
  - [📄 Licenza](#-licenza)
  - [👥 Crediti](#-crediti)
    - [Autore](#autore)
    - [Docente del corso](#docente-del-corso)
  - [🔗 Link utili](#-link-utili)
    - [Repository e codice](#repository-e-codice)
    - [Piattaforme didattiche](#piattaforme-didattiche)
    - [Strumenti e software](#strumenti-e-software)
    - [Documentazione di riferimento](#documentazione-di-riferimento)

---

## 🎯 Obiettivi del repository

Questo progetto rappresenta una **collezione organizzata** di materiali didattici, codice sorgente, appunti e risorse pratiche sviluppate durante il corso nell'anno accademico 2024/2025, con l'obiettivo di:

- **📝 Esempi Pratici:** Fornire implementazioni concrete di programmazione di rete utilizzando diversi linguaggi (C, Java, HTML/JavaScript)
- **🔬 Documentazione di Laboratorio:** Documentare procedure pratiche per l'utilizzo di strumenti come Wireshark, MQTT, WebSocket e REST
- **📖 Materiale di Studio:** Conservare materiali didattici, appunti personali e risorse per la preparazione all'esame
- **🔗 Riferimento Completo:** Creare un punto di riferimento organizzato per tutti gli argomenti del corso

## ⚙️ Requisiti di sistema

Per utilizzare completamente i materiali presenti nel repository:

| Software | Versione Minima | Note |
|----------|-----------------|------|
| **GCC / Clang** | 9.0 | Su Windows è consigliato utilizzare **Cygwin** o **WSL** |
| **Make** | – | Alcuni esempi includono `Makefile` per la compilazione automatica |
| **Wireshark** | 4.x | Strumento fondamentale per l'analisi del traffico di rete |
| **Mosquitto** | 2.x | Necessario solamente per le esercitazioni MQTT |
| **Python 3** | 3.10 | Per gli script di supporto opzionali |

> ⚠️ **Importante:** Per istruzioni dettagliate sull'installazione dell'ambiente C su Linux, macOS e Windows, consultare il file `Impostazione-PC-applicazioni-socket.pdf` presente nel repository.

### 🚀 Compilazione rapida

```bash
# Esempi UDP server/client
gcc network.c serverUDP.c -o serverUDP -lpthread
gcc network.c clientUDP.c -o clientUDP -lpthread

# Esempi TCP server/client
gcc network.c serverTCP.c -o serverTCP -lpthread
gcc network.c clientTCP.c -o clientTCP -lpthread
```

## 📁 Struttura del repository

| Cartella | Descrizione |
|----------|-------------|
| **`DOMANDE ESAME/`** | Raccolta di domande degli esami orali delle sessioni precedenti |
| **`FILE FORNITI DAL PROF/`** | File originali forniti dal docente tramite la piattaforma Moodle del corso (senza soluzioni) |
| **`MIEI ESERCIZI/`** | Soluzioni personali sviluppate dall'autore per gli esercizi di laboratorio |
| **`PER ESAME/`** | Soluzioni commentate e materiale ottimizzato per la preparazione all'esame |
| **`APPUNTI GOODNOTES/`** | Appunti manoscritti in formato PDF esportati da GoodNotes |
| **`LUCIDI/`** | Slide ufficiali del corso in formato PDF |
| **`.vscode/`** | Configurazioni e task per Visual Studio Code |

### 🔍 Dettaglio delle sottocartelle principali

**`PER ESAME/`** è organizzato in:
- **`CLIENT-SERVER/`**: Implementazioni TCP/UDP (sommatrici, trasferimento file, gestione richieste multiple)
- **`WEB e WEBSERVICES/`**: Server HTTP, CGI, WebSocket, REST API, MQTT pub-sub

**`MIEI ESERCIZI/`** contiene:
- **`Esempi-client-server/`**: Implementazioni base UDP/TCP
- **`Esempi-web/`**: Server HTTP, form, JavaScript
- **`Webservice/`**: API REST in C e Java
- **`web services/`**: CGI, polling, upload file

## 🛠️ Guida rapida all'utilizzo

### 📡 Socket UDP/TCP

**Test dei socket:**
1. Apri **due terminali** separati
2. Nel primo terminale, compila ed esegui il server:
   ```bash
   ./serverUDP      # oppure ./serverTCP
   ```
3. Nel secondo terminale, compila ed esegui il client:
   ```bash
   ./clientUDP      # oppure ./clientTCP
   ```
4. Utilizza **Wireshark** per filtrare per porta o protocollo e osservare i pacchetti scambiati

### 🌐 Server HTTP

```bash
# Compilazione
gcc serverHTTP.c -o serverHTTP -lpthread

# Esecuzione sulla porta 8000
./serverHTTP 8000

# Apertura nel browser
xdg-open "http://127.0.0.1:8000/"
```

### 💬 Chat WebSocket

Per testare le comunicazioni WebSocket:
1. Avviare il server (file `websocket_server.c` o equivalente)
2. Aprire il file `client.html` in un browser moderno
3. Utilizzare l'interfaccia per inviare messaggi broadcast a tutti i client connessi

### 🔄 API REST

```bash
# Compilazione e avvio del server REST
gcc serverHTTP-REST.c -o rest_server -lpthread
./rest_server 8080

# Test con curl
curl "http://127.0.0.1:8080/api/somma?x=5&y=7"
```

### 📨 Protocollo MQTT

```bash
# Avvio del broker Mosquitto
mosquitto -v

# Esecuzione del publisher
./publisher

# Esecuzione del subscriber
./subscriber
```

## 📊 Analisi del traffico con Wireshark

Il repository include file di cattura (`.pcapng`) per l'analisi del traffico di rete:

1. Aprire il file `.pcapng` corrispondente all'esercizio
2. Utilizzare la funzione **"Follow TCP/UDP Stream"** per ricostruire il dialogo completo
3. Analizzare i numeri di sequenza, i flag, l'handshake e le eventuali ritrasmissioni
4. Confrontare il comportamento osservato con quello teorico atteso

## 📖 Documentazione teorica

### 📋 Slide del corso
Nella cartella `LUCIDI/` sono disponibili le slide ufficiali che coprono:
- Programmazione socket
- Protocollo HTTP  
- Tecnologie WebSocket
- Architetture REST
- Protocollo MQTT
- Architettura TCP/IP
- Sicurezza delle reti

### 📝 Appunti personali
- **`APPUNTI GOODNOTES/`**: Appunti personali dell'autore esportati da GoodNotes
- **`2_ripasso-reti.pdf`**: Riassunto rapido e conciso dei concetti fondamentali di rete in vista dell'esame

## 🔮 Sviluppi futuri (Roadmap)

### TODO List
Il repository è in continua evoluzione. Gli sviluppi futuri pianificati includono:

- [ ] **Automazione Build:** Aggiunta di `Makefile` e script CI con GitHub Actions
- [ ] **Test di Integrazione:** Automatizzazione dei test di integrazione per i servizi REST  
- [ ] **Documentazione:** Trascrizione degli appunti GoodNotes in formato Markdown per una migliore accessibilità
- [ ] **Espansione Contenuti:** Aggiunta di ulteriori esempi pratici e casi d'uso avanzati

## 🤝 Contribuire

### Linee guida per i contributi
Il repository accoglie contributi dalla comunità. Per mantenere la coerenza del progetto:

1. Utilizzare branch descrittivi (es. `feature/nuova-funzionalità`, `fix/correzione-bug`)
2. Seguire la convenzione di stile `clang-format` fornita nel file `.clang-format` (work-in-progress)
3. Aggiornare la documentazione quando si modificano o aggiungono funzionalità
4. Testare accuratamente le modifiche prima di sottomettere una pull-request

### Processo di contribuzione
1. Fork del repository
2. Creazione di un branch per la propria modifica  
3. Implementazione delle modifiche
4. Test delle funzionalità
5. Sottomissione di una pull-request con descrizione dettagliata

## 📄 Licenza

> ⚠️ **Importante:** Il materiale è fornito **esclusivamente per uso didattico**. La licenza è attualmente in fase di definizione. È necessario contattare l'autore prima di ridistribuire pubblicamente il contenuto del repository.

## 👥 Crediti

### Autore
**Simone Mattioli**
- Studente di Informatica, Università degli Studi di Verona
- Sviluppatore del repository
- Anno di corso: 2024/2025

### Docente del corso
**Prof. Davide Quaglia**
- Docente del corso di Programmazione e Sicurezza delle Reti
- Università degli Studi di Verona
- Supervisione accademica del progetto

## 🔗 Link utili

### Repository e codice
- **[Repository GitHub Principale](https://github.com/simo-hue/Corso-Programmazione-Sicurezza-delle-Reti)**
- **[Profilo GitHub dell'autore](https://github.com/simo-hue)**
- **[LinkedIn dell'autore](https://www.linkedin.com/in/simonemattioli2003/)**

### Piattaforme didattiche
- **[Corso Moodle](https://moodledidattica.univr.it/course/view.php?id=20206)**
- **[Panopto - Registrazioni video](https://univr.cloud.panopto.eu/Panopto/Pages/Viewer.aspx?id=cc809fae-424b-40f2-b7a3-b2fa00ae8042&instance=MoodleDidattica)**
- **[NotebookLM - Appunti AI](https://notebooklm.google.com/notebook/1d3c8cae-cd8c-4ed7-9b1b-d9baa4fee5d7)**

### Strumenti e software
- **[Wireshark](https://www.wireshark.org/)** - Analizzatore di protocolli di rete
- **[Eclipse Mosquitto](https://mosquitto.org/)** - Broker MQTT
- **[GCC](https://gcc.gnu.org/)** - GNU Compiler Collection
- **[Visual Studio Code](https://code.visualstudio.com/)**

### Documentazione di riferimento
- **RFC 793** - Transmission Control Protocol
- **RFC 768** - User Datagram Protocol
- **RFC 2616** - Hypertext Transfer Protocol HTTP/1.1
- **RFC 6455** - The WebSocket Protocol
- **RFC 3986** - Uniform Resource Identifier (URI)

---

> 💡 **Per domande o suggerimenti:** [Scrivimi su LinkedIn](https://www.linkedin.com/in/simonemattioli2003/)

*Ultima revisione: Luglio 2025*