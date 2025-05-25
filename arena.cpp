#include "arena.h"

Arena::Arena(int width, int height, int numBots, int numItems) 
	: width(width), height(height)
{
	initializeBots(numBots);
	initializeItems(numItems);

	int itemCount = static_cast<int>(items.size());
	std::cout << std::format("Total items in arena: {}", itemCount) << std::endl;

	int botCount = static_cast<int>(bots.size());
	std::cout << std::format("Total bots in arena: {}", botCount) << std::endl;
}

// Initialize bots in the arena
void Arena::initializeBots(const int numOfBots)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	std::set<std::pair<int, int>> botPositions;

	// Initialize uniform distributions
	std::uniform_int_distribution<> distribWidth(0, width - 1);
	std::uniform_int_distribution<> distribHeight(0, height - 1);
	std::uniform_int_distribution<> botArchtypeDistrib(0, static_cast<int>(BotArchetype::Count) - 1);

	while (botPositions.size() < numOfBots) {
		int x = distribWidth(gen);
		int y = distribHeight(gen);

		auto result = botPositions.insert({ x, y });

		if (result.second) {
			// Randomly select a bot archetype
			BotArchetype archetype = static_cast<BotArchetype>(botArchtypeDistrib(gen));

			std::string name = "Bot_" + std::to_string(botPositions.size() - 1);

			switch (archetype) {
				case BotArchetype::Warrior:
					name += "_Warrior";
					this->bots.insert({ { x, y }, new WarriorBot(name, x, y) });
					break;
				case BotArchetype::Mage:
					name += "_Mage";
					this->bots.insert({ { x, y }, new MageBot(name, x, y) });
					break;
				case BotArchetype::Tank:
					name += "_Tank";
					this->bots.insert({ { x, y }, new TankBot(name, x, y) });
					break;
				case BotArchetype::Archer:
					name += "_Archer";
					this->bots.insert({ { x, y }, new ArcherBot(name, x, y) });
					break;
				default:
					printColoredText("BOT INITIALIZATION FAILED", Color::Red);
					std::cout << "Invalid bot archetype!" << std::endl;
					return;
			}

			// Store in botList for easy access
			this->botList.push_back(this->bots[{ x, y }]); 

			// Set the index of the bot
			this->bots[{ x, y }]->setIndex(static_cast<int>(botPositions.size()) - 1);
		}
	}

	// Output bots to verify
	printColoredText("Bots Initialized:", Color::Yellow);
	for (const auto& botPair : bots) {
		std::cout << format("{} at position x: {}, y: {} with {} health, attack power {}, defense power {}", 
			botPair.second->getName(), 
			botPair.second->getX(), 
			botPair.second->getY(), 
			botPair.second->getHealth(), 
			botPair.second->getAttackPower(),
			botPair.second->getDefensePower()
		) << std::endl;
	}
}

// Initialize items in the arena
void Arena::initializeItems(const int numOfItems)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	std::set<std::pair<int, int>> itemPositions;

	// Initialize uniform distributions
	std::uniform_int_distribution<> distribWidth(0, width - 1);
	std::uniform_int_distribution<> distribHeight(0, height - 1);
	std::uniform_int_distribution<> distribItemType(0, static_cast<int>(ItemType::Count) - 1);

	while (itemPositions.size() < numOfItems) {
		int x = distribWidth(gen);
		int y = distribHeight(gen);
		ItemType type = static_cast<ItemType>(distribItemType(gen));

		// Try to insert the pair — only unique pairs are kept
		auto result = itemPositions.insert({ x, y });

		if (result.second) {
			// Add to internal map
			// Create a new item based on the type
			switch (type) {
				case ItemType::Health:
					this->items.insert({ { x, y }, new HealthItem(x, y) });
					break;
				case ItemType::Weapon:
					this->items.insert({ { x, y }, new WeaponItem(x, y) });
					break;
			}
		}
	}

	// Output positions to verify
	printColoredText("Items Initialized:", Color::Yellow);
	for (const auto& itemPair : this->items) {
		std::cout << std::format("Item: type {} at position x: {}, y: {}", 
			itemPair.second->getDescription(), itemPair.second->getX(), itemPair.second->getY()) << std::endl;
	}
}

