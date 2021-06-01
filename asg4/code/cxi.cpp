// $Id: cxi.cpp,v 1.5 2021-05-18 01:32:29-07 - - $

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include "debug.h"
#include "logstream.h"
#include "protocol.h"
#include "socket.h"

logstream outlog (cout);
struct cxi_exit: public exception {};

unordered_map<string,cxi_command> command_map {
   {"exit", cxi_command::EXIT},
   {"help", cxi_command::HELP},
   {"ls"  , cxi_command::LS  },
   {"put"  , cxi_command::PUT},
   {"rm"  , cxi_command::RM  },
   {"get"  , cxi_command::GET},
};

static const char help[] = R"||(
exit         - Exit the program.  Equivalent to EOF.
get filename - Copy remote file to local host.
help         - Print help summary.
ls           - List names of files on remote server.
put filename - Copy local file to remote host.
rm filename  - Remove file from remote server.
)||";

void cxi_help() {
   cout << help;
}

void cxi_put(client_socket& server, string filename) {
    outlog << "FILENAME: " << filename << endl;
    std::ifstream pFile(filename.c_str(), std::ifstream::in);

    if (pFile.fail()) {
        outlog << "FILE DOESN'T EXIST ERROR" << endl;
    }
    else
    {
        cxi_header header;
        if (filename.length() >= 59) {
            return;
        }
        else {
            filename.copy(header.filename, 59);
            header.filename[filename.length()] = '\0';
        }
        pFile.seekg(0, ios::end);
        uint32_t file_size = pFile.tellg();
        uint32_t nbytes = file_size;
        header.command = cxi_command::PUT;
        header.nbytes = nbytes;
        std::string temp;
        pFile.seekg(0, ios::beg);
        std::string line;
        while (std::getline(pFile, line))
        {
            temp += line + "\n";
        }
        const char* data = temp.c_str();
        send_packet(server, &header, sizeof header);
        send_packet(server, data, header.nbytes);
        recv_packet(server, &header, sizeof header);
        if (header.command != cxi_command::ACK) {
            outlog << "ERROR" << endl;
        }
        else {
            outlog << "SUCCESS" << endl;
        }
    }
    return;
}


void cxi_rm(client_socket& server, string filename) {
    cxi_header header;
    if (filename.length() >= 59) {
        return;
    }
    else {
        filename.copy(header.filename, 59);
        header.filename[filename.length()] = '\0';
    }
    header.command = cxi_command::RM;
    header.nbytes = 0;
    send_packet(server, &header, sizeof header);
    recv_packet(server, &header, sizeof header);
    if (header.command != cxi_command::ACK) {
        outlog << "ERROR" << endl;
    }
    else {
        outlog << "SUCCESS" << endl;
    }
}

void cxi_get(client_socket& server, string filename) {
    cxi_header header;
    if (filename.length() >= 59) {
        outlog << "ERROR: filename too long" << endl;
        return;
    }
    else {
        filename.copy(header.filename, 59);
        header.filename[filename.length()] = '\0';
    }
    uint32_t nbytes = 0;
    header.command = cxi_command::GET;
    header.nbytes = nbytes;
    send_packet(server, &header, sizeof header);
    recv_packet(server, &header, sizeof header);
    if (header.command == cxi_command::NAK) {
        outlog << "ERROR: file does not exist" << endl;
        return;
    }
    //prep for payload
    //check if file exists, handle it if it does
    auto rec_buff = make_unique<char[]>(header.nbytes + 1);
    ofstream myFile(filename);
    outlog << "file made" << endl;
    recv_packet(server, rec_buff.get(), sizeof header);
    //handle payload
    myFile.write(reinterpret_cast<char*>(rec_buff.get()), header.nbytes);
    outlog << "file write" << endl;
    myFile.close();
    outlog << "cxid asdf" << endl;
}

void cxi_ls (client_socket& server) {
   cxi_header header;
   header.command = cxi_command::LS;
   DEBUGF ('h', "sending header " << header << endl);
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   DEBUGF ('h', "received header " << header << endl);
   if (header.command != cxi_command::LSOUT) {
      outlog << "sent LS, server did not return LSOUT" << endl;
      outlog << "server returned " << header << endl;
   }else {
      size_t host_nbytes = ntohl (header.nbytes);
      auto buffer = make_unique<char[]> (host_nbytes + 1);
      recv_packet (server, buffer.get(), host_nbytes);
      DEBUGF ('h', "received " << host_nbytes << " bytes");
      buffer[host_nbytes] = '\0';
      cout << buffer.get();
   }
}


void usage() {
   cerr << "Usage: " << outlog.execname() << " host port" << endl;
   throw cxi_exit();
}

pair<string,in_port_t> scan_options (int argc, char** argv) {
   for (;;) {
      int opt = getopt (argc, argv, "@:");
      if (opt == EOF) break;
      switch (opt) {
         case '@': debugflags::setflags (optarg);
                   break;
      }
   }
   if (argc - optind != 2) usage();
   string host = argv[optind];
   in_port_t port = get_cxi_server_port (argv[optind + 1]);
   return {host, port};
}

vector<string> split(const string& line, const string& delimiters) {
    vector<string> words;
    size_t end = 0;

    // Loop over the string, splitting out words, and for each word
    // thus found, append it to the output wordvec.
    for (;;) {
        size_t start = line.find_first_not_of(delimiters, end);
        if (start == string::npos) break;
        end = line.find_first_of(delimiters, start);
        words.push_back(line.substr(start, end - start));
    }
    return words;
}

int main (int argc, char** argv) {
   outlog.execname (basename (argv[0]));
   outlog << to_string (hostinfo()) << endl;
   try {
      auto host_port = scan_options (argc, argv);
      string host = host_port.first;
      in_port_t port = host_port.second;
      outlog << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      outlog << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         getline (cin, line);
         if (cin.eof()) throw cxi_exit();
         outlog << "command " << line << endl;
         vector<string> words = split(line, " \t");
         outlog << words[0] << ":" << endl;
         const auto& itor = command_map.find (words[0]);
         cxi_command cmd = itor == command_map.end()
                         ? cxi_command::ERROR : itor->second;
         switch (cmd) {
            case cxi_command::EXIT:
               throw cxi_exit();
               break;
            case cxi_command::HELP:
               cxi_help();
               break;
            case cxi_command::LS:
               cxi_ls (server);
               break;
            case cxi_command::PUT: 
            {
                if (words.size() < 2)
                {
                    outlog << line << ": invalid command" << endl;
                    break;
                }
                cxi_put(server, words[1]);
                break; 
            }
            case cxi_command::RM:
            {
                if (words.size() < 2)
                {
                    outlog << line << ": invalid command" << endl;
                    break;
                }
                cxi_rm(server, words[1]);
                break;
            }
            case cxi_command::GET:
            {
                if (words.size() < 2)
                {
                    outlog << line << ": invalid command" << endl;
                    break;
                }
                cxi_get(server, words[1]);
                break;
            }
            default:
               outlog << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cxi_exit& error) {
      DEBUGF ('x', "caught cxi_exit");
   }
   return 0;
}

