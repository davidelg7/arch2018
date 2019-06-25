cd $(dirname "$0")
for f in *.nasm; do
  filename=$(basename -- "$f")
  extension="${filename##*.}"
  filename="${filename%.*}"
  nasm -f elf32 "$f" -o "$filename".o
done

for f in *.c; do
  filename=$(basename -- "$f")
  extension="${filename##*.}"
  filename="${filename%.*}"
  gcc -c  -m32 "$filename".c -o "$filename".o
done
gcc -m32 -o  pqnn32c_18 $(ls *.o) -lm
./pqnn32c_18 $1 $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13} ${14} ${15} ${16} ${17} ${18} ${19} ${20} ${21} ${22} ${23} ${24} ${25} ${26} ${27} ${28} ${29} ${30}
