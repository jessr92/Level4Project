import os
import sys

fileList = []
rootdir = sys.argv[1]

for root, subFolders, files in os.walk(rootdir):
	for file in files:
		fileList.append(os.path.join(root, file))

for file in fileList:
	if file.endswith(".txt"):
		filename = file.partition('.')[0]
		command = "((head;tail) < \"" + file + "\") > \"" + filename + "Summary.txt\""
		print command
		os.system(command)

