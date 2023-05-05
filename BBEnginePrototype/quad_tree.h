#pragma once

#include "types.h"
#include "geometry2d.h"

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 255
#define CHUNK_SIZE_Z 16

namespace bb {

    int MAX_OBJECTS = 10;
    int MAX_LEVELS = 5;

    template <typename _Type>
    class QuadTree
    {
        enum { MaxDepth = 8 };

    public:
        QuadTree(const rect& size = { {0.f,0.f}, {100.f, 100.f } }, const size_t depth = 0)
            :depth_(depth)
        {
            resize(size);
        }

        void resize(const rect& area)
        {
            clear();
            rect_ = area;
            vec2 childsize = rect_.size * 0.5f;

            child_rects = {
                rect(rect_.pos,                                 childsize),
                rect({rect_.pos.x + childsize.x, rect_.pos.y},   childsize),
                rect({rect_.pos.x, rect_.pos.y + childsize.y},   childsize),
                rect(rect_.pos + childsize,                     childsize),
            };
        }
        void clear()
        {
            items_.clear();

            // Iterate through children, erase them too
            for (int i = 0; i < 4; i++)
            {
                if (potential_child[i])
                    potential_child[i]->clear();
                potential_child[i].reset();
            }
        }

        void size() const
        {
            size_t n = items.size();
            for (int i = 0; i < 4; ++i)
            {
                if (potential_child[i])
                {
                    n += potential_child[i]->size();
                }
            }
            return n;
        }

        void insert(const _Type& item, const rect& itemsize)
        {
            for (int i = 0; i < 4; ++i)
            {
                if (child_rects[i].contains(itemsize))
                {
                    if (depth_ + 1 < MaxDepth)
                    {
                        if (!potential_child[i])
                        {
                            potential_child[i] = std::make_shared<QuadTree<_Type>>(child_rects[i], depth_ + 1);
                        }

                        potential_child[i]->insert(item, itemsize);
                        return;
                    }
                }
            }

            items_.emplace_back(std::pair(itemsize, item));
        }

        List<_Type> search(const rect& area) const
        {
            List<_Type> list_items;
            search(area, list_items);
            return list_items;
        }

        void search(const rect& area, List<_Type>& list_items) const
        {
            for (const auto& e : items_)
            {
                if (area.overlaps(e.first))
                    list_items.emplace_back(e.second);
            }

            for (int i = 0; i < 4; ++i)
            {
                if (child_rects[i])
                {
                    if (area.contains(child_rects[i]))
                    {
                        items_[i]->items(list_items);
                    }
                    else if (child_rects[i].overlaps(area))
                    {
                        items_[i]->search(area, list_items);
                    }
                }
            }
        }

        void items(List<_Type>& list_items) const
        {
            for (const auto& e : items_)
            {
                list_items.emplace_back(e.second);
            }

            for (int i = 0; i < 4; ++i)
            {
                if (potential_child[i])
                {
                    potential_child[i]->items(list_items);
                }
            }
        }

        List<_Type> items() const
        {
            List<_Type> list_items;
            items_(list_items);
            return list_items;
        }

        const rect& area()
        {
            return rect_;
        }

    private:
        int depth_;
        rect rect_;
        std::array<rect, 4> child_rects;
        std::vector<std::pair<rect, _Type>> items_;
        std::array<std::shared_ptr<QuadTree<_Type>>, 4> potential_child;
    };

    template <typename _Type>
    class QuadTreeContainer
    {
    protected:
        List<_Type> all_items;
        QuadTree<typename List<_Type>::iterator> root;

    public:
        QuadTreeContainer(const rect& size = { {0.f, 0.f}, {100.f, 100.f} }, const size_t depth = 0)
            :root(size, depth) {}

        void resize(const rect& area)
        {
            root.clear();
            all_items.clear();
        }

        typename List<_Type>::iterator begin()
        {
            return all_items.begin();
        }
        typename List<_Type>::iterator end()
        {
            return all_items.end();
        }
        typename List<_Type>::const_iterator cbegin()
        {
            return all_items.cbegin();
        }
        typename List<_Type>::const_iterator cend()
        {
            return all_items.cend();
        }

        void insert(const _Type& item, const rect& itemsize)
        {
            all_items.emplace_back(item);
            root.insert(std::prev(all_items.end()), itemsize);
        }

        List<typename List<_Type>::iterator> search(const rect& area) const
        {
            List<typename List<QuadTreeContainer>::iterator> listitems_pointer;
            root.search(area, listitems_pointer);
            return listitems_pointer;
        }

        bool remove
    };
}