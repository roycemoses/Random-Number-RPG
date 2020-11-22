#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <string>

#include "header/Player.h"
#include "header/Enemy.h"
#include "header/EnemyFactory.h"
#include "header/battle/Battle.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Player* player;
    Battle* battle;
    EnemyFactory ef;
    std::vector<Enemy*> area_enemies;

    void update_main_menu_player_stats();

private slots:

    // functions
    void initializePlayer();
    void enterArea();
    void battleEnemy();

    // switch between menu pages
    void switchToMainMenu();
    void switchToBattleMenu();
};

#endif // MAINWINDOW_H
