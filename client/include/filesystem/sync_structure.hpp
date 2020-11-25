#pragma once

#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <unordered_map>

#include "../common/constants.hpp"
#include "../common/json.hpp"
#include "../common/singleton.hpp"
#include "file_entry.hpp"

using json = nlohmann::json;

class SyncStructure : public Singleton<SyncStructure> {

private:
  friend class Singleton;
  std::mutex mx;
  size_t server_news;
  bool server_ack;
  std::unordered_map<std::string, std::shared_ptr<FileEntry>> structure;
  size_t last_check;

  SyncStructure();

public:
  ~SyncStructure();
  /**
   * Aggiorna il file client-struct.json
   **/
  void store();

  /**
   * Effettua restore struttura da file
   * */
  void restore();

  /**
   * Effettua scan su filesystem ed aggiorna la struttura
   **/
  void update_from_fs();

  /**
   * Chiede aggiornamenti al server ed aggiorna la struttura.
   **/
  void update_from_remote();

  /**
   * Aggiunge entry alla struttura e permette di aggiornare
   * il conto dei chunk trasferiti
   * */
  void add_entry(const std::shared_ptr<FileEntry> &entry);

  /**
   * Rimuove entry dalla struttura
   * */
  void remove_entry(const std::shared_ptr<FileEntry> &entry);

  /**
   * Data un path recupera l'entry associata.
   **/
  std::optional<std::shared_ptr<FileEntry>> get_entry(const std::string &path);

  /**
   * Restituisce lista path contenuti in structure
   **/
  std::vector<std::string> get_paths();

  /**
   * Restituisce last_check
   **/
  size_t get_last_check() const;

  /**
   * Restituisce lista file_entry contenuti in structure
   **/
  std::vector<std::shared_ptr<FileEntry>> get_entries();

  /**
   * Restituisce il numero di cambiamenti notificati dal server
   **/
  size_t get_remote_news() const;

  /**
   * Resetta il numero di cambiamenti
   **/
  void reset_remote_news();
};
