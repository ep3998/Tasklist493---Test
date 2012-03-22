#ifndef TEMPLATEBUTTONGROUP_H
#define TEMPLATEBUTTONGROUP_H
#include <QButtonGroup>
#include <QPushButton>
#include "tasklist_main.h"

class tasklist_main;

//Overarching button grouping for template
//that minimizes code for additional templates
//Implemented here
class templateButtonGroup: public QButtonGroup
{
    Q_OBJECT
    public:
        //Constructor
        templateButtonGroup(QWidget* parent)
        {
            this->setParent(parent);

            connect(this, SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(buttonClick(QAbstractButton*)));
        }

        //Destructor
        ~templateButtonGroup(){}

    public slots:
        //Pseudo-enum button operation
        void buttonClick(QAbstractButton* button)
        {
            QString xmlFile = ":/tmp/xml/";
            if(button->text() == "Groceries")
                xmlFile += "task_groceries.xml";
            else if(button->text() == "Weekly Tasks")
                xmlFile += "task_week.xml";
            else if(button->text() == "Blank")
                xmlFile = "";

            emit openTemplate(xmlFile);
            emit windowClose();
        }

    signals:
        void openTemplate(QString &filePath);   //opens given task tmp.
        void windowClose();                     //closes window
};
#endif // TEMPLATEBUTTONGROUP_H
