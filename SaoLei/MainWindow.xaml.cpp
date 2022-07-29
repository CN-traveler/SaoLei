#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace winrt::Windows::UI;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Media;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::SaoLei::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
    }

    int32_t MainWindow::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainWindow::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    // 地图宽度
    int map_width;
    // 地图高度
    int map_height;
    // 地图雷数
    int map_num;
    // 地图数组 -1 雷 / 0-9 周围雷数
    int** map;
    // 随机交换地图数组的值到指定坐标
    void map_swap(int x, int y) {
        int rx = rand() % map_width;
        int ry = rand() % map_height;
        int t = map[y][x];
        map[y][x] = map[ry][rx];
        map[ry][rx] = t;
        return;
    }
    // 开始游戏
    void MainWindow::start_game()
    {
        // 地图格数
        int map_sum = map_width * map_height;
        // 地图雷数异常处理
        if (map_num > map_sum) {
            map_num = map_sum;
        }
        if (map_num < 1) {
            map_num = 1;
        }
        // 地图数组初始化
        if (map) {
            delete map;
        }
        map = new int* [map_height];
        // 初始化地图数组
        for (int i = 0; i < map_height; i++) {
            map[i] = new int[map_width];
            for (int j = 0; j < map_width; j++) {
                if (i * map_height + j < map_num) {
                    map[i][j] = -1;
                }
                else {
                    map[i][j] = 0;
                }
            }
        }
        // 随机打乱地图数组
        for (int i = 0; i < map_height; i++) {
            for (int j = 0; j < map_width; j++) {
                map_swap(j, i);
            }
        }
        // 清理地图
        map_panel().Children().Clear();
        // 生成地图
        for (int i = 0; i < map_height; i++) {
            StackPanel rowPanel = StackPanel();
            rowPanel.Orientation(Orientation::Horizontal);
            for (int j = 0; j < map_width; j++) {
                Button button = Button();
                button.Width(40);
                button.Height(40);
                if (map[i][j] >= 0) {
                    if (i > 0) {
                        map[i][j] += (map[i - 1][j] == -1) ? 1 : 0;
                        if (j > 0) {
                            map[i][j] += (map[i - 1][j - 1] == -1) ? 1 : 0;
                            map[i][j] += (map[i][j - 1] == -1) ? 1 : 0;
                        }
                        if (j < map_width - 1) {
                            map[i][j] += (map[i - 1][j + 1] == -1) ? 1 : 0;
                            map[i][j] += (map[i][j + 1] == -1) ? 1 : 0;
                        }
                    }
                    else {
                        if (j > 0) {
                            map[i][j] += (map[i][j - 1] == -1) ? 1 : 0;
                        }
                        if (j < map_width - 1) {
                            map[i][j] += (map[i][j + 1] == -1) ? 1 : 0;
                        }
                    }
                    if (i < map_height - 1) {
                        map[i][j] += (map[i + 1][j] == -1) ? 1 : 0;
                        if (j > 0) {
                            map[i][j] += (map[i + 1][j - 1] == -1) ? 1 : 0;
                        }
                        if (j < map_width - 1) {
                            map[i][j] += (map[i + 1][j + 1] == -1) ? 1 : 0;
                        }
                    }
                }
                //button.Content(box_value(map[i][j]));
                button.Name(to_hstring(i * map_width + j));
                Color ccc = Color();
                ccc.A = 127;
                ccc.R = map_width * 5;
                ccc.G = map_height * 10;
                ccc.B = 204;
                SolidColorBrush cccSolidColorBrush = SolidColorBrush(ccc);
                button.Background(cccSolidColorBrush);
                button.Click({ this, &MainWindow::game_step });
                rowPanel.Children().Append(button);
            }
            map_panel().Children().Append(rowPanel);
        }
        // 结束
        return;
    }
    // 走游戏步
    void MainWindow::game_step(IInspectable const& sender, RoutedEventArgs const& e)
    {
        // 获取点击对象
        Button button = sender.as<Button>();
        // 获取坐标
        int index = stoi(to_string(button.Name()));
        int i = index / map_width;
        int j = index % map_width;
        // 设置点击背景
        Color fff = Color();
        fff.A = 255;
        fff.R = 255;
        fff.G = 255;
        fff.B = 255;
        SolidColorBrush fffSolidColorBrush = SolidColorBrush(fff);
        // 检测是否已经点击
        if (button.Background().as< SolidColorBrush>().Color() == fff) {
            return;
        }
        // 检测是否为雷
        if (map[i][j] != -1) {
            // 显示点击后的背景
            button.Background(fffSolidColorBrush);
            // 显示数值
            if (map[i][j] != 0) {
                button.Content(box_value(map[i][j]));
            }
        }
        else {
            // 重新开始
            start_game();
        }
        // 批量打开
        if (map[i][j] == 0) {
            if (i > 0) {
                game_step(map_panel().FindName(to_hstring((i - 1) * map_width + j)), NULL);
                if (j > 0) {
                    game_step(map_panel().FindName(to_hstring((i - 1) * map_width + j - 1)), NULL);
                    game_step(map_panel().FindName(to_hstring(i * map_width + j - 1)), NULL);
                }
                if (j < map_width - 1) {
                    game_step(map_panel().FindName(to_hstring((i - 1) * map_width + j + 1)), NULL);
                    game_step(map_panel().FindName(to_hstring(i * map_width + j + 1)), NULL);
                }
            }
            else {
                if (j > 0) {
                    game_step(map_panel().FindName(to_hstring(i * map_width + j - 1)), NULL);
                }
                if (j < map_width - 1) {
                    game_step(map_panel().FindName(to_hstring(i * map_width + j + 1)), NULL);
                }
            }
            if (i < map_height - 1) {
                game_step(map_panel().FindName(to_hstring((i + 1) * map_width + j)), NULL);
                if (j > 0) {
                    game_step(map_panel().FindName(to_hstring((i + 1) * map_width + j - 1)), NULL);
                }
                if (j < map_width - 1) {
                    game_step(map_panel().FindName(to_hstring((i + 1) * map_width + j + 1)), NULL);
                }
            }
        }
        // 结束
        return;
    }
    // 修改难度
    void MainWindow::change_difficulty(NavigationView const& sender, NavigationViewSelectionChangedEventArgs const& args)
    {
        // 获取对象
        NavigationView navigationView = sender.as<NavigationView>();
        NavigationViewItem navigationViewItem = navigationView.SelectedItem().as<NavigationViewItem>();
        // 获取难度
        hstring difficulty = to_hstring(navigationViewItem.Name());
        // 难度处理
        if (difficulty == L"custom") {
            // 显示自定义配置
            visibleFooterPane(NULL, NULL);
            // 获取自定义难度值
            map_width = width().Value();
            map_height = height().Value();
            map_num = num().Value();
        }
        else {
            //隐藏自定义配置
            collapsedFooterPane(NULL, NULL);
            // 获取对应的难度值
            if (difficulty == L"easy") {
                map_width = 9;
                map_height = 9;
                map_num = 10;
            }
            else if (difficulty == L"normal") {
                map_width = 16;
                map_height = 16;
                map_num = 40;
            }
            else if (difficulty == L"difficulty") {
                map_width = 30;
                map_height = 16;
                map_num = 99;
            }
        }
        // 开始游戏
        start_game();
    }
    // 自定义难度值修改 监听
    void MainWindow::custom_change(IInspectable const& sender, Primitives::RangeBaseValueChangedEventArgs const& e)
    {
        // 判断是否完成初始化
        if (width() != NULL && height() != NULL && num() != NULL) {
            // 获取自定义难度值
            map_width = width().Value();
            map_height = height().Value();
            num().Maximum(map_width * map_height);
            map_num = num().Value();
            // 开始游戏
            start_game();
        }
    }
    // 隐藏自定义配置
    void MainWindow::collapsedFooterPane(NavigationView const& sender, IInspectable const& args)
    {
        footerPanel().Visibility(Visibility::Collapsed);
    }
    // 显示自定义配置
    void MainWindow::visibleFooterPane(NavigationView const& sender, IInspectable const& args)
    {
        footerPanel().Visibility(Visibility::Visible);
    }
}
