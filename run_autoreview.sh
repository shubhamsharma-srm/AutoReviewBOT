#!/bin/bash

CHANGED_FILES=$(git diff --name-only origin/main...HEAD | grep -E '\.cpp$')

echo "Changed .cpp files:"
echo "$CHANGED_FILES"

for file in $CHANGED_FILES; do
    if [ -f "$file" ]; then
        echo "Analyzing $file..."
        ./build/autocheckcpp "$file"
    fi
done