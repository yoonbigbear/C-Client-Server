#pragma once

#include "bb_math.h"
#include <vector>

class DynamicAABBTree
{
public:
    DynamicAABBTree(int size = 1024)
        :free_list_(Null)
        , size_(size)
        , root_(Null)
        , nodes_(size)
    {
        AddToFreeList(0);
    }

    enum { Null = -1, FattenOffset = 1, KStackCapacity = 256 };

    struct Node
    {
        bool IsLeaf(void) const
        {
            // The right leaf does not use the same memory as the userdata,
            // and will always be Null (no children)
            return right == Null;
        }

        // Fat AABB for leafs, bounding AABB for branches
        AABB2 aabb;

        union
        {
            int parent;
            int next; // free list
        };

        union
        {
            // Child indices
            struct
            {
                int left;
                int right;
            };

            // Since only leaf nodes hold userdata, we can use the
            // same memory used for left/right indices to store
            // the userdata void pointer
            int32_t userData;
        };

        // leaf = 0, free nodes = -1
        int height;

    };


    inline const AABB2& GetFatAABB(int32_t id)
    {
        _ASSERT(id >= 0 && id < size_);
        return nodes_[id].aabb;
    }
    inline void FattenAABB(AABB2& aabb)
    {
        Vector2 v(2, 2);
        aabb.min -= v;
        aabb.max += v;
    }

    int32_t AllocateNode()
    {
        if (free_list_ == Null)
        {
            size_ *= 2;
            nodes_.resize(size_);
            AddToFreeList(count_);
        }

        int32_t freeNode = free_list_;
        free_list_ = nodes_[free_list_].next;
        nodes_[freeNode].height = 0;
        nodes_[freeNode].left = Null;
        nodes_[freeNode].right = Null;
        nodes_[freeNode].parent = Null;
        nodes_[freeNode].userData = -1;
        ++count_;
        return freeNode;
    }

    int AllocateLeafNode(AABB2 box)
    {
        if (free_list_ == Null)
        {
            size_ *= 2;
            nodes_.resize(count_);
            AddToFreeList(count_);
        }

        int32_t freeNode = free_list_;
        free_list_ = nodes_[free_list_].next;
        nodes_[freeNode].height = 0;
        nodes_[freeNode].left = Null;
        nodes_[freeNode].right = Null;
        nodes_[freeNode].parent = Null;
        nodes_[freeNode].userData = -1;
        ++count_;
        return freeNode;
    }

    inline void AddToFreeList(int32_t index)
    {
        for (int32_t i = index; i < size_ - 1; ++i)
        {
            nodes_[i].next = i + 1;
            nodes_[i].height = Null;
        }

        nodes_[size_ - 1].next = Null;
        nodes_[size_ - 1].height = Null;
        free_list_ = index;
    }

    inline void DeallocateNode(int32_t index)
    {
        _ASSERT(index >= 0 && index < size_);

        nodes_[index].next = free_list_;
        nodes_[index].height = Null;
        free_list_ = index;

        --count_;
    }

    int32_t Insert(const AABB2& aabb, int32_t entity)
    {
        int32_t id = AllocateNode();

        // Fatten AABB and set height/userdata
        nodes_[id].aabb = aabb;
        FattenAABB(nodes_[id].aabb);
        nodes_[id].userData = entity;
        nodes_[id].height = 0;

        InsertLeaf(id);

        return id;
    }

    void InsertLeaf(int32_t id)
    {
        if (root_ == Null)
        {
            root_ = id;
            nodes_[root_].parent = Null;
            return;
        }

        // Search for sibling
        int32_t searchIndex = root_;
        AABB2 leafAABB = nodes_[id].aabb;
        while (!nodes_[searchIndex].IsLeaf())
        {
            // Cost for insertion at index (branch node), involves creation
            // of new branch to contain this index and the new leaf
            AABB2 combined = Union(leafAABB, nodes_[searchIndex].aabb);
            float combinedArea = combined.SurfaceArea();
            float branchCost = 2.0f * combinedArea;

            // Inherited cost (surface area growth from heirarchy update after descent)
            float inheritedCost = 2.0f * (combinedArea - nodes_[searchIndex].aabb.SurfaceArea());

            int32_t left = nodes_[searchIndex].left;
            int32_t right = nodes_[searchIndex].right;

            // Calculate costs for left/right descents. If traversal is to a leaf,
            // then the cost of the combind AABB represents a new branch node. Otherwise
            // the cost is only the inflation of the pre-existing branch.
            float leftDescentCost;
            if (nodes_[left].IsLeaf())
                leftDescentCost = Union(leafAABB, nodes_[left].aabb).SurfaceArea() + inheritedCost;
            else
            {
                float inflated = Union(leafAABB, nodes_[left].aabb).SurfaceArea();
                float branchArea = nodes_[left].aabb.SurfaceArea();
                leftDescentCost = inflated - branchArea + inheritedCost;
            }

            // Cost for right descent
            float rightDescentCost;
            if (nodes_[right].IsLeaf())
                rightDescentCost = Union(leafAABB, nodes_[right].aabb).SurfaceArea() + inheritedCost;
            else
            {
                float inflated = Union(leafAABB, nodes_[right].aabb).SurfaceArea();
                float branchArea = nodes_[right].aabb.SurfaceArea();
                rightDescentCost = inflated - branchArea + inheritedCost;
            }

            // Determine traversal direction, or early out on a branch index
            if (branchCost < leftDescentCost && branchCost < rightDescentCost)
                break;

            if (leftDescentCost < rightDescentCost)
                searchIndex = left;

            else
                searchIndex = right;
        }

        int32_t sibling = searchIndex;

        // Create new parent
        int32_t oldParent = nodes_[sibling].parent;
        int32_t newParent = AllocateNode();
        nodes_[newParent].parent = oldParent;
        nodes_[newParent].userData = -1;
        nodes_[newParent].aabb = Union(leafAABB, nodes_[sibling].aabb);
        nodes_[newParent].height = nodes_[sibling].height + 1;

        // Sibling was root
        if (oldParent == Null)
        {
            nodes_[newParent].left = sibling;
            nodes_[newParent].right = id;
            nodes_[sibling].parent = newParent;
            nodes_[id].parent = newParent;
            root_ = newParent;
        }

        else
        {
            if (nodes_[oldParent].left == sibling)
                nodes_[oldParent].left = newParent;

            else
                nodes_[oldParent].right = newParent;

            nodes_[newParent].left = sibling;
            nodes_[newParent].right = id;
            nodes_[sibling].parent = newParent;
            nodes_[id].parent = newParent;
        }

        SyncHeirarchy(nodes_[id].parent);
    }

