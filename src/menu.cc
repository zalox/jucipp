#include "menu.h"
#include "window.h"
#include "config.h"
#include <string>
#include <boost/property_tree/xml_parser.hpp>
#include "terminal.h"
#include "python_interpreter.h"

boost::property_tree::ptree Menu::generate_submenu(const std::string &label){
  boost::property_tree::ptree ptree;
  std::stringstream ss;
  std::string element(
      "<attribute name='label' translatable='yes'>"+label+"</attribute>"
  );
  ss << element;
  boost::property_tree::read_xml(ss, ptree);
  return ptree;
}

boost::property_tree::ptree Menu::generate_item(const std::string &label, const std::string &action, const std::string &accel){
  boost::property_tree::ptree ptree;
  std::stringstream ss;
  std::string item(
    "<attribute name='label' translatable='yes'>"+label+"</attribute>"
    "<attribute name='action'>app."+action+"</attribute>"
     +accel //For Ubuntu...
  );
  ss << item;
  boost::property_tree::read_xml(ss, ptree);
  return ptree;
}

Menu::Menu() {}

void Menu::add_action(const std::string &name, std::function<void()> action) {
 auto g_application=g_application_get_default();
 auto gio_application=Glib::wrap(g_application, true);
 auto application=Glib::RefPtr<Gtk::Application>::cast_static(gio_application);
 actions[name]=application->add_action(name, action);
}

void Menu::set_keys() {
  auto g_application=g_application_get_default();
  auto gio_application=Glib::wrap(g_application, true);
  auto application=Glib::RefPtr<Gtk::Application>::cast_static(gio_application);
  for(auto &key: Config::get().menu.keys) {
    if(key.second.size()>0 && actions.find(key.first)!=actions.end()) {
#if GTK_VERSION_GE(3, 12)
      application->set_accel_for_action("app."+key.first, key.second);
#else
      application->add_accelerator(key.second, "app."+key.first); //For Ubuntu 14...
#endif
    }
  }
}

