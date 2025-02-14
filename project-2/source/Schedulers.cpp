#include "../headers/Schedulers.h"
// #define DEBUG

Scheduler::Scheduler()
{
    next_pcb_index = -1;
    ready_queue.clear();
}

// constructor for non-RR algs
Scheduler::Scheduler(std::vector<DList<PCB> *> rq, CPU *cp, int alg)
{
    ready_queue = rq;
    cpu = cp;
    dispatcher = NULL;
    next_pcb_index = -1;
    current_queue_index = -1;
    algorithm = alg;
    num_queues = 1;
}

// constructor for RR alg
Scheduler::Scheduler(std::vector<DList<PCB> *> rq, CPU *cp, int alg, int tq)
{
    ready_queue = rq;
    cpu = cp;
    dispatcher = NULL;
    next_pcb_index = -1;
    algorithm = alg;
    num_queues = 1;
    timeq = timer = tq;
}
Scheduler::Scheduler(std::vector<DList<PCB> *> rq, CPU *cp, int alg, int n_queues, std::vector<int> t_quantum, std::vector<int> a_thresholds)
{
    ready_queue = rq;
    cpu = cp;
    dispatcher = NULL;
    next_pcb_index = -1;
    current_queue_index = -1;
    algorithm = alg;
    num_queues = n_queues;
    time_quantum = t_quantum;
    aging_thresholds = a_thresholds;
}

// dispatcher needed to be set after construction since they mutually include each other
// can only be set once
void Scheduler::setdispatcher(Dispatcher *disp)
{
    if (dispatcher == NULL)
        dispatcher = disp;
}

// dispatcher uses this to determine which process in the queue to grab
int Scheduler::getnext()
{
    return next_pcb_index;
}

int Scheduler::getCurrentQueueIndex()
{
    return current_queue_index;
}

int Scheduler::get_num_queues()
{
    return num_queues;
}

// switch for the different algorithms
void Scheduler::execute()
{
    if (timer > 0)
        timer -= .5;

    for (int i = 0; i < num_queues; i++)
    {
        if (ready_queue[i]->size() != NULL)
        {
            switch (algorithm)
            {
            case 0:
                fcfs();
                break;
            case 2:
                rr();
                break;
            case 4: // This is the algorithm number for MLFQ, please do not change it, just implement the MLFQ in Scheduler::mlfq()
                mlfq();
                break;
            default:
                break;
            }
            break;
        }
    }
}

// simply waits for cpu to go idle and then tells dispatcher to load next in queue
void Scheduler::fcfs()
{
    ready_queue[0]->getindex(0)->queue_num = next_pcb_index = current_queue_index = 0;
    if (cpu->isidle())
        dispatcher->interrupt();
}

// round robin, simply uses timer and interrupts dispatcher when timer is up, schedules next in queue
void Scheduler::rr()
{
    if (cpu->isidle() || timer <= 0)
    {
        timer = timeq;
        ready_queue[0]->getindex(0)->queue_num = next_pcb_index = current_queue_index = 0;
        dispatcher->interrupt();
    }
}

// MLFQ, not implemented
// You need to implement MLFQ in this function
void Scheduler::mlfq()
{
    for (int i = 0; i < num_queues; ++i)
    {
        // Iterate through queues (priority order)
        if (ready_queue[i]->size() > 0)
        {
            // Starvation prevention logic
            int queue_size = ready_queue[i]->size();
            int k = 0;
            while (i > 0 && k < queue_size) // Use while loop to handle removal properly
            {
                PCB *process = ready_queue[i]->getindex(k); // Get process pointer
                if (process != nullptr)                     // Ensure process is valid
                {
                    if (process->wait_time >= aging_thresholds[i])
                    {
#ifdef DEBUG
                        std::cout << "[DEBUG] Promoted PID " << process->pid << " (Waited @" << i << ":" << k << " for " << process->wait_time << "/" << aging_thresholds[i] << " secs)" << std::endl;
#endif
                        PCB moved_process = *process;
                        moved_process.wait_time = 0; // Reset wait time

                        // promote aged process
                        ready_queue[i]->removeindex(k);
                        ready_queue[i - 1]->add_end(moved_process);
                        queue_size--; // Decrement queue size without incrementing k
                    }
                    else
                    {
                        k++; // Move to the next index if no removal occurred
                    }
                }
            }

            // Select next available queue
            if (ready_queue[i]->size() > 0)
            {
                next_pcb_index = 0;                                               // Index 0 for the front of the current queue
                ready_queue[i]->getindex(0)->queue_num = current_queue_index = i; // Store the index of the current queue
                if (cpu->isidle() || timer <= 0)
                {
                    dispatcher->interrupt();
                    timer = time_quantum[i];
                    return;
                }
            }
        }
    }
}

