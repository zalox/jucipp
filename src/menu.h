#ifndef JUCI_MENU_H_
#define JUCI_MENU_H_

#include <string>
#include <unordered_map>
#include <gtkmm.h>
#include <boost/property_tree/ptree.hpp>

class Menu {
public:
  Menu();
  void init(); //For Ubuntu 14...
  
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
  
  class Entry {
  public:
      Entry(const std::string &json);
      boost::property_tree::ptree ptree;
  };
  
  std::vector<Entry> plugin_entries;
};




#endif  // JUCI_MENU_H_
