def to_str(n: int) -> str:
    return ''.join([chr((n >> 8 * i) & 0xFF) for i in reversed(range(len(str(n)[2:])))])

def to_hex(s: str) -> int:
    return int(f'0x{"".join(str(hex(ord(c)))[2:] for c in s)}', 16)

#print('%#x' % to_hex('Hello, I\'m Alex!'))
print(to_str(0x48656c6c6f2c2049276d20416c657821))
