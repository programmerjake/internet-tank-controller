#include <thread>
#include <atomic>
#include <array>
#include <chrono>
#include <sstream>
#include "platform.h"
#include "gui.h"
#include "stream.h"

using namespace std;

array<atomic<float>, 5> outputs;
atomic_bool done(false);

void communicationThreadFn(shared_ptr<StreamRW> streams)
{
    Reader & reader = streams->reader();
    Writer & writer = streams->writer();
    writer.writeBytes((const uint8_t *)"-1\n", 3); // reset
    while(!done)
    {
        this_thread::sleep_for(chrono::milliseconds(50));
        ostringstream os;
        for(size_t i = 0; i < outputs.size(); i++)
        {
            os << 1 + i << " " << limit(ifloor(0x100 * limit<float>(outputs[i], 0, 1)), 0, 0xFF) << "\n";
        }
        string str = os.str();
        writer.writeBytes((const uint8_t *)str.c_str(), str.size());
    }
    writer.writeBytes((const uint8_t *)"-1\n", 3); // reset
}

void runMainDialog()
{
    shared_ptr<GUIContainer> gui = make_shared<GUIContainer>(-Display::scaleX(), Display::scaleX(), -Display::scaleY(), Display::scaleY());
    gui->add(make_shared<GUIButton>([&gui]()
    {
        outputs[4] = limit(outputs[4] + 0.01f, 0.0f, 1.0f);
    }, L"Speed Up", -0.4, 0.4, -0.5, -0.4));
    gui->add(make_shared<GUIButton>([&gui]()
    {
        outputs[4] = limit(outputs[4] - 0.01f, 0.0f, 1.0f);
    }, L"Slow Down", -0.4, 0.4, -0.6, -0.5));
    gui->add(make_shared<GUIButton>([&gui]()
    {
        outputs[4] = 0;
    }, L"Stop", -0.4, 0.4, -0.7, -0.6));
    gui->add(make_shared<GUIDynamicLabel>([]()->wstring
    {
        wostringstream os;
        os << L"Outputs :\n";
        int index = 1;
        for(auto & v : outputs)
        {
            os << index++ << ": " << ifloor(v * 100 + 0.5) << "%\n";
        }
        return os.str();
    }, -0.6, 0.6, 0, 0.8));
    gui->add(make_shared<GUIButton>([&gui]()
    {
        GUIRunner::get(gui)->quit();
    }, L"Quit", -0.4, 0.4, -0.9, -0.8));
    runAsDialog(gui);
}

int main()
{
    for(auto & v : outputs)
        v = 0;
    wstring fileName = L"/dev/ttyUSB0";
    shared_ptr<StreamRW> streams = make_shared<StreamRWWrapper>(make_shared<FileReader>(fileName), make_shared<FileWriter>(fileName));
    thread communicationThread(communicationThreadFn, streams);
    startGraphics();
    runMainDialog();
    endGraphics();
    done = true;
    communicationThread.join();
    return 0;
}
