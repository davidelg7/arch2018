cd $(dirname "$0")
clear

for distance in -adc -sdc
do
  for nr in 400 800
  do
  for kc in 512 800
   do
  for k in 256 512
  do
    for m in 8 16
    do
      for knn in 1 2 4
      do
        echo -noexaustive $distance -k $k -knn $knn -m $m -kc $kc -nr $nr
        ./compiled/pqnn64 ./ds/prova -s 1 -noexaustive $distance -k $k -knn $knn -m $m -kc $kc -nr $nr
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
        echo -k $k -knn $knn -m $m $distance
        ./compiled/pqnn64 ./ds/prova -s 1 -k $k -knn $knn -m $m $distance
      done
    done

  done
done
