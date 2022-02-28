import opensimplex
from math import sqrt
import random
import normal_palette_gen

chunkScale = 32
normalPoints = normal_palette_gen.fibonacciPoints()

def naiveQuantizeNormal(v):
    m = sqrt(pow(v[0], 2) + pow(v[1], 2) + pow(v[2], 2));

    if (m == 0):
        return 2 + 2 * 5 + 2 * 25;

    vx = v[0] / m * 2;
    vy = v[1] / m * 2;
    vz = v[2] / m * 2;
    
    x = int(round(vx)) + 2;
    y = int(round(vy)) + 2;
    z = int(round(vz)) + 2;

    return x + y * 5 + z * 25;

def fibQuantizeNormal(v):
    # normalize
    m = sqrt(pow(v[0], 2) + pow(v[1], 2) + pow(v[2], 2));
    if m == 0:
        return 0
    nx = v[0] / m
    ny = v[1] / m
    nz = v[2] / m

    lowestD = 999
    best = 0
    for i in range(len(normalPoints)):
        p = normalPoints[i]
        d = sqrt(pow(nx - p[0], 2) + pow(ny - p[1], 2) + pow(nz - p[2], 2))
        if d < lowestD:
            lowestD = d
            best = i
    return best

def adjacentVoxPoints(x, y, z):
    points = [(0, 0, 0)] * 3 * 3 * 3
    i = 0
    for zd in range(-1, 2):
        for yd in range(-1, 2):
            for xd in range(-1, 2):
                points[i] = (x + xd, y + yd, z + zd)
                i += 1
    return points

def gradientFromAdjFloats(adjFloats):
    gx = 0
    gy = 0
    gz = 0
    for i in range(3 * 3 * 3):
        dx = i % 3 - 1
        dy = i // 3 % 3 - 1
        dz = i // 9 % 3 - 1
        gx -= dx * adjFloats[i]
        gy -= dy * adjFloats[i]
        gz -= dz * adjFloats[i]
    return (gx, gy, gz)
        

def simplexFloatSample(x, y, z, genInfo):
    noiseScale = genInfo["simplex_scale"]
    return opensimplex.noise3(x / noiseScale, y / noiseScale, z / noiseScale)

def sampleVox(x, y, z, genInfo):
    if genInfo["type"] == "sphere":
        radius = genInfo["sphere_radius"]
        distanceFromCenter = sqrt( \
            (x - radius) ** 2 \
            + (y - radius) ** 2 \
            + (z - radius) ** 2)
        colIndex = 0
        normalV = (x - radius, y - radius, z - radius)
        normalIndex = fibQuantizeNormal(normalV)
        if distanceFromCenter <= radius:
            return (colIndex, normalIndex)
        else:
            return None
    elif genInfo["type"] == "simplex":
        n = simplexFloatSample(x, y, z, genInfo)
        if n > 0:
            colIndex = 0
            adj = adjacentVoxPoints(x, y, z)
            adjFloats = [0.0] * 3 * 3 * 3
            for i in range(3 * 3 * 3):
                adjFloats[i] = simplexFloatSample(adj[i][0], adj[i][1], adj[i][2], genInfo)

            normalV = gradientFromAdjFloats(adjFloats)
            normalIndex = fibQuantizeNormal(normalV)
            return (colIndex, normalIndex)
        else:
            return None
    else:
        return None

def generateChunk(f, cx, cy, cz, genInfo, xo, yo, zo):
    skip = 0

    f.write("CHUNK {} {} {}\n".format(cx + xo, cy + yo, cz + zo))
    for i in range(chunkScale * chunkScale * chunkScale):
        x = i % chunkScale + cx * chunkScale
        y = i // chunkScale % chunkScale + cy * chunkScale
        z = i // chunkScale // chunkScale % chunkScale + cz * chunkScale
        vox = sampleVox(x, y, z, genInfo)
        if vox == None:
            skip += 1
        else:
            if skip > 0:
                f.write("-{}\n".format(skip))
                skip = 0
            colIndex, normalIndex = vox
            f.write("{} {}\n".format(colIndex, normalIndex))
        if i % (chunkScale * chunkScale) == 0:
            print("\tthis chunk {:.1f}%".format(100 * i / (chunkScale * chunkScale * chunkScale)))

    if skip > 0:
        f.write("-{}\n".format(skip))

def beginFile(filename, palette):
    f = open(filename, "w")
    f.write("CHUNK_SCALE={}\n".format(chunkScale))
    for i in range(256):
        col = palette[i]
        f.write("{} {} {}\n".format(col[0], col[1], col[2]))
    return f

def generateObject(f, genInfo, xo = 0, yo = 0, zo = 0):
    xChunkSize = (genInfo["xSize"] - 1) // chunkScale + 1
    yChunkSize = (genInfo["ySize"] - 1) // chunkScale + 1
    zChunkSize = (genInfo["zSize"] - 1) // chunkScale + 1
    countDone = 0
    for cx in range(xChunkSize):
        for cy in range(yChunkSize):
            for cz in range(zChunkSize):
                print("chunk {} / {}".format(countDone + 1, xChunkSize * yChunkSize * zChunkSize))
                generateChunk(f, cx, cy, cz, genInfo, xo, yo, zo)
                countDone += 1

def setColor(palette, index, rgb):
    palette[index] = rgb

def sphere(radius):
    genInfo = {}
    genInfo["xSize"] = radius * 2
    genInfo["ySize"] = radius * 2
    genInfo["zSize"] = radius * 2
    genInfo["type"] = "sphere"
    genInfo["sphere_radius"] = radius
    return genInfo

def simplex(size, noiseScale):
    genInfo = {}
    genInfo["xSize"] = size; 
    genInfo["ySize"] = size; 
    genInfo["zSize"] = size; 
    genInfo["type"] = "simplex"
    genInfo["simplex_scale"] = noiseScale
    return genInfo


green = (109, 219, 35)
palette = [(255, 0, 255)] * 256
setColor(palette, 0 , green)

f = beginFile("object1.svo", palette)

sphereGenInfo = sphere(16)
noiseGenInfo = simplex(64, 15)
generateObject(f, sphereGenInfo, 0, 0, 0)
generateObject(f, noiseGenInfo, 3, 0, 0)

f.close()
