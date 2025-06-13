
#include <iostream>


#include "comm_def.h"

#include "package_grid.h"

#include "test.h"


#ifdef _WIN32

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/dom/table.hpp"

#endif

#define TEST

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

int main() {
    int rows = 12;   // 网格行数
    int cols = 16;   // 网格列数
    int cell_size = 5; // 单元格大小（字符宽高）
    
    // 用于存储网格颜色
    std::vector<std::vector<Color>> grid_colors(rows, std::vector<Color>(cols));
    
    // 生成网格颜色 (彩虹色分布)
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            double hue = 360.0 * (c + r) / (cols + rows);
            grid_colors[r][c] = HSVColor(hue, 0.8, 0.9);
        }
    }
    
    // 创建网格布局
    auto create_grid = [&] {
        std::vector<Element> row_elements;
        
        for (int r = 0; r < rows; r++) {
            std::vector<Element> col_elements;
            
            for (int c = 0; c < cols; c++) {
                // 创建单元格 - 空字符组成固定尺寸的格子
                auto cell = text(" ") | size(WIDTH, EQUAL, cell_size) | 
                            size(HEIGHT, EQUAL, cell_size) | 
                            bgcolor(grid_colors[r][c]);
                
                col_elements.push_back(cell);
            }
            
            // 将一行单元格水平组合
            row_elements.push_back(hbox(std::move(col_elements)));
        }
        
        // 将所有行垂直组合
        return vbox(std::move(row_elements)) | border | center | bgcolor(Color::RGB(40, 40, 40));
    };
    
    // 创建交互组件
    auto grid_component = Renderer([&] {
        return create_grid();
    });
    
    // 添加键盘交互
    grid_component |= CatchEvent([&](Event event) {
        if (event == Event::Character('r') || event == Event::Character('R')) {
            // 重新生成随机颜色
            for (auto& row : grid_colors) {
                for (auto& color : row) {
                    color = RandomColor();
                }
            }
            return true;
        }
        if (event == Event::Character('g') || event == Event::Character('G')) {
            // 重新生成渐变颜色
            for (int r = 0; r < rows; r++) {
                for (int c = 0; c < cols; c++) {
                    double hue = 360.0 * (c + r) / (cols + rows);
                    grid_colors[r][c] = HSVColor(hue, 0.8, 0.9);
                }
            }
            return true;
        }
        if (event == Event::Character('+') && cell_size < 15) {
            // 增大单元格
            cell_size++;
            return true;
        }
        if (event == Event::Character('-') && cell_size > 1) {
            // 减小单元格
            cell_size--;
            return true;
        }
        if (event == Event::Escape) {
            // 退出程序
            return true;
        }
        return false;
    });
    
    // 添加标题和说明
    auto title = text("彩色网格可视化") | bold | hcenter | color(Color::Cyan);
    auto instructions = vbox({
        hbox(text("行: "), text(std::to_string(rows)) | bold | color(Color::Green)),
        hbox(text("列: "), text(std::to_string(cols)) | bold | color(Color::Green)),
        separator(),
        text("控制指令:") | color(Color::Yellow),
        hbox(text(" + / - "), text(" - 增加/减少单元格大小")),
        hbox(text("   R   "), text(" - 随机颜色")),
        hbox(text("   G   "), text(" - 渐变颜色")),
        hbox(text(" ESC   "), text(" - 退出程序"))
    }) | border | bgcolor(Color::RGB(60, 60, 60));
    
    // 完整布局
    auto layout = Container::Vertical({grid_component});
    auto renderer = Renderer(layout, [&] {
        return vbox({
            title,
            separator(),
            hbox({
                grid_component->Render() | flex,
                instructions | size(WIDTH, EQUAL, 30)
            }),
            filler()
        }) | border | bgcolor(Color::GrayDark);
    });
    
    // 运行
    auto screen = ScreenInteractive::Fullscreen();
    screen.Loop(renderer);
    
    return 0;
}
#endif