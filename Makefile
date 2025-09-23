CC=gcc

all: keepassxc-tui

#keepassxc-tui: keepass-cli.c
#	$(CC) keepass-cli.c -o keepass
#
#outputter: outputter.c
#	$(CC) outputter.c -o outputter

keepassxc-tui: keepassxc-tui.c
	$(CC) keepassxc-tui.c -o keepassxc-tui
