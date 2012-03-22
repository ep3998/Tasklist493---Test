#ifndef MY_LISTWIDGET_H
#define MY_LISTWIDGET_H

#include <QListWidget>

//Custom QListWidget class
//Different drag/drop handlers
class my_listwidget : public QListWidget
{
    Q_OBJECT
public:
    my_listwidget(QWidget *parent = 0);

protected:
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
};

#endif // MY_LISTWIDGET_H
