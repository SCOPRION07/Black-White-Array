#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

//========== UNCHANGED BWA IMPLEMENTATION ==========
#define K 4   // Maximum rank (segments 0,1,2,3)
#define N (1 << K)   // White array size (16)
#define MAX_ELEMENTS (N - 1)
#define VOID -1      // Special value for deleted entries

static int W[N] = {0}, B[N / 2] = {0}, total = 0;

// Segment index calculations
static int S(int j) { return 1 << j; }
static int T(int j) { return (1 << (j + 1)) - 1; }
static bool active(int i) { return (total & (1 << i)) != 0; }

//  INSERTION CODE 
static void merge_segments(int s, int t, int dest_rank)
{
    int dest_s = S(dest_rank);
    int *dest = active(dest_rank) ? B : W;
    int i = s, j = s, k = dest_s;
    while (i <= t && j <= t)
    {
        if (B[i] && W[j])
            dest[k++] = (B[i] < W[j]) ? B[i++] : W[j++];
        else
        {
            if (B[i])
                dest[k++] = B[i++];
            if (W[j])
                dest[k++] = W[j++];
        }
    }
    while (i <= t && B[i])
        dest[k++] = B[i++];
    while (j <= t && W[j])
        dest[k++] = W[j++];
    for (int x = s; x <= t; x++)
        B[x] = W[x] = 0;
}

static void merge(int rank)
{
    int s = S(rank), t = T(rank), nxt = rank + 1;
    merge_segments(s, t, nxt);
    if (active(nxt))
        merge(nxt);
}

void insert(int v)
{
    if (total >= MAX_ELEMENTS)
    {
        fprintf(stderr, "ERROR: BWA full (max %d)\n", MAX_ELEMENTS);
        return;
    }
    if (!active(0))
        W[1] = v;
    else
    {
        B[1] = v;
        merge(0);
    }
    total++;
}

// SEARCH CODE 
int Search(int v)
{
    for (int r = K - 1; r >= 0; r--)
    {
        if (active(r))
        {
            int s = S(r), t = T(r);
            int l = s, h = t;
            while (l <= h)
            {
                int m = (l + h) >> 1;
                if (W[m] == v)
                    return m;
                if (W[m] < v)
                    l = m + 1;
                else
                    h = m - 1;
            }
        }
    }
    return -1;
}

// DELETION CODE 
void rebuild()
{
    int temp[MAX_ELEMENTS], cnt = 0;
    for (int i = 1; i < N; i++)
    {
        if (W[i] != VOID && W[i] != 0)
            temp[cnt++] = W[i];
        W[i] = B[i] = 0;
    }
    total = 0;
    for (int i = cnt - 1; i >= 0; i--)
        insert(temp[i]);
}

void Delete(int v)
{
    int idx = Search(v);
    if (idx == -1)
    {
        fprintf(stderr, "Delete: %d not found\n", v);
        return;
    }
    W[idx] = VOID;
    rebuild();
}

// Find max element of BWA array 
int bwa_peek_max()
{
    int best = INT_MIN;
    for (int r = 0; r < K; r++)
    {
        if (active(r))
        {
            int idx = T(r);

            while (idx >= S(r) && W[idx] == VOID)
            {
                idx--;
            }
            if (idx >= S(r) && W[idx] > best)
            {
                best = W[idx];
            }
        }
    }
    return best;
}

// Initialize the white array
void clear_bwa()
{
    for (int i = 1; i < N; i++)
        W[i] = B[i] = 0;
    total = 0;
}

// Print the BWA Array
void print_bwa()
{
    printf("--- BWA State (total=%d) ---\n", total);
    printf("Bitmask: ");
    for (int i = K - 1; i >= 0; i--)
        printf("%c", (total >> i) & 1 ? '1' : '0');
    printf("\n");
    for (int r = 0; r < K; r++)
    {
        if (active(r))
        {
            printf(" Rank %d [", r);
            for (int i = S(r); i <= T(r); i++)
            {
                if (W[i] != VOID && W[i] != 0)
                    printf(" %d", W[i]);
            }
            printf(" ]\n");
        }
    }
    printf("---------------------------\n");
}

// Find rank from index
static int seg(int idx)
{
    int j = 0;
    while ((1 << (j + 1)) <= idx)
    {
        j++;
    }
    return j;
}

//    We'll use qsort here for simplicity.
static int _cmp_int(const void *a, const void *b)
{
    int ia = *(const int *)a, ib = *(const int *)b;
    return (ia < ib) ? -1 : (ia > ib) ? 1
                                      : 0;
}

