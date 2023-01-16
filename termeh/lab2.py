import matplotlib.pyplot as plt
import numpy as np
import sympy as sp
from matplotlib.animation import FuncAnimation

t, T = sp.Symbol('t'), np.linspace(0, 10, 1000)
phi, teta = sp.cos(t) + sp.sin(t), 2 * sp.cos(t)
omegaT, omegaP = sp.diff(teta, t), sp.diff(phi, t)

a = 3; b = 2; DO = 4
V_e = omegaP * a
V_r = omegaT * b
V_abs = sp.sqrt(V_e**2 + V_r**2)

x_D = 0; y_D = 0; z_D = 0

x_O = x_D; y_O = y_D; z_O = z_D - DO
x_Q = x_D; y_Q = y_D; z_Q = z_D + DO

x_C = a * sp.cos(phi) + x_D
y_C = a * sp.sin(phi) + y_D
z_C = z_D

x_A = x_C - b * sp.sin(teta) * sp.cos(phi)
y_A = y_C - b * sp.sin(teta) * sp.sin(phi)
z_A = b * sp.cos(teta)

x_B = x_C + b * sp.sin(teta) * sp.cos(phi)
y_B = y_C + b * sp.sin(teta) * sp.sin(phi)
z_B = -b * sp.cos(teta)

F_func = [sp.lambdify(t, i) for i in [V_abs, x_C, y_C, x_A, y_A, z_A, x_B, y_B, z_B]]
[V_ABS, X_C, Y_C, X_A, Y_A, Z_A, X_B, Y_B, Z_B] = [func(T) for func in F_func]

fig = plt.figure()
ax = fig.add_subplot(1, 1, 1, projection='3d')
ax.axis('auto'), ax.set_xlabel('x'), ax.set_ylabel('y'), 
ax.set(xlim=[x_D - 2 * a, x_D + 2 * a],
       ylim=[y_D - 2 * a, y_D + 2 * a],
       zlim=[z_D - 2 * b, z_D + 2 * b])


S_CD = ax.plot([x_D, X_C[0]], [y_D, Y_C[0]], [z_D, z_C], c="orange", lw=3)[0]
S_OQ = ax.plot([x_O, x_Q], [y_O, y_Q], [z_O, z_Q], c="r", lw=3)[0]
S_AB = ax.plot([X_A[0], X_B[0]], [Y_A[0], Y_B[0]], [Z_A[0], Z_B[0]], c="r", lw=3)[0]
O = ax.plot(x_O, y_O, z_O, marker='s', c='b', ms=6)[0]
Q = ax.plot(x_Q, y_Q, z_Q, marker='s', c='b', ms=6)[0]
A = ax.plot(X_A[0], Y_A[0], Z_A[0], c='green', marker='o', ms=10, label="A")[0]
B = ax.plot(X_B[0], Y_B[0], Z_B[0], c='m', marker='o', ms=10, label="D")[0]
C = ax.plot(X_C[0], Y_C[0], z_C, c='w', marker='o', mec='m', ms=6)[0]
D = ax.plot(x_D, y_D, z_D, c='black', marker='o', ms=6, label="B")[0]

isLegendSet = False

def anima(i):
    S_CD.set_data_3d([x_D, X_C[i]], [y_D, Y_C[i]], [z_D, z_C])
    S_OQ.set_data_3d([x_O, x_Q], [y_O, y_Q], [z_O, z_Q])
    S_AB.set_data_3d([X_A[i], X_B[i]], [Y_A[i], Y_B[i]], [Z_A[i], Z_B[i]])
    O.set_data_3d(x_O, y_O, z_O)
    Q.set_data_3d(x_Q, y_Q, z_Q)
    A.set_data_3d(X_A[i], Y_A[i], Z_A[i])
    B.set_data_3d(X_B[i], Y_B[i], Z_B[i])
    C.set_data_3d(X_C[i], Y_C[i], z_C)
    D.set_data_3d(x_D, y_D, z_D)

    global isLegendSet;
    if not isLegendSet:
        ax.legend()
        isLegendSet = True

    return S_AB, S_CD, S_OQ, O, Q, A, B, C, D


anim = FuncAnimation(fig, anima, frames=len(T), interval=10)

plt.show()