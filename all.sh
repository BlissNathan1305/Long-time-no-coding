#!/bin/bash

# Method 1: Using a loop and character increment

for i in {a..z}; do
  echo -n "$i"
done
echo  # Add a newline at the end

# Method 2: Using `seq` and `printf`

# printf "%c" $(seq 97 122)
# echo

# Method 3: Using `echo` and `tr`

# echo {a..z} | tr -d ' '
# echo

# Method 4: Using `printf` and a range of numbers

# printf '%s' $(printf '\\%o' $(seq 141 172))
# echo
