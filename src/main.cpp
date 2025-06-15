
#include <iostream>


#include "comm_def.h"

#include "package_grid.h"

#include "test.h"


#ifdef _WIN32

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/dom/table.hpp"

#endif

// #define TEST

#ifdef TEST

int main()
{
    // //////////////////////////////////////////
    TestGridKey();
    TestGridRect();
    TestPackageGridForLoop();
    TestPackageGrid();
    TestPackageGridRect();
    TestFillPackageGridRect();
    TestFillPackageGridRect2();
    std::cout << "All tests passed!" << std::endl;

    // //////////////////////////////////////////
    
    return 0;
}

#endif



#ifdef _WIN32
#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include <vector>
#include <random>
#include <cmath>

using namespace ftxui;

// 生成随机颜色
Color RandomColor() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dist(0, 255);
    
    return Color::RGB(dist(gen), dist(gen), dist(gen));
}


// 生成HSV颜色
Color HSVColor(double h, double s, double v) {
    h = fmod(h, 360.0);
    double c = v * s;
    double x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
    double m = v - c;
    
    double r = 0, g = 0, b = 0;
    
    if (0 <= h && h < 60) {
        r = c; g = x; b = 0;
    } else if (60 <= h && h < 120) {
        r = x; g = c; b = 0;
    } else if (120 <= h && h < 180) {
        r = 0; g = c; b = x;
    } else if (180 <= h && h < 240) {
        r = 0; g = x; b = c;
    } else if (240 <= h && h < 300) {
        r = x; g = 0; b = c;
    } else {
        r = c; g = 0; b = x;
    }
    
    return Color::RGB(
        static_cast<uint8_t>((r + m) * 255),
        static_cast<uint8_t>((g + m) * 255),
        static_cast<uint8_t>((b + m) * 255)
    );
}


// 生成随机莫兰迪色
Color RandomMutedColor() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> distH(0, 360);   // 色相
    static std::uniform_real_distribution<float> distS(0.2, 0.4); // 饱和度
    static std::uniform_real_distribution<float> distV(0.7, 0.9); // 明度

    float h = distH(gen);
    float s = distS(gen);
    float v = distV(gen);

    return HSVColor(h, s, v);
}

void PrintGrid(const std::vector<std::vector<Color>>& grid_colors)
{
    s32 rows = static_cast<s32>(grid_colors.size());
    s32 cols = static_cast<s32>(grid_colors[0].size());
    s32 cell_size = 4;

    // 创建网格布局
    Elements row_elements;
    for (int r = 0; r < rows; r++) {
        Elements col_elements;
        for (int c = 0; c < cols; c++) {
            // 创建单元格 - 使用空格字符构成等宽高的彩色格子
            auto cell = text(" ") | 
                        size(WIDTH, EQUAL, cell_size) | 
                        size(HEIGHT, EQUAL, cell_size) | 
                        bgcolor(grid_colors[r][c]);
            
            col_elements.push_back(cell);
        }
        row_elements.push_back(hbox(std::move(col_elements)));
    }
    
    // 组合所有网格行
    auto grid = vbox(std::move(row_elements));
    
    // 添加边框和背景以增强可视性
    auto document = 
        vbox({
            text("彩色网格展示") | bold | hcenter | color(Color::Cyan),
            separatorHeavy(),
            grid | border | center,
            filler(),
            text("行数: " + std::to_string(rows) + 
                 "  列数: " + std::to_string(cols)) | 
                 color(Color::GrayLight) | hcenter
        }) | border | bgcolor(Color::RGB(20, 20, 20));
    
    // 创建屏幕并渲染
    auto screen = Screen::Create(
        Dimension::Full(),       // 宽度
        Dimension::Fit(document) // 高度自适应内容
    );
    
    Render(screen, document);
    screen.Print();
}


