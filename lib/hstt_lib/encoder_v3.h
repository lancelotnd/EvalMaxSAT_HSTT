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
#include <memory>

class EncoderV3 {
    Times& t;
    Resources& r;
    Events& e;
    Constraints& c;
    int nb_clauses = 0;
    std::map<std::string, std::map<int, std::set<std::string>>> SameTimeSameDeptRes;
    std::vector<std::shared_ptr<Solver>> all_solvers;
    std::map<std::string, std::shared_ptr<Solver>> map_solvers;



public: EncoderV3(
        Times &t,
        Resources &r,
        Events &e,
        Constraints &c) : c(c), r(r), e(e), t(t){
        std::cout << "c Constructed Encoder V3" << std::endl;
    }

        void encode() {

        std::map<std::string, int> starting_index_for_event;
        std::vector<int> allTimes;
        propagate_constraints();
        std::map<std::string, std::string> allTypes = r.get_resources_types();
        for(auto &i : allTypes){
            for(auto res: r.getResourcesOfType(i.first)) {
                std::vector<int> allTimes_for_resources;
                Resource * tmp = r.getPrt(res);

                if(!tmp->getClashingEvents().empty()){
                    tmp->printResource();

                    map_solvers[tmp->getId()] =std::make_shared<Solver>();
                    std::shared_ptr<Solver> s = map_solvers[tmp->getId()];
                    s->setResource(tmp->getId());
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
                        event->AssignTimes(t, this_resource_times, Solver::toplit, s->getClauseSet(), s, all_solvers.size()-1);
                    }
                    for(auto index: same_time){
                        mto_encode_atmostN(Solver::toplit, s->getClauseSet(),index.second,1);
                    }


                    bool ret_code = s->solve();
                    assert(ret_code);
                    if(ret_code){
                        PrintSchedule printer(tmp->getId());

                        for(auto ev: associatedEvents){

                           int index_offset = ev->getIndexOffset();
                            std::vector<int> allocated_slot_id;
                            std::vector<std::string> allocated_slots;

                            for( int i = index_offset; i< index_offset+100; i++) {
                                if (s->get_val_lit(i) > 0) {
                                    int slot = s->get_val_lit(i) - (index_offset - 1);
                                    if(ev->getPrefferedRes() != "") {
                                        SameTimeSameDeptRes[ev->getPrefferedRes()][slot-1].insert(ev->getId());
                                    }
                                    allocated_slot_id.push_back(slot-1);
                                    allocated_slots.push_back(t[slot - 1].getId());
                                }
                            }
                            printer.add_course(ev->getId(), allocated_slot_id);
                            assert(allocated_slot_id.size() == ev->getDuration());
                        }
                        printer.print();
                    }
                }
            }
        }
        printSameTimeRes();
    }

    /**
     * Structure de données qui énumère {
     *
     */


    void printSameTimeRes(){

        std::set<std::string> test = {"un","deux"};
        test.erase("deux");

        assert(test.size() == 1);
        std::vector<std::string> color_code = {"\033[1;41m", "\033[1;42m", "\033[1;43m", "\033[1;44m", "\033[1;45m", "\033[1;46m", "\033[1;47m"};
        for(auto z:SameTimeSameDeptRes){
            bool conflict = true;
            int nb_periods = 0;
            while(conflict) {
                std::string to_print = "";
                for(auto &y: z.second){
                    if(!y.second.empty()){
                        if((int) r.getSizeOfGroup(z.first) >= (int) y.second.size()){
                            conflict = false;
                        } else {
                            nb_periods++;
                            conflict = true;
                            to_print += color_code[0] + "Period " + std::to_string(y.first)
                                        + "\033[0m is overbooked. NB EVENTS = "
                                        + std::to_string(y.second.size()) + " CAPACITY : "
                                        + std::to_string((int) r.getSizeOfGroup(z.first))
                                        + "\nWe need to reschedule "
                                        + std::to_string((int) y.second.size() -
                                                         (int) r.getSizeOfGroup(z.first))
                                        +" events\n";
                            std::cout << to_print;
                            bool result = false;
                            std::string event = *next(y.second.begin(),0);
                            std::cout << "Attempting to reschedule " << event << std::endl;
                            result = resolveOverBooking(event,y.first);

                            if(!result){
                                std::cout << "All of our attempts failed to reschedule any of the conflicting events" << std::endl;
                            } else {
                                std::cout << "Succesfully updated schedule" << std::endl;
                            }
                            if(conflict) break;

                        }
                    }
                    if(conflict) break;
                }
                if(conflict) {
                    std::cout << color_code[0] << z.first << " are overbooked "  << std::endl;
                    std::cout << to_print;
                } else {
                    std::cout << color_code[1]
                              << z.first << " can accomodate the current schedule without conflicts\033[0m"
                              << std::endl << std::endl;
                }
            }

            }

    }

    /**
     * Here we try to eliminate the overbooking in a given departement by taking out an event at that
     * particular slot time. For that we first need to gather the resource (teacher) and remove all
     * assignments of schedule made to correctly update the SameTimeSameDept map. Then we solve again,
     * with an assume false for the period in the schedule subject to overcrowding.
     *
     * @param event
     */

    bool resolveOverBooking(std::string event, int period_to_unschedule){
        Event & currentEvent = e.getEvent(event);
        std::string res = currentEvent.getResourceId();
        Resource * resource  = r.getPrt(res);

        std::cout << resource->getId() << " " << map_solvers[res]->getResource() << std::endl;
        assert(resource->getId() == map_solvers[res]->getResource());
        std::vector<Event*> associatedEvents = getEvents(resource->getClashingEvents());
        int block = currentEvent.getIndexOffset() + period_to_unschedule;
        std::cout << "Blocked "<< block << std::endl;
        map_solvers[res]->assume(-block);

        for(auto &_ :associatedEvents){
            if(_->getPrefferedRes()!= ""){
                auto pref = _->getPrefferedRes();
                int offset_index = _->getIndexOffset();
                auto assigned_slots = getAssignedPeriods(offset_index, map_solvers[res], "");
                std::cout <<"Before deletion ";
                for (auto v: SameTimeSameDeptRes[pref][period_to_unschedule] ){
                    std::cout << v << " ";
                }_->getId();
                for(auto &l : assigned_slots){
                    SameTimeSameDeptRes[pref][l].erase(_->getId());
                }
                std::cout << "After deletion : ";
                for (auto v: SameTimeSameDeptRes[pref][period_to_unschedule]){
                    std::cout << v << " ";
                }
                std::cout << std::endl;

            }
        }
        bool result = map_solvers[res]->solve();
        if(result){
            PrintSchedule printer(res);
            for(auto &re: associatedEvents){
                int offset_index = re->getIndexOffset();
                std::string pref = re->getPrefferedRes();
                auto assigned_slots = getAssignedPeriods(offset_index, map_solvers[res],  re->getId());
                std::cout << re->getId() << " ";
                printer.add_course(re->getId(), assigned_slots);
                for(auto &l : assigned_slots){
                    //std::cout << l << " ";
                    //SameTimeSameDeptRes[pref][l].insert(re->getId());
                }
                std::cout << std::endl;
            }
            printer.print();
        } else {
        }
        return result;
    }

    std::vector<int> getAssignedPeriods(int index_offset, std::shared_ptr<Solver> solver, std::string ev){
        std::vector<int> to_return;
        for(int i = index_offset; i < index_offset+100; i++){
            if(solver->get_val_lit(i)> 0) {
                int slot = (solver->get_val_lit(i) - (index_offset - 1))-1;
                to_return.emplace_back(slot);
            }
        }
        return to_return;
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
                        SplitEventsConstraint *s = dynamic_cast<SplitEventsConstraint*>(constraint);
                        auto v = s->getMinMax();
                        e->addSplitConstraint(v[0], v[1], v[2], v[3]);
                    }
                } else if(c[i]->getClassName() == "PreferTimesConstraint") {
                    PreferTimesConstraint * pref_constraint = dynamic_cast<PreferTimesConstraint*>(constraint);
                    std::set<Event*> se = pref_constraint->getApplied(e);
                    for(auto e:se) {
                        e->addTimePreference(pref_constraint->getTimes(t));
                    }
                } else if(c[i]->getClassName() =="SpreadEventsConstraint"){
                    SpreadEventsConstraint * spreadConstraint = dynamic_cast<SpreadEventsConstraint*>(constraint);
                    std::set<Event*> spread = spreadConstraint->getApplied(e);
                    if(spread.size() != 1){
                        assert(!"TODO : Implement case where the spread is across multiple events.");
                    } else {
                        for(auto e:spread) {
                            e->addSpreadEventConstraint(spreadConstraint->getMinMaxTimes());
                        }
                    }
                } else if(c[i]->getClassName() == "PreferResourcesConstraint"){
                    PreferResourcesConstraint * prefResource = dynamic_cast<PreferResourcesConstraint*>(constraint);
                    std::set<Event*> applied_events = prefResource->getApplied(e);
                    std::vector<std::string> resource_groups = prefResource->getResourceGroups();
                    if(resource_groups.empty()){
                        std::cout << "TODO" << std::endl;
                        //assert(!"TODO: Implement preference for a single resource instead of a group");
                    }
                    else if(resource_groups.size() > 1){
                        assert(!"TODO: Implement preferences for multiple resource groups.");
                    }else {
                        for(auto ev: applied_events){
                            ev->addPreferResourceConstraint(resource_groups[0]);
                        }
                    }
                }
            }
        }
    };