    void SyncHeirarchy(int32_t index)
    {
        while (index != Null)
        {
            index = Balance(index);

            int32_t left = nodes_[index].left;
            int32_t right = nodes_[index].right;

            nodes_[index].height = 1 + Max(nodes_[left].height, nodes_[right].height);
            nodes_[index].aabb = Union(nodes_[left].aabb, nodes_[right].aabb);

            index = nodes_[index].parent;
        }
    }

    void Remove(int32_t id)
    {
        _ASSERT(id >= 0 && id < size_);
        _ASSERT(nodes_[id].IsLeaf());

        RemoveLeaf(id);
        DeallocateNode(id);
    }

    void RemoveLeaf(int32_t id)
    {
        if (id == root_)
        {
            root_ = Null;
            return;
        }

        // Setup parent, grandParent and sibling
        int32_t parent = nodes_[id].parent;
        int32_t grandParent = nodes_[parent].parent;
        int32_t sibling;

        if (nodes_[parent].left == id)
            sibling = nodes_[parent].right;

        else
            sibling = nodes_[parent].left;

        // Remove parent and replace with sibling
        if (grandParent != Null)
        {
            // Connect grandParent to sibling
            if (nodes_[grandParent].left == parent)
                nodes_[grandParent].left = sibling;

            else
                nodes_[grandParent].right = sibling;

            // Connect sibling to grandParent
            nodes_[sibling].parent = grandParent;
        }

        // Parent was root
        else
        {
            root_ = sibling;
            nodes_[sibling].parent = Null;
        }

        DeallocateNode(parent);
        SyncHeirarchy(grandParent);
    }

    int32_t GetUserData(int32_t id) const
    {
        _ASSERT(id >= 0 && id < size_);

        return nodes_[id].userData;
    }

    bool Update(int32_t id, const AABB2& aabb)
    {
        _ASSERT(id >= 0 && id < size_);
        _ASSERT(nodes_[id].IsLeaf());

        if (nodes_[id].aabb.Contains(aabb))
            return false;

        RemoveLeaf(id);

        nodes_[id].aabb = aabb;
        FattenAABB(nodes_[id].aabb);

        InsertLeaf(id);

        return true;
    }

    template <typename T>
    inline void Query(T* cb, const AABB2& aabb) const
    {
        int32_t stack[KStackCapacity];
        uint32_t sp = 1;

        *stack = nodes_[root_];

        while (sp)
        {
            _ASSERT(sp < KStackCapacity); // k_stackCapacity too small!
            int32_t id = stack[--sp];

            const Node* n = &nodes_[id];
            if (TestOverlap(aabb, n->aabb))
            {
                if (n->IsLeaf())
                {
                    // Report, via callback, a collision with leaf
                    if (!cb->TreeCallBack(id))
                        return;
                }
                else
                {
                    stack[sp++] = n->left;
                    stack[sp++] = n->right;
                }
            }
        }
    }

    inline int32_t Query(const AABB2& aabb) const
    {
        int32_t stack[KStackCapacity];
        int32_t sp = 1;

        *stack = nodes_[root_].userData;

        while (sp)
        {
            _ASSERT(sp < KStackCapacity); // k_stackCapacity too small!
            int32_t id = stack[--sp];

            const Node* n = &nodes_[id];
            if (TestOverlap(aabb, n->aabb))
            {
                if (n->IsLeaf())
                {
                    return nodes_[id].userData;
                }
                else
                {
                    stack[sp++] = n->left;
                    stack[sp++] = n->right;
                }
            }
        }
        return Null;
    }

