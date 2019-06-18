from os import listdir
from os import sys
import os
import json
path=os.path.dirname(os.path.abspath(sys.argv[0]))
onlyfiles =[]
infos={}

for x in listdir(path+"/test"):
    if x.endswith(".txt"):
        onlyfiles.append(x)
infos["files"]=onlyfiles
lessThan=list(range(1,10,1))+list(range(10,800,4))
for file in onlyfiles:
    info={}
    mean = 0;
    found=0
    nMeasures=0
    less=[]
    knn=int(file[file.index("-knn")+5:file.index("-knn")+6])
    lines = [x.replace("\t"," ") for x in open(path+"/test/"+file).read().splitlines()]
    rt=lines[len(lines)-1-2].split()[1]
    rtv=float(rt.split("m")[0])*60+float(rt.split("m")[1][:len(rt.split("m")[1])-2].replace(",","."))
    cpu=lines[len(lines)-1-1].split()[1]
    cpuv=float(cpu.split("m")[0])*60
    cpuv+=float(cpu.split("m")[1][:len(cpu.split("m")[1])-2].replace(",","."))
    
    info["Real Time"]=str(rtv)[0:5]
    info["CPU Time"]=str(cpuv)[0:5]
    info["SpeedUp"]=cpuv/rtv
    info["Indexing Time"]=str(float(lines[0])/(cpuv/rtv))
    info["Searching Time"]=str(float(lines[1])/(cpuv/rtv))
    info["Total Time"]=float(lines[0])/(cpuv/rtv)+float(lines[1])/(cpuv/rtv)
    lines=lines[2:len(lines)-3]
    le={}
    for l in lessThan:
        le[l]=0
    for line in lines:
        if 0 in [float(y) for y in line.split()]:
          found+=1
        else:
            for x  in [abs(float(y)) for y in line.split()]:
                mean+=x
        for word in line.split():
            error = abs(float(word))
            for e in lessThan:
                if error<e:
                        le[e]+=1
                        break
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
