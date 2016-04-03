import libjuci, os, gi
gi.require_version('Gtk', '3.0')

from gi.repository import Gtk
from libjuci import beta, editor, terminal
from os import path

def init() :
  append = """
[
    {
        "label": "_Edit",
        "menu_elements": [
            {
                "label":"_Snippet",
                "menu_elements": [
                    {
                        "label":"Insert snippet",
                        "keybinding":"<primary>space",
                        "action":"snippet.insert_snippet"
                    }
                ]
            }
        ]
    }
]
"""
  libjuci.add_menu_element(append)

def get_ifndef() :
  file_name = editor.get_file_name()
  package, file_name = path.split(file_name)
  file_name, file_ext = path.splitext(file_name)
  package = path.basename(package)
  guard = package + "_" + file_name + "_" + file_ext[1:len(file_ext)] + "\n"
  res = "#ifndef " + guard + '_'
  res += "#define " + guard + '_'
  res += "#endif  // " + guard + '_'
  return res  

def get_snippet(word) :
  snippets = {}

  snippets["for"] = """\
for (int i = 0; i < v.size(); i++) {
  // std::cout << v[i] << std::endl;
  // Write code here
}\
"""
  snippets["if"] = """\
if(){
  // Write code here
}\
"""

  snippets["cout"] = """\
cout << \
"""
  
  snippets["io"] = """\
#include <iostream>
using namespace std;
"""
  snippets["ifndef"] = get_ifndef()
  
  try :
    output = snippets[word]
  except KeyError :
    output = word
  return output
    
def get_iter_at_cursor(gtk_text_buffer) :
  mark = gtk_text_buffer.get_insert()
  return gtk_text_buffer.get_iter_at_mark(mark)

def get_current_line_text(gtk_text_buffer) :
  line_number = get_iter_at_cursor(gtk_text_buffer).get_line()
  start_iter = gtk_text_buffer.get_iter_at_line(line_number)
  end_iter = gtk_text_buffer.get_iter_at_line(line_number+1)
  end_iter.backward_char()
  return gtk_text_buffer.get_text(start_iter, end_iter, 0)

def add_indention(text_line, output) :
  tab_info = editor.get_tab_char_and_size();
  indent = 0
  for c in text_line :
    if c is not tab_info[0]:
      break
    indent = indent + 1
  if indent <= 0 :
    return output
  text_indent = ""
  for x in range(0, indent) :
    text_indent = text_indent + tab_info[0]
  text_lines = output.split('\n')
  res = ""
  for text_line in text_lines :
    res += text_indent + text_line + '\n'
  return res[indent:len(res)]

def insert_snippet() :
  gtk_text_buffer = editor.get_current_gtk_text_buffer()
  if not gtk_text_buffer :
    return
  input_line = get_current_line_text(gtk_text_buffer)
  if not input_line or input_line is "" :
    return
  line_offset = get_iter_at_cursor(gtk_text_buffer).get_line_index()
  first_backwards_space = input_line.rfind(" ", 0, line_offset)
  first_backwards_space = first_backwards_space + 1
  snippet_key_word = input_line[first_backwards_space:line_offset]
  snippet = get_snippet(snippet_key_word)
  if snippet == snippet_key_word :
    return
  snippet = add_indention(input_line, snippet)
  line_number = get_iter_at_cursor(gtk_text_buffer).get_line()
  begin_iter = gtk_text_buffer.get_iter_at_line_offset(line_number,first_backwards_space)
  end_iter = gtk_text_buffer.get_iter_at_line_offset(line_number,line_offset)
  gtk_text_buffer.delete(begin_iter, end_iter)
  gtk_text_buffer.insert_at_cursor(snippet, len(snippet))

init()
