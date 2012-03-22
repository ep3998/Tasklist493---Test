#ifndef TASKLIST_LISTS_H
#define TASKLIST_LISTS_H

#include <QWidget>

class QListWidgetItem;
class QListWidget;

//Lists pane, holds information on "list of lists"
//Inherits from QWidget
class tasklist_lists : public QWidget
{
    Q_OBJECT
public:
    tasklist_lists(QWidget *parent = 0);
    QListWidget *mainList;
    bool deleteListClicked;

signals:
    void listSelectEmit(QListWidgetItem *listItem);

public slots:
    void listSelectChange();
    void newList(const QString &listName);
};

#endif // TASKLIST_LISTS_H
