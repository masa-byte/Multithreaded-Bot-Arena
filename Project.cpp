// Project.cpp : Defines the entry point for the application.

#include "Project.h"


int main()
{
	const int numberOfItems = { 5 };
	const int numberOfBots = { 8 };
	const int arenaWidth = { 10 };
	const int arenaHeight = { 10 };

	const int mainSleepMillis = arenaWidth * arenaHeight * 20;

	std::random_device rd;
	std::mt19937 gen(rd());

	// Initialize uniform distributions for random number generation
	std::uniform_int_distribution<> distribWidth(0, arenaWidth - 1);
	std::uniform_int_distribution<> distribHeight(0, arenaHeight - 1);
	std::uniform_int_distribution<> distribItemType(0, static_cast<int>(ItemType::Count) - 1);

	Arena arena(arenaWidth, arenaHeight, numberOfBots, numberOfItems);
	arena.displayArena();

	// Main thread is responsible for starting arena loop and threads
	std::vector<std::thread> botThreads;
	for (int i = 0; i < numberOfBots; ++i) {
		botThreads.emplace_back(&Arena::runBot, &arena, i);
	}

	// Sleep main thread
	std::this_thread::sleep_for(std::chrono::milliseconds(mainSleepMillis));

	while (true) {
		int x = distribWidth(gen);
		int y = distribHeight(gen);
		ItemType type = static_cast<ItemType>(distribItemType(gen));
		arena.spawnItem(x, y, type);

		arena.displayArena();

		if (arena.getNumOfBots() <= 1) 
			break;
		
		// Sleep main thread
		std::this_thread::sleep_for(std::chrono::milliseconds(mainSleepMillis));
	}

	// Join all bot threads
	for (auto& thread : botThreads) {
		if (thread.joinable()) {
			thread.join();
		}
	}

	std::cout << "Finished running arena loop." << std::endl;

	return 0;
}
