#include "../../include/filesystem/directory.hpp"

/**
 * Bisgona inizializzare la variabile statica altrimenti non e' possibile
 * accedere ad una variabile statica all'interno di una funzione non statica.
 * */
std::shared_ptr<Directory> Directory::root =
    std::shared_ptr<Directory>(nullptr);

/**
 * elenca ricorsivamente File e Directory figli della directory
 * corrente, indentati in modo appropriato.
 *
 * @indent: numero di spazi
 */
const void Directory::ls(int indent) {
  for (int i = 0; i < indent; i++) {
    std::clog << " ";
  }

  std::clog << "[+] " << this->getName() << "\n";

  for (std::pair<std::string, std::shared_ptr<Base>> el : this->children) {
    el.second.get()->ls(indent + 4);
  }
}

/**
 * restituisce il tipo dell’istanza (Directory o File) codificato come intero ?
 *
 * @return: tipo oggetto
 */
const int Directory::mType() { return 0; }

/**
 * restituisce uno smart pointer all’oggetto (Directory o File)
 * di nome “name” contenuto nella directory corrente. Se inesistente,
 * restituisce uno shared_ptr vuoto. I nomi speciali “​..​” e “​.”
 * permettono di ottenere rispettivamente lo shared_ptr alla directory genitore
 * di quella corrente, e quello all’istanza stessa.
 *
 * @name: riferimento al nome dell'oggetto da cercare
 *
 * @return: weak_ptr all'oggetto trovato, eventualmente vuoto se non si
 *          riscontra nessun match
 */
std::weak_ptr<Base> Directory::get(const std::string &name) {
  if (name.compare(".") == 0) {
    return std::dynamic_pointer_cast<Base>(this->self.lock());
  } else if (name.compare("..") == 0) {
    return std::dynamic_pointer_cast<Base>(this->parent.lock());
  } else {
    std::unordered_map<std::string, std::shared_ptr<Base>>::const_iterator got =
        this->children.find(name);
    return got == this->children.end() ? std::shared_ptr<Base>() : got->second;
  }
}

/**
 * funziona come il metodo get(nome), facendo un dynamic_pointer_cast
 * dal tipo Base al tipo Directory
 *
 * @name: riferimento al nome dell'oggetto da cercare
 *
 * @return: weak_ptr all'oggetto trovato, eventualmente vuoto se non si
 *          riscontra nessun match
 */
std::weak_ptr<Directory> Directory::getDir(const std::string &name) {
  return std::dynamic_pointer_cast<Directory>(get(name).lock());
}

/**
 * funziona come il metodo get(nome), facendo un dynamic_pointer_cast
 * dal tipo Base al tipo File
 *
 * @name: riferimento al nome dell'oggetto da cercare
 *
 * @return: weak_ptr all'oggetto trovato, eventualmente vuoto se non si
 *          riscontra nessun match
 */
std::weak_ptr<File> Directory::getFile(const std::string &name) {
  return std::dynamic_pointer_cast<File>(get(name).lock());
}

/**
 * crea, se ancora non esiste, l’oggetto di tipo Directory e ne
 * restituisce lo smart pointer.
 *
 * @return: shared_ptr alla cartella root
 */
std::shared_ptr<Directory> Directory::getRoot() {
  if (Directory::root.get() == nullptr) {
    Directory::root = Directory::makeDir("/", Directory::root);
  }
  return Directory::root;
}

/**
 * crea un nuovo oggetto di tipo Directory, il cui nome è desunto dal parametro,
 * e lo aggiunge alla cartella corrente. Se risulta già presente, nella cartella
 * corrente, un oggetto con il nome indicato, restituisce uno smart pointer
 * vuoto lo stesso vale per i nomi riservati ‘.’ e ‘..’
 *
 * @name: nome oggetto da creare
 *
 * @return: shared_ptr all'oggetto appena creato
 */
std::shared_ptr<Directory> Directory::addDirectory(const std::string &name) {
  if (name.compare(".") != 0 && name.compare("..") != 0 &&
      this->children.find(name) == this->children.end()) {
    std::shared_ptr<Directory> dir = Directory::makeDir(name, this->self);
    this->children.insert(
        std::make_pair(name, std::dynamic_pointer_cast<Base>(dir)));
    return dir;
  } else
    return std::shared_ptr<Directory>();
}

/**
 * aggiunge alla Directory un nuovo oggetto di tipo File, ricevendone
 * come parametri il nome e la dimensione in byte; l’aggiunta di un File con
 * nome già presente nella cartella corrente non è permessa e fa restituire
 * uno smart pointer vuoto lo stesso vale se si tenta di creare un file con
 * uno dei nomi riservati ‘.’ e ‘..’.
 *
 * @name: riferimento al nome del file da creare
 * @size: dimensione in byte del file
 *
 * @return: shared_ptr all'oggetto appena creato
 */
std::shared_ptr<File> Directory::addFile(const std::string &name,
                                         uintmax_t size) {
  if (name.compare(".") != 0 && name.compare("..") != 0 &&
      this->children.find(name) == this->children.end()) {
    std::shared_ptr<File> file(File::makeFile(name, size));
    this->children.insert(
        std::make_pair(name, std::dynamic_pointer_cast<Base>(file)));
    return file;
  } else
    return std::shared_ptr<File>();
}

/**
 * rimuove dalla collezione di figli della directory corrente l’oggetto
 * (Directory o File) di nome “nome”, se esiste, restituendo true.
 * Se l’oggetto indicato non esiste o se si tenta di rimuovere “​..​” e
 * “​.” viene restituito false.
 *
 * @name: riferimento al nome dell'oggetto da eliminare
 *
 * @return: boolean che indica se l'eliminazione e' andata a buon fine
 */
bool Directory::remove(const std::string &name) {
  return this->children.erase(name) == 1;
}

/**
 * Factory method che permette di creare una cartella dato il nome e il parent.
 *
 * @name: nome della cartella da creare
 * @parent: weak_ptr alla cartella parent
 *
 * @return: shared_ptr che si riferisce alla cartella appena creata
 */
std::shared_ptr<Directory> Directory::makeDir(std::string name,
                                              std::weak_ptr<Directory> parent) {
  // todo: attualmente il metodo e' invocabile dall'esterno, risolvere!
  Directory *d = new Directory(name);
  std::shared_ptr<Directory> dir(d);
  dir->self = dir;
  dir->parent = parent.lock().get() == nullptr
                    ? dir
                    : parent; // parent e' nullptr solo in caso di root
  return dir;
}