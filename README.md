<<<<<<< HEAD
# IntelliEditor — Module Moteur de Règles (DEV-D)

Projet C Avancé — L3 GL / UDBL 25-26  
Développeur : **DEV-D** — Moteur de règles & Intégration générale
=======
# IntelliEditor
# Description du projet
>>>>>>> 2b4f0c2296b45bbc0e0a7765efb831705e41095f

---

## Membres de l'équipe

| Rôle  | Responsabilité                                    |
|-------|---------------------------------------------------|
| DEV-A | Infrastructure & Éditeur de texte (gap buffer)    |
| DEV-B | Interface utilisateur Win32 + Scintilla           |
| DEV-C | LLM & Moteur NLP (Hunspell + llama.cpp)           |
| DEV-D | Moteur de règles & Intégration générale           |

<<<<<<< HEAD
---

## Fonctionnalités implémentées (DEV-D)

### Structures de données
- `Rule` — représente une règle issue du fichier JSON
- `RuleSet` — ensemble de règles chargées (jusqu'à 128)
- `RuleResult` — résultat de l'évaluation d'une règle
- `RuleReport` — rapport complet avec compteurs de synthèse

### Parser JSON (`rule_parser`)
- Lecture de fichiers JSON de règles sans dépendance externe
- Support des valeurs scalaires, tableaux (`[]`) et objets (`{}`) pour le champ `parameter`
- Tolérance aux variations de formatage JSON (espaces, guillemets)

### Moteur d'évaluation (`rule_engine`)
- `evaluate_rules()` — évalue l'ensemble des règles sur un document
- `update_rule_result()` — mise à jour asynchrone pour les résultats LLM (DEV-C)

### Vérificateurs (`checkers/`)

| Fichier               | Types supportés                                  |
|-----------------------|--------------------------------------------------|
| `section_checker`     | `section_exists`, `section_order`                |
| `count_checker`       | `word_count_min`, `word_count_max`               |
| `regex_checker`       | `regex_forbidden`, `regex_required` + alternance `\|` |
| `heading_checker`     | `heading_format` (uppercase / lowercase / capitalize) |
| `citation_checker`    | `citation_present` (alternatives séparées par `\|`) |

Les règles `llm_semantic` sont marquées `RULE_PENDING` ; le module LLM
(DEV-C) les met à jour via `update_rule_result()`.

### Rapport de conformité (`rule_report`)
- Affichage formaté avec statuts `OK / AVERT. / ERREUR / ATTENTE`
- Pourcentage de conformité calculé automatiquement

### Gabarit de règles (`data/rule_templates/`)
- `memory_license.json` — 13 règles complètes pour les mémoires de Licence UDBL

### Tests unitaires (`tests/test_rules.c`)
- 36 tests couvrant tous les modules (framework cmocka)

---

## Structure du projet (module DEV-D)

```
intelli/
├── CMakeLists.txt
├── main.c                          ← point d'entrée de démonstration
├── data/
│   └── rule_templates/
│       └── memory_license.json    ← gabarit de règles UDBL
├── src/
│   └── rules/
│       ├── rule_engine.h / .c     ← évaluateur central
│       ├── rule_parser.h / .c     ← lecteur JSON
│       ├── rule_report.h / .c     ← affichage du rapport
│       └── checkers/
│           ├── section_checker.h / .c
│           ├── count_checker.h / .c
│           ├── regex_checker.h / .c
│           ├── heading_checker.h / .c
│           └── citation_checker.h / .c
└── tests/
    └── test_rules.c               ← tests unitaires (cmocka)
```

---

## Compilation

### Prérequis
- MSYS2 + MinGW-w64 (Windows) ou GCC ≥ 9 (Linux)
- CMake ≥ 3.16

### Compilation standard

```bash
mkdir cmake-build-debug && cd cmake-build-debug
cmake .. -G "MinGW Makefiles"   # Windows
# cmake ..                      # Linux
cmake --build .
./intelli.exe                   # Lancement avec document de démo
./intelli.exe ../data/rule_templates/memory_license.json
./intelli.exe ../data/rule_templates/memory_license.json mon_document.txt
```

### Avec les tests unitaires (requiert cmocka)

```bash
cmake .. -DBUILD_TESTS=ON
cmake --build .
ctest --output-on-failure
```

---

## Format du fichier de règles

```json
{
  "meta": { "document_type": "Mémoire de Licence", "version": "1.0" },
  "rules": [
    {
      "id": "R001",
      "category": "structure",
      "severity": "error",
      "description": "Le document doit contenir une section Introduction",
      "check_type": "section_exists",
      "parameter": "Introduction"
    }
  ]
}
```

### Types de `check_type` supportés

| check_type        | Description                                       |
|-------------------|---------------------------------------------------|
| `section_exists`  | Présence d'une section (insensible à la casse)    |
| `section_order`   | Ordre des sections (tableau JSON ou liste CSV)    |
| `word_count_min`  | Nombre minimum de mots                            |
| `word_count_max`  | Nombre maximum de mots                            |
| `regex_forbidden` | Expression interdite (alternatives avec `\|`)     |
| `regex_required`  | Expression obligatoire                            |
| `heading_format`  | Format des titres H1–H4                           |
| `citation_present`| Présence d'une section bibliographique            |
| `llm_semantic`    | Vérification sémantique LLM (asynchrone)          |

---

## Intégration avec les autres modules

- **DEV-B (UI)** — appelle `evaluate_rules()` après 2 secondes d'inactivité
  et lit `RuleReport` pour mettre à jour le panneau latéral.
- **DEV-C (LLM)** — appelle `update_rule_result()` lorsque la réponse LLM
  est disponible pour les règles `llm_semantic`.
- **DEV-A (Éditeur)** — fournit le texte du document à `evaluate_rules()`.
=======
>>>>>>> 2b4f0c2296b45bbc0e0a7765efb831705e41095f
