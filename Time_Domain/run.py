import subprocess
import pandas as pd
import matplotlib.pyplot as plt
import os 

path = os.path.dirname(os.path.abspath(__file__))  # Get dir path of the python file (Here ferrite_loss/Time_Domain)
path_prog = os.path.join(path,"cpp_code", "build")
exec_path = os.path.join(path_prog, "main")


# Simulation parameters 
mesh_path = os.path.join(os.path.dirname(path), "mesh", "N30_P.msh")
source = "b"                # "b", "H" or "NI"
waveform = "triangle"       # "sine", "triangle" or "square"
nb_period = 3
nb_iter_by_period = 100
f = 500e3
peak = 10       # (mT for b, A/m for H and A.turns for NI)

# Material parameters. Here, the parameters of N30 ferrite core have been applied
N30 = "5.98e-2 4.44e-1 2.48e-6"     # format : "rho, sigma, epsilon"  
muN30 = "4300, 1.2e6"               # format : "mu_DC, fc"  such as mu(s) = mu_DC / (1 + s/fc)

# Execution on the program with the set parameters
print("Running simulation ...")
exec = subprocess.run([exec_path, "-p", mesh_path, "-s", source, "-w", waveform,
                       "-np", str(nb_period), "-ni", str(nb_iter_by_period), "-f", str(f),
                       "-pk", str(peak), "-d", N30, "-u", muN30, ], capture_output=True, cwd=path_prog, check=True)

print("Simulation has ended")

#Simulation data have been saved in ./Time_Domain/cpp_code/data/TD.csv
path_data = os.path.join(path,"cpp_code", "data", "TD.csv")
data = pd.read_csv(path_data, sep=";")   # Read csv file
t = data["t"]
fluxH = data["fluxH"]
flux = data['flux']
NI = data["NI"]
P_eddy = data['p_eddy']/1000
P_exce = data["p_exc"]/1000

# ----------------- Plots ----------------------
plt.figure()
plt.plot(t, NI)
plt.xlabel("Time (s)")
plt.ylabel('Current NI (A.turns)')
plt.title('Current NI ')
plt.grid(True)

# Plot of the power
plt.figure()
plt.plot(t, P_eddy, label="Eddy current losses")
plt.plot(t, P_exce, label="Excess losses")
plt.plot(t, P_eddy + P_exce, label="Total losses")
plt.xlabel("Time (s)")
plt.ylabel('Power (kW/m3)')
plt.title("Power Losses")
plt.legend()
plt.grid(True)


# Plot of the magnetic flux
fig, ax1 = plt.subplots()

ax1.plot(t, fluxH, color="tab:blue", label="flux of H")
ax1.set_xlabel("Time (s)", color="black")
ax1.set_ylabel("flux of H (A.m)", color="black")
ax1.tick_params(axis="y", colors="black")
ax1.tick_params(axis="x", colors="black")

ax2 = ax1.twinx()
ax2.plot(t, flux, color="tab:red", label="flux of B")
ax2.set_ylabel("flux of B (Wb)", color="black")
ax2.tick_params(axis="y", colors="black")

lines1, labels1 = ax1.get_legend_handles_labels()
lines2, labels2 = ax2.get_legend_handles_labels()
ax1.legend(lines1 + lines2, labels1 + labels2, loc="best")
ax1.grid()

fig.tight_layout()

plt.show()
