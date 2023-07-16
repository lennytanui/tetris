#ifndef STRING_H

struct String{
    char *val;
    unsigned int length;
};

unsigned int chars_length(const char *val){
    unsigned int result = 0;
    
    // TODO: Safeguard for overflow
    while(1){
        if(val[result] == 0){
            break;
        }

        result++;
    }

    return result;
}

// TODO: optimize
int IsStringEqual(String a, String b){
    int result = 1;
    if(a.length != b.length){
        result = 0;
        return result;
    }

    for(int i = 0; i < a.length; i++){
        if(a.val[i] != b.val[i]){
            result = 0;
            break;
        }
    }

    return result;
}

int StringToInt(String a){
    int result = 0;
    result = atoi(a.val);
    return result;
}

// null terminated string
struct String Create_String(const char *val){
    struct String result = {0};

    result.length = chars_length(val);
    result.val = (char *)calloc(result.length + 1, sizeof(char));

    for(int i = 0; i < result.length; i++){
        result.val[i] = val[i];
    }

    result.val[result.length]= '\0';
    return result;
}

void AddToString(struct String *string, const char *val){
    unsigned int chars_to_add_len = chars_length(val);
    
    char *temp = string->val;

    string->val = (char *)calloc((string->length + chars_to_add_len + 1), sizeof(char));
    

    for(int i = 0; i < string->length; i++){
        string->val[i] = temp[i];
    }

    free(temp);

    int index = 0;
    for(int i = string->length; i < string->length + chars_to_add_len; i++){
        string->val[i] = val[index++];
    }

    string->val[string->length + chars_to_add_len] = '\0';
    string->length += chars_to_add_len;
}

void AddToString(struct String *string, const char val){
   AddToString(string, &val); 
}

// Adds string_two to string_two
void AddToString(struct String *string_one, struct String *string_two){
    char *temp = string_one->val;
    string_one->val = (char *)calloc(string_one->length + string_two->length, sizeof(char));

    for(int i = 0; i < string_one->length; i++){
        string_one->val[i] = temp[i];
    }
    
    int index = 0;
    for(int i = string_one->length; i < (string_one->length + string_two->length); i++){
        string_one->val[i] = string_two->val[index++];
    }

    string_one->length += string_two->length;
    free(temp);
}

// Add float to string
void AddToString(struct String *string, float val){
    char tempChar[8] = {};
    snprintf(tempChar, 8, "%f", val);
    AddToString(string, tempChar);
}

void AddToString(struct String *string, int val){
    char tempChar[8] = {};
    snprintf(tempChar, 8, "%i", val);
    AddToString(string, tempChar);
}

#define STRING_H
#endif