Notes on HH model
---

Nernst potential:

Nernst equation measure density of ions at a given temperature,and given potential at x:

p = exp(-qu(x)/kT)

between two points in space we have:
 n(x1)        [   q(u(x1) - qu(x2) ]
------  = exp|  - ---------------- |
 n(x2)        [          kT        ]


And also
  Delta U = (kT / q ) ln (n2 / n1)  <--- Nernst potential


At equilibirum: E (Sodium) = +67 mV: Interior have a positive potential compared to membrane.
                E(Potatium) = -83 mV.

Resting potential of Membrane is around -65 mV.
At Rest: Potatium ions flows out while Sodium flows in.

Each Ion is modelised as a Batterie + Resistance (Can be variable)

I(t) = Ic(t) + Sum (Ik(t))

HH model: 3 channels: Potatium K, Sodium Na, and a leakage channel with fixed resistance R.

Charging current is C du/dt = - (I_k(t)+ I_Na(t) + I_l(t)) + I(t))


Leakage channel conductance g_L = 1/R. => I_l = gL (u-El).

Sodium channel: 1/R_Na  = g_Na m^3 * h. m is activation (openning) h is innactivation(closing).
Potatium channel: 1/R_k = g_K n^4, n describe activation channel


C = 1 microF/cm^2


x | E   | g  |
--+-----+----+
Na|  55 | 40 |
K | -77 | 35 |
L | -65 | 0.3|

Pyramidal neuron on the cortex

                        dx       - 1
m, n, and h evolves as ----    = ----- [x - x0(u)]   where x is m, n, or h
                        dt       Tau(u)


===> analytical solutions exists for m, h, t.
