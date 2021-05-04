// $Id: commands.cpp,v 1.20 2021-01-11 15:52:17-08 - - $
#include <vector>
#include <string>
#include "commands.h"
#include "debug.h"
#include "file_sys.h"
#include <string>
command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"rmr"   , fn_rmr   },
   {"#"     , fn_cmnt  },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such command");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int status = exec::status();
   cout << exec::execname() << ": exit(" << status << ")" << endl;
   return status;
}
void fn_cmnt(inode_state& state, const wordvec& words) {
    for (int i = 0; static_cast<unsigned long>(i) < words.size(); i++)
    {
        cout << words[i];
        if (static_cast<unsigned long>(i) != words.size())
        {
            cout << " ";
        }
    }
    cout << endl;
}

void fn_cat (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   if (words.size() == 1)
   {
       file_error error = file_error("ERROR: NO FILE NAME GIVEN");
       throw error;
       return;
   }
   //NAVIGATION ASSIGN SUPPOSED TO BE TO INODE PTR NOT CWD
   wordvec words2 = split(words[1], "/\t");
   inode_ptr stateHold = state.cwd;
   std::map<string, inode_ptr>::iterator it;
   for (int i = 0; static_cast<unsigned long>(i) < words2.size(); i++)
   {
       directory* a = static_cast<directory*>(state.cwd->contents.get());
       if (a->dirents.find(words2[i]) == a->dirents.end())
       {
           //throw error
           return;
       }
       it = a->dirents.find(words2[i]);
       state.cwd = it->second;
   }
   inode_ptr ptr = state.cwd;
   if (ptr->contents->type == file_type::PLAIN_TYPE) {
       plain_file* a = static_cast<plain_file*>(state.cwd->contents.get());
       const wordvec& b = a->readfile();
       for (size_t i = 0; i < b.size(); i++) {
           printf("%s ",b[i].c_str());
       }
       cout << endl;
   }
   DEBUGF ('c', words);
   state.cwd = stateHold;
}

void fn_cd(inode_state& state, const wordvec& words) {
    DEBUGF('c', state);
    DEBUGF('c', words);
    if (words.size() == 1)
    {
        file_error error = file_error("ERROR: NO DIRECTORY NAME GIVEN");
        throw error;
        return;
    }
    string pathname = words.back();
    directory* a = static_cast<directory*>(state.cwd->contents.get());
    std::map<string, inode_ptr>::iterator it;
    if (words.back() == ".." || words.back() == ".")
    {
        if (state.path.size() < 2)
        {
            return;
        }
        it = a->dirents.find("..");
        state.cwd = it->second;
        state.path.pop_back();
        return;
    }
    wordvec words2 = split(words[1], "/\t");
    inode_ptr stateHold = state.cwd;
    vector<string> holdVector; 
    for (int i = 0; static_cast<unsigned long>(i)
        < words2.size(); i++)
    {
        directory* b = static_cast<directory*>(state.cwd->contents.get());
        if (b->dirents.find(words2[i]) == b->dirents.end())
        {
            state.cwd = stateHold;
            file_error error = file_error("ERROR: DIRECTORY DOES NOT EXIST");
            throw error;
            return;
        }
        it = b->dirents.find(words2[i]);
        state.cwd = it->second;
        if (state.cwd->contents->type == file_type::PLAIN_TYPE)
        {
            state.cwd = stateHold;
            file_error error = file_error("ERROR: PATHNAME LEADS TO FILE NOT DIRECTORY");
            throw error;
        }
        holdVector.push_back(words2[i]);
    }
    for (int i = 0; static_cast<unsigned long>(i)
        < holdVector.size(); i++)
    {
        state.path.push_back(holdVector[i]);
    }
}

void fn_echo (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   try
   {
       int number = stoi(words.back());
       exec::status(number);
       throw ysh_exit();
   }
   catch (...)
   {
       exec::status(127);
       throw ysh_exit();

   }
}

