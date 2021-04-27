// $Id: file_sys.cpp,v 1.10 2021-04-10 14:23:40-07 - - $

#include <cassert>
#include <iostream>
#include <stdexcept>

using namespace std;

#include "debug.h"
#include "file_sys.h"

size_t inode::next_inode_nr {1};

ostream& operator<< (ostream& out, file_type type) {
   switch (type) {
      case file_type::PLAIN_TYPE: out << "PLAIN_TYPE"; break;
      case file_type::DIRECTORY_TYPE: out << "DIRECTORY_TYPE"; break;
      default: assert (false);
   };
   return out;
}

inode_state::inode_state() {
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
}

const string& inode_state::prompt() const { return prompt_; }

void inode_state::prompt(const string& str) {
    this->prompt_ = str;
}
inode_ptr inode_state::get_cwd() {
    return this->cwd;
}
inode_ptr inode_state::get_root() {
    return this->root;
}
ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           printf("LOCATION PLAIN: %p\n", static_cast<void*>(contents.get()));
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           printf("LOCATION DIR: %p\n", static_cast<void *>(contents.get()));
           break;
      default: assert (false);
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}
string inode::get_name() {
    return this->name;
}
size_t inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}


file_error::file_error (const string& what):
            runtime_error (what) {
}

const wordvec& base_file::readfile() const {
   throw file_error ("is a " + error_file_type());
}

void base_file::writefile (const wordvec&) {
   throw file_error ("is a " + error_file_type());
}

void base_file::remove (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkdir (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkfile (const string&) {
   throw file_error ("is a " + error_file_type());
}


size_t plain_file::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
}

size_t directory::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   if (this->dirents.find(dirname) == dirents.end()) {
       //ERROR DIRNAME ALREADY EXISTS
       return nullptr;
   }
    //MAKE DIRETORY
   file_type type = file_type::DIRECTORY_TYPE;
   inode_ptr ptr = make_shared<inode>(type);

   //MAP TO DIRNAME
   this->dirents.insert({ dirname, ptr });
   //new directory below this one
   //dirents.insert
   //inode node = inode(type);
   printf("NEW DIR POS: %p\n", static_cast<void*>(ptr.get()));
   return ptr;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   if (this->dirents.find(filename) == dirents.end()) {
       //ERROR DIRNAME ALREADY EXISTS
       return nullptr;
   }
   file_type type = file_type::PLAIN_TYPE;
   inode_ptr ptr = make_shared<inode>(type);
   this->dirents.insert({ filename, ptr });

   return nullptr;
}

