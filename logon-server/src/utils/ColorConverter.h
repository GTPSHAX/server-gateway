#pragma once
#include <cstdint>
class ColorConverter {
public:
    // Konversi ke format ARGB
    static uint32_t toARGB ( int alpha , int red , int green , int blue ) {
        validateColor ( alpha , red , green , blue );
        return (static_cast< uint32_t >(alpha) << 24) |
            (static_cast< uint32_t >(red) << 16) |
            (static_cast< uint32_t >(green) << 8) |
            static_cast< uint32_t >(blue);
    }

    // Konversi ke format RGBA
    static uint32_t toRGBA ( int red , int green , int blue , int alpha ) {
        validateColor ( alpha , red , green , blue );
        return (static_cast< uint32_t >(red) << 24) |
            (static_cast< uint32_t >(green) << 16) |
            (static_cast< uint32_t >(blue) << 8) |
            static_cast< uint32_t >(alpha);
    }

    // Konversi ke format RGB
    static uint32_t toRGB ( int red , int green , int blue ) {
        validateColor ( 255 , red , green , blue ); // Alpha default = 255 (tidak dipakai)
        return (static_cast< uint32_t >(red) << 16) |
            (static_cast< uint32_t >(green) << 8) |
            static_cast< uint32_t >(blue);
    }

    // Konversi ke format ABGR
    static uint32_t toABGR ( int alpha , int blue , int green , int red ) {
        validateColor ( alpha , red , green , blue );
        return (static_cast< uint32_t >(alpha) << 24) |
            (static_cast< uint32_t >(blue) << 16) |
            (static_cast< uint32_t >(green) << 8) |
            static_cast< uint32_t >(red);
    }

    // Konversi ke format BGRA
    static uint32_t toBGRA ( int blue , int green , int red , int alpha ) {
        validateColor ( alpha , red , green , blue );
        return (static_cast< uint32_t >(blue) << 24) |
            (static_cast< uint32_t >(green) << 16) |
            (static_cast< uint32_t >(red) << 8) |
            static_cast< uint32_t >(alpha);
    }

private:
    // Validasi nilai RGBA
    static void validateColor ( int alpha , int red , int green , int blue ) {
        if ( alpha < 0 || alpha > 255 || red < 0 || red > 255 ||
            green < 0 || green > 255 || blue < 0 || blue > 255 ) {
            throw std::runtime_error ( "RGBA values must be in the range [0, 255]" );
        }
    }
};