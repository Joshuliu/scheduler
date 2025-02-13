#include "../headers/StatUpdater.h"

StatUpdater::StatUpdater(std::vector<DList<PCB> *> rq, DList<PCB> *fq, Clock *cl, int alg, std::string fn, int tq)
{
    ready_queue = rq;
    finished_queue = fq;
    clock = cl;
    algorithm = alg;
    timeq = tq;
    filename = fn;
    last_update = 0;
}

// //main function that gets called every clock cycle to update times of pcbs
// void StatUpdater::execute() {
//     //increment handles situations where a context switch happens in middle of cycle
//     //allows updater to increment times
//     float increment = clock->gettime() - last_update;
//     last_update = clock->gettime();
//     for(int index = 0; index < ready_queue.size(); ++index){
//         //get pointer to each pcb in queue and update their waiting times
//         PCB* temp = ready_queue->getindex(index);
//         temp->wait_time += increment;
//     }
// }
void StatUpdater::execute()
{
    // Increment handles situations where a context switch happens in the middle of the cycle
    // Allows updater to increment times
    float increment = clock->gettime() - last_update;
    last_update = clock->gettime();

    // Iterate through each queue in ready_queue
    for (int queue_idx = 0; queue_idx < ready_queue.size(); ++queue_idx)
    {
        DList<PCB> *current_queue = ready_queue[queue_idx]; // Get the queue pointer at the current index

        // Iterate through each PCB in the current queue and update their waiting times
        for (int index = 0; index < current_queue->size(); ++index)
        {
            PCB *temp = current_queue->getindex(index); // Get the PCB at the current index in the queue
            temp->wait_time += increment;               // Update the wait time for the process
        }
    }
}

// straightforward print function that prints to file using iomanip and column for a table format
// uses finished queue to tally up final stats
void StatUpdater::print()
{
    num_tasks = finished_queue->size();
    std::string alg;
    int colwidth = 11;
    float tot_burst, tot_turn, tot_wait, tot_resp;
    int contexts;
    tot_burst = tot_turn = tot_wait = tot_resp = contexts = 0;

    std::ofstream outfile(filename);

    for (int id = 1; id < num_tasks + 1; ++id)
    {
        for (int index = 0; index < finished_queue->size(); ++index)
        {
            if (finished_queue->getindex(index)->pid == id)
            {
                PCB temp = finished_queue->removeindex(index);
                float turnaround = temp.finish_time - temp.arrival;
                tot_turn += turnaround;
                tot_wait += temp.wait_time;
                tot_resp += temp.resp_time;
                contexts += temp.num_context;
            }
        }
    }
    outfile << "Average Waiting Time: " << tot_wait / num_tasks << " ms" << std::endl
            << "Average Turnaround Time: " << tot_turn / num_tasks << " ms" << std::endl
            << "Average Response Time: " << tot_resp / num_tasks << " ms" << std::endl
            << "Total No. of Context Switching Performed: " << contexts << std::endl;
}