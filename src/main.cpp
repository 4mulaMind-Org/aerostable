#include <iostream>
#include <vector>
#include <complex>

import control_core;
import control_render;
import control_simulation;

int main() {
    std::cout << "--- AeroStable: Control System Analyzer ---\n";

    // 1. Stable System: s^2 + 2s + 5 (a1=2, a0=5)
    std::vector<double> stable_coeffs = {5.0, 2.0, 1.0}; 
    aero::draw_stability_plot(aero::find_roots(stable_coeffs), "plot_stable.ppm");
    auto time_stable = aero::simulate_step_response(2.0, 5.0);
    aero::draw_time_response(time_stable, "time_stable.ppm");

    // 2. Unstable System: s^2 - 2s + 5 (a1=-2, a0=5)
    std::vector<double> unstable_coeffs = {5.0, -2.0, 1.0}; 
    aero::draw_stability_plot(aero::find_roots(unstable_coeffs), "plot_unstable.ppm");
    auto time_unstable = aero::simulate_step_response(-2.0, 5.0);
    aero::draw_time_response(time_unstable, "time_unstable.ppm");

    std::cout << "\nGenerated Pole-Zero plots and Time Response plots!\n";

    return 0;
}