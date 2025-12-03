#include <stdio.h>

struct Book
{
    int id;
    int popularity;
    int lastAccess;
};

int main()
{
    int capacity, Q;
    scanf("%d %d", &capacity, &Q);

    struct Book shelf[capacity];
    int count = 0;
    int timer = 1;

    for (int i = 0; i < capacity; i++)
    {
        shelf[i].id = -1;
    }

    while (Q--)
    {
        char op[10];
        scanf("%s", op);

        if (op[0] == 'A' && op[1] == 'D')
        {
            int x, y;
            scanf("%d %d", &x, &y);

            int found = -1;

            for (int i = 0; i < capacity; i++)
            {
                if (shelf[i].id == x)
                {
                    shelf[i].popularity = y;
                    shelf[i].lastAccess = timer++;
                    found = 1;
                    break;
                }
            }

            if (found == 1)
                continue;

            if (count < capacity)
            {
                for (int i = 0; i < capacity; i++)
                {
                    if (shelf[i].id == -1)
                    {
                        shelf[i].id = x;
                        shelf[i].popularity = y;
                        shelf[i].lastAccess = timer++;
                        count++;
                        break;
                    }
                }
            }
            else
            {
                int lraIndex = 0;
                for (int i = 1; i < capacity; i++)
                {
                    if (shelf[i].lastAccess < shelf[lraIndex].lastAccess)
                    {
                        lraIndex = i;
                    }
                }

                shelf[lraIndex].id = x;
                shelf[lraIndex].popularity = y;
                shelf[lraIndex].lastAccess = timer++;
            }
        }

        else if (op[0] == 'A' && op[1] == 'C')
        {
            int x;
            scanf("%d", &x);

            int found = 0;

            for (int i = 0; i < capacity; i++)
            {
                if (shelf[i].id == x)
                {
                    printf("%d\n", shelf[i].popularity);
                    shelf[i].lastAccess = timer++;
                    found = 1;
                    break;
                }
            }

            if (!found)
                printf("-1\n");
        }
    }

    return 0;
}
