#!/bin/bash

# Leer entrada estándar
read input

# Extraer los números (pueden ser negativos)
num1=$(echo "$input" | sed -E 's/.*=([-+]?[0-9]+)&.*/\1/')
num2=$(echo "$input" | sed -E 's/.*&.*=([-+]?[0-9]+)/\1/')

# Calcular la resta
result=$((num1 - num2))

# Mostrar el resultado
echo "La resta es: $result"
