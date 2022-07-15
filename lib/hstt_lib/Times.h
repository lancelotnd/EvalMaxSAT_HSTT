/**
 * The Times section contains 4 entities:
 * TimeGroups, Weeks, Days and Times.
 * A Day or Week is simply a special TimeGroup, which can be added as a property to a Time.
 * At all other places it is referred to as TimeGroup. The reason for the existence of Days and Weeks is for displaying only.
 * In the datasets Weeks do not occur: all instances describe a weekly timetable. As can be guessed, a TimeGroup is a set of Times.
 * Times are assumed to be consecutive as given in the instance.
 */
#include <vector>
#include <map>
#include "../../lib/pugixml-1.12/src/pugixml.hpp"


class Time {
    std::string name;
    std::string id;
    std::string ref_day;
    std::vector<std::string> ref_timegroups;



public: Time(pugi::xml_node t){
        id = t.attribute("Id").as_string();
        name = t.child("Name").child_value();
        ref_day = t.child("Day").attribute("Reference").as_string();

        for(pugi::xml_node tg : t.child("TimeGroups").children()){
            ref_timegroups.push_back(tg.attribute("Reference").as_string());
        }
    }

    void printTime() {
        std::cout << id << " : " << name << ", " << ref_day << std::endl;
        for(auto e :ref_timegroups) {
            std::cout << "> " << e << std::endl;
        }
        std::cout << "-------------------------" << std::endl;
    }
};


class Times {
    std::map<std::string,std::string> timegroups;
    std::vector<Time> times;

public : Times(pugi::xml_node times_node){

            for(pugi::xml_node t : times_node.child("Times").children()){
                if((std::string)t.name() == "Time"){
                    addTime(t);
                } else if ( (std::string) t.name() == "TimeGroups") {
                    for(pugi::xml_node tg : t.children()){
                        addTimegroup(tg);
                    }
                }
        }
}

    void addTime(pugi::xml_node time_node){
        Time t = Time(time_node);
        t.printTime();
        times.push_back(t);
    }

    void addTimegroup(pugi::xml_node tg){
        std::string id = tg.attribute("Id").as_string();
        std::string name = tg.child("Name").child_value();
        timegroups[id] = name;
    }
};