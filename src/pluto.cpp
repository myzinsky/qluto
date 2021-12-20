#include "pluto.h"

iio_scan_context* pluto::getScanContext() {
    return iio_create_scan_context(nullptr,0);
}

iio_context* pluto::getContext(iio_scan_context *scanContext) {
	struct iio_context_info **info;
	int ret = iio_scan_context_get_info_list(scanContext, &info);
	struct iio_context *context = NULL;

    if(ret <= 0) {
        std::cout << "Unable to detect device" << std::endl;
    } else if (ret == 1) {
		std::cout << "Device found: " << iio_context_info_get_uri(info[0]) << std::endl;
		context = iio_create_context_from_uri(iio_context_info_get_uri(info[0]));
    } else {
        std::cout << "More than one device found: FIXME" << std::endl;
    }

    return context;
}

void pluto::connect() {

    auto scanContext = getScanContext();

	if (!scanContext) {
		std::cout << "Unable to create scan context" << std::endl;
	} else {
        auto context = getContext(scanContext);
    }

}