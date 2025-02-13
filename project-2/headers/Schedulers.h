#ifndef LAB2_SCHEDULER_H
#define LAB2_SCHEDULER_H

#include "DList.h"
#include "CPU.h"

class CPU;
class Scheduler;
// needs to be forward declared since Dispatcher and Scheduler mutually include each other
class Dispatcher
{
private:
    CPU *cpu;
    Scheduler *scheduler;
    std::vector<DList<PCB> *> ready_queue;
    Clock *clock;
    bool _interrupt;

public:
    Dispatcher();
    Dispatcher(CPU *cp, Scheduler *sch, std::vector<DList<PCB> *> rq, Clock *cl);
    PCB *switchcontext(int index);
    void execute();
    void interrupt();
};

class Scheduler
{
private:
    int next_pcb_index;
    int current_queue_index;
    std::vector<DList<PCB> *> ready_queue;
    DList<PCB> *queues;
    CPU *cpu;
    Dispatcher *dispatcher;
    int algorithm;
    float timeq, timer; // time quantum, timer to keep track of when to interrupt dispatcher

    // MLFQ queues
    int num_queues;
    std::vector<int> time_quantum;
    std::vector<int> aging_thresholds;

public:
    Scheduler();
    Scheduler(std::vector<DList<PCB> *> rq, CPU *cp, int alg);
    Scheduler(std::vector<DList<PCB> *> rq, CPU *cp, int alg, int tq);
    Scheduler(std::vector<DList<PCB> *> rq, CPU *cp, int alg, int n_queues, std::vector<int> t_quantum, std::vector<int> a_thresholds);
    // Scheduler(DList<PCB>* rq, CPU *cp, int alg, int n_queues, std::vector<int> t_quantum, std::vector<int> a_thresholds);
    void setdispatcher(Dispatcher *disp);
    int getnext();
    int getCurrentQueueIndex();
    void execute();
    void fcfs();
    void rr();
    void mlfq();
    int get_num_queues();
};

#endif // LAB2_SCHEDULER_H
