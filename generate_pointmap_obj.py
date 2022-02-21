import opensimplex
from math import sqrt

def write_header(file):
    file.write("end_header\r\n")

def write_vox(file, x, y, z, r, g, b):
    line = "{} {} {} {} {} {}\r\n".format(x, y, z, r, g, b);
    file.write(line)

def print_progress(curr, total):
    print("{:.1f}%".format(100 * curr / total))

def generate_3d_noise(f, w, h, d, noiseScale = 10, seed = 0):
    opensimplex.seed(seed)

    for v in range(w * h * d):
        x = v % w
        y = v // w % h
        z = v // w // h

        r = 109
        g = 219
        b = 35

        n = opensimplex.noise3(x / noiseScale, y / noiseScale, z / noiseScale)
        if n > 0:
            write_vox(f, x, y, z, r, g, b)

        if v % (w * h) == 0:
            print_progress(v, w * h * d)

def generate_sphere(s):
    volume = s * s * s
    for v in range(volume):
        x = v % s
        y = v // s % s
        z = v // s // s

        distanceFromCenter = sqrt( \
            (x - s / 2) ** 2 + \
            (y - s / 2) ** 2 + \
            (z - s / 2) ** 2 \
        );

        if (distanceFromCenter < s / 2):
            write_vox(f, x, y, z, 109, 219, 35)
        if v % (s * s) == 0:
            print_progress(v, volume)

def generate_cube(f, s):
    volume = s * s * s
    for v in range(volume):
        x = v % s
        y = v // s % s
        z = v // s // s
        write_vox(f, x, y, z, 109, 219, 35)
        if v % (s * s) == 0:
            print_progress(v, volume)

f = open("object1.ply", "w")
write_header(f)
#generate_3d_noise(f, 64, 64, 64, 15)
#generate_cube(f, 32)
generate_sphere(128)
f.close()
