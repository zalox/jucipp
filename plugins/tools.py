import libjuci, os

favorite = "/home/zalox/projects" # change to your favorite project
favorite_file = "/home/zalox/.juci/plugins/gtkplugin.py"

def init() :
  append_project = """
[
    {
        "label": "_Plugins",
        "menu_elements": [
            {
                "label":"_Tools",
                "menu_elements": [
                    {
                        "label":"Open favorite folder",
                        "keybinding":"<primary>j",
                        "action":"tools.open_favorite_folder"
                    },
                    {
                        "label":"Open plugins folder",
                        "keybinding":"<primary>p",
                        "action":"tools.open_plugin_folder"
                    },
                    {
                        "label":"Open plugins folder",
                        "keybinding":"<primary>i",
                        "action":"tools.open_favorite_file"
                    }
                ]
            }
        ]
    }
]
  """
  libjuci.add_menu_element(append_project)
  
def open_favorite_folder() :
  libjuci.directories.open(favorite)
  
def open_favorite_file() :
  libjuci.directories.open(favorite_file)
 
def open_plugin_folder() :
  folder = libjuci.get_juci_home()
  folder += "/plugins" # may break windows
  libjuci.directories.open(folder)

init()
