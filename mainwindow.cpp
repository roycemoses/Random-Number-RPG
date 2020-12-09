#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    const QSize BUTTON_SIZE = QSize(100, 10);
    ui->rollButton->setMinimumSize(BUTTON_SIZE);

    srand(time(0));
    // connections
    connect(ui->startButton, SIGNAL(clicked(bool)), this, SLOT(initializePlayer()));
    connect(ui->battleButton, SIGNAL(clicked(bool)), this, SLOT(enterArea()));
    connect(ui->rollButton, SIGNAL(clicked(bool)), this, SLOT(nextTurn()));
    connect(ui->returnToMainMenuButton, SIGNAL(clicked(bool)), this, SLOT(endArea()));
    connect(ui->changeEquipmentButton, SIGNAL(clicked(bool)), this, SLOT(switchToEquipmentMenu()));
    connect(ui->equipmentReturnToMainMenuButton, SIGNAL(clicked(bool)), this, SLOT(switchToMainMenu()));
    connect(ui->goToShopButton, SIGNAL(clicked(bool)), this, SLOT(switchToShopMenu()));
    connect(ui->shopReturnToMainMenuButton, SIGNAL(clicked(bool)), this, SLOT(switchToMainMenu()));
    connect(ui->nextBattleButton, SIGNAL(clicked(bool)), this, SLOT(nextBattle()));

    // battle
    connect(ui->battleMenuCards, SIGNAL(currentTextChanged(QString)), this, SLOT(updateBattleMenuCurrentlySelectedCard()));
    connect(ui->battleMenuUseCard, SIGNAL(clicked(bool)), this, SLOT(useCard()));

    // equipment
    // switch inventory item type
    connect(ui->viewInventoryWeaponsRadioButton, SIGNAL(clicked(bool)), this, SLOT(selectInventoryItemType()));
    connect(ui->viewInventoryArmorRadioButton, SIGNAL(clicked(bool)), this, SLOT(selectInventoryItemType()));
    connect(ui->viewInventoryBootsRadioButton, SIGNAL(clicked(bool)), this, SLOT(selectInventoryItemType()));
    connect(ui->viewInventoryCardsRadioButton, SIGNAL(clicked(bool)), this, SLOT(selectInventoryItemType()));

    connect(ui->equipmentEquipButton, SIGNAL(clicked(bool)), this, SLOT(equipItem()));
    connect(ui->equipmentMenuCurrentCards, SIGNAL(currentTextChanged(QString)), this, SLOT(updateEquipmentMenuCurrentlySelectedCard()));

    // shop
    // switch shop item type
    connect(ui->shopInventoryWeaponsRadioButton, SIGNAL(clicked(bool)), this, SLOT(selectShopItemType()));
    connect(ui->shopInventoryArmorRadioButton, SIGNAL(clicked(bool)), this, SLOT(selectShopItemType()));
    connect(ui->shopInventoryBootsRadioButton, SIGNAL(clicked(bool)), this, SLOT(selectShopItemType()));
    connect(ui->shopInventoryCardsRadioButton, SIGNAL(clicked(bool)), this, SLOT(selectShopItemType()));

    connect(ui->shopList, SIGNAL(currentTextChanged(QString)), this, SLOT(updateShopMenuCurrentlySelectedItem()));
    connect(ui->shopPurchaseButton, SIGNAL(clicked(bool)), this, SLOT(purchaseItem()));
}

MainWindow::~MainWindow()
{
    if (player != nullptr)
        delete player;
    if (battle != nullptr)
        delete battle;
    delete ui;
}

