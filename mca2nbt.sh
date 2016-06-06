#!/bin/bash
cd nbt
test=$(ls region/*mca)
for filename in $test; do mkdir ${filename%.mca}; done
echo "Converting .mca files to .nbt files"
./mca2nbt.exe region/*.mca
echo "Finished conversion"