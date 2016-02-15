/*
 * Neuromapp - pool.h, Copyright (c), 2015,
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
 * @file neuromapp/coreneuron_1.0/queueing/pool.h
 * \brief Contains pool class declaration.
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

#include "coreneuron_1.0/queueing/thread.h"
#include "utils/storage/neuromapp_data.h"

#ifdef _OPENMP
#include <omp.h>
#endif

#ifndef MAPP_POOL_H_
#define MAPP_POOL_H_

namespace queueing {

template<implementation I>
class nrn_thread_data;

template<implementation I>
class pool {
private:
#ifdef _OPENMP
    omp_lock spike_lock_;
#else
    dummy_lock spike_lock_;
#endif

    int cell_groups_;
    int events_per_step_;
    int percent_ite_;
    int percent_spike_;
    bool v_;
    bool perform_algebra_;
    int time_;
    int all_spiked_;
    const static int min_delay_ = 5;

    //needed for spike interface
    int num_out_;
    int num_in_;
    int num_procs_;
    int rank_;
    int total_received_;
    int total_relevent_;

    std::vector<nrn_thread_data<I> > thread_datas_;

public:
    std::vector<event> spikein_;
    std::vector<event> spikeout_;
    std::vector<int> input_presyns_;
    std::vector<int> output_presyns_;
    std::vector<int> nin_;
    std::vector<int> displ_;

    /** \fn pool(bool verbose, int eventsPer, int pITE, bool isSpike, bool algebra)
     *  \brief initializes a pool with a thread_datas_ array
     *  \param verbose verbose mode: 1 = on, 0 = off
     *  \param events_per_step_ number of events per time step
     *  \param percent_ite_ is the percentage of inter-thread events
     *  \param isSpike determines whether or not there are spike events
     *  \param algebra determines whether to perform linear algebra calculations
     */
    explicit pool(int numCells=1, int eventsPer=1, int pITE=0,
    bool verbose=false, bool algebra=false, int pSpike=0,
    int out=1, int in=1, int procs=1, int rank=0):
    cell_groups_(numCells), events_per_step_(eventsPer),
    percent_ite_(pITE), v_(verbose), perform_algebra_(algebra),
    percent_spike_(pSpike), num_out_(out), num_in_(in),
    num_procs_(procs), rank_(rank), all_spiked_(0), time_(0)
    {
        srand(time(NULL));

        thread_datas_.resize(cell_groups_);

        //taken from environment.cpp
        total_received_ = 0;
        total_relevent_ = 0;

        //assign input and output gid's
        if(num_procs_ > 1){
            for(int i = 0; i < (num_procs_ * num_out_); ++i){
                if(i >= (rank_ * num_out_) && i < ((rank_ * num_out_) + num_out_)){
                    output_presyns_.push_back(i);
                }
                else{
                    input_presyns_.push_back(i);
                }
            }
            assert(input_presyns_.size() >= num_in_);
        }
        else{
            for(int i = 0; i < num_out_; ++i){
                output_presyns_.push_back(i);
            }
            assert(input_presyns_.empty());
        }
        boost::random_shuffle(input_presyns_);
        input_presyns_.resize(num_in_);

        spikein_.reserve(num_procs_ * events_per_step_ * min_delay_);
        nin_.resize(num_procs_);
        displ_.resize(num_procs_);
    }

    void set_displ(){
        displ_[0] = 0;
        int total = nin_[0];
        for(int i=1; i < num_procs_; ++i){
            displ_[i] = total;
            total += nin_[i];
        }
        spikein_.resize(total);
    }

    bool matches(const event& item){
        for(int i = 0; i < input_presyns_.size(); ++i){
            if(item.data_ == input_presyns_[i]){
                return true;
            }
        }
        return false;
    }

    /** \fn accumulate_stats()
     *  \brief accumulates statistics from the threadData array and stores them using impl::storage
     */
    void accumulate_stats(){
        int all_ite_received = 0;
        int all_enqueued = 0;
        int all_delivered = 0;
        for(int i=0; i < thread_datas_.size(); ++i){
            all_ite_received += thread_datas_[i].ite_received_;
            all_enqueued += thread_datas_[i].enqueued_;
            all_delivered += thread_datas_[i].delivered_;
            if(v_){
                std::cout<<"Cellgroup "<<i<<" ite received: "
                <<thread_datas_[i].ite_received_<<std::endl;
                std::cout<<"Cellgroup "<<i<<" enqueued: "<<
                thread_datas_[i].enqueued_<<std::endl;
                std::cout<<"Cellgroup "<<i<<" delivered: "<<
                thread_datas_[i].delivered_<<std::endl;
            }
        }

        if(v_){
            std::cout<<"Total inter-thread received: "<<all_ite_received<<std::endl;
            std::cout<<"Total enqueued: "<<all_enqueued<<std::endl;
            std::cout<<"Total spiked: "<<all_spiked_<<std::endl;
            std::cout<<"Total delivered: "<<all_delivered<<std::endl;
        }
        neuromapp_data.put_copy("inter_received", all_ite_received);
        neuromapp_data.put_copy("enqueued", all_enqueued);
        neuromapp_data.put_copy("spikes", all_spiked_);
        neuromapp_data.put_copy("delivered", all_delivered);
    }

    /** \fn void time_step()
     *  \brief master function to call generate, enqueue, and deliver
     */
    void time_step(){
        int size = thread_datas_.size();
        #pragma omp parallel for schedule(static,1)
        for(int i = 0; i < size; ++i){
            send_events(i);

            //Have threads enqueue their interThreadEvents
            thread_datas_[i].enqueue_my_events();

            if(perform_algebra_)
                thread_datas_[i].l_algebra(time_);

                        /// Deliver events
            while(thread_datas_[i].deliver(i, time_));
        }
    }

    /** \fn void generate_all_events(int totalTime)
     *  \brief creates all events for each thread that will be sent
     *   and received during the simulation lifetime.
     *  these are stored in the vector generated_events[] for each thread
     *  \param totalTime provides the total simulation time
     *  \postcond all events for the simulation are generated
     */
    void generate_all_events(int totalTime){
        event ev;
        for(int i = 0; i < cell_groups_; ++i){
            for(int j = 0; j < totalTime; ++j){
                /// Simulated target of a NetCon and the event time
                for(int k = 0; k < events_per_step_; ++k){
                    //events can be generated with time range: (current time) to (current time + 10%)
                    gen_event g = create_event(i,j,totalTime);
                    thread_datas_[i].push_generated_event(
                        g.first.data_, g.first.t_, g.second);
                }
            }
        }
    }

    /** \fn send_events(int myID)
     *  \brief sends event to it's destination
     *  \param myID the thread index
     *  \precond generateAllEvents has been called
     *  \postcond thread_datas_[myID].generated_events size -= 1
     */
    void send_events(int myID){
        for(int i = 0; i < events_per_step_; ++i){
            gen_event g = thread_datas_[myID].pop_generated_event();
            event e = g.first;
            bool is_spike_ = g.second;
            //if spike event send to spike_out
            if(is_spike_){
                spike_lock_.acquire();
                e.t_ += min_delay_;
                spikeout_.push_back(e);
                spike_lock_.release();
            }
            //if destination id is my own, self event, else ite
            else if(e.data_ == myID)
                thread_datas_[e.data_].self_send(e.data_, e.t_);
            else
                thread_datas_[e.data_].inter_thread_send(e.data_, (e.t_ + min_delay_));
        }
    }

    /** \fn void filter()
     *  \brief compensates for the spike exchange by adding events every 5 timesteps
     */
    void filter(){
        total_received_ += spikein_.size();

        //"Receive" spikes
        event ev;
        for(int i = 0; i < spikein_.size(); ++i){
            ev = spikein_[i];
            if(matches(ev)){
                ++total_relevent_;
                //spikein_[i];
                int dst = ev.data_;
                //add non-mutex inter-thread send here
             //   thread_datas_[dst].self_send(ev.data_, ev.t_);
            }
        }
        //"Send" spikes
        spikeout_.clear();
        spikein_.clear();
    }

    /** \fn int create_event(int myID, int curTime, int totalTime)
     *  \brief randomly generates a new event with data dependent on the values of
     *  percent-ite and percent-spike
     *  \param myID the thread index
     *  \param curTime the current time
     *  \param totalTime the total simulation time
     *  \return new_event
     */
    gen_event create_event(int myID, int curTime, int totalTime){
        event new_event;
        int diff = 1;
        if(totalTime > 10)
               diff = rand() % (totalTime/10);

        //set time_ to be some time in the future t + diff
        new_event.t_ = static_cast<double>(curTime + diff);

        int dst = myID;
        bool is_spike = false;
        int percent = rand() % 100;
        if (percent < percent_spike_){
            //send as spike
            assert(output_presyns_.size() > 0);
            int index = rand() % output_presyns_.size();
            dst = output_presyns_[index];
            is_spike = true;
            all_spiked_++;
        }
        else if(percent < (percent_spike_ + percent_ite_)){
            //send as inter_thread_event_
            while(dst == myID)
                dst = rand() % thread_datas_.size();
        }
        //else self send
        new_event.data_ = dst;
        return gen_event(new_event,is_spike);
    }

    int mindelay(){return min_delay_;}
    int cells(){return cell_groups_;}
    int received(){return total_received_;}
    int relevent(){return total_relevent_;}
    void increment_time(){++time_;}

};

}
#endif
