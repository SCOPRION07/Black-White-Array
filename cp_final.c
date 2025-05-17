#include <stdio.h>
#include <stdbool.h>

#define K 4          // Maximum rank (segments 0,1,2,3)
#define N (1 << K)   // White array size (16)
#define N2 (N /2)   // Black array size (8)
#define VOID -1       // Special value for deleted entries

int W[N] = {0};    // White array (1-based indices)
int B[N2] = {0};   // Black array (1-based indices)
int V[K] = {0};       // Occupancy vector (tracks non-VOID values per rank)
int total = 0;       // Element count (bits = active segments)

// Segment index calculations
int S(int j) { return (1 << j); }         // Start index
int T(int j) { return (1 << (j+1)) - 1; } // End index

bool active(int i) { return (total & (1 << i)); }

// Determine the rank of an index
int seg(int index) {
    int j = 0;
    while ((1 << (j + 1)) <= index) j++;
    return j;
}

// All functions : 

void merge(int i);
void merge_segments(int s, int t, int dest_rank);
void insert(int v);

void print_bwa();

int Search(int v);
int binarySearch(int v, int s, int t) ;

void merge_demo(int rank);
void demote(int i);
int Delete(int v) ;


//  INSERTION CODE 
void insert(int v) {
    if (!active(0)) {
        W[1] = v;
    } else {
        B[1] = v;
        merge(0);
    }
    total += 1; 
}

void merge(int i) {
    int s = S(i), t = T(i);
    int dest_rank = i + 1;

    // if (dest_rank >= K) return; 
    
    if (!active(dest_rank)) {
        merge_segments(s, t, dest_rank);
    } else {
        merge_segments(s, t, dest_rank);
        merge(dest_rank);
    }
}

void merge_segments(int s, int t, int dest_rank) {
    int dest_s = S(dest_rank);
    int* dest = active(dest_rank) ? B : W;
    
    int i = s, j = s, k = dest_s;
    while (i <= t && j <= t) {
        if (B[i] && W[j]) {
            dest[k++] = (B[i] < W[j]) ? B[i++] : W[j++];
        } else {
            if (B[i]) dest[k++] = B[i++];
            if (W[j]) dest[k++] = W[j++];
        }
    }
    while (i <= t && B[i]) dest[k++] = B[i++];
    while (j <= t && W[j]) dest[k++] = W[j++];

    // Clear source segments
    for (int x = s; x <= t; x++) {
        B[x] = W[x] = 0;
    }
}

void print_bwa() {
    printf("Total: %d (Binary: ", total);
    for (int i = K-1; i >= 0; i--) 
        printf("%d", (total >> i) & 1);
    printf(")\n");

    for (int i = 0; i < K; i++) {
        if (active(i)) {
            int s = S(i), t = T(i);
            // Print WHITE segment for this rank
            printf("Rank %d W[%d-%d]: ", i, s, t);
            for (int j = s; j <= t; j++) printf("%d ", W[j]);
            printf("\n");

    if(i!=3){
            // Print BLACK segment for this rank (if not empty)
            printf("      B[%d-%d]: ", s, t);
            for (int j = s; j <= t; j++) printf("%d ", B[j]);
            printf("\n");
    }
        }
    }
}

// DELETION CODE 

// Merge two segments during demotion
void merge_demo(int rank) {
    int s = S(rank);
    int t = T(rank);
    int dest_s = S(rank - 1);
    int k = dest_s;

    // Move non-VOID values to lower rank
    for (int i = s; i <= t; i++) {
        if (W[i] != VOID) {
            W[k++] = W[i];
        }
    }
}

// Demote segment at rank i
void demote(int i) {
    int lower_rank = i - 1;

    if (active(lower_rank)) {
        merge_demo(i);  // Move values down
        // Merge black & white segments
        total &= ~(1 << i);  // Clear segment i
    } else {
        merge_demo(i);
    }

    V[lower_rank] += V[i];
    V[i] = 0;
}


int Delete(int v) {
    int idx = Search(v);

    if (idx == -1) {
        printf("Value %d not found!\n", v);
        return 0; // Deletion failed
    }

    int r = seg(idx); // Rank of the segment

    W[idx] = VOID;    // Mark as deleted
    V[r]--;           // Update occupancy vector

    // If segment becomes empty after deletion
    if (V[r] == 0) {
        total &= ~(1 << r);  // Mark segment inactive
        if (r != 0) {
            demote(r);       // Demote higher segment
        }
    }

    return idx; // Deletion successful
}


//  SEARCH CODE 

int binarySearch(int v, int s, int t) {
    int left = s, right = t;
    while (left <= right) {
        int mid = left + (right - left) / 2; 
        if (W[mid] == v) return mid;
        else if (W[mid] < v) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}


int Search(int v) {
    for (int i = K - 1; i >= 0; i--) {
        if (active(i)) {
            int s = S(i), t = T(i);
            int result = binarySearch(v, s, t);
            if (result != -1) {
                return result; 
            }
        }
    }
    return -1; 
}

int main() {

    // int values[] = {83,67,59,21,76,33,45,52,23,2,4,45,34,56,20};
    // int n = sizeof(values)/sizeof(values[0]);

    printf("___________BLACK WHITE ARRAY___________\n");

    int n;

    printf("Enter the size of the array (<=15): ");
    scanf("%d", &n);

    int values[n]; 

    printf("Enter %d elements:\n", n);
    for(int i = 0; i < n; i++) {
        scanf("%d", &values[i]);
    }

    // insertion

    for (int i = 0; i <n; i++) {
        printf("\nInserting %d\n", values[i]);
        insert(values[i]);
        print_bwa();
    }

    
    // search 

    printf("which value to be search : ");
    int search;
    scanf("%d",&search);

    int index = Search(search);
    if (index != -1) {
        printf("\nValue found at index: %d\n", index);
    } else {
        printf("\nValue not found\n");
    }
    

    // Delete 

    printf("which value to be deleted : ");
    int delete;
    scanf("%d",&delete);

    int deleted_index = Delete(delete);
    if (deleted_index != -1) {
        printf("\nDeleted value at index: %d\n", deleted_index);  
    }else{
        printf("\nValue not found\n");
    }
     
    printf("Final White Array: \n");
    for(int i=0;i<N;i++){
        printf(" %d - ", W[i]);
    }
    return 0;
}