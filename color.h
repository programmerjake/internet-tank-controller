#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

#include <cstdint>
#include <ostream>
#include <cmath>
#include "util.h"

using namespace std;

struct Color final /// RGBA color with rgba in the range 0.0 to 1.0
{
    float r, g, b, a; /// a is opacity -- 0 is transparent and 1 is opaque
    Color(float v, float a = 1)
    {
        r = g = b = v;
        this->a = a;
    }
    Color()
    {
        r = g = b = a = 0;
    }
    Color(float r, float g, float b, float a = 1)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }
    static Color RGBA(float r, float g, float b, float a)
    {
        return Color(r, g, b, a);
    }
    static Color RGB(float r, float g, float b)
    {
        return Color(r, g, b);
    }
    static Color VA(float v, float a)
    {
        return Color(v, a);
    }
    static Color V(float v)
    {
        return Color(v);
    }
    static Color HSVA(float hue, float saturation, float value, float a);
    static Color HSV(float h, float s, float v)
    {
        return HSVA(h, s, v, 1);
    }
    static Color HSBA(float hue, float saturation, float brightness, float a);
    static Color HSB(float h, float s, float b)
    {
        return HSBA(h, s, b, 1);
    }
    uint8_t ri() const /// get red value as a byte in the range 0 to 0xFF
    {
        return ifloor(limit(r * 256.0f, 0.0f, 255.0f));
    }
    uint8_t gi() const /// get green value as a byte in the range 0 to 0xFF
    {
        return ifloor(limit(g * 256.0f, 0.0f, 255.0f));
    }
    uint8_t bi() const /// get blue value as a byte in the range 0 to 0xFF
    {
        return ifloor(limit(b * 256.0f, 0.0f, 255.0f));
    }
    uint8_t ai() const /// get alpha value as a byte in the range 0 to 0xFF
    {
        return ifloor(limit(a * 256.0f, 0.0f, 255.0f));
    }
    void ri(uint8_t v) /// set red value as a byte in the range 0 to 0xFF
    {
        r = (unsigned)v * (1.0f / 255.0f);
    }
    void gi(uint8_t v) /// set green value as a byte in the range 0 to 0xFF
    {
        g = (unsigned)v * (1.0f / 255.0f);
    }
    void bi(uint8_t v) /// set blue value as a byte in the range 0 to 0xFF
    {
        b = (unsigned)v * (1.0f / 255.0f);
    }
    void ai(uint8_t v) /// set alpha value as a byte in the range 0 to 0xFF
    {
        a = (unsigned)v * (1.0f / 255.0f);
    }
    friend Color scale(Color l, Color r) /// multiply componentwise
    {
        return Color(l.r * r.r, l.g * r.g, l.b * r.b, l.a * r.a);
    }
    friend Color scale(float l, Color r) /// scales the color but not the alpha
    {
        return Color(l * r.r, l * r.g, l * r.b, r.a);
    }
    friend Color scale(Color l, float r) /// scales the color but not the alpha
    {
        return Color(l.r * r, l.g * r, l.b * r, l.a);
    }
    friend Color add(Color l, Color r)
    {
        return Color(l.r + r.r, l.g + r.g, l.b + r.b, min(l.a, r.a));
    }
    friend ostream & operator <<(ostream & os, const Color & c) /// writes a color to a ostream
    {
        return os << "RGBA(" << c.r << ", " << c.g << ", " << c.b << ", " << c.a << ")";
    }
    float hue() const
    {
        Color c = *this;
        float minV = min(c.r, min(c.g, c.b));
        c.r -= minV;
        c.g -= minV;
        c.b -= minV;
        float maxV = max(c.r, max(c.g, c.b));
        if(maxV < eps * minV)
            return 0;
        if(c.r == maxV)
        {
            if(c.g < c.b)
            {
                return (6 - c.b / maxV) / 6;
            }
            return (c.g / maxV) / 6;
        }
        if(c.g == maxV)
        {
            if(c.r < c.b)
            {
                return (2 + c.b / maxV) / 6;
            }
            return (2 - c.r / maxV) / 6;
        }
        if(c.r < c.g)
        {
            return (4 - c.g / maxV) / 6;
        }
        return (4 + c.r / maxV) / 6;
    }
    float saturation() const
    {
        float minV = min(r, min(g, b));
        float maxV = max(r, max(g, b));
        if(maxV == 0)
            return 0;
        return 1 - minV / maxV;
    }
    float value() const
    {
        return max(r, max(g, b));
    }
    float brightness() const
    {
        float minV = min(r, min(g, b));
        float maxV = max(r, max(g, b));
        return maxV - 0.5 * (1 - minV);
    }
};

template <>
inline const Color interpolate<Color>(const float t, const Color a, const Color b)
{
    return Color(interpolate(t, a.r, b.r), interpolate(t, a.g, b.g), interpolate(t, a.b, b.b), interpolate(t, a.a, b.a));
}

inline Color Color::HSVA(float hue, float saturation, float value, float a)
{
    hue -= floor(hue);
    saturation = limit<float>(saturation, 0, 1);
    hue *= 6;
    Color retval = Color(0, a);
    switch(ifloor(hue))
    {
    case 0:
        retval.r = 1;
        retval.g = hue;
        break;
    case 1:
        retval.g = 1;
        retval.r = 2 - hue;
        break;
    case 2:
        retval.g = 1;
        retval.b = hue - 2;
        break;
    case 3:
        retval.g = 4 - hue;
        retval.b = 1;
        break;
    case 4:
        retval.b = 1;
        retval.r = hue - 4;
        break;
    default:
        retval.r = 1;
        retval.b = 6 - hue;
        break;
    }
    retval = interpolate(saturation, Color(1, a), retval);
    return scale(value, retval);
}

inline Color Color::HSBA(float hue, float saturation, float brightness, float a)
{
    hue -= floor(hue);
    saturation = limit<float>(saturation, 0, 1);
    brightness = limit<float>(brightness, 0, 1);
    hue *= 6;
    Color retval = Color(0, a);
    switch(ifloor(hue))
    {
    case 0:
        retval.r = 1;
        retval.g = hue;
        break;
    case 1:
        retval.g = 1;
        retval.r = 2 - hue;
        break;
    case 2:
        retval.g = 1;
        retval.b = hue - 2;
        break;
    case 3:
        retval.g = 4 - hue;
        retval.b = 1;
        break;
    case 4:
        retval.b = 1;
        retval.r = hue - 4;
        break;
    default:
        retval.r = 1;
        retval.b = 6 - hue;
        break;
    }
    retval = interpolate(saturation, Color(0.5, a), retval);
    if(brightness < 0.5)
        return scale(2 * brightness, retval);
    return interpolate(2 - 2 * brightness, Color(1, a), retval);
}

#endif // COLOR_H_INCLUDED
