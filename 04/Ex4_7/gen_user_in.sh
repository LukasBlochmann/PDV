#!/bin/bash


file_path="/home/ubuntu/STTS-Entwicklungen/user_in.txt"

# random letter:
generate_random_letter() {
    letters="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
    echo -n "${letters:RANDOM%${#letters}:1}"
}

# alle 5 Minuten
while true; do

    random_letter=$(generate_random_letter)

    # in datei schreiben und dev-fortschritt simulieren
    echo -n "$random_letter" >> "$file_path"

    # 5 minuten
    sleep 300
done