// Sort the ranked segments in ascending order
static void sort_rank(int j)
{
    int s = S(j), t = T(j);

    int write = s;
    for (int i = s; i <= t; i++)
    {
        if (W[i] != VOID && W[i] != 0)
        {
            W[write++] = W[i];
        }
    }
    int m = write - s;

    if (m > 1)
    {
        qsort(&W[s], m, sizeof(int), _cmp_int);
    }

    for (int i = s + m; i <= t; i++)
    {
        W[i] = 0;
    }
}

void bwa_update(int old_key, int new_key)
{
    int idx = Search(old_key);
    if (idx < 0)
    {
        printf("Update: old key %d not found in BWA\n", old_key);
        return;
    }
    int j = seg(idx);

    W[idx] = new_key;

    if (j == 0)
        return;

    int s = S(j), t = T(j);
    bool need_sort = false;

    if (idx == s)
    {
        if (W[idx + 1] < W[idx])
            need_sort = true;
    }
    else if (idx == t)
    {
        if (W[idx - 1] > W[idx])
            need_sort = true;
    }
    else
    {
        if (W[idx - 1] > W[idx] || W[idx + 1] < W[idx])
        {
            need_sort = true;
        }
    }

    if (need_sort)
    {

        sort_rank(j);
    }
}

//========== INTERACTIVE MENU ==========

void bwa_menu(int P)
{
    // sales array for O(1) lookup
    int *sales = calloc(P, sizeof(int));
    if (!sales)
    {
        perror("calloc");
        return;
    }

    // initialize empty BWA
    clear_bwa();

    while (1)
    {
        printf("\nMenu:\n");
        printf("1. Insert/Update product sale\n");
        printf("2. Get top seller\n");
        printf("3. Search product sales\n");
        printf("4. Delete product\n");
        printf("5. Display BWA state\n");
        printf("6. Exit\n");
        printf("Enter choice: ");
        int choice;
        if (scanf("%d", &choice) != 1)
            break;

        switch (choice)
        {
        case 1:
        {
            int id, qty;
            printf("Enter product ID (0..%d) and units sold: ", P - 1);
            scanf("%d %d", &id, &qty);
            if (id < 0 || id >= P || qty < 0)
            {
                printf("Invalid input.\n");
                break;
            }

            int old = sales[id];
            int old_key = (old << 16) | id;
            int new_sales = old + qty;
            int new_key = (new_sales << 16) | id;

            if (old > 0)
            {
                bwa_update(old_key, new_key);
            }
            else
            {

                insert(new_key);
            }

            sales[id] = new_sales;
            printf("Product %d updated to %d sales.\n", id, new_sales);
            break;
        }
        case 2:
        {
            if (total == 0)
            {
                printf("No products in BWA.\n");
                break;
            }
            int maxkey = bwa_peek_max();
            int top_id = maxkey & 0xFFFF;
            int top_sales = maxkey >> 16;
            printf("Top seller: ID=%d with %d sales.\n", top_id, top_sales);
            break;
        }
        case 3:
        {
            int id;
            printf("Enter product ID to search (0..%d): ", P - 1);
            scanf("%d", &id);
            if (id < 0 || id >= P)
            {
                printf("Invalid ID.\n");
                break;
            }
            // get current sales count
            int cur = sales[id];
            if (cur == 0)
            {
                printf("Product %d has 0 sales.\n", id);
                break;
            }
            // form the BWA key and search it
            int key = (cur << 16) | id;
            int idx = Search(key);
            if (idx != -1)
            {
                printf("Product %d has %d sales. (found in BWA at index %d)\n",
                       id, cur, idx);
            }
            else
            {
                printf("Product %d has %d sales, but was NOT found in the BWA!\n",
                       id, cur);
            }
            break;
        }

        case 4:
        {
            int id;
            printf("Enter product ID to delete (0..%d): ", P - 1);
            scanf("%d", &id);
            if (id < 0 || id >= P)
            {
                printf("Invalid ID.\n");
                break;
            }
            int cur = sales[id];
            if (cur == 0)
            {
                printf("No sales to delete for product %d.\n", id);
                break;
            }
            int del_key = (cur << 16) | id;
            Delete(del_key);
            sales[id] = 0;
            printf("Product %d deleted from BWA.\n", id);
            break;
        }
        case 5:
            print_bwa();
            break;
        case 6:
            printf("Exiting.\n");
            free(sales);
            return;
        default:
            printf("Invalid choice.\n");
        }
    }
}

int main()
{
    int P;
    printf("Products and Sales Manager (using BWA)\n");
    printf("====================================\n");
    printf("Enter maximum number of products (P): ");
    if (scanf("%d", &P) != 1 || P <= 0 || P > MAX_ELEMENTS)
    {
        fprintf(stderr, "Invalid product count (must be 1..%d)\n", MAX_ELEMENTS);
        return 1;
    }
    bwa_menu(P);
    return 0;
}