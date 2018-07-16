
This repo contains an updated and modified version of a coursework project to make a C-89 to MIPs 1 compiler. The specification for this can be found at https://github.com/LangProc/langproc-2017-cw . This coursework was finished over the course of eight weeks, and was coded by myself and my partner, Kyzarok.

Readme from that project follows

This repo is for the Language Processors 2018 Compiler Project for Team KELP, consisting of Kai Lawrence and Edward Pickup

The compiler / translator is made by calling

"make bin/c_compiler"

They an then be run by the command

./bin/c_compiler $mode $sourcefile -o $destfile

Where $mode is either "-S" for compile, or "--translate" for translation, and $sourcefile & $destfile are paths to the two files.

Our compiler should work for everything we can translate, with the exception of functions with parameters.

