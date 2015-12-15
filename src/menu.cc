#include "menu.h"
#include "window.h"
#include "config.h"
#include <string>
#include <boost/property_tree/xml_parser.hpp>
#include "terminal.h"

boost::property_tree::ptree Menu::Generator::generate_submenu(const std::string &label){
  boost::property_tree::ptree ptree;
  std::stringstream ss;
  std::string element(
      "<attribute name='label' translatable='yes'>"+label+"</attribute>"
  );
  ss << element;
  boost::property_tree::read_xml(ss, ptree);
  return ptree;
}

boost::property_tree::ptree Menu::Generator::generate_item(const std::string &label, const std::string &action, const std::string &accel){
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

Glib::RefPtr<Gtk::Builder> Menu::build() {
  try {
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
    // add plugin menu items
    std::stringstream ss;
    ss << ui_xml;
    boost::property_tree::ptree ptree;
    boost::property_tree::read_xml(ss, ptree, boost::property_tree::xml_parser::trim_whitespace);
    auto &iface = ptree.get_child("interface");
    auto menu_range = iface.equal_range("menu");
    boost::property_tree::ptree * menu = nullptr;
    for(auto &it = menu_range.first; it!=menu_range.second; it++){
      if((it->second.get_child("<xmlattr>.id")).get_value<std::string>() == "window-menu"){
        menu = &it->second;
      }
    }
    Generator g;
    auto submenus = menu->equal_range("submenu");
    for (auto &json : plugin_entries) {
      auto main_menu = json.get<std::string>("main_menu", "");
      if(main_menu.empty()) {
        Terminal::get().print("Could't parse json, main_menu element missing");
        continue;
      }
      boost::property_tree::ptree *add_into = nullptr;
      for(auto &it = submenus.first; it!=submenus.second; ++it){
        auto label = (it->second.get_child("attribute")).get_value<std::string>();
        if(main_menu == label)
          add_into = &it->second;
      }
      if(add_into == nullptr)
        add_into = &menu->add_child("submenu", g.generate_submenu(main_menu));
      boost::property_tree::ptree empty;
      auto &sections = json.get_child("sections", empty);
      for(auto &elem : sections) {
        if(elem.first == "item") {
          auto label = elem.second.get<std::string>("label", "");
          auto action = elem.second.get<std::string>("action", "");
          if (!label.empty() && !action.empty()){
            add_into->add_child("item", g.generate_item(label, action, accels[label]));
          } else {
            Terminal::get().print("Could not parse json, item has no action or label");
          }
        } else if(elem.first == "submenu") {
          auto label = elem.second.get<std::string>("label", "");
          if(label.empty())
            continue;
          auto &submenu = add_into->add_child("submenu", g.generate_submenu(label));
          auto &inner_sections = elem.second.get_child("sections", empty);
          for(auto &subelem : inner_sections){
            if(subelem.first == "item") {
              auto label = subelem.second.get<std::string>("label", "");
              auto action = subelem.second.get<std::string>("action", "");
              if (!label.empty() && !action.empty()){
                submenu.add_child("item", g.generate_item(label, action, accels[label]));
              } else {
                Terminal::get().print("Could not parse json, submenu item has no action or label");
              }
            }
          }
        }
      }
    }
    ss = std::stringstream();
    boost::property_tree::write_xml(ss, ptree, boost::property_tree::xml_parser::trim_whitespace);
    ui_xml = ss.str();
    menu = nullptr;
    return Gtk::Builder::create_from_string(ui_xml);
  }
  catch (const Glib::Error &ex) {
    std::cerr << "building menu failed: " << ex.what();
    return Glib::RefPtr<Gtk::Builder>(nullptr);
  }
}
