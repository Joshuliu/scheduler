#include "../headers/CPU.h"

CPU::CPU(DList<PCB> *fq, Clock *cl) {
    pcb = NULL;
    idle = true;
    finished_queue = fq;
    clock = cl;
}

//used by others to determine what the cpu is working on like priority and time left
PCB* CPU::getpcb() {
    return pcb;
}

//check to see if cpu is currently working on a process
bool CPU::isidle() {
    return idle;
}

//called every clock cycle
void CPU::execute() {
    if(pcb != NULL){
        idle = false;
        if(!pcb->started){ //helps determine response time, only increments it if pcb hasn't been worked on yet
            pcb->started = true;
            pcb->resp_time = clock->gettime() - pcb->arrival;
            #ifdef DEBUG
            std::cout << "[DEBUG] Process " << pcb->pid << " has started at time " << clock->gettime() << " for the first time." << std::endl;
            #endif
        }
        #ifdef DEBUG
        std::cout << "[DEBUG] Process " << pcb->pid << " is being worked on at time " << clock->gettime() << std::endl;
        #endif
        pcb->time_left -= .5; //simulate process being worked on for a clock cycle
        if(pcb->time_left <= 0) { //terminate it if its done and set self to idle
            terminate();
            idle = true;
        }

    }
}

//routine to update termination related stats, for StatUpdater to use later
void CPU::terminate() {
    pcb->finish_time = clock->gettime()+.5;
    #ifdef DEBUG
    std::cout << "[DEBUG] Process " << pcb->pid << " has finished at time " << pcb->finish_time << std::endl;
    #endif
    finished_queue->add_end(*pcb);
    delete pcb;
    pcb = NULL;
}