void MainWindow::updateMainMenuPlayerStats()
{
    ui->main_menu_name->setText("Name: " + QString::fromStdString(player->getName()));
    ui->main_menu_level->setText("Level: " + QString::number(player->getLevel()));
    ui->main_menu_exp->setText("EXP: " + QString::number(player->getEXP()) + '/' + QString::number(player->getMaxEXP()));
    ui->main_menu_health->setText("Health: " + QString::number(player->combatStats->HP) + '/' + QString::number(player->combatStats->maxHP));
    ui->main_menu_attack->setText("ATK: " + QString::number(player->combatStats->ATK));
    ui->main_menu_defense->setText("DEF: " + QString::number(player->combatStats->DEF));

    Inventory equipped = player->getEquipped();
    if (!equipped.getWeapons().empty())
        ui->mainMenuWeaponLabel->setText("Weapon: " + QString::fromStdString(equipped.getWeapons().at(0)->getName()));
    else
        ui->mainMenuWeaponLabel->setText("Weapon: <None>");

    if (!equipped.getArmor().empty())
        ui->mainMenuArmorLabel->setText("Armor: " + QString::fromStdString(equipped.getArmor().at(0)->getName()));
    else
        ui->mainMenuArmorLabel->setText("Armor: <None>");

    std::vector<Card*> cards = equipped.getCards();
    if (!cards.empty())
    {
        for (unsigned i = 0; i < cards.size(); ++i)
        {
            QString name = QString::fromStdString(cards[i]->getName());
            if (i == 0) { ui->mainMenuCard1->setText("Card 1: " + name); }
            else if (i == 1) { ui->mainMenuCard2->setText("Card 2: " + name); }
            else if (i == 2) { ui->mainMenuCard3->setText("Card 3: " + name); }
            else if (i == 3) { ui->mainMenuCard4->setText("Card 4: " + name); }
        }
    }
    else
    {
        ui->mainMenuCard1->setText("Card 1: <None>");
        ui->mainMenuCard2->setText("Card 2: <None>");
        ui->mainMenuCard3->setText("Card 3: <None>");
        ui->mainMenuCard4->setText("Card 4: <None>");
    }
}

void MainWindow::updateBattleMenuPlayerStats()
{
    QString name = QString::fromStdString(player->getName());
    QString hp = QString::number(player->combatStats->HP);
    QString maxHP = QString::number(player->combatStats->maxHP);
    QString level = QString::number(player->getLevel());
    QString exp = QString::number(player->getEXP());
    QString maxEXP = QString::number(player->getMaxEXP());
    QString atk = QString::number(player->combatStats->ATK);
    QString def = QString::number(player->combatStats->DEF);

    ui->battleMenuCards->clear();
    std::vector<Card*> cards = player->getEquipped().getCards();
    for (unsigned i = 0; i < cards.size(); ++i)
        ui->battleMenuCards->addItem(QString::fromStdString(cards[i]->getName()));

    ui->battleMenuPlayerNameAndLevel->setText(name + " (Level " + level + ")");
    ui->battleMenuPlayerHP->setText("Health: " + hp + '/' + maxHP);
    ui->battleMenuPlayerEXP->setText("EXP: " + exp + '/' + maxEXP);
    ui->battleMenuPlayerATK->setText("ATK: " + atk);
    ui->battleMenuPlayerDEF->setText("DEF: " + def);

}

void MainWindow::updateBattleMenuCurrentlySelectedCard()
{
    std::string cardName = ui->battleMenuCards->currentText().toStdString();
    if (!cardName.empty())
    {
        Card* card = player->getInventory().getCard(cardName);
        ui->battleMenuCardDescription->setPlainText(ui->battleMenuCards->currentText() + ": " + QString::fromStdString(card->getDescription()));
    }
}

void MainWindow::updateBattleMenuEnemyStats()
{
    QString name = QString::fromStdString(currentEnemy->getName());
    QString hp = QString::number(currentEnemy->combatStats->HP);
    QString maxHP = QString::number(currentEnemy->combatStats->maxHP);
    QString level = QString::number(currentEnemy->getLevel());
    QString atk = QString::number(currentEnemy->combatStats->ATK);
    QString def = QString::number(currentEnemy->combatStats->DEF);
    QString desc = QString::fromStdString(currentEnemy->getDescription());

    ui->battleMenuEnemyNameAndLevel->setText(name + " (Level " + level + ")");
    ui->battleMenuEnemyHP->setText("Health: " + hp + '/' + maxHP);
    ui->battleMenuEnemyATK->setText("ATK: " + atk);
    ui->battleMenuEnemyDEF->setText("DEF: " + def);
    ui->battleMenuEnemyDescription->setPlainText(desc);
}

