#include <stdio.h>
#include <string.h>

#define MAX_PRODUCTS 5
#define PROMO_CODE "Eid2025"
#define DISCOUNT_RATE 0.25f

int   product_code[MAX_PRODUCTS]   = {1, 2, 3, 4, 5};
float product_prices[MAX_PRODUCTS] = {220.0f, 180.0f, 350.0f, 300.0f, 750.0f};
int   product_stock[MAX_PRODUCTS]  = {100,   150,   200,   80,    50};

int cart[MAX_PRODUCTS] = {0};

char customer_name[100] = "N/A";
char customer_cnic[20]  = "N/A";

float total_bill = 0.0f;
float discounted_bill = 0.0f;
int   discount_applied = 0;

static void flush_line(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void get_customer_info(void) {
  printf("\n--- Customer Information ---\n");

  printf("Enter Customer Name: ");
  scanf(" %99[^\n]", customer_name);

  printf("Enter Customer CNIC (e.g., 12345-6789012-3): ");
  scanf(" %19s", customer_cnic);

  printf("\nCustomer information updated successfully!\n");
}

void display_inventory(void) {
  printf("\n--- Current Inventory ---\n");
  printf("ID\tPrice\t\tStock\n\n");

  for (int i = 0; i < MAX_PRODUCTS; i++) {
    printf("%d\tRs. %-7.2f\t%d\n", product_code[i], product_prices[i],
           product_stock[i]);
  }
  printf("\n");
}

void add_item_to_cart(void) {
  int product_id, quantity;
  int choice;

  do {
    display_inventory();

    printf("\nEnter Product ID to add to cart (1-%d): ", MAX_PRODUCTS);
    if (scanf("%d", &product_id) != 1) {
      printf("Invalid input. Please enter a number.\n");
      flush_line();
      continue;
    }

    if (product_id < 1 || product_id > MAX_PRODUCTS) {
      printf("Error: Invalid Product ID. Please try again.\n");
      flush_line();
      continue;
    }

    int index = product_id - 1;

    printf("Enter quantity for PRODUCT %d: ", product_code[index]);
    if (scanf("%d", &quantity) != 1) {
      printf("Invalid input. Please enter a number.\n");
      flush_line();
      continue;
    }

    if (quantity <= 0) {
      printf("Error: Quantity must be greater than 0.\n");
    } else if (quantity > product_stock[index]) {
      printf("Error: Not enough stock! Only %d available.\n",
             product_stock[index]);
    } else {
      cart[index] += quantity;
      product_stock[index] -= quantity;
      printf("\n%d piece(s) of PRODUCT %d added to cart. Remaining stock: %d\n",
             quantity, product_code[index], product_stock[index]);
    }

    printf("\nDo you want to add another item? (1=yes / 0=no): ");
    if (scanf("%d", &choice) != 1) {
      choice = 0;
    }
    flush_line();

  } while (choice == 1);
}

void display_total_bill(void) {
  total_bill = 0.0f;
  discounted_bill = 0.0f;
  discount_applied = 0;

  printf("\n--- Checkout & Bill Calculation ---\n");

  int items_in_cart = 0;
  for (int i = 0; i < MAX_PRODUCTS; i++) {
    if (cart[i] > 0) {
      total_bill += (cart[i] * product_prices[i]);
      items_in_cart++;
    }
  }

  if (items_in_cart == 0) {
    printf("Your cart is empty. Please add items first.\n");
    return;
  }

  printf("Your total bill (before discount) is: Rs. %.2f\n", total_bill);

  printf("\nDo you have a voucher code? (1=yes / 0=no): ");
  int choice = 0;
  if (scanf("%d", &choice) != 1) {
    choice = 0;
  }
  flush_line();

  if (choice == 1) {
    char promo_input[50];
    printf("Enter promo code: ");
    scanf("%49s", promo_input);

    if (strcmp(promo_input, PROMO_CODE) == 0) {
      discounted_bill = total_bill * (1.0f - DISCOUNT_RATE); /* 25% off */
      discount_applied = 1;
      printf("Success! '%s' applied. You get a 25%% discount.\n", PROMO_CODE);
    } else {
      printf("Invalid promo code. No discount applied.\n");
      discounted_bill = total_bill;
      discount_applied = 0;
    }
  } else {
    printf("No promo code entered.\n");
    discounted_bill = total_bill;
    discount_applied = 0;
  }

  printf("\nYour final amount to pay is: Rs. %.2f\n", discounted_bill);
}

void show_invoice(void) {
  printf("\n--- FINAL INVOICE ---\n");

  printf("Customer Name: %s\n", customer_name);
  printf("Customer CNIC: %s\n", customer_cnic);

  if (total_bill <= 0.0f) {
    printf("\nNo items have been processed for checkout.\n");
    printf("Please 'Add Item to Cart' (Option 3) and 'Proceed to Checkout' "
           "(Option 4) first.\n");
    return;
  }

  printf("\nItems Purchased:\n\n");
  printf("%-15s\tQty\tUnit Price\tTotal\n\n", "Product Code");

  for (int i = 0; i < MAX_PRODUCTS; i++) {
    if (cart[i] > 0) {
      printf("%-15d\t%d\tRs. %-7.2f\tRs. %.2f\n",
             product_code[i], cart[i], product_prices[i],
             (cart[i] * product_prices[i]));
    }
  }
  printf("\n");

  printf("Original Total:\t\t\tRs. %.2f\n", total_bill);

  if (discount_applied) {
    float discount_amount = total_bill - discounted_bill;
    printf("Discount (%s -> 25%%):\t-Rs. %.2f\n", PROMO_CODE, discount_amount);
  } else {
    printf("Discount:\t\t\t-Rs. 0.00\n");
  }

  printf("---------------------------------------------------\n");
  printf("Final Bill (Paid):\t\tRs. %.2f\n", discounted_bill);
  printf("\nThank you for shopping with us!\n");
}

int main(void) {
  int choice;

  do {
    printf("\n--- Supermarket Management System ---\n");
    printf("[1] Enter Customer Information\n");
    printf("[2] Display Inventory\n");
    printf("[3] Add Item to Cart\n");
    printf("[4] Proceed to Checkout\n");
    printf("[5] Show Final Invoice\n");
    printf("[6] Exit\n");
    printf("Enter your choice (1-6): ");

    if (scanf("%d", &choice) != 1) {
      printf("\nError: Invalid choice. Please select from 1-6.\n");
      flush_line();
      continue;
    }
    flush_line();

    switch (choice) {
      case 1: get_customer_info();  
      break;
      case 2: display_inventory();  
      break;
      case 3: add_item_to_cart();   
      break;
      case 4: display_total_bill(); 
      break;
      case 5: show_invoice();       
      break;
      case 6: printf("\nExiting system.\n");
      break;
      default: printf("\nError: Invalid choice. Please select from 1-6.\n");
    }

  } while (choice != 6);

  return 0;
}
