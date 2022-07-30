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
    int mapWidth;
    // 地图高度
    int mapHeight;
    // 地图雷数
    int mapNum;
    // 地图数组 -1 雷 / 0-8 周围雷数
    int** map;
    // 随机交换地图数组的值到指定坐标
    void mapSwap(int x, int y)
    {
        int rx = rand() % mapWidth;
        int ry = rand() % mapHeight;
        int t = map[y][x];
        map[y][x] = map[ry][rx];
        map[ry][rx] = t;
        return;
    }
    // 开始游戏
    void MainWindow::startGame()
    {
        // 地图格数
        int mapSum = mapWidth * mapHeight;
        // 地图雷数异常处理
        if (mapNum > mapSum)
        {
            mapNum = mapSum;
        }
        if (mapNum < 1)
        {
            mapNum = 1;
        }
        // 地图数组初始化
        if (map != NULL)
        {
            delete map;
        }
        map = new int* [mapHeight];
        // 初始化地图数组
        for (int i = 0; i < mapHeight; i++)
        {
            map[i] = new int[mapWidth];
            for (int j = 0; j < mapWidth; j++)
            {
                if (i * mapHeight + j < mapNum)
                {
                    map[i][j] = -1;
                }
                else
                {
                    map[i][j] = 0;
                }
            }
        }
        // 随机打乱地图数组
        for (int i = 0; i < mapHeight; i++)
        {
            for (int j = 0; j < mapWidth; j++)
            {
                mapSwap(j, i);
            }
        }
        // 清理地图
        mapPanel().Children().Clear();
        // 生成地图
        for (int i = 0; i < mapHeight; i++)
        {
            StackPanel rowPanel = StackPanel();
            rowPanel.Orientation(Orientation::Horizontal);
            for (int j = 0; j < mapWidth; j++)
            {
                Button button = Button();
                button.Width(40);
                button.Height(40);
                if (map[i][j] >= 0)
                {
                    if (i > 0)
                    {
                        map[i][j] += (map[i - 1][j] == -1) ? 1 : 0;
                        if (j > 0)
                        {
                            map[i][j] += (map[i - 1][j - 1] == -1) ? 1 : 0;
                            map[i][j] += (map[i][j - 1] == -1) ? 1 : 0;
                        }
                        if (j < mapWidth - 1)
                        {
                            map[i][j] += (map[i - 1][j + 1] == -1) ? 1 : 0;
                            map[i][j] += (map[i][j + 1] == -1) ? 1 : 0;
                        }
                    }
                    else 
                    {
                        if (j > 0)
                        {
                            map[i][j] += (map[i][j - 1] == -1) ? 1 : 0;
                        }
                        if (j < mapWidth - 1)
                        {
                            map[i][j] += (map[i][j + 1] == -1) ? 1 : 0;
                        }
                    }
                    if (i < mapHeight - 1)
                    {
                        map[i][j] += (map[i + 1][j] == -1) ? 1 : 0;
                        if (j > 0)
                        {
                            map[i][j] += (map[i + 1][j - 1] == -1) ? 1 : 0;
                        }
                        if (j < mapWidth - 1)
                        {
                            map[i][j] += (map[i + 1][j + 1] == -1) ? 1 : 0;
                        }
                    }
                }
                button.Name(to_hstring(i * mapWidth + j));
                Color ccc = Color();
                ccc.A = 127;
                ccc.R = mapWidth * 5;
                ccc.G = mapHeight * 10;
                ccc.B = 204;
                SolidColorBrush cccSolidColorBrush = SolidColorBrush(ccc);
                button.Background(cccSolidColorBrush);
                button.Click({ this, &MainWindow::gameStep });
                rowPanel.Children().Append(button);
            }
            mapPanel().Children().Append(rowPanel);
        }
        // 结束
        return;
    }
    // 走游戏步
    void MainWindow::gameStep(IInspectable const& sender, RoutedEventArgs const& e)
    {
        // 获取点击对象
        Button button = sender.as<Button>();
        // 获取坐标
        int index = stoi(to_string(button.Name()));
        int i = index / mapWidth;
        int j = index % mapWidth;
        // 设置点击背景
        Color fff = Color();
        fff.A = 255;
        fff.R = 255;
        fff.G = 255;
        fff.B = 255;
        SolidColorBrush fffSolidColorBrush = SolidColorBrush(fff);
        // 检测是否已经点击
        if (button.Background().as< SolidColorBrush>().Color() == fff)
        {
            return;
        }
        // 检测是否为雷
        if (map[i][j] != -1)
        {
            // 显示点击后的背景
            button.Background(fffSolidColorBrush);
            // 显示数值
            if (map[i][j] != 0)
            {
                button.Content(box_value(map[i][j]));
            }
        }
        else
        {
            // 重新开始
            startGame();
        }
        // 批量打开
        if (map[i][j] == 0)
        {
            if (i > 0)
            {
                gameStep(mapPanel().FindName(to_hstring((i - 1) * mapWidth + j)), NULL);
                if (j > 0)
                {
                    gameStep(mapPanel().FindName(to_hstring((i - 1) * mapWidth + j - 1)), NULL);
                    gameStep(mapPanel().FindName(to_hstring(i * mapWidth + j - 1)), NULL);
                }
                if (j < mapWidth - 1)
                {
                    gameStep(mapPanel().FindName(to_hstring((i - 1) * mapWidth + j + 1)), NULL);
                    gameStep(mapPanel().FindName(to_hstring(i * mapWidth + j + 1)), NULL);
                }
            }
            else
            {
                if (j > 0)
                {
                    gameStep(mapPanel().FindName(to_hstring(i * mapWidth + j - 1)), NULL);
                }
                if (j < mapWidth - 1)
                {
                    gameStep(mapPanel().FindName(to_hstring(i * mapWidth + j + 1)), NULL);
                }
            }
            if (i < mapHeight - 1)
            {
                gameStep(mapPanel().FindName(to_hstring((i + 1) * mapWidth + j)), NULL);
                if (j > 0)
                {
                    gameStep(mapPanel().FindName(to_hstring((i + 1) * mapWidth + j - 1)), NULL);
                }
                if (j < mapWidth - 1)
                {
                    gameStep(mapPanel().FindName(to_hstring((i + 1) * mapWidth + j + 1)), NULL);
                }
            }
        }
        // 结束
        return;
    }
    // 修改难度
    void MainWindow::changeDifficulty(NavigationView const& sender, NavigationViewSelectionChangedEventArgs const& args)
    {
        // 获取对象
        NavigationView navigationView = sender.as<NavigationView>();
        NavigationViewItem navigationViewItem = navigationView.SelectedItem().as<NavigationViewItem>();
        // 获取难度
        hstring difficulty = to_hstring(navigationViewItem.Name());
        // 难度处理
        if (difficulty == L"custom")
        {
            // 显示自定义配置
            visibleFooterPane(NULL, NULL);
            // 获取自定义难度值
            mapWidth = width().Value();
            mapHeight = height().Value();
            mapNum = num().Value();
        }
        else
        {
            //隐藏自定义配置
            collapsedFooterPane(NULL, NULL);
            // 获取对应的难度值
            if (difficulty == L"easy")
            {
                mapWidth = 9;
                mapHeight = 9;
                mapNum = 10;
            }
            else if (difficulty == L"normal")
            {
                mapWidth = 16;
                mapHeight = 16;
                mapNum = 40;
            }
            else if (difficulty == L"difficulty")
            {
                mapWidth = 30;
                mapHeight = 16;
                mapNum = 99;
            }
        }
        // 开始游戏
        startGame();
    }
    // 自定义难度值修改 监听
    void MainWindow::customChange(IInspectable const& sender, Primitives::RangeBaseValueChangedEventArgs const& e)
    {
        // 判断是否完成初始化
        if (width() != NULL && height() != NULL && num() != NULL)
        {
            // 获取自定义难度值
            mapWidth = width().Value();
            mapHeight = height().Value();
            num().Maximum(mapWidth * mapHeight);
            mapNum = num().Value();
            // 开始游戏
            startGame();
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
