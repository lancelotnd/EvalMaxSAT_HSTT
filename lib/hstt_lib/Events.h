/**
* The Events section contains 3 entities:
 * EventGroups, Courses and Events. Courses are special EventGroups.
*/


#pragma once

#include <vector>
#include <map>
#include "../../lib/pugixml-1.12/src/pugixml.hpp"
#include "Resources.h"

struct ResourceEvent {
    std::string res_ref;
    std::string role;
    std::string res_type_ref;
    Resource* res_ptr;
    bool is_wild_card = true;
};

class Event {
    int duration;
    std::string name;
    std::string id;
    std::string course_ref;
    std::vector<std::string> event_groups_ref;
    std::vector<ResourceEvent> resources;

public: Event(pugi::xml_node e, Resources &all_resources){
        id = e.attribute("Id").as_string();
        name = e.child("Name").child_value();
        duration = std::stoi(e.child("Duration").child_value());
        course_ref = e.child("Course").attribute("Reference").as_string();

        for(pugi::xml_node eg: e.child("EventGroups").children()){
            event_groups_ref.push_back(eg.attribute("Reference").as_string());
        }

        for(pugi::xml_node r: e.child("Resources").children()){
            ResourceEvent res;
            res.res_ref = r.attribute("Reference").as_string();
            if(res.res_ref != ""){
                res.is_wild_card = false;
                res.res_ptr = all_resources.getPrt(res.res_ref);
            }
            res.role = r.child("Role").child_value();
            res.res_type_ref = r.child("ResourceType").attribute("Reference").as_string();
            resources.push_back(res);
        }
    }

    Event(){
    }

    std::string getId(){
        return id;
    }

    void printEvent() {
        std::cout << id << " : " << name << std::endl;
        std::cout << "Duration : " << duration << " | Course ref : " << course_ref << std::endl;
        std::cout << "-- Resources --" << std::endl;
        for(auto r : resources) {
            std::cout << "> " << r.res_ref << " : " << r.role << " " << r.res_type_ref << std::endl;
        }
        std::cout << "-- EventGroups --" << std::endl;
        for(auto r : event_groups_ref) {
            std::cout << ">> " << r << std::endl;
        }
        std::cout << "------------------------------" << std::endl;
    }

};

class Events {
    std::map<std::string, std::string> event_groups;
    std::map<std::string, Event> map_events;
    int nb_event;


public: Events(pugi::xml_node events_node, Resources& r){
        for(pugi::xml_node e : events_node.child("Events").children()){
            if ((std::string) e.name() == "EventGroups"){
                addEventGroups(e);
            }
            else if ((std::string) e.name() == "Event"){
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


    void addEvent(pugi::xml_node event_node, Resources &r){
        Event e = Event(event_node, r);
        e.printEvent();
        map_events[e.getId()] = e;
    }

    size_t size(){
        return map_events.size();
    }
};