void fn_ls(inode_state& state, const wordvec& words) {
    DEBUGF('c', state);
    inode_ptr stateHold = state.cwd;
    if (words.size() > 1) {
        wordvec words2 = split(words[1], "/\t");
        std::map<string, inode_ptr>::iterator it;
        for (int i = 0; static_cast<unsigned long>(i) < words2.size(); i++)
        {
            directory* a = static_cast<directory*>(state.cwd->contents.get());
            if (a->dirents.find(words2[i]) == a->dirents.end())
            {
                state.cwd = stateHold;
                file_error error = file_error("ERROR: DIRECTORY DOES NOT EXIST");
                throw error;
                return;
            }
            if (state.cwd->contents->type == file_type::PLAIN_TYPE)
            {
                file_error error = file_error("ERROR: PATHNAME LEADS TO FILE NOT DIRECTORY");
                throw error;
            }
            it = a->dirents.find(words2[i]);
            state.cwd = it->second;
        }
    }
     if (state.cwd->contents->type == file_type::PLAIN_TYPE)
     {
        file_error error = file_error("ERROR: PATHNAME LEADS TO FILE NOT DIRECTORY");
        throw error;
     }
     directory* dir = static_cast<directory*>(state.get_cwd()->contents.get());
     dir->list_dirents();
     DEBUGF('c', words);
     state.cwd = stateHold;
}
void fn_lsr(inode_state& state, const wordvec& words) {
    DEBUGF('c', state);
    inode_ptr stateHold = state.cwd;
    stack<string> name_stack;
    wordvec words2 = split(words[1], "/\t");
    std::map<string, inode_ptr>::iterator it;
    for (int i = 0; static_cast<unsigned long>(i) < words2.size(); i++)
    {
        directory* a = static_cast<directory*>(state.cwd->contents.get());
        if (a->dirents.find(words2[i]) == a->dirents.end())
        {
            state.cwd = stateHold;
            file_error error = file_error("ERROR: DIRECTORY DOES NOT EXIST");
            throw error;
            return;
        }
        it = a->dirents.find(words2[i]);
        state.cwd = it->second;
        if (state.cwd->contents->type == file_type::PLAIN_TYPE)
        {
            state.cwd = stateHold;
            file_error error = file_error("ERROR: PATHNAME LEADS TO FILE NOT DIRECTORY");
            throw error;
        }
    }
    inode_ptr dir = state.cwd;
    name_stack.push(state.path[state.path.size() - 1]);
    stack <inode_ptr>  dir_stack;
    dir_stack.push(dir);
    while (dir_stack.empty() != true) {
        dir = dir_stack.top();
        directory* temp = static_cast<directory*>(dir->contents.get());
        string name = name_stack.top();
        dir_stack.pop();
        name_stack.pop();
        temp->list_dirents_add_to(dir_stack, name_stack);
    }
    DEBUGF('c', words);
    state.cwd = stateHold;
}

void fn_make(inode_state& state, const wordvec& words) {
    DEBUGF('c', state);
    if (words.size() == 1)
    {
        file_error error = file_error("ERROR: NO FILE NAME GIVEN");
        throw error;
        return;
    }
    wordvec words2 = split(words[1], "/\t");
inode_ptr stateHold = state.cwd;
std::map<string, inode_ptr>::iterator it;
for (int i = 0; static_cast<unsigned long>(i) < words2.size() - 1; i++)
{
    directory* a = static_cast<directory*>(state.cwd->contents.get());
    if (a->dirents.find(words2[i]) == a->dirents.end())
    {
        state.cwd = stateHold;
        file_error error = file_error("ERROR: DIRECTORY DOES NOT EXIST");
        throw error;
        return;
    }
    if (state.cwd->contents->type == file_type::PLAIN_TYPE)
    {
        file_error error = file_error("ERROR: PATHNAME LEADS TO FILE NOT DIRECTORY");
        throw error;
    }
    it = a->dirents.find(words2[i]);
    state.cwd = it->second;
}
directory* a = static_cast<directory*>(state.cwd->contents.get());
if (words.size() > 2) {
    //PROVIDED INIT VAL
    wordvec vec;
    for (size_t i = 2; i < words.size(); i++) {
        vec.push_back(words[i]);
    }
    inode_ptr ptr = a->mkfile(words2.back());
    if (ptr == nullptr) {
        return;
    }
    plain_file* new_file = static_cast<plain_file*>(ptr->contents.get());
    new_file->writefile(vec);
}
else {
    a->mkfile(words2.back());
}
DEBUGF('c', words);
state.cwd = stateHold;
}

