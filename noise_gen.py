import opensimplex

f = open("object1.ply", "w")
f.write("end_header\r\n")

opensimplex.seed(100)

w = 128
h = 128
d = 128

scale = 20

for v in range(w * h * d):
    x = v % w
    y = v // w % h
    z = v // w // h
    r = 100 if y % 4 == 0 else 110
    g = 100 if y % 4 == 0 else 110
    b = 100 if y % 4 == 0 else 110

    n = opensimplex.noise3(x / scale, y / scale, z / scale)
    if n > 0:
        l = "{} {} {} {} {} {}\r\n".format(x, y, z, r, g, b);
        f.write(l)

    if v % (w * h) == 0:
        print("{:.1f}%".format((v * 100) / (w * h * d)))

