#include "ChunkManager.h"
#include "PerlinNoise.h"
#include <SDL3/SDL.h>
#include <iostream>

const int FullRange = 16;
const int yRange = 10;


int FindSlot(Block* Inventory, short Type) {
	for (int i = 0; i < 8; i++) {
		if ((Inventory[i].Type == Type || Inventory[i].Type == 0) && Inventory[i].Amount < 69) {
			Inventory[i].Type = Type;
			return i;
		}
	}
}
void Input(Vector2* PlayerDirection, bool OnGround, int* InventorySlots, Vector2* PlayerPos) {  
	const bool* KeyboardState = SDL_GetKeyboardState(NULL);  

	PlayerDirection->x = 0;  
	
	if (KeyboardState[SDL_SCANCODE_A] || KeyboardState[SDL_SCANCODE_LEFT]) {  
		PlayerDirection->x = -1;  
	}  
	if (KeyboardState[SDL_SCANCODE_D] || KeyboardState[SDL_SCANCODE_RIGHT]) {  
		PlayerDirection->x = 1;  
	}  

	if ((KeyboardState[SDL_SCANCODE_W] || KeyboardState[SDL_SCANCODE_UP] || KeyboardState[SDL_SCANCODE_SPACE]) && Collition(PlayerPos, { 0, -1 }, FullRange, yRange, true, false)) {
		PlayerDirection->y = 1;  
	}  

	if (KeyboardState[SDL_SCANCODE_1]) {  
		*InventorySlots = 0;  
	} else if (KeyboardState[SDL_SCANCODE_2]) {  
		*InventorySlots = 1;  
	} else if (KeyboardState[SDL_SCANCODE_3]) {
		*InventorySlots = 2;
	} else if (KeyboardState[SDL_SCANCODE_4]) {
		*InventorySlots = 3;
	} else if (KeyboardState[SDL_SCANCODE_5]) {
		*InventorySlots = 4;
	} else if (KeyboardState[SDL_SCANCODE_6]) {
		*InventorySlots = 5;
	} else if (KeyboardState[SDL_SCANCODE_7]) {
		*InventorySlots = 6;
	} else if (KeyboardState[SDL_SCANCODE_8]) {
		*InventorySlots = 7;
	}
}
void DrawBG(SDL_Renderer* Renderer, Vector2* PlayerPos){
	int CurrentChunck = (int)floorf((PlayerPos->x) / 16);
	int RelativeX = (int)(PlayerPos->x) % 16;
	int xRange = FullRange - RelativeX;
	Mesh mesh = { 0 };
	ChunkGenerator(CurrentChunck);
	if (PlayerPos->y == 1000) {
		PlayerPos->y = GetHeight(CurrentChunck);
		//std::cout << "PlayerPos: " << PlayerPos.x << ", " << PlayerPos.y << std::endl;
	}

	//std::cout << "PlayerPos: " << PlayerPos.x << ", " << PlayerPos.y << std::endl;
	//std::cout << "CurrentChunck: " << CurrentChunck << std::endl;

	DrawChunk(CurrentChunck, RelativeX, PlayerPos->y, xRange, yRange, FullRange, &mesh, true);
	SDL_RenderGeometry(Renderer, nullptr, mesh.Vertices, xRange * yRange * 4, mesh.Indices, xRange * yRange * 6);
	//PrintChunk(CurrentChunck, 0, 0, xRange, 64, FullRange);


	if (RelativeX > 0)
	{
		ChunkGenerator(CurrentChunck + 1);
		xRange = RelativeX;
		RelativeX = 0;
		CurrentChunck++;

		DrawChunk(CurrentChunck, RelativeX, PlayerPos->y, xRange, yRange, FullRange, &mesh, false);
		SDL_RenderGeometry(Renderer, nullptr, mesh.Vertices, xRange * yRange * 4, mesh.Indices, xRange * yRange * 6);
		//PrintChunk(CurrentChunck, RelativeX, PlayerPos.y, xRange, yRange, FullRange);
	}

}
void DrawPlayer(SDL_Renderer* Renderer) {

	SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);
	SDL_FRect PlayerRect = { (FullRange / 2 - 1) * BlockSize, (yRange / 2 - 2) * BlockSize, BlockSize, BlockSize * 2};
	SDL_RenderFillRect(Renderer, &PlayerRect);
}
int Ui()
{
	int width = 600;
	int height = 400;

	Size(width, height, yRange, FullRange);

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		std::cout << "Error initializing SDL: " << SDL_GetError();
		return 1;
	}

	SDL_Window* Window = SDL_CreateWindow("Bit Miner", width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);// | SDL_WINDOW_MAXIMIZED);
	if (Window == NULL) {
		std::cout << "Error creating window: " << SDL_GetError();
		SDL_Quit();
		return 1;
	}
	
	SDL_Renderer* Renderer = SDL_CreateRenderer(Window, NULL);
	if (Renderer == NULL) {
		std::cout << "Error creating renderer: " << SDL_GetError();
		SDL_DestroyWindow(Window);
		SDL_Quit();
		return 1;
	}

	bool FullScreen = false;
	bool Running = true;
	Vector2 PlayerPos = { 800, 1000 };
	Vector2 PlayerDirection = { 0, 0 };
	SDL_Event Event;
	Block Inventory[8] = { {60, 1}, {5 , 5}, 0, 0, 0, 0, 0, 0 };
	int InventorySlot = 0;

	while (Running){

		PlayerDirection.x = 0;
		bool OnGround = Collition(&PlayerPos, { 0, -1 }, FullRange, yRange, false, false);

		if (!OnGround) {
			PlayerDirection.y -= 0.5f;
		} else {
			PlayerDirection.y = 0;
		}

		while (SDL_PollEvent(&Event)) {
			if (Event.type == SDL_EVENT_QUIT) {
				Running = false;
				break;
			} else if (Event.type == SDL_EVENT_WINDOW_RESIZED) {
				width = Event.window.data1;
				height = Event.window.data2;

				Size(width, height, yRange, FullRange);
			}

			if (Event.type == SDL_EVENT_KEY_DOWN)
			{
				SDL_Keycode KeyPressed = Event.key.scancode;

				if (KeyPressed == SDL_SCANCODE_ESCAPE) {
					Running = false;
					break;
				}
				else if (KeyPressed == SDL_SCANCODE_F11) {
					FullScreen = !FullScreen;
					SDL_SetWindowFullscreen(Window, FullScreen);
				}
			}
			else if (Event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
				if (Event.button.button == SDL_BUTTON_LEFT) {
					short Type = 0;
					PlaceBlock(0, { Event.button.x, Event.button.y }, yRange, PlayerPos, &Type);
					if (Type != 0)
					{
						Inventory[FindSlot(Inventory, Type)].Amount++;
						SimulateWater((int)((Event.button.x / BlockSize) + PlayerPos.x) / 16);

					}
				}
				else if (Event.button.button == SDL_BUTTON_RIGHT && Inventory[InventorySlot].Amount > 0) {
					short Type = NULL;
					if (PlaceBlock(Inventory[InventorySlot].Type, { Event.button.x, Event.button.y }, yRange, PlayerPos, &Type))
					{
						//std::cout << Type << std::endl;
						Inventory[InventorySlot].Amount--;
						SimulateWater((int)((Event.button.x / BlockSize) + PlayerPos.x) / 16);


						if (Inventory[InventorySlot].Amount == 0)
						{
							Inventory[InventorySlot].Type = 0;
						}
					}

				}
			}
		}

		Input(&PlayerDirection, OnGround, &InventorySlot, &PlayerPos);

		PlayerDirection.x = SDL_clamp(PlayerDirection.x, -1, 1);
		PlayerDirection.y = SDL_clamp(PlayerDirection.y, -1, 1);

		if (PlayerDirection.x != 0  && !Collition(&PlayerPos, { PlayerDirection.x, 0 }, FullRange, yRange, false, false)){
			PlayerPos.x += PlayerDirection.x;
		}
		if (PlayerDirection.y != 0 && !Collition(&PlayerPos, { 0, PlayerDirection.y }, FullRange, yRange, false, false)) {
			PlayerPos.y += PlayerDirection.y;
		}

		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
		SDL_RenderClear(Renderer);
		

		DrawBG(Renderer, &PlayerPos);
		ShowInventor(Renderer, width, height, Inventory, InventorySlot);

		PlayerPos.x = SDL_clamp(PlayerPos.x, 9, 1600 - (int)(FullRange / 2));
		PlayerPos.y = SDL_clamp(PlayerPos.y, -4, 64 - yRange);
		
		DrawPlayer(Renderer);

		SDL_RenderPresent(Renderer);
		SDL_Delay(1000 / 10);
	}
	SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(Window);
	SDL_Quit();
	return 0;
}
int GameSetUp() 
{
	srand(time(0));

	int seed = rand();
	std::cout << "Seed: " << seed << std::endl;
	//srand(20464);
	srand(seed);

	SetGradients();

	return 0;
}
int main(int Argc, char* Argv[])
{
	if (GameSetUp() != 0) {
		std::cout << "Error setting up game." << std::endl;
		return 1;
	}
	else if (Ui() != 0) {
		std::cout << "Error setting up UI." << std::endl;
		return 1;
	}

	return 0;
}