FOR /D %%G IN ("r.*") DO (
	FOR %%i IN ("%%G/*.nbt") DO python nbt2yaml -n %%G/%%i > ../chunks/%%G.%%i
)
DIR "../chunks/"*.nbt /b > ../chunk.lst