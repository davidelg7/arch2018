cd $(dirname "$0")
clear

for distance in -adc -sdc
do
  for k in 256 512 1024
  do
    for m in 8 16 32
    do
      for knn in 1 2 4 8
      do
        echo -k $k -knn $knn -m $m $distance
        ./compiled/pqnn64 ./ds/prova -s 1 -k $k -knn $knn -m $m $distance
      done
    done

  done

done

for k in 256 512 1024
do
  for m in 8 16 32
  do
    for knn in 1 2 4 8
    do
      echo -k $k -knn $knn -m $m $distance
      ./compiled/pqnn64 ./ds/prova -s 1 -noexaustive -k $k -knn $knn -m $m
    done
  done

done
