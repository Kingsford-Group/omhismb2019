import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import argparse
from scipy import stats
import csv
import gzip
import numpy as np

font = {'family' : 'normal',
        'size'   : 8}

matplotlib.rc('font', **font)
plt.rc('text', usetex=True)
plt.rc('font', family='serif')
plt.rc('axes', linewidth=0.1)

parser = argparse.ArgumentParser(description='Make difference boxplots')
parser.add_argument('-in_fname',dest='ifname', type=str)
parser.add_argument('-out_fname',dest='ofname', type=str)
parser.add_argument('-n',dest='n', type=int, default=100)

args = parser.parse_args()

X = list()
Y = list()
Z = list()
H = list()
with open(args.ifname,'rt') as tsvinf:
  tsvin = csv.reader(tsvinf, delimiter=' ')
  for row in tsvin:
    while len(X) < int(row[0])+1:
      X.append(list())
    X[int(row[0])].append(float(row[1]))
    while len(Y) < int(row[0])+1:
      Y.append(list())
    Y[int(row[0])].append(float(row[2]))
    while len(Z) < int(row[0])+1:
      Z.append(list())
    Z[int(row[0])].append(float(row[3]))
    while len(H) < int(row[0])+1:
      H.append(list())
    H[int(row[0])].append(1.0 - float(row[4])/args.n)
    


def set_box_color(bp, color):
    plt.setp(bp['boxes'], color=color)
    plt.setp(bp['medians'], color=color)
    plt.setp(bp['whiskers'], color=color)
    plt.setp(bp['caps'], color=color)


f, (ax) = plt.subplots(1, 1, figsize=(3.25, 3.25), dpi=200)
ax.set_xlabel("$\mathrm{E}_\mathrm{d}$", fontdict={'fontsize': 10})
ax.set_ylabel("$\mathrm{Similarity}$ $\mathrm{Measure}$", fontdict={'fontsize': 10})
#ax.set_ylabel("Weighted Jaccard - Jaccard", fontdict={'fontsize': 10, 'fontweight': 'bold'})
#plt.title(args.ifname)
j = ax.boxplot(X, positions=np.array(range(len(X)))*2.0-0.4, showfliers=False, whis=0, widths=0.0, capprops={'linewidth': 0.5}, whiskerprops={'linewidth': 0}, medianprops={'linewidth': 0.5}, boxprops={'linewidth': 0.25})
wj = ax.boxplot(Y, positions=np.array(range(len(Y)))*2.0,  showfliers=False, whis=0, widths=0.0, capprops={'linewidth': 0.5}, whiskerprops={'linewidth': 0}, medianprops={'linewidth': 0.5}, boxprops={'linewidth': 0.25})
omh = ax.boxplot(Z, positions=np.array(range(len(Z)))*2.0+0.4,  showfliers=False, whis=0, widths=0.0, capprops={'linewidth': 0.5}, whiskerprops={'linestyle':'-', 'linewidth': 0.25}, medianprops={'linewidth': 0.5}, boxprops={'linewidth': 0.25})
h = ax.boxplot(H, positions=np.array(range(len(H)))*2.0+0.8,  showfliers=False, whis=0, widths=0.0, capprops={'linewidth': 0.5}, whiskerprops={'linewidth': 0}, medianprops={'linewidth': 0.5}, boxprops={'linewidth': 0.25})

set_box_color(j, 'red')
set_box_color(wj, 'blue')
set_box_color(omh, 'green')
set_box_color(h, 'purple')
plt.plot([], c='red', label='Jaccard')
plt.plot([], c='blue', label='Weighted Jaccard')
plt.plot([], c='purple', label='Hamming')
plt.plot([], c='green', label='Order Min Hash')
#ax.boxplot(X, showfliers=False, whiskerprops={'linestyle': 'solid', 'linewidth': 0.5, 'color': '#777777'}, capprops={'linestyle': 'solid', 'linewidth': 0.5, 'color': '#777777'})
f.tight_layout()
ax.set_ylim([-0.01,1.01])
ax.set_xlim([-2.5,95])
leg = ax.legend(loc="lower left", prop={'size': 6, 'weight': 'ultralight'}, markerfirst=False)
leg.get_frame().set_linewidth(0.0)
ax.set_xticks(ax.get_xticks()[::10])
ax.set_xticklabels([str(10*i/float(args.n)) for i in range(0,len(ax.get_xticks())) ])
ax.tick_params(direction="in", top=False, right=False)
#ax.set_xticklabels([str(i) if i%5==0 else "" for i in range(0,len(X))])


  
#plt.tight_layout()
plt.savefig(args.ofname)
