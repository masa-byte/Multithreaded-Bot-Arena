#pragma once

#include <string>
#include <atomic>

// Forward declaration of Arena class
class Arena;

enum class BotHealth {
    Strong,
    Normal,
    Weak,
    Count
};

enum class BotAttackPower {
	High,
	Medium,
	Low,
	Count
};

enum class BotDefensePower {
	High,
	Medium,
	Low,
	Count
};

enum class BotSpeed {
	Normal,
	Fast,
	Fly,
	Count
};

enum class BotArchetype {
	Warrior,
	Mage,
	Tank,
	Archer,
	Count
};

class Bot {
private:
    std::string name;

    int idx;
    int x;
    int y;

    int health;
	int attackPower;
	int defensePower;
	int speed;

public:
	Bot(const std::string& name, int x, int y, BotHealth health,
		BotAttackPower attackPower, BotDefensePower defensePower, BotSpeed speed);

	virtual ~Bot() = default;

	std::atomic<bool> isAlive{ true };

	std::string getName() const { return name; }
	int getIdx() const { return idx; }
	int getHealth() const { return health; }
	int getAttackPower() const { return attackPower; }
	int getDefensePower() const { return defensePower; }
	int getSpeed() const { return speed; }
	int getX() const { return x; }
	int getY() const { return y; }

    void setPosition(int newX, int newY);
    void setIndex(int index);

    void takeDamage(int amount);
    bool heal(int amount);
    bool increaseAttackPower(int amount);
	std::pair<int, int> calculateMove(int targetX, int targetY, int botReduction) const;

	// Virtual methods for bot archetypes
	virtual std::string getArchetype() const = 0;
	virtual std::pair<int, int> decideMove(const Arena& arena) = 0;
};

// Warrior
class WarriorBot : public Bot {
public:
	WarriorBot(const std::string& name, int x, int y)
	: Bot(name, x, y, BotHealth::Normal, BotAttackPower::High, BotDefensePower::Medium, BotSpeed::Normal)
	{
	}

	std::string getArchetype() const override {
		return "Warrior";
	}

	std::pair<int, int> decideMove(const Arena& arena) override;
};

// Mage
class MageBot : public Bot {
public:
	MageBot(const std::string& name, int x, int y)
		: Bot(name, x, y, BotHealth::Weak, BotAttackPower::Medium, BotDefensePower::Low, BotSpeed::Fly)
	{
	}

	std::string getArchetype() const override {
		return "Mage";
	}

	std::pair<int, int> decideMove(const Arena& arena) override;
};

// Tank
class TankBot : public Bot {
public:
	TankBot(const std::string& name, int x, int y)
		: Bot(name, x, y, BotHealth::Strong, BotAttackPower::Low, BotDefensePower::High, BotSpeed::Normal)
	{
	}

	std::string getArchetype() const override {
		return "Tank";
	}

	std::pair<int, int> decideMove(const Arena& arena) override;
};

// Archer
class ArcherBot : public Bot {
public:
	ArcherBot(const std::string& name, int x, int y)
		: Bot(name, x, y, BotHealth::Normal, BotAttackPower::Medium, BotDefensePower::Medium, BotSpeed::Fast)
	{
	}

	std::string getArchetype() const override {
		return "Archer";
	}

	std::pair<int, int> decideMove(const Arena& arena) override;
};
