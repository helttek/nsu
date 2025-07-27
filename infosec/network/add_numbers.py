import sys
import subprocess

server = "nsuctf.ru"
port = 30002

process = subprocess.Popen(
    ["nc", server, str(port)],
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    text=True
)

fin = open("input", "w")
fin.close()
fout = open("output", "w")
fout.close()

fin = open("input", "w")
fout = open("output", "w")

for line in process.stdout:
    fin.write(line[:-4] + '\n')
    fin.flush()
    if (line == 'Not a number, bye' or line == 'Wrong answer, bye'):
        fout.write('server said: '+ line)
        fout.flush()
        break
    try:
        ans = eval(line[:-4])
    except SyntaxError:
        fout.write('a syntax error occured: '+ line +'\n')
        break
    fout.write(str(ans) + '\n')
    fout.flush()
    process.stdin.write(str(ans) + '\n')
    process.stdin.flush()

fin.close()
fout.close()