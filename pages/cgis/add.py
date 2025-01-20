from time import sleep
import sys

if __name__ == '__main__':
	line = sys.stdin.read()

	line = line.split("&")
	n1 = int(line[0].split("=")[1])
	n2 = int(line[1].split("=")[1])

	print(f"La suma es: {n1 + n2}")

	sys.exit(0)