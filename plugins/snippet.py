import libjuci, os

from libjuci import beta, editor
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
    res = "#ifndef " + guard
    res += "#define " + guard
    res += "#endif  // " + guard
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

def get_iter_at_cursor(view) :
  buf = view.get_buffer()
  mark = buf.get_insert() # cursor pos
  return buf.get_iter_at_mark(mark)

def get_line_number(view) :
  text_iter = get_iter_at_cursor(view)
  if text_iter :
    return text_iter.get_line()

def get_current_line_text(view) :
  buf = view.get_buffer()
  start_iter = buf.get_iter_at_line(get_line_number(view))
  end_iter = buf.get_iter_at_line(get_line_number(view)+1)
  end_iter.backward_char()
  return buf.get_text(start_iter, end_iter, 0)
  
def insert_snippet() :
  view = editor.get_current_gtk_text_view()
  if view :
    input_line = get_current_line_text(view)
    if input_line != "" :
      line_offset=get_iter_at_cursor(view).get_line_index()
      first_backwards_space = input_line.rfind(" ", 0, line_offset)
      first_backwards_space = first_backwards_space + 1
      key_word = input_line[first_backwards_space:line_offset]
      output = get_snippet(key_word)
      if output != key_word :
        buf = view.get_buffer()
        begin_iter = buf.get_iter_at_line_offset(get_line_number(view),first_backwards_space)
        end_iter = buf.get_iter_at_line_offset(get_line_number(view),line_offset)
        buf.delete(begin_iter, end_iter)
        buf.insert_at_cursor(output, len(output))
    
init()