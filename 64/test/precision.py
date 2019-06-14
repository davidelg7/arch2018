from os import listdir
from os import sys
import os
import json
path=os.path.dirname(sys.argv[0])+"/"
onlyfiles =[]
infos={}

for x in listdir(path):
    if x.endswith(".txt"):
        onlyfiles.append(x)
infos["files"]=onlyfiles
lessThan=range(0,800,4)
for file in onlyfiles:
    info={}
    mean = 0;
    found=0
    nMeasures=0
    less=[]
    knn=int(file[file.index("-knn")+5:file.index("-knn")+6])
    lines = [x.replace("\t"," ") for x in open(path+file).read().splitlines()]
    info["Indexing Time"]=str(float(lines[0]))
    info["Searching Time"]=str(float(lines[1]))
    lines=lines[2:len(lines)-2]
    le={}
    for l in lessThan:
        le[l]=0
    for line in lines:
        for word in line.split():
            error = abs(float(word))
            if float(word)==0:
              found+=1
            for e in lessThan:
                if error<e:
                        le[e]+=1
                        break
            mean+=float(word)
            nMeasures+=1
    less={}
    for error in le:
        less[str(error)] =int(le[error] / knn)
    info["Found"]=found
    info["Precision"]=less
    info["Mean Error"]=abs(mean/nMeasures)
    info["knn"]=knn
    infos[file]=json.dumps(info)
with open(path+"runResult","w") as outfile:
    json.dump(infos, outfile)
