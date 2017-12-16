
def keyToTimeStamp(key):
	PUSH_CHARS = '-0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz'

	s1 = key[0:8]

	time = 0

	for letter in s1:
		time = time * 64 + PUSH_CHARS.index(letter)

	return time 