std::pair<int, int> Arena::getNearestEnemy(int botIndex) const
{
	auto& bot = botList[botIndex];

	int closestDist = INT_MAX;
	int targetX = bot->getX();
	int targetY = bot->getY();

	for (const auto& otherBot : bots)
	{
		if (otherBot.second == bot)
			continue;  // skip self

		int dist = std::abs(otherBot.second->getX() - bot->getX()) + std::abs(otherBot.second->getY() - bot->getY());
		if (dist < closestDist)
		{
			closestDist = dist;
			targetX = otherBot.second->getX();
			targetY = otherBot.second->getY();
		}
	}

	int dx = 0, dy = 0;
	if (targetX > bot->getX())
		dx = 1;
	else if (targetX < bot->getX())
		dx = -1;

	if (targetY > bot->getY())
		dy = 1;
	else if (targetY < bot->getY())
		dy = -1;

	return { dx, dy };
}

std::pair<int, int> Arena::getNearestItem(int botIndex, ItemType type) const
{
	auto& bot = botList[botIndex];

	int closestDist = INT_MAX;
	int targetX = bot->getX();
	int targetY = bot->getY();

	for (const auto& item : items)
	{
		if (item.second->getType() == type)
		{
			int dist = std::abs(item.second->getX() - bot->getX()) + std::abs(item.second->getY() - bot->getY());
			if (dist < closestDist)
			{
				closestDist = dist;
				targetX = item.second->getX();
				targetY = item.second->getY();
			}
		}
	}

	// If an item was found, return its position if it's within a reasonable distance
	if (closestDist < INT_MAX && closestDist < width / 2 && closestDist < height / 2)
	{
		return { targetX, targetY };
	}

	// Return -1, -1 to indicate no item found
	return { -1, -1 };
}

// Function each thread will run
void Arena::runBot(int botIndex)
{
	// Collect the bot from the list - each thread will have its own bot index
	// Bots are NOT spawned, meaning this is a thread-safe operation
	auto& bot = botList[botIndex];

	// Random generator for bot movement
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<> sleepDistrib(100, 1000); // Random sleep time in milliseconds
	std::uniform_int_distribution<> actionDistrib(0, 1); // Random action (0: move, 1: battle)

	// Loop until the game is over
	while (!isGameOver()) 
	{
		checkItemCollection(botIndex);

		// Check if the bot is dead
		if (bot->isAlive == false)
			break;

		// Randomly decide to move or battle
		int action = actionDistrib(gen);
		if (action == 0)
		{
			moveBot(botIndex);
		}
		else
		{
			// Check for potential battles
			std::lock_guard<std::mutex> lock(arenaMutex);

			auto battlePositions = checkBattles(botIndex);
			if (!battlePositions.empty()) 
			{
				// Randomly select a target bot from the battle positions
				std::uniform_int_distribution<> targetDistrib(0, static_cast<int>(battlePositions.size()) - 1);

				int targetIndex = targetDistrib(gen);
				auto targetPos = battlePositions[targetIndex];
				auto targetBotIt = bots.find(targetPos);

				if (targetBotIt != bots.end()) {
					battle(botIndex, targetBotIt->second->getIdx());
				}
				else {
					printColoredText("BATTLE FAILED", Color::Red);
					std::cout << "Target bot not found!" << std::endl;
				}
			}
			else
			{
				printColoredText("NO BATTLE", Color::Yellow);
				std::cout << std::format("{} found no potential battles.", bot->getName()) << std::endl;
			}
		}

		// Random sleep to simulate time between moves
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepDistrib(gen)));
	}

	std::lock_guard<std::mutex> lock(arenaMutex);

	printColoredText("BOT LEFT", Color::Yellow);
	std::cout << std::format("{} left. Bot had {} health. Bot {}", 
		bot->getName(), 
		bot->getHealth(), 
		bot->getHealth() <= 0 ? "LOST" : "WON"
	) << std::endl;

	// Remove the bot from the arena
	bots.erase({ bot->getX(), bot->getY() }); // Remove from the map
	botList[botIndex] = nullptr; // Remove from the list
	delete bot; // Free memory

	displayArena();
}

