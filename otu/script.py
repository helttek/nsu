# first PID

# n = 4
# T1 = 10
# T0 = 1.23
# Ta = 4.46 * T0
# alpha = 0.25
# Temk = 1.43 * T0
# tau = Temk + T1
# Tint = 4 * T0 + T1
# K = 1 / (1.552 * (tau / Tint) + 0.078)
# Ti = 0.186 * tau + 0.532 * Ta
# Td = alpha * Ti
# Tc = Td / 8

# print("T: " + str(T1))
# print("K: " + str(K))
# print("Ti: " + str(Ti))
# print("Td: " + str(Td))
# print("Tc: " + str(Tc))

# second PID

n = 4
T1 = 2
T0 = 1.23
# Ta = 4.46 * T0
alpha = 0.4
Temk = 1.43 * T0
tau = Temk + T1
Tint = 4 * T0 + T1
K = 1 / (2.766 * (tau / Tint) - 0.521)
Ti = (-0.150) * tau + 0.552 * Tint
Td = alpha * Ti
Tc = Td / 8

print("T: " + str(T1))
print("K: " + str(K))
print("Ti: " + str(Ti))
print("Td: " + str(Td))
print("Tc: " + str(Tc))
