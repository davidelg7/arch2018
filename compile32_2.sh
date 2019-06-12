clear
cd /home/starquake/Desktop/arch2018/32/nasm
nasm -f elf32 distanza32.nasm -o ../compiledTmp/distanza32.o
nasm -f elf32 pqnn32.nasm -o ../compiledTmp/pqnn32.o
nasm -f elf32 sseutils32.nasm -o ../compiledTmp/sseutils32.o
cd ../c

gcc -c  -m32 k_means.c -o ../compiledTmp/k_means.o
gcc -c  -m32 noExaustive.c -o ../compiledTmp/noExaustive.o
gcc -c  -m32 pqnn64c.c -o ../compiledTmp/pqnn64c.o
gcc -c  -m32 search.c -o ../compiledTmp/search.o
gcc -c  -m32 writeDataset.c -o ../compiledTmp/writeDataset.o
cd ../compiledTmp
rm -f ../compiled/*

gcc -m32 -o  pqnn32 $(ls) -lm
mv * ../compiled