// Display the current state of the arena
void Arena::displayArena()
{
	printColoredText("ARENA STATE:", Color::Cyan);

	// Print column indices
	std::cout << "     ";
	for (int x = 0; x < width; ++x) {
		std::cout << std::format("{}    ", x);
	}
	std::cout << std::endl;

	for (int x = 0; x < width; ++x) {
		std::cout << std::format("{}    ", x);

		for (int y = 0; y < height; ++y) {
			auto botIt = bots.find({ x, y });
			auto itemIt = items.find({ x, y });

			// Bots and items can occupy the same position
			if (botIt != bots.end() && itemIt != items.end()) {
				std::cout << std::format("B{}/{} ", botIt->second->getIdx(), itemIt->second->printType());; // Both bot and item
			}
			else if (botIt != bots.end()) {
				std::cout << std::format("B{}   ", botIt->second->getIdx()); // Bot
			}
			else if (itemIt != items.end()) {
				std::cout << std::format("{}    ", itemIt->second->printType()); // Item
			}
			else {
				std::cout << ".    "; // Empty space
			}	
		}
		std::cout << std::endl;
	}
}

// Check if the game is over
bool Arena::isGameOver()
{
	std::lock_guard<std::mutex> lock(arenaMutex);

	if (bots.size() == 1)
		return true;

	return false;
}

// Thread-safe moving of bots
void Arena::moveBot(int botIndex)
{
	std::lock_guard<std::mutex> lock(arenaMutex);

	auto& bot = botList[botIndex];

	// Get the move direction from the bot based on the strategy of its archetype
	std::pair<int, int> moveDirection = bot->decideMove(*this); 

	// New positions with boundary check - allows for wrapping around
	int newX = std::clamp(bot->getX() + moveDirection.first, 0, width - 1);
	int newY = std::clamp(bot->getY() + moveDirection.second, 0, height - 1);

	// Check if the new position is occupied by another bot
	auto newPos = std::make_pair(newX, newY);
	auto oldPos = std::make_pair(bot->getX(), bot->getY());

	if (newPos == oldPos) {
		printColoredText("MOVE FAILED", Color::Red);
		std::cout << std::format("{} cannot move to position x: {}, y: {} - already there",
			bot->getName(), 
			newX, 
			newY
		) << std::endl;
		return;
	}

	if (bots.find(newPos) != bots.end()) {
		printColoredText("MOVE FAILED", Color::Red);
		std::cout << std::format("{} cannot move to position x: {}, y: {} - occupied by another bot",
			bot->getName(), 
			newX, 
			newY
		) << std::endl;
		return;
	}

	// Change position in the map
	bot->setPosition(newX, newY);
	
	bots.erase(oldPos);
	bots.insert({ newPos, bot });

	printColoredText("MOVE", Color::Yellow);
	std::cout << std::format("{} moved to position x: {}, y: {}", bot->getName(), newX, newY) << std::endl;

	displayArena();
}

// Check if the bot is on a tile with an item and collect it
void Arena::checkItemCollection(int botIndex)
{
	std::lock_guard<std::mutex> lock(arenaMutex);

	auto& bot = botList[botIndex];

	auto botPos = std::make_pair(bot->getX(), bot->getY());
	auto itemIt = items.find(botPos);

	// Check if the bot is on a tile with an item
	if (itemIt != items.end()) {
		// Use the item
		bool result = itemIt->second->use(bot); 

		if (result)
		{
			printColoredText("ITEM COLLECTED", Color::Yellow);
			std::cout << std::format("{} collected a {} at position x: {}, y: {}",
				bot->getName(), 
				itemIt->second->getDescription(), 
				bot->getX(), 
				bot->getY()
			) << std::endl;

			// Remove the item from the arena
			delete itemIt->second; // Free memory
			items.erase(itemIt);

			int itemCount = static_cast<int>(items.size());
			std::cout << std::format("Total items in arena: {}", itemCount) << std::endl;

			displayArena();
		}
	}
}

