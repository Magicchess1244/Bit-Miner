#include "ChunkManager.h"
#include "Chunck.h"
#include <iostream>
#include <map>
#include <queue>
#include <utility>

ChunckPrefab Chunks[100] = {0};
std::map<int, SDL_FColor> blockColorMap = {
	{0, { 0, 0.7f, 1, 1 }}, // Air
	{1, { 0, 0.6f, 0, 1 }}, // Grass
	{2, { 0.5f, 0.25f, 0, 1 }}, // Dirt
	{3, { 0.4f, 0.4f, 0.4f, 1 }}, // Stone
	{4, { 0.15f, 0.15f, 0.15f, 1 }}, // BedRock
	{5, { 0, 0.4f, 0.8f, 1}}, // Water
};

int BlockSize = 50;
bool isTransparent(int blockID)
{
	return blockID == 0 || blockID == 5;
}
bool canSwim(int blockID)
{
	return blockID == 5;
}
void PrintChunk(int i, int xPlayerPos, int yPlayerPos, int xRange, int yRange, int FullRange)
{
	int xSize = Chunks[i].xSize;
	int ySize = Chunks[i].ySize;
	int xPos = Chunks[i].xPos;

	for (int y = 0; y < yRange; y++) {
		for (int x = 0; x < xRange; x++) {
			std::cout << Chunks[i].Blocks[xPlayerPos + x][yPlayerPos + y];
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
}
void DrawChunk(int i, int xPlayerPos, int yPlayerPos, int xRange, int yRange, int FullRange, Mesh* mesh, bool FirstChunck)
{
	int xSize = Chunks[i].xSize;
	int ySize = Chunks[i].ySize;
	int xPos = Chunks[i].xPos;
	int quadIndex = 0;
	int OffSet = ((int)xPlayerPos - xPlayerPos);

	if (!FirstChunck) {
		OffSet += FullRange - xRange;
	}

	for (int y = 0; y < yRange; y++) {
		for (int x = 0; x < xRange; x++) {
			
			SDL_FColor Color;

			Color = blockColorMap[Chunks[i].Blocks[xPlayerPos + x][yPlayerPos + y]];
			
			int vIndex = quadIndex * 4;
			int iIndex = quadIndex * 6;
			float FlipX = (x + OffSet) * BlockSize;
			float FlipY = (yRange - y -1) * BlockSize;

			mesh->Vertices[vIndex + 0] = { {FlipX, FlipY}, Color };
			mesh->Vertices[vIndex + 1] = { {FlipX + BlockSize, FlipY}, Color};
			mesh->Vertices[vIndex + 2] = { {FlipX, FlipY + BlockSize}, Color};
			mesh->Vertices[vIndex + 3] = { {FlipX + BlockSize, FlipY + BlockSize}, Color};

			mesh->Indices[iIndex + 0] = vIndex + 0;
			mesh->Indices[iIndex + 1] = vIndex + 1;
			mesh->Indices[iIndex + 2] = vIndex + 2;
			mesh->Indices[iIndex + 3] = vIndex + 2;
			mesh->Indices[iIndex + 4] = vIndex + 1;
			mesh->Indices[iIndex + 5] = vIndex + 3;

			quadIndex++;
		}
	}
}
bool Collition(Vector2* PlayerPos, Vector2 Direction, int FullRange, int yRange, bool Swim, bool Block)
{

		int newX = PlayerPos->x + (int)(FullRange / 2.0f - 1) + Direction.x;
		int newY = PlayerPos->y + (int)(yRange / 2.0f) + Direction.y;

		int chunk = (int)(newX) / 16;
		int localX = (int)(newX % 16);
		int footY = (int)(newY);
		int headY = (int)(newY + 1);

		//std::cout << "Chunk: " << chunk << ", LocalX: " << localX << ", FootY: " << footY << ", HeadY: " << headY << ", Block Info: " << Chunks[chunk].Blocks[localX][footY] << std::endl;

		bool blockFoot = !isTransparent(Chunks[chunk].Blocks[localX][footY]);
		bool blockHead = !isTransparent(Chunks[chunk].Blocks[localX][headY]);

		if (Swim && !(blockFoot || blockHead)) {
			blockFoot = canSwim(Chunks[chunk].Blocks[localX][footY]);
			blockHead = canSwim(Chunks[chunk].Blocks[localX][headY]);
		}

		if (Block) {
			blockHead = false;
		}

		return (blockFoot || blockHead);
}
bool PlaceBlock(int BlockType, Vector2 Position, int yRange, Vector2 PlayerPosition, short* Type) 
{
	Position.x = (int)(Position.x / BlockSize ) + PlayerPosition.x;
	Position.y = (yRange - (int)(Position.y / BlockSize) - 1) + PlayerPosition.y;


	int CurrrentChunk = (int)floorf((Position.x) / 16);
	int RelativeX = (int)(Position.x) % 16;

	
	bool notBedRock = (Chunks[CurrrentChunk].Blocks[RelativeX][(int)Position.y] != 4);
	bool water = (Chunks[CurrrentChunk].Blocks[RelativeX][(int)Position.y] == 5);
	bool canPlace = (BlockType != 0 && (Chunks[CurrrentChunk].Blocks[RelativeX][(int)Position.y] == 0 || water));
	bool canBreak = (BlockType == 0 && (Chunks[CurrrentChunk].Blocks[RelativeX][(int)Position.y] != 0 && !water));

	//std::cout << canBreak << " " << canPlace << std::endl;

	if (notBedRock && (canPlace || canBreak)) {
		std::cout << "Placing Block: " << BlockType << ", Position: " << Position.x << ", " << Position.y << "Chunk: " << CurrrentChunk << std::endl;
		*Type = Chunks[CurrrentChunk].Blocks[RelativeX][(int)Position.y];
		Chunks[CurrrentChunk].Blocks[RelativeX][(int)Position.y] = BlockType;
		return true;
	}
	return false;
}
void Size(int PixelSizeX, int PixelSizeY, int yRange, int FullRange)
{
	if (PixelSizeX > PixelSizeY)
	{
		BlockSize = (int)(PixelSizeY / yRange);
	}
	else
	{
		BlockSize = (int)(PixelSizeX / FullRange);
	}
}
int GetHeight(int xPos)
{
	return Chunks[xPos].Height;
}
void ShowInventor(SDL_Renderer* Renderer, int width, int height, Block* Inventory, int InventorySlot)
{
	SDL_SetRenderDrawColor(Renderer, 157, 76, 0, 255);
	SDL_FRect InventoryRect = { (width / 2) - (BlockSize * 4), (height - (BlockSize * 1.3f)), (BlockSize * 1.1f) * 8 + (BlockSize * .1f), (BlockSize * 1.2f) };
	SDL_RenderFillRect(Renderer, &InventoryRect);

	for (int i = 0; i < 8; i++)
	{
		if (InventorySlot == i)
		{
			SDL_SetRenderDrawColor(Renderer, 255, 153, 56, 255);
		}
		else
		{
			SDL_SetRenderDrawColor(Renderer, 204, 102, 0, 255);
		}
		SDL_FRect InventoryRect = { ((width / 2) - (BlockSize * 4)) + (BlockSize * 1.1f * i) + (BlockSize * 0.1f), (height - (BlockSize * 1.3f)) + (BlockSize * 0.1f), BlockSize, BlockSize };
		SDL_RenderFillRect(Renderer, &InventoryRect);

		if (Inventory[i].Type != 0)
		{
			SDL_SetRenderDrawColor(Renderer, blockColorMap[Inventory[i].Type].r * 255, blockColorMap[Inventory[i].Type].g * 255, blockColorMap[Inventory[i].Type].b * 255, blockColorMap[Inventory[i].Type].a * 255);
			SDL_FRect BlockRect = { ((width / 2) - (BlockSize * 4)) + (BlockSize * 1.1f * i) + (BlockSize * 0.3f), (height - (BlockSize * 1.3f)) + (BlockSize * 0.3f), BlockSize * 0.6f, BlockSize * 0.6f };
			SDL_RenderFillRect(Renderer, &BlockRect);
			//Add be a number
		}

	}
}
void SimulateWater(int chunkIndex) {
	std::queue<std::pair<int, int> > q; 

	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 64; y++) {
			if (Chunks[chunkIndex].Blocks[x][y] == 5) {
				int globalX = chunkIndex * 16 + x;
				q.push(std::make_pair(globalX, y));
			}
		}
	}

	while (!q.empty()) {
		std::pair<int, int> pos = q.front();
		q.pop();

		int x = pos.first;
		int y = pos.second;

		int chunk = x / 16;
		int localX = x % 16;

		// Move down
		if (y - 1 >= 0 && (Chunks[chunk].Blocks[localX][y - 1] == 0 || Chunks[chunk].Blocks[localX][y - 1] == 5)) {
			Chunks[chunk].Blocks[localX][y - 1] = 5;
			q.push(std::make_pair(x, y - 1));
		}
		else {
			// Try left
			if (x - 1 >= 0) {
				int leftChunk = (x - 1) / 16;
				int leftX = (x - 1) % 16;
				if (Chunks[leftChunk].Blocks[leftX][y] == 0) {
					Chunks[leftChunk].Blocks[leftX][y] = 5;
					q.push(std::make_pair(x - 1, y));
				}
			}
			// Try right
			if (x + 1 < 1600) {
				int rightChunk = (x + 1) / 16;
				int rightX = (x + 1) % 16;
				if (Chunks[rightChunk].Blocks[rightX][y] == 0) {
					Chunks[rightChunk].Blocks[rightX][y] = 5;
					q.push(std::make_pair(x + 1, y));
				}
			}
		}
	}
}

void ChunkGenerator(int Chunk)
{
	if (Chunks[Chunk].xPos == -1) {
		Chunks[Chunk].xPos = Chunk * Chunks[Chunk].xSize;
		Chunks[Chunk].GenerateChunk();
		SimulateWater(Chunk);
		return;
	}
}