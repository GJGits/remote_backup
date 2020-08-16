//
// Created by alessandra on 30/07/20.
//

#include "../../include/filesystem/handle_watcher.hpp"
#include "../../include/filesystem/linux_watcher.hpp"
#include "../../include/filesystem/sync_structure.hpp"

HandleWatcher *HandleWatcher::getInstance() {
    if (instance == nullptr) {
        instance = new HandleWatcher();
        //instance->set_toclean();
        instance->run_workers();
    }
    return instance;
}

HandleWatcher::~HandleWatcher() {
    finish = true;
    for (size_t i = 0; i < workers.size(); i++) {
        workers[i].join();
    }
}

void HandleWatcher::run_workers() {

        for (size_t i = 0; i < 8; i++) {

            workers.emplace_back([&]() {
            try {
                while (!finish) {
                    std::unique_lock lk{m};

                    if (!events.empty()) {
                        Event event = events.front();
                        events.pop();
                        lk.unlock();
                        switch (event.getType()) {
                            case EVENT_TYPE::CREATE:
                                handle_InCloseWrite(event.getArgument1());
                                break;
                            case EVENT_TYPE::DELETE:
                                handle_InDelete(event.getArgument1());
                                break;
                            case EVENT_TYPE::EXPAND:
                                handle_expand(event.getArgument1());
                                break;
                            case EVENT_TYPE::RENAME:
                                std::clog << "Faccio Rename\n";

                                std::clog << "Event 1: " << event.getArgument1() << " " << "Event 2: "
                                          << event.getArgument2() << "\n";
                                if (event.getArgument1().size() == 0) {
                                    handle_InCloseWrite(event.getArgument2());

                                } else {
                                    std::clog << "sono in rename e lancio la handle in rename\n";
                                    handle_InRename(event.getArgument1(), event.getArgument2());
                                }
                                break;
                            case EVENT_TYPE::PRUNING: {
                                std::clog << "Faccio pruning\n";
                                std::unique_lock lock_clean{real_clean_mutex};
                                to_clean = true;
                            }

                                break;

                            default:
                                break;

                        }

                    } else {
                        if (to_clean) {
                            std::clog << "STO PRUNANDOOOOOOOOOOOO\n";
                            handle_prune();
                            to_clean = false;
                        }
                        cv.wait(lk, [&]() { return !events.empty() || finish; });

                    }


                }
            }
            catch(std::filesystem::__cxx11::filesystem_error &e){
                std::clog << e.what() << "\n";
            }
            catch(...){
                std::clog << "Errore sconosciuto\n";
            }
            });
        }

}


void HandleWatcher::push_event(const Event &event) {
    std::unique_lock lk{m};
    events.push(event);
    cv.notify_one();
}

void HandleWatcher::handle_InCloseWrite(const std::string &path) {
    std::clog << " Evento: InCloseWrite , path : " << path << "\n";

    if (std::filesystem::exists(path)) {
        SyncStructure *sync = SyncStructure::getInstance();
        sync->add_entry(path);
        sync->write_structure();
    }
}

void HandleWatcher::handle_expand(const std::string &path) {
    std::clog << " Evento: expand , path : " << path << "\n";
    SyncStructure *sync = SyncStructure::getInstance();
    for (auto &p : std::filesystem::recursive_directory_iterator(path)) {
        if (std::filesystem::is_regular_file(p.path().string())) {
            sync->add_entry(p.path().string());
        }
    }
    sync->write_structure();
}

void HandleWatcher::handle_InDelete(const std::string &path) {
    std::clog << " Evento: InDelete, path :" << path << "\n";
    SyncStructure *sync = SyncStructure::getInstance();
    sync->remove_entry(path);
    sync->write_structure();
    // todo: send delete to server
}

void HandleWatcher::handle_prune() {
    std::clog << " Evento: Pruning\n";
    SyncStructure *sync = SyncStructure::getInstance();
    sync->prune();
    sync->write_structure();
    // todo: send bulk_delete to server
}

void HandleWatcher::handle_InModify(const std::string &path) {
    std::clog << " Evento: InModify, path :" << path << "\n";
    SyncStructure *sync = SyncStructure::getInstance();
    sync->add_entry(path);
    sync->write_structure();
}

void HandleWatcher::handle_InRename(const std::string &old_path, const std::string &new_path) {
    std::clog << " Evento: InRename, old_path :" << old_path << ", new_path: " << new_path << "\n";
    SyncStructure *sync = SyncStructure::getInstance();
    sync->rename_entry(old_path, new_path);
    sync->write_structure();
}

void HandleWatcher::handle_InMove(const std::string &path) {
    std::clog << " Evento: InMove, old_path =" << path << "\n";
}