// Spawn an item at a specific position
void Arena::spawnItem(int x, int y, ItemType type)
{
	std::lock_guard<std::mutex> lock(arenaMutex);

	auto itemIt = items.find({ x, y });

	// Check if the position is already occupied by another item - if not, spawn a new item
	if (itemIt == items.end()) {
		
		// Create a new item based on the type
		Item* newItem = nullptr;
		switch (type) {
			case ItemType::Health:
				newItem = new HealthItem(x, y);
				break;
			case ItemType::Weapon:
				newItem = new WeaponItem(x, y);
				break;
			default:
				printColoredText("ITEM SPAWN FAILED", Color::Red);
				std::cout << "Invalid item type!" << std::endl;
				return;
		}

		items.insert({ { x, y }, newItem });

		printColoredText("ITEM SPAWNED", Color::Blue);
		std::cout << std::format("Spawned a {} at position x: {}, y: {}",
			newItem->getDescription(), 
			newItem->getX(), 
			newItem->getY()
		) << std::endl;
	}
	else {
		printColoredText("ITEM SPAWN FAILED", Color::Red);
		std::cout << std::format("Item already exists at position x: {}, y: {}", x, y) << std::endl;
	}

	int itemCount = static_cast<int>(items.size());
	std::cout << std::format("Total items in arena: {}", itemCount) << std::endl;
}

// Returns all of the adjacent positions of a bot which are occupied by other bots - potential battle positions
std::vector<std::pair<int, int>> Arena::checkBattles(int botIndex)
{
	auto& bot = botList[botIndex];

	auto botPos = std::make_pair(bot->getX(), bot->getY());

	// Check all adjacent positions
	std::vector<std::pair<int, int>> battlePositions;
	std::vector<std::pair<int, int>> directions = {
		{ -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 }, { 1, 1 }, { -1, -1 }, { 1, -1 }, { -1, 1} 
	};  //  Up, Down, Left, Right, Diagonal directions
	

	// Check all four directions
	for (const auto& dir : directions) {
		int newX = bot->getX() + dir.first;
		int newY = bot->getY() + dir.second;

		// Check if the new position is within bounds
		if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
			auto pos = std::make_pair(newX, newY);

			// Check if the position is occupied by another bot
			if (bots.find(pos) != bots.end()) {
				battlePositions.push_back(pos);
			}
		}
	}

	// Output battle positions
	printColoredText("BATTLE CHECK", Color::Yellow);
	for (const auto& pos : battlePositions) {
		std::cout << std::format("Potential battle for {} at position x: {}, y: {}", 
			bot->getName(), 
			pos.first, 
			pos.second
		) << std::endl;
	}

	return battlePositions;
}

// Perform a battle between two bots
void Arena::battle(int botIndex, int targetBotIndex)
{
	auto& attacker = botList[botIndex];
	auto& target = botList[targetBotIndex];

	printColoredText("BATTLE", Color::Yellow);
	std::cout << std::format("{} is battling {} at position x: {}, y: {}",
		attacker->getName(), 
		target->getName(), 
		target->getX(), 
		target->getY()
	) << std::endl;

	// Simple battle logic: reduce health of the target bot
	int previousHealth = target->getHealth();
	target->takeDamage(attacker->getAttackPower()); // Reduce health

	printColoredText("BATTLE RESULT", Color::Yellow);
	std::cout << std::format("{} attacked {} for {} damage. {} defense: {}, health: {} -> {}",
		attacker->getName(), 
		target->getName(), 
		attacker->getAttackPower(), 
		target->getName(), 
		target->getDefensePower(), 
		previousHealth, 
		target->getHealth()
	) << std::endl;

	if (target->getHealth() == 0) {
		printColoredText("BOT DEFEATED", Color::Magenta);
		std::cout << std::format("{} has been defeated!", target->getName()) << std::endl;

		target->isAlive = false; // Mark as dead
	}
}

