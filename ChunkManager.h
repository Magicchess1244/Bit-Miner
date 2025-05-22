#ifndef	__CHUNKMANAGER_HPP__
#define __CHUNKMANAGER_HPP__

#include <map>
#include <SDL3/SDL.h>
#include "PerlinNoise.h"

typedef struct
{
	SDL_Vertex Vertices[512 * 4];
	int Indices[512 * 6];
} Mesh;

typedef struct  {
	short Amount;
	short Type;
} Block;


extern int BlockSize;

void ChunkGenerator(int Chunk);
void DrawChunk(int i, int xPlayerPos, int yPlayerPos, int xRange, int yRange, int FullRange, Mesh* mesh, bool FirstChunck);
void PrintChunk(int i, int xPlayerPos, int yPlayerPos, int xRange, int yRange, int FullRange);
bool Collition(Vector2* PlayerPos, Vector2 Direction, int FullRange, int yRange, bool Swim, bool Block);
bool PlaceBlock(int BlockType, Vector2 Position, int yRange, Vector2 PlayerPosition, short* Type);
void Size(int PixelSizeX, int PixelSizeY, int yRange, int FullRange);
int GetHeight(int xPos);
void ShowInventor(SDL_Renderer* Renderer, int width, int height, Block* Inventory, int InventorySlot);
void SimulateWater(int chunkIndex);
#endif