#pragma once

#include <string>
#include <iostream>
#include <format>

// Forward declaration of Bot class
class Bot;

enum class ItemType {
    Health,
	Weapon,
    Count
    // Future types: Shield, SpeedBoost, etc.
};

class Item {
private:
    int x;
    int y;

public:
    Item(int x, int y) : x(x), y(y) {}
    virtual ~Item() = default;

    int getX() const { return x; }
    int getY() const { return y; }

	// Virtual methods for item behavior
    virtual std::string getDescription() const = 0;
	virtual std::string printType() const = 0;
	virtual ItemType getType() const = 0;
    virtual bool use(Bot* bot) = 0;
};

class HealthItem : public Item {
public:
    HealthItem(int x, int y) : Item(x, y) {}

    std::string getDescription() const override {
        return "Health Potion";
    }

    std::string printType() const override {
        return "H";
    }

	ItemType getType() const override {
		return ItemType::Health;
	}

	bool use(Bot* bot) override;
};

class WeaponItem : public Item {
public:
	WeaponItem(int x, int y) : Item(x, y) {}

	std::string getDescription() const override {
		return "Weapon Add-On";
	}

	std::string printType() const override {
		return "W";
	}

	ItemType getType() const override {
		return ItemType::Weapon;
	}

    bool use(Bot* bot) override;
};