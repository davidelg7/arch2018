cd $(dirname "$0")
clear

for distance in -adc -sdc
do
  for nr in 400 4000
  do
  for kc in 256  2048
   do
  for k in 256 512
  do
    for m in 8 16
    do
      for knn in 1 2 4
      do
        echo -noexaustive -s 1 $distance -nr $nr -kc $kc -k $k -m $m -knn $knn
        ./compiled/pqnn64 ./ds/prova -noexaustive -s 1 $distance -nr $nr -kc $kc -k $k -m $m -knn $knn  | tee test/"-noexaustive $distance -nr $nr -kc $kc -k $k -m $m -knn $knn.txt"
      done
    done
  done
  done
  done
  for k in 256 512
  do
    for m in 8 16
    do
      for knn in 1 2 4
      do
        echo -exaustive -k $k -knn $knn -m $m $distance
        ./compiled/pqnn64 ./ds/prova -s 1 -exaustive $distance -k $k -m $m -knn $knn | tee test/"-exaustive $distance -k $k -m $m -knn $knn.txt"
      done
    done

  done
done
