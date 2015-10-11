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

class MyEventReceiver : public IEventReceiver
{
public:
    // This is the one method that we have to implement
    virtual bool OnEvent(const SEvent& event)
    {
        // Remember whether each key is down or up
        if (event.EventType == irr::EET_KEY_INPUT_EVENT)
            KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

        return false;
    }

    // This is used to check whether a key is being held down
    virtual bool IsKeyDown(EKEY_CODE keyCode) const
    {
        return KeyIsDown[keyCode];
    }
    
    MyEventReceiver()
    {
        for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
            KeyIsDown[i] = false;
    }

private:
    // We use this array to store the current state of each key
    bool KeyIsDown[KEY_KEY_CODES_COUNT];
};

int main(int argc, char *args[])
{
    std::cout << "test1" << std::endl;

    // ask user for driver
    video::E_DRIVER_TYPE driverType=driverChoiceConsole();
    if (driverType==video::EDT_COUNT)
        return 1;

	MyEventReceiver receiver;

    IrrlichtDevice *device = createDevice(driverType,
            core::dimension2d<u32>(640, 480), 16, false, false, false, &receiver);
        
    if (device == 0)
        return 1; // could not create selected driver.

    // create engine and camera

    device->setWindowCaption(L"Custom Scene Node - Irrlicht Engine Demo");

    video::IVideoDriver* driver = device->getVideoDriver();
    scene::ISceneManager* smgr = device->getSceneManager();

    ICameraSceneNode * camera;

    camera = smgr->addCameraSceneNode(0, core::vector3df(0,-40,0), core::vector3df(0,0,0));
    camera->bindTargetAndRotation(false);
    SMesh *mesh = new SMesh();
/////
    video::S3DVertex Vertices[4];
    Vertices[0] = video::S3DVertex(0,0,10, 1,1,0,
            video::SColor(255,0,255,255), 0, 1);
    Vertices[1] = video::S3DVertex(10,0,-10, 1,0,0,
            video::SColor(255,255,0,255), 1, 1);
    Vertices[2] = video::S3DVertex(0,20,0, 0,1,1,
            video::SColor(255,255,255,0), 1, 0);
    Vertices[3] = video::S3DVertex(-10,0,-10, 0,0,1,
            video::SColor(255,0,255,0), 0, 0);

    u16 indices[] = {   0,2,3, 2,1,3, 1,0,3, 2,0,1  };

    video::SMaterial material;
    material.Wireframe = false;
    material.Lighting = false;    

    scene::SMeshBuffer *buf = new scene::SMeshBuffer();
    buf->Material = material;
    buf->append(Vertices, 4, indices, 12);
    mesh->addMeshBuffer(buf);
////

    smgr->addMeshSceneNode(mesh);

    int frameDeltaTime = 1;
    const int MOVEMENT_SPEED = 1;

    while(device->run())
    {

        core::vector3df nodePosition = camera->getPosition();
        core::vector3df nodeTarget = camera->getTarget();

        if(receiver.IsKeyDown(irr::KEY_KEY_W)) {
            nodePosition.Z += MOVEMENT_SPEED * frameDeltaTime;
            nodeTarget.Z += MOVEMENT_SPEED * frameDeltaTime;
        }
        else if(receiver.IsKeyDown(irr::KEY_KEY_S)) {
            nodePosition.Z -= MOVEMENT_SPEED * frameDeltaTime;
            nodeTarget.Z -= MOVEMENT_SPEED * frameDeltaTime;
        }

        if(receiver.IsKeyDown(irr::KEY_KEY_A)) {
            nodePosition.X -= MOVEMENT_SPEED * frameDeltaTime;
            nodeTarget.X -= MOVEMENT_SPEED * frameDeltaTime;
        }
        else if(receiver.IsKeyDown(irr::KEY_KEY_D)) {
            nodePosition.X += MOVEMENT_SPEED * frameDeltaTime;
            nodeTarget.X += MOVEMENT_SPEED * frameDeltaTime;
        }

        if (receiver.IsKeyDown(irr::KEY_KEY_Q)) {
        	break;
        }

        camera->setPosition(nodePosition);
        camera->setTarget(nodeTarget);

        driver->beginScene(true, true, SColor(255,100,101,140));

        smgr->drawAll();
//        guienv->drawAll();

        driver->endScene();
    }

    device->drop();

    return 0;
}