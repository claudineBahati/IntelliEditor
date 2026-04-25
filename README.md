# IntelliEditor
 Ce projet vise à créer un éditeur de texte intelligent capable d'appliquer des règles dynamiques sur le contenu textuel à l'aide de fichiers de configuration JSON.

La branche actuelle dev-d est dédiée spécifiquement au développement et à la validation du Moteur de Règles (RuleEngine).

#  Structure du Projet

L'architecture suit les principes de la Clean Architecture pour séparer la logique métier de l'infrastructure technique :

src/ : Le module principal

data/rule_template/ : Contient les fichiers de configuration, notamment memoire.json.

tests/ : Contient les scripts de test, comme test_rules.c.

build/ : Répertoire dédié à la compilation (exclu de Git via .gitignore).

# Prérequis et Installation

Pour compiler ce projet sous Windows, assurez-vous d'utiliser l'environnement MSYS2 / MinGW64 avec les bibliothèques suivantes installées :

# Installation des dépendances via le gestionnaire de paquets Pacman
pacman -S mingw-w64-x86_64-cjson
pacman -S mingw-w64-x86_64-pcre2
pacman -S mingw-w64-x86_64-cmocka



