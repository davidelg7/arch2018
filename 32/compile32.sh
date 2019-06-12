cd $(dirname "$0")/compiledTmp
rm -f *
cd ../nasm
for f in *32.nasm; do
  filename=$(basename -- "$f")
  extension="${filename##*.}"
  filename="${filename%.*}"
  echo $filename
  nasm -f elf32 "$f" -o "$filename".o
  #nasm -f elf32 -g -Fdwarf
done
echo "nasm ok"
mv *o ../compiledTmp
cd ../c

for f in *.c; do
  filename=$(basename -- "$f")
  extension="${filename##*.}"
  filename="${filename%.*}"
  echo $filename
  gcc -c  -m32 "$filename".c
done
mv *.o ../compiledTmp

cd ../compiledTmp

gcc -m32 -o  $(ls)

rm -f ../compiled/*
mv * ../compiled
cd ../compiled

time ./pqnn32 ../ds/prova
