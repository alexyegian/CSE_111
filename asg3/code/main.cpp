//Alex Soliz ID: amsoliz
//Alexander Yegian ID: ayegian
// $Id: main.cpp,v 1.13 2021-02-01 18:58:18-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <regex>
#include <cassert>
#include <fstream>

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

int main(int argc, char** argv) {
    sys_info::execname(argv[0]);
    scan_options(argc, argv);

    str_str_map test;

    int exitStatus = 0;
    regex comment_regex{ R"(^\s*(#.*)?$)" };
    regex key_value_regex{ R"(^\s*(\S{1}[^=]*?)=(.*?\S+)\s*$)" };
    regex trimmed_regex{ R"(^\s*([^=]+?)\s*$)" };
    regex printAll_regex{ R"(^\s*(=)\s*$)" };
    regex printValue_regex{ R"(^\s*=(\S*)\s*$)" };
    regex erase_regex{ R"(^\s*(\S+)\s*=\s*$)" };
    if (argc == 1)
    {
        int count = 1;
        for (;;) {
            string line;
            getline(cin, line);
            if (cin.eof()) break;
            cout << "-: " << count << ": " << line << endl;
            smatch result;
            if (regex_search(line, result, comment_regex)) {
            }
            else if (regex_search(line, result, key_value_regex)) {
                const string key = result[1];
                const string value = result[2];
                str_str_pair  newPair(key, value);
                auto insertItr = test.insert(newPair);
                auto insertNode = insertItr.get_where();
                cout << insertNode->value.first << 
                    " = " << insertNode->value.second << endl;
            }
            else if (regex_search(line, result, trimmed_regex)) {
                auto findResult = test.find(result[1]);
                auto holdNode = findResult.get_where();
                if (findResult == test.end())
                {
                    cout << result[1] << ": key not found" << endl;
                }
                else
                {
                    cout << holdNode->value.first << 
                        " = " << holdNode->value.second << endl;
                }
            }
            else if (regex_search(line, result, printAll_regex)) {
                test.printall();
            }
            else if (regex_search(line, result, printValue_regex)) {
                str_str_pair testPair(result[1], result[1]);
                test.printValue(testPair.second);
            }
            else if (regex_search(line, result, erase_regex)) {
                auto findResult = test.find(result[1]);
                test.erase(findResult);
            }
            else {
                assert(false and "This can not happen.");
            }
            ++count;
        }
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            string fileName = argv[i];
            int count = 1;
            string line;
            string dash = "-";
            string name = *(argv + i);
            if (name == dash) {
                while (getline(cin, line)) {
                    cout << *(argv + i) << ": " 
                        << count << ": " << line << endl;
                    smatch result;
                    if (regex_search(line, result, comment_regex)) {
                        ++count;
                        continue;
                    }
                    else if (regex_search(line, 
                        result, key_value_regex)) {
                        const string key = result[1];
                        const string value = result[2];
                        str_str_pair  newPair(key, value);
                        auto insertItr = test.insert(newPair);
                        auto insertNode = insertItr.get_where();
                        cout << insertNode->value.first << 
                            " = " << insertNode->value.second 
                            << endl;
                    }
                    else if (regex_search(line, 
                        result, trimmed_regex)) {
                        auto findResult = test.find(result[1]);
                        auto holdNode = findResult.get_where();
                        if (findResult == test.end())
                        {
                            cout << result[1] <<
                                ": key not found" << endl;
                        }
                        else
                        {
                            cout << holdNode->value.first << 
                                " = " << holdNode->value.second << endl;
                        }
                    }
                    else if (regex_search(line, 
                        result, printAll_regex)) {
                        test.printall();
                    }
                    else if (regex_search(line, 
                        result, printValue_regex)) {
                        str_str_pair testPair(result[1], result[1]);
                        test.printValue(testPair.second);
                    }
                    else if (regex_search(line, result, erase_regex)) {
                        auto findResult = test.find(result[1]);
                        test.erase(findResult);
                    }
                    else {
                        assert(false and "This can not happen.");
                    }
                    ++count;
                }
            }
            else {
                ifstream MyReadFile(*(argv + i));
                if (!MyReadFile.is_open())
                {
                    cerr << "keyvalue: " << *(argv + i) 
                        << ": " << "No such file or directory" << endl;
                    exitStatus = 1;
                    continue;
                }
                while (getline(MyReadFile, line))
                {
                    cout << *(argv + i) << ": " <<
                        count << ": " << line << endl;
                    smatch result;
                    if (regex_search(line, result, comment_regex)) {
                        ++count;
                        continue;
                    }
                    else if (regex_search(line, 
                        result, key_value_regex)) {
                        const string key = result[1];
                        const string value = result[2];
                        str_str_pair  newPair(key, value);
                        auto insertItr = test.insert(newPair);
                        auto insertNode = insertItr.get_where();
                        cout << insertNode->value.first 
                            << " = " << insertNode->value.second 
                            << endl;
                    }
                    else if (regex_search(line, 
                        result, trimmed_regex)) {
                        auto findResult = test.find(result[1]);
                        auto holdNode = findResult.get_where();
                        if (findResult == test.end())
                        {
                            cout << result[1] << 
                                ": key not found" << endl;
                        }
                        else
                        {
                            cout << holdNode->value.first << " = " << 
                                holdNode->value.second << endl;
                        }
                    }
                    else if (regex_search(line, 
                        result, printAll_regex)) {
                        test.printall();
                    }
                    else if (regex_search(line, 
                        result, printValue_regex)) {
                        str_str_pair testPair(result[1], result[1]);
                        test.printValue(testPair.second);
                    }
                    else if (regex_search(line, result, erase_regex)) {
                        auto findResult = test.find(result[1]);
                        test.erase(findResult);
                    }
                    else {
                        assert(false and "This can not happen.");
                    }
                    ++count;
                }
            }

        }
        return exitStatus;
    }
}
