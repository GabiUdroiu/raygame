#pragma once
class Entity {
protected:
    int health;
    int maxHealth;
    int damage;
    float damageCooldown = 0.0f;
    const float damageInterval = 0.5f;
    const float damageTintDuration = 0.2f; // how long to show red tint
public:
    Entity(int hp = 100, int dmg = 10) : health(hp), maxHealth(hp), damage(dmg) {}
    virtual ~Entity() = default;

    void takeDamage(int amount) {
        if (damageCooldown <= 0.0f) {
            health -= amount;
            damageCooldown = damageInterval;
        }
    }

    void UpdateDamageCooldown(float deltaTime) {
        if (damageCooldown > 0.0f)
            damageCooldown -= deltaTime;
    }

    bool canTakeDamage() const { return damageCooldown <= 0.0f; }

    int getHealth() const { return health; }

    int getMaxHealth() const { return maxHealth; }

    int getDamage() const { return damage; }

    bool isAlive() const { return health > 0; }
};
