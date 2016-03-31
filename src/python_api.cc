#include "python_api.h"
#include "python_interpreter.h"
#include "notebook.h"
#include "config.h"
#include "juci.h"
#include "menu.h"
#include "directories.h"
#include "window.h"
#include <pygobject.h>
#include <pybind11/stl.h>

template <class T> pybind11::object pyobject_from_gobj(T ptr){
  if(G_IS_OBJECT(ptr)){
    auto obj=G_OBJECT(ptr);
    if(obj)
      return pybind11::object(pygobject_new(obj), true);
  }
  return pybind11::object(Py_None, false);
}

template<class T> pybind11::object pyobject_from_refptr_type(const T &ptr){
  if(ptr)
    return pyobject_from_gobj(ptr->gobj());
  pybind11::object(Py_None, false);
}

extern "C" PYBIND_EXPORT PyObject *init_juci_api() {
  
  pybind11::module api("libjuci", "Python API for juCi++");
  
  pygobject_init(-1,-1,-1);
  
  api.def_submodule("beta")
    .def("get_notebook",
      [](){
        auto gtk_notebook=reinterpret_cast<GtkNotebook*>(Notebook::get().gobj());
        return pyobject_from_gobj(gtk_notebook);
      }
    );  
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
        Menu::get().plugin_entries.emplace_back(ptree);
      } catch (std::exception &ex) {
        std::cerr << "There was an error while parsing json:\n" << json;
        std::cerr << std::endl << ex.what() << std::endl;
      }
    },
    "(void) Builds a menu configured by 'json'",
    pybind11::arg("(str) json")
  );
  
  api.def_submodule("directories")
    .def("open",
      [] (std::string dir) { // ex. libjuci.directories.open("/home/")
        boost::filesystem::path path(dir);
        if(boost::filesystem::is_directory(path))
          Directories::get().open(path);
        if(boost::filesystem::is_regular_file(path))
          Notebook::get().open(path);
      },
      "(void) Opens 'dir' in file tree",
      pybind11::arg("(str) dir")
    )
    .def("update",
      [](){
        Directories::get().update(); 
      },
      "(void) Updates the file tree"
    )
    .def("get_directories",
      [](){
        auto view=reinterpret_cast<GtkTreeView*>(Directories::get().gobj());
        return pyobject_from_gobj(view);
      }, pybind11::return_value_policy::reference_internal
    );
  
  api.def_submodule("editor")
    .def("get_file_name",
      [] () -> std::string {
        auto view = Notebook::get().get_current_view();
        if (view != nullptr) {
          return view->file_path.string();
        }
        return "";
      },
      "(str) Returns the current open file. If no file is open it returns empty a string"
    )
    .def("get_tab_char_and_size",
      [](){
        auto view = Notebook::get().get_current_view();
        if (view != nullptr)
          return view->get_tab_char_and_size();
        return std::make_pair<char,unsigned int>(0,0);
      }
    )
    .def("get_current_gtk_text_view",
      [](){
        auto view=Notebook::get().get_current_view();
        if(view){
          // Hack to force Gtk.TextView to be selected during introspection.
          auto gtk_buffer=reinterpret_cast<GtkTextBuffer*>(view->get_buffer()->gobj());
          auto gtk_view=gtk_text_view_new_with_buffer(gtk_buffer); // mem leak?
          return pyobject_from_gobj(gtk_view);
        }
        return pybind11::object(Py_None, false);
      },pybind11::return_value_policy::reference_internal
    )
  ;
  
  api.def_submodule("terminal")
    .def("println",
      [] (std::string message) {
        return Terminal::get().print(message + "\n"); //TODO alternative for print, python key word
      },
      "Returns int, Prints 'message' to terminal", //TODO what does the return represent?
      pybind11::arg("str message")
    )
    .def("get_textview",
      [](){
        return pyobject_from_gobj(Terminal::get().gobj());
      }, pybind11::return_value_policy::reference_internal
    )
  ;
  
//  api.def("",
//    [](const char *json){
//      boost::property_tree::ptree menu_elements;
//      std::stringstream ss;
//      ss << json;
//      try{
//        boost::property_tree::read_json(ss, menu_elements);
//      }catch(std::exception &ex){
//        std::cerr << "Error while parsing json" << std::endl;
//        return;
//      }
//      for(auto &element:menu_elements){
//
//      }
//    }
//  );
  
  return api.ptr();
}
