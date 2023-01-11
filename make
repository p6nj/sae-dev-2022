#!/usr/bin/python
from sys import argv
from os import system

def main():
  if len(argv)<2: return
  else:
    arg = argv[1]
    if arg == "curses":
      system("cc -lcurses curses.c -o curses && ./curses && rm curses")
    elif arg == "client":
      system("cc client.c -o client && ./client "+argv[2]+" && rm client")
    elif arg == "server":
      system("cc server.c -o server && ./server && rm server")

main()
