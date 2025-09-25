CC=clang

all: keepassxc-tui

#keepassxc-tui: keepass-cli.c
#	$(CC) keepass-cli.c -o keepass
#
#outputter: outputter.c
#	$(CC) outputter.c -o outputter

keepassxc-tui: keepassxc-tui.c
	$(CC) -Wall -Wextra ./keepassxc-tui.c -o keepassxc-tui
