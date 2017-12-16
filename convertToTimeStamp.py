
def keyToTimeStamp(key):
        PUSH_CHARS ='-0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz'
        idString = key[0:8]
        time = 0
        for letter in idString:
                time = time * 64 + PUSH_CHARS.index(letter)

        return time

print (keyToTimeStamp("-L0RhEU-MG5lfV35Ut_c"))
