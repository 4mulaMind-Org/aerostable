module;
#include <vector>
#include <complex>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <cstdint>
#include <string>
#include <stdexcept>

export module control_render;
import control_core;

export namespace aero {

using Complex = std::complex<double>;

inline std::uint32_t hsv_to_rgb(double h, double s, double v) {
    h = std::fmod(h, 360.0); if (h < 0) h += 360.0;
    double c = v * s;
    double x = c * (1.0 - std::fabs(std::fmod(h / 60.0, 2.0) - 1.0));
    double m = v - c;
    double r=0, g=0, b=0;
    if      (h <  60) { r=c; g=x; }
    else if (h < 120) { r=x; g=c; }
    else if (h < 180) { g=c; b=x; }
    else if (h < 240) { g=x; b=c; }
    else if (h < 300) { r=x; b=c; }
    else              { r=c; b=x; }
    auto q = [&](double f) {
        return static_cast<std::uint32_t>(std::clamp((f + m) * 255.0, 0.0, 255.0));
    };
    return (q(r) << 16) | (q(g) << 8) | q(b);
}

// 1. Pole-Zero Map Plot
inline void draw_stability_plot(
    const std::vector<Complex>& poles, 
    const std::string& path, 
    int width = 800, int height = 800) 
{
    std::vector<std::uint32_t> pixels(static_cast<std::size_t>(width) * height);

    double center_x = width / 2.0;
    double center_y = height / 2.0;
    double scale = 100.0;

    for (int py = 0; py < height; ++py) {
        for (int px = 0; px < width; ++px) {
            double real = (px - center_x) / scale;
            double imag = -(py - center_y) / scale;

            double v = 0.1; // Darker background
            if (real < 0) {
                v = 0.1 + 0.05 * (std::abs(real) / (width / (2.0 * scale)));
                pixels[py * width + px] = hsv_to_rgb(120.0, 1.0, v); 
            } else {
                v = 0.1 + 0.05 * (std::abs(real) / (width / (2.0 * scale)));
                pixels[py * width + px] = hsv_to_rgb(0.0, 1.0, v);
            }

            if (std::abs(imag) < 0.02 || std::abs(real) < 0.02) {
                pixels[py * width + px] = 0x444444;
            }
        }
    }

    for (const auto& p : poles) {
        int cx = static_cast<int>(p.real() * scale + center_x);
        int cy = static_cast<int>(-p.imag() * scale + center_y);
        int size = 12; // Bada kar diya

        for (int i = -size; i <= size; ++i) {
            if (cy >= 0 && cy < height && (cx + i) >= 0 && (cx + i) < width)
                pixels[cy * width + (cx + i)] = 0xFFFFFF;
            if (cx >= 0 && cx < width && (cy + i) >= 0 && (cy + i) < height)
                pixels[(cy + i) * width + cx] = 0xFFFFFF;
        }
    }

    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("Cannot open: " + path);
    out << "P6\n" << width << " " << height << "\n255\n";
    for (auto p : pixels) {
        out.put(static_cast<char>((p >> 16) & 0xFF));
        out.put(static_cast<char>((p >>  8) & 0xFF));
        out.put(static_cast<char>( p        & 0xFF));
    }
} // <--- Ye bracket miss ho gaya tha

// 2. Time Domain Plot
inline void draw_time_response(
    const std::vector<double>& response, 
    const std::string& path, 
    int width = 800, int height = 400) 
{
    std::vector<std::uint32_t> pixels(static_cast<std::size_t>(width) * height, 0x000000); 
    
    double max_val = 0.0;
    double min_val = 0.0;
    for (double v : response) {
        max_val = std::max(max_val, v);
        min_val = std::min(min_val, v);
    }
    max_val = std::max(max_val, 1.5);
    min_val = std::min(min_val, -1.5);

    double x_scale = static_cast<double>(width) / response.size();
    double y_scale = static_cast<double>(height) / (max_val - min_val);

    int y_zero = static_cast<int>(-min_val * y_scale);
    for(int x = 0; x < width; ++x) {
        if (y_zero >= 0 && y_zero < height) pixels[y_zero * width + x] = 0x444444;
    }

    for (size_t i = 0; i < response.size() - 1; ++i) {
        int x1 = static_cast<int>(i * x_scale);
        int y1 = static_cast<int>((max_val - response[i]) * y_scale);
        
        int x2 = static_cast<int>((i + 1) * x_scale);
        int y2 = static_cast<int>((max_val - response[i + 1]) * y_scale);

        int dx = std::abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
        int dy = -std::abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
        int err = dx + dy;
        
        while (true) {
            if (x1 >= 0 && x1 < width && y1 >= 0 && y1 < height) {
                pixels[y1 * width + x1] = 0x00FF00; 
            }
            if (x1 == x2 && y1 == y2) break;
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; x1 += sx; }
            if (e2 <= dx) { err += dx; y1 += sy; }
        }
    }

    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("Cannot open: " + path);
    out << "P6\n" << width << " " << height << "\n255\n";
    for (auto p : pixels) {
        out.put(static_cast<char>((p >> 16) & 0xFF));
        out.put(static_cast<char>((p >>  8) & 0xFF));
        out.put(static_cast<char>( p        & 0xFF));
    }
}

} // namespace aero