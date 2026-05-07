#ifndef TOKENIZER_H
#define TOKENIZER_H

// On définit une structure pour transporter nos mots découpés
typedef struct {
    char** words;  // Tableau de chaînes de caractères
    int count;     // Nombre de mots trouvés
} TokenizedText;

// Fonction pour découper le texte en mots
TokenizedText nlp_tokenize(const char* input_text);

// Fonction pour libérer la mémoire après usage
void nlp_free_tokens(TokenizedText* tokens);

#endif