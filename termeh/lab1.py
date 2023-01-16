import matplotlib.pyplot as plt
import numpy as np
import sympy as sp
from matplotlib.animation import FuncAnimation

t, T = sp.Symbol('t'), np.linspace(1, 5, 5000)
r, phi = sp.cos(6 * t), t + 0.2 * sp.cos(3 * t)
x, y = r * sp.cos(phi), r * sp.sin(phi)

Vx, Vy = sp.diff(x), sp.diff(y)
V = sp.sqrt(Vx ** 2 + Vy ** 2)

Wx, Wy = sp.diff(Vx), sp.diff(Vy)
W = sp.sqrt(Wx ** 2 + Wy ** 2)

Wtan = sp.diff(V)
Wtanx, Wtany = (Vx / V) * Wtan, (Vy / V) * Wtan

Wnor = sp.sqrt(W ** 2 - Wtan ** 2)
Wnorx, Wnory = (Wx - Wtanx) / Wnor, (Wy - Wtany) / Wnor

CurvRad = V ** 2 / Wnor

F_func = [sp.lambdify(t, i) for i in [x, y, Vx, Vy, Wx, Wy, Wnorx, Wnory, V, W, Wtan, Wnor, CurvRad]]
[X, Y, Vx, Vy, Wx, Wy, Wnorx, Wnory, V, W, Wtan, Wnor, CurvRad] = [func(T) for func in F_func]

fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
ax.axis('equal'), ax.set_xlabel('x'), ax.set_ylabel('y'), ax.plot(X, Y), \
    ax.set(xlim=[-1, 1], ylim=[-1, 1])

P = ax.plot(X[0], Y[0], marker='.', color = "black")[0]
isLegendSet = False


def anima(i):
    P.set_data(X[i], Y[i])
    Vvec = ax.arrow(X[i], Y[i], Vx[i], Vy[i], width=0.02, color='red', label='- скорость')
    Wvec = ax.arrow(X[i], Y[i], Wx[i], Vy[i], width=0.02, color='blue', label='- ускорение')
    Cvec = ax.arrow(X[i], Y[i], -(X[i] + ((Vy[i] * CurvRad[i]) / V[i])), -(Y[i] - ((Vx[i] * CurvRad[i]) / V[i])), width=0.02, color="green",
                 label='- кривизна')
    # Cvec = ax.arrow(X[i], Y[i], -(X[i] + (((Y[i] + Vy[i]) * CurvRad[i]) / ((Y[i] + Vy[i]) ** 2 + (X[i] + Vx[i]) ** 2) ** 0.5)), -(Y[i] - (((X[i] + Vx[i]) * CurvRad[i]) / ((Y[i] + Vy[i]) ** 2 + (X[i] + Vx[i]) ** 2) ** 0.5)), width=0.02, color="green",
    #              label='- кривизна')
    
    global isLegendSet;
    if not isLegendSet:
        ax.legend()
        isLegendSet = True

    return P, Vvec, Wvec, Cvec


anim = FuncAnimation(fig, anima, frames=len(T), interval=10, blit=True)

plt.show()




