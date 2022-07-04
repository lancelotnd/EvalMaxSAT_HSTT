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
#include "../pugixml-1.12/src/pugixml.hpp"


class Time {
    std::string name;
    std::string id;
    std::string ref_day;
    std::vector<std::string> ref_timegroups;
};


class Times {
    //We have an array of timegroups
    std::map<std::string,std::string> timegroups;
    //And an array of time.
    std::vector<Time> times;


public : Times(pugi::xml_node times_node){
            for(pugi::xml_node tg : times_node.child("TimeGroups").children()){
                std::string id = tg.attribute("Id").as_string();
                std::string name = tg.child("Name").child_value();
                addTimegroup(id, name);
            }

            for(pugi::xml_node t : times_node.child("Times").children()){
                std::cout << t.name() << std::endl;
        }
        }

    void addTime(Time t){
        times.push_back(t);
    }

    void addTimegroup(std::string id, std::string name){
        timegroups[id] = name;
    }
};