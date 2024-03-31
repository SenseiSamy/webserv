#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>

std::map<std::vector<unsigned short>, std::string> parseErrorLine(const std::string& line) {
    std::map<std::vector<unsigned short>, std::string> errorMap;
    std::istringstream stream(line);
    std::string token;

    while (getline(stream, token, ',')) {
        std::istringstream tokenStream(token);
        std::vector<unsigned short> codes;
        std::string code;
        std::string page;

        // Lire les codes d'erreur
        while (tokenStream >> code) {
            if (isdigit(code[0])) { // Vérifie si c'est un code d'erreur ou une URL
                codes.push_back(static_cast<unsigned short>(atoi(code.c_str())));
            } else {
                page = code; // La première chaîne non-numérique est l'URL
                break;
            }
        }

        // Extraire le reste de l'URL si elle contient des espaces
        std::getline(tokenStream, code);
        page += code;

        if (!codes.empty() && !page.empty()) {
            errorMap[codes] = page;
        }
    }

    return errorMap;
}

int main() {
    std::string line = "error_page: 404 /404.html /778, 500 448 1 1123 456 /500.html";
    // Suppression du préfixe "error_page: " pour simplifier l'analyse
    std::string errors = line.substr(11);

    std::map<std::vector<unsigned short>, std::string> errorMap = parseErrorLine(errors);

    // Afficher le résultat pour vérification
    for (std::map<std::vector<unsigned short>, std::string>::iterator it = errorMap.begin(); it != errorMap.end(); ++it) {
        std::cout << "Codes: ";
        for (size_t i = 0; i < it->first.size(); ++i) {
            std::cout << it->first[i] << " ";
        }
        std::cout << "-> Page: " << it->second << std::endl;
    }

    return 0;
}
