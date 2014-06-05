#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include <memory>
#include <functional>
#include <vector>
#include <unordered_map>
#include "guielement.h"
#include "guicontainer.h"
#include "guibutton.h"
#include "guilabel.h"
#include "guicanvas.h"
#include "guicircle_arrangement.h"

using namespace std;

class GUIRunner final : public enable_shared_from_this<GUIRunner>
{
    shared_ptr<GUIContainer> gui;
    vector<function<void()>> functionList;
    static unordered_map<shared_ptr<GUIContainer>, weak_ptr<GUIRunner>> * runners;
    static void makeRunners();
    GUIRunner(shared_ptr<GUIContainer> gui)
        : gui(gui)
    {
        makeRunners();
    }
    GUIRunner(const GUIRunner &) = delete;
    const GUIRunner & operator =(const GUIRunner &) = delete;
    bool runRetval = false;
    bool needQuit = false;
    shared_ptr<GUIElement> grabbingElement = nullptr;
public:
    ~GUIRunner()
    {
        if(gui != nullptr && runners != nullptr)
        {
            runners->erase(gui);
        }
    }
    static shared_ptr<GUIRunner> make(shared_ptr<GUIContainer> gui)
    {
        makeRunners();
        gui = gui->getTopLevelParent();
        shared_ptr<GUIRunner> retval = shared_ptr<GUIRunner>(new GUIRunner(gui));
        (*runners)[gui] = retval;
        return retval;
    }
    static shared_ptr<GUIRunner> get(shared_ptr<GUIContainer> gui)
    {
        makeRunners();
        gui = gui->getTopLevelParent();
        auto iter = runners->find(gui);
        if(iter == runners->end())
            return shared_ptr<GUIRunner>(nullptr);
        return std::get<1>(*iter).lock();
    }
    static shared_ptr<GUIRunner> get(shared_ptr<GUIElement> element)
    {
        makeRunners();
        auto gui = element->getTopLevelParent();
        auto iter = runners->find(gui);
        if(iter == runners->end())
            return shared_ptr<GUIRunner>(nullptr);
        return std::get<1>(*iter).lock();
    }
    bool run();
    void scheduleFunction(function<void()> fn)
    {
        functionList.push_back(fn);
    }
    void quit(bool retval = false)
    {
        needQuit = true;
        runRetval = retval;
    }
    shared_ptr<GUIContainer> getGUI()
    {
        return gui;
    }
    shared_ptr<GUIElement> getGrabbingElement()
    {
        return grabbingElement;
    }
    void setGrabbingElement(shared_ptr<GUIElement> element);
    void clearGrab()
    {
        setGrabbingElement(nullptr);
    }
    void removeGrabbingElement(shared_ptr<GUIElement> element)
    {
        if(getGrabbingElement() == element || element == nullptr)
            clearGrab();
    }
};

inline bool runAsDialog(shared_ptr<GUIContainer> gui)
{
    shared_ptr<GUIRunner> runner = GUIRunner::make(gui);
    return runner->run();
}

class GUIScrollBar : public GUIElement
{
    function<float()> getValueFn;
    function<void(float)> setValueFn;
    bool mouseDown = false;
protected:
    float getPadding() const
    {
        return 0.25 * (maxY - minY);
    }
    float translateEventPosition(const MouseEvent &event) const
    {
        VectorF p = Display::transformMouseTo3D(event.x, event.y);
        float padding = getPadding();
        return (p.x - (minX + padding)) / (maxX - minX - padding * 2);
    }
public:
    GUIScrollBar(function<float()> getValueFn, function<void(float)> setValueFn, float minX, float maxX, float minY, float maxY)
        : GUIElement(minX, maxX, minY, maxY), getValueFn(getValueFn), setValueFn(setValueFn)
    {
    }
    virtual bool handleMouseUp(MouseUpEvent &event) override
    {
        if(event.button != MouseButton_Left || !mouseDown)
            return true;
        setValueFn(limit(translateEventPosition(event), 0.0f, 1.0f));
        mouseDown = false;
        GUIRunner::get(shared_from_this())->removeGrabbingElement(shared_from_this());
        return true;
    }
    virtual bool handleMouseDown(MouseDownEvent &event) override
    {
        if(event.button != MouseButton_Left)
            return true;
        setValueFn(limit(translateEventPosition(event), 0.0f, 1.0f));
        mouseDown = true;
        GUIRunner::get(shared_from_this())->setGrabbingElement(shared_from_this());
        return true;
    }
    virtual bool handleMouseMove(MouseMoveEvent &event) override
    {
        if(!mouseDown)
            return true;
        setValueFn(limit(translateEventPosition(event), 0.0f, 1.0f));
        return true;
    }
    virtual bool handleMouseMoveOut(MouseEvent &event) override
    {
        if(!mouseDown)
            return true;
        setValueFn(limit(translateEventPosition(event), 0.0f, 1.0f));
        GUIRunner::get(shared_from_this())->removeGrabbingElement(shared_from_this());
        mouseDown = false;
        return true;
    }
    virtual bool handleMouseMoveIn(MouseEvent &event) override
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
    virtual void reset() override
    {
        mouseDown = false;
    }
protected:
    virtual Mesh render(float minZ, float maxZ, bool hasFocus) override
    {
        float lineHeight = min(0.5 * (maxY - minY), 0.05);
        float minLineY = (minY + maxY - lineHeight) * 0.5;
        float maxLineY = (minY + maxY + lineHeight) * 0.5;
        float padding = getPadding();
        Mesh line = Generate::quadrilateral(TextureAtlas::Line.td(),
                                               VectorF(minX + padding, minLineY, -1), Color::V(1),
                                               VectorF(maxX - padding, minLineY, -1), Color::V(1),
                                               VectorF(maxX - padding, maxLineY, -1), Color::V(1),
                                               VectorF(minX + padding, maxLineY, -1), Color::V(1));
        float boxX = interpolate(limit(getValueFn(), 0.0f, 1.0f), minX + padding, maxX - padding);
        float minBoxX = boxX - padding;
        float maxBoxX = boxX + padding;
        Mesh box = Generate::quadrilateral(TextureAtlas::Point.td(),
                                               VectorF(minBoxX, minY, -1), Color::V(1),
                                               VectorF(maxBoxX, minY, -1), Color::V(1),
                                               VectorF(maxBoxX, maxY, -1), Color::V(1),
                                               VectorF(minBoxX, maxY, -1), Color::V(1));
        Mesh retval = (Mesh)transform(Matrix::scale(minZ), box);
        retval->add(transform(Matrix::scale((maxZ + minZ) * 0.5), line));
        return retval;
    }
};

#endif // GUI_H_INCLUDED
