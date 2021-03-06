#include "map.h"


bool MapSector::isVisible(int i, int j, int k)
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



void MapSector::buildMesh(vector3df startPosition, vector<video::S3DVertex> &vertices, vector<u16> &indices, video::SColor color, int visible_layer)
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

//================================================================================================================

Map::Map()
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

void Map::buildMesh(vector<video::S3DVertex> &vertices, vector<u16> &indices)
{
    video::SColor color(255, 255, 0, 0);
    for (int i = 0; i < 2; i++)
    {
        for (auto sector: sectors)
        {
            sector->buildMesh(vector3df(sector->position.X*40,sector->position.Y*40,sector->position.Z*40), vertices, indices, color, global_layer);
        }
    }
    
}

bool Map::readFromStream(iostream &stream)
{
	int i,j,k;

	if (stream.bad()) {
		std::cout << "error: bad stream" << std::endl;
		return false;
	}

	stream >> i >> j >> k;

	std::cout << "i: " << i << "j: " << j << "k: " << k << endl;
	int sectorCount = 0;

	for (size_t x = 0; x < i; x++) {
		for (size_t y = 0; y < j; y++) {
			for (size_t z = 0; z < k; z++) {
				MapSector *sector = new MapSector({x,y,z});
				readSector(stream, *sector);
				sectors.push_back(sector);
				sectorCount++;
			}
		}
	}

	std::cout << "read finished: " << sectorCount << std::endl;
	return true;
}

void Map::readSector(iostream &stream, MapSector &sector)
{
	for (int i = 0; i < SECTOR_DIMENTION; i++)
		for (int j = 0; j < SECTOR_DIMENTION; j++)
			for (int k = 0; k < SECTOR_DIMENTION; k++) {
				unsigned int type;
				stream >> type;
				sector.blocks[i][j][k].type = (BlockType)type;
			}
}