FOR %%i IN (*.nbt) DO python nbt2yaml -n %%i > %%i%.txt

DIR /b *.txt > chunk.lst