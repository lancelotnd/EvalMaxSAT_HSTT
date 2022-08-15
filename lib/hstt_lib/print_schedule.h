class PrintSchedule {

    std::map<std::string, std::vector<int>> schedule;
    std::string name;
    std::vector<std::string> output;
    std::string blank_line = "";
    int max_length = 0;

public:   explicit PrintSchedule(std::string teacher_name) {
            name = teacher_name;

            }

    void add_course(std::string course_name, std::vector<int> course_schedule) {
        if(course_name.size() + 4 > max_length){
            max_length = course_name.size() + 4;
            fill_line();
        }
        schedule[course_name] = course_schedule;
    }
    void fill_line(){
        blank_line = "";
        for (int i = 0; i < max_length; i++){
            blank_line += " ";
        }
        blank_line+= "  ";
    }

    void print(){
        for(int i =0; i<100; i++){
            output.push_back(blank_line);
        }

        std::cout << "==================== " <<  name <<  "==================== " <<std::endl;
        int color = 0;
        for(auto k: schedule){
            bool isOpen = false;
            int previous = -1;
            for(auto l : k.second){

                if(previous < l-1 || l%20 ==0){
                    std::string text = num2string(l) + " "+k.first;
                    output[l] = padText(color, text);
                } else {
                    output[l] = padText(color, num2string(l) + " ");
                }
                previous= l;
            }
            color++;
        }
        std::cout << padText(-1, "Monday") << "│" <<  padText(-1, "Tuesday") << "│"<<  padText(-1, "Wednesday") << "│"<<  padText(-1, "Thursday") << "│"<<  padText(-1, "Friday") << "│" << std::endl;
        for(int r =0; r<20; r++){
            std::cout << output[r] << "│"<< output[r+20]<< "│" << output[r+40]<< "│" << output[r+60]<<"│" << output[r+80]<<"│" << std::endl;
        }


    }

    std::string num2string(int num){
        std::string to_return = "";
        to_return += std::to_string(num);
        if( to_return.size() ==1) to_return = " "+ to_return;
        if( to_return.size() ==2) to_return = " "+ to_return;
        return to_return;
    }

    std::string padText(int color, std::string text){
        std::string to_return ="";
        std::vector<std::string> color_code = {"\033[1;41m", "\033[1;42m", "\033[1;43m", "\033[1;44m", "\033[1;45m", "\033[1;46m", "\033[1;47m"};
        if(color!= -1){
            to_return =  color_code[color% color_code.size()];
        }
        std::string content = blank_line;
        for(int i = 0; i<text.size(); i++){
            content[i+1] = text[i];
        }
        to_return+=content;
        to_return+="\033[0m";
        return to_return;
    }

};