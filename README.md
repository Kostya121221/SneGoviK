# SneGoviK
Проект для расчётно-графической работы по дисциплине:"Программирование"

Сборка библиотеки
gcc -shared -fPIC ciphers/{name}.cpp -o lib/lib{name}W.so

Сборка исполняемого файла
g++ main.cpp scripts/*.cpp -L./lib -ldes -lelgamal -lrc4 -lrc5 -lrsa -lshamir -o main -Wl,-rpath,'./lib