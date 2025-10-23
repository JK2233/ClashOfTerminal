//MSVC exclusive pragma
#include <cstdio>
#pragma execution_character_set("utf-8")

//includes
#include <vector>
#include <string>
#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

#define COMPILE_WITH_TOOLS

#define RENDER_VERSION "b_0.3.1"

#define TRY_TO_RESOLVE_MINOR_ERRORS_INSTED_OF_CRASHING false
#define SCREEN_SIZE_X 200
#define SCREEN_SIZE_Y 60

#include "render.h"




#ifdef __linux__

#endif
#ifdef _WIN32

#endif


/*
render - specification, technical documantation and architecture

Notice: you should not set renderThreadBusIdle or mainThreadFinnishedDataTransfer direcly unless you know what your doing

The main thread can only send data to the render thrad when renderThreadBusIdle is true;
at the same time render thread can only start copying current frame data to ints internal buffer dater mainThreadFinnishedDataTransfer becomes false;
after reciving data render thread will pull renderThreadBusIdle to the true state informing the main thread that it can recive more data now.
BUT render thread WILL NOT do that if its still rendering a frame out, it will wait for that untill it finishes current frame and advances to the next.

Bottleneck cases:
    render thread is stuck rendering:
        - main thread will not me able to send additional data to the thread cuz renderThreadBusIdle will be false cosing a frame skip
    main thread is working:
        - render thread will simply await for the main thread to send the next frame over
*/



int main(int argc, char* argv[])
{
    render::Log("initializing the render namespace");
    render::Initialize();

    render::LockRenderThread.lock();
    render::VideoStreamMenager bootVideo = render::VideoStreamMenager("./bad apple/frame-#####.bmp", 6549, render::VideoStreamMenager::e_BMP);
    render::VideoStreems.push_back(bootVideo);
    render::ObjectsToRender.clear();
    render::LockRenderThread.unlock();

    static bool gameRunning = true;
    while (gameRunning) {
        {
            render::ScreenObject tmp;
            tmp.type = render::e_NewFrame;
            if (render::renderThreadBusIdle) {
                render::LockRenderThread.lock();
                render::ObjectsToRender.clear();
                render::ObjectsToRender.push_back(tmp);
                tmp.type = render::e_Square;
                tmp.posX = 0;
                tmp.posY = 0;
                tmp.sizeX = 255;
                tmp.SizeY = 255;
                tmp.character = U'🮕';
                tmp.bacColor = 0;
                tmp.forColor = 250;
                render::ObjectsToRender.push_back(tmp);
                tmp.type = render::e_Video;
                tmp.resorceID = 0;
                render::ObjectsToRender.push_back(tmp);
                //tmp.type = render::e_BadApple;
                //render::ObjectsToRender.push_back(tmp);
                render::LockRenderThread.unlock();
                render::mainThreadFinnishedDataTransfer = true;
            }
            std::this_thread::sleep_for (std::chrono::milliseconds(15)); // Sleep 16 ms
        }
    }
    render::DeInitialize();
    return 0;
}