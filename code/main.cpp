// $Id: main.cpp,v 1.11 2020-10-28 11:37:57-07 - - $

#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <unistd.h>

using namespace std;

#include "commands.h"
#include "debug.h"
#include "file_sys.h"
#include "util.h"

// scan_options
//    Options analysis:  The only option is -Dflags. 

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << static_cast<char> (option)
                       << ": invalid option" << endl;
            break;
      }
   }
   if (optind < argc) {
      complain() << "operands not permitted" << endl;
   }
}


// main -
//    Main program which loops reading commands until end of file.

int main (int argc, char** argv) {
   exec::execname (argv[0]);
   cout << boolalpha;  // Print false or true instead of 0 or 1.
   cerr << boolalpha;
   cout << argv[0] << " build " << __DATE__ << " " << __TIME__ << endl;
   scan_options (argc, argv);
   bool need_echo = want_echo();
   inode_state state;
   //CREATING NEW ROOT
   printf("INODE ROOT: %p  CWD: INODE ROOT: %p\n", static_cast<void*>(state.root.get()),  static_cast<void*>(state.cwd.get()));
   //inode_ptr ptr = 0;
   file_type type2 = file_type::DIRECTORY_TYPE;
   inode ptr2 = inode(type2);
   try {
      for (;;) {
         try {
            // Read a line, break at EOF, and echo print the prompt
            // if one is needed.
            cout << state.prompt();
            string line;
            getline (cin, line);
            if (cin.eof()) {
               if (need_echo) cout << "^D";
               cout << endl;
               DEBUGF ('y', "EOF");
               break;
            }
            if (need_echo) cout << line << endl;
            printf("LINE: %s\n", line.c_str());
            // Split the line into words and lookup the appropriate
            // function.  Complain or call it.
            wordvec words = split (line, " \t");
            DEBUGF ('y', "words = " << words);

            for (auto i = words.begin(); i != words.end(); ++i) {
                printf("WORD: %s\n", i->c_str());
            }

            command_fn fn = find_command_fn (words.at(0));
            fn(state, words);
            size_t bef_size = words.size();
            for (size_t i = 0; i <bef_size; i++) {
                printf("ONCE: %lu\n", words.size());
                words.pop_back();
            }
         }catch (file_error& error) {
            complain() << error.what() << endl;
            exec::status(1);
         }catch (command_error& error) {
            complain() << error.what() << endl;
         }
      }
   } catch (ysh_exit&) {
      // This catch intentionally left blank.
   }

   return exit_status_message();
}

