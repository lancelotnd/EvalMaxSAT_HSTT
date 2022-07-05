#include "../pugixml-1.12/src/pugixml.hpp"
#include <vector>
#include <map>

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
    }
};

/**
 * An assign resource constraint specifies that solution resources should be assigned resources.
 * More importantly, it defines the cost of failing to make these assignments.
 */

class AssignResourceConstraint : Constraint {
    std::vector<std::string> applies_to_events;
    std::vector<std::string> applies_to_event_groups;

public: explicit AssignResourceConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * An assign time constraint specifies that solution events should be assigned times.
 * More importantly, it defines the cost of failing to make these assignments.
 */

class AssignTimeConstraint : Constraint {

public: explicit AssignTimeConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * A split events constraint places limits on the number of solution events that may be derived
 * from a given instance event, and on their durations.
 */

class SplitEventsConstraint : Constraint {

public: explicit SplitEventsConstraint(pugi::xml_node node) : Constraint(node) {

}
};

/**
 * A distribute split events constraint places limits on the number of solution events
 * of a particular duration that may be derived from a given instance event.
 */

class DistributeSplitEventsConstraint : Constraint {

public: explicit DistributeSplitEventsConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * A prefer resources constraint specifies that some resources are preferred
 * for assignment to some solution resources.
 */

class PreferResourcesConstraint : Constraint {

public: explicit PreferResourcesConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * A prefer times constraint specifies that some times
 * are preferred for assignment to some solution events.
 */

class PreferTimesConstraint : Constraint {

public: explicit PreferTimesConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * Each solution resource may be assigned only one resource,
 * which attends the enclosing solution event for its full duration.
 * However, when an instance event is split into solution events,
 * each of its event resources is split into several solution resources,
 * and a different resource may be assigned to each of these solution resources.
 */

class AvoidSplitAssignmentsConstraint : Constraint {

public: explicit AvoidSplitAssignmentsConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * A spread events constraint specifies that the solution events
 * of an event group should be spread out in time.
 */

class SpreadEventsConstraint : Constraint {

public: explicit SpreadEventsConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * A link events constraint specifies that certain events should be assigned the same times.
 */
class LinkEventsConstraint : Constraint {

public: explicit LinkEventsConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * An order events constraint specifies that the times of two events
 * should be constrained so that the first event ends before the second begins.
 */

class OrderEventsConstraint : Constraint {

public: explicit OrderEventsConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * An avoid clashes constraint specifies that certain resources should have no clashes;
 * that is, they should not attend two or more events simultaneously.
 */

class AvoidClashesConstraint : Constraint {

public: explicit AvoidClashesConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * An avoid unavailable times constraint specifies that certain resources
 * are unavailable to attend any events at certain times.
 */
class AvoidUnavailableTimesConstraint : Constraint {

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

class LimitIdleTimesConstraint : Constraint {

public: explicit LimitIdleTimesConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * A resource is busy at some time when it attends at least one solution event at that time,
 * and busy during some time group if it is busy at one or more times of that time group.
 * A cluster busy times constraint places limits on the number of time groups
 * during which a resource may be busy.
 */
class ClusterBusyTimesConstraint : Constraint {

public: explicit ClusterBusyTimesConstraint(pugi::xml_node node) : Constraint(node) {

    }
};

/**
 * A resource is busy at some time when it attends at least one solution event at that time.
 * A limit busy times constraint places limits on the number of times during
 * certain time groups that a resource may be busy.
 */

class LimitBusyTimesConstraint : Constraint {

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

class LimitWorkloadConstraint : Constraint {

public: explicit LimitWorkloadConstraint(pugi::xml_node node) : Constraint(node) {

    }
};


class Constraints {

public : Constraints(pugi::xml_node instance) {

}
};
