from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import matplotlib.pyplot as plt

plt.close("all")

x0 = np.array([85.6257, -105.693, 244.707])
x1 = np.array([76.2104 ,-101.675, 239.891])

ttt = 52.8929

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')


ax.scatter(x0[0], x0[1], x0[2])
ax.scatter(x1[0], x1[1], x1[2],c="r")
ax.scatter(0,0,0,c="m")



nh = 20
nphi = 20


cone = np.zeros((nh, nphi, 3))
h = np.linspace(-100, 0, nh)
phi = np.linspace(-180, 180, nphi)

talpha = np.tan(ttt*np.pi/180.)

import math

ax.axis("equal")
ax.set_xlim(-500,500)
ax.set_ylim(-500,500)

XForTrafo = x1 - x0
norm = 0
for x in XForTrafo:
    norm += x**2
norm = np.sqrt(norm)

theta = math.acos(XForTrafo[2]/norm)
alpha = math.atan2(XForTrafo[1], XForTrafo[0])

print(theta)


#theta = 30*np.pi/180
stheta = np.sin(theta)
ctheta = np.cos(theta)

#alpha = 45*np.pi/180
sal = np.sin(alpha)
cal = np.cos(alpha)

xline = [[0, 0, 0], [10, 0, 0]]
yline = [[0, 0, 0], [0, 10, 0]]
zline = np.zeros((100, 3))

for i in range(len(zline)):
    zline[i, 2] = -1000 + 1000/100*i
    #ax.scatter(zline[i, 0], zline[i, 1], zline[i, 2], color="k", s=0.55)

Mtheta = np.array([[ctheta, 0, stheta], [0, 1, 0], [-stheta, 0, ctheta]])
Malpha = np.array([[cal, -sal, 0], [sal, cal, 0], [0, 0, 1]])

for i, hh in enumerate(h):
    for j, p in enumerate(phi):
        cone[i, j, 0] = hh*talpha*np.cos(p*np.pi/180)
        cone[i, j, 1] = hh*talpha*np.sin(p*np.pi/180)
        cone[i, j, 2] = hh
        #ax.scatter(cone[i, j, 0], cone[i, j, 1], cone[i, j, 2],color="royalblue",s=1)
        x = Mtheta.dot(cone[i, j, :])
        if i == 0 and j == 0:
            for k in range(len(zline)):
                ztmp = Mtheta.dot(zline[k, :])
                #ax.scatter(ztmp[0], ztmp[1],
                #           ztmp[2], color="k", s=0.5)
                ztmp2 = Malpha.dot(ztmp)
                #ax.scatter(ztmp2[0], ztmp2[1], ztmp2[2], color="k", s=0.5)
                #ax.scatter(ztmp2[0]+3, ztmp2[1]+3,
                #           ztmp2[2]+3, color="k", s=0.5)
        #ax.scatter(x[0], x[1],
        #           x[2], color="r", s=1)
        y = Malpha.dot(x)
        #ax.scatter(y[0], y[1],
        #           y[2], color="forestgreen", s=1)
        ax.scatter(y[0]+x0[0], y[1]+x0[1],
                   y[2]+x0[2], color="m", s=1)

xxx = np.linspace(0, 10, 100)
for x in xxx:
    ax.scatter(x, 0, 0, color="b", s=0.25)
    ax.scatter(0, x, 0, color="b", s=0.25)
    ax.scatter(0, 0, x, color="b", s=0.25)
xx, yy = np.meshgrid(range(-100,100), range(-100,100))

ax.plot_surface(xx, yy, 0, alpha=0.2)
