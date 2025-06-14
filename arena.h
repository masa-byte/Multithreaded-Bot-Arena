#pragma once

#include <vector>
#include <unordered_map>
#include <random>
#include <set>
#include <iostream>
#include <format>
#include <chrono>

#include "bot.h"
#include "item.h"
#include "utils.h"
#include "timedMutex.h"

// Forward declaration of Bot class
class Bot;

class Arena {
private:
    int width;
    int height;

    std::unordered_map<std::pair<int, int>, Bot*, pair_hash> bots;
	std::vector <Bot*> botList; // For easy access to all bots

    std::unordered_map<std::pair<int, int>, Item*, pair_hash> items;

	TimedMutex arenaMutex;

	std::unordered_map<std::thread::id, std::chrono::duration<double>> threadExecutionTimeMap;

    void initializeBots(const int numOfBots);
    void initializeItems(const int numOfItems);

public:
    Arena(int width, int height, int numBots, int numItems);

	~Arena() {
		for (auto& botPair : bots) {
			delete botPair.second; // Free memory for each bot
		}
		for (auto& itemPair : items) {
			delete itemPair.second; // Free memory for each item
		}
	}

	std::unordered_map<std::thread::id, std::chrono::duration<double>> getThreadExecutionTimeMap() const {
		return threadExecutionTimeMap;
	}

	std::unordered_map<std::thread::id, std::chrono::duration<double>> getThreadWaitTimeMap() const {
		return arenaMutex.threadWaitMap;
	}

	// Utility functions
	std::pair<int, int> getNearestEnemy(int botIndex) const;
	std::pair<int, int> getWeakestEnemy(int botIndex) const;
	std::pair<int, int> getNearestItem(int botIndex, ItemType type) const;
    std::vector<std::pair<int, int>> checkBattles(int botIndex);
	int getNumOfBots() const { return static_cast<int>(bots.size()); }

	// Arena state
    void displayArena();            
    bool isGameOver();
	void spawnItem(int x, int y, ItemType type);

	// Bot function
    void runBot(int botIndex); // Function each thread will run
    void moveBot(int botIndex);
    void checkAndCollectItem(int botIndex);
	void battle(int botIndex, int targetBotIndex);
};
