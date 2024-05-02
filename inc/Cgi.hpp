#pragma once

#include "Request.hpp"
#include <map>
#include <string>

class Cgi
{
  private:
    static std::map<std::string, std::string> generate_meta_variables(const
		Request& request);
    static char** map_to_env(std::map<std::string, std::string>& meta_var);
    static int fork_and_exec(std::map<std::string, std::string>& meta_var,
		int* fd, int& pid, std::string path_to_root, std::string
		path_to_exec_prog, std::string url);
    static std::string get_cgi_output(int* fd);

  public:
    Cgi();
    Cgi(const Cgi& other);
    Cgi& operator=(const Cgi& other);
    ~Cgi();

    static int handle_cgi(const Request& request, std::string &rep, std::string 
		url, std::string path_to_root, std::string path_to_exec_prog);
};
