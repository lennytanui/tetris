#ifndef SHAPES_H

#define SHAPES_COUNT 7
#define MAX_ROTATIONS 5

#define RED {255.0f, 0.0f, 0.0f, 255.0f}
#define GREEN {255.0f, 0.0f, 0.0f, 255.0f}
#define BLUE {255.0f, 0.0f, 0.0f, 255.0f}
#define YELLOW {255.0f, 0.0f, 0.0f, 255.0f}
#define ORANGE {255.0f, 0.0f, 0.0f, 255.0f}
#define VIOLET {255.0f, 0.0f, 0.0f, 255.0f}

struct Child_Block{
    v2 structure[4];
};

struct Block_Info{
    int rotations_count;
    RGBA color;
    Child_Block rotations[MAX_ROTATIONS];
};

static const Block_Info SHAPE_straight = {
    2, RGBA{255.0f, 0.0f, 0.0f, 255.0f}, 
    {
        {v2{0.0f, 0.0f}, v2{1.0f, 0.0f}, v2{2.0f, 0.0f}, v2{3.0f, 0.0f}}, 
        {v2{0.0f, 0.0f}, v2{0.0f, 1.0f}, v2{0.0f, 2.0f}, v2{0.0f, 3.0f}}
    }
};

static const Block_Info SHAPE_l1 = {
    4, RGBA{0.0f, 255.0f, 0.0f, 255.0f}, 
    {
        {v2{0.0f, 0.0f}, v2{1.0f, 0.0f}, v2{2.0f, 0.0f}, v2{0.0f, 1.0f}}, 
        {v2{0.0f, 0.0f}, v2{1.0f, 0.0f}, v2{1.0f, 1.0f}, v2{1.0f, 2.0f}},
        {v2{2.0f, 0.0f}, v2{2.0f, 1.0f}, v2{1.0f, 1.0f}, v2{0.0f, 1.0f}},
        {v2{0.0f, 0.0f}, v2{0.0f, 1.0f}, v2{0.0f, 2.0f}, v2{1.0f, 2.0f}}
    }
};


static const Block_Info SHAPE_l2 = {
    4, RGBA{0.0f, 0.0f, 255.0f, 255.0f}, 
    {
        {v2{0.0f, 0.0f}, v2{1.0f, 0.0f}, v2{0.0f, 1.0f}, v2{0.0f, 2.0f}}, 
        {v2{0.0f, 0.0f}, v2{1.0f, 0.0f}, v2{2.0f, 0.0f}, v2{2.0f, 1.0f}},
        {v2{1.0f, 0.0f}, v2{1.0f, 1.0f}, v2{1.0f, 2.0f}, v2{0.0f, 2.0f}},
        {v2{0.0f, 0.0f}, v2{0.0f, 1.0f}, v2{1.0f, 1.0f}, v2{2.0f, 1.0f}}
    }
};

static const Block_Info SHAPE_square = {
    1, RGBA{0.0f, 255.0f, 255.0f, 255.0f}, 
    {
        {v2{0.0f, 0.0f}, v2{1.0f, 0.0f}, v2{0.0f, 1.0f}, v2{1.0f, 1.0f}}
    }
};

static const Block_Info SHAPE_zigzag1 = {
    2, RGBA{255.0f, 0.0f, 255.0f, 255.0f}, 
    {
        {v2{0.0f, 0.0f}, v2{1.0f, 0.0f}, v2{1.0f, 1.0f}, v2{2.0f, 1.0f}},
        {v2{1.0f, 0.0f}, v2{1.0f, 1.0f}, v2{0.0f, 1.0f}, v2{0.0f, 2.0f}},
    }
};

static const Block_Info SHAPE_zigzag2 = {
    2, RGBA{255.0f, 0.0f, 255.0f, 255.0f}, 
    {
        {v2{1.0f, 0.0f}, v2{2.0f, 0.0f}, v2{0.0f, 1.0f}, v2{1.0f, 1.0f}},
        {v2{0.0f, 0.0f}, v2{0.0f, 1.0f}, v2{1.0f, 1.0f}, v2{1.0f, 2.0f}},
    }
};

static const Block_Info SHAPE_t = {
    4, RGBA{255.0f, 255.0f, 0.0f, 255.0f}, 
    {
        {v2{1.0f, 0.0f}, v2{0.0f, 1.0f}, v2{1.0f, 1.0f}, v2{2.0f, 1.0f}},
        {v2{1.0f, 0.0f}, v2{1.0f, 1.0f}, v2{1.0f, 2.0f}, v2{0.0f, 1.0f}},
        {v2{0.0f, 0.0f}, v2{0.0f, 1.0f}, v2{0.0f, 2.0f}, v2{1.0f, 1.0f}},
        {v2{0.0f, 0.0f}, v2{1.0f, 0.0f}, v2{2.0f, 0.0f}, v2{1.0f, 1.0f}},
    }
};

#define SHAPES_H
#endif