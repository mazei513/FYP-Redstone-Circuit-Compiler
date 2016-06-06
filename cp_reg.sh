#!/bin/bash
cwd=$(pwd)
cd "$APPDATA"
cd .minecraft/saves
ls
echo "Choose a save folder"
read folder_name
echo "You chose : $folder_name"
cd $folder_name/region
cp -r $(pwd) $cwd/nbt