/**
* The Events section contains 3 entities:
 * EventGroups, Courses and Events. Courses are special EventGroups.
*/



#include <vector>
#include <map>
#include "../../lib/pugixml-1.12/src/pugixml.hpp"
#include "Resources.h"
#include "../pysat/clset.hh"
#include "../pysat/cardenc/mto.hh"
#include "solver.h"
#pragma once

struct ResourceEvent {
    std::string res_ref;
    std::string role;
    std::string res_type_ref;
    Resource* res_ptr;
    bool is_wild_card = true;
};

class Event {
    int min_duration = 0;
    int max_duration = 0;
    int min_amount = 0;
    int max_amount = 0;
    int duration;

    std::string name;
    std::string id;
    std::string course_ref;
    std::vector<std::string> event_groups_ref;
    std::vector<ResourceEvent> resources;
    std::set<Time*> preffered_time;
    std::map<std::string, std::pair<int ,int>> spread_time;

    std::vector<int> solution_time;
    int index_offset;

public:
    Event(pugi::xml_node e, Resources &all_resources) {
        id = e.attribute("Id").as_string();
        name = e.child("Name").child_value();
        duration = std::stoi(e.child("Duration").child_value());
        course_ref = e.child("Course").attribute("Reference").as_string();

        for (pugi::xml_node eg: e.child("EventGroups").children()) {
            event_groups_ref.push_back(eg.attribute("Reference").as_string());
        }

        for (pugi::xml_node r: e.child("Resources").children()) {
            ResourceEvent res;
            res.res_ref = r.attribute("Reference").as_string();
            if (res.res_ref != "") {
                res.is_wild_card = false;
                res.res_ptr = all_resources.getPrt(res.res_ref);
                res.res_ptr->associateEvent(id, duration);
            } else {
                res.is_wild_card = true;
            }
            res.role = r.child("Role").child_value();
            res.res_type_ref = r.child("ResourceType").attribute("Reference").as_string();
            resources.push_back(res);
        }
    }
    int getDuration(){
        return duration;
    }

    Event() {
    }

    ~Event(){}

    void AssignTimes(Times &times, std::vector<int> &assignment, int& top_lit, ClauseSet &clauses, Solver s) {
        index_offset = assignment[0];
        //First there shall be exactly *n* periods allocated where *n* is the total event duration.
        kmto_encode_equalsN(top_lit, clauses, assignment, duration);

        ClauseSet c;
        std::vector<int> soft_lits;

        bool has_split_event_constraint = !((min_duration == 0) && (max_duration == 0) && (min_amount == 0) && (max_amount == 0));
        bool has_preffered_time = !preffered_time.empty();
        bool has_spread_event_constraint = !spread_time.empty();


        if(has_spread_event_constraint){
            std::cout << name << " ";
            for(auto & sp: spread_time){
                 std::cout << " [" << sp.first << " : " << sp.second.first << " " << sp.second.second << " ]," ;
            }
            std::cout << std::endl;


            for(auto k: spread_time) {
                auto v = times.getIndexes_for_day(k.first);
                std::vector<int> constraint = getTimesFromOffset(v);
                if(max_duration!= 0){
                    kmto_encode_atmostN(top_lit, clauses, constraint, get<1>(k.second)* max_duration);
                } else {
                    std::cout << "Oups we didnt plan for that. " << std::endl;
                }
                if(min_duration != 0){
                    //s.encode_hard_at_least_n(constraint, get<0>(k.second)* min_duration);
                    kmto_encode_atleastN(top_lit, clauses, constraint, get<0>(k.second)* min_duration);
                }
            }
        }


        if(has_split_event_constraint) {
            std::cout << min_duration << " " << max_duration << " " << min_amount << " " << max_amount << std::endl;
            if(has_preffered_time){
                for(auto &time: preffered_time){
                    int index = time->getIndex();
                    int dur = max_duration;
                    std::vector<int> event_slot;
                    for (int i = 0; i < dur; i++) {
                        event_slot.push_back(assignment[index+i-1]);
                    }
                    add_padding(assignment[0], event_slot,times);
                    mto_encode_atleastN(top_lit, c,event_slot,(int) event_slot.size());
                    top_lit++;
                    soft_lits.push_back(-top_lit);
                    for(auto cl : c.get_clauses()) {
                        cl.push_back(top_lit);
                        clauses.add_clause(cl);

                    }
                    c.clear();
                }

                if(max_amount > 0) {
                    kmto_encode_equalsN(top_lit,clauses,soft_lits,max_amount);
                }

            } else{
                std::cout << id << " has split BUT NO PREFERRED TIME. ffs" << std::endl;
            }
        } else {
            if(has_preffered_time){
            } else{
                std::cout << id << " has NO SPLIT and NO PREFERRED CONSTRAINTS. WAT????" << std::endl;
            }
        }
    }




