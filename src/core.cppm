module;
#include <vector>
#include <complex>
#include <cmath>
#include <iostream>
#include <stdexcept>

export module control_core;

export namespace aero {

using Complex = std::complex<double>;

// Durand-Kerner Method: Polynomial ke saare roots ek saath nikalta hai
std::vector<Complex> find_roots(const std::vector<double>& coeffs) {
    int n = coeffs.size() - 1; // Degree of polynomial
    if (n < 1) return {};

    // Polynomial evaluation: P(z) = a_n*z^n + ... + a_0
    auto eval_poly = [&](Complex z) {
        Complex res = 0;
        for (int i = n; i >= 0; --i) {
            res = res * z + coeffs[i];
        }
        return res;
    };

    // Initial guesses (circle in complex plane)
    std::vector<Complex> roots(n);
    Complex seed(0.4, 0.9);
    roots[0] = 1.0; // Start with 1
    for (int i = 1; i < n; ++i) {
        roots[i] = roots[i - 1] * seed;
    }

    // Iterative refinement (Durand-Kerner)
    for (int iter = 0; iter < 1000; ++iter) {
        bool converged = true;
        for (int i = 0; i < n; ++i) {
            Complex numerator = eval_poly(roots[i]);
            Complex denominator = 1.0;
            
            for (int j = 0; j < n; ++j) {
                if (i != j) {
                    denominator *= (roots[i] - roots[j]);
                }
            }
            
            Complex delta = numerator / denominator;
            roots[i] -= delta;

            if (std::abs(delta) > 1e-10) converged = false;
        }
        if (converged) break;
    }
    return roots;
}

// Stability Check: Agar koi bhi pole right side me hai, toh system unstable hai
bool is_stable(const std::vector<Complex>& poles) {
    for (const auto& p : poles) {
        if (p.real() > 0.0) return false; // Right-Half Plane (RHP)
    }
    return true;
}

} // namespace aero