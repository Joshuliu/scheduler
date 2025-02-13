//
// Created by Joseph on 10/7/2017.
//

#ifndef LAB2_PCB_H
#define LAB2_PCB_H

#include <iostream>
#include "vector"

//data structure to hold process information
struct PCB{
    int pid, arrival, burst, priority, num_context, queue_num;
    float time_left, resp_time, wait_time, finish_time;
    bool started;
    std::vector<int> process_state; //1=ready,2=running,3=blocked

    PCB(){pid = arrival = burst = time_left = priority = resp_time = wait_time = num_context = finish_time = started = 0;}
    PCB(int id, int arr, int time, int prio) : pid(id), arrival(arr), burst(time), time_left(time), priority(prio){
        resp_time = wait_time = num_context = finish_time = started = 0;
        process_state.push_back(1);
    }
    void print(){
        std::cout << pid << " " << arrival << " " << time_left << " " << priority << std::endl;
    }
    void set_state(int state){
        process_state.push_back(state);
    }
    std::string print_process_state_nums() {
        std::string state_str = "Process State for PCB " + std::to_string(pid) + ": ";
        for (int state : process_state) {
            state_str += std::to_string(state) + " ";
        }
        state_str += "\n";
        return state_str;
    }
    std::string print_process_state() {
        std::string state_str = "";
        for (int state : process_state) {
            if (state == 1) {
                state_str += "Re ";
            } else if (state == 2) {
                state_str += "Ru ";
            } else if (state == 3) {
                state_str += "Bl ";
            }
        }
        state_str += "\n";
        return state_str;
    }
};
#endif //LAB2_PCB_H