void MainWindow::updateShopMenuPlayerStats()
{
    ui->shopMenuName->setText("Name: " + QString::fromStdString(player->getName()));
    ui->shopMenuLevel->setText("Level: " + QString::number(player->getLevel()));
    ui->shopMenuEXP->setText("EXP: " + QString::number(player->getEXP()) + '/' + QString::number(player->getMaxEXP()));
    ui->shopMenuHP->setText("Health: " + QString::number(player->combatStats->HP) + '/' + QString::number(player->combatStats->maxHP));
    ui->shopMenuATK->setText("ATK: " + QString::number(player->combatStats->ATK));
    ui->shopMenuDEF->setText("DEF: " + QString::number(player->combatStats->DEF));
    ui->shopMenuCurrentGold->setText("Current Gold: " + QString::number(player->getGold()));

    Inventory equipped = player->getEquipped();
    if (!equipped.getWeapons().empty())
        ui->shopMenuCurrentWeapon->setText("Weapon: " + QString::fromStdString(equipped.getWeapons().at(0)->getName()));
    else
        ui->shopMenuCurrentWeapon->setText("Weapon: <None>");

    if (!equipped.getArmor().empty())
        ui->shopMenuCurrentArmor->setText("Armor: " + QString::fromStdString(equipped.getArmor().at(0)->getName()));
    else
        ui->shopMenuCurrentArmor->setText("Armor: <None>");

    ui->shopMenuCards->clear();
    std::vector<Card*> cards = player->getInventory().getCards();
    if (!cards.empty())
    {
        for (unsigned i = 0; i < cards.size(); ++i)
        {
            QString name = QString::fromStdString(cards[i]->getName());
            ui->shopMenuCards->addItem(name);
        }
    }
}

void MainWindow::updateShopMenuInventory()
{
    ui->shopList->clear();

    if (ui->shopInventoryCardsRadioButton->isChecked())
    {
        std::vector<Card*> cards = shop.getShopInventory().getCards();
        for (unsigned i = 0; i < cards.size(); ++i)
        {
            QString name = QString::fromStdString(cards[i]->getName());
            ui->shopList->addItem(name);
        }
    }
    else
    {
        std::vector<Item*> items;
        if (ui->shopInventoryWeaponsRadioButton->isChecked())
            items = shop.getShopInventory().getWeapons();
        else if (ui->shopInventoryArmorRadioButton->isChecked())
            items = shop.getShopInventory().getArmor();
        for (unsigned i = 0; i < items.size(); ++i)
            ui->shopList->addItem(QString::fromStdString(items.at(i)->getName()));
    }
}

void MainWindow::updateShopMenuCurrentlySelectedItem()
{
    ui->shopMenuCurrentItemStats->clear();
    std::string currentItem = ui->shopList->currentText().toStdString();
    if (!currentItem.empty())
    {
        if (ui->shopInventoryCardsRadioButton->isChecked())
        {
            Card* card = shop.getShopInventory().getCard(currentItem);
            ui->shopMenuCurrentItemStats->setPlainText(QString::fromStdString(card->getDescription()));
        }
        else
        {
            Item* item = shop.getShopInventory().getItem(currentItem);
            QString stats = "";
            if (item->combatStats->ATK != 0) { stats += "ATK: " + QString::number(item->combatStats->ATK) + '\n'; }
            if (item->combatStats->DEF != 0) { stats += "DEF: " + QString::number(item->combatStats->DEF) + '\n'; }
            if (item->combatStats->HP != 0) { stats += "HP: " + QString::number(item->combatStats->HP) + '\n'; }
            ui->shopMenuCurrentItemStats->setPlainText(stats);
        }
    }
}

