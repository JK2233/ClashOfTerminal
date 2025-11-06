#pragma execution_character_set("utf-8")

//includes
#include <cstddef>
#include <cstdint>

void LoadTextures() {
    {
        render::Texel tmpTexel[] = {
            { 100, 200, U'A' }, { 100, 200, U'B' }, { 100, 200, U'C' },
            { 100, 200, U'U' }, { 100, 200, U'U' }, { 100, 200, U'U' },
            { 100, 200, U'U' }, { 100, 200, U'V' }, { 100, 200, U'U' }
        };
        render::AddTextureTemplate<3, 3, 0>("test", tmpTexel);
    }
    {
        render::Texel tmpTexel[] = {
            { 100, 055, U'?' }, { 22, 055, U'?' }
        };
        render::AddTextureTemplate<1, 2, 0>("map_undef", tmpTexel);
    }
    {
        render::Texel tmpTexel[] = {
            { 33, 39, U'∼' }, { 33, 39, U'~' }, //1
            { 33, 39, U'∼' }, { 33, 39, U'~' },
            { 33, 39, U'∼' }, { 33, 39, U'~' },
            { 33, 39, U'∼' }, { 33, 39, U'~' },
            { 33, 39, U'∼' }, { 33, 39, U'~' },
            { 33, 39, U'∼' }, { 33, 39, U'~' },
            { 33, 39, U'∼' }, { 33, 39, U'~' }, //9
            { 33, 39, U'∼' }, { 33, 39, U'~' },
            { 33, 39, U'~' }, { 33, 39, U'∼' },
            { 33, 39, U'~' }, { 33, 39, U'∼' },
            { 33, 39, U'~' }, { 33, 39, U'∼' },
            { 33, 39, U'~' }, { 33, 39, U'∼' },
            { 33, 39, U'~' }, { 33, 39, U'∼' },
            { 33, 39, U'~' }, { 33, 39, U'∼' },
            { 33, 39, U'~' }, { 33, 39, U'∼' },
            { 33, 39, U'~' }, { 33, 39, U'∼' }   //20
        };
        render::AddTextureTemplate<1, 2, 20>("map_water", tmpTexel);
    }
    {
        render::Texel tmpTexel[] = {
            { 22, 28, U'🬤' }, { 22, 28, U'🬤' }
        };
        render::AddTextureTemplate<1, 2, 0>("map_plains0", tmpTexel);
    }
    {
        render::Texel tmpTexel[] = {
            { 22, 28, U'🮕' }, { 22, 28, U'▚' }
        };
        render::AddTextureTemplate<1, 2, 0>("map_plains1", tmpTexel);
    }
    {
        render::Texel tmpTexel[] = {
            { 22, 28, U'▖' }, { 22, 28, U'🮕' }
        };
        render::AddTextureTemplate<1, 2, 0>("map_plains2", tmpTexel);
    }
    {
        render::Texel tmpTexel[] = {
            { 22, 28, U'▚' }, { 22, 28, U'🮕' }
        };
        render::AddTextureTemplate<1, 2, 0>("map_plains3", tmpTexel);
    }
    {
        render::Texel tmpTexel[] = {
            { 22, 28, U' ' }, { 22, 28, U'▖' }
        };
        render::AddTextureTemplate<1, 2, 0>("map_plains4", tmpTexel);
    }
    {
        render::Texel tmpTexel[] = {
            { 22, 28, U'▚' }, { 22, 28, U' ' }
        };
        render::AddTextureTemplate<1, 2, 0>("map_plains5", tmpTexel);
    }
    {
        render::Texel tmpTexel[] = {
            { 22, 28, U' ' }, { 22, 28, U'🮕' }
        };
        render::AddTextureTemplate<1, 2, 0>("map_plains6", tmpTexel);
    }
    {
        render::Texel tmpTexel[] = {
            { 22, 28, U'■' }, { 22, 28, U' ' }
        };
        render::AddTextureTemplate<1, 2, 0>("map_plains7", tmpTexel);
    }
    {
        render::Texel tmpTexel[] = {
            { 244, 244, U' ' }, { 244, 244, U' ' }
        };
        render::AddTextureTemplate<1, 2, 0>("map_base", tmpTexel);
    }
    {
        render::Texel tmpTexel[] = {
            { 245, 245, U' ' }, { 245, 245, U' ' }
        };
        render::AddTextureTemplate<1, 2, 0>("map_mainBase", tmpTexel);
    }
    {
        render::Texel tmpTexel[] = {
            { 130, 94, U'🮗' }, { 130, 94, U'🮗' }
        };
        render::AddTextureTemplate<1, 2, 0>("map_bridge", tmpTexel);
    }
}