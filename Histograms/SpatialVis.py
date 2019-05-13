import numpy as np
import matplotlib.pyplot as plt
from numpy import random
from mpl_toolkits.mplot3d import Axes3D

from matplotlib import rc
rc("text",usetex=True)

plt.close("all")

fig = plt.figure()
ax = fig.gca(projection='3d')
ax.set_aspect("equal")
#ax.axis("equal")
# draw sphere

anfang = 10*np.pi/180.
ende = 50*np.pi/180.

u, v = np.mgrid[70*np.pi/180:2*np.pi:10j, anfang:ende:10j]
x = 235*np.cos(u)*np.sin(v)
y = 235*np.sin(u)*np.sin(v)
z = 235*np.cos(v)
ax.plot_wireframe(x, y, z, color="r")

x = 325*np.cos(u)*np.sin(v)
y = 325*np.sin(u)*np.sin(v)
z = 325*np.cos(v)
ax.plot_wireframe(x, y, z, color="r")
ax.set_zlim(-300,300)
ax.set_xlim(-300,300)
ax.set_ylim(-300, 300)

# create x,y
xx, yy = np.meshgrid(range(-100,100), range(-100,100))
xx2, yy2 = np.meshgrid(range(-30, 30), range(-30, 30))
# calculate corresponding z


ax.plot_surface(xx, yy, 0, alpha=0.2)
ax.plot_surface(xx2, yy2, 0, alpha=1,color="b")

ax.plot_surface(xx, yy, 100, alpha=0.2,color="k")
ax.plot_surface(xx2, yy2, 100, alpha=1, color="b")

import math
maxN = 100000
g = np.zeros((maxN,2))
gtrue = np.zeros((maxN, 2))

for i in range(maxN):
    r = random.uniform(235,325,1)
    while True:
        phi = random.uniform(0,2*np.pi,1)
        if phi >= 70*np.pi/180 and phi <= 2*np.pi:
            break
    while True:
        th = math.acos(1 - 2*random.uniform(0,1))
        if th >= anfang and th <= ende:
            break
    xs = [0,0,0]
    xs[0] = r*np.sin(th)*np.cos(phi)
    xs[1] = r*np.sin(th)*np.sin(phi)
    xs[2] = r*np.cos(th)

    x = random.uniform(-30,30,3)
    x[2] = 0
    dir = [xs[i] - x[i] for i in range(3)]
    n = 0
    for d in dir:
        n += d**2
    n = np.sqrt(n)
    dir /= n

    lam = (100 - x[2])/dir[2]
    for j in range(2):
        g[i,j] = x[j] + lam*dir[j]
    for j in range(2):
        gtrue[i,j] = x[j]


fig,ax = plt.subplots(2,1,figsize=(5,8),sharex=True)

ax[0].hist2d(g[:, 0], g[:, 1], range=[[-300, 300], [-300, 300]],
             bins=(300, 300), cmap=plt.cm.jet, cmin=1)
ax[0].hist2d(gtrue[:, 0], gtrue[:, 1], range=[[-300, 300], [-300, 300]],
             bins=(300, 300), cmap=plt.cm.autumn, cmin=1)


for i in range(maxN):
    r = random.uniform(235, 325, 1)
    while True:
        phi = random.uniform(0, 2*np.pi, 1)
        if phi >= 70*np.pi/180 and phi <= 2*np.pi:
            break
    while True:
        th = math.acos(1 - 2*random.uniform(0, 1))
        if th >= anfang and th <= ende:
            break
    xs = [0, 0, 0]
    xs[0] = r*np.sin(th)*np.cos(phi)
    xs[1] = r*np.sin(th)*np.sin(phi)
    xs[2] = r*np.cos(th)

    x = random.uniform(-30, 30, 3)
    x[2] = 100
    dir = [xs[i] - x[i] for i in range(3)]
    n = 0
    for d in dir:
        n += d**2
    n = np.sqrt(n)
    dir /= n

    lam = (0 - x[2])/dir[2]
    for j in range(2):
        g[i, j] = x[j] + lam*dir[j]
    for j in range(2):
        gtrue[i, j] = x[j]


ax[1].hist2d(g[:, 0], g[:, 1], range=[[-300, 300], [-300, 300]],
             bins=(300, 300), cmap=plt.cm.jet, cmin=1)
ax[1].hist2d(gtrue[:, 0], gtrue[:, 1], range=[[-300, 300], [-300, 300]],
             bins=(300, 300), cmap=plt.cm.autumn, cmin=1)

for i in range(2):
    ax[i].tick_params(axis='both', which='major', labelsize=15)
    ax[i].tick_params(axis='both', which='minor', labelsize=10)
    ax[i].set_ylabel("$y$ (mm)", fontsize=17)

ax[1].set_xlabel("$x$ (mm)",fontsize=17)
plt.subplots_adjust(hspace=0.1)
plt.savefig("targetplot.pdf",bbox_inches="tight")
