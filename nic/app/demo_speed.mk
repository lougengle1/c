
OBJS = demo_speed_test.o
TARGET = test_speed
CXXFLAGS += -I../../api -I/home/user0/common_libs_pub/include/linux/
LDFLAGS += -pthread -lrt
LDFLAGS += ../../api/lib/linux/release/libsld_slnic.a

$(TARGET):$(OBJS)
	$(CXX) $< -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	rm $(TARGET) $(OBJS) -f
