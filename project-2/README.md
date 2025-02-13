# Process Scheduling Simulator

This is a software implementation of how an OS might schedule processes on a CPU given a file of "processes." The simulator currently supports FCFS and RR scheduling algorithms and outputs performance metrics to a specified file.

This directory contains a `makefile` to help you compile, run, test, debug, and evaluate the simulator. Please read the instructions below carefully.

---

## Getting Started

### Compile and Run
Before running, the program need to compile `.cpp` files into `.o` files and link them to create the final executable. You can compile everything using the following command:
```
make
```
During compiling, you may see many warnings. Don't worry and you can ignore them for now.

After compilation, the executable will be located in the `bin/` folder. You can run the program manually using the following format:
```
./bin/schedsim <inputFileName> <outputFileName> <AlgorithmNumber> [<timeQuantum>]
```

#### Example Usage
```
./bin/schedsim sample_input.txt output_fcfs.txt 0     # Test with FCFS
./bin/schedsim sample_input.txt output_rr.txt 2 1     # Test with RR
```

#### Input/Output Files
- **Input File**: Each line in the input file should follow the format:
  ```
  ProcessID ArrivalTime BurstTime Priority
  ```
  For example:
  ```
  1 0 8 2
  2 1 4 3
  ```

- **Output File**: The output contains performance metrics for the scheduling algorithm:
  - Average Waiting Time
  - Average Turnaround Time
  - Average Response Time
  - Context Switch Count

#### Algorithm Numbers
0 - FCFS, First Come First Serve

2 - RR, Round Robin (must enter time quantum to execute)

#### Time Quantum
How long each process runs on the CPU for RR (minimum = .5)

#### Prerequisites

Program can be compile and ran with specified arguments using most IDEs that support C++. Implementation will depend on platform.

### Clean Compiled Files
If you want to remove all compiled `.o` files and the executable, use:
```
make clean
```
After that, you can run `make` to recomplie the whole program to ensure that any changes to your source code are reflected in the program.

---

## Project 2: Implementing MLFQ

In Project 2, your goal is to extend the simulator by implementing the Multilevel Feedback Queue (MLFQ) scheduling algorithm. Detailed descriptions of tasks and objectives are available on Blackboard.


### Testing MLFQ
Use `4` for the algorithm number to call MLFQ. Additionally, task 1 asks you to provide the following inputs and load them to the program:
- Number of queues.
- Time quanta for each queue (from highest to lowest priority).
- Aging thresholds for starvation prevention (one less than the number of queues).

#### Example Command
```
./bin/schedsim sample_input.txt output_mlfq.txt 4 3 1 2 3 8 16
```

Alternatively, you can use the `Makefile` for automated testing:
```
make test-MLFQ QUEUES=3 QUANTA='1 2 3' AGING='8 16'
```

### Debugging
To display debug messages during execution, use:
```
make debug-MLFQ QUEUES=3 QUANTA='1 2 3' AGING='8 16'
```

There are some useful debug messages in source files that can help you understand how the simulator works. Once you finished Task 1, you can always use debug command to help you finish the other tasks. You can also add your own debug messages by wraping debug statements with the `DEBUG` preprocessor directive, and here is an example:
```cpp
#ifdef DEBUG
std::cout << "[DEBUG] Debug message here." << std::endl;
#endif
```

### Evaluation

You can evaluate the performance of your MLFQ implementation using the following command:

```bash
make evaluate-MLFQ QUEUES=3 QUANTA='1 2 3' AGING='8 16'
```

The above command will:
1. Compile your implementation.
2. Use the `sample_input.txt` file along with the specified input arguments (`QUEUES`, `QUANTA`, `AGING`) to run both your implementation and the TA’s implementation.
3. Display a comparison table of performance metrics.

Below is an example of the output you can expect:

```
Running MLFQ with QUEUES=3, QUANTA=1 2 3, AGING=8 16...
+---------------------+---------------------+---------------------+------------+
| Performance Metrics | Your Implementation | TA's Implementation | Difference |
+---------------------+---------------------+---------------------+------------+
| Waiting Time        | 42.5                | 40.2                | 2.3        |
| Turnaround Time     | 48.85               | 45.85               | 3.0        |
| Response Time       | 3.2                 | 2.1                 | 1.1        |
| Context Switches    | 49                  | 43                  | 6          |
+---------------------+---------------------+---------------------+------------+
```

You can modify the input arguments (`QUEUES`, `QUANTA`, `AGING`) to evaluate how your implementation performs under different configurations, which will help you improve your implementation and get bonus points if you outperforms the TA’s implementation in grading.


### Submission

There is a `Makefile` in the `proj2-sched/` directory to help you prepare for submission. Please note that it will include only the `.h` files from the `headers/` folder and the `.cpp` files from the `source/` folder in the submission. Any other files will not be included in the submission. Make sure to place all necessary `.h` and `.cpp` files including your modifications in their respective folders before running `make submit`.
