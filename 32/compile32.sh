cd $(dirname "$0")/compiledTmp
rm -f *
cd ../nasm
for f in *32.nasm; do
  filename=$(basename -- "$f")
  extension="${filename##*.}"
  filename="${filename%.*}"
  nasm -f elf32 "$f" -o ../compiledTmp/"$filename".o
done
cd ../c

for f in *.c; do
  filename=$(basename -- "$f")
  extension="${filename##*.}"
  filename="${filename%.*}"
  gcc -c  -m32 "$filename".c -o ../compiledTmp/"$filename".o
done

cd ../compiledTmp

gcc -m32 -o  pqnn32 $(ls) -lm

rm -f ../compiled/*

mv * ../compiled
cd ../compiled
clear
time ./pqnn32 ../ds/prova -d
