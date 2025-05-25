#include "bot.h"
#include "arena.h"

Bot::Bot(const std::string& name, int x, int y, BotHealth health,
	BotAttackPower attackPower, BotDefensePower defensePower, BotSpeed speed)
{
	this->name = name;
	this->x = x;
	this->y = y;
	this->idx = -1; // Default index, will be set later

	// Health
	switch (health) {
		case BotHealth::Strong:
			this->health = 100;
			break;
		case BotHealth::Normal:
			this->health = 75;
			break;
		case BotHealth::Weak:
			this->health = 50;
			break;
		default:
			this->health = 0; // Invalid health
			break;
	}

	// Attack Power
	switch (attackPower) {
		case BotAttackPower::High:
			this->attackPower = 35;
			break;
		case BotAttackPower::Medium:
			this->attackPower = 25;
			break;
		case BotAttackPower::Low:
			this->attackPower = 15;
			break;
		default:
			this->attackPower = 0; // Invalid attack power
			break;
	}

	// Defense Power
	switch (defensePower) {
		case BotDefensePower::High:
			this->defensePower = 10;
			break;
		case BotDefensePower::Medium:
			this->defensePower = 5;
			break;
		case BotDefensePower::Low:
			this->defensePower = 2;
			break;
		default:
			this->defensePower = 0; // Invalid defense power
			break;
	}

	// Speed
	switch (speed) {
		case BotSpeed::Normal:
			this->speed = 1;
			break;
		case BotSpeed::Fast:
			this->speed = 2;
			break;
		default:
			this->speed = 0; // Invalid speed
			break;
	}
}

void Bot::setPosition(int newX, int newY) 
{
    x = newX;
    y = newY;
}

void Bot::setIndex(int index)
{
	idx = index;
}

void Bot::takeDamage(int amount) 
{
	health = health - amount + defensePower; // Reduce health by amount minus defense power
    if (health < 0) 
        health = 0;
}

bool Bot::heal(int amount) 
{
	if (health <= 0)
	    return false; // Cannot heal if already dead

    health += amount;
    if (health > 100) 
        health = 100;

	return true; // Successfully healed
}

bool Bot::increaseAttackPower(int amount)
{
	if (health <= 0)
		return false; // Cannot increase attack power if already dead

	attackPower += amount;
	if (attackPower > 100)
		attackPower = 100;

	return true; // Successfully increased attack power
}

std::pair<int, int> WarriorBot::decideMove(const Arena& arena)
{
	// Logic for Warrior: move towards the nearest enemy
	printColoredText("WARRIOR HUNTING", Color::White);
	return arena.getNearestEnemy(getIdx());
}

std::pair<int, int> MageBot::decideMove(const Arena& arena)
{
	// Logic for Mage:

	// Critical health - move towards health potion if available
	if (getHealth() < 15)
	{
		std::pair<int, int> healthPotionPos = arena.getNearestItem(getIdx(), ItemType::Health);
		if (healthPotionPos.first != -1 && healthPotionPos.second != -1) 
		{
			int dx = 0, dy = 0;
			if (healthPotionPos.first > getX())
				dx = 1;
			else if (healthPotionPos.first < getX())
				dx = -1;

			if (healthPotionPos.second > getY())
				dy = 1;
			else if (healthPotionPos.second < getY())
				dy = -1;

			printColoredText("MAGE MOVING TO HEALTH POTION", Color::White);
			return { dx, dy }; // Move towards health potion
		}
	}
	
	// Low health - stay in place to heal
	if (getHealth() < 30) {
		int previousHealth = getHealth();
		heal(10);

		printColoredText("HEAL", Color::Green);
		std::cout << std::format("MAGE HEALED - {} healed from {} to {} health using MAGIC", 
			getName(), 
			previousHealth, 
			getHealth()
		) << std::endl;

		return { 0, 0 };
	}

	// Otherwise, move towards the nearest enemy
	printColoredText("MAGE HUNTING", Color::White);
	return arena.getNearestEnemy(getIdx());
}

std::pair<int, int> TankBot::decideMove(const Arena& arena)
{
	// Logic for Tank:

	// Go for Weapon if health low
	if (getHealth() < 40)
	{
		std::pair<int, int> weaponPos = arena.getNearestItem(getIdx(), ItemType::Weapon);
		if (weaponPos.first != -1 && weaponPos.second != -1)
		{
			int dx = 0, dy = 0;
			if (weaponPos.first > getX())
				dx = 1;
			else if (weaponPos.first < getX())
				dx = -1;

			if (weaponPos.second > getY())
				dy = 1;
			else if (weaponPos.second < getY())
				dy = -1;

			printColoredText("TANK MOVING TO WEAPON", Color::White);
			return { dx, dy }; // Move towards weapon
		}
	}

	// Otherwise, move towards the nearest enemy
	printColoredText("TANK HUNTING", Color::White);
	return arena.getNearestEnemy(getIdx());
}

std::pair<int, int> ArcherBot::decideMove(const Arena& arena)
{
	// Logic for Archer:

	// If health is critical, move towards health potion with a step of 2 (because of fast speed)
	if (getHealth() < 10) {
		std::pair<int, int> healthPotionPos = arena.getNearestItem(getIdx(), ItemType::Health);
		if (healthPotionPos.first != -1 && healthPotionPos.second != -1)
		{
			int dx = 0, dy = 0;
			if (healthPotionPos.first > getX())
				dx = (abs(healthPotionPos.first - getX()) > 1) ? 2 : 1; // Step of 2 if far, else step of 1
			else if (healthPotionPos.first < getX())
				dx = (abs(healthPotionPos.first - getX()) > 1) ? -2 : -1; // Step of 2 if far, else step of 1

			if (healthPotionPos.second > getY())
				dy = (abs(healthPotionPos.second - getY()) > 1) ? 2 : 1; // Step of 2 if far, else step of 1
			else if (healthPotionPos.second < getY())
				dy = (abs(healthPotionPos.second - getY()) > 1) ? -2 : -1; // Step of 2 if far, else step of 1

			printColoredText("ARCHER MOVING TO HEALTH POTION", Color::White);
			return { dx, dy }; // Move towards health potion
		}
	}

	// If health is low, increase attack power
	if (getHealth() < 20) {
		int previousAttackPower = getAttackPower();
		increaseAttackPower(5);

		printColoredText("POWER UP", Color::Green);
		std::cout << std::format("ARCHER POWER UP - {} increased attack power from {} to {} using SKILLS", 
			getName(), 
			previousAttackPower, 
			getAttackPower()
		) << std::endl;

		return { 0, 0 }; // Stay in place to increase attack power
	}

	// Otherwise, move towards the nearest enemy
	printColoredText("ARCHER HUNTING", Color::White);
	return arena.getNearestEnemy(getIdx());
}
