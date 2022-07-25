#include "../../lib/pugixml-1.12/src/pugixml.hpp"
#include <vector>
#include <map>
#include <memory>

enum CostFunction {Linear, Quadratic, Step};

class Constraint {

std::map<std::string, bool> bool_map = {{"true",true}, {"false",false}};
std::map<std::string, CostFunction> cost_map = {{"Linear",Linear}, {"Quadratic",Quadratic}, {"Step", Step}};

protected:
    std::string name;
    std::string id;
    int weight;
    CostFunction cost_function;
    bool required;

public: Constraint(pugi::xml_node node){
        id = node.attribute("Id").as_string();
        name = node.child("Name").child_value();
        weight = std::stoi(node.child("Weight").child_value());
        required = bool_map[node.child("Required").child_value()];
        cost_function = cost_map[node.child("CostFunction").child_value()];
        std::cout << "Constructed " << name << " constraint" << std::endl;
    }

    virtual ~Constraint() = default;
};

/**
 * An assign resource constraint specifies that solution resources should be assigned resources.
 * More importantly, it defines the cost of failing to make these assignments.
 */

class AssignResourceConstraint : public Constraint {
    std::vector<std::string> applies_to_events;
    std::vector<std::string> applies_to_event_groups;

public: explicit AssignResourceConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * An assign time constraint specifies that solution events should be assigned times.
 * More importantly, it defines the cost of failing to make these assignments.
 */

class AssignTimeConstraint : public Constraint {

public: explicit AssignTimeConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * A split events constraint places limits on the number of solution events that may be derived
 * from a given instance event, and on their durations.
 */

class SplitEventsConstraint : public Constraint {

public: explicit SplitEventsConstraint(pugi::xml_node node) : Constraint(node) {
}
};

/**
 * A distribute split events constraint places limits on the number of solution events
 * of a particular duration that may be derived from a given instance event.
 */

class DistributeSplitEventsConstraint : public Constraint {

public: explicit DistributeSplitEventsConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * A prefer resources constraint specifies that some resources are preferred
 * for assignment to some solution resources.
 */

class PreferResourcesConstraint : public Constraint {

public: explicit PreferResourcesConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * A prefer times constraint specifies that some times
 * are preferred for assignment to some solution events.
 */

class PreferTimesConstraint : public Constraint {
    std::vector<std::string> applies;
    std::vector<std::string> times_ref;
    std::vector<std::string> time_groups_ref;

public: explicit PreferTimesConstraint(pugi::xml_node node) : Constraint(node) {
        for(pugi::xml_node rg : node.child("AppliesTo").children()){
            for (pugi::xml_node event : rg.children()){
                applies.push_back(event.attribute("Reference").as_string());
            }
        }

        for(pugi::xml_node time: node.child("Times").children()){
            times_ref.push_back(time.attribute("Reference").as_string());
        }
        for(pugi::xml_node time: node.child("TimeGroups").children()){
            time_groups_ref.push_back(time.attribute("Reference").as_string());
        }
        std::cout << ">> Applies to : ";
        for (auto e:applies){
            std::cout << e << " ";
        }
        std::cout<< std::endl;

        std::cout << ">> Preferred times : ";

        for (auto e:times_ref){
            std::cout << e << " ";
        }
        std::cout<< std::endl;

        std::cout << ">> Preferred timeGroups : ";

        for (auto e:time_groups_ref){
            std::cout << e << " ";
        }
        std::cout<< std::endl;

    }
};

/**
 * Each solution resource may be assigned only one resource,
 * which attends the enclosing solution event for its full duration.
 * However, when an instance event is split into solution events,
 * each of its event resources is split into several solution resources,
 * and a different resource may be assigned to each of these solution resources.
 */

class AvoidSplitAssignmentsConstraint : public Constraint {

public: explicit AvoidSplitAssignmentsConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * A spread events constraint specifies that the solution events
 * of an event group should be spread out in time.
 */

class SpreadEventsConstraint : public Constraint {

public: explicit SpreadEventsConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * A link events constraint specifies that certain events should be assigned the same times.
 */
class LinkEventsConstraint : public Constraint {

public: explicit LinkEventsConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * An order events constraint specifies that the times of two events
 * should be constrained so that the first event ends before the second begins.
 */

class OrderEventsConstraint : public Constraint {

public: explicit OrderEventsConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * An avoid clashes constraint specifies that certain resources should have no clashes;
 * that is, they should not attend two or more events simultaneously.
 */

class AvoidClashesConstraint : public Constraint {

public: explicit AvoidClashesConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * An avoid unavailable times constraint specifies that certain resources
     * are unavailable to attend any events at certain9 times.
 */
class AvoidUnavailableTimesConstraint : public Constraint {

public: explicit AvoidUnavailableTimesConstraint(pugi::xml_node node) : Constraint(node) {

    }
};


/**
 * A resource is busy at some time when it attends at least one solution event at that time.
 * A resource is idle at some time (more precisely, idle with respect to a given time group)
 * when it is not busy at that time but it is busy at some earlier time of the time group,
 * and at some later time, taking the time group's times in the order in which
 * they are defined in the file (chronological order).
 *
 * For example, suppose the time group is the Wednesday times,
 * and the resource is busy at the third and seventh times of that time group,
 * and not busy at its other times. Then it is idle at the fourth, fifth, and sixth times
 * of that time group, but not at its first or second times or after the seventh time.
 *
 * A limit idle times constraint places limits on the number of times that resources may be idle.
 *
 */

class LimitIdleTimesConstraint : public Constraint {

public: explicit LimitIdleTimesConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * A resource is busy at some time when it attends at least one solution event at that time,
 * and busy during some time group if it is busy at one or more times of that time group.
 * A cluster busy times constraint places limits on the number of time groups
 * during which a resource may be busy.
 */
class ClusterBusyTimesConstraint : public Constraint {

public: explicit ClusterBusyTimesConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * A resource is busy at some time when it attends at least one solution event at that time.
 * A limit busy times constraint places limits on the number of times during
 * certain time groups that a resource may be busy.
 */

class LimitBusyTimesConstraint : public Constraint {

public: explicit LimitBusyTimesConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * The workload of an instance event is the value of its Workload child category if it has one,
 * or its duration if not. The workload of an event resource is the value of
 * its Workload child category if it has one, or the workload of the enclosing instance event
 * if not. It is an integer. The workload of a solution resource *sr* lying in solution event *se*
 * and derived from event resource *er* of event *e* is defined by the formula.
 *
 * Workload(sr) = Duration(se) * Workload(er) / Duration(e)
 *
 * where Duration(se) is the duration of se, Workload(er) is the workload of er, and Duration(e) is the duration of e.
 * This value is a floating-point number.
 *
 * A limit workload constraint places limits on the total workload of solution
 * resources that certain resources are assigned to.
 *
 */

class LimitWorkloadConstraint : public Constraint {

public: explicit LimitWorkloadConstraint(pugi::xml_node node) : Constraint(node) {

    }
};


class Constraints {
    std::vector<std::unique_ptr<Constraint>> constraints;

public : Constraints(pugi::xml_node instance) {
    for(pugi::xml_node c : instance.child("Constraints").children()){
        constraints.push_back(parseConstraint(c));
    }
}

std::unique_ptr<Constraint> parseConstraint(pugi::xml_node c){
    std::string c_name = c.name();
    std::unique_ptr<Constraint> to_return;
    if(c_name == "AssignResourceConstraint")
        to_return = std::unique_ptr<Constraint>(new AssignResourceConstraint(c));
    if(c_name == "AssignTimeConstraint")
        to_return = std::unique_ptr<Constraint>(new AssignTimeConstraint(c));
    if(c_name == "SplitEventsConstraint")
        to_return = std::unique_ptr<Constraint>(new SplitEventsConstraint(c));
    if(c_name == "DistributeSplitEventsConstraint")
        to_return = std::unique_ptr<Constraint>(new DistributeSplitEventsConstraint(c));
    if(c_name == "PreferResourcesConstraint")
        to_return = std::unique_ptr<Constraint>(new PreferResourcesConstraint(c));
    if(c_name == "PreferTimesConstraint")
        to_return = std::unique_ptr<Constraint>(new PreferTimesConstraint(c));
    if(c_name == "AvoidSplitAssignmentsConstraint")
       to_return = std::unique_ptr<Constraint>(new AvoidSplitAssignmentsConstraint(c));
    if(c_name == "SpreadEventsConstraint")
        to_return = std::unique_ptr<Constraint>(new SpreadEventsConstraint(c));
    if(c_name == "LinkEventsConstraint")
        to_return = std::unique_ptr<Constraint>(new LinkEventsConstraint(c));
    if(c_name == "OrderEventsConstraint")
        to_return = std::unique_ptr<Constraint>(new OrderEventsConstraint(c));
    if(c_name == "AvoidClashesConstraint")
        to_return = std::unique_ptr<Constraint>(new AvoidClashesConstraint(c));
    if(c_name == "AvoidUnavailableTimesConstraint")
        to_return = std::unique_ptr<Constraint>(new AvoidUnavailableTimesConstraint(c));
    if(c_name == "LimitIdleTimesConstraint")
        to_return = std::unique_ptr<Constraint>(new LimitIdleTimesConstraint(c));
    if(c_name == "ClusterBusyTimesConstraint")
        to_return = std::unique_ptr<Constraint>(new ClusterBusyTimesConstraint(c));
    if(c_name == "LimitBusyTimesConstraint")
        to_return = std::unique_ptr<Constraint>(new LimitBusyTimesConstraint(c));
    if(c_name == "LimitWorkloadConstraint")
        to_return = std::unique_ptr<Constraint>(new LimitWorkloadConstraint(c));
    return to_return;
}


};
