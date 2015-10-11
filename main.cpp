#include <iostream>
#include <vector>

#include <irrlicht.h>
#include "driverChoice.h"

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

int main(int argc, char *args[])
{
    std::cout << "main" << std::endl;

    // ask user for driver
    video::E_DRIVER_TYPE driverType=driverChoiceConsole();
    if (driverType==video::EDT_COUNT)
        return 1;

    IrrlichtDevice *device = createDevice(driverType,
            core::dimension2d<u32>(640, 480), 16, false);
        
    if (device == 0)
        return 1; // could not create selected driver.

    // create engine and camera

    device->setWindowCaption(L"Custom Scene Node - Irrlicht Engine Demo");

    video::IVideoDriver* driver = device->getVideoDriver();
    scene::ISceneManager* smgr = device->getSceneManager();

    smgr->addCameraSceneNode(0, core::vector3df(0,-40,0), core::vector3df(0,0,0));

    while(device->run())
    {
        driver->beginScene(true, true, SColor(255,100,101,140));

        smgr->drawAll();
//        guienv->drawAll();

        driver->endScene();
    }

    device->drop();

    return 0;
}