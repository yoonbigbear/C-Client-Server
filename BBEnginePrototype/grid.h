#pragma once

#include "geometry2d.h"

namespace bb {

    struct Grid
    {
        vec2i8 pos;
    };

    template<int _XSize = 10, int _YSize = 10>
    class Sector
    {
    public:
        enum {SizeX = _XSize, SizeY = _YSize};

        Sector()
        {
            for (int i = 0; i < _YSize; ++i)
            {
                for (int j = 0; j < _XSize; ++j)
                {
                    grids_[j + (j * i)] = { j , i };
                }
            }
        }

    private:
        std::array<Grid, SizeX * SizeY>  grids_;
    };

    class GridMap
    {
    public:
        GridMap(int w, int h, bb::vec2 pos)
            :grids_(w* h)
        {
            size_.x = w;
            size_.y = h;

            bounds_.size.x = static_cast<float>(w * 1);
            bounds_.size.y = static_cast<float>(h * 1);
            bounds_.pos = pos;

            for (int i = 0; i < h; ++i)
            {
                for (int j = 0; j < w; ++j)
                {
                    grids_[j + (h * i)].pos = vec2(j * 1, i * 1);
                    grids_[j + (h * i)].size = vec2(1, 1);
                }
            }
        }

        std::list<int> contains(const bb::rect& rect)
        {
            std::list<int> list;

            auto x = std::floor(rect.pos.x / static_cast<float>(size_.x));
            auto y = std::floor(rect.pos.y / static_cast<float>(size_.y));

            auto index = y * size_.y + x;
            list.emplace_back(index);
            return list;
        }

        vec2i8 size_;
        rect bounds_;
        std::vector<rect> grids_;
    };
}
