#pragma once

#include <string>
#include <map>

class Cgi
{
  private:
    std::map<std::string, std::string> generate_meta_variables(std::string const &request);
    char **map_to_env(std::map<std::string, std::string> &meta_var);
    int fork_and_exec(std::map<std::string, std::string> &metaVar, int *fd, int pid);
    std::string get_cgi_output(int *fd);

  public:
    Cgi();
    Cgi(const Cgi &other);
    Cgi &operator=(const Cgi &other);
    ~Cgi();

    int handle_cgi(std::string &request, int sock);
    bool is_cgi_request(std::string &request);
};
