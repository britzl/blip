CPP_FILES = src/sfxr.c
OUT = out

init:
	mkdir -p $(OUT)

sfxr: init
	$(CC) $(CPP_FILES) -I include -o $(OUT)/sfxr -Wall -Werror

clean:
	$(RM) -rf *.o
	$(RM) -rf $(OUT)
