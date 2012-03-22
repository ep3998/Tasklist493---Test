#include "tasklist_templates.h"
#include "tasklist_main.h"
#include "templateButtonGroup.h"
#include <QtGui>

//If cancel is clicked, close modal window
void tasklist_templates::cancelClick(){
    this->hide();
    this->setDisabled(true);
}

//Create and customize modal window
tasklist_templates::tasklist_templates(QWidget *parent, Qt::WindowFlags f) :
    QWidget(parent)
{
    templateGrid = new QGridLayout();
    //Template buttons
    QPushButton *tmpOneBut = new QPushButton(QIcon(":/tmp/icon/grocery_icon.png"),"Groceries");
    QPushButton *tmpTwoBut = new QPushButton(QIcon(":/tmp/icon/week_icon.png"),"Weekly Tasks");
    QPushButton *tmpThrBut = new QPushButton(QIcon(":/tmp/icon/blank_icon.png"),"Blank");
    //Cancel button
    QPushButton *cancelBut = new QPushButton("Cancel");

    //Associate buttons with template xml items
    templateButtonGroup *tmpGroup = new templateButtonGroup(this);
    tmpGroup->addButton(tmpOneBut, 1);
    tmpGroup->addButton(tmpTwoBut, 2);
    tmpGroup->addButton(tmpThrBut, 3);

    //Create grid layout
    templateGrid->addWidget(tmpOneBut, 0, 0);
    templateGrid->addWidget(tmpTwoBut, 0, 1);
    templateGrid->addWidget(tmpThrBut, 0, 2);
    templateGrid->addWidget(cancelBut, 1, 2);
    this->setLayout(templateGrid);
    this->setWindowTitle("Select Template");

    //Connect buttons and groupings with one another
    connect(cancelBut, SIGNAL(clicked()), this, SLOT(cancelClick()));
    connect(tmpGroup, SIGNAL(openTemplate(QString&)), (TaskList_Main*)this->parentWidget(), SLOT(openXML(QString&)));
    connect(tmpGroup, SIGNAL(windowClose()), this, SLOT(cancelClick()));

    //Define window flags (modal, primarily)
    this->setWindowFlags(f);
}
