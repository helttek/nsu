import math

print("---------------------------------")
print("PI (hodograph)")

a1 = 0.418
Ka1 = 20 * math.log10(1 / a1)
print("Ka: " + str(Ka1))

adj1 = 0.558
phi1 = math.degrees(math.acos(adj1))
print("phi: " + str(phi1))

print("---------------------------------")
print("PID (hodograph)")

a2 = 0.518
Ka2 = 20 * math.log10(1 / a2)
print("Ka: " + str(Ka2))

adj2 = 0.901
phi2 = math.degrees(math.acos(adj2))
print("phi: " + str(phi2))
print("---------------------------------")

print("PI (Bode)")

a1 = 0.422
Ka1 = 20 * math.log10(1 / a1)
print("Ka: " + str(Ka1))

phi1 = 56
print("phi: " + str(phi1))

print("---------------------------------")
print("PID (Bode)")

a2 = 0.531
Ka2 = 20 * math.log10(1 / a2)
print("Ka: " + str(Ka2))

phi2 = 21
print("phi: " + str(phi2))
print("---------------------------------")
