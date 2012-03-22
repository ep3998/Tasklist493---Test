#ifndef TASKLIST_NOTES_H
#define TASKLIST_NOTES_H

#include <QWidget>
#include <QMap>
#include <map>

class my_listwidget;
class QPushButton;
class QAbstractButton;
class QListWidgetItem;
class QVBoxLayout;
class QHBoxLayout;
class map;

//Notes pane, contains the actual tasks for
//a given list instantiation (of my_listwidget)
class tasklist_notes : public QWidget
{
    Q_OBJECT
public:
    tasklist_notes(QWidget *parent = 0);
    std::map<QListWidgetItem*, my_listwidget* > listMap;
    QListWidgetItem *currList;
    void addItemAction(QString &inName, bool isSub);

signals:
    void itemSelectEmit(QListWidgetItem &passedItem);

public slots:
    void addClick();
    void delClick();
    void applyClicked(QListWidgetItem &passedInItem);
    void itemSelectChange();
    void checkedOrUnchecked(QListWidgetItem *item);
    void listChanged(QListWidgetItem *newList);

private:
    QPushButton *addButton;
    QPushButton *delButton;
    QVBoxLayout *listLayout;
    QHBoxLayout *changeLayout;

protected:
    void keyPressEvent(QKeyEvent * event);

};

#endif // TASKLIST_NOTES_H
