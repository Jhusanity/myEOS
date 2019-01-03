CC = gcc

window:
%: %.c
	$(CC) $< -o $@ -lpthread -lm

rpi:
%: %.c
	$(CC) $< -o $@ -lpthread -lwiring -lm
