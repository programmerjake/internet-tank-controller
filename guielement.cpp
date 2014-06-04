#include "guielement.h"

GUIElement::GUIElement(float minX, float maxX, float minY, float maxY)
    : minX(minX), maxX(maxX), minY(minY), maxY(maxY)
{
    assert(minX < maxX && minY < maxY);
}

GUIElement::~GUIElement()
{
}

void GUIElement::setFocus()
{
    if(!canHaveKeyboardFocus())
        return;
    if(getParent() == nullptr)
        return;
    getParent()->setFocus(shared_from_this());
}