    std::string getId() const {
        return id;
    }

    int getIndexOffset()  {
        return index_offset;
    }

    std::vector<std::string> getEventGroups() {
        return event_groups_ref;
    }

    void addSplitConstraint(int md, int xd, int ma, int xa)
    {
        min_duration = md;
        max_duration = xd;
        min_amount   = ma;
        max_amount   = xa;
    }

    void addSpreadEventConstraint(std::map<std::string, std::pair<int ,int>> timegroups){
        spread_time = timegroups;
    }

    void addTimePreference(std::set<Time*> _) {
        preffered_time = _;
    }

    std::vector<int> getTimesFromOffset(std::vector<int> pure_time){
        std::vector<int> to_return;
        int offset = index_offset-1;
        for(auto time :pure_time) {
           to_return.push_back(time+offset);
       }
        return to_return;
    }


    void printEvent() const {
        std::cout << id << " : ";
        for(auto r : resources) {
            std::cout << "> " << r.res_ref << " " << r.res_type_ref << " ";
        }
    }

    /**
     * returns all the resource types of this event that need to be assigned
     * by the solver.
     */
    std::vector<std::string> getWildCards(){
        std::vector<std::string> toReturn;
        for(auto r: resources){
            if(r.is_wild_card){
                toReturn.push_back(r.res_type_ref);
            }
        }
        return toReturn;
    }

private:
    /**
     * This function ensures that a sub event has no other sub event
     * immediatly after or before as it would only be one big subevent.
     */

    void add_padding(int index_offset, std::vector<int>& times, Times &t){
        int offset = index_offset -1;

        int start = times[0];
        int end = times[times.size()-1];
        int before_padding = start-1;
        int after_padding = end+1;

        if(before_padding-offset > 0){
            if(t[before_padding-offset-1].getDay() ==
                t[start-offset-1].getDay()){
                times.push_back(-before_padding);
            }
        }
        if(after_padding-offset <= t.size()){
            if(t[after_padding-offset-1].getDay() ==
            t[end-offset-1].getDay()) {
                times.push_back(-after_padding);
            }
        }
    }

};

class Events {
    std::map<std::string, std::string> event_groups;
    std::map<std::string, std::vector<Event*>> event_of_groups;
    std::map<std::string, Event> map_events;
    std::vector<std::string> map_keys;
    int nb_event;


public: Events(pugi::xml_node events_node, Resources& r) {
        for(pugi::xml_node e : events_node.child("Events").children()) {
            if ((std::string) e.name() == "EventGroups") {
                addEventGroups(e);
            }
            else if ((std::string) e.name() == "Event") {
                addEvent(e, r);
            }

        }
    }

    void addEventGroups(pugi::xml_node groups){
        for(pugi::xml_node g:groups.children()){
            std::string id = g.attribute("Id").as_string();
            std::string name = g.child("Name").child_value();
            event_groups[id] = name;
        }
    }

    void addEvent(pugi::xml_node event_node, Resources &r)  {
        Event e = Event(event_node, r);
        map_events[e.getId()] = e;
        map_keys.push_back(e.getId());
        auto v = e.getEventGroups();
        for (const auto & g:v){
            event_of_groups[g].push_back(&map_events[e.getId()]);
        }
    }



    size_t size(){
        return map_events.size();
    }

    Event & getEvent(std::string key){
        return map_events[key];
    }

    std::vector<Event*> getEventGroups(std::string group){
        return event_of_groups[group];
    }

    Event & operator [](int i) {return map_events[map_keys[i]];}
};