    /*template <typename T>
    void Query(T* cb, q3RaycastData& rayCast) const
    {
        const r32 k_epsilon = r32(1.0e-6);
        const i32 k_stackCapacity = 256;
        i32 stack[k_stackCapacity];
        i32 sp = 1;

        *stack = m_root;

        q3Vec3 p0 = rayCast.start;
        q3Vec3 p1 = p0 + rayCast.dir * rayCast.t;

        while (sp)
        {
            // k_stackCapacity too small
            assert(sp < k_stackCapacity);

            i32 id = stack[--sp];

            if (id == Node::Null)
                continue;

            const Node* n = m_nodes + id;

            q3Vec3 e = n->aabb.max - n->aabb.min;
            q3Vec3 d = p1 - p0;
            q3Vec3 m = p0 + p1 - n->aabb.min - n->aabb.max;

            r32 adx = q3Abs(d.x);

            if (q3Abs(m.x) > e.x + adx)
                continue;

            r32 ady = q3Abs(d.y);

            if (q3Abs(m.y) > e.y + ady)
                continue;

            r32 adz = q3Abs(d.z);

            if (q3Abs(m.z) > e.z + adz)
                continue;

            adx += k_epsilon;
            ady += k_epsilon;
            adz += k_epsilon;

            if (q3Abs(m.y * d.z - m.z * d.y) > e.y * adz + e.z * ady)
                continue;

            if (q3Abs(m.z * d.x - m.x * d.z) > e.x * adz + e.z * adx)
                continue;

            if (q3Abs(m.x * d.y - m.y * d.x) > e.x * ady + e.y * adx)
                continue;

            if (n->IsLeaf())
            {
                if (!cb->TreeCallBack(id))
                    return;
            }

            else
            {
                stack[sp++] = n->left;
                stack[sp++] = n->right;
            }
        }
    }*/

    int32_t Balance(int32_t iA)
    {
        Node* A = &nodes_[iA];

        if (A->IsLeaf() || A->height == 1)
            return iA;

        /*      A
              /   \
             B     C
            / \   / \
           D   E F   G
        */

        int32_t iB = A->left;
        int32_t iC = A->right;
        Node* B = &nodes_[iB];
        Node* C = &nodes_[iC];

        int32_t balance = C->height - B->height;

        // C is higher, promote C
        if (balance > 1)
        {
            int32_t iF = C->left;
            int32_t iG = C->right;
            Node* F = &nodes_[iF];
            Node* G = &nodes_[iG];

            // grandParent point to C
            if (A->parent != Null)
            {
                if (nodes_[A->parent].left == iA)
                    nodes_[A->parent].left = iC;

                else
                    nodes_[A->parent].right = iC;
            }
            else
                root_ = iC;

            // Swap A and C
            C->left = iA;
            C->parent = A->parent;
            A->parent = iC;

            // Finish rotation
            if (F->height > G->height)
            {
                C->right = iF;
                A->right = iG;
                G->parent = iA;
                A->aabb = Union(B->aabb, G->aabb);
                C->aabb = Union(A->aabb, F->aabb);

                A->height = 1 + Max(B->height, G->height);
                C->height = 1 + Max(A->height, F->height);
            }

            else
            {
                C->right = iG;
                A->right = iF;
                F->parent = iA;
                A->aabb = Union(B->aabb, F->aabb);
                C->aabb = Union(A->aabb, G->aabb);

                A->height = 1 + Max(B->height, F->height);
                C->height = 1 + Max(A->height, G->height);
            }

            return iC;
        }

        // B is higher, promote B
        else if (balance < -1)
        {
            int32_t iD = B->left;
            int32_t iE = B->right;
            Node* D = &nodes_[iD];
            Node* E = &nodes_[iE];

            // grandParent point to B
            if (A->parent != Null)
            {
                if (nodes_[A->parent].left == iA)
                    nodes_[A->parent].left = iB;
                else
                    nodes_[A->parent].right = iB;
            }

            else
                root_ = iB;

            // Swap A and B
            B->right = iA;
            B->parent = A->parent;
            A->parent = iB;

            // Finish rotation
            if (D->height > E->height)
            {
                B->left = iD;
                A->left = iE;
                E->parent = iA;
                A->aabb = Union(C->aabb, E->aabb);
                B->aabb = Union(A->aabb, D->aabb);

                A->height = 1 + Max(C->height, E->height);
                B->height = 1 + Max(A->height, D->height);
            }

            else
            {
                B->left = iE;
                A->left = iD;
                D->parent = iA;
                A->aabb = Union(C->aabb, D->aabb);
                B->aabb = Union(A->aabb, E->aabb);

                A->height = 1 + Max(C->height, D->height);
                B->height = 1 + Max(A->height, E->height);
            }

            return iB;
        }

        return iA;
    }

    int32_t free_list_;
    int32_t root_;
    int32_t size_;
    int32_t count_;
    std::vector<Node> nodes_;
};
