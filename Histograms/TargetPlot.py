import numpy as np
import matplotlib.pyplot as plt

plt.close("all")

data = np.loadtxt("Histograms/File_0_0")
data2 = np.loadtxt("Histograms/File_0_1")


from matplotlib.colors import LogNorm


fig,ax = plt.subplots(1,2,figsize=(10,4),sharex=True,sharey=True)
ax[0].imshow(data,origin="lower",extent=[-1000,1000,-1000,1000],aspect="auto")#,norm=LogNorm(vmin=1))
ax[1].imshow(data2,origin="lower",extent=[-1000,1000,-1000,1000],aspect="auto")#,norm=LogNorm(vmin=1))
#ax[2].imshow(data-data2,origin="lower",extent=[-1000,1000,-1000,1000],aspect="auto",cmap=plt.cm.seismic)
for i in range(2):
    ax[i].vlines(-100,-100,100)
    ax[i].vlines(100,-100,100)
    ax[i].hlines(-100,-100,100)
    ax[i].hlines(100,-100,100)
    ax[i].axis("equal")

plt.savefig("krasskrass.png",bbox_inches="tight",dpi=300)