void Menu::build(){
  auto accels=Config::get().menu.keys;
  for(auto &accel: accels) {
#ifdef JUCI_UBUNTU_BUGGED_MENU
    size_t pos=0;
    std::string second=accel.second;
    while((pos=second.find('<', pos))!=std::string::npos) {
      second.replace(pos, 1, "&lt;");
      pos+=4;
    }
    pos=0;
    while((pos=second.find('>', pos))!=std::string::npos) {
      second.replace(pos, 1, "&gt;");
      pos+=4;
    }
    if(second.size()>0)
      accel.second="<attribute name='accel'>"+second+"</attribute>";
    else
      accel.second="";
#else
   accel.second="";
#endif
    }
    std::string ui_xml =
      "<?xml version='1.0' encoding='utf-8'?>"
      "<interface>"
      "  <menu id='juci-menu'>"
      "    <section>"
      "      <item>"
      "        <attribute name='label' translatable='yes'>_About</attribute>"
      "        <attribute name='action'>app.about</attribute>"
               +accels["about"]+ //For Ubuntu...
      "      </item>"
      "    </section>"
      "    <section>"
      "      <item>"
      "        <attribute name='label' translatable='yes'>_Preferences</attribute>"
      "        <attribute name='action'>app.preferences</attribute>"
               +accels["preferences"]+ //For Ubuntu...
      "      </item>"
      "    </section>"
      "    <section>"
      "      <item>"
      "        <attribute name='label' translatable='yes'>_Quit</attribute>"
      "        <attribute name='action'>app.quit</attribute>"
               +accels["quit"]+ //For Ubuntu...
      "      </item>"
      "    </section>"
      "  </menu>"
      ""
      "  <menu id='window-menu'>"
      "    <submenu id='file'>"
      "      <attribute name='label' translatable='yes'>_File</attribute>"
      "      <section>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_New _File</attribute>"
      "          <attribute name='action'>app.new_file</attribute>"
                 +accels["new_file"]+ //For Ubuntu...
      "        </item>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_New _Folder</attribute>"
      "          <attribute name='action'>app.new_folder</attribute>"
                 +accels["new_folder"]+ //For Ubuntu...
      "        </item>"
      "        <submenu>"
      "          <attribute name='label' translatable='yes'>_New _Project</attribute>"
      "          <item>"
      "            <attribute name='label' translatable='no'>C++</attribute>"
      "            <attribute name='action'>app.new_project_cpp</attribute>"
                   +accels["new_project_cpp"]+ //For Ubuntu...
      "          </item>"
      "        </submenu>"
      "      </section>"
      "      <section>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Open _File</attribute>"
      "          <attribute name='action'>app.open_file</attribute>"
                 +accels["open_file"]+ //For Ubuntu...
      "        </item>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Open _Folder</attribute>"
      "          <attribute name='action'>app.open_folder</attribute>"
                 +accels["open_folder"]+ //For Ubuntu...
      "        </item>"
      "      </section>"
      "      <section>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Save</attribute>"
      "          <attribute name='action'>app.save</attribute>"
                 +accels["save"]+ //For Ubuntu...
      "        </item>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Save _As</attribute>"
      "          <attribute name='action'>app.save_as</attribute>"
                 +accels["save_as"]+ //For Ubuntu...
      "        </item>"
      "      </section>"
      "    </submenu>"
      ""
      "    <submenu id='edit'>"
      "      <attribute name='label' translatable='yes'>_Edit</attribute>"
      "      <section>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Undo</attribute>"
      "          <attribute name='action'>app.edit_undo</attribute>"
                 +accels["edit_undo"]+ //For Ubuntu...
      "        </item>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Redo</attribute>"
      "          <attribute name='action'>app.edit_redo</attribute>"
                 +accels["edit_redo"]+ //For Ubuntu...
      "        </item>"
      "      </section>"
      "      <section>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Cut</attribute>"
      "          <attribute name='action'>app.edit_cut</attribute>"
                 +accels["edit_cut"]+ //For Ubuntu...
      "        </item>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Copy</attribute>"
      "          <attribute name='action'>app.edit_copy</attribute>"
                 +accels["edit_copy"]+ //For Ubuntu...
      "        </item>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Paste</attribute>"
      "          <attribute name='action'>app.edit_paste</attribute>"
                 +accels["edit_paste"]+ //For Ubuntu...
      "        </item>"
      "      </section>"
      "      <section>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Find</attribute>"
      "          <attribute name='action'>app.edit_find</attribute>"
                 +accels["edit_find"]+ //For Ubuntu...
      "        </item>"
      "      </section>"
      "    </submenu>"
      ""
      "    <submenu id='source'>"
      "      <attribute name='label' translatable='yes'>_Source</attribute>"
      "      <section>"
      "        <submenu id='spellcheck'>"
      "          <attribute name='label' translatable='yes'>_Spell _Check</attribute>"
      "          <item>"
      "            <attribute name='label' translatable='yes'>_Spell _Check _Buffer</attribute>"
      "            <attribute name='action'>app.source_spellcheck</attribute>"
                   +accels["source_spellcheck"]+ //For Ubuntu...
      "          </item>"
      "          <item>"
      "            <attribute name='label' translatable='yes'>_Clear _Spelling _Errors</attribute>"
      "            <attribute name='action'>app.source_spellcheck_clear</attribute>"
                   +accels["source_spellcheck_clear"]+ //For Ubuntu...
      "          </item>"
      "          <item>"
      "            <attribute name='label' translatable='yes'>_Go _to _Next _Spelling _Error</attribute>"
      "            <attribute name='action'>app.source_spellcheck_next_error</attribute>"
                   +accels["source_spellcheck_next_error"]+ //For Ubuntu...
      "          </item>"
      "        </submenu>"
      "      </section>"
      "      <section>"
      "        <submenu id='indentation'>"
      "          <attribute name='label' translatable='yes'>_Indentation</attribute>"
      "          <item>"
      "            <attribute name='label' translatable='yes'>_Set _Current _Buffer _Tab</attribute>"
      "            <attribute name='action'>app.source_indentation_set_buffer_tab</attribute>"
                   +accels["source_indentation_set_buffer_tab"]+ //For Ubuntu...
      "          </item>"
      "          <item>"
      "            <attribute name='label' translatable='yes'>_Auto-Indent _Current _Buffer</attribute>"
      "            <attribute name='action'>app.source_indentation_auto_indent_buffer</attribute>"
                   +accels["source_indentation_auto_indent_buffer"]+ //For Ubuntu...
      "          </item>"
      "        </submenu>"
      "      </section>"
      "      <section>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Go _to _Line</attribute>"
      "          <attribute name='action'>app.source_goto_line</attribute>"
                 +accels["source_goto_line"]+ //For Ubuntu...
      "        </item>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Center _Cursor</attribute>"
      "          <attribute name='action'>app.source_center_cursor</attribute>"
                 +accels["source_center_cursor"]+ //For Ubuntu...
      "        </item>"
      "      </section>"
      "      <section>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Find _Documentation</attribute>"
      "          <attribute name='action'>app.source_find_documentation</attribute>"
                 +accels["source_find_documentation"]+ //For Ubuntu...
      "        </item>"
      "      </section>"
      "      <section>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Go to Declaration</attribute>"
      "          <attribute name='action'>app.source_goto_declaration</attribute>"
                 +accels["source_goto_declaration"]+ //For Ubuntu...
      "        </item>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Go to Usage</attribute>"
      "          <attribute name='action'>app.source_goto_usage</attribute>"
                 +accels["source_goto_usage"]+ //For Ubuntu...
      "        </item>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Go to Method</attribute>"
      "          <attribute name='action'>app.source_goto_method</attribute>"
                 +accels["source_goto_method"]+ //For Ubuntu...
      "        </item>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Rename</attribute>"
      "          <attribute name='action'>app.source_rename</attribute>"
                 +accels["source_rename"]+ //For Ubuntu...
      "        </item>"
      "      </section>"
      "      <section>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Go to Next Diagnostic</attribute>"
      "          <attribute name='action'>app.source_goto_next_diagnostic</attribute>"
                 +accels["source_goto_next_diagnostic"]+ //For Ubuntu...
      "        </item>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Apply Fix-Its</attribute>"
      "          <attribute name='action'>app.source_apply_fix_its</attribute>"
                 +accels["source_apply_fix_its"]+ //For Ubuntu...
      "        </item>"
      "      </section>"
      "    </submenu>"
      ""
      "    <submenu id='project'>"
      "      <attribute name='label' translatable='yes'>_Project</attribute>"
      "      <section>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Compile _and _Run</attribute>"
      "          <attribute name='action'>app.compile_and_run</attribute>"
                 +accels["compile_and_run"]+ //For Ubuntu...
      "        </item>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Compile</attribute>"
      "          <attribute name='action'>app.compile</attribute>"
                 +accels["compile"]+ //For Ubuntu...
      "        </item>"
      "      </section>"
      "      <section>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Run _Command</attribute>"
      "          <attribute name='action'>app.run_command</attribute>"
                 +accels["run_command"]+ //For Ubuntu...
      "        </item>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Kill _Last _Process</attribute>"
      "          <attribute name='action'>app.kill_last_running</attribute>"
                 +accels["kill_last_running"]+ //For Ubuntu...
      "        </item>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Force _Kill _Last _Process</attribute>"
      "          <attribute name='action'>app.force_kill_last_running</attribute>"
                 +accels["force_kill_last_running"]+ //For Ubuntu...
      "        </item>"
      "      </section>"
      "    </submenu>"
      ""
      "    <submenu id='window'>"
      "      <attribute name='label' translatable='yes'>_Window</attribute>"
      "      <section>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Next _Tab</attribute>"
      "          <attribute name='action'>app.next_tab</attribute>"
                 +accels["next_tab"]+ //For Ubuntu...
      "        </item>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Previous _Tab</attribute>"
      "          <attribute name='action'>app.previous_tab</attribute>"
                 +accels["previous_tab"]+ //For Ubuntu...
      "        </item>"
      "      </section>"
      "      <section>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Close _Tab</attribute>"
      "          <attribute name='action'>app.close_tab</attribute>"
                 +accels["close_tab"]+ //For Ubuntu...
      "        </item>"
      "      </section>"
      "    </submenu>"
      "  </menu>"
      "</interface>";
    std::stringstream ss;
    ss << ui_xml;
    boost::property_tree::ptree ptree;
  try {
    boost::property_tree::read_xml(ss, ptree, boost::property_tree::xml_parser::trim_whitespace);
    auto &xml_interface = ptree.get_child("interface");
    auto menu_range = xml_interface.equal_range("menu");
    boost::property_tree::ptree * menu = nullptr;
    for(auto &it = menu_range.first; it!=menu_range.second; it++) {
      if((it->second.get_child("<xmlattr>.id")).get_value<std::string>() == "window-menu"){
       menu = &it->second;
      }
    }
    if(menu != nullptr) {
      PythonInterpreter::get();
      for (auto &plugin : plugin_entries) {
        add_sections(*menu, plugin, accels);
      }
    }
    ss = std::stringstream();
    boost::property_tree::write_xml(ss, ptree, boost::property_tree::xml_parser::trim_whitespace);
    ui_xml = ss.str();
    menu = nullptr;
    builder = Gtk::Builder::create_from_string(ui_xml);
    auto object = builder->get_object("juci-menu");
    menu_refrences["juci_menu"] = Glib::RefPtr<Gio::Menu>::cast_dynamic(object);
    object = builder->get_object("window-menu");
    menu_refrences["window_menu"] = Glib::RefPtr<Gio::Menu>::cast_dynamic(object);
    menu_refrences["plugin_menu"] = Gio::Menu::create();
    menu_refrences["window_menu"]->insert_submenu(5, "_Plugins", menu_refrences["plugin_menu"]);
  } catch (const std::exception &ex) {
    std::cerr << "building menu failed: " << ex.what();
  }
}