void fn_mkdir(inode_state& state, const wordvec& words) {
    DEBUGF('c', state);
    if (words.size() == 1)
    {
        file_error error = file_error("ERROR: NO DIRECTORY NAME GIVEN");
        throw error;
        return;
    }
    wordvec words2 = split(words[1], "/\t");
    inode_ptr stateHold = state.cwd;
    std::map<string, inode_ptr>::iterator it;
    for (int i = 0; static_cast<unsigned long>(i) < words2.size() - 1; i++)
    {
        directory* a = static_cast<directory*>(state.cwd->contents.get());
        if (a->dirents.find(words2[i]) == a->dirents.end())
        {
            state.cwd = stateHold;
            file_error error = file_error("ERROR: DIRECTORY DOES NOT EXIST");
            throw error;
            return;
        }
        if (state.cwd->contents->type == file_type::PLAIN_TYPE)
        {
            file_error error = file_error("ERROR: PATHNAME LEADS TO FILE NOT DIRECTORY");
            throw error;
        }
        it = a->dirents.find(words2[i]);
        state.cwd = it->second;
    }
    directory* a = static_cast<directory*>(state.cwd->contents.get());
    a->mkdir(words2.back());
    state.cwd = stateHold;
}

void fn_prompt(inode_state& state, const wordvec& words) {
    DEBUGF('c', state);
    if (words.size() == 1)
    {
        file_error error = file_error("ERROR: NO PROMPT STRING GIVEN");
        throw error;
        return;
    }
    string temp = "";
    for (long i = 1; static_cast<unsigned>(i)
        < words.size(); i++)
    {
        if (static_cast<unsigned long>(i) == words.size() - 1)
        {
            temp.append(words[i]);
            continue;
        }
        temp.append(words[i]);
        temp.append(" ");
    }
    state.prompt(temp);
    DEBUGF('c', words);
}

void fn_pwd(inode_state& state, const wordvec& words) {
    DEBUGF('c', state);
    DEBUGF('c', words);
    for (long i = 0; static_cast<unsigned>(i)
        < state.path.size(); i++)
    {
        if (static_cast<unsigned long>(i) == state.path.size() - 1)
        {
            cout << state.path[i] << endl;
            continue;
        }
        cout << state.path[i];
        if (i != 0)
        {
            cout << "/";
        }

    }
}

void fn_rm (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   wordvec words2 = split(words[1], "/\t");
   inode_ptr stateHold = state.cwd;
   std::map<string, inode_ptr>::iterator it;
   for (int i = 0; static_cast<unsigned long>(i) < words2.size() - 1; i++)
   {
       directory* a = static_cast<directory*>(state.cwd->contents.get());
       if (a->dirents.find(words2[i]) == a->dirents.end())
       {
           state.cwd = stateHold;
           file_error error = file_error("ERROR: DIRECTORY DOES NOT EXIST");
           throw error;
           return;
       }
       if (state.cwd->contents->type == file_type::PLAIN_TYPE)
       {
           file_error error = file_error("ERROR: PATHNAME LEADS TO FILE NOT DIRECTORY");
           throw error;
       }
       it = a->dirents.find(words2[i]);
       state.cwd = it->second;
   }
   directory* a = static_cast<directory*>(state.cwd->contents.get());

   //string remove_word = "";
   a->remove(words2.back());
   DEBUGF ('c', words);
   state.cwd = stateHold;
}

void fn_rmr(inode_state& state, const wordvec& words) {
    DEBUGF('c', state);
    wordvec words2 = split(words[1], "/\t");
    inode_ptr stateHold = state.cwd;
    std::map<string, inode_ptr>::iterator it;
    bool isNested = false;
    if (words2.size() > 0)
    {
        isNested = true;
    }
    for (int i = 0; static_cast<unsigned long>(i) < words2.size(); i++)
    {
        directory* a = static_cast<directory*>(state.cwd->contents.get());
        if (a->dirents.find(words2[i]) == a->dirents.end())
        {
            state.cwd = stateHold;
            file_error error = file_error("ERROR: DIRECTORY DOES NOT EXIST");
            throw error;
            return;
        }
        if (state.cwd->contents->type == file_type::PLAIN_TYPE)
        {
            file_error error = file_error("ERROR: PATHNAME LEADS TO FILE NOT DIRECTORY");
            throw error;
        }
        it = a->dirents.find(words2[i]);
        state.cwd = it->second;
    }
    inode_ptr dir = state.cwd;
    directory* temp = static_cast<directory*>(dir->contents.get());
    temp->remove_recursive();
    if (isNested == true)
    {
        string dirName = it->first;
        it = temp->dirents.find("..");
        state.cwd = it->second;
        state.cwd->contents->remove(dirName);
    }
    DEBUGF('c', words);
    state.cwd = stateHold;
}