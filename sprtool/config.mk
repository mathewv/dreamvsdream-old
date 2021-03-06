NAME		:= sprtool
VERSION		:= 0.2
ARCH		?= 64
CFG		?= release

ifndef PLATFORM
	ifeq ($(OS),Windows_NT)
		PLATFORM := w32
	else
		UNAME_S := $(shell uname -s)
		ifeq ($(UNAME_S),Linux)
			PLATFORM := linux
		else ifeq ($(UNAME_S),Darwin)
			PLATFORM := osx
		endif
	endif
endif

ifeq ($(CFG),debug)
	CXXFLAGS	+= -DDEBUG -g
	LDFLAGS		+= -g
else
	CXXFLAGS	+= -O3
	LDFLAGS		+= -O3
endif

ifeq ($(PLATFORM),linux)
	TARGET		:= $(NAME)-$(ARCH)
	CXXFLAGS	:= -m$(ARCH)
	LDFLAGS		:= -m$(ARCH) -lGL

	CXX		:= clang++
	LD		:= clang++
else ifeq ($(PLATFORM),osx)
	TARGET	:= $(NAME)
	CXXFLAGS	:= -mmacosx-version-min=10.6 -isysroot /Developer/SDKs/MacOSX10.6.sdk -arch i386 -arch x86_64 -stdlib=libc++
	LDFLAGS		:= -mmacosx-version-min=10.6 -isysroot /Developer/SDKs/MacOSX10.6.sdk -arch i386 -arch x86_64 -stdlib=libc++ -framework OpenGL

	CXX		:= /opt/clang/bin/clang++
	LD		:= /opt/clang/bin/clang++
else ifeq ($(PLATFORM),w32)
	TARGET		:= $(NAME)-$(ARCH).exe
	CXXFLAGS	:= 
	LDFLAGS		:= -mwindows -lmingw32 -lSDL2main -lshlwapi -lopengl32

	ifeq ($(ARCH),32)
		CXX	:= i686-w64-mingw32-g++
		LD	:= i686-w64-mingw32-g++
		WINDRES	:= i686-w64-mingw32-windres
	else
		CXX	:= x86_64-w64-mingw32-g++
		LD	:= x86_64-w64-mingw32-g++
		WINDRES	:= x86_64-w64-mingw32-windres
	endif
endif

# flags
CXXFLAGS	+= -DVERSION=\"$(VERSION)\" -DSPRTOOL -DNO_ZLIB -std=c++11
LDFLAGS		+= -lSDL2 -lpng -std=c++11

