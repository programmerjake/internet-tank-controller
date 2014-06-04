#include "gui.h"

unordered_map<shared_ptr<GUIContainer>, weak_ptr<GUIRunner>> * GUIRunner::runners = nullptr;

void GUIRunner::makeRunners()
{
    if(runners == nullptr)
        runners = new unordered_map<shared_ptr<GUIContainer>, weak_ptr<GUIRunner>>;
}

namespace
{
Image getBackground()
{
    static Image * pbackground = nullptr;
    if(pbackground == nullptr)
        pbackground = new Image();
    Image & background = *pbackground;
    if(!background)
        background = Image(L"background.png");
    return background;
}
}

bool GUIRunner::run()
{
    Renderer renderer;
    Image background = getBackground();
    needQuit = false;
    runRetval = false;
    bool first = true;
    while(!needQuit)
    {
        Display::initFrame();
        if(first)
        {
            first = false;
            gui->reset();
        }
        Display::handleEvents(gui);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        TextureDescriptor td = TextureDescriptor(background, 0, (float)Display::width() / background.width(), 0,
                               (float)Display::height() / background.height());
        Mesh backgroundmesh = Generate::quadrilateral(td, VectorF(-Display::scaleX(), -Display::scaleY(),
                              -1), Color(1), VectorF(Display::scaleX(), -Display::scaleY(), -1), Color(1),
                              VectorF(Display::scaleX(), Display::scaleY(), -1), Color(1), VectorF(-Display::scaleX(),
                                      Display::scaleY(), -1), Color(1));
        renderer << backgroundmesh;
        Display::initOverlay();
        renderer << gui->render();
        Display::flip(60);
        vector<function<void()>> fns = std::move(functionList);
        functionList.clear();

        for(function<void()> fn : fns)
        {
            fn();
        }
    }
    return runRetval;
}
