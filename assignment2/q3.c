#include <stdio.h>

#define ROW 3
#define COL 3
#define BIT_POWER  (1 << 0) 
#define BIT_OVER   (1 << 1)  
#define BIT_MAINT  (1 << 2) 


static int in_bounds_1based(int r, int c) { return r >= 1 && r <= ROW && c >= 1 && c <= COL; }

static void display_sector(int rowZ, int colZ, int grid[ROW][COL]) {
  int value = grid[rowZ][colZ];
  printf("\n--- Sector: %d x %d ---\n", rowZ + 1, colZ + 1);
  printf("Current Status Flag: %d\n", value);
  printf("Power Status:         %s\n", (value & BIT_POWER) ? "ON" : "OFF");
  printf("Overload Warning:     %s\n", (value & BIT_OVER)  ? "Overloaded" : "Normal");
  printf("Maintenance Required: %s\n\n", (value & BIT_MAINT) ? "Yes" : "No");
}

static void update_sector_flag(int grid[ROW][COL]) {
  int r, c;
  printf("Enter row and column (1-%d) (1-%d): ", ROW, COL);
  if (scanf("%d %d", &r, &c) != 2 || !in_bounds_1based(r, c)) {
    printf("\nInvalid row/col! Try again.\n\n"); while (getchar()!='\n'); return;
  }
  while (getchar()!='\n');  
  int rz = r - 1, cz = c - 1;

  display_sector(rz, cz, grid);

  printf("Choose flag to modify:\n");
  printf("  1) Power (bit 0)\n");
  printf("  2) Overload (bit 1)\n");
  printf("  3) Maintenance Required (bit 2)\n");
  printf("Enter option: ");
  int which; if (scanf("%d", &which) != 1) { printf("Invalid.\n"); while (getchar()!='\n'); return; }

  int mask = 0;
  if      (which == 1) mask = BIT_POWER;
  else if (which == 2) mask = BIT_OVER;
  else if (which == 3) mask = BIT_MAINT;
  else { printf("Invalid flag choice.\n"); while (getchar()!='\n'); return; }

  printf("Set or Clear? (1 = set, 0 = clear): ");
  int action; if (scanf("%d", &action) != 1 || (action != 0 && action != 1)) {
    printf("Invalid action.\n"); while (getchar()!='\n'); return;
  }
  while (getchar()!='\n');

  if (action == 1) grid[rz][cz] |=  mask;  
  else             grid[rz][cz] &= ~mask;  

  printf("\nUpdated sector %d x %d. New status flag = %d\n\n", r, c, grid[rz][cz]);
}

static void update_sector_raw(int grid[ROW][COL]) {
  int r, c, status_flag;
  printf("Enter row and column (1-%d) (1-%d): ", ROW, COL);
  if (scanf("%d %d", &r, &c) != 2 || !in_bounds_1based(r, c)) {
    printf("\nInvalid row/col! Try again.\n\n"); while (getchar()!='\n'); return;
  }
  int rz = r - 1, cz = c - 1;

  display_sector(rz, cz, grid);

  printf("Enter new status flag (0-7): ");
  if (scanf("%d", &status_flag) != 1 || status_flag < 0 || status_flag > 7) {
    printf("Invalid Status Flag! Try again.\n"); while (getchar()!='\n'); return;
  }
  while (getchar()!='\n');

  grid[rz][cz] = status_flag;
  printf("\nSuccessfully updated status flag to %d for grid %d x %d\n\n", grid[rz][cz], r, c);
}

static void query_sector(int grid[ROW][COL]) {
  int r, c;
  printf("Enter row and column (1-%d) (1-%d): ", ROW, COL);
  if (scanf("%d %d", &r, &c) != 2 || !in_bounds_1based(r, c)) {
    printf("\nInvalid row/col! Try again.\n\n"); while (getchar()!='\n'); return;
  }
  while (getchar()!='\n');
  display_sector(r - 1, c - 1, grid);
}

static void system_diagnostic(int grid[ROW][COL]) {
  int over_cnt = 0, maint_cnt = 0;
  printf("\n--- System Diagnostic ---\n");
  for (int i = 0; i < ROW; i++) {
    for (int j = 0; j < COL; j++) {
      int v = grid[i][j];
      if (v & BIT_OVER) {
        over_cnt++;
        printf("Sector %d x %d value=%d : Overload\n", i + 1, j + 1, v);
      }
      if (v & BIT_MAINT) {
        maint_cnt++;
        printf("Sector %d x %d value=%d : Maintenance required\n", i + 1, j + 1, v);
      }
    }
  }
  printf("\nTotals:\n");
  printf("  Overloaded sectors:       %d\n", over_cnt);
  printf("  Maintenance required:     %d\n\n", maint_cnt);
}

int main(void) {
  int grid[ROW][COL] = {
    {3, 1, 2},
    {3, 4, 5},
    {6, 7, 7}
  };

  int choice;
  do {
    printf("\n--- IESCO Grid-Sector Management System ---\n");
    printf("[1] Update sector (set/clear a specific flag)\n");
    printf("[2] Update sector (set raw value 0-7)\n");
    printf("[3] Query sector status\n");
    printf("[4] Run system diagnostic\n");
    printf("[5] Exit\n");
    printf("Enter your choice (1-5): ");

    if (scanf("%d", &choice) != 1) { while (getchar()!='\n'); choice = -1; }

    switch (choice) {
      case 1: update_sector_flag(grid); break;
      case 2: update_sector_raw(grid);  break;
      case 3: query_sector(grid);       break;
      case 4: system_diagnostic(grid);  break;
      case 5: printf("\nThank you for using the app!\n"); break;
      default: printf("\nInvalid input! Please try again.\n\n"); break;
    }
  } while (choice != 5);

  return 0;
}
