import functools

@functools.lru_cache()
def lev(a, b):
    len_a = len(a)
    len_b = len(b)
    
    if len_b == 0: return len_a
    if len_a == 0: return len_b
    
    if a[0] == b[0]:
        return lev(a[1:], b[1:])
    
    return 1 + min([ lev(a[1:], b), lev(a, b[1:]), lev(a[1:], b[1:]) ])

commands = {
    'push': 0,
    'pull': 0,
    'commit': 0,
    'update': 0,
    'new': 0,
    'init': 0
}

text = input()
    
commands = { c: lev(c, text) for c in commands.keys() }
    
val = min(commands, key=commands.get)

if commands[val] == 0:
    print(val)
else:
    print('Incorrect command, did you mean "%s"?' % val)