int main() {    
    // 生成网格颜色 (彩虹色分布)
    // for (int r = 0; r < rows; r++) {
    //     for (int c = 0; c < cols; c++) {
    //         double hue = 360.0 * (c + r) / (cols + rows);
    //         grid_colors[r][c] = HSVColor(hue, 0.8, 0.9);
    //     }
    // }


    PackageGrid grid;

    // 根据当前时间初始化随机种子
    srand(time(nullptr));

    std::vector<std::tuple<s32, s32, s32>> rects;
    std::unordered_map<s32, Color> color_map;
    for (s32 i = 1; i <= 300; ++i)
    {
        int row = rand() % 5 + 1; // [1, 5]
        // int col = rand() % 5 + 1; // [1, 5]
        int col = row;
        rects.emplace_back(row, col, i);
        color_map[i] = RandomMutedColor();
    }

    GridRect global_rect(GridKey(0, 0), GridKey(300, 60));

    auto snapshot = grid.MakeSnapshot(); // 创建一个快照

    for (auto& rect : rects)
    {
        s32 height, width, value;
        std::tie(height, width, value) = rect;

        // 从快照中找到一个空闲区域
        auto free_rect = snapshot.GetOneFreeRect(global_rect, height, width);
        if (free_rect.IsValid())
        {
            snapshot.SetRect(free_rect, value);
            // std::cout << "[INFO] " << height << " " << width << " put to " << free_rect << std::endl;
        }
        else
        {
            std::cout << "[ERROR] " << height << " " << width << " can't put " << global_rect << std::endl;
        }
    }

    std::vector<std::vector<Color>> grid_colors(global_rect.Height(), std::vector<Color>(global_rect.Width()));
    for (s32 r = 0; r < global_rect.Height(); r++)
    {
        for (s32 c = 0; c < global_rect.Width(); c++)
        {
            if (snapshot.Get({r, c}))
            {
                grid_colors[r][c] = color_map[snapshot.Get({r, c})];
            }
            else
            {
                grid_colors[r][c] = Color::Black;
            }
        }
    }
    
    PrintGrid(grid_colors);
    
    return 0;
}
#endif



// #define TEST_MAKE_RECT

#ifdef TEST_MAKE_RECT
#include <algorithm>
#include <climits>

GridRect MakeRect(const std::vector<u32>& slots, u32 pkg_col)
{

    if (slots.empty())
    {
        return {};
    }

    if (pkg_col == 0)
    {
        return {};
    }

    std::vector<u32> sorted_slots(slots.begin(), slots.end());
    std::sort(sorted_slots.begin(), sorted_slots.end());

    GridKey top_left;
    s32 rect_height = 0;
    s32 rect_width = 0;

    s32 min_col = INT_MAX;
    s32 max_col = 0;
    s32 min_row = INT_MAX;
    s32 max_row = 0;

    for (auto slot : slots)
    {
        s32 row = slot / pkg_col;
        s32 col = slot % pkg_col;
        min_row = std::min(min_row, row);
        max_row = std::max(max_row, row);
        min_col = std::min(min_col, col);
        max_col = std::max(max_col, col);
    }

    top_left = GridKey{ min_row, min_col };
    rect_height = max_row - min_row + 1;
    rect_width = max_col - min_col + 1;

    if (slots.size() % rect_width != 0)
    {
        return {};
    }

    if (slots.size() != rect_height * rect_width)
    {
        return {};
    }

    for (s32 i = 0; i < sorted_slots.size(); i += rect_width)
    {
        if (i > 0 && sorted_slots[i] != sorted_slots[i - rect_width] + pkg_col)
        {
            return {};
        }
        for (s32 j = i; j < i + rect_width; ++j)
        {
            if (j > i && sorted_slots[j] != sorted_slots[j - 1] + 1)
            {
                return {};
            }
        }
    }

    return GridRect{ top_left, rect_height, rect_width };
}

int main()
{
    {
        u32 col1 = 7;
        std::vector<u32> v_rect1 = {
            14, 15,
            21, 22
        };

        auto rect1 = MakeRect(v_rect1, col1);
        std::cout << rect1 << std::endl;
    }

    {
        u32 col1 = 3;
        std::vector<u32> v_rect1 = {
            3, 4, 5,
            6, 7, 8
        };

        auto rect1 = MakeRect(v_rect1, col1);
        std::cout << rect1 << std::endl;
    }

    return 0;
}

#endif