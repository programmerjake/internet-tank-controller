#include <thread>
#include <atomic>
#include <array>
#include <chrono>
#include <sstream>
#include <iostream>
#include "platform.h"
#include "gui.h"
#include "stream.h"
#include "serial.h"

using namespace std;

array<atomic<float>, 5> outputs;
atomic_bool done(false);

void writeToStream(string str, shared_ptr<Writer> pwriter)
{
    for(char ch : str)
    {
        pwriter->writeByte((uint8_t)ch);
        pwriter->flush();
    }
}

void readerThreadFn(shared_ptr<Reader> preader)
{
    Reader & reader = *preader;
    while(!done)
    {
        cout << (char)reader.readByte() << flush;
    }
}

void communicationThreadFn(shared_ptr<StreamRW> streams)
{
    thread readerThread(readerThreadFn, streams->preader());
    shared_ptr<Writer> pwriter = streams->pwriter();
    writeToStream("  -1  -1  ", pwriter); // reset
    while(!done)
    {
        this_thread::sleep_for(chrono::nanoseconds((uint64_t)floor(0.025 * 1e9)));
        ostringstream os;
        for(size_t i = 0; i < outputs.size(); i++)
        {
            os << 1 + i << "  " << limit(ifloor(0x100 * limit<float>(outputs[i], 0, 1)), 0, 0xFF) << "  ";
        }
        os << "0  0  0  ";
        writeToStream(os.str(), pwriter);
    }
    writeToStream("-1  ", pwriter); // reset
    readerThread.join();
}

void runMainDialog()
{
    shared_ptr<GUIContainer> gui = make_shared<GUIContainer>(-Display::scaleX(), Display::scaleX(), -Display::scaleY(), Display::scaleY());
    for(int i = 0; i < (int)outputs.size(); i++)
    {
        gui->add(make_shared<GUIScrollBar>([i]()->float{return outputs[i];}, [i](float v){outputs[i] = v;}, -0.4, 0.4, -0.09 - 0.1 * i, -0.1 * i));
    }
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

int main(int argc, char ** argv)
{
    for(auto & v : outputs)
        v = 0;
    wstring fileName = L"/dev/ttyUSB0";
    if(argv[1])
        fileName = stringToWString(argv[1]);
    auto fileWriter = make_shared<SerialWriter>(fileName);
    auto fileReader = make_shared<SerialReader>(fileName);
    shared_ptr<StreamRW> streams = make_shared<StreamRWWrapper>(fileReader, fileWriter);
    thread communicationThread(communicationThreadFn, streams);
    startGraphics();
    runMainDialog();
    endGraphics();
    done = true;
    communicationThread.join();
    return 0;
}
