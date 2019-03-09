#ifndef FFT_H
#define FFT_H

#include <complex>
#include <iostream>
#include <valarray>
#include <cmath>
#include <QVector>

#include "frequencyspectrum.h"

typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

enum WindowFunction{ NoWindow, HannWindow};

class FFT
{
public:
    FFT();
    void forwardfft(CArray &x);
    void forwardfft2(CArray &x);
    void ifft(CArray& x);
    void setWindowFunction(WindowFunction function);
    const FrequencySpectrum *getFrequency(CArray &x, qreal sampleRate);

private:
    void calculateWindow();

    WindowFunction m_windowFunction;
    QVector<double> m_window;
    FrequencySpectrum m_spectrum;
};

#endif // FFT_H
