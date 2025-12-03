#include <stdio.h>

double calculateRepayment(double loan, double interestRate, int years)
{
    if (loan <= 0 || years == 0)
    {
        printf("Loan fully paid or no years remaining.\n");
        return 0;
    }

    loan = loan + (loan * interestRate);

    double installment = 30000;

    double remaining = loan - installment;
    if (remaining < 0)
        remaining = 0;

    printf("Year %d: Remaining loan = %.2f\n", years, remaining);

    return installment + calculateRepayment(remaining, interestRate, years - 1);
}

int main()
{
    double loan = 100000;
    double rate = 0.05;
    int years = 3;

    printf("Loan Repayment Schedule:\n");
    double totalPaid = calculateRepayment(loan, rate, years);

    printf("\nTotal amount paid in %d years = %.2f\n", years, totalPaid);

    return 0;
}
