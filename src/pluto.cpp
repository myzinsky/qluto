#include "pluto.h"

pluto::pluto() {
    sampleRate = 3000000;
    sampleBufferSize = sampleRate/10;
    lnbReference = 24000UL; // kHz
    baseQrg = 10489470UL; // KHz
    baseQrgRx = double((baseQrg - 390UL*lnbReference)*1000UL); // Hz
    baseQrgTx = double((2400000UL - 30UL)*1000UL); // Hz
    bandwidthRx = 1'000'000; 
    bandwidthTx =   100'000;
    rxBuffer = nullptr;
    txBuffer = nullptr;
}

iio_scan_context* pluto::getScanContext() {
    return iio_create_scan_context(nullptr,0);
}

iio_context* pluto::getContext(iio_scan_context *scanContext) {
    struct iio_context_info **info;
    int ret = iio_scan_context_get_info_list(scanContext, &info);

    if(ret <= 0) {
        std::cout << "Unable to detect device" << std::endl;
        return nullptr;
    } else if (ret == 1) {
        std::cout << "Device found: " << iio_context_info_get_uri(info[0]) << std::endl;
        return iio_create_context_from_uri(iio_context_info_get_uri(info[0]));
    } else {
        std::cout << "More than one device found: FIXME" << std::endl;
        return nullptr;
    }
}

iio_device* pluto::getDevice(iio_context* context) 
{
    return iio_context_find_device(context, "ad9361-phy");
}

bool pluto::getStreamDevice(iio_context *context, iodev d, iio_device **device) 
{
    if(d == TX) {
        *device = iio_context_find_device(context, "cf-ad9361-dds-core-lpc");
        return *device != nullptr;
    } else if (d == RX) { 
        *device = iio_context_find_device(context, "cf-ad9361-lpc");
        return *device != nullptr;
    }
    return false;
}

bool pluto::getStreamChannel(iio_context *context, iodev d, iio_device *device, int chid, iio_channel **channel) {
    *channel = iio_device_find_channel(device, getChannelName("voltage", chid).c_str(), d == TX);
    if (!*channel)
        *channel = iio_device_find_channel(device, getChannelName("altvoltage", chid).c_str(), d == TX);
    return *channel != NULL;
}

// Finds AD9361 phy IIO configuration channel with id chid
bool pluto::getPhyConfigChannel(iio_context *context, iodev d, int chid, iio_channel **channel) 
{
    if(d == RX) {
        *channel = iio_device_find_channel(getDevice(context), getChannelName("voltage", chid).c_str(), false);
        return *channel != NULL;
    } else if (d == TX) {
        *channel = iio_device_find_channel(getDevice(context), getChannelName("voltage", chid).c_str(), true);
        return *channel != NULL;
    }
    return false;
}

// Finds AD9361 local oscillator IIO configuration channels 
bool pluto::getLocalOscillatorChannel(iio_context *context, iodev d, iio_channel **channel) 
{
    if(d == RX) {
        *channel = iio_device_find_channel(getDevice(context), getChannelName("altvoltage", 0).c_str(), true);
        return *channel != NULL;
    } else if (d == TX) {
        *channel = iio_device_find_channel(getDevice(context), getChannelName("altvoltage", 1).c_str(), true); 
        return *channel != NULL;
    }
    return false;
}

std::string pluto::getChannelNameModify(const char* type, int id, char modify) 
{
    std::stringstream ss;
    ss << type << id << "_" << modify;
    std::cout << ss.str() << std::endl;
    return ss.str();
}

void pluto::writeToChannel(iio_channel *channel, std::string what, std::string val)
{
    iio_channel_attr_write(channel, what.c_str(), val.c_str());
}

void pluto::writeToChannel(iio_channel *channel, std::string what, double val)
{
    iio_channel_attr_write_double(channel, what.c_str(), val);
}

void pluto::writeToChannel(iio_channel *channel, std::string what, int64_t val)
{
    iio_channel_attr_write_longlong(channel, what.c_str(), val);
}

std::string pluto::getChannelName(const char* type, int id) 
{
    std::stringstream ss;
    ss << type << id;
    std::cout << ss.str() << std::endl;
    return ss.str();
}

