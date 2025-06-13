#pragma once

#include "comm_def.h"
#include <unordered_map>
#include <string>

class GridKey
{
public:
    GridKey() = default;
    explicit GridKey(u64 key): k1_(HIGH32(key)), k2_(LOW32(key)) {}
    GridKey(s32 k1, s32 k2): k1_(k1), k2_(k2) {}
    s32 Row() const { return k1_; }
    s32 Col() const { return k2_; }
    operator u64() const { return MAKE64(k1_, k2_); }
    GridKey operator+(const GridKey& key) const
    {
        return GridKey(k1_ + key.k1_, k2_ + key.k2_);
    }
    GridKey& operator+=(const GridKey& key)
    {
        k1_ += key.k1_;
        k2_ += key.k2_;
        return *this;
    }
    bool IsValid() const { return k1_ >= 0 && k2_ >= 0; }
private:
    s32 k1_ = -1;
    s32 k2_ = -1;
};

template<class OS>
OS& operator<<(OS& os, const GridKey& key)
{
    return os << key.Row() << "," << key.Col();
}

namespace std
{
    template <> struct hash<GridKey>
    {
        size_t operator()(const GridKey& key) const
        {
            return std::hash<u64>{}(key);
        }
    };
}



class GridRect
{
public:
    GridRect() = default;
    GridRect(const GridKey& top_left, s32 height, s32 width) : top_left_(top_left), height_(height), width_(width) {}
    GridRect(const GridKey& top_left, const GridKey& bottom_right)
        : top_left_(top_left)
        , height_(bottom_right.Row() - top_left.Row() + 1)
        , width_(bottom_right.Col() - top_left.Col() + 1) {}
    GridKey TopLeft() const { return top_left_; }
    GridKey BottomRight() const { return top_left_ + GridKey(height_ - 1, width_ - 1); }
    s32 Height() const { return height_; }
    s32 Width() const { return width_; }
    bool IsValid() const { return top_left_.IsValid() && height_ > 0 && width_ > 0; }
    bool operator==(const GridRect& rect) const { return top_left_ == rect.top_left_ && height_ == rect.height_ && width_ == rect.width_;}
    bool Contains(const GridKey& key) const
    {
        return top_left_.Row() <= key.Row() && top_left_.Col() <= key.Col() &&
               key.Row() < top_left_.Row() + height_ && key.Col() < top_left_.Col() + width_;
    }
    bool Contains(const GridRect& rect) const
    {
        return top_left_.Row() <= rect.top_left_.Row() && top_left_.Col() <= rect.top_left_.Col() &&
               rect.top_left_.Row() + rect.height_ <= top_left_.Row() + height_ &&
               rect.top_left_.Col() + rect.width_ <= top_left_.Col() + width_;
    }
private:
    GridKey top_left_;
    s32 height_ = 0;
    s32 width_ = 0;
};
template<class OS>
OS& operator<<(OS& os, const GridRect& rect)
{
    return os << rect.TopLeft() << ":" << rect.Height() << "," << rect.Width();
}


class PackageGrid
{
public:
    // 这个地方可以改为ShmHashMap
    using GridType = std::unordered_map<GridKey, u64>;  // 哈希表
    // using GridType = std::map<GridKey, u64>;  // 红黑树
    using IteratorType = GridType::iterator;
    using ConstIteratorType = GridType::const_iterator;
    using KeyType = GridKey;

    u64& operator[](const KeyType& key)
    {
        return pos_matrix_[key];
    }
    
    std::pair<IteratorType, bool> Insert(const KeyType& key, u64 value)
    {
        return pos_matrix_.insert(std::make_pair(key, value));
    }

    bool Contains(const KeyType& key) const
    {
        return pos_matrix_.find(key) != pos_matrix_.end();
    }

    // s32 Count(const KeyType& key) const
    // {
    //     return pos_matrix_.count(key);
    // }

    void Set(const KeyType& key, u64 value)
    {
        pos_matrix_[key] = value;
    }

    u64 Get(const KeyType& key) const
    {
        if (Contains(key))
        {
            return pos_matrix_.at(key);
        }
        return 0;
    }

    bool Remove(const KeyType& key)
    {
        return pos_matrix_.erase(key) > 0;
    }

    // 迭代器 begin
    ConstIteratorType begin() const
    {
        return pos_matrix_.begin();
    }
    ConstIteratorType end() const
    {
        return pos_matrix_.end();
    }
    IteratorType begin()
    {
        return pos_matrix_.begin();
    }
    IteratorType end()
    {
        return pos_matrix_.end();
    }
    // 迭代器 end


    // 创建一个格子快照
    PackageGrid MakeSnapshot() const
    {
        // 快照是否需要时间信息/版本信息
        PackageGrid snapshot;
        snapshot.pos_matrix_ = pos_matrix_;
        return snapshot;
    }


    // ================================== 矩阵op ==================================
    // 只判断某个区域是否为空， 不存在边界
    bool RectIsFree(const GridRect& rect) const;

    // 在某个区域rect内寻找一块空闲的矩阵
    GridRect GetOneFreeRect(const GridRect& rect, s32 height, s32 width, bool can_rotate = true) const;

    // 在某个区域rect内寻找一块空闲的矩阵， 优先在hint附近寻找
    GridRect GetNearestFreeRect(const GridRect& rect, const GridKey& hint, s32 height, s32 width, bool can_rotate = true) const;

    void SetRect(const GridRect& rect, u64 value);

    // return: remove count
    s32 RemoveRect(const GridRect& rect);

    // ================================== 矩阵op end ==================================


    std::string DebugShapeString() const;

private:
    GridType pos_matrix_;
};
