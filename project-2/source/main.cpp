#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include "../headers/Clock.h"
#include "../headers/PCBGenerator.h"
#include "../headers/CPU.h"
#include "../headers/StatUpdater.h"
using namespace std;
// #define DEBUG

void print_ready_queue(std::vector<DList<PCB> *> ready_queue, int num_queues)
{
#ifdef DEBUG
    // prints ready queue for debugging
    for (int i = 0; i < num_queues; i++)
    {
        std::cout << "Queue " << i << ":" << std::endl;
        if (ready_queue[i])
        {
            ready_queue[i]->print();
        }
        else
        {
            std::cout << "Queue is empty." << std::endl;
        }
    }
#endif
}

bool ready_queues_non_empty(std::vector<DList<PCB> *> &ready_queue, int num_queues)
{
    // returns if any ready queues are not empty
    for (int i = 0; i < num_queues; i++)
    {
        if (ready_queue[i]->size() != 0)
        {
            return true; // Found a non-empty queue
        }
    }
    return false;
}

int main(int argc, char *argv[])
{
    // initial args validation
    if (argc < 4)
    {
        cout << "Not enough arguments sent to main." << endl;
        cout << "Format should be: ./bin/schedsim inputfile outputfile algorithm timequantum(if algorithm == 2)" << endl;
        return EXIT_FAILURE;
    }

    if (atoi(argv[3]) == 2 && argc == 4)
    {
        cout << "Need to provide time quantum when using Round Robin algorithm" << endl;
        return EXIT_FAILURE;
    }
    // variables to hold initial arguments
    int algorithm = std::atoi(argv[3]);
    int timeq = -1;
    if (algorithm == 2)
        timeq = atoi(argv[4]);

    int num_queues = 1;
    std::vector<int> time_quantum;
    std::vector<int> aging_thresholds;

    if (algorithm == 4)
    { // MLFQ
        num_queues = atoi(argv[4]);
        if (argc < 5)
        {
            std::cout << "Not enough arguments for MLFQ scheduling." << std::endl;
            return EXIT_FAILURE;
        }

        if (argc != 5 + num_queues + (num_queues - 1))
        {
            std::cout << "Invalid number of arguments for MLFQ. Expected format:" << std::endl;
            std::cout << "./bin/schedsim inputfile outputfile 4 <num_queues> <time_quantum_list> <aging_threshold_list>" << std::endl;
            return EXIT_FAILURE;
        }
        time_quantum.resize(num_queues);
        aging_thresholds.resize(num_queues);

        // Read time quanta
        for (int i = 0; i < num_queues; ++i)
        {
            time_quantum[i] = std::atoi(argv[5 + i]);
            if (time_quantum[i] <= 0)
            {
                std::cerr << "Time quantum must be greater than zero." << std::endl;
                return EXIT_FAILURE;
            }
        }

        // Read aging thresholds (one fewer than num_queues)
        aging_thresholds[0] = 0;
        for (int i = 0; i < num_queues - 1; ++i)
        {
            aging_thresholds[i + 1] = std::atoi(argv[5 + num_queues + i]);
            if (aging_thresholds[i + 1] <= 0)
            {
                std::cerr << "Aging threshold must be greater than zero." << std::endl;
                return EXIT_FAILURE;
            }
        }

#ifdef DEBUG
        std::cout << "MLFQ Configuration: " << num_queues << " queues, Time quanta: ";
        for (int tq : time_quantum)
        {
            std::cout << tq << " ";
        }
        std::cout << " Aging thresholds: ";
        for (int at : aging_thresholds)
        {
            std::cout << at << " ";
        }
        std::cout << std::endl;
#endif
    }

    // queues to hold PCBs throughout
    std::vector<DList<PCB> *> ready_queue;
    ready_queue.resize(num_queues);
    for (int i = 0; i < num_queues; i++)
    {
        ready_queue[i] = new DList<PCB>();
    }
    auto finished_queue = new DList<PCB>();

    try
    {
        // all the objects that will work together to simulate OS process management
        Clock clock;
        PCBGenerator pgen(argv[1], ready_queue, &clock);
        StatUpdater stats(ready_queue, finished_queue, &clock, algorithm, argv[2], timeq);
        CPU cpu(finished_queue, &clock);
        Scheduler *scheduler;
        switch (algorithm)
        {
        case 4: // MLFQ
            scheduler = new Scheduler(ready_queue, &cpu, algorithm, num_queues, time_quantum, aging_thresholds);
            break;

        case 2: // RR
            scheduler = new Scheduler(ready_queue, &cpu, algorithm, timeq);
            break;

        default: // For other algorithms
            scheduler = new Scheduler(ready_queue, &cpu, algorithm);
            break;
        }

        Dispatcher dispatcher(&cpu, scheduler, ready_queue, &clock);
        scheduler->setdispatcher(&dispatcher);

        // loop will continue until no more processes are going to be generated, no more in ready queue, and cpu is done
        while (!pgen.finished() || ready_queues_non_empty(ready_queue, num_queues) || !cpu.isidle())
        {
#ifdef DEBUG
            cout << "[DEBUG] clock.gettime()=" << clock.gettime() << " pgen.finished()=" << pgen.finished() << " cpu.isidle()=" << cpu.isidle() << std::endl;
#endif
            pgen.generate();
            print_ready_queue(ready_queue, num_queues);
            scheduler->execute();
            dispatcher.execute();
            cpu.execute();
            stats.execute();
            clock.step();
            // usleep(1 * 1000000);
        }

        // final printing of performance metrics
        stats.print();
    }
    catch (int)
    {
        delete &ready_queue;
        delete finished_queue;
        return EXIT_FAILURE;
    }

    return 0;
}