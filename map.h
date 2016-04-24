#ifndef MAP_H
#define MAP_H

#include <irrlicht.h>
#include <iostream>
#include <vector>
#include <cassert>

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;
enum BlockType
{
    EMPTY = 0,
    DERT
};

static const int SECTOR_DIMENTION = 4;

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

struct MapSector
{
    static const int delta = 10;

public:
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

    bool isVisible(int i, int j, int k);
    void buildMesh(vector3df startPosition, vector<video::S3DVertex> &vertices, vector<u16> &indices, video::SColor color, int visible_layer);

};

class Map
{
    vector<MapSector*> sectors;

public:

    int global_layer;
    
    Map();
    void buildMesh(vector<video::S3DVertex> &vertices, vector<u16> &indices);
    bool readFromStream(iostream &stream);
    void readSector(iostream &stream, MapSector &sector);

};

#endif