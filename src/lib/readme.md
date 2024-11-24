## Libreria SQLITE

> Para compilar el programa es necesario obtener el codigo fuente de la libreria sqlite:

```bash
sudo apt update && sudo apt upgrade
sudo apt install unzip
curl https://www.sqlite.org/2024/sqlite-amalgamation-3470000.zip -o src/lib/sqlite-amalgamation-3470000.zip
unzip src/lib/sqlite-amalgamation-3470000.zip -d src/lib
rm src/lib/sqlite-amalgamation-3470000.zip
mkdir -p src/lib/sqlite # Source for sqlite
mkdir -p src/include/sqlite # Headers for sqlite
mv src/lib/sqlite-amalgamation-3470000/sqlite3.c src/lib/sqlite
mv src/lib/sqlite-amalgamation-3470000/sqlite3.h src/include/sqlite
rm --force --recursive src/lib/sqlite-amalgamation-3470000
```
