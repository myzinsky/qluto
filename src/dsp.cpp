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

    if(sampleCounter == (N)) {
        sampleCounter = 0;
        fft_execute(liquidFFT);

        std::vector<std::complex<float>> result = fftShift();
        std::vector<float> waterfallSamples;

        for(uint64_t i = 0; i < N; i++) {
            waterfallSamples.push_back(std::abs(result[i]));
        }    

        emit notifyWaterfall(waterfallSamples);
    }
}

std::vector<std::complex<float>> fft::fftShift() 
{
    std::vector<std::complex<float>> result;
    
    result.reserve(N);

    result[N/2].imag(fftOut[0].imag());
    result[N/2].real(fftOut[0].real());

    for(uint64_t i = 1; i < N/2; i++) {
        result[N/2 + i].imag(fftOut[i].imag());
        result[N/2 + i].real(fftOut[i].real());
    }

    for(uint64_t i = N/2+1; i < N; i++) {
        result[i-N/2].imag(fftOut[i].imag());
        result[i-N/2].real(fftOut[i].real());
    }
    return result;

}