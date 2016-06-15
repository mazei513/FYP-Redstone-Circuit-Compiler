#!/bin/bash
./cp_reg.sh
./mca2nbt.sh
echo "Run nbt2yaml.bat"
echo "Press 'Enter' once nbt2yaml.bat finishes"
read
./ccompiler.exe
rm -rf chunks
echo "Compilation of Redstone circuit complete"