CC=gcc

all: keepassxc-tui outputter

keepassxc-tui: keepass-cli.c
	$(CC) keepass-cli.c -o keepass

outputter: outputter.c
	$(CC) outputter.c -o outputter
