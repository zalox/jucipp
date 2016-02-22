#ifndef JUCI_DIRECTORIES_H_
#define JUCI_DIRECTORIES_H_

#include <gtkmm.h>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include "cmake.h"
#include <thread>
#include <mutex>
#include <atomic>
#include "dispatcher.h"

class Directories : public Gtk::TreeView {
public:
  class ColumnRecord : public Gtk::TreeModel::ColumnRecord {
  public:
    ColumnRecord() {
      add(id);
      add(name);
      add(path);
      add(color);
    }
    Gtk::TreeModelColumn<std::string> id;
    Gtk::TreeModelColumn<std::string> name;
    Gtk::TreeModelColumn<boost::filesystem::path> path;
    Gtk::TreeModelColumn<Gdk::RGBA> color;
  };

private:
  Directories();
public:
  static Directories &get() {
    static Directories singleton;
    return singleton;
  }
  ~Directories();
  void open(const boost::filesystem::path& dir_path="");
  void update();
  void select(const boost::filesystem::path &path);
  
  std::function<void(const boost::filesystem::path &path)> on_row_activated;
  std::unique_ptr<CMake> cmake;
  boost::filesystem::path current_path;
  
private:
  void add_path(const boost::filesystem::path& dir_path, const Gtk::TreeModel::Row &row);
  Glib::RefPtr<Gtk::TreeStore> tree_store;
  ColumnRecord column_record;
  
  std::unordered_map<std::string, std::pair<Gtk::TreeModel::Row, std::time_t> > last_write_times;
  std::mutex update_mutex;
  std::thread update_thread;
  std::atomic<bool> stop_update_thread;
  Dispatcher dispatcher;
  std::vector<std::string> update_paths;
};

#endif  // JUCI_DIRECTORIES_H_
