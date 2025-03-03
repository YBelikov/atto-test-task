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
2. Go to repo dir: cd att-test-task
3. Create and enter a build dir: mkdir build && cd build
4. Generate build files: cmake ..
5. Enter build directory: cd build
6. Build a target: cmake --build . --target <target name> (one of the listed targets above)
7. Under this conditions a built binary lands into build/src/file_sort/ (for file_sort and file_generator targers) and in build/src/database_cache (for database_cache and database_cache_lib targers)
8. Unit tests test_lru_cache binary land into build/tests/database_cache directory

Running a program is just launching a binary.
For file_generator target run (assuming you are in build directory): file_generator \<full to the intended path of generation\>. Example: file_generator "/Users/user/test_file.txt"

For file_sort target run: file_sort \<path to an unsorted file\> \<path to the sorted file\>. Example: file_sort "/Users/user/test_file.txt" "/Users/user/sorted_file.txt"

For test_lru_cache just run the binary itself. Example: ./test_lru_cache

<h2> How it works </h2>

<h4>File generator</h4> 

For the file generation I decided to generate separately mantissa and exponent and combine them into a a double of scientific representation. 
I made calculating the output data size a bit brute force. Since we are spitting numbers in a resulting file I just generate a number put it in a string and calculate
the size of the string including new line character. The sum of the sizes of all generated string representations should be 1e9 bytes (bytes in GB).

For file sorting I choose external sorting approach + K-way merge. Since the file is 1GB in size I can't load all the contents in memory. Consequently I have to process data in chunks.
There were no explicit requirements on hard drive utilization (I hope "memory" requirement didn't impose that I am prohibites to use more than 100MB of hard drive as well). 
To cope with data this big I decided to:
1. Read doubles from a file in 80MB buffer.
2. Sort numbers when a buffer is full
3. Put the sorted numbers in temporary text in "/tmp" folder (I tested on Unix-based systems, so Windows was not my consideration)

When all the data from the input file is processed start merging temporary files
1. Read first numbers from each temp file
2. Put those numbers in a min-heap
3. Took a minimun out of a heap together with an associated index of input stream in the corresponding array in the program
4. Write this min double into an output file
5. Read the next number from the deduced stream
6. Repeat steps 1-4 until all temp files contents are processed
7. Remove all temporary files from the filesystem.

The current implementation imposes quite an overhead on CPU but satisfies RAM requiremens (I didn't manage to allocate enough time to reevaluate CPU performance).
On the tested systems memory utilization never spiked higher than 80MB for my tests. However, the test machine has to have enough hard drive memory for temporary files.

<h4>Database cache</h4>
So I had three requirements:
1. Transaction-based implementation
2. Multithreading support
3. Caching

I wasn't sure how to properly approach this task so I decided to implement the provided interface integrating transaction, cache and multithreading support.
Also I had doubts how to define one transaction: could it be only one modification operation or it could be several? Different software may have own definitions of "transaction". 
I sticked to the second option: a chain of set/remove operations count as one transaction until commit_transaction is called. Get query may be called in any time - I am still not sure 
how right I was regarding this decision since during a transaction I have to decide which data to return if a user queries the key modified during an opened but not commited transaction
(if the value for a key was set but not commited should get() return in-transaction or pre-transaction data? I sticked to **in-transaction** option).

I implemented a transaction state struct that tracks the actual state of transaction (whether it is active or not), pending updates to the database: which keys were set to which values
and which keys were removed during a transaction. Get queries just go straight through the transaction data (for freshly set fields), through the cache or the "internal storage". 
This way aborting transaction is just ignoring and clearing the data about set and removed keys. Commiting transaction is just merging the data into cache and internal storage.

To support caching I tried implementing a straighforward LRU cache. I have given it the capacity of 5 elements (not an impressive number but it is easier to test).
Since LRU kicks out the least used element when sets new value it is a good combination of space and space efficiency + I it was the first time I tried implementing it in C++.
Since database interface provides synchronization logic I wasn't adding multithreading support for the cache, consequently it is not safe to utilize the cache separately under such conditions.

Regarding multhithreading support I tried sticking to reader-writer model: something I am familiar with in Swift but not really in C++. 
Anyways it was interesting to try: I used std::unique_lock for write(set, remove) operations and std::shared_lock for read (get). This way any number of
threads can read database but only one thread at a time is capable of writing data there.

To test all this I tried implementing a couple of unit tests using GTest. All tests remain in test_lru_cache target and there are two test suits:
1. **DatabaseInterfaceTest**. Covers various cases for database manipulation: transaction rollback, multithreading support, prohibtion of changes before transaction is opened, basic operations
2. **LRUCacheTest**. Covers basic caching, cache eviction.

I am still not sure how right I was regarding the definition of transaction: multiple operations during one transaction may grow the transaction state to a quite big size, so that's my concern.

In general that was a fun test, there are a still points to improve, so maybe I will continue with this tasks independently of the feedback :)


