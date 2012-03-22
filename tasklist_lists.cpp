#include "tasklist_lists.h"
#include "tasklist_notes.h"
#include <QtGui>
#include <QDebug>

//Creates a new list to hold tasks
void tasklist_lists::newList(const QString &listName){
    int itemCount = mainList->count();

    //Customizing flags for new item
    QListWidgetItem *toInsert = new QListWidgetItem("new list");
    toInsert->setFlags(0);
    toInsert->setSelected(true);
    toInsert->setFlags(toInsert->flags() | Qt::ItemIsSelectable);
    toInsert->setFlags(toInsert->flags() | Qt::ItemIsDragEnabled);
    toInsert->setFlags(toInsert->flags() | Qt::ItemIsDropEnabled);
    toInsert->setFlags(toInsert->flags() | Qt::ItemIsEnabled);
    toInsert->setFlags(toInsert->flags() | Qt::ItemIsEditable);

    //Insert and change to new list
    mainList->insertItem(itemCount, toInsert);
    mainList->setCurrentRow(itemCount);

    //If called with a given list name (template tasklist)
    if(!listName.isNull()){
        mainList->item(itemCount)->setText(listName);
    }
    //Else make editable
    else{
        QModelIndex index = mainList->model()->index(itemCount,0, QModelIndex());
        mainList->edit(index);
    }

}

//Emits the current list has been changed to children
void tasklist_lists::listSelectChange(){
    QListWidgetItem *listItem = mainList->currentItem();

    //If delete destroyed last list, don't emit change
    if(deleteListClicked == false)
        emit listSelectEmit(listItem);
}

//Constructor
//Lays out the QWidgetList
tasklist_lists::tasklist_lists(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    deleteListClicked = false;

    mainList = new QListWidget();
    mainList->setMinimumHeight(100);
    mainList->setMinimumWidth(200);
    mainList->setWordWrap(true);
    mainList->setDragDropMode(QAbstractItemView::InternalMove);
    mainList->setSelectionMode(QAbstractItemView::SingleSelection);
    mainList->setDropIndicatorShown(true);

    mainLayout->addWidget(mainList);

    this->setLayout(mainLayout);
    this->setParent(parent);

    connect(mainList, SIGNAL(currentRowChanged(int)),this,SLOT(listSelectChange()));
}
