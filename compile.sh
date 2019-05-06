cd /home/"$USER"/Google/Architettura/compiledTmp
rm -f *
cd ../nasm

for f in *.nasm; do
  filename=$(basename -- "$f")
  extension="${filename##*.}"
  filename="${filename%.*}"
  nasm -felf64 -g -Fdwarf "$f" -o "$filename".o
done

mv *o ../compiledTmp
cd ../c
gcc -c $(ls )
mv *.o ../compiledTmp

cd ../compiledTmp

gcc -Wall -o pqnn64 $(ls)

rm -f ../compiled/*
mv * ../compiled
cd ../compiled
./pqnn64 ../ds/prova
