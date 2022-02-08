import opensimplex

f = open("object1.ply", "w")
f.write("end_header\r\n")

opensimplex.seed(123)

w = 128
h = 128
d = 128

scale = 10

for v in range(w * h * d):
    x = v % w
    y = v // w % h
    z = v // w // h

    sr = opensimplex.noise3(x / 5, y / 5, z / 5)
    if sr > 0:
        r = 109
        g = 219
        b = 35
    else:
        r = 84
        g = 240
        b = 14

    sh = opensimplex.noise2(x / 5, y / 5)
    r = 101 if y % 4 == sh else r
    g = 184 if y % 4 == sh else g
    b = 47 if y % 4 == sh else b

    n = opensimplex.noise3(x / scale, y / scale, z / scale)
    if n > 0:
        l = "{} {} {} {} {} {}\r\n".format(x, y, z, r, g, b);
        f.write(l)

    if v % (w * h) == 0:
        print("{:.1f}%".format((v * 100) / (w * h * d)))