void MainWindow::updateEquipmentMenuPlayerStats()
{
    ui->equipmentMenuName->setText("Name: " + QString::fromStdString(player->getName()));
    ui->equipmentMenuLevel->setText("Level: " + QString::number(player->getLevel()));
    ui->equipmentMenuEXP->setText("EXP: " + QString::number(player->getEXP()) + '/' + QString::number(player->getMaxEXP()));
    ui->equipmentMenuHP->setText("Health: " + QString::number(player->combatStats->HP) + '/' + QString::number(player->combatStats->maxHP));
    ui->equipmentMenuATK->setText("ATK: " + QString::number(player->combatStats->ATK));
    ui->equipmentMenuDEF->setText("DEF: " + QString::number(player->combatStats->DEF));

    Inventory equipped = player->getEquipped();
    if (!equipped.getWeapons().empty())
        ui->equipmentMenuCurrentWeapon->setText("Weapon: " + QString::fromStdString(equipped.getWeapons().at(0)->getName()));
    else
        ui->equipmentMenuCurrentWeapon->setText("Weapon: <None>");

    if (!equipped.getArmor().empty())
        ui->equipmentMenuCurrentArmor->setText("Armor: " + QString::fromStdString(equipped.getArmor().at(0)->getName()));
    else
        ui->equipmentMenuCurrentArmor->setText("Armor: <None>");

    ui->equipmentMenuCurrentCards->clear();
    std::vector<Card*> cards = equipped.getCards();
    if (!cards.empty())
    {
        for (unsigned i = 0; i < cards.size(); ++i)
        {
            QString name = QString::fromStdString(cards[i]->getName());
            ui->equipmentMenuCurrentCards->addItem(name);
        }
    }

    ui->equipmentList->clear();
    Inventory inventory = player->getInventory();    
    if (ui->viewInventoryCardsRadioButton->isChecked())
    {
        std::vector<Card*> cards = inventory.getCards();
        if (cards.empty())
            ui->equipmentList->addItem("No items found");
        else
            for (unsigned i = 0; i < cards.size(); ++i)
                ui->equipmentList->addItem(QString::fromStdString(cards.at(i)->getName()));
    }
    else
    {
        std::vector<Item*> items;
        if (ui->viewInventoryWeaponsRadioButton->isChecked())
            items = inventory.getWeapons();
        else if (ui->viewInventoryArmorRadioButton->isChecked())
            items = inventory.getArmor();

        if (items.empty())
            ui->equipmentList->addItem("No items found");
        else
            for (unsigned i = 0; i < items.size(); ++i)
                ui->equipmentList->addItem(QString::fromStdString(items.at(i)->getName()));
    }

}

void MainWindow::updateEquipmentMenuCurrentlySelectedCard()
{
    std::string cardName = ui->equipmentMenuCurrentCards->currentText().toStdString();
    if (!cardName.empty())
    {
        Card* card = player->getInventory().getCard(cardName);
        ui->equipmentMenuCardDescription->setPlainText(QString::fromStdString(card->getDescription()));
    }
}

/*************************************************************************/

void MainWindow::initializePlayer()
{
    std::string name = ui->enter_name_lineedit->text().toStdString();
    if (name.empty())
        name = "Hero";

    player = new Player(name);
    player->combatStats->ATK = 50;
    player->combatStats->DEF = 50;
//    player->addWeaponToInventory("Wooden Sword", weaponFactory);
//    player->addWeaponToInventory("Stone Sword", weaponFactory);
//    player->addArmorToInventory("Leather Armor", armorFactory);
//    player->equipWeapon("Wooden Sword");
//    player->equipArmor("Leather Armor");

    player->addCardToInventory("Enhance ATK", cardFactory);
    player->equipCard("Enhance ATK");
    player->addCardToInventory("Enhance DEF", cardFactory);
    player->equipCard("Enhance DEF");
    player->addCardToInventory("Big Heal", cardFactory);
    player->equipCard("Big Heal");
    player->addCardToInventory("Deal Damage", cardFactory);
    player->equipCard("Deal Damage");

    player->setGold(5000);

    updateMainMenuPlayerStats();
    updateShopMenuPlayerStats();
    updateEquipmentMenuPlayerStats();
    updateShopMenuInventory();
    switchToMainMenu();
}

void MainWindow::enterArea()
{
    areaEnemiesCount = 0;
    for (int i = 1; i <= 10; ++i)
    {
        int temp = rand() % 3 + 1;
        if (temp == 1)
            area_enemies.push_back(ef.createEnemy("Green Slime"));
        if (temp == 2)
            area_enemies.push_back(ef.createEnemy("Red Slime"));
        if (temp == 3)
            area_enemies.push_back(ef.createEnemy("Goblin"));
    }
    initializeBattleWithEnemy();

    ui->enemiesDefeatedLabel->setText("Enemies Defeated: " + QString::number(areaEnemiesCount) + "/10");
    ui->battleMenuBattleResult->clear();
    ui->battleMenuRollDifference->clear();
    ui->battleMenuTurnResult->clear();
    ui->battleMenuEnemyRoll->clear();
    ui->battleMenuPlayerRoll->clear();
    switchToBattleMenu();
}

