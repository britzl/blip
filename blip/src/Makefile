CPP_FILES = sfxr.cpp
OUT = out

init:
	mkdir -p $(OUT)

sfxr: init
	$(CC) $(CPP_FILES) -o $(OUT)/sfxr -Wall -Werror -std=c++11 -lstdc++

clean:
	$(RM) -rf *.o
	$(RM) -rf $(OUT)
