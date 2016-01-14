ifeq ($(shell uname), Linux)
	EXE_SUFFIX =
	OSFLAG = -D LINUX
	CLEAN = rm -f
else ifeq ($(shell uname), Darwin)
	EXE_SUFFIX =
	OSFLAG = -D MAC_OS
	CLEAN = rm -f
else
	EXE_SUFFIX = .exe
	OSFLAG = -D WIN
	CLEAN = del
endif


CC=	g++
PROGRAM= oak_sign$(EXE_SUFFIX)
SOURCE= oak_sign.cpp
all: $(PROGRAM)

$(PROGRAM): $(SOURCE)
	$(CC) $(OSFLAG) $(SOURCE) -o $(PROGRAM)

clean:
	$(CLEAN) $(PROGRAM)
