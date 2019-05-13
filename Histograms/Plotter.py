import numpy as np
import matplotlib.pyplot as plt

plt.close("all")

data = np.loadtxt("dataT.data")


plt.figure(1,figsize=(5,4))
plt.clf()
plt.hist(data[:,0],range=[0,2000],bins=4000,histtype="step",color="b")
plt.hist(data[:, 1], range=[0, 2000], bins=4000, histtype="step", color="r")


plt.figure(2, figsize=(5, 4))
plt.clf()
plt.hist2d(data[:, 2], data[:, 3], range=[[-250, 250], [-250, 250]], bins=(200,200), cmap=plt.cm.jet,cmin=1)

from matplotlib.colors import LogNorm

plt.figure(3, figsize=(5, 4))
plt.clf()
plt.hist2d(data[:, 6], data[:, 5], range=[[0, 4000],
                                          [0, 4000]], bins=(1000, 1000), cmap=plt.cm.jet, cmin=1,norm=LogNorm())
