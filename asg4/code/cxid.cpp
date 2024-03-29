// $Id: cxid.cpp,v 1.8 2021-05-18 01:32:29-07 - - $

#include <iostream>
#include <memory>
#include <string>
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

void reply_put(accepted_socket& client_sock, cxi_header& header) {
    //ALWAYS ACK NOW ADD IN ERRORS LATER
    header.command = cxi_command::ACK;

    //size_t host_nbytes = htonl(header.nbytes);
    auto rec_buff = make_unique<char[]>(header.nbytes + 1);
    recv_packet(client_sock, rec_buff.get(), header.nbytes);
    rec_buff[header.nbytes] = '\0';
    string filenameStr;
    string holdStr;
    for (int i = 0; i < 59; i++)
    {
        //check if filename has a slash
        if (header.filename[i] == ' ')
        {
            holdStr = holdStr + header.filename[i];
        }
        else
        {
            filenameStr = filenameStr + holdStr + header.filename[i];
            holdStr = "";
        }
    }
    //check if file exists, handle it if it does
    ofstream myFile(filenameStr);
    myFile.write(reinterpret_cast<char*>(rec_buff.get()), header.nbytes);
    myFile.close();
    header.nbytes = 0;
    send_packet(client_sock, &header, sizeof header);
}

void reply_rm(accepted_socket& client_sock, cxi_header& header) {
    //ALWAYS ACK NOW ADD IN ERRORS LATER
    header.command = cxi_command::ACK;
    string filenameStr;
    string holdStr;
    for (int i = 0; i < 59; i++)
    {
        //check if filename has a slash
        if (header.filename[i] == ' ')
        {
            holdStr = holdStr + header.filename[i];
        }
        else
        {
            filenameStr = filenameStr + holdStr + header.filename[i];
            holdStr = "";
        }
    }
    //check if file exists, handle it if it does
    if (remove(header.filename) != 0)
    {
        perror("File deletion failed"); //???
        header.command = cxi_command::NAK;
        send_packet(client_sock, &header, sizeof header);
    }
    else
    {
        send_packet(client_sock, &header, sizeof header);
    }
}

void reply_get(accepted_socket& client_sock, cxi_header& header) {
    string filenameStr;
    string holdStr;
    for (int i = 0; i < 59; i++)
    {
        //check if filename has a slash
        if (header.filename[i] == ' ')
        {
            holdStr = holdStr + header.filename[i];
        }
        else
        {
            filenameStr = filenameStr + holdStr + header.filename[i];
            holdStr = "";
        }
    }
    //check if file exists, handle it if it does
    outlog << "FILENAME: " << filenameStr << endl;
    std::ifstream pFile(filenameStr.c_str(), std::ifstream::in);
    if (pFile.fail()) 
    {
        header.command = cxi_command::NAK;
        send_packet(client_sock, &header, sizeof header);
        return;
    }

    pFile.seekg(0, ios::end);
    uint32_t file_size = pFile.tellg();
    uint32_t nbytes = file_size;
    header.nbytes = nbytes;
    std::string temp;
    pFile.seekg(0, ios::beg);
    std::string line;
    while (std::getline(pFile, line))
    {
        temp += line + "\n";
    }
    outlog << "TEMP: " << temp << endl;
    const char* data = temp.c_str();
    outlog << "DATA: " << data << endl;
    send_packet(client_sock, &header, sizeof header);
    send_packet(client_sock, data, header.nbytes);
}

void reply_ls (accepted_socket& client_sock, cxi_header& header) {
   static const char ls_cmd[] = "ls -l 2>&1";
   FILE* ls_pipe = popen (ls_cmd, "r");
   if (ls_pipe == nullptr) { 
      outlog << ls_cmd << ": " << strerror (errno) << endl;
      header.command = cxi_command::NAK;
      header.nbytes = htonl (errno);
      send_packet (client_sock, &header, sizeof header);
      return;
   }
   string ls_output;
   char buffer[0x1000];
   for (;;) {
      char* rc = fgets (buffer, sizeof buffer, ls_pipe);
      if (rc == nullptr) break;
      ls_output.append (buffer);
   }
   pclose (ls_pipe);
   header.command = cxi_command::LSOUT;
   header.nbytes = htonl (ls_output.size());
   memset (header.filename, 0, FILENAME_SIZE);
   DEBUGF ('h', "sending header " << header);
   send_packet (client_sock, &header, sizeof header);
   send_packet (client_sock, ls_output.c_str(), ls_output.size());
   DEBUGF ('h', "sent " << ls_output.size() << " bytes");
}


