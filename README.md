# Black-White-Array
This project implements a compact and efficient data structure called the Black-White Array (BWA) to manage and query product sales data. It supports insertion, deletion, searching, and top-k retrieval operations with guaranteed segment-wise sorting and efficient updates.

Think of it as a mini database of products with intelligent memory layout and operations like merge-based ranking and selective sorting, ideal for fixed-size scenarios.

📌 **Features**

**Segmented Memory Layout:** Divides array into log₂N fixed segments (ranks 0 to 3).

**Ranked Merging & Sorting:** Merge operations combine segments in increasing order using a bitmask representation.

**Interactive Console Menu:** For inserting, updating, deleting, querying sales data.

**Max Element Tracking:** Quickly identifies the top-selling product.

**Smart Updates:** Efficient in-place update & re-sort on modification of product sales.

**Handles Deletion and Rebuild:** Removes elements and rebalances BWA via rebuild operation.

🔧 **How It Works**

📦**Data Structure**

**White Array W[N]:** Holds the active sorted segments.

**Black Array B[N/2]:** Temporary buffer used during merges.

**Bitmask total:** Encodes which segments (by rank) are active.

**Segments:** Power-of-two-sized blocks indexed by rank (0 to K-1), where K=4 by default.

⚙️**Key Operations**

**Insert:** Adds a new (sales << 16 | product_id) key. If base rank (0) is occupied, merges are triggered.

**Search:** Binary search over active segments to locate the product.

**Update:** Modifies a key and ensures local segment order is preserved (with re-sort if necessary).

**Delete:** Marks a key as deleted (VOID = -1) and rebuilds the structure to maintain integrity.

**Peek Max:** Scans segments to retrieve the highest key (most sales).

