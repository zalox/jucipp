#include "python_api.h"
#include "python_interpreter.h"
#include "notebook.h"
#include "config.h"
#include "juci.h"
#include "menu.h"
#include "directories.h"
#include "window.h"

extern "C" PYBIND_EXPORT PyObject *init_juci_api() {
  
  pybind11::module api("libjuci", "Python API for juCi++");
  
  api.def("get_juci_home",
    [] () {
      return Config::get().juci_home_path().string();
    },
    "(str) Returns the path of the juci home folder"
  );
  api.def("add_menu_element",
    [] (const char *json) {
      boost::property_tree::ptree ptree;
      std::stringstream ss;
      ss << json;
      try {
        boost::property_tree::read_json(ss, ptree);  
      } catch (std::exception &ex) {
        
      }
      boost::property_tree::ptree empty;
      auto & sections = ptree.get_child("sections", empty);
      if(sections.empty()) {
        std::cerr << "Error parsing json from plugin menu definition, please verify" << std::endl;
        return;
      }
      for(auto &elem : sections) { //TODO Do this recursive
        if(elem.first == "item"){
          auto label = elem.second.get<std::string>("label", "");
          auto accel = elem.second.get<std::string>("keybinding", "");
          auto action = elem.second.get<std::string>("action", "");
          if(!action.empty() || !accel.empty())
            Config::get().menu.keys[action] = accel;
          Menu::get().add_action(action, [action]() {
            auto res = PythonInterpreter::get().exec(action);
            res.dec_ref();
          });
        }
        if(elem.first == "submenu"){
          auto & inner_sections = elem.second.get_child("sections", empty);
          if(inner_sections.empty()) {
            std::cerr << "Error parsing json from plugin menu definition, please verify" << std::endl;
            return;
          }
          for(auto &item : inner_sections){
            if(item.first == "item"){
              auto label = item.second.get<std::string>("label", "");
              auto accel = item.second.get<std::string>("keybinding", "");
              auto action = item.second.get<std::string>("action", "");
              if(!accel.empty() || !action.empty())
                Config::get().menu.keys[action] = accel;
                Menu::get().add_action(action, [action]() {
                  auto res = PythonInterpreter::get().exec(action);
                  res.dec_ref();
                });
            }
          }
        }
      }
      Menu::get().plugin_entries.emplace_back(ptree);
    },
    "(void) Builds a menu configured by 'json'",
    pybind11::arg("(str) json")
  );
  
  api.def_submodule("directories")
    .def("open",
      [] (const char *dir) { // ex. libjuci.directories.open("/home/")
        boost::filesystem::path path(dir);
        if (boost::filesystem::is_directory(path))
          Directories::get().open(path);
      },
      "(void) Opens 'dir' in file tree",
      pybind11::arg("(str) dir")
    )
    .def("update",
      [] () {
        Directories::get().update(); 
      },
      "(void) Updates the file tree"
    );
  
  api.def_submodule("editor")
    .def("get_file",
      [] () {
        auto& window = Window::get();
        auto view = window.notebook.get_current_view();
        if (view != nullptr) {
          return view->file_path.c_str();
        }
        return "";
      },
      "(str) Returns the current open file. If no file is open it returns empty a string")
    .def("get_text",
      [] () {
        auto& window = Window::get();
        auto view = window.notebook.get_current_view();
        if (view != nullptr) {
          return view->get_buffer()->get_text().c_str();
        }
        return "";
      },
      "returns the currently open buffers content")
    .def("get_line_number",
      [] () {
        auto& window = Window::get();
        auto view = window.notebook.get_current_view();
        if (view != nullptr) {
          return view->get_source_buffer()->get_insert()->get_iter().get_line();
        }
        return -1;
      },
      "(int) Returns the line which is being edited, -1 if no file is open or no line is selected")
    .def("get_line_offset",
      [] () {
        auto& window = Window::get();
        auto view = window.notebook.get_current_view();
        if (view != nullptr) {
          return view->get_source_buffer()->get_insert()->get_iter().get_line_offset();
        }
        return -1;       
      },
      "(int) Returns the number of characters the caret currently is positioned from the previous"
      " newline, 0 is the first character -1 if no file is open or no line is selected")
    .def("get_offset",
      [] () {
        auto& window = Window::get();
        auto view = window.notebook.get_current_view();
        if (view != nullptr) {
          return view->get_source_buffer()->get_insert()->get_iter().get_offset();
        }
        return -1;
      },
      "(int) Returns the number of characters the caret currently is positioned from start of the"
      " file. 0 is the first character. Returns -1 if no lines or files are selected/open")
    .def("get_text_range",
      [] (int begin_offset, int end_offset) {
        auto& window = Window::get();
        auto view = window.notebook.get_current_view();
        if (view != nullptr) {
          auto begin_iter = view->get_buffer()->get_iter_at_offset(begin_offset);
          auto end_iter = view->get_buffer()->get_iter_at_offset(end_offset);
          return view->get_source_buffer()->get_text(begin_iter, end_iter).c_str();
        }
        return "";
      },
      "(str) Returns the text between 'begin_offset' and 'end_offset'. Returns empty"
      "string if editor isn't focused on a file")
    .def("scroll_to",
      [] (int char_offset) {
        auto& window = Window::get();
        auto view = window.notebook.get_current_view();
        if (view != nullptr) {
          auto scroll_iter = view->get_buffer()->get_iter_at_offset(char_offset);
          view->scroll_to(scroll_iter);
        }
      },
      "(void) Scrolls cursor to 'char_offset'")
    .def("insert_at",
      [](int char_offset, const char* text){
        auto& window = Window::get();
        auto view = window.notebook.get_current_view();
        if (view != nullptr) {
          auto insert = view->get_buffer()->get_iter_at_offset(char_offset);
          view->get_buffer()->insert(insert, text);
        }
      },
      "(void) Inserts text at the given offset, caller is responsible to scroll to the insertion")
    .def("insert-at-cursor",
    [](int char_offset, const char* text){
      auto& window = Window::get();
      auto view = window.notebook.get_current_view();
      if (view != nullptr) {
        view->get_buffer()->insert_at_cursor(text);
      }
    },
    "(void) Inserts text at the cursor")
    .def("get_tab_char_and_size",
      [](){
        auto& window = Window::get();
        auto view = window.notebook.get_current_view();
        if (view != nullptr) {
          std::stringstream ss;
          boost::property_tree::ptree ptree;
          auto pair = view->get_tab_char_and_size();
          ptree.add("tab_char", pair.first);
          ptree.add("size", pair.second);
          boost::property_tree::write_json(ss, ptree);
          return ss.str();
        }
        return std::string();
      },
      "(str) Returns a key-value json-object with 'tab_char' and 'size' keys")
    .def("is_saved",
      [] () {
        //TODO finish this     
      },
      "(bool) Returns true if the current open file is saved")
    .def("get_highlighted_word",
      [] () {
        auto& window = Window::get();
        auto view = window.notebook.get_current_view();
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
              Terminal::get().print(res);
              return res;
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
    )
  ;
  api.def_submodule("terminal")
    .def("println",
      [] (const char *message) {
        return Terminal::get().print(message);
      },
      "Returns int, Prints 'message' to terminal", //TODO what does the return represent?
      pybind11::arg("str message, add \n for line break")
    )
  ;
  return api.ptr();
}
