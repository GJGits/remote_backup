<h1 align="center">
  <a href="https://github.com/GJGits/remote_backup" title="Pedibus Documentation">
    <img alt="Pedibus" src="imgs/remote-icon.png" height="100px" />
  </a>
  <br/>
</h1>

<p align="center">
  Sitema di backup remoto
</p>

<p align="center">
 <img alt="Languages" src="https://img.shields.io/badge/Languages-C++ |  HTML | CSS | JS-orange"/>
 <img alt="Framework" src="https://img.shields.io/badge/Framework-Docker | Electron | Boost.Asio-green"/>
</p>

# Introduzione

L'applicazione ha il compito di fornire un sistema di incremental backup del contenuto di una cartella definita dall'utente. Una volta lanciata, un processo in background provvede a sincronizzare il contenuto della cartella scelta dall'utente con un'analoga sul server.

# Indice
* [Team](#Team)
* [Environment(Docker)](#Environment)
    * [Aggiungere docker ad un gruppo (Linux)](Aggiungere_docker_ad_un_gruppo_(Linux))
    * [Interagire con un servizio](#interagire_con_servizio)
    * [Utilizzare MySQL con Docker](#Utilizzare_MySQL_con_Docker)
* [Architettura applicazione](#Architettura_applicazione)
    * [Architettura Frontend](#Architettura_Frontend)
    * [Architettura Backend](#Architettura_Backend)
    * [Architettura DB](#Architettura_DB)
* [REST API](#REST_API)
* [Descrizione processi](#Descrizione_processi) 
    * [Autenticazione](#Autenticazione)
    * [Startup](#Startup)
         * [Utilizzare interfaccia grafica](#Utilizzare_interfaccia_grafica)
         * [Eseguire servizi Docker](#Eseguire_servizi_Docker)
    * [Eventi da monitorare](#Eventi_da_monitorare)
## Team <a name="Team"></a>

- <img src="imgs/marco_nanci_clemente.png" width="32" height="32"/> Marco Nanci Clemente      

- <img src="imgs/alessandra_musone.png" width="32" height="32"/> Alessandra Musone      

- <img src="imgs/piero_gangemi.png" width="32" height="32"/> Piero Gangemi   

## Environment(Docker) <a name="Environment"></a>

L'environment scelto per lo sviluppo dell'applicativo si basa su [Docker](https://www.docker.com/why-docker) [container](https://www.docker.com/resources/what-container). La scelta ricade su questo tipo di tecnologia per i seguenti motivi:

- **virtualizzazione:** tramite virtualizzazione si abbattono le differenze provenienti dai vari sistemi, di conseguenza ogni sviluppatore può interagire con lo stesso identico environment che utilizzeranno anche gli altri. Questo permette di evitare conflitti dovuti a diverse versioni di librerie e permette di evitare il classico problema del *"Funziona sul mio pc"*.

- **filesystem:** la tecnologia dei container si basa sul concetto di [image](https://docs.docker.com/get-started/overview/). Vista da un punto di vista molto astratto un immagine docker non è nient'altro che un filesystem; questo oltre a risolvere il problema di avere diverse versioni di librerie si presta molto al contesto dell'applicazione da sviluppare. Utilizzando diversi container infatti si può testare in maniera semplice la sincronizzazione di una cartella tra versione locale e remota. Ogni container infatti ha il suo filesystem che è isolato da quello degli altri.

- **network:** la tecnologia docker permette di creare in maniera molto comoda delle reti sulle quali i container possono comunicare. In questo modo si possono simulare dei contesti di rete che sono fedeli a quelli nei quali si viene a trovare l'applicativo da distribuire.

- **docker-compose:** [docker compose](https://docs.docker.com/compose/) è un tool che permette in maniera molto semplice di coordinare diversi container, in questo modo è possibile simulare diversi client che agiscono sul sistema, ma anche creare delle repliche del server nell'ottica di rendere più scalabile l'applicazione.

### Aggiungere docker ad un gruppo (Linux) <a name="Aggiungere_docker_ad_un_gruppo_(Linux)"></a>

Per utilizzare comodamente docker su Linux può risultare utile aggiungere docker ad un gruppo. 

1. Creare il gruppo con il comando: `sudo groupadd docker`

2. Aggiungere utente al gruppo docker: `sudo usermod -aG docker $USER`

3. A questo punto effettuare logout e login per rendere effettive le modifiche, alternativamente eseguire il comando: `newgrp docker`

4. Testare eseguendo il comando: `docker run hello-world`.

### Utilizzare la bash di docker per interagire con un servizio <a name="interagire_con_servizio"></a>

Dopo aver lanciato i vari servizi aprire una finestra di terminale ed eseguire il seguente comando:

`docker exec -it remote_backup_<nome_servizio>_<id_progressivo_servizio> /bin/bash`

### Utilizzare MySQL con Docker <a name="Utilizzare_MySQL_con_Docker"></a>

Dopo aver lanciato i vari servizi aprire una finestra di terminale ed eseguire i seguenti comandi in ordine:

1. `docker exec -it remote_backup_db_<id_progressivo_db> /bin/bash`.

2. Una volta entrati all'interno del container, loggarsi tramite il comando `mysql -u root -p`. A questo punto verrà richiesta la password che nel nostro caso è `example`. 

3. Una volta effettuato il login è possibile eseguire comandi SQL a piacere. Alcuni comandi utili:
  
    - `show databases;`: permette di visualizzare quali databases esistono all'interno del server.
  
    - `use <db_name>;`: permette di porsi all'interno di un db, solo da qui sono eseguibili le query per questo determinato db.
    
4. Una volta selezionato il dabase, eseguire tali comandi per interagire col db:
  
    - `show tables;`: permette di elencare le tabelle presenti all'interno del db precedentemente selezionato.

    - `describe <tablename>;`: permette di mostrare la struttura della tabella (nomi e tipo campi...) selezionata.

    - `SELECT * from <tablename>;` : Permette di visualizzare il contenuto della tabella selezionata.

## Architettura applicazione<a name="Architettura_applicazione"></a>


### Architettura Frontend<a name="Architettura_Frontend"></a>

> **Link utili per gestione IPC e tray app:** [menubar](https://github.com/maxogden/menubar), [electron tray](https://www.electronjs.org/docs/api/tray), [ipc main](https://www.electronjs.org/docs/api/ipc-main), [ipc render](https://www.electronjs.org/docs/api/ipc-renderer).

### Architettura Backend<a name="Architettura_Backend"></a>



### Architettura DB<a name="Architettura_DB"></a>

## REST API<a name="REST_API"></a>

<details>
  <summary>POST /auth/signup</summary>
  <br />  
  
  * **Descrizione**:&nbsp;&nbsp;&nbsp;*Endpoint che permette di registrare un nuovo utente, ponendo l'hashed_status = "empty_hashed_status"*  

  * **Authenticated**:&nbsp;&nbsp;&nbsp;`FALSE`
  <br />
  
  * **Parametri**:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{ username : username , password : password , password : password }`  
  <br />  
  
  * **Risposta**:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;In caso negativo viene generato un messaggio HTTP 1.1 400 `{ err_msg : message here }`, in caso positivo invece viene inviata una risposta HTTP 1.1 200 OK con relativo token generato nell'header della risposta.   
</details>
<br />  

<details>
  <summary>POST /auth/signin</summary>
  <br />  
  
  * **Descrizione**:&nbsp;&nbsp;&nbsp;*Endpoint che permette di autenticare un utente precedentemente registrato*  
  
  * **Authenticated**:&nbsp;&nbsp;&nbsp;`FALSE`
  <br />
  
  * **Parametri**:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`{ username : username , password : password }`
  <br />  
  
  * **Risposta**:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;In caso negativo viene generato un messaggio HTTP 1.1 400  `{ err_msg : message here }`, in caso positivo invece viene inviata una risposta HTTP 1.1 200 OK con relativo token generato nell'header della risposta. 
</details>
<br />  

<details>
  <summary>POST /chunk/{chunk_id}/{chunk_hash}/{num_chunks}/{file_pathBASE64}/{local_last_change}</summary>
  <br />  
  
  * **Descrizione**:&nbsp;&nbsp;&nbsp;*endpoint che permette, se il client è autenticato, di aggiungere un file appena creato. `{chunk_id}` corrisponde al numero di chunk che stiamo inviando, 0 per il primo chunk. Il parametro `{chunk_size}` corrisponde alla dimensione del chunk che stiamo inviando, questo corrisponde a `full` se si invia un chunk di dimensione massima (2MB), altrimenti la dimensione in byte.*  

  * **Authenticated**:&nbsp;&nbsp;&nbsp;`TRUE`
  <br /> 

  * **Parametri**:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;HTTP headers: MIME: application/octect-stream body: `binary data here` 
  <br />  
  
  * **Risposta**:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;In caso negativo viene generato un messaggio HTTP 1.1 400 `{ err_msg : message here }`, in caso positivo invece viene inviata una risposta HTTP 1.1 200 OK senza alcun body all'interno.   

  </details> 

<br />

<details>
<summary>GET /status/list/{num_pages}/{timestamp_locale}</summary>

* **Descrizione**:&nbsp;&nbsp;&nbsp;*endpoint che permette di ottenere il JSON file del server con i dettagli sullo stato della cartella remota.*

* **Authenticated**:&nbsp;&nbsp;&nbsp;`TRUE`
  <br /> 

</details>
<br />
<details>
<summary>GET /chunk/{chunk_id}/{file_pathBASE64}</summary>

* **Descrizione**:&nbsp;&nbsp;&nbsp;*endpoint che permette di ottenere un chunk di un file dal server.*

* **Authenticated**:&nbsp;&nbsp;&nbsp;`TRUE`
</details>

## Descrizione processi<a name="Descrizione_processi"></a>

### Autenticazione<a name="Autenticazione"></a>

### Startup<a name="Startup"></a>

#### Utilizzare_interfaccia_grafica <a name="Utilizzare_interfaccia_grafica"></a>

L'interfaccia grafica è rappresentata da una tray app creata utilizzando [Electron](https://www.electronjs.org/docs/tutorial/first-app). Essendo questo un framework basato su [node js](https://nodejs.org/it/) occorre soddisfare alcuni prerequisiti per poter essere utilizzata:

- **node js**: testato con v10.17.0
- **npm**: testato con v6.14.6
- **yarn**: installato con comando `npm install -g yarn` per un'installazione globale

Una volta verificati i prerequisiti posizionarsi nella cartella `ui` ed accertarsi che esista la cartella `node_modules`, altrimenti eseguire il comando `npm install`. A questo punto è possibile eseguire l'applicazione tramite il comando `npm start`.

#### Eseguire servizi Docker <a name="Eseguire_servizi_Docker"></a>

Una volta posizionati nella cartella relativa al progetto tramite terminale, eseguire `docker-compose up --build`. Questo comando permette di eseguire i vari servizi che compongono l'applicativo. La terminazione può essere fatta tranquillamente in maniera ordinata con il comando `CTRL+C`. Nel caso si voglia rendere un servizio scalabile si può apporre un opzione `--scale <nome_servizio>=<num_repliche>` per ogni servizio che si vuole replicare.

> **N.B:** al primo avvio nel file docker-compose, nel servizio di mysql, è necessario commentare l'opzione `command` riguardante il plugin di autenticazione con password e lasciare semplicemnte l'opzione `command` che fa riferimento all'inizializzazione dei db, a questo punto spegnere tramite `CTRL+C` e riavviare con le opzioni invertite (init off e plugin on).

### Eventi da monitorare (ad applicazione accesa)<a name="Eventi_da_monitorare"></a>

Gli eventi da monitorare sono:

- Creazione di un file
- Aggiornamento file
- Eliminazione file esistente
- Rinominazione file
- Error su FileWatcher