/*
 *
 * Dispatcher Implementation
 *
 */
Dispatcher::Dispatcher()
{
    cpu = NULL;
    scheduler = NULL;
    ready_queue.clear();
    clock = NULL;
    _interrupt = false;
}

Dispatcher::Dispatcher(CPU *cp, Scheduler *sch, std::vector<DList<PCB> *> rq, Clock *cl)
{
    cpu = cp;
    scheduler = sch;
    ready_queue = rq;
    clock = cl;
    _interrupt = false;
};

// function to handle switching out pcbs and storing back into ready queue
PCB *Dispatcher::switchcontext(int index)
{
    PCB *old_pcb = cpu->pcb;
    if (old_pcb != NULL)
    {
#ifdef DEBUG
        std::cout << "[DEBUG] Old process (PID: " << old_pcb->pid << ") is being moved to ready state." << std::endl;
#endif
        old_pcb->set_state(1);
    }
    PCB *new_pcb = new PCB(ready_queue[scheduler->getCurrentQueueIndex()]->removeindex(scheduler->getnext()));
#ifdef DEBUG
    if (new_pcb != NULL)
    {
        std::cout << "[DEBUG] Process (PID: " << new_pcb->pid << ") removed from ready_queue and added to CPU." << std::endl;
    }
#endif
    if (new_pcb != NULL)
    {
        new_pcb->set_state(2);
#ifdef DEBUG
        std::cout << "[DEBUG] New process (PID: " << new_pcb->pid << ") is being assigned to the CPU." << std::endl;
#endif
    }

    cpu->pcb = new_pcb;
#ifdef DEBUG
    if (old_pcb != NULL & new_pcb != NULL)
    {
        std::cout << "[DEBUG] Context switch from process (PID: " << old_pcb->pid << ") to process (PID: " << new_pcb->pid << ")" << std::endl;
    }
#endif
    return old_pcb;
}

// executed every clock cycle, only if scheduler interrupts it
void Dispatcher::execute()
{
    if (_interrupt)
    {
        PCB *old_pcb = switchcontext(scheduler->getnext());
        if (old_pcb != NULL)
        { // only consider it a switch if cpu was still working on process
            old_pcb->num_context++;
            // context switch overhead is 0.5
            cpu->getpcb()->wait_time += .5;
#ifdef DEBUG
            std::cout << "[DEBUG] Context switch overhead for process (PID: " << old_pcb->pid << ") is 0.5" << std::endl;
#endif
            clock->step();

            // demotion start
            if (old_pcb->queue_num < scheduler->get_num_queues() - 1)
            {
                ready_queue[old_pcb->queue_num + 1]->add_end(*old_pcb); // Add back to correct queue
            }
            else
            {
                ready_queue[old_pcb->queue_num]->add_end(*old_pcb);
            }
            // demotion end

#ifdef DEBUG
            std::cout << "[DEBUG] Process (PID: " << old_pcb->pid << ") added back to ready_queue." << std::endl;
#endif
            delete old_pcb;
        }
        _interrupt = false;
    }
}

// routine for scheudler to interrupt it
void Dispatcher::interrupt()
{
#ifdef DEBUG
    std::cout << "[DEBUG] Interrupt triggered!" << std::endl;
#endif
    _interrupt = true;
}