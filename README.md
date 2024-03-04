# webserv

`webserv` est un serveur HTTP 1.1 simple écrit en C++. Il est conçu pour être léger, performant, et facile à configurer tout en mettant l'accent sur les standards HTTP.

## Fonctionnalités

- Prise en charge complète du protocole HTTP/1.1. 
- Support des méthodes GET, POST, et DELETE.
- Configuration simple via un fichier de configuration
- Logging détaillé des requêtes et des réponses.
- Architecture multithread pour une meilleure performance.

## Commencer

### Prérequis

- Suivre le standard HTTP rfc 9112: [lien](https://datatracker.ietf.org/doc/html/rfc9112)
- Compilateur C++ supportant le standard C++98.
- Make ou un système de build similaire.
- (Optionnel) Clang-format pour formater le code selon les conventions du projet.

### Installation

```bash
git clone https://github.com/<username>/webserv.git
cd webserv
make
```

### Usage

```bash
./webserv config
```
Remplacez `config` par le chemin de votre fichier de configuration.

## Contribution
Nous encourageons la contribution à notre projet. Si vous souhaitez contribuer, veuillez suivre les directives suivantes :

## Code de Conduite
- **Commits**: Commencez vos messages de commit par l'un des mots-clés suivants : `fix`, `feat`. Ceci aide à comprendre rapidement le but du commit.

    Exemples :

    - `fix: corriger fuite de mémoire dans parser HTTP`
    - `feat: ajouter support des requêtes POST`
- **Standard C++**: Utilisez le standard C++98 pour garantir la compatibilité.
- **Formatage du code**: Formatez le code en utilisant le fichier .clang-format fourni dans le projet.

## Pull Requests
- Assurez-vous que votre code adhère aux standards établis.

