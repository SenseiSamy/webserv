# webserv

`webserv` est un serveur HTTP 1.1 simple écrit en C++.

## Fonctionnalités

- Prise en charge complète du protocole HTTP/1.1.
- Support des méthodes GET, POST, et DELETE.
- Configuration simple via un fichier de configuration.
- Logging détaillé des requêtes et des réponses.
- Architecture multithread pour une meilleure performance.

### Prérequis

- Respect des standards HTTP, spécifiquement le RFC 9112 : [RFC 9112](https://datatracker.ietf.org/doc/html/rfc9112)
- Un compilateur C++ conforme au standard C++98. Les compilateurs recommandés incluent GCC et Clang.
- Un système de build tel que GNU Make. [Instructions d'installation de GNU Make](https://www.gnu.org/software/make/).
- (Optionnel) Clang-format pour le formatage du code selon les conventions du projet.

### Installation

```sh
git clone https://github.com/<username>/webserv.git
cd webserv
make
```

### Usage

Démarrer le serveur avec un fichier de configuration spécifique :

```sh
./webserv <config_file>
```
Remplacez `<config_file>` par le chemin de votre fichier de configuration.

#### Fichier de configuration

Respectez les règles suivantes pour assurer une interprétation correcte par le serveur :

**Format Général**
- Chaque instruction doit être placée à l'intérieur d'un bloc `server{...}`.
- Les blocs `server` peuvent contenir des sous-blocs `routes` pour des configurations spécifiques aux itinéraires.

**Syntaxe de Base**
- Les paires clé/valeur sont séparées par un signe égal `=` et les valeurs doivent être entre guillemets `"..."`.
- Chaque déclaration se termine par un point-virgule `;`.
- Les blocs sont délimités par des accolades `{...}`.

**Paramètres de Configuration**

##### Serveur

| Clé            | Description                                         |
|----------------|-----------------------------------------------------|
| `port`         | Le port sur lequel le serveur doit écouter.         |
| `host`         | L'adresse IP ou le nom d'hôte.                      |
| `server_names` | Les noms de domaine que le serveur accepte.         |
| `error_pages`  | Associe des codes d'erreur HTTP à des fichiers HTML.|
| `body_size`    | Taille maximale du corps des requêtes en octets.    |

##### Routes

| Clé             | Description                                        |
|-----------------|----------------------------------------------------|
| `methods`       | Méthodes HTTP autorisées pour les routes.          |
| `redirect`      | URL de redirection pour cette route.               |
| `root`          | Répertoire racine pour les fichiers de cette route.|
| `autoindex`     | Active ou désactive l'indexation automatique.      |
| `default_index` | Page par défaut pour cette route.                  |
| `cgi`           | Extensions et gestionnaire CGI pour les scripts.   |

**Commentaires**
- Les commentaires peuvent être ajoutés en utilisant le caractère dièse `#` et ils ne sont pas interprétés par le serveur.

### Contribution

Nous encourageons la contribution à notre projet. Si vous souhaitez contribuer, veuillez suivre les directives suivantes :

**Code de Conduite**
- **Commits**: Commencez vos messages de commit par l'un des mots-clés suivants : `fix`, `feat`. Ceci aide à comprendre rapidement le but du commit.
  Exemples :
  - `fix: corriger fuite de mémoire dans parser HTTP`
  - `feat: ajouter support des requêtes POST`
- **Standard C++**: Utilisez le standard C++98 pour garantir la compatibilité.
- **Formatage du code**: Formatez le code en utilisant le fichier .clang-format fourni dans le projet.

### Pull Requests

- Assurez-vous que votre code adhère aux standards établis.
