/**
* This class shall combine all solvers and keep track of
 * conflicts and overall cost of the solutions found.
 * It is also responsible to optimise the cost of the overall
 * solution.
*/

#include "solver.h"
#include "Events.h"
#include "Resources.h"
#include "conflict_tree.h"

class MetaSolver {
    Resources& r;
    Events& e;

    std::map<std::string, std::map<int, std::set<std::string>>> SameTimeSameDeptRes;
    std::map<std::string, int> groups_capacity;
    int number_of_conflicts = 0;
    std::map<std::string, std::vector<int>> conflicts;
    std::vector<std::shared_ptr<Solver>> all_solvers;
    std::map<std::string, std::shared_ptr<Solver>> map_solvers;
    int global_objective = 0;


public:


    MetaSolver(Events& e, Resources& r): e(e), r(r){

    }

    MetaSolver& operator=(const MetaSolver& other)
    {
        if(this == &other) return *this;
        r = other.r;
        e = other.e;
        SameTimeSameDeptRes = other.SameTimeSameDeptRes;
        groups_capacity = other.groups_capacity;
        number_of_conflicts = other.number_of_conflicts;
        conflicts = other.conflicts;
        all_solvers = other.all_solvers;
        map_solvers = other.map_solvers;
        global_objective = other.global_objective;
        return *this;
    }

    void add_solver(std::string solver_id){
        map_solvers[solver_id] = std::make_shared<Solver>();
    }

    bool solve(std::string solver){
        bool is_sat = map_solvers[solver]->solve();
        if(is_sat){
            global_objective += map_solvers[solver]->get_objective();
            std::cout << "Global objective " << global_objective << std::endl;
        }

        return is_sat;
    }

    void output_stats(){
        for(auto group : SameTimeSameDeptRes){
                int capacity = groups_capacity[group.first];
                for(auto period: group.second){
                    if(period.second.size() > capacity){
                        number_of_conflicts += period.second.size() - capacity;
                        conflicts[group.first].emplace_back(period.first);
                    }
                }
        }
        std::cout << "Number of conflicts : " << number_of_conflicts << std::endl;
    }

    void add_group_capacity(std::string group, int capacity){
        groups_capacity[group] = capacity;
    }

    std::shared_ptr<Solver> operator [](std::string s) {return map_solvers[s];}

    std::map<std::string, std::map<int, std::set<std::string>>> & stsdr(){
        return SameTimeSameDeptRes;
    }

    bool resolveOverBooking(std::string event, int period_to_unschedule, MetaSolver &meta_solver){
        Event & currentEvent = e.getEvent(event);
        std::string teacher_id = currentEvent.getResourceId();

        //Replace stsdr by node map.
        erase_teacher_schedule(teacher_id, SameTimeSameDeptRes);
        block_conflict(teacher_id, event, period_to_unschedule);

        //Pas oublier de supprimer le cout de ce solver du global objective avant de faire
        //un nouveau solve.
        bool result = meta_solver[teacher_id]->solve();
        if(result){
            update_teacher_schedule(teacher_id, SameTimeSameDeptRes);
        }
        return result;
    }

    void block_conflict(std::string teacher_id, std::string event_id, int period_to_unschedule){
        Event & currentEvent = e.getEvent(event_id);
        int block = currentEvent.getIndexOffset() + period_to_unschedule;
        map_solvers[teacher_id]->assume(-block);

    }

    std::string get_event_id(int period, std::string teacher){
        std::string event_id = "";
        Resource* t = r.getPrt(teacher);
        auto associated_events = getEvents(t->getClashingEvents());
        for(auto &e :associated_events){
            int index_offset = e->getIndexOffset();
        }
    }


    std::vector<int> getAssignedPeriods(int index_offset, std::shared_ptr<Solver> solver){
        std::vector<int> to_return;
        for(int i = index_offset; i < index_offset+100; i++){
            if(solver->get_val_lit(i)> 0) {
                int slot = (solver->get_val_lit(i) - (index_offset - 1))-1;
                to_return.emplace_back(slot);
            }
        }
        return to_return;
    }

    void walk_tree(){
        NodeConflict root("", 0, "", global_objective, SameTimeSameDeptRes,conflicts);
        add_children(&root,conflicts,SameTimeSameDeptRes, global_objective);

    }

    void clear_all_assumptions()
    {
        for(auto &k : map_solvers) {
            k.second->clear_assumptions();
        }
    }


    void add_children(NodeConflict *root,  std::map<std::string, std::vector<int>> &map_conflicts,
                      std::map<std::string, std::map<int, std::set<std::string>>> &stsdr,
                      int &global_obj)
   {
        for(auto &group: map_conflicts){
            std::string group_name = group.first;
            for(auto &period : group.second){
                for(auto &teacher : stsdr[group_name][period]){
                    root->insert_child(new NodeConflict(group_name, period,teacher,global_objective,stsdr, map_conflicts));
                }
            }
        }
    }

    void solve_node(NodeConflict* node){
        std::string teacher_id = node->get_teacher();
        int period = node->get_period();
        erase_teacher_schedule(teacher_id, node->get_stsdr());

    }

    /**
     * Remove all assigned schedule of a given teacher form the Stsdr map.
     * @param teacher
     */
    void erase_teacher_schedule(std::string teacher_id, std::map<std::string, std::map<int, std::set<std::string>>> &stsdr){
        Resource * resource  = r.getPrt(teacher_id);
        std::vector<Event*> associatedEvents = getEvents(resource->getClashingEvents());

        for(auto &event :associatedEvents){
            if(! event->getPrefferedRes().empty()){
                auto pref = event->getPrefferedRes();
                int offset_index = event->getIndexOffset();
                auto assigned_slots = getAssignedPeriods(offset_index, map_solvers[teacher_id]);
                for(auto &l : assigned_slots){
                    stsdr[pref][l].erase(event->getId());
                }
            }
        }
    }


    /**
     * Reinserts the teacher schedule into the map stsdr.
     * @param teacher_id
     */
    void update_teacher_schedule(std::string teacher_id,  std::map<std::string, std::map<int, std::set<std::string>>> &stsdr){
        Resource * resource  = r.getPrt(teacher_id);
        std::vector<Event*> associatedEvents = getEvents(resource->getClashingEvents());

        for(auto &event: associatedEvents){
            int offset_index = event->getIndexOffset();
            std::string pref = event->getPrefferedRes();
            auto assigned_slots = getAssignedPeriods(offset_index, map_solvers[teacher_id]);
            for(auto &l : assigned_slots){
                stsdr[pref][l].insert(event->getId());
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

};