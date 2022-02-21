import opensimplex
from math import sqrt
import random

chunkScale = 32

def quantizeNormal(v):
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

def sampleVox(x, y, z, genInfo):
    if genInfo["type"] == "sphere":
        radius = genInfo["sphere_radius"]
        distanceFromCenter = sqrt( \
            (x - radius) ** 2 \
            + (y - radius) ** 2 \
            + (z - radius) ** 2)
        colIndex = 0
        normalV = (x - radius, y - radius, z - radius)
        normalIndex = quantizeNormal(normalV)
        if distanceFromCenter <= radius:
            return (colIndex, normalIndex)
        else:
            return None
    else:
        return None

def generateChunk(f, cx, cy, cz, genInfo):
    skip = 0

    f.write("CHUNK {} {} {}\n".format(cx, cy, cz))
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

    if skip > 0:
        f.write("-{}\n".format(skip))

def generateObject(filename, genInfo):
    f = open(filename, "w")
    f.write("CHUNK_SCALE={}\n".format(chunkScale))
    for i in range(256):
        col = genInfo["palette"][i]
        f.write("{} {} {}\n".format(col[0], col[1], col[2]))
    xChunkSize = (genInfo["xSize"] - 1) // chunkScale + 1
    yChunkSize = (genInfo["ySize"] - 1) // chunkScale + 1
    zChunkSize = (genInfo["zSize"] - 1) // chunkScale + 1
    countDone = 0
    for cx in range(xChunkSize):
        for cy in range(yChunkSize):
            for cz in range(zChunkSize):
                fractionDone = countDone / (xChunkSize * yChunkSize * zChunkSize)
                print("{:.1f}%".format(fractionDone * 100))
                generateChunk(f, cx, cy, cz, genInfo)
                countDone += 1
    f.close()

def GenInfo_init(genInfo):
    global chunkScale
    genInfo["palette"] = [(255, 0, 255)] * 256

def GenInfo_setColor(genInfo, index, rgb):
    genInfo["palette"][index] = rgb

def sphere(radius, color):
    genInfo = {}
    GenInfo_init(genInfo)
    GenInfo_setColor(genInfo, 0, color)
    genInfo["xSize"] = radius * 2
    genInfo["ySize"] = radius * 2
    genInfo["zSize"] = radius * 2
    genInfo["type"] = "sphere"
    genInfo["sphere_radius"] = radius
    return genInfo

green = (109, 219, 35)
genInfo = sphere(64, green)
generateObject("object1.svo", genInfo)
