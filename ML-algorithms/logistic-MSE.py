#!/usr/bin/python2.4

__author__ = 'zhuhcheng@gmail.com (Hongcheng Zhu)'

import sys
import math
import getopt

def readData(filename):
  sys.stderr.write('Loading data file:'+filename+'\n')
  X,Y=[],[]
  Dim=-1
  for line in file(filename):
    #print line.strip().split(',')
    t=map(float,line.strip().split(','))
    if Dim!=-1 and Dim != len(t):
      sys.stderr.write('Bad line:'+line)
      continue
    Dim=len(t)
    Y.append(t[len(t)-1])
    t.pop()
    t.append(1.0)
    X.append(t)
  return X,Y

def calculate_sigmod(X,Para):
  res=0.0
  for i in range(len(X)): res+=X[i]*Para[i]
  return 1.0/(1.0+math.exp(-res))

def prediction(X,Para):
  Y=[]
  for i in range(len(X)):
    Y.append(calculate_sigmod(X[i],Para))
  return Y
def squaredSum(Y1,Y2):
  res=0.0
  for i in range(len(Y1)):
    res+=(Y1[i]-Y2[i])**2
  return res

def doTraining(X,Y,itr_times):
  eps=10.0
  dim=len(X[0])
  P=[0]*dim
  Y1=prediction(X,P)
  pre_sum=squaredSum(Y1,Y)
  itr=-1
  good=0
  while eps>1e-6:
    itr+=1
    if itr>itr_times: break
    Z=[]
    for i in range(len(X)):
      Z.append(2.0*(Y1[i]-Y[i])*Y1[i]*(1-Y1[i]))
    new_P=list(P)
    new_Y1=list(Y1)
    for k in range(dim):
      for i in range(len(Y)):
        new_P[k]-=eps*X[i][k]*Z[i]
    new_Y1=prediction(X,new_P)
    new_sum=squaredSum(new_Y1,Y)
    #print P,new_P
    if new_sum<pre_sum:
      pre_sum,P,Y1=new_sum,new_P,new_Y1
      good+=1
      if good==10:
        eps*=1.1
        good=0
    else:
      eps/=1.2
      good=0
    sys.stderr.write('Iteration #'+str(itr)+': error='+str(pre_sum)+' eps='+str(eps)+'\n')
  return P

def main(argv):
  try:
    #p: predict.  d: data file(csv). t: iteration times
    opts,args=getopt.getopt(argv,'p:d:t:')
  except getopt.GetoptError,err:
    sys.stderr.write(str(err))
    sys.exit(2)
  do_pred=False
  datafile=''
  parafile=''
  itr_times=1000000000
  for o,a in opts:
    if o in ('-p'):
      parafile=a
      do_pred=True
    elif o in ('-d'): datafile=a
    elif o in ('-t'): itr_times=int(a)
  if datafile=='':
    sys.stderr.write('no data file\n')
    sys.exit(1)
  if do_pred and parafile=='':
    sys.stderr.write('no para file for prediction\n')
    sys.exit(1)
  X,Y=readData(datafile)
  sys.stderr.write('training sample size='+str(len(Y))+'\n')
  sys.stderr.write('feature dimention='+str(len(X[0]))+'\n')
  sys.stderr.write('itr times='+str(itr_times)+'\n')
  if not do_pred:
    P=doTraining(X,Y,itr_times)
    for item in P: print '%f'%(item)
  elif do_pred:
    P=[]
    for line in file(parafile):
      P.append(float(line.strip()))
    if len(P)!=len(X[0]):
      sys.stderr.write('Dimension mismatched in parameter file.\n')
      sys.exit(1)
    Y1=prediction(X,P)
    for i in range(len(Y1)):
      print '%f\t%f'%(Y[i],Y1[i])

if __name__ == '__main__':
  main(sys.argv[1:])
