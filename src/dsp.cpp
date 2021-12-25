#include "dsp.h"

fft::fft(uint64_t N) : N(N)
{
    sampleCounter = 0;

    // Init Liquid FFT:
    fftIn  = new std::complex<float>[N];
    fftOut = new std::complex<float>[N];
    liquidFFT = fft_create_plan(N, fftIn, fftOut, LIQUID_FFT_FORWARD, 0);
}

fft::~fft()
{
    fft_destroy_plan(liquidFFT);
    delete [] fftIn;
    delete [] fftOut;
}

void fft::processSample(std::complex<float> sample)
{
    fftIn[sampleCounter++] = sample;

    if(sampleCounter == (N-1)) {
        sampleCounter = 0;
        fft_execute(liquidFFT);
        prepareWaterfall();
    }
}

void fft::prepareWaterfall() 
{
    std::vector<float> waterfallSamples;

    for(uint64_t i = 0; i < N; i++) {
        waterfallSamples.push_back(sqrt(pow(fftOut[i].imag(),2) + pow(fftOut[i].real(),2))); 
    }    

    emit notifyWaterfall(waterfallSamples);
}