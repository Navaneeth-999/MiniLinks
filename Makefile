all:
	gcc src/server.c src/mongoose.c -o server -O2