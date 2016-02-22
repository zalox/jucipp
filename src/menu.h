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
  boost::property_tree::ptree generate_submenu(const std::string &label);
  boost::property_tree::ptree generate_item(const std::string &label, const std::string &action, const std::string &accel);
  void add_action(const std::string &name, std::function<void()> action);
  void add_sections(boost::property_tree::ptree &ptree, boost::property_tree::ptree &menu, const std::unordered_map<std::string, std::string> &accels);
  void set_keys();
  std::vector<boost::property_tree::ptree> plugin_entries;
  std::unordered_map<std::string, Glib::RefPtr<Gio::SimpleAction> > actions;
  void build();
  Glib::RefPtr<Gio::Menu> juci_menu;
  Glib::RefPtr<Gio::Menu> window_menu;
  
private:
  Glib::RefPtr<Gtk::Builder> builder;
  std::string ui_xml;
};

#endif  // JUCI_MENU_H_
