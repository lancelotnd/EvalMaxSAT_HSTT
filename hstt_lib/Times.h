/**
 * The Times section contains 4 entities:
 * TimeGroups, Weeks, Days and Times.
 * A Day or Week is simply a special TimeGroup, which can be added as a property to a Time.
 * At all other places it is referred to as TimeGroup. The reason for the existence of Days and Weeks is for displaying only.
 * In the datasets Weeks do not occur: all instances describe a weekly timetable. As can be guessed, a TimeGroup is a set of Times.
 * Times are assumed to be consecutive as given in the instance.
 */