bool pluto::configureChannel(iio_context *context, stream_cfg *config, iodev type, int chid)
{
    iio_channel *channel = nullptr;

    if(!getPhyConfigChannel(context, type, chid, &channel)) {
        return false;
    }

    writeToChannel(channel, "rf_port_select", config->rfport);
    writeToChannel(channel, "rf_bandwidth", config->bw_hz);
    writeToChannel(channel, "sampling_frequency", config->fs_hz);

    if(type == TX) {
        setTxPower(0.0);
        setTxQrg(config->lo_hz);
    } else {
        writeToChannel(channel, "gain_control_mode", "slow_attack");
        setRxQrg(config->lo_hz);
    }

    return true;
}

bool pluto::setTxPower(double power) {
    struct iio_channel *channel = nullptr;
    if (!getPhyConfigChannel(context, TX, 0, &channel)) {
        return false;
    }

    writeToChannel(channel, "hardwaregain", power);
    return true;
}

bool pluto::setTxQrg(int64_t qrg)
{
    struct iio_channel *channel = nullptr;
    if (!getLocalOscillatorChannel(context, TX, &channel)) {
        return false;
    }
    writeToChannel(channel, "frequency", qrg);
}

bool pluto::setRxQrg(int64_t qrg)
{
    struct iio_channel *channel = NULL;
    if (!getLocalOscillatorChannel(context, RX, &channel)) {
        return false;
    }
    writeToChannel(channel, "frequency", qrg);
}

void pluto::connect()
{
    // RX stream config:
    rxCfg.bw_hz = bandwidthRx;
    rxCfg.fs_hz = sampleRate;
    rxCfg.lo_hz = baseQrgRx;
    rxCfg.rfport = "A_BALANCED";

    // TX stream config:
    txCfg.bw_hz = bandwidthRx;
    txCfg.fs_hz = sampleRate;
    txCfg.lo_hz = baseQrgTx;
    txCfg.rfport = "A";

    // Find Pluto:
    auto scanContext = getScanContext();

    if (scanContext == nullptr) {
        emit connectionError("Cannot connect to Pluto: Unable to create scan context");
        return;
    }

    context = nullptr;
    context = getContext(scanContext);

    if(context == nullptr) {
        emit connectionError("Cannot connect to Pluto: Unable to create context");
        return;
    }

    // Get TX and RX Stream Devices:
    if(!getStreamDevice(context, TX, &tx)) {
        emit connectionError("Cannot connect to Pluto: Unable to find TX steraming device");
        return;
    }

    if(!getStreamDevice(context, RX, &rx)) {
        emit connectionError("Cannot connect to Pluto: Unable to find RX steraming device");
        return;
    }

    if(!configureChannel(context, &rxCfg, RX, 0)) {
        emit connectionError("Cannot connect to Pluto: Unable to configure RX channel");
        return;
    }

    if(!configureChannel(context, &txCfg, TX, 0)) {
        emit connectionError("Cannot connect to Pluto: Unable to configure RX channel");
        return;
    }

    bool result = true;
    result &= getStreamChannel(context, RX, rx, 0, &rx0i);
    result &= getStreamChannel(context, RX, rx, 1, &rx0q);
    result &= getStreamChannel(context, TX, tx, 0, &tx0i);
    result &= getStreamChannel(context, TX, tx, 1, &tx0q);

    if(!result) {
        emit connectionError("Cannot connect to Pluto: Unable to find streaming channel");
        return;
    }

    iio_channel_enable(rx0i);
    iio_channel_enable(rx0q);
    iio_channel_enable(tx0i);
    iio_channel_enable(tx0q);

    rxBuffer = iio_device_create_buffer(rx, sampleBufferSize, false);
    if (!rxBuffer) {
        emit connectionError("Could not create RX buffer");
        return;
    }

    txBuffer = iio_device_create_buffer(tx, sampleBufferSize, false);
    if (!txBuffer) {
        emit connectionError("Could not create TX buffer");
        return;
    }

    emit connected();
}

void pluto::start() {
}
