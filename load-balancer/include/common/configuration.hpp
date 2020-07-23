
/**
 * Classe che serve a racchiudere le configurazioni lette da file.
 * La classe deve essere un signleton e instanziata all'avvio dell'applicazione. 
 * Un oggetto di questa classe deve permettere di accedere ad un campo 'key' nel seguente
 * modo: value = conf["key"]. La classe deve rimanere più generale possibile perché i 
 * campi all'interno del file di configurazione potrebbero cambiare.
 * 
 * esempi di utilizzo:
 * 
 * 1. accedere al campo "servers": value = conf["servers"];
 * 2. accedere al campo "token-conf.kd_alg": value = conf["token-conf.kd_alg"];
 */



class Configuration {

};