void Menu::add_sections(boost::property_tree::ptree &xml_menus, boost::property_tree::ptree &menu_elements, const std::unordered_map<std::string, std::string> &accels){
  boost::property_tree::ptree empty;
  for(auto &element:menu_elements){
    auto label=element.second.get<std::string>("label", "");
    auto &sub_menu_elements=element.second.get_child("menu_elements", empty);
    if(sub_menu_elements.empty()){ // item
      auto action=element.second.get<std::string>("action", "");
      auto keybinding=element.second.get<std::string>("keybinding", "");
      if(!action.empty()&&!keybinding.empty()){
        Config::get().menu.keys[action] = keybinding;
      }
      if(!label.empty() && !action.empty()){
        add_action(action, [action](){
          auto res = PythonInterpreter::get().exec(action);
          res.dec_ref();
        });
        auto accel = accels.find(label);
        auto items=xml_menus.equal_range("item");
        bool item_exists=false;
        for(auto &it=items.first; it!=items.second; it++){
          auto item_label=(it->second.get_child("attribute")).get_value<std::string>();
          if(item_label==label){
            item_exists=true;
          }
        }
        if(!item_exists)
          xml_menus.add_child("item",generate_item(label,action,accel!=accels.end()?accel->second:""));
      }
      if(label.empty() && action.empty()){
        Terminal::get().print("Couldn't parse json, item has no action or label\n");
        continue;
      } 
    }else{
      for(auto &sub_menu_element:sub_menu_elements){
        if(!label.empty()){
          auto submenus=xml_menus.equal_range("submenu");
          boost::property_tree::ptree *xml_submenu=nullptr;
          for(auto &it=submenus.first;it!=submenus.second;it++){
            auto submenu_label=(it->second.get_child("attribute")).get_value<std::string>();
            if(label==submenu_label){
              xml_submenu=&it->second;
              break;
            }
          }
          if(xml_submenu==nullptr)
            xml_submenu=&xml_menus.add_child("submenu", generate_submenu(label));
          add_sections(*xml_submenu, sub_menu_elements, accels);
        }else{
          Terminal::get().print("Couldn't parse json, item has no action or label\n");  
          continue;
        }
      }
    }
  }
}
