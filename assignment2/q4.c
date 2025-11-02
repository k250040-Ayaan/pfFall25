#include <stdio.h>

#define MAX_SIZE 100

int stack[MAX_SIZE] = {0};
int top = -1;

void push(void) {
    if (top >= MAX_SIZE - 1) {
        printf("\nError: Stack overflow. Max stack size reached.\n");
        return;
    }

    int element;
    printf("Enter a value to add into stack: ");
    if (scanf("%d", &element) != 1) {
        printf("Invalid input.\n");
        while (getchar() != '\n'); 
        return;
    }
    while (getchar() != '\n'); 

    stack[++top] = element;
    printf("PUSH: inserted %d at index %d\n", element, top);
}

void pop(void) {
    if (top == -1) {
        printf("Error: Stack underflow. No elements to pop.\n");
        return;
    }
    int popped = stack[top];
    printf("\nPOP: removed index %d => `%d`\n\n", top, popped);
    top--;
}

void peek(void) {
    if (top == -1) {
        printf("\nPEEK: stack is empty.\n\n");
        return;
    }
    printf("\nPEEK: top element is `%d` at index %d\n\n", stack[top], top);
}

void display(void) {
    printf("\n--- Current Stack (Top -> Bottom) ---\n\n");
    if (top == -1) {
        printf("Stack is empty...\n\n");
        return;
    }
    for (int i = top; i >= 0; i--) {
        printf("[%d] = %d\n", i, stack[i]);
    }
    printf("\n");
}

int main(void) {
    int choice;
    do {
        printf("\n--- Stack Management System ---\n\n");
        printf("[1] PUSH\n");
        printf("[2] POP\n");
        printf("[3] PEEK\n");
        printf("[4] DISPLAY\n");
        printf("[5] EXIT\n");
        printf("Enter your choice (1-5): ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            while (getchar() != '\n'); 
            continue;
        }
        while (getchar() != '\n'); 

        switch (choice) {
            case 1: push();    break;
            case 2: pop();     break;
            case 3: peek();    break;
            case 4: display(); break;
            case 5: printf("\nThank you for using this application!\n"); break;
            default: printf("\nInvalid choice! Please try again.\n");
        }
    } while (choice != 5);

    return 0;
}
