// Response.cpp
#include "Response.hpp"

void Response::_get()
{
	// Déterminer le chemin complet en fonction de la route et du serveur
	std::string full_path = _server.root + _url;

	// Vérifier si le chemin correspond à une route spécifique
	for (size_t i = 0; i < _server.routes.size(); ++i)
	{
		if (_request.get_url() == _server.routes[i].path) // Si une route est trouvée, utiliser les paramètres de la route
		{
			_route = &_server.routes[i];
			full_path = _route->root.empty() ? _server.root : _route->root;
			full_path += _route->file_path.empty() ? _url : _route->file_path;
			break;
		}
	}

	// Vérifier si le chemin est un dossier ou un fichier
	int dir_or_file = _is_directory_or_file(full_path);
	if (dir_or_file == 1) // Si c'est un dossier, chercher le fichier par défaut
	{
		full_path += _route && !_route->default_file.empty() ? _route->default_file : _server.default_file;
		dir_or_file = _is_directory_or_file(full_path);
	}

	if (dir_or_file == 0)				// Si c'est un fichier, générer la réponse avec le contenu du fichier
		_generate_response(full_path);
	else if (dir_or_file == -1)	// Si une erreur est survenue, générer une réponse d'erreur 404
		_generate_response_body(404);
	else 												// Si le chemin n'est ni un fichier ni un dossier, générer une réponse d'erreur 400
		_generate_response_body(400);
}
