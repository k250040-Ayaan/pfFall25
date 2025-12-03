#include <stdio.h>

void calculateFuel(int fuel, int consumption, int recharge,
                   int solarBonus, int planet, int totalPlanets)
{
    if (fuel <= 0)
    {
        printf("Fuel exhausted at planet %d.\n", planet);
        return;
    }

    if (planet > totalPlanets)
    {
        printf("Journey completed successfully!\n");
        return;
    }

    fuel -= consumption;
    fuel += recharge;

    if (planet % 4 == 0)
    {
        fuel += solarBonus;
        printf("(Solar recharge applied!)\n");
    }

    printf("Planet %d: Fuel Remaining = %d\n", planet, fuel);

    calculateFuel(fuel, consumption, recharge, solarBonus,
                  planet + 1, totalPlanets);
}

int main()
{
    int fuel = 500;
    int consumption = 60;
    int recharge = 20;
    int solarBonus = 50;
    int totalPlanets = 10;

    printf("Spacecraft Fuel Tracking:\n");
    calculateFuel(fuel, consumption, recharge, solarBonus, 1, totalPlanets);

    return 0;
}
