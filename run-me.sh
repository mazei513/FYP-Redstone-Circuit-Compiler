#!/bin/bash
./cp_reg.sh
./mca2nbt.sh
echo "Run test_par.bat"
echo "Press 'Enter' once test_par.bat finishes"
read
./ccompiler.exe
rm -rf chunks