#ifndef SAVE_H
#include "str.c"
#include <fstream>

struct DataElement{
    int score;
    std::string username;
    std::string date;
    std::string time;
};


struct DataElement_return{
    int id;
    char* username;
    int score;
    char* date;
    char* time;
};

struct ReadDataResult{
    int success;
    DataElement *data;
    int data_len;
};

int DE_SetChild(DataElement *de, std::string name, std::string value){
    int result = 1;

    if(value.length() == 0 || name.length() == 0){
        result = 0;
        return result;
    }
    
    if(name == "username"){
        de->username = std::stoi(value);
    }else if(name == "date"){
        de->date = value;
    } else if(name == "time"){
        de->time = value;
    } else if(name == "score"){
        de->score = std::stoi(value);
    }else{
        result = 0;
    }

    return result;
}

ReadDataResult ReadDataFile(const char *file_path){
    ReadDataResult result = {};
    // read file
    std::ifstream data_file(file_path);
    if(data_file.is_open()){
        result.success = 1;
        int entries_count = 0;

        std::string raw_line;
        while(std::getline(data_file, raw_line)){
            // removing space from the line
            if(raw_line[0] == '{')
                entries_count++;
        }

        DataElement *de_data = (DataElement *)malloc(sizeof(DataElement) * entries_count);
        result.data = de_data;
        result.data_len = entries_count;

        data_file.clear();
        data_file.seekg(0);

        DataElement de = {};
        int index = -1;
        while(std::getline(data_file, raw_line)){
            // removing space from the line
            if(raw_line[0] == '{' && index >= 0){

                de = {};
                continue;
            }

            if(raw_line[0] == '}'){
                de_data[index++] = de;
                continue;
            }

            if(raw_line[0] == '#'){
                continue;
            }
            if(index < 0){
                index++;
            }

            if(raw_line[0] == '#')
                continue;

            // take spaces off
            int is_name = true;
            std::string name = "";
            std::string value = "";
            for(int i = 0; i < raw_line.size(); i++){
                if(raw_line[i] != ' ' && raw_line[i] != ':' && raw_line[i] != '\n'){
                    if(is_name){
                        name += raw_line[i];
                    }else{
                        value += raw_line[i];
                    }
                }

                if(raw_line[i] == ':'){
                    is_name = false;
                }   
            }

            if(name.length() == 0 || value.length() == 0){
                continue;
            }

            if(!DE_SetChild(&de, name, value)){
                printf("error parsing data element-> {%s, %s}\n", name.c_str(), value.c_str());
            }
            // printf("%s : %s\n", name.val, value.val);
        }

        data_file.close();
    }else{
        result.success = 0;
        printf("Failed opened file %s \n", file_path);
    }
    return result;
} 

void WriteDataFile(const char* file_path, DataElement *data, int data_len, int overwrite){

    std::ios_base::openmode  over_write_flags = std::ios_base::out;
    if(overwrite){
        over_write_flags = std::ofstream::trunc;
    }

    std::ofstream myfile (file_path, over_write_flags);
    if (myfile.is_open())
    {
        for(int i = 0; i < data_len; i++){
            myfile << "{\n";
            myfile << " id : " << data[i].username<< "\n";
            myfile << " score : " << data[i].score<< "\n";
            myfile << " date : " << data[i].date << "\n";
            myfile << " time : " << data[i].time << "\n";
            myfile << "}\n";
        }
        myfile.close();
    }else{
        printf("Unable to open file\n");
    } 
}

void AddToDataFile(const char* file_path, DataElement de){

    ReadDataResult rdr = ReadDataFile(file_path);
    DataElement *temp_de = rdr.data;
    rdr.data = (DataElement *)malloc(sizeof(DataElement) * (rdr.data_len + 1));
    for(int i = 0; i < rdr.data_len; i++){
        rdr.data[i] = temp_de[i];
    }

    rdr.data[rdr.data_len++] = de;
    free(temp_de);

    WriteDataFile(file_path, rdr.data, rdr.data_len, 0);
}

#define SAVE_H
#endif