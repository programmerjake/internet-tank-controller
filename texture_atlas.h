#ifndef TEXTURE_ATLAS_H_INCLUDED
#define TEXTURE_ATLAS_H_INCLUDED

#include "texture_descriptor.h"

class TextureAtlas final
{
public:
    static const Image texture;
    const int left, top, width, height;
    const float minU, maxU, minV, maxV;
    enum {textureXRes = 256, textureYRes = 256};
    static constexpr float pixelOffset = 0.05f;
    explicit TextureAtlas(int left, int top, int width, int height)
        : left(left), top(top), width(width), height(height),
        minU((left + pixelOffset) / textureXRes),
        maxU((left + width - pixelOffset) / textureXRes),
        minV(1 - (top + height - pixelOffset) / textureYRes),
        maxV(1 - (top + pixelOffset) / textureYRes)
    {
    }
    const TextureAtlas & operator =(const TextureAtlas &) = delete;
    TextureDescriptor td() const
    {
        return TextureDescriptor(texture, minU, maxU, minV, maxV);
    }
    TextureDescriptor tdNoOffset() const
    {
        return TextureDescriptor(texture, (float)left / textureXRes, (float)(left + width) / textureXRes, 1 - (float)(top + height) / textureYRes, 1 - (float)top / textureYRes);
    }
    static const TextureAtlas Font8x8,
    Line,
    Point,
    ButtonLeftDiffuse,
    ButtonRightDiffuse,
    ButtonMiddleDiffuse,
    ButtonLeftSpecular,
    ButtonRightSpecular,
    ButtonMiddleSpecular,
    MenuGearTop,
    MenuGearBottom;
};

#endif // TEXTURE_ATLAS_H_INCLUDED
