// $Id: main.cpp,v 1.13 2021-02-01 18:58:18-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <regex>
#include <cassert>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

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
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

int main (int argc, char** argv) {
   sys_info::execname (argv[0]);
   scan_options (argc, argv);

   str_str_map test;
   cout << test << endl;
   for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) {
      str_str_pair pair (*argp, to_string<int> (argp - argv));
      cout << "Before insert: " << pair << endl;
      test.insert (pair);
   }

   regex comment_regex{ R"(^\s*(#.*)?$)" };
   regex key_value_regex{ R"(^\s*(.*?)\s*=\s*(.*?)\s*$)" };
   regex trimmed_regex{ R"(^\s*([^=]+?)\s*$)" };
   for (;;) {
       string line;
       getline(cin, line);
       if (cin.eof()) break;
       cout << "input: \"" << line << "\"" << endl;
       smatch result;
       if (regex_search(line, result, comment_regex)) {
           //cout << "Comment or empty line." << endl;
           const string asdf = "b";
           str_str_pair testPair (asdf, asdf);
           test.printValue(testPair.second);
       }
       else if (regex_search(line, result, key_value_regex)) {
           cout << "key  : \"" << result[1] << "\"" << endl;
           cout << "value: \"" << result[2] << "\"" << endl;
           const string key = result[1];
           const string value = result[2];
           str_str_pair  newPair (key,value);
           test.insert(newPair);
       }
       else if (regex_search(line, result, trimmed_regex)) {
           cout << "query: \"" << result[1] << "\"" << endl;
           auto findResult = test.find(result[1]);
           auto holdNode = findResult.get_where();
           if (findResult == test.end())
           {
               cout << result[1] << ": key not found" << endl;
           }
           else
           {
               cout << holdNode->value.first << " = " << holdNode->value.second << endl;
           }
       }
       else {
           assert(false and "This can not happen.");
       }
   }

   cout << test.empty() << endl;
   for (str_str_map::iterator itor = test.begin();
        itor != test.end(); ++itor) {
      cout << "During iteration: " << *itor << endl;
   }

   str_str_map::iterator itor = test.begin();
   test.erase (itor);

   cout << "EXIT_SUCCESS" << endl;
   return EXIT_SUCCESS;
}



