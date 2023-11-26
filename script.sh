#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Gresit! Utilizare: $0 <caracter>"
    exit 1
fi

caracter="$1"
count_lines=0

while IFS= read -r line;
    do
    # verificare daca o linie incepe cu o litera mare
    if [[ $line =~ ^[A-Z] ]]
        then
        # verificare propozitii corecte
        if [[ $line =~ ^[A-Za-z0-9\ \,\.\!\?]+[A-Za-z0-9\ \.\!\?]+[\.!\?]$ && ! $line =~ \,.*\si && $line =~ $caracter ]];
        then
            ((count_lines++))
        fi
    fi
done
echo $count_lines
