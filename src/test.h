#include <iostream>
#include <cassert>

#include <vector>
#include "package_grid.h"

void TestGridKey()
{
    std::cout << "Testing GridKey..." << std::endl;

    // 测试构造函数和操作符
    GridKey key1(10, 20);
    GridKey key2(10, 21);
    GridKey key3(10, 100);

    // 测试 `==` 运算符
    assert(key1 == key1);  // 同一对象应该相等
    assert(key1 != key2);  // 不同的对象应该不相等

    // 测试 `+` 和 `+=` 运算符
    GridKey key4 = key1 + key2;
    assert(key4.Row() == 20 && key4.Col() == 41);  // Row = 10 + 10, Col = 20 + 21

    key1 += key2;
    assert(key1.Row() == 20 && key1.Col() == 41);  // Row = 10 + 10, Col = 20 + 21

    // 测试比较运算符
    GridKey key5(10, 20);
    GridKey key6(10, 21);
    assert(key5 < key6);  // key5 应该小于 key6
    assert(key5 <= key6);  // key5 应该小于等于 key6
    assert(key6 > key5);  // key6 应该大于 key5
    assert(key6 >= key5);  // key6 应该大于等于 key5

    std::cout << "GridKey tests passed!" << std::endl;
}

void TestGridRect()
{
    std::cout << "Testing GridRect..." << std::endl;

    GridKey topLeft(0, 0);
    GridRect rect(topLeft, 2, 3);

    // 测试基本属性
    assert(rect.TopLeft() == topLeft);  // TopLeft should be (0, 0)
    assert(rect.Height() == 2);         // Height should be 2
    assert(rect.Width() == 3);          // Width should be 3
    assert(rect.BottomRight() == GridKey(1, 2));  // BottomRight should be (1, 2)
    assert(rect.IsValid());             // Rect should be valid

    // 测试无效 GridRect
    GridRect invalidRect(topLeft, -1, 3);
    assert(!invalidRect.IsValid());  // Invalid rect with negative height

    std::cout << "GridRect tests passed!" << std::endl;
}

void TestPackageGridForLoop()
{
    std::cout << "Testing PackageGrid with for loop..." << std::endl;

    PackageGrid grid;

    // 设置一些数据
    grid.Set(GridKey(1, 1), 100);
    grid.Set(GridKey(2, 2), 200);
    grid.Set(GridKey(3, 3), 300);

    // 使用 for 循环遍历并验证数据
    for (auto it = grid.begin(); it != grid.end(); ++it)
    {
        const GridKey& key = it->first;
        u64 value = it->second;

        // 验证键值是否正确
        if (key.Row() == 1 && key.Col() == 1)
        {
            assert(value == 100);
        }
        else if (key.Row() == 2 && key.Col() == 2)
        {
            assert(value == 200);
        }
        else if (key.Row() == 3 && key.Col() == 3)
        {
            assert(value == 300);
        }
        else
        {
            std::cerr << "Unexpected key found: (" << key.Row() << ", " << key.Col() << ")" << std::endl;
            assert(false);  // 如果出现未预期的键，测试失败
        }
    }

    std::cout << "PackageGrid for loop tests passed!" << std::endl;
}


void TestPackageGrid()
{
    std::cout << "Testing PackageGrid..." << std::endl;

    PackageGrid grid;

    // 测试插入和访问
    GridKey key1(1, 1);
    grid.Set(key1, 100);
    assert(grid.Get(key1) == 100);

    // 测试更新值
    grid.Set(key1, 200);
    assert(grid.Get(key1) == 200);

    // 测试是否包含
    assert(grid.Contains(key1));

    // 测试删除
    grid.Remove(key1);
    assert(grid.Get(key1) == 0);
    assert(!grid.Contains(key1));

    // 测试快照
    PackageGrid snapshot = grid.MakeSnapshot();
    assert(snapshot.Get(key1) == 0);  // 快照应该保留原有值

    // 测试矩阵操作
    GridRect rect(key1, 3, 3);
    grid.SetRect(rect, 50);  // 设置矩阵区域的值
    assert(grid.Get(GridKey(1, 1)) == 50);
    assert(grid.Get(GridKey(2, 1)) == 50);

    // 测试矩阵区域删除
    s32 rm_count = grid.RemoveRect(rect);
    assert(rm_count == 9);  // 应该移除 3x3 区域的 9 个格子

    std::cout << "PackageGrid tests passed!" << std::endl;
}

