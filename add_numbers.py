import subprocess


def Parse2add(line):
    idx = line.find(b" + ")
    if (idx == -1):
        print("bad parsing")
        return False
    first = int(line[0:idx-1])
    eq = line.find(b" =")
    second = int(line[idx+3:eq])
    print(first+second)
    proc.stdin.write((first+second).to_bytes(length=10000000))
    proc.stdin.flush()
    return True


proc = subprocess.Popen(["nc nsuctf.ru 30002"], shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE)
while True:
    line = proc.stdout.readline()
    if (Parse2add(line) == False):
        print("bad input from server")
        break

proc.terminate()
