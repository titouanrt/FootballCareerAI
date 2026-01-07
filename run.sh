#!/bin/bash
cd "$(dirname "$0")"

# Si le dossier build n'existe pas, on le configure
if [ ! -d "build" ]; then
    echo "Configuration du projet..."
    cmake -S . -B build
fi

# Lance la compilation
echo "Compilation en cours..."
cmake --build build -- -j8

# Si la compilation réussit, lance le jeu
if [ $? -eq 0 ]; then
    echo "Lancement du jeu..."
    ./build/FootballManagerGame
else
    echo "Erreur lors de la compilation."
fi
