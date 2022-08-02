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
    std::vector<int> solution_time;

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

    Event() {
    }

    ~Event(){}

    void AssignTimes(Times &times, std::vector<int> &assignment, int& top_lit, ClauseSet &clauses){
        if(min_duration != 0 || max_duration !=0) {
            ClauseSet c;
            std::vector<int> soft_lits;
            for(auto l : assignment){
                solution_time.push_back(l);
            }

            if(!preffered_time.empty()) {
                std::cout << "This event has preffered times and " << max_amount << " slots." << std::endl;
                    for(auto &time: preffered_time){
                        int index = time->getIndex();
                        int duration = max_duration;
                        std::vector<int> event_slot;
                        for (int i = 0; i < duration; i++) {
                            event_slot.push_back(assignment[index+i-1]);
                            //std::cout << assignment[index+i-1] << " ";
                        }
                        mto_encode_atleastN(top_lit, c,event_slot,(int) event_slot.size());
                        top_lit++;
                        soft_lits.push_back(-top_lit);
                        for(auto cl : c.get_clauses()) {
                            cl.push_back(-top_lit);
                            clauses.add_clause(cl);

                        }
                        c.clear();
                    }
                kmto_encode_equalsN(top_lit,clauses,soft_lits,max_amount);
            }

        }
    }

    std::string getId() const {
        return id;
    }

    std::vector<int> getSolutionSet() const {
        return solution_time;
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

    void addTimePreference(std::set<Time*> _) {
        preffered_time = _;
    }


    void printEvent() const {
        std::cout << id << " : " << name << " ";
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