void MainWindow::nextBattle()
{
    ++areaEnemiesCount;
    ui->enemiesDefeatedLabel->setText("Enemies Defeated: " + QString::number(areaEnemiesCount) + "/10");
    ui->battleMenuBattleResult->clear();
    ui->battleMenuRollDifference->clear();
    ui->battleMenuTurnResult->clear();
    ui->battleMenuEnemyRoll->clear();
    ui->battleMenuPlayerRoll->clear();
    area_enemies.pop_back();

    if (area_enemies.size() > 0)
        initializeBattleWithEnemy();
    else
    {
        ui->battleMenuBattleResult->setText("You defeated all ten enemies!");
        ui->returnToMainMenuButton->setEnabled(true);
    }
    ui->nextBattleButton->setEnabled(false);
}

void MainWindow::initializeBattleWithEnemy()
{
    ui->rollButton->setEnabled(true);
    ui->returnToMainMenuButton->setEnabled(false);
    if (battle != nullptr)
        delete battle;
    currentEnemy = area_enemies[area_enemies.size() - 1];
    battle = new Battle(player, currentEnemy);

    if (area_enemies.size() > 1)
    {
        QString name = QString::fromStdString(area_enemies[area_enemies.size() - 2]->getName());
        ui->nextEnemy->setText("Next Enemy: " + name);
    }
    else
        ui->nextEnemy->setText("Next Enemy: Boss!");
    updateBattleMenuPlayerStats();
    updateBattleMenuEnemyStats();
}

void MainWindow::nextTurn()
{
    battle->nextTurn();
    QString playerRoll = QString::number(battle->getPlayerRoll());
    QString enemyRoll = QString::number(battle->getEnemyRoll());
    QString rollDifference = QString::number(abs(playerRoll.toInt() - enemyRoll.toInt()));

    QString damageDone = QString::number(battle->getDamageDone());
    QString playerName = QString::fromStdString(player->getName());
    QString enemyName = QString::fromStdString(currentEnemy->getName());

    ui->battleMenuPlayerRoll->setText(playerName + " Roll: " + playerRoll);
    ui->battleMenuEnemyRoll->setText(enemyName + " Roll: " + enemyRoll);

    if (battle->getPlayerWonRoll())
    {
        ui->battleMenuTurnResult->setText(playerName + " deals " + damageDone + " damage to " + enemyName + "!");
    }
    else
        ui->battleMenuTurnResult->setText(enemyName + " deals " + damageDone + " damage to " + playerName + "!");

    ui->battleMenuRollDifference->setText("Roll difference: " + rollDifference);

    if (player->combatStats->HP <= 0 || currentEnemy->combatStats->HP <= 0)
    {
        if (player->combatStats->HP <= 0)
        {
            ui->battleMenuBattleResult->setText(enemyName + " has won!");
            ui->returnToMainMenuButton->setEnabled(true);
        }
        else if (currentEnemy->combatStats->HP <= 0)
        {
            ui->battleMenuBattleResult->setText(playerName + " has won!");
            ui->nextBattleButton->setEnabled(true);
        }
        ui->rollButton->setEnabled(false);
    }
    updateBattleMenuPlayerStats();
    updateBattleMenuEnemyStats();
}

void MainWindow::useCard()
{
    QString playerName = QString::fromStdString(player->getName());
    QString enemyName = QString::fromStdString(currentEnemy->getName());
    battle->useCard(player, currentEnemy, ui->battleMenuCards->currentText().toStdString());
    ui->battleMenuTurnResult->setText(playerName + " used " + ui->battleMenuCards->currentText() + " card.");
    updateBattleMenuPlayerStats();
    updateBattleMenuEnemyStats();
    if (player->combatStats->HP <= 0 || currentEnemy->combatStats->HP <= 0)
    {
        if (player->combatStats->HP <= 0)
        {
            ui->battleMenuBattleResult->setText(enemyName + " has won!");
            ui->returnToMainMenuButton->setEnabled(true);
        }
        else if (currentEnemy->combatStats->HP <= 0)
        {
            ui->battleMenuBattleResult->setText(playerName + " has won!");
            ui->nextBattleButton->setEnabled(true);
        }
        ui->rollButton->setEnabled(false);
    }
}

