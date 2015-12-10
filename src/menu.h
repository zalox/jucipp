#ifndef JUCI_MENU_H_
#define JUCI_MENU_H_

#include <string>
#include <unordered_map>
#include <gtkmm.h>
#include <boost/property_tree/ptree.hpp>

class Menu {
private:
  Menu();
public:
  static Menu &get() {
    static Menu singleton;
    return singleton;
  }
  
  void add_action(const std::string &name, std::function<void()> action);
  std::unordered_map<std::string, Glib::RefPtr<Gio::SimpleAction> > actions;
  void set_keys();
  
  void build();
  Glib::RefPtr<Gtk::Builder> builder;
  
  std::string ui_xml;
  
  class Generator {
      public:
        boost::property_tree::ptree generate_submenu(const std::string &label);
        boost::property_tree::ptree generate_item(const std::string &label, const std::string &action, const std::string &accel);
  };
  
  std::vector<boost::property_tree::ptree> plugin_entries;
};




#endif  // JUCI_MENU_H_
