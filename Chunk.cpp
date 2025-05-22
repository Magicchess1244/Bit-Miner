#include "Chunck.h"
#include "PerlinNoise.h"
#include "BiomeBuilder.h"


void ChunckPrefab::ShowChunk()
{
	for (int y = ySize - 1; y > 0; y--) {
		for (int x = 0; x < xSize; x++) {
			if (Blocks[x][y] == 1) {
				std::cout << "O";
			}
			else {
				std::cout << " ";
			}
		}
		std::cout << std::endl;
	}
}
void ChunckPrefab::GenerateChunk()
{
	GenerateChunkSurface();
	GenerateChunkCaves();
}
void ChunckPrefab::GenerateChunkSurface()
{
	Height = 30 + (PerlinNoise(xPos, 0, 4, 0, 0.1f) * 25);
	for (int x = 0; x < this->xSize; x++) {
		float Height = 35 + (PerlinNoise(xPos + x, 0, 4, 0, 0.1f) * 25);
		int ActualHeight = (int)Height;

		if (Height < 33) {
			Height = 33;
		}
		for (int y = (int)Height; y > -1; y--) {
			if (y <= ActualHeight)
			{
				if (y == 0) {
					Blocks[x][y] = 4;
				}
				else if (y == (int)Height) {
					Blocks[x][y] = 1;
				}
				else if (y > (int)Height - 4)
				{
					Blocks[x][y] = 2;
				}
				else if (y < 2)
				{
					Blocks[x][y] = 4;
				}
				else
				{
					Blocks[x][y] = 3;
				}
			} else {
				Blocks[x][y] = 5;
			}

		}
	}
}
void ChunckPrefab::GenerateChunkCaves()
{
	for (int x = 0; x < this->xSize; x++) {
		for (int y = 2; y < this->ySize; y++)
		{
			float Hole = PerlinNoise(xPos + x, y, 3, 3, 0.1f);

			bool CheeseCave = Hole <= -0.9f || Hole >= 0.9f;
			bool NodleCave = (0.04f > Hole && Hole > -0.04f);

			if (CheeseCave || NodleCave) {
				Blocks[x][y] = 0;
			}
		}
	}
}