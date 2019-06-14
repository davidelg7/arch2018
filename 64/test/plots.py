import json
import os
from os import sys
import matplotlib.pyplot as plt



def makeMultiplePlots(name,c,files,data,n1,n2):
    n=len(files)
    i=1
    f=plt.figure(figsize=(n1,n2))
    for p in files:
        x=[]
        y=[]
        d=json.loads(data[p])
        precision=d["Precision"]
        for valx in precision:
            x.append(float(valx))
            y.append(float(precision[valx]))
        fig =plt.subplot(n/c+1, c, i)
        plt.plot(x, y, color='blue')
        plt.xlabel('Catalogati con Errore')
        plt.ylabel('Count')
        plt.title(str(i))
        knn= d["knn"]
        st=int(float(d["Searching Time"]))
        it=int(float(d["Indexing Time"]))
        err=int(float(d["Mean Error"]))
        legend="Searching: "+str(st)+" s\nIndexin: "+str(it)+"s \nError: "+str(err)
        fig.legend([legend])
        i+=1
        #plt.show()
    f.savefig(path+name+".pdf", bbox_inches='tight')


path=os.path.dirname(sys.argv[0])+"/"
with open(path+'runResult') as json_file:
    data = json.load(json_file)
    es=[]
    ea=[]
    nes=[]
    nea=[]
    for f in data["files"]:
        if "-sdc" in f and "-exaustive" in f:
            es.append(f)
        if "-sdc" in f and "-noexaustive" in f:
            nes.append(f)
        if "-adc" in f  and "-exaustive" in f :
            ea.append(f)
        if "-adc" in f and "-noexaustive" in f :
            nea.append(f)
    c=4
    makeMultiplePlots("Esaustiva Simmetrica",c,es,data,20,20)
    makeMultiplePlots("Esaustiva Asimmetrica",c,ea,data,20,20)
    makeMultiplePlots("Non Esaustiva Simmetrica",c,nes,data,40,80)
    makeMultiplePlots("Non Esaustiva Asimmetrica",c,nea,data,40,80)
