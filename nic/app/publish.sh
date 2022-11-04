
PUB_DIR=slnic_pulish
make clean
make debug
make release

DRIVER_DIR=/home/user0/shengli_driver/trunk/project/shengli_nic/src_nic

rm -rf ${PUB_DIR} 
mkdir -p ${PUB_DIR}
mkdir -p ${PUB_DIR}/api
mkdir -p ${PUB_DIR}/api/interface
mkdir -p ${PUB_DIR}/api/lib 
mkdir -p ${PUB_DIR}/demo 
mkdir -p ${PUB_DIR}/driver 
mkdir -p ${PUB_DIR}/speed_tool/slnic_rec

cp -r interface/* ${PUB_DIR}/api/interface/
cp -r lib/* ${PUB_DIR}/api/lib

#data
cp test_main.cc ${PUB_DIR}/demo/demo.cpp
cp demo.mk ${PUB_DIR}/demo/Makefile

#speed
cp test_main_speed_test.cc ${PUB_DIR}/speed_tool/slnic_rec/demo_speed_test.cpp
cp demo_speed.mk ${PUB_DIR}/speed_tool/slnic_rec/Makefile



cd ${DRIVER_DIR}
sh publish.sh
cd -
mv ${DRIVER_DIR}/*.tar.gz ${PUB_DIR}/driver

make clean
