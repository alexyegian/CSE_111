// $Id: commands.cpp,v 1.20 2021-01-11 15:52:17-08 - - $
#include <vector>
#include <string>
#include "commands.h"
#include "debug.h"
#include "file_sys.h"
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

void fn_cat (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_cd(inode_state& state, const wordvec& words) {
    DEBUGF('c', state);
    DEBUGF('c', words);
    string pathname = words.back();
    directory* a = static_cast<directory*>(state.cwd->contents.get());
    std::map<string, inode_ptr>::iterator it;
    if (words.back() == "..")
    {
        it = a->dirents.find("..");
        state.cwd = it->second;
        state.path.pop_back();
        return;
    }
    //check if pathname is a directory not a file
    wordvec words2 = split(words[1], "/\t");
    inode_ptr stateHold = state.cwd;
    for (int i = 0; static_cast<unsigned long>(i)
        < words2.size(); i++)
    {
        if (a->dirents.find(pathname) == a->dirents.end())
        {
            state.cwd = stateHold;
            //error
            return;
        }
        directory* b = static_cast<directory*>(state.cwd->contents.get());
        it = b->dirents.find(words2[i]);
        state.cwd = it->second;
        state.path.push_back(words2[i]);
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
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   inode_ptr start_ptr = state.get_cwd();
   if (words.size() > 1) {
       printf("WORDS 1: %s\n", words[1].c_str());

       // "dir1/2/3"
       // dir1 2 3
       //NAVIGATION GOES HERE

       //CHANGE DIRECTORY TO DESTINATION, DO THIS, THEN CHANGE CWD BACK TO OLD ONE
   }

   directory* dir = static_cast<directory*>(state.get_cwd()->contents.get());
   dir->list_dirents();
   state.cwd = start_ptr;
   DEBUGF ('c', words);
}

void fn_lsr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   directory* a = static_cast<directory*>(state.cwd->contents.get());
   printf("%s\n", words.back().c_str());
   a->mkfile(words.back());
   DEBUGF ('c', words);
}

void fn_mkdir (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   directory* a = static_cast<directory*>(state.cwd->contents.get());
   printf("%s\n", words.back().c_str());
   a->mkdir(words.back());
   DEBUGF ('c', words);
}

void fn_prompt (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   string new_prompt = words.back();
   state.prompt(new_prompt);
   DEBUGF ('c', words);
}

void fn_pwd(inode_state& state, const wordvec& words) {
    DEBUGF('c', state);
    DEBUGF('c', words);
    for (long i = 0; static_cast<unsigned>(i)
        < state.path.size(); i++)
    {
        if (state.path.size() == 0)
        {
            cout << "PWD PATHCOPY EMPTY" << endl;
        }
        if (i != 0 && static_cast<unsigned long>(i) == state.path.size() - 1)
        {
            cout << state.path[i] << endl;
            continue;
        }
        cout << state.path[i] << "\\";

    }
}

void fn_rm (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   if (words.size() > 1) {
   }
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

