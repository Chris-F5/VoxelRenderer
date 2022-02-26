import math

language = "c"

def addPoint(x, y, z):
    global language
    print("\tvec3({}, {}, {}),".format(x, y, z))

def printCArr(points):
    print("const vec3 normalPalette[] = {")
    for p in points:
        x, y, z = p
        print("\tvec3({}, {}, {}),".format(x, y, z))
    print("};")

def naivePoints():
    points = []
    for i in range(125):
        x = i % 5 - 2
        y = (i // 5) % 5 - 2
        z = (i // 25) % 5 - 2

        magnitude = math.sqrt(x**2 + y**2 + z**2)
        nx = 0
        ny = 0
        nz = 0
        if magnitude != 0:
            nx = x / magnitude
            ny = y / magnitude
            nz = z / magnitude
        points.append((nx, ny, nz))
    return points

# found this on stack overflow https://stackoverflow.com/a/26127012/11046811
def fibonacciPoints(samples = 256):
    points = []
    phi = math.pi * (3 - math.sqrt(5)) # golden angle in radians

    for i in range(samples):
        y = 1 - (i / float(samples - 1)) * 2  # y goes from 1 to -1
        radius = math.sqrt(1 - y * y)  # radius at y

        theta = phi * i  # golden angle increment

        x = math.cos(theta) * radius
        z = math.sin(theta) * radius
        points.append((x, y, z))
    return points

if __name__ == "__main__":
    printCArr(fibonacciPoints())
