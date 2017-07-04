#include "juci.h"
#include "window.h"
#include "notebook.h"
#include "directories.h"
#include "menu.h"
#include "config.h"
#include "python_interpreter.h"

int Application::on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine> &cmd) {
  Glib::set_prgname("juci");
  Glib::OptionContext ctx("[PATH ...]");
  Glib::OptionGroup gtk_group(gtk_get_option_group(true));
  ctx.add_group(gtk_group);
  int argc;
  char **argv = cmd->get_arguments(argc);
  ctx.parse(argc, argv);
  if(argc>=2) {
    boost::system::error_code current_path_ec;
    auto current_path=boost::filesystem::current_path(current_path_ec);
    if(current_path_ec)
      errors.emplace_back("Error: could not find current path\n");
    for(int c=1;c<argc;c++) {
      boost::filesystem::path path(argv[c]);
      if(path.is_relative() && !current_path_ec)
        path=current_path/path;
      if(boost::filesystem::exists(path)) {
        if(boost::filesystem::is_regular_file(path))
          files.emplace_back(path, 0);
        else if(boost::filesystem::is_directory(path))
          directories.emplace_back(path);
      }
      //Open new file if parent path exists
      else {
        if(path.is_absolute() && boost::filesystem::is_directory(path.parent_path()))
          files.emplace_back(path, 0);
        else
          errors.emplace_back("Error: could not create "+path.string()+".\n");
      }
    }
  }
  activate();
  return 0;
}

void Application::on_activate() {
  add_window(Window::get());
  Window::get().show();
  
  std::string last_current_file;
  
  if(directories.empty() && files.empty()) {
    try {
      boost::property_tree::ptree pt;
      boost::property_tree::read_json((Config::get().home_juci_path/"last_session.json").string(), pt);
      auto folder=pt.get<std::string>("folder");
      if(!folder.empty() && boost::filesystem::exists(folder) && boost::filesystem::is_directory(folder))
        directories.emplace_back(folder);
      for(auto &pt_file: pt.get_child("files")) {
        auto notebook=pt_file.second.get<size_t>("notebook", -1);
        auto file=pt_file.second.get<std::string>("file", "");
        if(!file.empty() && boost::filesystem::exists(file) && !boost::filesystem::is_directory(file))
          files.emplace_back(file, notebook);
      }
      last_current_file=pt.get<std::string>("current_file");
      if(!boost::filesystem::exists(last_current_file) || boost::filesystem::is_directory(last_current_file))
        last_current_file.clear();
    }
    catch(const std::exception &) {}
  }
  
  bool first_directory=true;
  for(auto &directory: directories) {
    if(first_directory) {
      Directories::get().open(directory);
      first_directory=false;
    }
    else {
      std::string files_in_directory;
      for(auto it=files.begin();it!=files.end();) {
        if(it->first.generic_string().compare(0, directory.generic_string().size()+1, directory.generic_string()+'/')==0) {
          files_in_directory+=" "+it->first.string();
          it=files.erase(it);
        }
        else
          it++;
      }
      std::thread another_juci_app([this, directory, files_in_directory](){
        Terminal::get().async_print("Executing: juci "+directory.string()+files_in_directory+"\n");
        Terminal::get().process("juci "+directory.string()+files_in_directory, "", false);
      });
      another_juci_app.detach();
    }
  }
  
  for(auto &file: files)
    Notebook::get().open(file.first, file.second);
  
  for(auto &error: errors)
    Terminal::get().print(error, true);
  
  if(!last_current_file.empty())
    Notebook::get().open(last_current_file);
  if(Config::get().python.enabled)
    Python::Interpreter::get();
}

void Application::on_startup() {
  Gtk::Application::on_startup();
  
  Menu::get().build();

  if (!Menu::get().juci_menu || !Menu::get().window_menu) {
    std::cerr << "Menu not found." << std::endl;
  }
  else {
    set_app_menu(Menu::get().juci_menu);
    set_menubar(Menu::get().window_menu);
  }
}

Application::Application() : Gtk::Application("no.sout.juci", Gio::APPLICATION_NON_UNIQUE | Gio::APPLICATION_HANDLES_COMMAND_LINE) {
  Glib::set_application_name("juCi++");
  
  //Gtk::MessageDialog without buttons caused text to be selected, this prevents that
  Gtk::Settings::get_default()->property_gtk_label_select_on_focus()=false;
}

int main(int argc, char *argv[]) {
  return Application().run(argc, argv);
}
