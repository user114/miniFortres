#include <iostream>
#include <vector>
#include <cassert>

#include <irrlicht.h>
#include "driverChoice.h"

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

enum BlockType
{
    EMPTY,
    DERT
};

struct MapBlock
{
    BlockType type;

    MapBlock()
    : type(DERT)
    {}
    MapBlock(BlockType type)
    : type(type)
    {}

};

static const int SECTOR_DIMENTION = 4;

struct MapSector
{
    MapBlock blocks[SECTOR_DIMENTION][SECTOR_DIMENTION][SECTOR_DIMENTION];
    MapBlock &operator()(unsigned int x, unsigned int y, unsigned int z)
    {
        assert(x < SECTOR_DIMENTION && y < SECTOR_DIMENTION && z < SECTOR_DIMENTION);
        return blocks[x][y][z];
    }

    static const int delta = 10;

    void buildMesh(vector<video::S3DVertex> &vertices, vector<u16> &indices)
    {
        for (int i = 0; i < SECTOR_DIMENTION; i++)
        {
            for (int j = 0 ; j < SECTOR_DIMENTION; j++)
            {
                for (int k = 0; k < SECTOR_DIMENTION; k++)
                {
                    if (blocks[i][j][k].type != EMPTY)
                    {
                        u16 si = vertices.size();
                        vertices.push_back(video::S3DVertex(i*delta,j*delta,k*delta, 1,1,0, video::SColor(255,0,255,255), 0, 1));
                        vertices.push_back(video::S3DVertex(i*delta+delta,j*delta,k*delta, 1,0,0, video::SColor(255,255,0,255), 1, 1));
                        vertices.push_back(video::S3DVertex(i*delta,j*delta+delta,k*delta, 0,1,1, video::SColor(255,255,255,0), 1, 0));
                        vertices.push_back(video::S3DVertex(i*delta+delta,j*delta+delta,k*delta, 0,0,1, video::SColor(255,0,255,0), 0, 0));

                        vertices.push_back(video::S3DVertex(i*delta,j*delta,k*delta+delta, 1,1,0, video::SColor(255,0,255,255), 0, 1));
                        vertices.push_back(video::S3DVertex(i*delta+delta,j*delta,k*delta+delta, 1,0,0, video::SColor(255,255,0,255), 1, 1));
                        vertices.push_back(video::S3DVertex(i*delta,j*delta+delta,k*delta+delta, 0,1,1, video::SColor(255,255,255,0), 1, 0));
                        vertices.push_back(video::S3DVertex(i*delta+delta,j*delta+delta,k*delta+delta, 0,0,1, video::SColor(255,0,255,0), 0, 0));                        

                        u16 ind[] = {   si,si+3,si+1, si,si+2,si+3, si+4,si+5,si+7, si+4,si+7,si+6
//                                        si,si+4,si+3, si+3,si+4,si+7, si+6,si+5,si+1, si+2,si+6,si+1,
//                                        si+3,si+7,si+2, si+2,si+7,si+6, si+5,si+4,si+1, si+4,si,si+1
                                    };

                        indices.insert(indices.end(), ind, ind + sizeof(ind)/sizeof(u16));
                    }
                }
            }
        }
    }
};

class Map
{
    MapSector sector;
public:
    Map()
    {
        sector(0,0,0) = DERT;
        sector(0,0,1) = DERT;
        sector(0,0,2) = DERT;
        sector(0,0,3) = DERT;
        sector(1,0,3) = DERT;
        sector(1,1,3) = DERT;
        sector(3,3,3) = DERT;
    }

    void buildMesh(vector<video::S3DVertex> &vertices, vector<u16> &indices)
    {
        sector.buildMesh(vertices, indices);
    }
};

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

    Map map;

    // ask user for driver
    video::E_DRIVER_TYPE driverType=EDT_OPENGL;

    MyEventReceiver receiver;

    IrrlichtDevice *device = createDevice(driverType,
            dimension2d<u32>(640, 480), 16, false, false, false, &receiver);
        
    if (device == 0)
        return 1; // could not create selected driver.

    // create engine and camera

    device->setWindowCaption(L"Custom Scene Node - Irrlicht Engine Demo");

    video::IVideoDriver* driver = device->getVideoDriver();
    scene::ISceneManager* smgr = device->getSceneManager();

    ICameraSceneNode * camera;

    camera = smgr->addCameraSceneNode(0, vector3df(-100,-100,100), vector3df(0,0,0));
    camera->bindTargetAndRotation(false);
    SMesh *mesh = new SMesh();
/////
    vector<video::S3DVertex> vertices;
    vector<u16> indices;

    map.buildMesh(vertices, indices);

    video::SMaterial material;
    material.Wireframe = false;
    material.Lighting = false;    

    scene::SMeshBuffer *buf = new scene::SMeshBuffer();
    buf->Material = material;
    buf->append(vertices.data(), vertices.size(), indices.data(), indices.size());
    mesh->addMeshBuffer(buf);
////

    smgr->addMeshSceneNode(mesh);

    int frameDeltaTime = 1;
    const int MOVEMENT_SPEED = 1;

    while(device->run())
    {

        vector3df nodePosition = camera->getPosition();
        vector3df nodeTarget = camera->getTarget();

        if(receiver.IsKeyDown(irr::KEY_KEY_A)) {
            nodePosition.Z += MOVEMENT_SPEED * frameDeltaTime;
            nodeTarget.Z += MOVEMENT_SPEED * frameDeltaTime;
        }
        else if(receiver.IsKeyDown(irr::KEY_KEY_D)) {
            nodePosition.Z -= MOVEMENT_SPEED * frameDeltaTime;
            nodeTarget.Z -= MOVEMENT_SPEED * frameDeltaTime;
        }

        if(receiver.IsKeyDown(irr::KEY_KEY_W)) {
            nodePosition.X -= MOVEMENT_SPEED * frameDeltaTime;
            nodeTarget.X -= MOVEMENT_SPEED * frameDeltaTime;
        }
        else if(receiver.IsKeyDown(irr::KEY_KEY_S)) {
            nodePosition.X += MOVEMENT_SPEED * frameDeltaTime;
            nodeTarget.X += MOVEMENT_SPEED * frameDeltaTime;
        }
        else if(receiver.IsKeyDown(irr::KEY_KEY_Q)) {
//            nodePosition.X += MOVEMENT_SPEED * frameDeltaTime;
            nodePosition.Y += MOVEMENT_SPEED * frameDeltaTime;
        }
        else if(receiver.IsKeyDown(irr::KEY_KEY_E)) {
//            nodePosition.X += MOVEMENT_SPEED * frameDeltaTime;
            nodePosition.Y -= MOVEMENT_SPEED * frameDeltaTime;
        }

        if (receiver.IsKeyDown(irr::KEY_ESCAPE)) {
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