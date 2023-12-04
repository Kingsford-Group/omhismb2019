import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import argparse
import re
import os
from math import ceil,floor
import numpy as np
from scipy import stats
import random

parser = argparse.ArgumentParser(description='Exaustively Search Edit Distance and Jaccard With and Without Weight, prints sequences and weights to standard out')
parser.add_argument('-n', dest='n', type=int, default=3, help="Total sequence length")
parser.add_argument('-i', dest='i', type=int, default=1, help="Indel cost for edit distance/alignment scoring")
parser.add_argument('-m', dest='m', type=int, default=2, help="Mismatch cost for edit distance/alignment scoring")
parser.add_argument('-k', dest='k', type=int, default=2, help="k-mer length")
parser.add_argument('-s', dest='s', type=int, default=2, help="Alphabet size (sigma)")
parser.add_argument('-r', dest='r', type=int, default=1000, help="Number of samples")
parser.add_argument('-trim', dest='trim', default=False, action='store_true', help="Enable to ensure all random sequences are exactly n characters")
parser.add_argument('-fname',dest='fname', type=str, help="Output figure comparing scores")

args = parser.parse_args()

chars = ['A','C','D','E','F','G','H','I','K','L','M','N','P','Q','R','S','T','V','W','Y']
chars_inv = {'A':0,'C':1,'D':2,'E':3,'F':4,'G':5,'H':6,'I':7,'K':8,'L':9,'M':10,'N':11,'P':12,'Q':13,'R':14,'S':15,'T':16,'V':17,'W':18,'Y':19}

def hamming_distance(s1, s2):
    n = min(len(s1), len(s2))
    d = 0
    for i in range(0, n-1):
        d = d + 1 if (s1[i] != s2[i]) else d

    return d + (len(s1) - n) + (len(s2) - n)


def edit_distance(s1, s2, indel, mismatch):
    m=len(s1)+1
    n=len(s2)+1

    tbl = {}
    for i in range(m): tbl[i,0]=i
    for j in range(n): tbl[0,j]=j
    for i in range(1, m):
        for j in range(1, n):
            cost = 0 if s1[i-1] == s2[j-1] else mismatch
            tbl[i,j] = min(tbl[i, j-1]+indel, tbl[i-1, j]+indel, tbl[i-1, j-1]+cost)

    return tbl[i,j]

def global_alignment(s1, s2, indel, mismatch):
    m=len(s1)+1
    n=len(s2)+1

    tbl = {}
    for i in range(m): tbl[i,0]= (-1 * i * indel)
    for j in range(n): tbl[0,j]= (-1 * j * indel)
    for i in range(1, m):
        for j in range(1, n):
            cost = 1 if s1[i-1] == s2[j-1] else (-1 * mismatch)
            tbl[i,j] = max(tbl[i, j-1]-indel, tbl[i-1, j]-indel, tbl[i-1, j-1]+cost)

    return tbl[i,j]

def local_alignment(s1, s2, indel, mismatch):
    m=len(s1)+1
    n=len(s2)+1

    tbl = {}
    for i in range(m): tbl[i,0]= 0
    for j in range(n): tbl[0,j]= 0
    mx = 0
    for i in range(1, m):
        for j in range(1, n):
            cost = 1 if s1[i-1] == s2[j-1] else (-1 * mismatch)
            tbl[i,j] = max(0, tbl[i, j-1]-indel, tbl[i-1, j]-indel, tbl[i-1, j-1]+cost)
            mx = max(mx,tbl[i,j])

    return mx

def jaccard(s1, s2, k):
    X = list()
    Y = list()
    for i in range(0,len(s1)-k+1):
        X.append(s1[i:i+k])
    for i in range(0,len(s2)-k+1):
        Y.append(s2[i:i+k])
    X = sorted(set(X))
    Y = sorted(set(Y))
    i = 0
    j = 0
    intersection = 0
    union = 0
    while i<len(X) and j<len(Y):
        #print str(i) + " " + str(j) + " " + str(len(X)) + " " + str(len(Y))
        if(X[i]==Y[j]):
            union += 1
            intersection += 1
            i += 1
            j += 1
        elif(X[i]>Y[j]):
            union += 1
            j += 1
        else:
            union += 1
            i += 1
    union += (len(X) - i) + (len(Y) - j)

    #print str(intersection) + "/" + str(union) 
    return float(intersection)/float(union)

