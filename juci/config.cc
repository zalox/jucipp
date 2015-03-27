#include "config.h"


MainConfig::MainConfig() :
keybindings_cfg_(), source_cfg_() {
  boost::property_tree::json_parser::read_json("config.json", cfg_);
  GenerateSource();
  GenerateKeybindings();
    //    keybindings_cfg_ = cfg_.get_child("keybindings");
    //    notebook_cfg_ = cfg_.get_child("notebook");
    //    menu_cfg_ = cfg_.get_child("menu");
}

void MainConfig::GenerateSource() {
  boost::property_tree::ptree source_json = cfg_.get_child("source");
  boost::property_tree::ptree syntax_json = source_json.get_child("syntax");
  boost::property_tree::ptree colors_json = source_json.get_child("colors");
  for ( auto &i : colors_json ) {
    source_cfg_.InsertTag(i.first, i.second.get_value<std::string>());
    std::cout << "inserting tag, key: " << i.first << " value: " << i.second.get_value<std::string>() << std::endl;
  }
  for ( auto &i : syntax_json ) {
    source_cfg_.InsertType(i.first, i.second.get_value<std::string>());    
    std::cout << "inserting type, key: " << i.first << " value: " << i.second.get_value<std::string>() << std::endl;
  }
}

void MainConfig::GenerateKeybindings() {
  string line;
  std::ifstream menu_xml("menu.xml");
  if (menu_xml.is_open()) {
    while (getline(menu_xml, line)) {
      keybindings_cfg_.AppendXml(line);
    }
  }
  boost::property_tree::ptree keys_json = cfg_.get_child("keybindings");
  for (auto &i : keys_json)
    keybindings_cfg_.key_map()[i.first] = i.second.get_value<std::string>();
}
Keybindings::Config& MainConfig::keybindings_cfg() {
  return keybindings_cfg_;
}
Source::Config& MainConfig::source_cfg() {
  return source_cfg_;
}
