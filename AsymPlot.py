import numpy as np
import matplotlib.pyplot as plt
import matplotlib

plt.close("all")

from matplotlib.colors import LogNorm

data = np.loadtxt("Asyms.File")

target1 = np.zeros((len(data),5))
target2 = np.zeros((len(data),5))

iter1 = 0
iter2 = 0


for i in range(len(data)):
    if data[i,1] == 1:# and abs(data[i,0]-511)<=5:
        target1[iter1,0] = data[i,0]
        target1[iter1, 1] = data[i, 2]
        target1[iter1, 2] = data[i, 4]
        target1[iter1, 3] = data[i, 5]
        target1[iter1, 4] = 0

        iter1 += 1
    if data[i, 1] == -1:# and abs(data[i, 0]-511) <= 5:
        target2[iter2,0] = data[i, 0]
        target2[iter2, 1] = data[i, 2]
        target2[iter2, 2] = data[i, 4]
        target2[iter2, 3] = data[i, 5]
        target2[iter2, 4] = 100
        iter2 += 1

plt.figure(1,figsize=(5,4))
plt.clf()
plt.hist2d(data[:,0],data[:,1],range=[[0,1000],[-1.5,1.5]],bins=(1000,300),cmap=plt.cm.jet,norm=LogNorm())


plt.figure(2, figsize=(5, 4))
plt.clf()

dful = np.concatenate((target1[0:iter1, 0], target2[0:iter2, 0]),axis=0)
plt.hist(dful,range=[0,1000],bins=500,color="royalblue",histtype="step")

#plt.hist(np.log(target1[0:iter1,1]), range=[-100, 1], bins=500//2,histtype="step",color="royalblue")
#plt.hist(np.log(target2[0:iter2,1]), range=[-100, 1], bins=500//2, histtype="step", color="r")


plt.figure(3, figsize=(5, 4))
plt.clf()
plt.hist2d(target1[0:iter1, 0], np.log(target1[0:iter1, 1]), range=[[0, 1000], [-100, 10]],
           bins=(500, 500), cmap=plt.cm.jet, norm=LogNorm())

from mpl_toolkits.mplot3d import Axes3D  # noqa: F401 unused import


h1, xedges, yedges = np.histogram2d(
    target1[0:iter1, 2], target1[0:iter1, 3], range=[[-30, 30], [-30, 30]],bins=(300,300))


plt.figure(1221)
plt.clf()
im = plt.imshow(h1,extent=[-30,30,-30,30],origin="lower",aspect="auto",cmap=plt.cm.jet,norm=LogNorm())

mH1 = np.max(h1)

targetZ1 = np.zeros(len(h1))

x1 = np.linspace(-30,30,300)
y1 = np.linspace(-30, 30, 300)
z1 = np.zeros(300)
from scipy import stats

#

xx,yy = np.meshgrid(x1,y1)
zz = np.zeros(xx.shape)

X = xx
Y = yy


fig = plt.figure(4, figsize=(5, 4))
ax = fig.add_subplot(111, projection='3d')
ax.set_aspect('equal')

x = xx#, y = np.ogrid[0:im.shape[0], 0:im.shape[1]]
y = yy

ax.plot_surface(x, y, zz, rstride=1, cstride=1, facecolors=zz)
#ax.plot_surface(X, Y, z)
#ax.scatter(X, targetZ1,
#           Y, c=z, marker="o", s=1, cmap=plt.cm.jet)
"""
print(len(h1),len(xedges))
print(h1)

ax.hist2d(target1[0:iter1, 0], np.log(target1[0:iter1, 1]), zs=0, zdir='z', range=[[0, 1000], [-100, 10]],
          bins=(500, 500), cmap=plt.cm.jet, norm=LogNorm())

#ax.scatter(target2[0:iter2, 4], target2[0:iter2, 2],
#            target2[0:iter2, 3], c="r", marker="o", s=1)
"""
