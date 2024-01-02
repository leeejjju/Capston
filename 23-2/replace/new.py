fd = open('new.txt', 'w')
count = 0
while True:
	if (count%100):
		fd.write(str(count))
		count = count + 1
fd.close()
