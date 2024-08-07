
namespace Notes{

TextRendererManager trm;

struct State{
    int x;
    int currentLine;
};

String testString;
Render_Square *testSquare;
Render_Square *cursorSquare;
v2 cursorPos = {0.0f, 0.0f};
v2 cursorDim = {15.0f, 50.0f};
RGBA cursorColor = {150.0f, 20.0f, 15.0f, 255.0f};

v2 testSquarePos = {0.0f, 0.0f};
v2 testSquareDim = {PROJ_RIGHT, PROJ_TOP};
RGBA testSquareClr = {60.0f, 60.0f, 60.0f, 255.0f};
RGBA testSquareBrdClr = testSquareClr;

Text textBox= {0};
int currentRow = 0;

bool recievingTextInput = true;

State state = {0};

int GetCursorIndex(Text *textBox){
    int cursorColumn = textBox->cursorColumn;
    int cursorRow = textBox->cursorRow;
    int index = textBox->string.length;
    if(cursorRow < textBox->rowsCount){
        for(int i = cursorRow; i <= textBox->rowsCount; i++){
            index -= GetRowCharCount(textBox, i);
        }
        index += cursorColumn;   
    }else{
        index -= GetRowCharCount(textBox, cursorRow);
        index += cursorColumn - 1; 
    }
    char left_c = textBox->string.val[index - 1];
    char right_c = textBox->string.val[index];
    Character *left_char = &trm.cts[0].characters[left_c];
    Character *right_char = &trm.cts[0].characters[right_c];

    return index;
}

// codepoint is UTF-32
void ReceiveCharacter(unsigned int codepoint){

    if(recievingTextInput){
        char val = (char)codepoint;
        int index = GetCursorIndex(&textBox);

        AddToString(&textBox.string, (index + 1), val);
        textBox.cursorColumn++;

        // Move Cursor Position
        Character *c = &trm.cts[0].characters[val];
        // Note (Lenny) : Might have to take scale account in the future
        cursorPos.x += c->advance >> 6; 
        IncreaseRowWidthCharCount(&textBox, currentRow, (c->advance >> 6), 1);
    }
}

void NotesKeyPress(int key, int action){
    if(key == GLFW_KEY_BACKSPACE && textBox.string.length > 0){
        
        char charToRemove = textBox.string.val[textBox.string.length - 1];
        // Move Cursor Position
        Character *character = &trm.cts[0].characters[charToRemove];
        // Note (Lenny) : Might have to take scale account in the future
        cursorPos.x -= character->advance >> 6; 

        DecreaseRowWidthCharCount(&textBox, currentRow, (character->advance >> 6), 1);
       
        if(charToRemove == '\n'){
            cursorPos.y += 48;
            cursorPos.x = textBox.left;
            int rowWidth = GetRowWidth(&textBox, textBox.rowsCount - 1);
            cursorPos.x += rowWidth;
            printf("%i : Row Width : %d\n", textBox.rowsCount - 1, rowWidth);

            textBox.rowsCount--;
            if(currentRow > 0){
                currentRow--;
            }
        }


        RemoveCharInString(&textBox.string, textBox.string.length - 1);
        // printf("Length : %i ... %s\n", textBox.string.length, textBox.string.val); 
    }

    int cursorColumn = textBox.cursorColumn;
    int cursorRow = textBox.cursorRow;
    int index = textBox.string.length;
    if(cursorRow < textBox.rowsCount){
        for(int i = cursorRow; i <= textBox.rowsCount; i++){
            index -= GetRowCharCount(&textBox, i);
        }
        index += cursorColumn;   
    }else{
        index -= GetRowCharCount(&textBox, cursorRow);
        index += cursorColumn - 1; 
    }
    char left_c = textBox.string.val[index - 1];
    char right_c = textBox.string.val[index];
    Character *left_char = &trm.cts[0].characters[left_c];
    Character *right_char = &trm.cts[0].characters[right_c];

    if(key == GLFW_KEY_LEFT){
        printf("Index : %i .. (%c, %c)\n", index, left_c, right_c);
        if(left_c == '\n' && (cursorRow > 0)){
            cursorColumn = GetRowCharCount(&textBox, cursorRow - 1) - 1;
            cursorRow--;
            cursorPos.y += 48;
            cursorPos.x = textBox.left + GetRowWidth(&textBox, cursorRow);
        }else{
            cursorColumn--;
            cursorPos.x -= left_char->advance >> 6;
        }
        printf("Cursor Column : %i\nCursor Row : %i\n", cursorColumn, cursorRow);
    }
    
    if(key == GLFW_KEY_RIGHT){
        printf("Index : %i .. (%c, %c)\n", index, left_c, right_c);
        if(right_c == '\n' && (cursorRow < textBox.rowsCount)){
            cursorColumn = 0;
            cursorRow++;
            cursorPos.x = textBox.left;
            cursorPos.y -= 48;
        }else{
            cursorColumn++;
            cursorPos.x += right_char->advance >> 6;
        }
        printf("Cursor Column : %i\nCursor Row : %i\n", cursorColumn, cursorRow);
    }

    textBox.cursorColumn = cursorColumn;
    textBox.cursorRow = cursorRow;
    
    if(key == GLFW_KEY_ENTER){
        char newLine = '\n';
        AddToString(&textBox.string, newLine);
        
        cursorPos.x = textBox.left;
        cursorPos.y -= 48;
        textBox.rowsCount++;
        currentRow++;
    }
}

void start(AppState *app_state){
    SetupTextRenderer(&trm, PROJ_RIGHT, PROJ_TOP, app_state->window_width, app_state->window_height );
    Setup2dRendering(&trm);


    String ts = Create_String("123789");
    AddToString(&ts, 6, "876\0");
    textBox.left = 100.0f;
    textBox.top = 400.0f; 

    cursorPos.x = textBox.left;
    cursorPos.y = textBox.top;

    testString = Create_String("The quick brown fox jumps over the lazy dog!");
    textBox.string = Create_String("1\n12\n123\n1234\n123\n12\n1");
    
    for(int i = 0; i < textBox.string.length;i++){
        char val = textBox.string.val[i];
        Character *c = &trm.cts[0].characters[val];

        // Left 20 bits are row width (Max width = 1,048,576)
        // Right 12 bits are row count (Max characters = 4096)
        IncreaseRowWidthCharCount(&textBox, currentRow, (c->advance >> 6), 1);

        if(val == '\n'){
            currentRow++;
            textBox.cursorRow++;
            textBox.cursorColumn = 0;
        } else{
            textBox.cursorColumn++;
        }
    }
    // First row is 0
    textBox.rowsCount = currentRow;
    printf("Rows Count :%i\n", currentRow);

    // Moving cursor to the end of the line
    cursorPos.y = textBox.top - (currentRow * 48);

    for(int i = 0; i < GetRowCharCount(&textBox, textBox.rowsCount); i++){
        char val = textBox.string.val[textBox.string.length - i];
        Character *c = &trm.cts[0].characters[val];
        cursorPos.x += c->advance >> 6;
    }
}

float timeBtwBlink = 0.65f;
float currBlinkTime = 0.0f;

void draw(AppState *app_state){

    testSquare = create_render_square(app_state, testSquarePos, 
        testSquareDim, testSquareClr, testSquareBrdClr);
    cursorSquare = create_render_square(app_state, cursorPos,
        cursorDim, cursorColor, cursorColor);

    if(currBlinkTime <= timeBtwBlink){
        if(cursorSquare){
            cursorSquare->color.a = 255;
            cursorSquare->border_clr.a = 255;
        }
    }else if(currBlinkTime >= (timeBtwBlink * 2)){
        currBlinkTime = 0.0f;
    }else{
        if(cursorSquare){
            cursorSquare->color.a = 0;
            cursorSquare->border_clr.a = 0;
        }
    }

    DrawText(&trm, testString, 1.0f, HMM_Vec2{100.0f, 600.0f}, 
        HMM_Vec3{0.0f, 255.0f, 0.0f});
    DrawText(&trm, textBox.string, 1.0f, HMM_Vec2{textBox.left, textBox.top}, 
        HMM_Vec3{0.0f, 255.0f, 0.0f});
}

void update(AppState *app_state, float dt){
    if(recievingTextInput){

    }

    currBlinkTime += dt;    
}
};