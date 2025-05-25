#include "item.h"
#include "utils.h"
#include "bot.h"

bool HealthItem::use(Bot* bot)
{
    int previousHealth = bot->getHealth();
    bool healed = bot->heal(30); // Heal the bot

    if (healed) {
        printColoredText("HEAL", Color::Green);
        std::cout << std::format("{} healed from {} to {} health", 
            bot->getName(), 
            previousHealth, 
            bot->getHealth()
        ) << std::endl;
        return true;
    }
    else {
        printColoredText("HEAL FAILED", Color::Red);
        std::cout << std::format("{}: health {}", 
            bot->getName(), 
            bot->getHealth()
        ) << std::endl;
        return false;
    }
}

bool WeaponItem::use(Bot* bot)
{
    int previousAttackPower = bot->getAttackPower();
    bool power = bot->increaseAttackPower(10); // Increase attack power by 10

    if (power) {
        printColoredText("POWER UP", Color::Green);
        std::cout << std::format("{} increased attack power from {} to {}", 
            bot->getName(), 
            previousAttackPower, 
            bot->getAttackPower()
        ) << std::endl;
        return true;
    }
    else {
        printColoredText("POWER UP FAILED", Color::Red);
        std::cout << std::format("{}: attack power {}", 
            bot->getName(), bot->getAttackPower()
        ) << std::endl;
        return false;
    }
}
