#include "python_interpreter.h"
#include <pybind11/pybind11.h>
#include "singletons.h"
#include "juci.h"

PYBIND_PLUGIN(libjuci) {
  pybind11::module m("libjuci", "Python API for juCi++");

  m.def("directories_open",
    [] (const char *dir) {
      boost::filesystem::path path(dir);
      if (boost::filesystem::is_directory(path))
        Singleton::directories->open(path);
    },
    "Returns void. Opens 'dir' in file tree",
    pybind11::arg("str dir")
  );

  m.def("get_highlighted_word",
    [] () {
      auto g_application = g_application_get_default();
      auto gio_application = Glib::wrap(g_application, true);
      auto application = Glib::RefPtr<Application>::cast_static(gio_application);
      if (application) {
        auto view = application->window->notebook.get_current_view();
        if (view != nullptr) {
          if (view->get_token) {
            auto token = view->get_token();
            if (token.type != -1) {
              boost::property_tree::ptree ptree;
              ptree.add("word", token.spelling);
              ptree.add("type", token.type);
              std::stringstream ss;
              boost::property_tree::json_parser::write_json(ss, ptree);
              std::string res(ss.str());
              Singleton::terminal->print(res);
              return res;
            }
          }
        }
      }
      return std::string(""); // const char * not viable
    },
    "returns json, empty string on error"
    "{"
      "'word': 'word',"
      "'type': '-1'"
    "}"
  );

  m.def("terminal_print",
    [] (const char *message) {
      return Singleton::terminal->print(message);
    },
    "Returns int, Prints 'message' to terminal", //TODO what does the return represent?
    pybind11::arg("str message, add \n for line break")
  );

  m.def("add_menu_element",
    [] (const char *json) {
      Singleton::menu->plugin_entries.emplace_back(json);
    },
    "Returns void. Adds menu configured by 'json'",
    pybind11::arg("str json")
  );
  
  m.def("get_project_folder",
    [] () {
      return Singleton::directories->cmake->project_path.string();
    },
    "Returns path of current open directory."
  );
  
  m.def("get_juci_home",
    [] () {
      return Singleton::config->juci_home_path().string();
    },
    "Returns the path of the juci home folder."
  );
 
  return m.ptr();
}