void MainWindow::endArea()
{
    player->restoreHP();
    switchToMainMenu();
}

void MainWindow::selectInventoryItemType()
{
    if (ui->viewInventoryWeaponsRadioButton->isChecked())
        ui->viewInventoryItemType->setText("Weapons");
    else if (ui->viewInventoryArmorRadioButton->isChecked())
        ui->viewInventoryItemType->setText("Armor");
    else if (ui->viewInventoryBootsRadioButton->isChecked())
        ui->viewInventoryItemType->setText("Boots");
    else if (ui->viewInventoryCardsRadioButton->isChecked())
        ui->viewInventoryItemType->setText("Cards");
    updateEquipmentMenuPlayerStats();
}

void MainWindow::selectShopItemType()
{
    if (ui->shopInventoryWeaponsRadioButton->isChecked())
        ui->shopInventoryItemType->setText("Weapons");
    else if (ui->shopInventoryArmorRadioButton->isChecked())
        ui->shopInventoryItemType->setText("Armor");
    else if (ui->shopInventoryBootsRadioButton->isChecked())
        ui->shopInventoryItemType->setText("Boots");
    else if (ui->shopInventoryCardsRadioButton->isChecked())
        ui->shopInventoryItemType->setText("Cards");
    updateShopMenuInventory();
}

void MainWindow::equipItem()
{
    std::string selectedItem = ui->equipmentList->currentItem()->text().toStdString();

    if (ui->viewInventoryWeaponsRadioButton->isChecked())
    {
        player->equipWeapon(selectedItem);
        ui->equipItemResult->setText("Equipped " + ui->equipmentList->currentItem()->text());
    }
    else if (ui->viewInventoryArmorRadioButton->isChecked())
    {
        player->equipArmor(selectedItem);
        ui->equipItemResult->setText("Equipped " + ui->equipmentList->currentItem()->text());
    }
    else if (ui->viewInventoryCardsRadioButton->isChecked())
    {
        if (player->getEquipped().getCards().size() >= 4)
            ui->equipItemResult->setText("Error: Max cards equipped (4)");
        else
            player->equipCard(selectedItem);
    }
    updateMainMenuPlayerStats();
    updateEquipmentMenuPlayerStats();
}

void MainWindow::purchaseItem()
{
    std::string selectedItem = ui->shopList->currentText().toStdString();
    if (selectedItem.empty())
    {
        ui->purchaseItemResult->setText("Error: No selected item");
        return;
    }

    if (ui->shopInventoryWeaponsRadioButton->isChecked())
    {
        if (shop.buyWeapon(player, selectedItem))
        {
            ui->purchaseItemResult->setText("Successfully bought " + QString::fromStdString(selectedItem));
            updateShopMenuInventory();
        }
        else
            ui->purchaseItemResult->setText("Error: Insufficient funds");
    }
    else if (ui->shopInventoryArmorRadioButton->isChecked())
    {
        if (shop.buyArmor(player, selectedItem))
        {
            ui->purchaseItemResult->setText("Successfully bought " + QString::fromStdString(selectedItem));
            updateShopMenuInventory();
        }
        else
            ui->purchaseItemResult->setText("Error: Insufficient funds");
    }
    else if (ui->shopInventoryCardsRadioButton->isChecked())
    {
        if (shop.buyCard(player, selectedItem))
        {
            ui->purchaseItemResult->setText("Successfully bought " + QString::fromStdString(selectedItem));
            updateShopMenuInventory();
        }
        else
            ui->purchaseItemResult->setText("Error: Insufficient funds");
    }
    updateShopMenuPlayerStats();
}

void MainWindow::switchToMainMenu()
{
    updateMainMenuPlayerStats();
    ui->menu_pages->setCurrentIndex(1);
}

void MainWindow::switchToBattleMenu()
{
    ui->menu_pages->setCurrentIndex(2);
    updateBattleMenuPlayerStats();
    updateBattleMenuEnemyStats();
}

void MainWindow::switchToShopMenu()
{
    updateShopMenuPlayerStats();
    updateShopMenuInventory();
    ui->menu_pages->setCurrentIndex(3);
}

void MainWindow::switchToEquipmentMenu()
{
    updateEquipmentMenuPlayerStats();
    ui->menu_pages->setCurrentIndex(4);
}
