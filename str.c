#ifndef STRING_H

struct String{
    char *val;
    unsigned int length;
    char availableSpace;
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
    int bytesToAlloc =  (int)ceil((result.length / 32.0f)) * 32 + 32;
    result.availableSpace = bytesToAlloc;

    result.val = (char *)calloc(bytesToAlloc, sizeof(char));

    for(int i = 0; i < result.length; i++){
        result.val[i] = val[i];
    }

    result.availableSpace -= result.length;

    result.val[result.length]= '\0';
    return result;
}

void AddToString(struct String *string, const char *val){
    unsigned int chars_to_add_len = chars_length(val);
    
    if((chars_to_add_len + 1) > string->availableSpace){
        int bytesToAlloc =  0;
        char *temp = string->val;
        
        bytesToAlloc = (int)ceil((string->length + chars_to_add_len) / 32.0f) * 32 + 32;
        string->val = (char *)calloc(bytesToAlloc, sizeof(char));

        for(int i = 0; i < string->length; i++){
            string->val[i] = temp[i];
        }

        free(temp);
    }

    int index = 0;
    for(int i = string->length; i < string->length + chars_to_add_len; i++){
        string->val[i] = val[index++];
    }

    string->val[string->length + chars_to_add_len] = '\0';
    string->length += chars_to_add_len;
    string->availableSpace -= string->length;
}

void AddToString(struct String *string, int index, const char *val){
    unsigned int chars_to_add_len = chars_length(val);
    
    if((chars_to_add_len + 1) > string->availableSpace){
        int bytesToAlloc = 0;
        char *temp = string->val;
        
        bytesToAlloc = (int)ceil((string->length + chars_to_add_len) / 32.0f) * 32 + 32;
        string->val = (char *)calloc(bytesToAlloc, sizeof(char));

        for(int i = 0; i < string->length; i++){
            string->val[i] = temp[i];
        }

        free(temp);
    }

    // shift buffer right from index
    int newLength = string->length + chars_to_add_len; 
    int gap = chars_to_add_len;
    for(int i = newLength; i > index; i--){
        string->val[i] = string->val[newLength - (gap++)];
    }

    int j = 0;
    for(int i = index; i < (index + chars_to_add_len); i++){
        string->val[i] = val[j++];
    }

    string->val[string->length + chars_to_add_len] = '\0';
    string->length += chars_to_add_len;
    string->availableSpace -= string->length;
}

void AddToString(struct String *string, const char val){
    const char _val[2] = {val, '\0'}; // Null terminated strings required
    AddToString(string, _val); 
}

void AddToString(struct String *string, int index, const char val){
    const char _val[2] = {val, '\0'}; // Null terminated strings required
    AddToString(string, index, _val); 
}

// Adds string_two to string_two
void AddToString(struct String *string_one, struct String *string_two){
    printf("\nCan't add two strings together for now.....\n\n");
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

void RemoveCharInString(struct String *string, int index){
    if((index > string->length) || (index < 0) || !string->val)
        return;

    if(index == 23){
        int x = 0;    
    }

    if(string->length > 1){
        for(int i = index; i < string->length; i++){
            string->val[i] = string->val[i + 1];
        }
        string->length--;

        // TODO: reduced allocated space if more than 32 bytes free.
    } else {
        string->length = 0;
    }
}

#define STRING_H
#endif