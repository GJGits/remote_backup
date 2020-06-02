# Remote Backup

Descrizione progetto qui...

## Environment

L'environment scelto per lo sviluppo dell'applicativo si basa su [Docker](https://www.docker.com/why-docker) [container](https://www.docker.com/resources/what-container). La scelta ricade su questo tipo di tecnologia per i seguenti motivi:

- **virtualizzazione:** tramite virtualizzazione si abbattono le differenze provenienti dai vari sistemi, di conseguenza ogni sviluppatore può interagire con lo stesso identico environment che utilizzeranno anche gli altri. Questo permette di evitare conflitti dovuti a diverse versioni di librerie e permette di evitare il classico problema del *"Funziona sul mio pc"*.

- **filesystem:** la tecnologia dei container si basa sul concetto di [image](https://docs.docker.com/get-started/overview/). Vista da un punto di vista molto astratto un immagine docker non è nient'altro che un filesystem, questo oltre a risolvere il problema di avere diverse versioni di librerie si presta molto al contesto di questa applicazione. Utilizzando diversi container infatti si può testare in maniera semplice la sincronizzazione di una cartella tra versione locale e remota. Ogni container infatti ha il suo filesystem che è isolato da quello degli altri.

- **network:** la tecnologia docker permette di creare in maniera molto comoda delle reti sulle quali i container possono comunicare. In questo modo si possono simulare dei contesti di rete che sono fedeli a quelli nei quali si viene a trovare l'applicativo da distribuire.

- **docker-compose:** [docker compose](https://docs.docker.com/compose/) è un tool che permette in maniera molto semplice di coordinare diversi container, in questo modo è possibile simulare diversi client che agiscono sul sistema, ma anche creare delle repliche del server nell'ottica di rendere più scalabile l'applicazione.

### Utilizzare applicazione con Docker

Una volta posizionati nella cartella relativa al progetto, eseguire `docker-compose up --build`. La terminazione può essere fatta tranquillamente in maniera ordinata con il comando `CTRL+C`.

## Architettura applicazione

L'archetettura ad alto livello delle componenti che compongono l'applicazione è la seguente.

![](imgs/design.png)

Il supporto C++ alle varie componenti dell'applicazione viene fornito dalle librerie standard e da [boost](https://www.boost.org/), di conseguenza dove possibile viene preferita un'implementazione già fornita all'interno di questa collezione di librerie piuttosto che una custom. Le comunicazioni tra client e server avvengono tramite TCP/IP e le comunicazioni a livello applicativo avvengono tramite HTTP con preferenza JSON per la rappresentazione dei messaggi. La scelta ricade su queste tecnologie in quanto ormai sono di fatto uno standard ed ampiamente utilizzate in diversi contesti applicativi, oltre assere semplici, versatili e portabili. Il server a tale scopo espone un API con tutti i metodi necessari, descritti nel seguente paragrafo, ad erogare il servizio di sincronizzazione. Un ultimo modulo che troviamo è un DB che serve per memorizzare informazioni relative agli utenti, allo stato dei file ecc. (si rimanda per i dettagli alla sezione sul DB).

### Architettura Frontend

L'applicativo lato client presenta due processi, uno serve a presentare l'interfaccia (un menù da console), il secondo invece svolge le operazioni di monitoring e serve a comunicare con il server. I due processi comunicano tramite pipe, il menù invia i comandi da effettuare e riceve eventualmente indietro dei messaggi dal processo che comunica con il server. La scelta di sdoppiare i processi risiede sia nel fatto che in questo modo l'applicazione è più manutenibile (si potrebbe passare da un'interfaccia da linea di comando ad una grafica in futuro), sia perché in questo modo è gestibile in maniera più ordinata sia la chiusura del programma che un eventuale messa in background del processo che lavora con il server.

### Architettura Backend

L'architettura lato backend presenta un processo che serve a ricevere richieste da parte dall'utente e n processi che fungono da controller. Il numero di controller è dato dal numero di subpath di primo livello presenti nelle url dell'API esposta al client. Se le URL esposte sono per esempio: - `\foo \bar \foo\paz` al momento di startup l'applicazione creerà tre processi, un processo padre e due controller figli per rispondere alle richieste ricevute rispettivamente per - `\foo` e per `\bar`. Anche questo tipo di scelta deriva da una struttura abbastanza consolidata e che riguarda gli applicativi backend che ormai, a prescindire dal linguaggio utilizzato, sfruttano la suddivizione delle richieste tramite dei controller. Questo permette ovviamente di aumentare la modularità e di rendere l'applicativo più scalabile. L'informazione su quanti e quali processi creare viene letta in fase di startup da un apposito file di configurazione `server-conf.json`.

> **Approfondimento:** valutare uso di load balancer, in questo caso il server esposto al client servirebbe solo da bilanciatore di carico, questo inoltrerebbe le richieste ai veri server che avrebbero la stessa struttura indicata prima, con la differenza che ora dovrebbero comunicare in qualche modo il loro stato di carico.

### Architettura DB

Descrivere DB (scelta tra MySql e MongoDB) engine scelta qui e motivare scelta.

**tabella username:** In questa tabella troviamo un id interno che rappresenta un utente, un username scelto dall'utente stesso, la password memorizzata memorizzata tramite hash, il sale e il campo più importante che è l'hashed_status, questo rappresenta lo stato della cartella di riferimento dell'utente. Tramite questo campo si riesce a capire se sono stati effettuati cambiamenti all'interno della cartella. Questo campo viene calcolato a partire da checksum di riferimento dei vari file e directory presenti all'interno della directory dell'utente.

| id | username | password | sale | hashed_status |
|:--:|:--------:|:--------:| :--: |:-------------:|
| 0 | myuser | hash_value | 3 |ahjsdkfjh343! |



## Descrizione processi

Le varie azioni possibili sono riassunte di seguito in tabella. Ad ogni azione viene assegnato un codice che rimanda al sottoparagrafo dove vengono spiegati i vari dettagli.

| id | comando | descrizione | parametri | risposta |
| :--: | :--: | :--: | :--: | :--: |
| 0 | POST /signup | endpoint che permette di registrare un nuovo utente. | `{"user":"username", "pass1":"password","pass2":"password"}` | in caso positivo HTTP 1.1 200 `{"access_token":"xxxxx.yyyyy.zzzzz"}`, in caso negativo HTTP 1.1 400 `{"err_msg":"message here"}` |
| 1 | POST /signin | endpoint che permette di autenticare un utente precedentemente registrato | `{"user":"username", "pass1":"password"}` | come sopra |

### Autenticazione

L'autenticazione all'interno dell'applicazione si basa su [JWT](https://jwt.io/introduction/). Le informazioni relative all'autenticazione vengono memorizzate sia lato client che lato server. Lato client viene memorizzato il token in un apposito file `token.json` che si presenta nel seguente modo:

```json
{
    "access_token":"xxxxx.yyyyy.zzzzz"
}
```
Lato server nel file di configurazione `server-conf.json`:

```json
{
    ...,
    "token-conf":
    {
    "kd_alg":"hmac-sha256",
    "expiration":172800000,
    "secret":"key_here"
    },
    ...
}

```

 L'autenticazione avviene solamente in due casi, al momento della registrazione e quando un token scade e va rinnovato. Per registrarsi un utente seleziona il comando `-r` dal menù, inserisce le informazioni necessarie ed invia il comando `/signup` al server.

![](imgs/signup.png)

Una volta ottenuto un token dal server il client ha la possibilità di sfruttare i comandi offerti dal server. Una generica richiesta del client quindi viene autenticata nel seguente modo.

![](imgs/tok-req.png)

Se il client ha già effettuato la registrazione, ma possiede un token scaduto allora esso dovrà selezionare la voce `-a` dal menù per autenticarsi. Da console inserirà quindi username e password ed invierà una richiesta `/signin` al server. Questo dovrà verificare le credenziali ricevute interrogando un database e se l'autenticazione va a buon fine esso invierà un token all'utente.

![](imgs/sign-in.png)

Per verificare le credenziali del client il server recupera l'hash ed il sale con cui è stato calcolato dal db, a questo punto procede a calcolare l'hash tramite algoritmo sha-256 e il sale recuperato, se le due stringhe coincidono allora l'utente è da considerarsi autenticato ed il server procederà con la generazione di un token.