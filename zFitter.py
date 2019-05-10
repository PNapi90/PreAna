from matplotlib.ticker import LinearLocator, FormatStrFormatter
from matplotlib import cm
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import matplotlib.pyplot as plt
import scipy.optimize as opt

plt.close("all")

from matplotlib import rc
rc('text', usetex=True)



def gaussian_2d(xy_mesh, amp, xc, yc, sigma_x, sigma_y):

    # unpack 1D list into 2D x and y coords
    (x, y) = xy_mesh

    # make the 2D Gaussian matrix
    gauss = amp*np.exp(-np.abs((x-xc)/(sigma_x)) - np.abs((y-yc)/(sigma_y)))

    # flatten the 2D Gaussian down to 1D
    return np.ravel(gauss)


x = np.linspace(-1000, 1000, 1000)
y = np.linspace(-1000, 1000, 1000)

xy_mesh = np.meshgrid(x, y)

fitvals = np.zeros((40,5))

fig,ax = plt.subplots(1,figsize=(5,4))

colors = plt.cm.jet(np.linspace(0, 1, 20))


for i in range(20):
    
    data = np.loadtxt("Histograms/File_Merge_Cs_" + str(i))

    initial_guess = (100, 0, 0 , 10, 10)
    popt, pcov = opt.curve_fit(
        gaussian_2d, xy_mesh, np.ravel(data), p0=initial_guess)
    #ax.plot(x,data[499,:],color=colors[i],ls="None",lw=1,ms=2,marker="o")

    print(i,popt)

    data_fitted = gaussian_2d(xy_mesh, *popt)
    ax.imshow(data.reshape(1000, 1000), cmap=plt.cm.jet, origin='bottom',
             extent=(x.min(), x.max(), y.min(), y.max()))
    for k in range(5):
        fitvals[i,k] = popt[k]
    
    #ax.contour(xy_mesh, data_fitted.reshape(1000, 1000), 8, colors='w')


def Gauss(x,a,mu,sig):
    return a*np.exp(-np.abs((x-mu)/sig))


xvals = np.linspace(-1000,1000,1000)

colors = plt.cm.jet(np.linspace(0,1,20))

plt.figure(2,figsize=(5,4))

for i in range(20):
    yval = Gauss(xvals, 1, fitvals[i, 1], fitvals[i, 3])
    plt.plot(xvals,yval,color=colors[i])

zPos = np.array([-200 + i*400/20. for i in range(20)])

fig22,ax22 = plt.subplots(1,figsize=(5,4))

for i in range(20):
    meanPos = (fitvals[i, 1]**2 + fitvals[i, 2]**2)
    meanPos = np.sqrt(meanPos)
    ax22.scatter(zPos[i],meanPos,color="b")
ax22.set_xlabel("$z$ (mm)",fontsize=16)
ax22.set_ylabel("$\\Delta r$ (mm)", fontsize=16, color="b")

ax33 = ax22.twinx()
ax33.set_ylabel("$\\sigma$ (mm)", fontsize=16,color="r")
for i in range(20):
    meanSig = (fitvals[i, 2]**2 + fitvals[i, 3]**2)
    meanSig = np.sqrt(meanSig)
    plt.scatter(zPos[i], meanSig,color="r")
ax22.tick_params(axis='both', which='major', labelsize=15)
ax22.tick_params(axis='both', which='minor', labelsize=10)
ax22.set_ylim(0,50)
ax33.set_ylim(95, 160)
ax22.tick_params(axis='y', labelcolor="b")
ax33.tick_params(axis='y', labelcolor="r")

ax33.tick_params(axis='both', which='major', labelsize=15)
ax33.tick_params(axis='both', which='minor', labelsize=10)

plt.savefig("improve.pdf",bbox_inches="tight")
