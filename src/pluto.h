#ifndef PLUTO_H
#define PLUTO_H

#include <iostream>
#include <sstream>
#include <QSettings>
#include <QThread>
#include <iio.h>
#include "dsp.h"

class pluto : public QObject
{
    Q_OBJECT

    enum iodev { RX, TX };

    public:
    pluto(fft* fourier);

    public slots:
    void connect();
    void start();

    signals: 
    void connected();
    void connectionError(QString message);

    private:

    // Config:
    uint64_t sampleRate;
    uint64_t sampleBufferSize;
    uint64_t lnbReference;
    uint64_t baseQrg; 
    double baseQrgTx;
    double baseQrgRx;
    int64_t bandwidthRx;
    int64_t bandwidthTx;

    // Context:
    iio_context *context;

    // Streaming devices:
    iio_device *tx;
    iio_device *rx;

    // Channels:
    iio_channel *rx0i;
    iio_channel *rx0q;
    iio_channel *tx0i;
    iio_channel *tx0q;

    // Buffers:
    iio_buffer  *rxBuffer;
    iio_buffer  *txBuffer;

    // Threads:
    QThread *rxThread;
    void rxFunction();

    // DSP:
    fft* fourier;

    // Methods that encapsulate pluto access (i.e. driver):
    iio_scan_context* getScanContext();
    iio_context* getContext(iio_scan_context* scanContext);
    iio_device* getDevice(iio_context* context);
    bool getStreamDevice(iio_context *context, iodev d, iio_device **device);
    bool getStreamChannel(iio_context *context, iodev d, iio_device *device, int chid, iio_channel **channel);
    bool getPhyConfigChannel(iio_context *context, iodev d, int chid, iio_channel **channel);
    bool getLocalOscillatorChannel(iio_context *context, iodev d, iio_channel **channel);
    void writeToChannel(iio_channel *channel, std::string what, std::string val);
    void writeToChannel(iio_channel *channel, std::string what, double val);
    void writeToChannel(iio_channel *channel, std::string what, int64_t val);

    bool configureChannel(iio_context *context, int64_t bandwidth, int64_t sampleRate, double baseQrg, std::string port, iodev type, int chid);
    bool setTxPower(double power);
    bool setTxQrg(int64_t qrg);
    bool setRxQrg(int64_t qrg);

    // Helpers:
    std::string getChannelNameModify(const char* type, int id, char modify);
    std::string getChannelName(const char* type, int id);
};

#endif