import opensimplex

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
    
        n = opensimplex.noise3(x / noiseScale, y / noiseScale, z / noiseScale)
        if n > 0:
            write_vox(f, x, y, z, r, g, b)
    
        if v % (w * h) == 0:
            print_progress(v, w * h * d)

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
generate_3d_noise(f, 128, 128, 128, 10)
#generate_cube(f, 32)
f.close()
