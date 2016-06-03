cd ..
mkdir chunks
cd nbt
FOR /D %%G IN ("r.*") DO (
	mkdir %%G
	FOR %%i IN ("%%G/*.nbt") DO python nbt2yaml -n %%G/%%i > ../chunks/%%G.%%i
)
DIR "../chunks/"*.nbt /b > ../chunk.lst