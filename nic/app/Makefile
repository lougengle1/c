
CXX=g++
AR=ar
OUT_NAME=libsld_slnic.a

OUT_PATH=./lib/linux/
SRC_PATH=./ ./device_handle/ ./tool_lib/
OBJ_PATH=./obj/linux/
OUT_INTERFACE_PATH=../../include/
OUT_INTERFACE_DIR=sse_hard_local_api/

publish_shfe_local_path=../publish/
publish_shfe_local_inc_path=../publish/interface
publish_local_demo_path=${publish_shfe_local_path}demo_bin
publish_shfe_local_hard_path=$(publish_shfe_local_path)shfe_local_hard

DEBUG_PATH=debug/
RELEASE_PATH=release/

WORKING_PATH=../../../../../../common_libs_pub/

COMMON_INC_PATH=-I $(WORKING_PATH)/include/linux/
PUBLISH_INC_LINUX=../../../include/linux/sse/e

PUBLISH_LIB_LINUX=../../lib/linux/

COMMIT_MSG="auto commit"

SVN_VERSION:=$(shell svn info | grep ' ' -m 10 | tail -n 1 | cut -d' ' -f2)
COMP_DATE:=$(shell date)

LIB_DEBUG_FILE= -lpthread -ldl \
				$(WORKING_PATH)/lib/linux/centos6.4/debug/shengli_xml.a \
				$(WORKING_PATH)/lib/linux/centos6.4/debug/shengli_tool.a  \


LIB_RELEASE_FILE=-lpthread -ldl \
				$(WORKING_PATH)/lib/linux/centos6.4/debug/shengli_xml.a \
				$(WORKING_PATH)/lib/linux/centos6.4/debug/shengli_tool.a  \


LIB_DEBUG_FILE=

LIB_RELEASE_FILE=

SRC_FILE=$(foreach SUB_DIR, $(SRC_PATH), $(wildcard $(SUB_DIR)*.cpp))
ALL_FILE=$(SRC_FILE)
OBJ_FILE=$(patsubst %.cpp,%.o,$(ALL_FILE))

#CSV_FILE=$(foreach SUB_DIR, $(SRC_PATH), $(wildcard $(SUB_DIR)*.csv))
#ALL_CSV_FILE=$(CSV_FILE)
#OBJ_FILE+=$(patsubst %.csv,%.o,$(ALL_CSV_FILE))

MK32=

DEBUG_CXXFLAGS= -DSL_DEBUG_DATA -g $(MK32) -Wall -Wno-format-security -Wno-format-security -fPIC -DSHENGLI_SOCK_EXPORTS -DSVN_VERSION='"$(SVN_VERSION)"' -DCOMP_DATE='"$(COMP_DATE)"'
RELEASE_CXXFLAGS=-O3 -DSL_DEBUG_DATA $(MK32) -Wall -Wno-format-security -Wno-format-security -fPIC -DSHENGLI_SOCK_EXPORT -DSVN_VERSION='"$(SVN_VERSION)"' -DCOMP_DATE='"$(COMP_DATE)"'

DEBUG_ARFLAGS=cqs
RELEASE_ARFLAGS=cqs

OUT_DEBUG_FILE=$(OUT_PATH)$(DEBUG_PATH)$(OUT_NAME)
OBJ_DEBUG_FILE=$(addprefix  $(OBJ_PATH)$(DEBUG_PATH),$(OBJ_FILE))

OUT_RELEASE_FILE=$(OUT_PATH)$(RELEASE_PATH)$(OUT_NAME)
OBJ_RELEASE_FILE=$(addprefix  $(OBJ_PATH)$(RELEASE_PATH),$(OBJ_FILE))

vpath %.cpp  ./linux/ 
vpath %.cpp  ./thread/

debug : pre_debug $(OUT_DEBUG_FILE)

release : pre_release $(OUT_RELEASE_FILE)

all : debug release test_d test_r

