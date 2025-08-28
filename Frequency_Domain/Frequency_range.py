### Python code for running the compiled c++ code simFD.cpp with different parameters
### The goal is to be able to easily run the simulation with differents Frequencies 
### and material parameters.

### It is also possible to plot the measured data to compare them with the simulation.

import os
import subprocess
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

path = os.path.dirname(os.path.abspath(__file__))
path_prog = os.path.join(path,"cpp_code", "build")
exec_path = os.path.join(path_prog, "simFD")

mesh_path = os.path.join(os.path.dirname(path), "mesh", "N30_P.msh")


N30 = "5.98e-2 4.44e-1 2.48e-6" # format : "rho, sigma, epsilon"
muN30 = "4300, 1.2e6"

N87 = "4.24e-2, 1.48e-1, 2.68e-6"
muN87 = "2200, 4e6"

T38 = "4.04e-2, 1.07e1, 8.06e-6"
muT38 = "10000, 380e3"

# Lists used for the frequency range
f_list = np.linspace(100e3, 500e3, 19)
P_eddy = []
P_mag = []
P_tot = []
flux_real = []
flux_imag = []
NI = []


for f in f_list: 
    #Run c++ simulation (Must be compiled before)
    exec = subprocess.run([exec_path, "-p", mesh_path, "-f", str(f), "-d", N30, "-u", muN30, "-b", "15"], capture_output=True, cwd=path_prog, check=True)

    # The compiled code print all the results as the last line using the following syntax :
    # f;PLoss_eddy;PLoss_mag;Ptot;flux_real;flux_imag;NI
    data = str(exec.stdout.strip()).replace("'", "").split('\\n')[-1].split(";")  # Get the last line printed by the simulation
    # Parse the differents results
    P_eddy.append(float(data[1]))
    P_mag.append(float(data[2]))
    P_tot.append(float(data[3]))
    flux_real.append(float(data[4]))
    flux_imag.append(float(data[5]))
    NI.append(float(data[6]))

# Get absolute value of the flux (NI is real as it's been used as the phase reference)
flux_abs = np.abs(np.array(flux_real) + 1j * np.array(flux_imag))

# Get measured power losses data
data_p = pd.read_csv(os.path.join(os.path.dirname(path), "measurements_data","Princeton_data", "Sinusoidal", "N30", "N30-Sinusoidal_Phi_15.csv"))
Power_P = data_p["Power_Loss"]
Freq_P = data_p["Frequency"]

### Plotting the results
# Plotting the losses
plt.figure()
plt.plot(f_list/1000, P_tot, label="MFEM")
plt.plot(Freq_P/1000, Power_P, label="Measurement")
plt.legend()
plt.grid()
plt.xlabel('f(kHz)')
plt.ylabel('Total losses (W/m3)')

#Plotting the current
plt.figure()
plt.plot(f_list/1000, NI)
# plt.plot(Freq_P, Power_P)
plt.grid()
plt.xlabel('f(kHz)')
plt.ylabel('NI (A.turns)')

# Plotting the flux
plt.figure()
plt.plot(f_list/1000, flux_abs)
plt.grid()
plt.xlabel('f(kHz)')
plt.ylabel('flux (Wb)')
plt.show()
