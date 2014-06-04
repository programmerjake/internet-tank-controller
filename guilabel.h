#ifndef GUILABEL_H_INCLUDED
#define GUILABEL_H_INCLUDED

#include "guielement.h"
#include "platform.h"
#include "text.h"
#include <functional>
#include <stdexcept>

using namespace std;

struct GUILabel : public GUIElement
{
    wstring text;
    Color textColor;
    GUILabel(wstring text, float minX, float maxX, float minY, float maxY, Color textColor = Color::V(0))
        : GUIElement(minX, maxX, minY, maxY), text(text), textColor(textColor)
    {
    }
    virtual bool canHaveKeyboardFocus() const override final
    {
        return false;
    }
protected:
    virtual Mesh render(float minZ, float maxZ, bool hasFocus) override
    {
        float textWidth = Text::width(text);
        float textHeight = Text::height(text);
        if(textWidth == 0)
            textWidth = 1;
        if(textHeight == 0)
            textHeight = 1;
        float textScale = 0.5 * (maxY - minY) / textHeight;
        textScale = min(textScale, (maxX - minX) / textWidth);
        float xOffset = -0.5 * textWidth, yOffset = -0.5 * textHeight;
        xOffset = textScale * xOffset + 0.5 * (minX + maxX);
        yOffset = textScale * yOffset + 0.5 * (minY + maxY);
        return (Mesh)transform(Matrix::scale(textScale).concat(Matrix::translate(xOffset, yOffset, -1).concat(Matrix::scale(minZ))), Text::mesh(text, textColor));
    }
};

struct GUIDynamicLabel : public GUILabel
{
    GUIDynamicLabel(function<wstring()> textFn, float minX, float maxX, float minY, float maxY, Color textColor = Color::V(0))
        : GUILabel(L"", minX, maxX, minY, maxY, textColor), textFn(textFn)
    {
    }
protected:
    virtual Mesh render(float minZ, float maxZ, bool hasFocus) override
    {
        text = textFn();
        return GUILabel::render(minZ, maxZ, hasFocus);
    }
private:
    function<wstring()> textFn;
};

#endif // GUILABEL_H_INCLUDED
