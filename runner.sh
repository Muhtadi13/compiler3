#!/usr/bin/env bash
yacc --yacc -Wcounterexample -d 2005013.y -o y.tab.cpp
flex -o 2005013.cpp 2005013.l
g++ -w y.tab.cpp 2005013.cpp
rm -f 2005013.cpp y.tab.cpp y.tab.hpp
./a.out input.txt
