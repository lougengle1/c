#include <iostream>
using namespace std;

class dy_link_library
{
private:
    /* data */
public:
    dy_link_library();
    ~dy_link_library();

    bool init();
    bool exit();
    bool start();
    bool stop();
};

