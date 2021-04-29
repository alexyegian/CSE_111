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
    //MAKE DIRETORY
    file_type type_ = file_type::DIRECTORY_TYPE;
    inode_ptr ptr = make_shared<inode>(type_);
    //MAP TO DIRNAME
    directory* a = static_cast<directory*>(ptr->contents.get());
    a->dirents.insert({ ".", ptr });
    a->dirents.insert({ "..", ptr });
    path.push_back("\\");
    //new directory below this one
    //dirents.insert
    //inode node = inode(type);
   // printf("NEW DIR POS: %p\n", static_cast<void*>(ptr.get()));
   //DEBUGF ('i', "root = " << root << ", cwd = " << cwd
   //       << ", prompt = \"" << prompt() << "\"");
    root = ptr;
    cwd = ptr;
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

inode::inode(file_type type_): inode_nr (next_inode_nr++) {
   switch (type_) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           printf("LOCATION PLAIN: %p\n", static_cast<void*>(contents.get()));
           contents->type = type_;
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           printf("LOCATION DIR: %p\n", static_cast<void *>(contents.get()));
           contents->type = type_;
           break;
      default: assert (false);
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type_);
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
   size += this->data.size();
   for (auto i = this->data.begin(); i != this->data.end(); ++i){
       string temp = *i;
       size += temp.length();
   }
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
    data = words;
   DEBUGF ('i', words);
}

size_t directory::size() const {
   size_t size {0};
   printf("CHECKING MAP SIZE\n");
   size = this->dirents.size();
   printf("CHECKING SIZE DIR, THIS SIZE IS: %lu\n", size);
   DEBUGF ('i', "size = " << size);
   return size;
}
void directory::list_dirents() {
    for (auto i = this->dirents.begin(); i != this->dirents.end(); ++i) {
        if (i->first != "." && i->first != "..") {
            if (i->second->contents->type == file_type::PLAIN_TYPE) {
                plain_file* temp = static_cast<plain_file*>(i->second->contents.get());
                printf("INODE: %lu  SIZE: %lu  NAME: %s\n", i->second->get_inode_nr(), temp->size(), i->first.c_str());
            }
            else {
                directory* temp = static_cast<directory*>(i->second->contents.get());
                printf("INODE: %lu  SIZE: %lu  NAME: %s\n", i->second->get_inode_nr(), temp->size()-2, i->first.c_str());
            }
        }

    }
}
void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
   auto child = this->dirents.find(filename);
   if (child != dirents.end()) {
       inode_ptr real_child = child->second;
       if (real_child->contents->type == file_type::PLAIN_TYPE) {
           //file not a direcotry
           this->dirents.erase(filename);
           real_child.reset();
           return;
       }
       directory* a = static_cast<directory*>(real_child->contents.get());
       if (a->size()) {
           printf("HELLOW");
           this->dirents.erase(filename);
           real_child.reset();
       }

   }
   //ERROR PATH NO EXIST
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   if (this->dirents.find(dirname) != dirents.end()) {
       //ERROR DIRNAME ALREADY EXISTS
       return nullptr;
   }
    //MAKE DIRETORY
   file_type type_ = file_type::DIRECTORY_TYPE;
   inode_ptr ptr = make_shared<inode>(type_);

   //MAP TO DIRNAME
   this->dirents.insert({ dirname, ptr });
   directory* a = static_cast<directory*>(ptr->contents.get());
   inode_ptr this_node = this->dirents.find(".")->second;
   a->dirents.insert({".", ptr});
   a->dirents.insert({ "..", this_node });
   //new directory below this one
   //dirents.insert
   //inode node = inode(type);
   printf("NEW DIR POS: %p\n", static_cast<void*>(ptr.get()));
   return ptr;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   if (this->dirents.find(filename) != dirents.end()) {
       //ERROR DIRNAME ALREADY EXISTS
       return nullptr;
   }
   file_type type_ = file_type::PLAIN_TYPE;
   inode_ptr ptr = make_shared<inode>(type_);
   this->dirents.insert({ filename, ptr });

   return ptr;
}

//inode_ptr directory::mkfile2(const string& filename, const wordvec& init_val) {
//    DEBUGF('i', filename);
//    if (this->dirents.find(filename) != dirents.end()) {
//        //ERROR DIRNAME ALREADY EXISTS
//        return nullptr;
//    }
//    printf("%s\n", init_val[0].c_str());
//    file_type type_ = file_type::PLAIN_TYPE;
//    inode_ptr ptr = make_shared<inode>(type_);
//    this->dirents.insert({ filename, ptr });
//
//    return nullptr;
//}

//inode_ptr directory::mkfile(const string& filename, const wordvec& init_val) {
//    DEBUGF('i', filename);
//    if (this->dirents.find(filename) != dirents.end()) {
//        //ERROR DIRNAME ALREADY EXISTS
//        return nullptr;
//    }
//    file_type type_ = file_type::PLAIN_TYPE;
//    inode_ptr ptr = make_shared<inode>(type_);
//    printf("%s\n", init_val[0].c_str());
//    //plain_file* a = static_cast<plain_file*>(ptr->contents.get());
//    //a->writefile(init_val);
//    this->dirents.insert({ filename, ptr });
//
//    return nullptr;
//}

