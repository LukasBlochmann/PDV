#!/bin/bash

# Get the current day of the week (0 = Sunday, 1 = Monday, ..., 6 = Saturday)
day=$(date +%u)

# Check if it's not Saturday (6) or Sunday (7)
if [ "$day" -ne 6 ] && [ "$day" -ne 7 ]; then
    # Create a TAR archive of STTS-Entwicklungen directory
    tar -czf ~/Sicherungen/Sicherung_$(date +"%Y-%m-%d_%H-%M-%S").tar.gz ~/STTS-Entwicklungen
fi
