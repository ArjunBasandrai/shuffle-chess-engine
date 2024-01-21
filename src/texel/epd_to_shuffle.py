with open('quiet-labeled.epd', 'r') as f:
    lines = f.readlines()

fens = []
results = []

for line in lines:
    fen, result, _ = (line.split('"'))
    fens.append(fen)
    if result == '1-0':
        result = '1'
    elif result == '0-1':
        result = '0'
    else:
        result = '0.5'
    results.append(result)

c = 1
with open("fen.txt", 'w') as f:
    for fen in fens:
        f.write(fen + '\n')

with open("result.txt", 'w') as f:
    for result in results:
        f.write(result + '\n')
