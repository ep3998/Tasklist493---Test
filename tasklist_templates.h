#ifndef TASKLIST_TEMPLATES_H
#define TASKLIST_TEMPLATES_H

#include <QWidget>

class QGridLayout;

//Modal window that pops up upon newList action
//containing templated task lists
class tasklist_templates : public QWidget
{
    Q_OBJECT
public:
    explicit tasklist_templates(QWidget *parent = 0, Qt::WindowFlags f = 0);

public slots:
    void cancelClick();

private:
    QGridLayout *templateGrid;
};

#endif // TASKLIST_TEMPLATES_H
