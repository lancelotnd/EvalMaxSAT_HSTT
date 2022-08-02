/**
 * The Resources section contains 3 entities:
 * ResourceTypes, ResourceGroups and Resources.
 * A Resource has exactly one ResourceType.
 * The ResourceType is introduced for displaying (for example: distinguishes teachers from students),
 * and consistency in assigning resources to events, see the Role section below.
 * ResourceGroups are groups of Resources of the same ResourceType.
*/



#pragma once
#include <map>
#include <vector>
#include "../../lib/pugixml-1.12/src/pugixml.hpp"
#include <set>



struct ResourceGroup {
    std::string id;
    std::string name;
    std::string reference;
};

class Resource {
    std::string name;
    std::string id;
    std::string ref_res_type;
    std::vector<std::string> ref_res_group;
    std::set<std::string> associated_events;
    int total_scheduled_duration = 0;
    std::vector<int> all_durations;



public: Resource(pugi::xml_node t){
        id = t.attribute("Id").as_string();
        name = t.child("Name").child_value();
        ref_res_type = t.child("ResourceType").attribute("Reference").as_string();

        for(pugi::xml_node rg : t.child("ResourceGroups").children()){
            ref_res_group.push_back(rg.attribute("Reference").as_string());
        }
    }

    std::string getResType(){
        return ref_res_type;
    }

    Resource() {
        //Default constructor required for maps.
    }

    std::set<std::string> getClashingEvents(){
        return  associated_events;
    }

    void printAssociatedEvents(){
        std::cout << name << " " << ref_res_type << " : ";
        for (auto e: associated_events) {
            std::cout << e << ", ";
        }
        std::cout << std::endl;
    }

    void associateEvent(std::string e, int duration) {
        associated_events.insert(e);
        total_scheduled_duration += duration;
        all_durations.push_back(duration);
    }

    std::string getId(){
        return id;
    }

    int getTotalDuration(){
        return total_scheduled_duration;
    }

    std::vector<int> getDurations(){
        return all_durations;
    }

    void printResource() {
        std::cout << id << " : " << name << ", " << ref_res_type <<" Duration : " << total_scheduled_duration << " periods" << std::endl;
        for(auto e :ref_res_group) {
            std::cout << "> " << e << std::endl;
        }
    }
};

class Resources {
    std::map<std::string, std::string> resourceTypes;
    std::map<std::string, ResourceGroup> resourceGroups;
    std::map<std::string, Resource> resourceMap;
    std::map<std::string, std::vector<std::string>> resources_of_types;



public: Resources(pugi::xml_node resources_node) {
        for(pugi::xml_node r : resources_node.child("Resources").children()){
            if ((std::string) r.name() == "ResourceTypes"){
                addResourceTypes(r);
            }
            else if ((std::string) r.name() == "ResourceGroups"){
                addResourceGroups(r);
            }
            else if ((std::string) r.name() == "Resource"){
                Resource res = Resource(r);
                resourceMap[res.getId()] = res;
                resources_of_types[res.getResType()].push_back(res.getId());
            }
        }
    }

    void addResourceTypes(pugi::xml_node rt){
        for (pugi::xml_node r : rt.children()) {
            resourceTypes[r.attribute("Id").as_string()] = r.child("Name").child_value();
        }
    }


    size_t resources_types_size(){
            return resourceTypes.size();
    }
    std::map<std::string, std::string> get_resources_types(){
        return resourceTypes;
    }

    std::vector<std::set<std::string>> getAllClashingEvents(){
        std::vector<std::set<std::string>> toReturn;
        for(auto e :resourceMap){
            toReturn.push_back(e.second.getClashingEvents());
        }
        return toReturn;
    }

    std::vector<std::string> getResourcesOfType(std::string type) {
        return resources_of_types[type];
    }

    void addResourceGroups(pugi::xml_node rg){
        for (pugi::xml_node r : rg.children()) {
            ResourceGroup res_g;
            res_g.id = r.attribute("Id").as_string();
            res_g.name = r.child("Name").child_value();
            res_g.reference = r.child("ResourceType").attribute("Reference").as_string();
            resourceGroups[res_g.id] = res_g;
        }

    }



    Resource* getPrt(std::string res_ref) {
        return &resourceMap[res_ref];
    }
};

