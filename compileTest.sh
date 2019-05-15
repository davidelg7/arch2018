cd $(dirname "$0")/compiledTmp
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

gcc -Wall -o pqnn64 $(ls) -lm

rm -f ../compiled/*
mv * ../compiled
cd ../compiled
./pqnn64 ../ds/Test -k 2 -m 8
