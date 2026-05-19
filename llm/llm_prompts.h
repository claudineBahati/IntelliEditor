#ifndef PROMPTS_H
#define PROMPTS_H

// Template pour la correction orthographique et grammaticale
#define PROMPT_CORRECTION_FORMAT \
    "Tu es un correcteur expert. Corrige ce texte : \"%s\". " \
    "Réponds uniquement avec le texte corrigé."

// Template pour la reformulation professionnelle
#define PROMPT_REFORMULATE_FORMAT \
    "Reformule ce texte de manière plus formelle et professionnelle : \"%s\". " \
    "Réponds uniquement avec la reformulation."

#endif