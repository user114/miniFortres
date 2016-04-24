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
    vector3df position;

    MapSector(vector3df pos = {0,0,0})
        : position(pos)
    {

    }

    MapBlock &operator()(unsigned int x, unsigned int y, unsigned int z)
    {
        assert(x < SECTOR_DIMENTION && y < SECTOR_DIMENTION && z < SECTOR_DIMENTION);
        return blocks[x][y][z];
    }

    bool isVisible(int i, int j, int k)
    {
        if (i == 0 || j == 0 || k == 0 || i == SECTOR_DIMENTION - 1 || j == SECTOR_DIMENTION - 1 || k == SECTOR_DIMENTION - 1)
            return true;
        if (blocks[i+1][j][k].type == EMPTY)
            return true;
        if (blocks[i-1][j][k].type == EMPTY)
            return true;
        if (blocks[i][j-1][k].type == EMPTY)
            return true;
        if (blocks[i][j+1][k].type == EMPTY)
            return true;
        if (blocks[i][j][k+1].type == EMPTY)
            return true;
        if (blocks[i][j][k-1].type == EMPTY)
            return true;

        return false;
    }

    static const int delta = 10;

    void buildMesh(vector3df startPosition, vector<video::S3DVertex> &vertices, vector<u16> &indices, video::SColor color, int visible_layer)
    {
        video::SColor colors[SECTOR_DIMENTION] = {video::SColor(255, 255, 0, 0), video::SColor(255, 0, 255, 0), 
                                                  video::SColor(255, 0, 0, 255), video::SColor(255, 255, 255, 0)};

        for (int i = 0; i < SECTOR_DIMENTION; i++)
        {
            for (int j = 0 ; j < SECTOR_DIMENTION; j++)
            {
                for (int k = 0; k < SECTOR_DIMENTION; k++)
                {
                    if (blocks[i][j][k].type != EMPTY && k < visible_layer && isVisible(i,j,k))
                    {
                        u16 si = vertices.size();
                        vertices.push_back(video::S3DVertex(startPosition + vector3df(i*delta,j*delta,k*delta), vector3df(-1,-1,-1), colors[k], vector2d<f32>(0, 1)));
                        vertices.push_back(video::S3DVertex(startPosition + vector3df(i*delta+delta,j*delta,k*delta), vector3df(-1,-1,1), colors[k], vector2d<f32>(1, 1)));
                        vertices.push_back(video::S3DVertex(startPosition + vector3df(i*delta,j*delta+delta,k*delta), vector3df(1,-1,-1), colors[k], vector2d<f32>(1, 0)));
                        vertices.push_back(video::S3DVertex(startPosition + vector3df(i*delta+delta,j*delta+delta,k*delta), vector3df(1,-1,1), colors[k], vector2d<f32>(0, 0)));

                        vertices.push_back(video::S3DVertex(startPosition + vector3df(i*delta,j*delta,k*delta+delta), vector3df(-1,1,-1), colors[k], vector2d<f32>(0, 1)));
                        vertices.push_back(video::S3DVertex(startPosition + vector3df(i*delta+delta,j*delta,k*delta+delta), vector3df(-1,1,1), colors[k], vector2d<f32>(1, 1)));
                        vertices.push_back(video::S3DVertex(startPosition + vector3df(i*delta,j*delta+delta,k*delta+delta), vector3df(1,1,-1), colors[k], vector2d<f32>(1, 0)));
                        vertices.push_back(video::S3DVertex(startPosition + vector3df(i*delta+delta,j*delta+delta,k*delta+delta), vector3df(1,1,1), colors[k], vector2d<f32>(0, 0)));

                        u16 ind[] = {   si,si+3,si+1, si,si+2,si+3, si+4,si+5,si+7, si+4,si+7,si+6,
                                        si,si+4,si+2, si+4,si+6,si+2, si+5,si+1,si+3, si+5,si+3,si+7,
                                        si+2,si+6,si+7, si+2,si+7,si+3, si+5,si+4,si, si+5,si,si+1
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

    int global_layer;

    Map()
        : global_layer(4)
    {
        // sector(0,0,0) = EMPTY;
        // sector(0,0,1) = EMPTY;
        // sector(1,1,2) = EMPTY;
        // sector(0,0,3) = EMPTY;
        // sector(1,0,3) = EMPTY;
        // sector(1,1,3) = EMPTY;
        // sector(3,3,3) = EMPTY;
    }

    void buildMesh(vector<video::S3DVertex> &vertices, vector<u16> &indices)
    {
        video::SColor color(255, 255, 0, 0);
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                sector.buildMesh(vector3df(i*40,j*40,0), vertices, indices, color, global_layer);
            }
        }
        
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

IMeshSceneNode *rebuildMesh(scene::ISceneManager* smgr, Map &map, IMeshSceneNode * mesh_node);

int main(int argc, char *args[])
{
    Map map;
    float angle = 0;
    ILightSceneNode * sun_node;
    SLight sun_data;

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

    camera = smgr->addCameraSceneNode(0, vector3df(-100,0,100), vector3df(0,0,0));
    camera->bindTargetAndRotation(false);
    camera->setUpVector(vector3df(0,0,1));
    IMeshSceneNode * mesh_node = 0;

    mesh_node = rebuildMesh(smgr, map, 0);

    smgr->setShadowColor(video::SColor(150,0,0,0));    

    sun_node=smgr->addLightSceneNode();
    sun_data.Direction=vector3df(0,1,1);
    sun_data.Type=video::ELT_DIRECTIONAL;

    sun_data.AmbientColor=video::SColorf(0.6f,0.6f,0.6f,1);
    sun_data.SpecularColor=video::SColorf(0.8f,0.8f,0.8f,1);
    sun_data.DiffuseColor=video::SColorf(1.0f,1.0f,1.0f,1);
    sun_data.CastShadows=false;
    sun_node->setPosition(vector3df(0,0,0));
    sun_node->setRotation(vector3df(1,1,0));
    sun_node->setRadius(800);
    sun_node->setLightData(sun_data);
    sun_node->setLightType(video::ELT_DIRECTIONAL);


    int frameDeltaTime = 1;
    const int MOVEMENT_SPEED = 2;
    const int STEP_ANGLE = 2;
    bool need_rebuild_mesh = false;
    bool layer_button_not_pressed = true;

    while(device->run())
    {

        vector3df nodePosition = camera->getPosition();
        vector3df up = camera->getUpVector();
        vector3df nodeTarget = camera->getTarget();

        vector3df dir = nodeTarget - nodePosition;
        dir.normalize();

        vector3df moveTo;

        if(receiver.IsKeyDown(irr::KEY_KEY_A)) {
            vector3df left = up.crossProduct(dir);
            left.normalize();

            float distanse = MOVEMENT_SPEED * frameDeltaTime;
            left *= distanse;
            left.Z = 0;

            nodePosition += left;
            nodeTarget += left;
        }
        else if(receiver.IsKeyDown(irr::KEY_KEY_D)) {
            vector3df left = up.crossProduct(dir);
            left.normalize();

            float distanse = MOVEMENT_SPEED * frameDeltaTime;
            left *= distanse;
            left.Z = 0;

            nodePosition -= left;
            nodeTarget -= left;

        }

        if(receiver.IsKeyDown(irr::KEY_KEY_W)) 
        {
            float distanse = MOVEMENT_SPEED * frameDeltaTime;
            dir *= distanse;
            dir.Z = 0;
            nodePosition -= dir;
            nodeTarget -= dir;
        }
        else if(receiver.IsKeyDown(irr::KEY_KEY_S)) 
        {
            float distanse = MOVEMENT_SPEED * frameDeltaTime;
            dir *= distanse;
            dir.Z = 0;
            nodePosition += dir;
            nodeTarget += dir;
        }
        else if(receiver.IsKeyDown(irr::KEY_KEY_Q)) 
        {
            nodePosition.rotateXYBy(STEP_ANGLE, nodeTarget);
        }
        else if(receiver.IsKeyDown(irr::KEY_KEY_E)) 
        {
            nodePosition.rotateXYBy(-STEP_ANGLE, nodeTarget);        
        }
        else if(receiver.IsKeyDown(irr::KEY_KEY_R) && layer_button_not_pressed) 
        {
            if (map.global_layer<4)
            {
                map.global_layer++;
                need_rebuild_mesh = true;
            }
            layer_button_not_pressed = false;
        }
        else if(receiver.IsKeyDown(irr::KEY_KEY_F) && layer_button_not_pressed) 
        {
            if (map.global_layer > 0)
            {
                map.global_layer --;
                need_rebuild_mesh = true;
            }
            layer_button_not_pressed = false;
        }
        else
        {
            layer_button_not_pressed = true;
        }

        if (receiver.IsKeyDown(irr::KEY_ESCAPE)) 
        {
            break;
        }

        if (need_rebuild_mesh)
        {
            mesh_node = rebuildMesh(smgr, map, mesh_node);
            need_rebuild_mesh = false;
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

IMeshSceneNode *rebuildMesh(scene::ISceneManager* smgr, Map &map, IMeshSceneNode * mesh_node)
{
    if (mesh_node)
        mesh_node->remove();

    SMesh *new_mesh = new SMesh();

    vector<video::S3DVertex> vertices;
    vector<u16> indices;

    map.buildMesh(vertices, indices);

    video::SMaterial material;
    material.Lighting = true;

    material.NormalizeNormals=true;
    material.AmbientColor=SColor(255,255,255,255);
    material.DiffuseColor=SColor(255,0,0,0);
    material.EmissiveColor=SColor(255,0,0,0);
    material.SpecularColor=SColor(255,0,0,0);

    scene::SMeshBuffer *buf = new scene::SMeshBuffer();
    buf->Material = material;
    buf->append(vertices.data(), vertices.size(), indices.data(), indices.size());
    new_mesh->addMeshBuffer(buf);

    return smgr->addMeshSceneNode(new_mesh);
}