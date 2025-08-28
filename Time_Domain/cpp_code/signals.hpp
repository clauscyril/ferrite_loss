#include "mfem.hpp"

using namespace mfem;

real_t sine_func(real_t t, real_t &peak, real_t f) 
{
    return peak * std::sin(2 * M_PI * f * t);
};


real_t square_func(real_t t, real_t &peak, real_t f) {
    real_t T = 1./f;
    real_t alpha = std::fmod(t,T)/T;
    if (alpha <= 0.5) {
        return peak;
    } else {
        return -peak;
    } 
}

real_t triangle_func(real_t t, real_t &peak, real_t f) {
    real_t T = 1./f;
    real_t phase = fmod(t, T);
    if (phase < 0) phase += T;
    real_t value;
    if (phase < T / 4) {
        value = (4 * peak / T) * phase;
    } else if (phase < 3 * T / 4) {
        value = peak - (4 * peak / T) * (phase - T / 4);
    } else {
        value = -peak + (4 * peak / T) * (phase - 3 * T / 4);
    }
    return value;
    
}