def weighted_jaccard(s1, s2, k):
  X = {}
  Y = {}
  for i in range(0,len(s1)-k+1):
    if s1[i:i+k] in X:
      X[s1[i:i+k]] += 1
    else:
      X[s1[i:i+k]] = 1
  for i in range(0,len(s2)-k+1):
    if s2[i:i+k] in Y:
      Y[s2[i:i+k]] += 1
    else:
      Y[s2[i:i+k]] = 1
      
  X_ind = sorted(set(X))
  Y_ind = sorted(set(Y))
  
  #print X
  
  i = 0
  j = 0
  intersection = 0
  union = 0
  while i<len(X_ind) and j<len(Y_ind):
    #print str(i) + " " + str(j) + " " + str(X_ind[i]) + " " + str(Y_ind[j]) + " " + str(X_ind[i] == Y_ind[j]) + " " + str(len(X_ind)) + " " + str(len(Y_ind))
    if(X_ind[i]==Y_ind[j]):
        union += max(X[X_ind[i]],Y[Y_ind[j]])
        intersection += min(X[X_ind[i]],Y[Y_ind[j]])
        i += 1
        j += 1
    elif(X_ind[i]>Y_ind[j]):
        union += Y[Y_ind[j]]
        j += 1
    else:
        union += X[X_ind[i]]
        i += 1
  union += (len(X_ind) - i) + (len(Y_ind) - j)

  #print str(intersection) + "/" + str(union) 
  return float(intersection)/float(union)

E = list()
J = list()
W = list()

def mutate(s1, n):
  #s2 = s1
  m = random.randint(1,3)
  if len(s1) < 3:
    m = random.randint(1,2)
  i = random.randint(1,len(s1)-1)
  #print "n:" + str(n) + " m:" + str(m) + " i:" + str(i) 

  if m == 1:
    s2 = s1[0:i] + str(chars[(int(chars_inv[s1[i]]) + 1) % args.s])
    if i < len(s1) - 1: 
      s2 += s1[i+1:]
  elif m == 3:
    s2 = s1[0:i] 
    if i < len(s1) - 1: 
      s2 += s1[i+1:]
  else:
    s2 = s1[0:i] + str(chars[random.randint(0,args.s-1)]) 
    if i < len(s1): 
      s2 += s1[i:]
  #print " l:" + str(len(s2)) + " " + s1 + "->" + s2
  if n <= 1:
    return s2
  else:
    return mutate(s2, n-1)

for i in range(0,args.r):
  s1 = ""
  s2 = ""
  for j in range(0,args.n):
    s1 += str(chars[random.randint(0,args.s-1)])
    #s2 += str(random.randint(0,args.s-1))
  s2 = mutate(s1, random.randint(1,args.n))
  if args.trim:
    if len(s2) > args.n:
        s2 = s2[0:args.n]
    elif len(s2) < args.n:
        s2 += str('0'.zfill(args.n - len(s2)))
  
  if s2 < s1:
    temp = s1
    s1 = s2
    s2 = temp
  edi = edit_distance(s1, s2, args.i, args.m)
  ji = jaccard(s1,s2,args.k)
  wji = weighted_jaccard(s1, s2, args.k)
  ga = global_alignment(s1, s2, args.i, args.m)
  la = local_alignment(s1, s2, args.i, args.m)
  hdi = hamming_distance(s1, s2)
  E.append(la)
  J.append(ji)
  W.append(wji)
  print(s1 + " " + s2 + " " + str(edi) + " " + str(ga) + " " + str(la) + " " + str(ji) + " " + str(wji) + " " + str(hdi))
    
    
slope_j, intercept_j, r_value_j, p_value_j, std_err_j = stats.linregress(E, J)
slope_w, intercept_w, r_value_w, p_value_w, std_err_w = stats.linregress(E, W)
#print str(r_value_j**2) + " " + str(r_value_w**2)

#fig = plt.figure(figsize=[6,4.5], dpi=120)
plt.plot(E,J,'x',color="blue",label="Jaccard (r^2: " + str(r_value_j**2) + ")")
plt.plot(E,W,'+',color="red",label="Weighted Jaccard (r^2: " + str(r_value_w**2) + ")")
plt.xlabel("Edit Distance")
plt.ylabel("(Weighted) Jaccard")
plt.title("n=" + str(args.n) + " k=" + str(args.k) + "")
plt.legend()
plt.tight_layout()
plt.savefig(args.fname)
