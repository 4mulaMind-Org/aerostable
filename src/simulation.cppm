module;
#include <vector>
#include <cmath>
#include <complex>
#include <iostream>

export module control_simulation;
import control_core;

export namespace aero {

// RK4 Method to solve ODE (Time-domain simulation)
std::vector<double> simulate_step_response(double a1, double a0, double dt = 0.01, double t_max = 10.0) {
    std::vector<double> response;
    
    double y = 0.0;
    double v = 0.0; 
    
    auto f = [&](double current_y, double current_v) {
        return 1.0 - a1 * current_v - a0 * current_y;
    };

    int steps = static_cast<int>(t_max / dt);
    for (int i = 0; i < steps; ++i) {
        response.push_back(y);
        
        double k1_v = f(y, v);
        double k1_y = v;

        double k2_v = f(y + 0.5 * dt * k1_y, v + 0.5 * dt * k1_v);
        double k2_y = v + 0.5 * dt * k1_v;

        double k3_v = f(y + 0.5 * dt * k2_y, v + 0.5 * dt * k2_v);
        double k3_y = v + 0.5 * dt * k2_v;

        double k4_v = f(y + dt * k3_y, v + dt * k3_v);
        double k4_y = v + dt * k3_v;

        y = y + (dt / 6.0) * (k1_y + 2*k2_y + 2*k3_y + k4_y);
        v = v + (dt / 6.0) * (k1_v + 2*k2_v + 2*k3_v + k4_v);
    }
    
    return response;
}

} // namespace aero