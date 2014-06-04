#ifndef GUIELEMENT_H
#define GUIELEMENT_H

#include "event.h"
#include "mesh.h"

class GUIContainer;
class GUIRunner;

class GUIElement : public EventHandler
{
public:
    shared_ptr<GUIElement> shared_from_this()
    {
        return dynamic_pointer_cast<GUIElement>(EventHandler::shared_from_this());
    }

    shared_ptr<const GUIElement> shared_from_this() const
    {
        return dynamic_pointer_cast<const GUIElement>(EventHandler::shared_from_this());
    }

    GUIElement(float minX, float maxX, float minY, float maxY);
    virtual ~GUIElement();
    Mesh render()
    {
        return render(0.1, 10, true);
    }
    virtual bool isPointInside(float x, float y) const
    {
        if(x < minX || x > maxX || y < minY || y > maxY)
            return false;
        return true;
    }
    virtual bool canHaveKeyboardFocus() const
    {
        return false;
    }
    virtual bool handleMouseUp(MouseUpEvent &event) override
    {
        return true;
    }
    virtual bool handleMouseDown(MouseDownEvent &event) override
    {
        setFocus();
        return true;
    }
    virtual bool handleMouseMove(MouseMoveEvent &event) override
    {
        return true;
    }
    virtual bool handleMouseMoveOut(MouseEvent &event)
    {
        return true;
    }
    virtual bool handleMouseMoveIn(MouseEvent &event)
    {
        return true;
    }
    virtual bool handleMouseScroll(MouseScrollEvent &event) override
    {
        return false;
    }
    virtual bool handleKeyUp(KeyUpEvent &event) override
    {
        return false;
    }
    virtual bool handleKeyDown(KeyDownEvent &event) override
    {
        return false;
    }
    virtual bool handleKeyPress(KeyPressEvent &event) override
    {
        return false;
    }
    virtual bool handleQuit(QuitEvent &event) override
    {
        return false;
    }
    virtual shared_ptr<GUIElement> getFocusElement()
    {
        return shared_from_this();
    }
    virtual bool nextFocusElement() /// returns true when reached container boundary
    {
        return true;
    }
    virtual bool prevFocusElement() /// returns true when reached container boundary
    {
        return true;
    }
    virtual void firstFocusElement()
    {
    }
    virtual void lastFocusElement()
    {
    }
    void setFocus();
    float minX, maxX, minY, maxY;
    virtual void reset()
    {
    }
protected:
    virtual Mesh render(float minZ, float maxZ, bool hasFocus) = 0;
    friend class GUIContainer; // so GUIContainer can set parent
    friend class GUIRunner;
    shared_ptr<GUIContainer> getParent() const
    {
        return parent;
    }
    virtual shared_ptr<GUIContainer> getTopLevelParent(); // definition in guicontainer.h
private:
    shared_ptr<GUIContainer> parent;
};

#include "guicontainer.h"

#endif // GUIELEMENT_H
