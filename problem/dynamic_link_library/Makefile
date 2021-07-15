CC=g++
SRC=./
OUT_NAME=dll.so
OUT_DEBUG_PATH=./bin/debug/
OUT_RELEASE_PATH=./bin/release/

DEBUG_PATH=obj/debug/
RELEASE_PATH=obj/release/

SRC_FILE=$(wildcard $(src) *.c)
ALL_FILE=$(notdir $(SRC_FILE))
OBJ_FILE=$(patsubst %.c,%.o,$(ALL_FILE))

DEBUG_CXXFLAGS= -g -ggdb3 -Wall -fPIC -shared
RELEASE_CXXFLAGS= -O3 -ggdb3 -Wall -fPIC -shared



OUT_DEBUG_FILE=$(OUT_DEBUG_PATH)$(OUT_NAME)
OUT_RELEASE_FILE=$(OUT_RELEASE_PATH)$(OUT_NAME)

OBJ_DEBUG_FILE =$(addprefix $(SRC)$(DEBUG_PATH),$(OBJ_FILE))
OBJ_RELEASE_FILE =$(addprefix $(SRC)$(RELEASE_PATH),$(OBJ_FILE))

debug:pre_debug $(OUT_DEBUG_FILE)
release:pre_release $(OUT_RELEASE_FILE)

	
pre_debug:
	mkdir -p $(DEBUG_PATH)
	mkdir -p $(OUT_DEBUG_PATH)

pre_release:
	mkdir -p $(RELEASE_PATH)
	mkdir -p $(OUT_RELEASE_PATH)
	
$(OUT_DEBUG_FILE):$(OBJ_DEBUG_FILE)
	$(CC)  $(DEBUG_CXXFLAGS) $^ -o $@ 

$(SRC)$(DEBUG_PATH)%.o:%.c
	$(CC) -c $(DEBUG_CXXFLAGS) $(SRC)$< -o $@




