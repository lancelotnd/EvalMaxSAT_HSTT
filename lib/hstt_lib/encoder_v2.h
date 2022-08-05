/**
* The goal of this version is to reduce drastically the number
 * of variables of the problem by essentially solving smaller sections
 * of the problem
 *
*/


/**
 * Here we consider viewing the problem with the perspective of a single resource type.
 * We take each resource of this type and count the total duration it is sollicited
 * accross all events. Then we try and allocate a schedule for the resource taking into
 * account constraints PrefferedTimes and the likes. If there are wildcards, such as a room
 * we will try and assign the same wildcard for the same resource as much as possible. Finally
 * we will put the events into the Resource's schedule.
 */

#include <vector>
#include "../../ipamir.h"
#include "../pysat/clset.hh"
#include "../pysat/cardenc/mto.hh"
#include "../../ipamir.h"
#include "print_schedule.h"
#include "solver.h"

class EncoderV2 {
    Times& t;
    Resources& r;
    Events& e;
    Constraints& c;
    int nb_clauses = 0;


public: EncoderV2(
        Times &t,
        Resources &r,
        Events &e,
        Constraints &c) : c(c), r(r), e(e), t(t){
        std::cout << "c Constructed Encoder V2" << std::endl;
    }

        void encode() {
        ClauseSet clauses;
        std::map<std::string, int> starting_index_for_event;
        std::vector<int> allTimes;
        propagate_constraints();
        std::map<std::string, std::string> allTypes = r.get_resources_types();
        for(auto &i : allTypes){
            for(auto res: r.getResourcesOfType(i.first)) {
                std::vector<int> allTimes_for_resources;
                Resource * tmp = r.getPrt(res);

                if(!tmp->getClashingEvents().empty()){
                    void * solver = ipamir_init();

                    tmp->printResource();

                    std::vector<Event*> associatedEvents = getEvents(tmp->getClashingEvents());
                    std::map<int, std::vector<int>> same_time;
                    for (auto & event: associatedEvents){
                        int frozen_top_lit = Solver::toplit;
                        std::vector<int> this_resource_times;
                        starting_index_for_event[event->getId()] = Solver::toplit+1;

                        for(int i = frozen_top_lit; i < frozen_top_lit+ t.size(); i++){
                            this_resource_times.push_back(i+1);
                            same_time[frozen_top_lit-i+1].push_back(i+1);
                            Solver::toplit = i+1;
                        }
                        allTimes_for_resources.insert(allTimes_for_resources.end(), this_resource_times.begin(), this_resource_times.end());
                        event->AssignTimes(t, this_resource_times, Solver::toplit, clauses);
                    }
                    for(auto index: same_time){
                        mto_encode_atmostN(Solver::toplit, clauses,index.second,1);
                    }


                    for(auto clau:clauses.get_clauses()){
                        for(auto l: clau){
                            ipamir_add_hard(solver,l);
                        }
                        ipamir_add_hard(solver,0);
                    }
                    clauses.clear();
                    int ret_code = ipamir_solve(solver);
                    assert(ret_code ==30);
                    if(ret_code == 30){
                        PrintSchedule printer(tmp->getId());

                        for(auto ev: associatedEvents){
                            int index_offset = ev->getIndexOffset();
                            std::vector<int> allocated_slot_id;
                            std::vector<std::string> allocated_slots;

                            for( int i = index_offset; i< index_offset+100; i++) {
                                if (ipamir_val_lit(solver, i) > 0) {
                                    int slot = ipamir_val_lit(solver, i) - (index_offset - 1);
                                    allocated_slot_id.push_back(slot-1);
                                    allocated_slots.push_back(t[slot - 1].getId());
                                }
                            }
                            printer.add_course(ev->getId(), allocated_slot_id);
                            assert(allocated_slots.size() == ev->getDuration());
                        }
                        printer.print();
                    }
                }
            }
        }
    }


        std::vector<Event*> getEvents(std::set<std::string> events){
            std::vector<Event*> to_return;
            for(auto & e_str: events){
                to_return.push_back(&e.getEvent(e_str));
            }
            return to_return;
        }

        void propagate_constraints()
        {
            for(int i =0; i < c.size(); i++) {
                Constraint * constraint = c[i].get();
                if(c[i]->getClassName() == "SplitEventConstraint") {
                    std::set<Event*> se = c[i]->getApplied(e);
                    for(auto e:se){
                        SplitEventsConstraint *s = static_cast<SplitEventsConstraint*>(constraint);
                        auto v = s->getMinMax();
                        e->addSplitConstraint(v[0], v[1], v[2], v[3]);
                    }
                } else if(c[i]->getClassName() == "PreferTimesConstraint") {
                    PreferTimesConstraint * pref_constraint = static_cast<PreferTimesConstraint*>(constraint);
                    std::set<Event*> se = pref_constraint->getApplied(e);
                    for(auto e:se) {
                        e->addTimePreference(pref_constraint->getTimes(t));
                    }
                } else if("SpreadEventConstraint"){
                    SpreadEventsConstraint * spreadConstraint = static_cast<SpreadEventsConstraint*>(constraint);

                }
            }
        }
    };




