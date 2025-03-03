<h2> Project structure </h2>

<h4> Requirements </h4>

**Unix-based OS**: I tested on macOS Sequoia and Alma Linux

**CMake** 3.15+

**Compiler capable of supporting C++17**

I have played with CMake bit for this task and structured the prject accordingly.
In short, it has following targets:

1. **file_generator**: generates 1GB file filled with doubles
2. **file_sort**: sorts data in a generated file
3. **database_cache**: executable to play with database cache
4. **database_cache_lib**: target that exists solely for unit testing
5. **test_lru_cache**: unit tests for database interface and cache

<h2> How to build and how to run </h2>

1. Clone a repo
2. Go to repo dir: ```cd atto-test-task```
3. Create and enter a build dir: ```mkdir build && cd build```
4. Generate build files: ```cmake ..```
5. Enter build directory: cd build
6. Build a target: ```cmake --build . --target <target name>``` (one of the listed targets above)
7. Under this conditions a built binary lands into ```build/src/file_sort/``` (for file_sort and file_generator targers) and in ```build/src/database_cache``` (for database_cache and database_cache_lib targers)
8. Unit tests ```test_lru_cache``` binary land into ```build/tests/database_cache directory```

Running a program is just launching a binary.

**For file_generator target run (assuming you are in build directory):** ```file_generator <full to the intended path of generation>```. 
**Example:** ```file_generator "/Users/user/test_file.txt"```

**For file_sort target run:** ```file_sort <path to an unsorted file> <path to the sorted file>```. 
**Example:** ```file_sort "/Users/user/test_file.txt" "/Users/user/sorted_file.txt"```

**For test_lru_cache just run the binary itself. Example:** ```./test_lru_cache```

<h2> How it works </h2>

<h4>File generator</h4> 

For file generation, I decided to generate the mantissa and exponent separately and then combine them into a double using scientific notation.  

Calculating the output data size was done in a somewhat brute-force manner. Since the resulting file consists of numbers, I generate a number, convert it to a string, and measure the string's size, including the newline character. The sum of all generated string representations should add up to **1 GB (1e9 bytes).**  

For sorting the file, I chose an **external sorting approach with K-way merge.** Since the file size is **1 GB**, it cannot be fully loaded into memory, so the data must be processed in chunks.  

There were no explicit constraints regarding **hard drive utilization** (I assumed the "memory" requirement did not imply that I was prohibited from using more than 100 MB of disk space).  

To handle large data efficiently, I followed this approach:

1. Read doubles from the file into an **80 MB buffer**.
2. Sort the numbers once the buffer is full.
3. Store the sorted numbers in temporary text files in the **`/tmp`** directory (tested on Unix-based systems, Windows support was not a priority).

Once all input data is processed, the merging phase begins:

1. Read the first number from each temporary file.
2. Insert these numbers into a **min-heap**.
3. Extract the **minimum** from the heap and retrieve the corresponding input stream index.
4. Write this minimum value into the output file.
5. Read the next number from the same input stream.
6. Repeat steps **3–5** until all temporary files are fully processed.
7. Delete all temporary files.

The current implementation imposes a significant **CPU overhead** but adheres to memory constraints. I haven't had time to optimize CPU performance further.  

On tested systems, **memory usage never exceeded 80 MB**. However, the test machine must have **sufficient disk space** for temporary files.

## Database Cache

I focused on three key requirements:

1. **Transaction-based implementation**
2. **Multithreading support**
3. **Caching**

I wasn't entirely sure how to approach this task, so I implemented the provided interface, integrating **transactions, caching, and multithreading support.**  

One question I had was how to define a **"transaction"**: should it consist of a single modification operation, or could it include multiple operations? Since different software may interpret transactions differently, I chose the latter:  
**A transaction consists of multiple `set` and `remove` operations until `commit_transaction` is called.**  

**`get()` queries can be made at any time**, but I was unsure what data to return if a key was modified during an ongoing (but uncommitted) transaction. Should `get()` return the **in-transaction** or **pre-transaction** value? I opted for the **in-transaction** value.

To manage transaction states, I implemented a **transaction state struct**, which tracks:

- Whether a transaction is active
- Pending database updates (keys set/removed within the transaction)

`get()` queries check:

1. The **transaction state** (for freshly set values)
2. The **cache**
3. The **internal storage**  

This approach makes transaction rollbacks simple—just discard pending updates. **Committing** a transaction merges its data into the cache and internal storage.

### Caching Implementation

For caching, I implemented a **basic LRU cache** with a **capacity of 5 elements** (small but sufficient for testing).  
It balances space efficiency and performance by evicting the **least recently used** element when inserting new values. This was also my first time implementing an LRU cache in **C++**, so it was a learning experience.

Since the **database interface already handles synchronization**, I did **not** add explicit multithreading support for the cache. As a result, the cache **is not thread-safe** if used independently.

### Multithreading Support

For **multithreading**, I followed a **reader-writer model**—something I'm familiar with in **Swift** but not as much in **C++**.  

- **`get()` operations** use `std::shared_lock` - allowing **multiple threads** to read simultaneously.
- **`set()` and `remove()` operations** use `std::unique_lock` - ensuring **only one writer** at a time.

This approach allows for high concurrency while maintaining data integrity.

### Unit Testing

To validate my implementation, I wrote **unit tests** using **Google Test (GTest)**.  

There are **two test suites**:

1. **`DatabaseInterfaceTest`**  
   - Tests database operations, including:
     - Transaction rollbacks
     - Multithreading behavior
     - Restrictions on modifications before a transaction starts
     - Basic CRUD operations  

2. **`LRUCacheTest`**  
   - Tests **cache behavior**:
     - Caching logic
     - Cache eviction when full

I'm still unsure about my **transaction design choice**—allowing multiple operations within a transaction could lead to **large transaction states**, which might be a concern.  

Overall, this was a **fun challenge**! There’s still room for improvement, so I might continue refining it regardless of feedback :)
