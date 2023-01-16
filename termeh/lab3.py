import matplotlib.pyplot as plt
import numpy as np
import sympy as sp
from matplotlib.animation import FuncAnimation
from scipy.integrate import odeint

def odesys(y, t, m, a, b, k):
    yt = np.zeros_like(y)
    yt[0] = y[2]
    yt[1] = y[3]

    a11 = m
    a12 = 0
    a21 = 0
    a22 = m * (a ** 2 + (b ** 2 * (np.sin(y[0])) ** 2))

    b1 = -k * y[2] + m * y[3] ** 2 * np.sin(y[0]) * np.cos(y[0])
    b2 = -k * y[3] * (a ** 2 + b ** 2 * (np.sin(y[0])) ** 2)- y[3] * m * y[2] * b ** 2 * np.sin(2 * y[0])

    yt[2] = (b1 * a22 - a12 * b2) / (a11 * a22 - a12 * a21)
    yt[3] = (b2 * a11 - a21 * b1) / (a11 * a22 - a12 * a21)

    return yt

T = np.linspace(0, 50, 1000)
DO = 1

m = 1
a = 2    # CD
b = 0.5  # AC
k = 0.75
phi0, teta0 = 0, 0
dphi0, dteta0 = 0.2, 0.2

# m = 50
# a = 1 
# b = 0.25
# k = 0.75
# phi0, teta0 = 0, 0
# dphi0, dteta0 = 0.2, 0.2

# m = 1
# a = 1 
# b = 0.25
# k = 1
# phi0, teta0 = 0, 0
# dphi0, dteta0 = 7, 7

m = 1
a = 1 
b = 0.25
k = 0
phi0, teta0 = 0, 1
dphi0, dteta0 = 6, 0


y0 = [teta0, phi0, dteta0, dphi0]
res = odeint(odesys, y0, T, (m, a, b, k))

teta = res[:, 0]
phi = res[:, 1]
Vteta = res[:, 2]
Vphi = res[:, 3]

Vp = Vphi * a
Vt = Vteta * b
V_abs = np.zeros_like(T)

for i in np.arange(len(T)):
        V_abs[i] = np.sqrt(Vp[i] ** 2 + Vt[i] ** 2)

x_D = 0; y_D = 0; z_D = 0
x_O = x_D; y_O = y_D; z_O = z_D - DO
x_Q = x_D; y_Q = y_D; z_Q = z_D + DO
z_C = z_D

fig = plt.figure(figsize=[13, 9])
ax = fig.add_subplot(1, 2, 1, projection='3d')
ax.axis('auto'), ax.set_xlabel('x'), ax.set_ylabel('y'), 
ax.set(xlim=[x_D - 3 * a, x_D + 3 * a],
       ylim=[y_D - 3 * a, y_D + 3 * a],
       zlim=[z_D - 3 * b, z_D + 3 * b])

ax1 = fig.add_subplot(4, 2, 2)
ax1.plot(Vphi)
plt.ylabel('Vphi')

ax1 = fig.add_subplot(4, 2, 4)
ax1.plot(Vteta)
plt.ylabel('Vteta')

ax1 = fig.add_subplot(4, 2, 6)
ax1.plot(V_abs)
plt.ylabel('V_abs')

S_CD = ax.plot([x_D, a * sp.cos(phi[0]) + x_D], [y_D, a * sp.sin(phi[0]) + y_D], [z_D, z_C], c="orange", lw=3)[0]
S_OQ = ax.plot([x_O, x_Q], [y_O, y_Q], [z_O, z_Q], c="r", lw=3)[0]
S_AB = ax.plot([a * sp.cos(phi[0]) - b * np.sin(teta[0]) * np.cos(phi[0]), a * sp.cos(phi[0]) + b * np.sin(teta[0]) * np.cos(phi[0])], 
    [a * sp.sin(phi[0]) - b * np.sin(teta[0]) * np.sin(phi[0]), a * sp.sin(phi[0]) + b * np.sin(teta[0]) * np.sin(phi[0])], 
    [z_C + b * sp.cos(teta[0]), z_C - b * sp.cos(teta[0])], c="r", lw=3)[0]
O = ax.plot(x_O, y_O, z_O, marker='s', c='b', ms=6)[0]
Q = ax.plot(x_Q, y_Q, z_Q, marker='s', c='b', ms=6)[0]
A = ax.plot(a * sp.cos(phi[0]) - b * np.sin(teta[0]) * np.cos(phi[0]), 
    a * sp.sin(phi[0]) - b * np.sin(teta[0]) * np.sin(phi[0]), 
    z_C + b * sp.cos(teta[0]), c='green', marker='o', ms=10, label="A")[0]
B = ax.plot(a * sp.cos(phi[0]) + b * np.sin(teta[0]) * np.sin(phi[0]), 
    a * sp.sin(phi[0]) + b * np.sin(teta[0]) * np.sin(phi[0]), 
    z_C - b * sp.cos(teta[0]), c='m', marker='o', ms=10, label="B")[0]
C = ax.plot(a * sp.cos(phi[0]) + x_D, a * sp.sin(phi[0]) + y_D, z_C, c='w', marker='o', mec='m', ms=6, label="C")[0]
D = ax.plot(x_D, y_D, z_D, c='black', marker='o', ms=6, label="D")[0]

isLegendSet = False

def anima(i):
    S_CD.set_data_3d([x_D, a * np.cos(phi[i]) + x_D], [y_D, a * np.sin(phi[i]) + y_D], [z_D, z_C])
    S_OQ.set_data_3d([x_O, x_Q], [y_O, y_Q], [z_O, z_Q])
    S_AB.set_data_3d([a * np.cos(phi[i]) - b * np.sin(teta[i]) * np.cos(phi[i]), a * np.cos(phi[i]) + b * np.sin(teta[i]) * np.cos(phi[i])], 
        [a * np.sin(phi[i]) - b * np.sin(teta[i]) * np.sin(phi[i]), a * np.sin(phi[i]) + b * np.sin(teta[i]) * np.sin(phi[i])], 
        [z_C + b * np.cos(teta[i]), z_C - b * np.cos(teta[i])])
    O.set_data_3d(x_O, y_O, z_O)
    Q.set_data_3d(x_Q, y_Q, z_Q)
    A.set_data_3d(a * np.cos(phi[i]) - b * np.sin(teta[i]) * np.cos(phi[i]), 
        a * np.sin(phi[i]) - b * np.sin(teta[i]) * np.sin(phi[i]), 
        z_C + b * np.cos(teta[i]))
    B.set_data_3d(a * np.cos(phi[i]) + b * np.sin(teta[i]) * np.cos(phi[i]), 
        a * np.sin(phi[i]) + b * np.sin(teta[i]) * np.sin(phi[i]), 
        z_C - b * np.cos(teta[i]))
    C.set_data_3d(a * np.cos(phi[i]) + x_D, a * np.sin(phi[i]) + y_D, z_C)
    D.set_data_3d(x_D, y_D, z_D)

    global isLegendSet;
    if not isLegendSet:
        ax.legend()
        isLegendSet = True

    return S_AB, S_CD, S_OQ, O, Q, A, B, C, D


anim = FuncAnimation(fig, anima, frames=len(T), interval=10)

plt.show()