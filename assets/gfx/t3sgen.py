# Generates .t3s files for use with tex3ds
# Made by Nanquitas

import os

def comparator(f1, f2):
	f1id = int(f1[f1.rfind("_") + 1 : -4])
	f2id = int(f2[f2.rfind("_") + 1 : -4])

	return f1id - f2id

ofiles = []

for path, subdirs, files in os.walk("."):
	ofiles = []
	for filename in files:
		if filename.endswith(".png"):
			f = os.path.join(path, filename.replace('\\','/'))
			ofiles.append(f)
	try:
		ofiles.sort(comparator)
	except:
		print "couldn't sort the files"

	if len(ofiles) > 0:
		a = open(path[path.rfind('\\') + 1 :] + "_ss.t3s", "w+")
		a.write("--atlas -f rgba8888 -z auto" + os.linesep) 
		for file in ofiles:
			a.write(file + "\n")
		a.close()

