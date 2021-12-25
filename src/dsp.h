#ifndef FFT_H
#define FFT_H

#include <iostream>
#include <QObject>
#include <QDebug>
#include <complex>
#include <liquid.h>

class fft : public QObject
{
    Q_OBJECT

    public:
    fft(uint64_t N = 4096);
    void processSample(std::complex<float> sample);
    void prepareWaterfall();

    private:
    uint64_t N;
    uint64_t sampleCounter;

    fftplan liquidFFT;
    std::complex<float> *fftIn;
    std::complex<float> *fftOut;

    signals: 
    void notifyWaterfall(std::vector<float> samples);

};

#endif