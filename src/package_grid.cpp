#include "package_grid.h"


#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_set>
#include <climits>
#include <iomanip>


// 只判断某个区域是否为空， 不存在边界
bool PackageGrid::RectIsFree(const GridRect& rect) const
{
    for (s32 i = 0; i < rect.Height(); ++i)
    {
        for (s32 j = 0; j < rect.Width(); ++j)
        {
            auto tmp_key = rect.TopLeft() + GridKey(i, j);
            if (Get(tmp_key) != 0)
            {
                return false;
            }
        }
    }
    return true;
}

// 在某个区域rect内寻找一块空闲的矩阵
GridRect PackageGrid::GetOneFreeRect(const GridRect& rect, s32 height, s32 width, bool can_rotate) const
{
    for (s32 i = 0; i < rect.Height(); ++i)
    {
        for (s32 j = 0; j < rect.Width(); ++j)
        {
            auto left_top = rect.TopLeft() + GridKey(i, j);
            if (Contains(left_top))
            {
                continue;
            }
            GridRect tmp_rect{left_top, height, width};
            if (! rect.Contains(tmp_rect))
            {
                continue;
            }
            if (RectIsFree(tmp_rect))
            {
                return tmp_rect;
            }

            // 判断旋转后是否可以放置
            if (can_rotate && height != width)
            {
                tmp_rect = GridRect{left_top, width, height};
                if (! rect.Contains(tmp_rect))
                {
                    continue;
                }
                if (RectIsFree(tmp_rect))
                {
                    return tmp_rect;
                }
            }
        }
    }
    return {};
}

void PackageGrid::SetRect(const GridRect& rect, u64 value)
{
    for (s32 i = 0; i < rect.Height(); ++i)
    {
        for (s32 j = 0; j < rect.Width(); ++j)
        {
            Set(rect.TopLeft() + GridKey(i, j), value);
        }
    }
}

// return: remove count
s32 PackageGrid::RemoveRect(const GridRect& rect)
{
    s32 rm_count = 0;
    for (s32 i = 0; i < rect.Height(); ++i)
    {
        for (s32 j = 0; j < rect.Width(); ++j)
        {
            rm_count += Remove(rect.TopLeft() + GridKey(i, j));
        }
    }
    return rm_count;
}

GridRect PackageGrid::GetNearestFreeRect(const GridRect& rect, const GridKey& hint, s32 height, s32 width, bool can_rotate) const
{
    // 8个方向的偏移量
    static constexpr s32 directions[8][2] = {
        {-1, -1}, // 左上
        {-1, 0},  // 上
        {-1, 1},  // 右上
        {0, -1},  // 左
        {0, 1},    // 右
        {1, -1},  // 左下
        {1, 0},   // 下
        {1, 1},   // 右下
    };

    std::queue<GridKey> search_queue;
    std::unordered_set<GridKey> searched;
    
    search_queue.emplace(hint);
    searched.emplace(hint);

    while (search_queue.size() > 0)
    {
        auto left_top = search_queue.front();
        search_queue.pop();

        if (! Contains(left_top))
        {
            GridRect tmp_rect{left_top, height, width};
            if (rect.Contains(tmp_rect) && RectIsFree(tmp_rect))
            {
                return tmp_rect;
            }
            
            if (can_rotate && height != width)
            {
                tmp_rect = GridRect{left_top, width, height};
                if (rect.Contains(tmp_rect) && RectIsFree(tmp_rect))
                {
                    return tmp_rect;
                }
            }
        }


        for (const auto& direction : directions)
        {
            auto next = left_top + GridKey(direction[0], direction[1]);
            if (next.Row() < 0 || next.Row() > rect.BottomRight().Row() || next.Col() < 0 || next.Col() > rect.BottomRight().Col())
            {
                continue;
            }

            if (searched.find(next) == searched.end())
            {
                searched.emplace(next);
                search_queue.emplace(next);
            }
        }
    }

    return {};
}

// ================================== 矩阵op end ==================================


std::string PackageGrid::DebugShapeString() const
{
    std::ostringstream oss;
    std::unordered_map<u64, char> color_map;
    s32 current_char = 0; // 33~127, 可显示的ASCII字符
    char cg[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                 '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                 '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '-', '_', '=', '+', '[', ']', '{', '}', '\\', '|', ';', ':', '\'', '\"', ',',
                 '.', '<', '>', '/', '?',};
    const s32 cg_len = sizeof(cg) / sizeof(cg[0]);

    s32 min_row = INT_MAX;
    s32 min_col = INT_MAX;
    s32 max_row = 0;
    s32 max_col = 0;
    for (auto& kv : pos_matrix_)
    {
        auto key = kv.first;
        auto value = kv.second;
        if (color_map.find(value) == color_map.end())
        {
            color_map[value] = cg[current_char++];
            if (current_char >= cg_len)
            {
                current_char = 0;
            }
        }
        max_row = std::max(max_row, key.Row());
        max_col = std::max(max_col, key.Col());
        min_row = std::min(min_row, key.Row());
        min_col = std::min(min_col, key.Col());
    }

    for (auto& kv : color_map)
    {
        oss << kv.first << " char is " << kv.second << std::endl;
    }

    std::vector<std::string> shape(max_row + 1, std::string(max_col + 3, ' '));

    for (s32 i = min_row; i <= max_row; ++i)
    {
        shape[i][0] = shape[i][max_col + 2] = '|';

        for (s32 j = min_col; j <= max_col; ++j)
        {
            auto key = GridKey(i, j);
            auto value = Get(key);
            if (value == 0)
            {
                shape[i][j + 1] = ' ';
                continue;
            }
            char x = color_map[value];
            shape[i][j + 1] = x;
        }
        oss << std::setw(6) << i << ": " << shape[i] << std::endl;
    }

    return oss.str();
}
