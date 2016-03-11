/*
 * Neuromapp - pool.cpp, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/coreneuron_1.0/queueing/pool.cpp
 * \brief Contains pool class definition.
 */

#include <boost/array.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <ctime>
#include <numeric>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/range/algorithm_ext/iota.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random.hpp>

#include "coreneuron_1.0/queueing/pool.h"

namespace queueing {

pool::pool(int numCells, int nLocal, int nIte,
int simTime, bool verbose, bool algebra, int nSpike,
int out, int in, int nc, int procs, int rank){
    num_cells_ = numCells;
    num_local_ = nLocal;
    num_ite_ = nIte;
    num_spike_ = nSpike;
    sim_time_ = static_cast<double>(simTime);
    std::cout<<simTime<<std::endl;
    v_ = verbose;
    perform_algebra_ = algebra;
    num_out_ = out;
    num_in_ = in;
    netcons_per_input_ = nc;
    num_procs_ = procs;
    rank_ = rank;
    spike_events_ = 0;
    total_received_ = 0;
    total_relevant_ = 0;

    thread_datas_.resize(num_cells_);

    //assign input and output gid's
    std::vector<int> available_inputs;
    std::vector<int> cellgroups;
    assert(num_cells_ > 2);
    if(num_procs_ > 1){
        for(int i = 0; i < (num_procs_ * num_out_); ++i){
            if(i >= (rank_ * num_out_) && i < ((rank_ * num_out_) + num_out_)){
                output_presyns_.push_back(i);
            }
            else{
                available_inputs.push_back(i);
            }
        }
        //create a randomly ordered list of input_presyns_
        assert(available_inputs.size() >= num_in_);

        //random presyn and netcon selection
        boost::mt19937 generator(time(NULL) + rank_);
        boost::uniform_int<> uni_dist;
        boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
            randomNumber(generator, uni_dist);
        boost::random_shuffle(available_inputs, randomNumber);
        available_inputs.resize(num_in_);
        //create a vector of randomly ordered cellgroups
        cellgroups.resize(num_cells_);
        boost::iota(cellgroups, 0);

        //for each input presyn,
        //select N unique netcons to cell groups
        boost::random_shuffle(cellgroups, randomNumber);
        for(int i = 0; i < num_in_; ++i){
            int presyn = available_inputs[i];
            for(int j = 0; j < netcons_per_input_; ++j){
                input_presyns_[presyn].push_back(cellgroups[j]);
            }
        }
    }
    else{
        for(int i = 0; i < num_out_; ++i){
            output_presyns_.push_back(i);
        }
        assert(input_presyns_.empty());
    }

    nin_.resize(num_procs_);
    displ_.resize(num_procs_);
}

void pool::set_displ(){
    displ_[0] = 0;
    int total = nin_[0];
    for(int i=1; i < num_procs_; ++i){
        displ_[i] = total;
        total += nin_[i];
    }
    spikein_.resize(total);
}

void pool::accumulate_stats(){
    int all_ite_received = 0;
    int all_enqueued = 0;
    int all_delivered = 0;
    for(int i=0; i < thread_datas_.size(); ++i){
        all_ite_received += thread_datas_[i].ite_received_;
        all_enqueued += thread_datas_[i].enqueued_;
        all_delivered += thread_datas_[i].delivered_;
    }

    if(v_){
        std::cout<<"Total inter-thread received: "<<all_ite_received<<std::endl;
        std::cout<<"Total enqueued: "<<all_enqueued<<std::endl;
        std::cout<<"Events generated by spikes: "<<spike_events_<<std::endl;
        std::cout<<"Total delivered: "<<all_delivered<<std::endl;
    }
    neuromapp_data.put_copy("inter_received", all_ite_received);
    neuromapp_data.put_copy("enqueued", all_enqueued);
    neuromapp_data.put_copy("spikes", spike_events_);
    neuromapp_data.put_copy("delivered", all_delivered);
}

void pool::calculate_probs(double& lambda, double* cdf){
    double sum = num_spike_ + num_ite_ + num_local_;
    //mean here the amount of time between spikes for entire sim
    double mean = sim_time_ / sum;
    std::cout<<"MEAN = "<<mean<<std::endl;
    lambda = 1.0 / (mean * num_cells_);
    std::cout<<"LAMBDA = "<<lambda<<std::endl;
    cdf[SPIKE] = num_spike_/sum;
    cdf[ITE] = (num_spike_ + num_ite_)/sum;
}

void pool::generate_all_events(){
    int dest = 0;
    int n = 0;
    double event_time = 0;
    int int_tt = 0;
    double percent = 0;
    event_type type;
    boost::mt19937 rng(rank_ + time(NULL));

    double cumulative_percents[2];
    double lambda = 0;
    calculate_probs(lambda, cumulative_percents);

    //generates percentages between 0 and 1
    boost::random::uniform_real_distribution<> percent_g(0.0,1.0);
    //generates random increment for event time
    //follows poisson process
    boost::random::exponential_distribution<double> time_g(lambda);

    //generates indices for output_presyns_
    boost::random::uniform_int_distribution<> gid_g(0, (output_presyns_.size() - 1));

    //generates indices for threadDatas
    boost::random::uniform_int_distribution<> cellgroup_g(0, (num_cells_ - 1));

    for(size_t i = 0; i < thread_datas_.size(); ++i){
        event_time = 0;
        //create events up until simulation end
        while(event_time < sim_time_){
            //increment event time
            double diff = time_g(rng);
            event_time += diff;
            if(event_time <= sim_time_){
                ++n;
            }
            percent = percent_g(rng);
            //SPIKE EVENT
            if(percent < cumulative_percents[SPIKE]){
                type = SPIKE;
                dest = output_presyns_[gid_g(rng)];
            }
            //INTER THREAD EVENT
            else if(percent < cumulative_percents[ITE]){
                type = ITE;
                dest = cellgroup_g(rng);
                //dst cannot equal i
                while(dest == i)
                    dest = cellgroup_g(rng);

            }
            //LOCAL EVENT
            else{
                type = LOCAL;
                dest = i;//myID
            }
            int_tt = static_cast<int>(event_time);
            //push into generated events array
            thread_datas_[i].push_generated_event(dest, int_tt, type);
        }
        //std::cout<<"created "<<n<<" events"<<std::endl;
    }
}

double pool::send_events(int myID){
    int curTime = thread_datas_[myID].time_;
    while((thread_datas_[myID].gen_size() != 0) &&
          (thread_datas_[myID].top_event_time() <= curTime)){
        gen_event g = thread_datas_[myID].pop_generated_event();
        event e = g.first;
        event_type type = g.second;
        assert(e.data_ < num_cells_);
        //if spike event send to spike_out
        switch(type){
            case SPIKE:
                spike_lock_.acquire();
                e.t_ += min_delay_;
                spikeout_.push_back(e);
                spike_lock_.release();
                break;
            case LOCAL:
            //if destination id is my own, self event, else ite
                thread_datas_[e.data_].self_send(e.data_, e.t_);
                break;
            case ITE:
                thread_datas_[e.data_].inter_thread_send(
                    e.data_, (e.t_ + min_delay_));
                break;
            default:
                std::cerr<<"error: invalid event type:"<<type<<std::endl;
                exit(EXIT_FAILURE);
        }
    }
    return thread_datas_[myID].top_event_time();
}

void pool::filter(){
    total_received_ += spikein_.size();
    std::map<int, std::vector<int> >::iterator it;
    event ev;
    for(int i = 0; i < spikein_.size(); ++i){
        it = input_presyns_.begin();
        ev = spikein_[i];
        it = input_presyns_.find(ev.data_);
        if(it != input_presyns_.end()){
            ++total_relevant_;
            for(size_t j = 0; j < it->second.size(); ++j){
                int dest = it->second[j];
                //send using non-mutex inter-thread send here
                thread_datas_[dest].inter_send_no_lock(dest, ev.t_);
                ++spike_events_;
            }
        }
    }
    spikeout_.clear();
    spikein_.clear();
}

//PARALLEL FUNCTIONS
void pool::fixed_step(){
    int size = thread_datas_.size();
    double top_time = 0;
    int curTime;
    #pragma omp parallel for schedule(static,1)
    for(int i = 0; i < size; ++i){
        for(int j = 0; j < size; ++j){
            curTime = thread_datas_[i].time_;
            if(top_time <= curTime){
                assert(curTime <= sim_time_);
                std::cout<<"sim time:"<<sim_time_<<" time: "<<curTime<<std::endl;
                top_time = send_events(i);
            }
            //Have threads enqueue their interThreadEvents
            thread_datas_[i].enqueue_my_events();

            if(perform_algebra_)
                thread_datas_[i].l_algebra(curTime);

            /// Deliver events
            while(thread_datas_[i].deliver(i, curTime));

            ++thread_datas_[i].time_;
        }
    }
}

void pool::parallel_send(){
    int size = thread_datas_.size();
    #pragma omp parallel for schedule(static,1)
    for(int i = 0; i < size; ++i){
        send_events(i);
    }
}

void pool::parallel_enqueue(){
    int size = thread_datas_.size();
    #pragma omp parallel for schedule(static,1)
    for(int i = 0; i < size; ++i){
        thread_datas_[i].enqueue_my_events();
    }
}

void pool::parallel_algebra(){
    if(perform_algebra_){
        int size = thread_datas_.size();
        #pragma omp parallel for schedule(static,1)
        for(int i = 0; i < size; ++i){
            thread_datas_[i].l_algebra(thread_datas_[i].time_);
        }
    }
}

void pool::parallel_deliver(){
    int size = thread_datas_.size();
    #pragma omp parallel for schedule(static,1)
    for(int i = 0; i < size; ++i){
        while(thread_datas_[i].deliver(i, thread_datas_[i].time_));
    }
}

}
