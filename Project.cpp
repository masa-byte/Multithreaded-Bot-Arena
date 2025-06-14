// Project.cpp : Defines the entry point for the application.

#include "Project.h"


int main()
{
	const int numberOfItems = { 5 };
	const int numberOfBots = { 7 };
	const int arenaWidth = { 12 };
	const int arenaHeight = { 12 };

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
	for (int i = 0; i < numberOfBots; i++) 
	{
		botThreads.emplace_back(&Arena::runBot, &arena, i);
	}

	// Sleep main thread
	std::this_thread::sleep_for(std::chrono::milliseconds(mainSleepMillis));

	while (true) 
	{
		int x = distribWidth(gen);
		int y = distribHeight(gen);
		ItemType type = static_cast<ItemType>(distribItemType(gen));
		arena.spawnItem(x, y, type);

		if (arena.getNumOfBots() <= 1) 
			break;
		
		// Sleep main thread
		std::this_thread::sleep_for(std::chrono::milliseconds(mainSleepMillis));
	}

	// Join all bot threads
	for (auto& thread : botThreads) 
	{
		if (thread.joinable()) 
		{
			thread.join();
		}
	}

	// Writing execution and waiting time for each thread to a file
	auto threadWaitTimeMap = arena.getThreadWaitTimeMap();
	auto threadExecutionTimeMap = arena.getThreadExecutionTimeMap();
	const std::string& filename = "threadTimes.txt";
	
	try
	{
		std::ofstream outFile(filename, std::ios::out | std::ios::app);
		if (!outFile.is_open()) 
		{
			std::cerr << "Failed to open " << filename << std::endl;
		}

		const int width = 20;
		outFile << "Arena Size: " << arenaWidth << "x" << arenaHeight << "\n";
		outFile << "Number of Bots: " << numberOfBots << "\n";

		outFile << std::left << std::setw(width) << "Thread ID"
			<< std::setw(width) << "Exec Time (ms)"
			<< std::setw(width) << "Wait Time (ms)"
			<< std::setw(width) << "Percent Wait" << "\n";

		for (const auto& [id, execTime] : threadExecutionTimeMap) {
			auto waitTime = threadWaitTimeMap.at(id);
			auto milisecondsExecTime = std::chrono::duration_cast<std::chrono::milliseconds>(execTime).count();
			auto milisecondsWaitTime = std::chrono::duration_cast<std::chrono::milliseconds>(waitTime).count();

			outFile << std::setw(width) << id
				<< std::setw(width) << std::fixed << std::setprecision(4) << milisecondsExecTime
				<< std::setw(width) << std::fixed << std::setprecision(4) << milisecondsWaitTime
				<< std::setw(width) << std::fixed << std::setprecision(2) << (milisecondsWaitTime * 100.0 / (milisecondsExecTime)) << "\n";
		}

		outFile.close();

		std::cout << "Thread execution and wait times written to " << filename << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error writing to file: " << e.what() << std::endl;
	}

	std::cout << "Finished running arena loop." << std::endl;

	return 0;
}
