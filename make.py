#!/usr/bin/python3
from sys import argv
from os import system

if len(argv)<2: exit()
else:
  arg = argv[1]
  if arg == "curses":
    system("cc -lcurses curses.c -o curses && ./curses")
  elif arg == "client":
    system("cc client.c -o client && ./client "+argv[2])
  elif arg == "server":
    system("cc server.c -o server && ./server")
  elif arg == "clean":
    system("rm server \n rm log.txt \n rm client \n rm curses")
