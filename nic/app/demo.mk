
OBJS = demo.o
TARGET = demo_main
CXXFLAGS += -I../api -I/home/user0/common_libs_pub/include/linux/
LDFLAGS += -pthread -lrt
LDFLAGS += ../api/lib/linux/release/libsld_slnic.a

$(TARGET):$(OBJS)
	$(CXX) $< -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	rm $(TARGET) $(OBJS) -f