void run_server (accepted_socket& client_sock) {
   outlog.execname (outlog.execname() + "*");
   outlog << "connected to " << to_string (client_sock) << endl;
   try {   
      for (;;) {
         cxi_header header; 
         recv_packet (client_sock, &header, sizeof header);
         DEBUGF ('h', "received header " << header);
         switch (header.command) {
            case cxi_command::LS: 
               reply_ls (client_sock, header);
               break;
            case cxi_command::PUT: {
                reply_put(client_sock, header);
                break;
            }
            case cxi_command::RM: {
                reply_rm(client_sock, header);
                break;
            }
            case cxi_command::GET: {
                reply_get(client_sock, header);
                break;
            }
            default:
               outlog << "invalid client header:" << header << endl;
               outlog << "catch" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cxi_exit& error) {
      DEBUGF ('x', "caught cxi_exit");
   }
   throw cxi_exit();
}

void fork_cxiserver (server_socket& server, accepted_socket& accept) {
   pid_t pid = fork();
   if (pid == 0) { // child
      server.close();
      run_server (accept);
      throw cxi_exit();
   }else {
      accept.close();
      if (pid < 0) {
         outlog << "fork failed: " << strerror (errno) << endl;
      }else {
         outlog << "forked cxiserver pid " << pid << endl;
      }
   }
}


void reap_zombies() {
   for (;;) {
      int status;
      pid_t child = waitpid (-1, &status, WNOHANG);
      if (child <= 0) break;
      if (status != 0) {
         outlog << "child " << child
                << " exit " << (status >> 8)
                << " signal " << (status & 0x7F)
                << " core " << (status >> 7 & 1) << endl;
      }
   }
}

void signal_handler (int signal) {
   DEBUGF ('s', "signal_handler: caught " << strsignal (signal));
   reap_zombies();
}

void signal_action (int signal, void (*handler) (int)) {
   struct sigaction action;
   action.sa_handler = handler;
   sigfillset (&action.sa_mask);
   action.sa_flags = 0;
   int rc = sigaction (signal, &action, nullptr);
   if (rc < 0) outlog << "sigaction " << strsignal (signal)
                      << " failed: " << strerror (errno) << endl;
}



void usage() {
   cerr << "Usage: " << outlog.execname() << " port" << endl;
   throw cxi_exit();
}

in_port_t scan_options (int argc, char** argv) {
   for (;;) {
      int opt = getopt (argc, argv, "@:");
      if (opt == EOF) break;
      switch (opt) {
         case '@': debugflags::setflags (optarg);
                   break;
      }
   }
   if (argc - optind != 1) usage();
   return get_cxi_server_port (argv[optind]);
}

int main (int argc, char** argv) {
   outlog.execname (basename (argv[0]));
   signal_action (SIGCHLD, signal_handler);
   try {
      in_port_t port = scan_options (argc, argv);
      server_socket listener (port);
      for (;;) {
         outlog << to_string (hostinfo()) << " accepting port "
             << to_string (port) << endl;
         accepted_socket client_sock;
         for (;;) {
            try {
               listener.accept (client_sock);
               break;
            }catch (socket_sys_error& error) {
               switch (error.sys_errno) {
                  case EINTR:
                     outlog << "listener.accept caught "
                         << strerror (EINTR) << endl;
                     break;
                  default:
                     throw;
               }
            }
         }
         outlog << "accepted " << to_string (client_sock) << endl;
         try {
            fork_cxiserver (listener, client_sock);
            reap_zombies();
         }catch (socket_error& error) {
            outlog << error.what() << endl;
         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cxi_exit& error) {
      DEBUGF ('x', "caught cxi_exit");
   }
   return 0;
}

