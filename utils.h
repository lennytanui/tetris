#pragma once

struct RGBA{
    union {
        struct{
            float r;
            float g;
            float b;
            float a;
        };

        float elements[4];
    };
};