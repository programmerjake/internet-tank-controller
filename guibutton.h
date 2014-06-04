#ifndef GUIBUTTON_H_INCLUDED
#define GUIBUTTON_H_INCLUDED

#include "guielement.h"
#include "platform.h"
#include "text.h"
#include "texture_atlas.h"
#include "generate.h"
#include <functional>
#include <stdexcept>

using namespace std;

struct GUIButton : public GUIElement
{
    wstring title;
    Color textColor;
    Color selectedTextColor;
    Color buttonColor;
    Color selectedButtonColor;
    GUIButton(function<void()> pressEvent, wstring title, float minX, float maxX, float minY,
              float maxY, Color buttonColor = Color::V(0.5), Color textColor = Color::V(0),
              Color selectedButtonColor = Color::RGB(185 / 255.0, 12 / 255.0, 12 / 255.0), Color selectedTextColor = Color::V(0))
        : GUIElement(minX, maxX, minY, maxY), title(title), textColor(textColor),
          selectedTextColor(selectedTextColor), buttonColor(buttonColor),
          selectedButtonColor(selectedButtonColor), buttonPressEvent(pressEvent)
    {
    }
protected:
    GUIButton(wstring title, float minX, float maxX, float minY, float maxY,
              Color buttonColor = Color::V(0.75), Color textColor = Color::V(0),
              Color selectedButtonColor = Color::V(0.75), Color selectedTextColor = Color::RGB(0, 0, 1))
        : GUIElement(minX, maxX, minY, maxY), title(title), textColor(textColor),
          selectedTextColor(selectedTextColor), buttonColor(buttonColor),
          selectedButtonColor(selectedButtonColor), buttonPressEvent([]()
    {
        throw logic_error("button press event function called instead of overridden member");
    })
    {
    }
private:
    float getLeftToMiddleX() const
    {
        return minX + 0.5 * (maxY - minY);
    }
    float getMiddleToRightX() const
    {
        return maxX - 0.5 * (maxY - minY);
    }
public:
    virtual bool isPointInside(float x, float y) const override
    {
        if(!GUIElement::isPointInside(x, y))
        {
            return false;
        }

        float leftToMiddleX = getLeftToMiddleX();
        float middleToRightX = getMiddleToRightX();

        if(x > leftToMiddleX)
        {
            x -= middleToRightX;

            if(x < 0)
            {
                return true;
            }
        }
        else
        {
            x -= leftToMiddleX;
        }

        float halfHeight = 0.5 * (maxY - minY);
        y -= 0.5 * (minY + maxY);

        if(x * x + y * y < halfHeight * halfHeight)
        {
            return true;
        }

        return false;
    }
    virtual bool canHaveKeyboardFocus() const override
    {
        return true;
    }
    virtual bool handleMouseUp(MouseUpEvent &event)
    {
        if(event.button == MouseButton_Left)
        {
            isMousePressed = false;
            return true;
        }

        return GUIElement::handleMouseUp(event);
    }
    virtual bool handleMouseDown(MouseDownEvent &event) override
    {
        if(event.button == MouseButton_Left)
        {
            isMousePressed = true;
            lastDownTime = Display::timer();
            setFocus();
            handleButtonPress();
            return true;
        }

        return GUIElement::handleMouseDown(event);
    }
    virtual bool handleMouseMoveOut(MouseEvent &event) override
    {
        isMousePressed = false;
        return true;
    }
    virtual bool handleKeyDown(KeyDownEvent &event) override
    {
        if(!event.isRepetition && (event.key == KeyboardKey_Return || event.key == KeyboardKey_Space))
        {
            lastDownTime = Display::timer();
            handleButtonPress();
            return true;
        }

        return GUIElement::handleKeyDown(event);
    }
protected:
    bool isPressed() const
    {
        return isMousePressed || (Display::timer() - lastDownTime < 0.1);
    }
    virtual void handleButtonPress()
    {
        buttonPressEvent();
    }
    virtual Mesh render(float minZ, float maxZ, bool hasFocus) override
    {
        Color buttonColor = this->buttonColor;
        Color textColor = this->textColor;

        if(hasFocus)
        {
            buttonColor = this->selectedButtonColor;
            textColor = this->selectedTextColor;
        }

        bool pressed = isPressed();
        float textWidth = Text::width(title);
        float textHeight = Text::height(title);
        float leftToMiddleX = getLeftToMiddleX();
        float middleToRightX = getMiddleToRightX();
        Mesh diffuse = Generate::quadrilateral(TextureAtlas::ButtonLeftDiffuse.td(),
                                               VectorF(minX, minY, -1), buttonColor,
                                               VectorF(leftToMiddleX, minY, -1), buttonColor,
                                               VectorF(leftToMiddleX, maxY, -1), buttonColor,
                                               VectorF(minX, maxY, -1), buttonColor);
        diffuse->add(Generate::quadrilateral(TextureAtlas::ButtonMiddleDiffuse.td(),
                                             VectorF(leftToMiddleX, minY, -1), buttonColor,
                                             VectorF(middleToRightX, minY, -1), buttonColor,
                                             VectorF(middleToRightX, maxY, -1), buttonColor,
                                             VectorF(leftToMiddleX, maxY, -1), buttonColor));
        diffuse->add(Generate::quadrilateral(TextureAtlas::ButtonRightDiffuse.td(),
                                             VectorF(middleToRightX, minY, -1), buttonColor,
                                             VectorF(maxX, minY, -1), buttonColor,
                                             VectorF(maxX, maxY, -1), buttonColor,
                                             VectorF(middleToRightX, maxY, -1), buttonColor));
        Mesh specular = Generate::quadrilateral(TextureAtlas::ButtonLeftSpecular.td(),
                                                VectorF(minX, minY, -1), Color(1),
                                                VectorF(leftToMiddleX, minY, -1), Color(1),
                                                VectorF(leftToMiddleX, maxY, -1), Color(1),
                                                VectorF(minX, maxY, -1), Color(1));
        specular->add(Generate::quadrilateral(TextureAtlas::ButtonMiddleSpecular.td(),
                                              VectorF(leftToMiddleX, minY, -1), Color(1),
                                              VectorF(middleToRightX, minY, -1), Color(1),
                                              VectorF(middleToRightX, maxY, -1), Color(1),
                                              VectorF(leftToMiddleX, maxY, -1), Color(1)));
        specular->add(Generate::quadrilateral(TextureAtlas::ButtonRightSpecular.td(),
                                              VectorF(middleToRightX, minY, -1), Color(1),
                                              VectorF(maxX, minY, -1), Color(1),
                                              VectorF(maxX, maxY, -1), Color(1),
                                              VectorF(middleToRightX, maxY, -1), Color(1)));
        Matrix buttonTransform = Matrix::identity();

        if(pressed)
        {
            float midX = (minX + maxX) * 0.5, midY = (minY + maxY) * 0.5;
            buttonTransform = Matrix::translate(-midX, -midY,
                                                0).concat(Matrix::rotateZ(M_PI)).concat(Matrix::translate(midX, midY, 0));
        }

        Mesh retval = (Mesh)transform(buttonTransform.concat(Matrix::scale(interpolate(2.0 / 3, minZ,
                                      maxZ))), diffuse);

        if(textHeight > 0 && textWidth > 0)
        {
            float textScale = 0.5 * (maxY - minY) / textHeight;
            textScale = min(textScale, (middleToRightX - leftToMiddleX) / textWidth);

            if(pressed)
            {
                textScale *= 0.9;
            }

            float xOffset = -0.5 * textWidth, yOffset = -0.5 * textHeight;
            xOffset = textScale * xOffset + 0.5 * (minX + maxX);
            yOffset = textScale * yOffset + 0.5 * (minY + maxY);
            retval->add(transform(Matrix::scale(textScale).concat(Matrix::translate(xOffset, yOffset,
                                  -1).concat(Matrix::scale(minZ))), Text::mesh(title, textColor)));
        }

        retval->add(transform(buttonTransform.concat(Matrix::scale(interpolate(1.0 / 3, minZ, maxZ))),
                              specular));
        return retval;
    }
    virtual void reset()
    {
        isMousePressed = false;
        lastDownTime = -1;
    }
private:
    bool isMousePressed = false;
    double lastDownTime = -1;
    function<void()> buttonPressEvent;
};

#endif // GUIBUTTON_H_INCLUDED
