#include <iostream>
#include <iio.h>

class pluto {
    public:
    void connect();

    private:
    iio_scan_context* getScanContext();
    iio_context* getContext(iio_scan_context* scanContext);
};