void TestPackageGridRect()
{
    std::cout << "Testing PackageRect..." << std::endl;

    std::vector<GridRect> rects;
    rects.emplace_back(GridKey(0 ,0), 3, 3);
    rects.emplace_back(GridKey(3, 1), 1, 1);
    rects.emplace_back(GridKey(3, 0), 3, 1);
    rects.emplace_back(GridKey(1, 3), 1, 1);
    rects.emplace_back(GridKey(5, 2), GridKey(6, 3));

    PackageGrid grid;

    int idx = 10000;
    for (auto& rect : rects)
    {
        bool is_free = grid.RectIsFree(rect);
        assert(is_free);
        grid.SetRect(rect, idx ++);
    }

    for (auto& cell : grid)
    {
        std::cout << cell.first << " " << cell.second << std::endl;
    }

    auto free_rect = grid.GetOneFreeRect(GridRect(GridKey(2,1), GridKey(6,3)), 2, 2);
    std::cout << "free_rect: " << free_rect << std::endl;
    assert(free_rect == GridRect(GridKey(3, 2), GridKey(4, 3)));
    

    std::cout << "PackageRect tests passed!" << std::endl;
}

void TestFillPackageGridRect()
{
    std::cout << "Testing FillPackageGridRect..." << std::endl;

    PackageGrid grid;

    // 根据当前时间初始化随机种子
    srand(time(nullptr));

    std::vector<std::tuple<s32, s32, s32>> rects;
    for (s32 i = 1; i <= 4000; ++i)
    {
        int row = rand() % 5 + 1; // [1, 5]
        int col = rand() % 5 + 1; // [1, 5]
        rects.emplace_back(row, col, i * 10);
    }

    GridRect global_rect(GridKey(0, 0), GridKey(1000, 100));

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

    std::cout << snapshot.DebugShapeString() << std::endl;

    std::cout << "FillPackageGridRect tests passed!" << std::endl;
}


void TestFillPackageGridRect2()
{
    std::cout << "Testing FillPackageGridRect2..." << std::endl;

    PackageGrid grid;

    // 根据当前时间初始化随机种子
    srand(time(nullptr));

    std::vector<std::tuple<s32, s32, s32>> rects;
    for (s32 i = 1; i <= 100; ++i)
    {
        int row = rand() % 5 + 1; // [1, 5]
        int col = rand() % 5 + 1; // [1, 5]
        rects.emplace_back(row, col, i * 10);
    }

    GridRect global_rect(GridKey(0, 0), GridKey(100, 100));

    auto snapshot = grid.MakeSnapshot(); // 创建一个快照

    GridKey random_key = GridKey(rand() % 100, rand() % 100);
    std::cout << "random key: " << random_key << std::endl;

    for (auto& rect : rects)
    {
        s32 height, width, value;
        std::tie(height, width, value) = rect;

        // 从快照中找到一个空闲区域
        auto free_rect = snapshot.GetNearestFreeRect(global_rect, random_key, height, width);
        if (free_rect.IsValid())
        {
            snapshot.SetRect(free_rect, value);
            std::cout << "[INFO] " << height << " " << width << " put to " << free_rect << std::endl;
        }
        else
        {
            std::cout << "[ERROR] " << height << " " << width << " can't put " << global_rect << std::endl;
        }
    }

    std::cout << snapshot.DebugShapeString() << std::endl;

    std::cout << "FillPackageGridRect2 tests passed!" << std::endl;
}