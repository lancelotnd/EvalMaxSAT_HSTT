class ConflictTree{

};

class NodeConflict {
    bool visited = false;
    int global_objective;
    int number_conflicts;
    std::string group;
    int period;
    std::string teacher;
    std::map<std::string, std::vector<int>> conflicts;
    std::map<std::string, std::map<int, std::set<std::string>>> stsdr;
    std::vector<NodeConflict*> children;
public:
    NodeConflict(std::string group, int period, std::string teacher, int global_objective
    ,     std::map<std::string, std::map<int, std::set<std::string>>> stsdr,
          std::map<std::string, std::vector<int>> conflicts){
        this->group = group;
        this->period = period;
        this->teacher = teacher;
        this->global_objective = global_objective;
        this->stsdr = stsdr;
        this->conflicts = conflicts;
    }

    void insert_child(NodeConflict* node){
        this->children.emplace_back(node);
    }

    std::string get_group(){
        return group;
    }

    int get_period(){
        return period;
    }
    std::string get_teacher(){
        return teacher;
    }
    std::map<std::string, std::map<int, std::set<std::string>>> & get_stsdr(){
        return stsdr;
    }

};