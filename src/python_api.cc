#include "python_api.h"
#include "singletons.h"
#include "juci.h"

extern "C" PYBIND_EXPORT PyObject *init_juci_api() {
  pybind11::module api("libjuci", "Python API for juCi++");
  api.def("directories_open",
    [] (const char *dir) {
      boost::filesystem::path path(dir);
      if (boost::filesystem::is_directory(path))
        Singleton::directories->open(path);
    },
    "Returns void. Opens 'dir' in file tree",
    pybind11::arg("str dir")
  );

  api.def("get_highlighted_word",
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
  
  api.def("terminal_print",
    [] (const char *message) {
      return Singleton::terminal->print(message);
    },
    "Returns int, Prints 'message' to terminal", //TODO what does the return represent?
    pybind11::arg("str message, add \n for line break")
  );
  
  api.def("add_menu_element",
    [] (const char *json) {
      boost::property_tree::ptree ptree;
      std::stringstream ss;
      ss << json;
      boost::property_tree::read_json(ss, ptree);
      for(auto &elem : ptree.get_child("sections")) { //TODO Do this recursive
        if(elem.first == "item"){
          auto label = elem.second.get<std::string>("label", "");
          auto accel = elem.second.get<std::string>("keybinding", "");
          auto action = elem.second.get<std::string>("action", "");
          if(label.empty())
            continue;
          if(!accel.empty())
            Singleton::config->menu.keys[action] = accel;
          Singleton::menu->add_action(action, [action]() {
            auto res = Singleton::python_interpreter->exec(action);
            res.dec_ref();
          });
        }
        if(elem.first == "section"){
          for(auto &item : elem.second){
            if(item.first == "item"){
              auto label = item.second.get<std::string>("label", "");
              auto accel = item.second.get<std::string>("keybinding", "");
              auto action = item.second.get<std::string>("action", "");
              if(label.empty())
                continue;
              if(!accel.empty())
                Singleton::config->menu.keys[action] = accel;
              Singleton::menu->add_action(action, [action]() {
                auto res = Singleton::python_interpreter->exec(action);
                res.dec_ref();
              });
            }
          }
        }
      }
      Singleton::menu->plugin_entries.emplace_back(ptree);
    },
    "Returns void. Adds menu configured by 'json'",
    pybind11::arg("str json")
  );
  
  api.def("get_project_folder",
    [] () {
      return Singleton::directories->cmake->project_path.string();
    },
    "Returns path of current open directory."
  );
  
  api.def("get_juci_home",
    [] () {
      return Singleton::config->juci_home_path().string();
    },
    "Returns the path of the juci home folder."
  );
  
  return api.ptr();
}
