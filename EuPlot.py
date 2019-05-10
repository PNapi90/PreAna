from matplotlib import rc
from scipy.interpolate import interp1d
import numpy as np
import matplotlib.pyplot as plt

plt.close("all")

rc('text', usetex=True)
plt.close("all")

data = np.loadtxt("hits.dat")

Eupeaks = [121.779,244.693,344.272,411.111,443.979,778.890,964.014,1085.793,1112.070,1407.993]

plt.figure(1,figsize=(5,4))
plt.clf()
plt.hist(data,range=[0,2000],bins=4000,color="royalblue",histtype="step")
for i in Eupeaks:
    plt.axvline(i,color="crimson",lw=0.5,ls="--")
plt.tick_params(axis='both', which='major', labelsize=15)
plt.tick_params(axis='both', which='minor', labelsize=10)
plt.xlabel("Energy (keV)",fontsize=17)
plt.ylabel("Counts per 0.5 keV", fontsize=17)
plt.savefig("histE.pdf",bbox_inches="tight")
