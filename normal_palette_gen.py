from math import sqrt

print("const vec3 normalPalette[] = {")
for i in range(125):
    x = i % 5 - 2
    y = (i // 5) % 5 - 2
    z = (i // 25) % 5 - 2

    magnitude = sqrt(x**2 + y**2 + z**2)
    nx = 0
    ny = 0
    nz = 0
    if magnitude != 0:
        nx = x / magnitude
        ny = y / magnitude
        nz = z / magnitude
    print("\tvec3({}, {}, {}),".format(nx, ny, nz))
print("};")
