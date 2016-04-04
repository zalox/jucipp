import libjuci, gi, os
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, Gio

from libjuci import beta

favorite = os.environ['HOME']
plugin = os.environ['HOME']
favorite_file = os.environ['HOME']

def add_menu(position, label, items) :
  plugin_menu = beta.get_menu_from_name("plugin_menu")
  if not plugin_menu :
    libjuci.terminal.println("plugin_menu menu does not exist")
    return
  sub_menu = Gio.Menu.new()
  i = 0
  app = beta.get_gtk_application()
  for item in items :
    sub_menu.insert(i, item["label"], "app."+item["action"])
    i = i + 1
    python_action = Gio.SimpleAction.new(item["action"], None)
    python_action.connect("activate", item["method"])
    app.add_action(python_action)
    app.add_accelerator(item["accel"], "app."+item["action"], None)
    
  if plugin_menu.get_n_items() >= position :
    plugin_menu.remove(position)
    
  plugin_menu.insert_submenu(position, label, sub_menu)

def init() :
  items = [
            {
              "label": "Open plugin folder",
              "action": "open_plugin_folder",
              "accel": "<primary>p",
              "method": open_plugin_folder
            },
            {
              "label": "Open favorite folder",
              "action": "open_favorite_folder",
              "accel": "<primary>j",
              "method": open_favorite_folder
            },
            {
              "label": "Open favorite file",
              "action": "open_favorite_file",
              "accel": "<primary><shift>j",
              "method": open_favorite_file
            }
  ]
  add_menu(0, "_Tools", items)
  
def open_favorite_folder(action, param) :
  libjuci.directories.open(favorite)
  
def open_favorite_file(action, param) :
  libjuci.directories.open(favorite_file)
 
def open_plugin_folder(action, param) :
  libjuci.directories.open(plugin)

init()