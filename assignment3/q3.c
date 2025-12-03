#include <stdio.h>
#include <string.h>

struct Employee
{
    int id;
    char name[50];
    char designation[50];
    float salary;
};

void displayEmployees(struct Employee emp[], int n)
{
    printf("\n------------------------------------------------------------\n");
    printf("%-10s %-20s %-15s %-10s\n", "ID", "Name", "Designation", "Salary");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < n; i++)
    {
        printf("%-10d %-20s %-15s %-10.2f\n",
               emp[i].id,
               emp[i].name,
               emp[i].designation,
               emp[i].salary);
    }
}

void findHighestSalary(struct Employee emp[], int n)
{
    int maxIndex = 0;

    for (int i = 1; i < n; i++)
    {
        if (emp[i].salary > emp[maxIndex].salary)
        {
            maxIndex = i;
        }
    }

    printf("\nEmployee With Highest Salary:\n");
    printf("ID: %d\n", emp[maxIndex].id);
    printf("Name: %s\n", emp[maxIndex].name);
    printf("Designation: %s\n", emp[maxIndex].designation);
    printf("Salary: %.2f\n", emp[maxIndex].salary);
}

void searchEmployee(struct Employee emp[], int n)
{
    int choice;
    printf("\nSearch by:\n1. ID\n2. Name\nEnter choice: ");
    scanf("%d", &choice);

    if (choice == 1)
    {
        int id;
        printf("Enter Employee ID: ");
        scanf("%d", &id);

        for (int i = 0; i < n; i++)
        {
            if (emp[i].id == id)
            {
                printf("\nEmployee Found:\n");
                printf("ID: %d\nName: %s\nDesignation: %s\nSalary: %.2f\n",
                       emp[i].id, emp[i].name, emp[i].designation, emp[i].salary);
                return;
            }
        }
        printf("Employee with ID %d not found.\n", id);
    }

    else if (choice == 2)
    {
        char name[50];
        printf("Enter Employee Name: ");
        scanf("%s", name);

        for (int i = 0; i < n; i++)
        {
            if (strcmp(emp[i].name, name) == 0)
            {
                printf("\nEmployee Found:\n");
                printf("ID: %d\nName: %s\nDesignation: %s\nSalary: %.2f\n",
                       emp[i].id, emp[i].name, emp[i].designation, emp[i].salary);
                return;
            }
        }
        printf("Employee with Name '%s' not found.\n", name);
    }

    else
    {
        printf("Invalid choice.\n");
    }
}

void updateSalaryBonus(struct Employee emp[], int n, float threshold)
{
    for (int i = 0; i < n; i++)
    {
        if (emp[i].salary < threshold)
        {
            emp[i].salary += emp[i].salary * 0.10;
        }
    }
}

int main()
{
    int n;

    printf("Enter number of employees: ");
    scanf("%d", &n);

    struct Employee emp[n];

    for (int i = 0; i < n; i++)
    {
        printf("\nEnter details for Employee %d:\n", i + 1);
        printf("ID: ");
        scanf("%d", &emp[i].id);

        printf("Name: ");
        scanf("%s", emp[i].name);

        printf("Designation: ");
        scanf("%s", emp[i].designation);

        printf("Salary: ");
        scanf("%f", &emp[i].salary);
    }

    printf("\nAll Employee Records:\n");
    displayEmployees(emp, n);

    findHighestSalary(emp, n);

    searchEmployee(emp, n);

    updateSalaryBonus(emp, n, 50000);

    printf("\nAfter Applying 10%% Bonus to Salaries Below 50,000:\n");
    displayEmployees(emp, n);

    return 0;
}
