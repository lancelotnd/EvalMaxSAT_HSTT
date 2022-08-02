class PrintSchedule {

    std::map<std::string, std::vector<int>> schedule;
    std::string name;

public:   explicit PrintSchedule(std::string teacher_name) {
            name = teacher_name;
            }

    void add_course(std::string course_name, std::vector<int> course_schedule) {
        schedule[course_name] = course_schedule;
    }

    void print(){
        std::cout << "==================== " <<  name <<  "==================== " <<std::endl;
    }
};