
NOW_TIME=`date +"%Y%02m0%2d_%02H%02M%02S"`

DRIVER_VERSION=$(svn info --no-auth-cache | grep "版本: \|Revision: " |tail -n -1 | cut -d ' ' -f2)

PUB_TAR=slnic_pub_driver_${DRIVER_VERSION}_${NOW_TIME}.tar.gz
echo ${DRIVER_VERSION} > version
PUB_DIR=slnic_pub_driver

TOOLS_BUILD_DIR=../../../slk_tools2
TOOLS_BIN_DIR=../../../slk_tools2_bin

#rm -rf ${PUB_DIR}
mkdir -p ${PUB_DIR}


rm -rf ${TOOLS_BIN_DIR}
mkdir -p ${TOOLS_BIN_DIR}

make -C ${TOOLS_BUILD_DIR} clean
make -C ${TOOLS_BUILD_DIR}
cp version ${PUB_DIR}
cp *.h ${PUB_DIR}
cp *.c ${PUB_DIR}
cp load_driver ${PUB_DIR}
cp unload_driver ${PUB_DIR}
cp Makefile_pub ${PUB_DIR}/Makefile
cp test.modules ${PUB_DIR}
cp -r ${TOOLS_BIN_DIR} ${PUB_DIR}
mv ${PUB_DIR}/slk_tools2_bin ${PUB_DIR}/slk_tools

#cp -r ${TOOLS_BIN_DIR}/*  ${TOOLS_DIR} 

#for COPY_OBJ in ${NEED_OBJS}
#do
#cp ${TOOLS_BIN_DIR}/${COPY_OBJ} ${TOOLS_DIR}
#done

#cp ${TOOLS_BIN_DIR}/*.h ${TOOLS_DIR}
#cp ${TOOLS_BIN_DIR}/*.c ${TOOLS_SRC_DIR}

#rm -rf ${TOOLS_BIN_DIR}
echo "build ${NOW_TIME}" > tar_file_list.txt
#rm *.tar.gz
tar zcvf ${PUB_TAR} ${PUB_DIR} >> tar_file_list.txt
rm -rf ${PUB_DIR}

