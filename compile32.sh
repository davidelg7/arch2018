cd $(dirname "$0")/compiledTmp
rm -f *
cd ../nasm

for f in *32.nasm; do
  filename=$(basename -- "$f")
  extension="${filename##*.}"
  filename="${filename%.*}"
  nasm -felf32 -g -Fdwarf "$f" -o "$filename".o
done

mv *o ../compiledTmp
cd ../c
gcc -c  $(ls )  -fopenmp
mv *.o ../compiledTmp

cd ../compiledTmp

gcc -Wall -fopenmp -o pqnn32 $(ls) -lm

rm -f ../compiled/*
mv * ../compiled
cd ../compiled
time ./pqnn32 ../ds/prova
