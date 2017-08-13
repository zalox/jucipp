#ifndef JUCI_WINDOW_H_
#define JUCI_WINDOW_H_

#include <gtkmm.h>
#include <atomic>

#include <pythoncpp.h>

class Window : public Gtk::ApplicationWindow {
  Window();
public:
  static Window &get() {
    static Window singleton;
    return singleton;
  }
  py::Interpreter python;
protected:
  bool on_key_press_event(GdkEventKey *event) override;
  bool on_delete_event(GdkEventAny *event) override;

private:
  Gtk::AboutDialog about;
  
  Glib::RefPtr<Gtk::CssProvider> css_provider;

  void configure();
  void set_menu_actions();
  void activate_menu_items();
  void search_and_replace_entry();
  void set_tab_entry();
  void goto_line_entry();
  void rename_token_entry();
  std::string last_search;
  std::string last_replace;
  std::string last_run_command;
  std::string last_run_debug_command;
  bool case_sensitive_search=true;
  bool regex_search=false;
  bool search_entry_shown=false;
};

#endif  // JUCI_WINDOW_H
