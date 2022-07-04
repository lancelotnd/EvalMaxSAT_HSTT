/**
* The Events section contains 3 entities:
 * EventGroups, Courses and Events. Courses are special EventGroups.
 * An Event has a property Course. At all other places Courses are referred to as EventGroup.
 * Obviously, an EventGroup is simply a set of Events.
 * Now what is an Event? This question has two answers in XHSTT.
 * The first answer is that an Event is a lesson of a fixed Duration.
 * Hence our timetabling problem is to set a begin Time to events, which implies that the Event is planned to this Time,
 * and some consecutive Times if the Duration exceeds 1.
 * Since an Event has (can have) Resources attached to it,
 * these Resources are now busy at these Times. Here we meet the 2 basic requirements in timetabling
 * , which are made explicit as constraint in XHSTT: we need to assign a (begin)Time to each Event (AssignTimeConstraint)
 * and we have to make sure that Resources are not planned double (AvoidClashesConstraint).
 * Some datasets have only these constraints, see the artificial datasets (all except the Sudoku).
 * The other interpretation of Event is slightly more complicated:
 * an Event can represent all lessons with exactly the same properties, i.e. a course.
 * These properties are for example the Resources class and teacher.
 * In this case the duration is the total Duration required for this class-teacher combination.
 * So now a part of the planning problem is to divide the "instance" Event in "solution" Events; here the "solution"
 * Events are the lessons in the first interpretation. To know which interpretation is valid, a dataset with Events of
 * Durations greater than 1 will contain one or more SplitEventsConstraints, explaining how the "instance" event can be
 * divided into "solution" events. If these constraints don't give enough details,
 * one can add DistributeSplitEventsConstraints to control the number of "solution" Events of a certain Duration.
 * An example of the first interpretation is FinlandArtificialSchool.
 * Note that SplitEventsConstraint has MinimumAmount=1 and MaximumAmount=1,
 * which means that the "instance" Event should lead to exactly 1 "solution" Event.
 * An example of the second interpretation is BrazilInstance1. Here MinimumAmount=1 and MaximumAmount=999,
 * so we can split the "instance" Event to as many "solution" Events as you wish.
 * Since the summed Durations of the "solution" Events must be the Duration of the "instance" Event
 * (otherwise the solution is marked invalid by HSEval), this total Duration is an upper bound for the number of
 * "solution" Events. Note that in BrazilInstance1 MinimumDuration=1 and MaximumDuration=2, implying that "solution"
 * Events should have Duration 1 or 2. In BrazilInstance1 there is also several DistributeSplitEventsConstraints,
 * which express that for some Events we need at least one or two lessons of Duration 2.
*/