publish : 
	@mkdir -p $(publish_local_demo_path)
	@mkdir -p $(publish_shfe_local_hard_path)
	@mkdir -p $(publish_shfe_local_inc_path)
	@cp -r ./interface/*.h $(publish_shfe_local_inc_path)
	@cp $(OUT_RELEASE_FILE) ${publish_shfe_local_hard_path}
	@cp demo.cc ${publish_shfe_local_hard_path}
	$(CXX) $(DEBUG_CXXFLAGS) demo.cc $(OUT_RELEASE_FILE) -pthread -o ${publish_local_demo_path}/shfe_hard_local_demo

publish_all : publish_debug publish_release

commit : commit_debug

commit_all : commit_debug commit_release

pre_debug :
	-$(shell mkdir $(OBJ_PATH) -p)
	-$(shell mkdir $(OBJ_PATH)$(DEBUG_PATH) -p)
	-$(shell mkdir $(OUT_PATH) -p)
	-$(shell mkdir $(OUT_PATH)$(DEBUG_PATH) -p)
	
pre_release :
	-$(shell mkdir $(OBJ_PATH) -p)
	-$(shell mkdir $(OBJ_PATH)$(RELEASE_PATH) -p)
	-$(shell mkdir $(OUT_PATH) -p)
	-$(shell mkdir $(OUT_PATH)$(RELEASE_PATH) -p)

test_debug:
	g++ -O3 test_main.cc -I/home/user0/common_libs_pub/include/linux/ lib/linux/debug/libsld_slnic.a -pthread -lrt -o ./lib/linux/debug/test
	
test_release:
	g++ -O3 test_main.cc -I/home/user0/common_libs_pub/include/linux/ lib/linux/release/libsld_slnic.a -pthread -lrt -o ./lib/linux/release/test

$(OUT_DEBUG_FILE) : $(OBJ_DEBUG_FILE)
	@mkdir -p `dirname $@`
	@echo AR $@
	@$(AR) $(DEBUG_ARFLAGS) $@ $^ $(LIB_DEBUG_FILE)
	@rm -rf $(OUT_INTERFACE_PATH)$(OUT_INTERFACE_DIR)

$(OUT_RELEASE_FILE) : $(OBJ_RELEASE_FILE)
	@mkdir -p `dirname $@`
	@echo AR $@
	@$(AR) $(RELEASE_ARFLAGS) $@ $^ $(LIB_RELEASE_FILE)
	@rm -rf $(OUT_INTERFACE_PATH)$(OUT_INTERFACE_DIR)


$(OBJ_PATH)$(DEBUG_PATH)%.o : %.cpp
	@mkdir -p `dirname $@`
	@echo CXX $@
	@$(CXX) -c $(DEBUG_CXXFLAGS) $(DEBUG_DEF) $(COMMON_INC_PATH)  $< -o $@

	
$(OBJ_PATH)$(RELEASE_PATH)%.o : %.cpp
	@mkdir -p `dirname $@`
	@echo CXX $@
	@$(CXX) -c $(RELEASE_CXXFLAGS) $(RELEASE_DEF) $(COMMON_INC_PATH)  $< -o $@


$(OBJ_PATH)$(DEBUG_PATH)%.o:%.csv
	@mkdir -p `dirname $@`
	@echo OBJCPY $@
	@objcopy -I binary -O elf64-x86-64 -B i386:x86-64 $< $@

$(OBJ_PATH)$(RELEASE_PATH)%.o:%.csv
	@mkdir -p `dirname $@`
	@echo OBJCPY $@
	@objcopy -I binary -O elf64-x86-64 -B i386:x86-64 $< $@
	
publish_debug: debug
	-$(shell mkdir $(PUBLISH_LIB_LINUX)$(DEBUG_PATH) -p)
	-$(shell cp $(OUT_PATH)$(DEBUG_PATH)$(OUT_NAME)  $(PUBLISH_LIB_LINUX)$(DEBUG_PATH)$(OUT_NAME) )
	-$(shell g++ -O3 test_main.cc -I /home/user0/common_libs_pub/include/linux/ lib/linux/release/libsld_slnic.a -pthread -lrt -o $(OUT_PATH)$(DEBUG_PATH)test)
	@echo "linux publish debug finish, but not commit!"

publish_release: release
	-$(shell mkdir $(PUBLISH_LIB_LINUX)$(RELEASE_PATH) -p)
	-$(shell cp $(OUT_PATH)$(RELEASE_PATH)$(OUT_NAME)  $(PUBLISH_LIB_LINUX)$(RELEASE_PATH)$(OUT_NAME) )
	-$(shell g++ -O3 test_main.cc -I /home/user0/common_libs_pub/include/linux/ lib/linux/release/libsld_slnic.a -pthread -lrt -o $(OUT_PATH)$(RELEASE_PATH)test)
	@echo "linux publish debug finish, but not commit"

commit_debug:
	svn ci ./ -m "auto linux"
	svn ci $(PUBLISH_INC_LINUX)  -m $(COMMIT_MSG)
	svn ci $(PUBLISH_LIB_LINUX)$(DEBUG_PATH)$(OUT_NAME)  -m $(COMMIT_MSG)
	@echo "linux commit"

commit_release:
	svn ci ./ -m "auto linux"
	svn ci $(PUBLISH_INC_LINUX)  -m $(COMMIT_MSG)
	svn ci $(PUBLISH_LIB_LINUX)$(RELEASE_PATH)$(OUT_NAME)  -m $(COMMIT_MSG)
	@echo "linux commit"

clean :
	@echo  "make clean"
	$(shell rm $(OBJ_PATH) -rf)
	$(shell rm $(OUT_PATH)$(DEBUG_PATH)$(OUT_NAME) -f)
	$(shell rm $(OUT_PATH)$(RELEASE_PATH)$(OUT_NAME) -f)

