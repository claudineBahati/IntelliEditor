# 📝 IntelliEditor

**IntelliEditor** est un éditeur de texte professionnel et intelligent développé en C pour l'environnement Windows (Win32 API). Inspiré de Microsoft Word, il propose un affichage de page physique centré, un correcteur d'orthographe temps réel adapté au français, un gestionnaire d'onglets multiples, et un moteur de règles métiers combiné à une interface d'IA.

Ce projet a été réalisé en équipe dans un cadre académique.

---

## 🚀 Fonctionnalités Clés

* **Affichage de Page Réelle (Look & Feel Word)** : Rendu d'une page physique centrée avec des marges de page personnalisables (Étroites, Normales, Larges) et synchronisation des thèmes.
* **Barre de Menus & Raccourcis Word-like** : Organisation des menus (Fichier, Édition, Insertion, Mise en page, Révision & IA, Affichage) avec tous les raccourcis clavier standards (Ctrl+N, Ctrl+S, Ctrl+Z, etc.).
* **Correcteur Orthographique Temps Réel** : Dictionnaire local français avec filtrage intelligent des élisions et contractions (`l'`, `d'`, `qu'`, etc.) pour éliminer les faux positifs.
* **Dialogue des Paramètres Interactif** : Configuration à la volée de la police, de sa taille, des marges et du Mode Sombre avec rendu dynamique en temps réel.
* **Gestion Avancée des Onglets** : Navigation fluide entre documents ouverts et fermeture ciblée par clic droit sur l'onglet concerné.
* **Moteur de Règles & IA** : Chargement dynamique de règles de conformité stylistique (JSON) et intégration de fonctions d'analyse textuelle (statistiques, résumé automatique).

---

## 🛠️ Organisation de l'Équipe de Développement

Le projet est le résultat d'une intégration fluide des modules suivants :

1. **Dev-A (Divine Fatuma)** : Fondations techniques de l'application et structure de base de l'éditeur de texte.
2. **Dev-B (Lumière Tshisabi)** : Interface utilisateur Win32, wrapper Scintilla, mise en page physique, correcteur d'orthographe français, boîte de dialogue des paramètres et gestion contextuelle des onglets.
3. **Dev-C (Déborah Kabedi)** : Module de recherche/remplacement et moteur d'intelligence artificielle (NLP).
4. **Dev-D (Claudine Bahati)** : Moteur de règles métiers (JSON) et coordination de l'intégration générale.

---

## 📦 Compilation et Exécution

### Prérequis
* Un environnement Windows.
* Compilateur **MinGW-w64** (GCC) installé et ajouté au PATH système.

### Compilation
Un script de build automatique est fourni à la racine. Exécutez simplement la commande suivante dans PowerShell :
```powershell
.\build.bat
```

### Exécution
Lancez l'exécutable généré :
```powershell
.\IntelliEditor.exe
```
