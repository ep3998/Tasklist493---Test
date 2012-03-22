#include "tasklist_notes.h"
#include "my_listwidget.h"
#include <iostream>
#include <map>
#include <queue>
#include <QDialog>
#include <QtGui>
#include <QListWidget>
#include <QListWidgetItem>

using namespace std;

//Globals
int itemCount = 0;
bool deleteClicked = false;
my_listwidget *newPtr;

//Changes out listing if mainList changes the overarching
//"list" item containing tasks
void tasklist_notes::listChanged(QListWidgetItem *newList){
    //If currently no list, activate buttons
    if(currList == NULL){
        newPtr->setParent(0);
        newPtr->setDisabled(true);
        addButton->setEnabled(true);
        delButton->setEnabled(true);
        delete newPtr;
    }
    //Else if new list being made
    else if (newList == NULL){
        currList = NULL;
        newPtr = new my_listwidget();
        listLayout->addWidget(newPtr);
        addButton->setDisabled(true);
        delButton->setDisabled(true);
        return;
    }

    //See if list is found in listMap, based on
    //QWidgetItem* from mainList
    if(listMap.find(currList) != listMap.end()){
        qDebug("removing listMap[currList] UI");
        listMap[currList]->setParent(0);
        listMap[currList]->setDisabled(true);
        listMap[currList]->hide();
    }

    //If found, change to that list
    if(listMap.find(newList) != listMap.end()){
        qDebug("found newList in map");
    }
    //Else, create a new my_listwidget
    else{
        qDebug("creating new my_listwidget");
        newPtr = new my_listwidget();
        newPtr->setMinimumHeight(100);
        newPtr->setMinimumWidth(200);
        newPtr->setWordWrap(true);
        newPtr->setAlternatingRowColors(true);
        newPtr->setDragDropMode(QAbstractItemView::InternalMove);
        newPtr->setSelectionMode(QAbstractItemView::SingleSelection);
        newPtr->setDropIndicatorShown(true);

        //Add list to listMap
        listMap[newList] = newPtr;
        newPtr = NULL;

    }

    //Add new/found list to UI
    listLayout->addWidget(listMap[newList]);
    listMap[newList]->setDisabled(false);
    listMap[newList]->show();

    //Connect add/delete buttons to the correct list
    connect(listMap[newList], SIGNAL(currentRowChanged(int)), this, SLOT(itemSelectChange()));
    connect(listMap[newList], SIGNAL(itemChanged(QListWidgetItem*)),this,SLOT(checkedOrUnchecked(QListWidgetItem*)));

    //Cleanup, set up and sync to tasklist_options
    currList = newList;
    if(listMap[currList]->count() > 0){
        listMap[currList]->setCurrentRow(listMap[currList]->count()-1);
        emit itemSelectEmit(*listMap[currList]->currentItem());
    }
}

//Keypress events
//--Starts editing task on "return" press
void tasklist_notes::keyPressEvent(QKeyEvent * event){
    if(event->key() == Qt::Key_Return){
        QModelIndex index = listMap[currList]->model()->index(listMap[currList]->currentRow(),0, QModelIndex());
        listMap[currList]->edit(index);
    }
    else
        QWidget::keyPressEvent(event);
}

//Changes text color based on checked status
void tasklist_notes::checkedOrUnchecked(QListWidgetItem *item){
    if(item->checkState() == Qt::Checked){
        item->setTextColor(QColor("green"));
    }
    else{
        item->setTextColor(QColor("black"));
    }
}

//Delete button clicked
//Includes various edge case scenarios
//for when deleting a task from the list
void tasklist_notes::delClick(){
    QListWidgetItem *remItem;

    if(listMap[currList]->count()>0){
        int currRow = listMap[currList]->currentRow();
        deleteClicked = true;

        //If currently being deleted has a subtext note also in list
        if(listMap[currList]->currentItem()->data(35).toBool() == true){
            QListWidgetItem *remItem2 = listMap[currList]->takeItem(currRow);
            delete remItem2;
        }
        remItem = listMap[currList]->takeItem(currRow);
        delete remItem;

        itemCount = listMap[currList]->count();
        deleteClicked = false;

        //Emit selected item change
        QListWidgetItem selectedItem;
        emit itemSelectEmit(selectedItem);
    }
}

//Add additional task based on what QString
//is given in (either from XML or from addButton)
void tasklist_notes::addItemAction(QString &inName, bool isSub){
    deleteClicked = false;
    itemCount = listMap[currList]->count();

    //Change name of task
    QListWidgetItem *toInsert;
    if(inName.isNull())
        toInsert = new QListWidgetItem("new item");
    else
        toInsert = new QListWidgetItem(inName);

    //If a sub text, make subtext
    if(isSub){
        toInsert->setFlags(0);
    }
    //Else, set properties of editable task
    else{
        toInsert->setFlags(toInsert->flags() | Qt::ItemIsEditable);
        toInsert->setCheckState(Qt::Unchecked);
        toInsert->setSelected(true);

        toInsert->setData(32, QVariant(""));
        toInsert->setData(33, QVariant(false));
        toInsert->setData(34, QVariant(QDate::currentDate()));
        toInsert->setData(35, QVariant(false));
    }

    //Insert into current list
    listMap[currList]->insertItem(itemCount, toInsert);
    listMap[currList]->setCurrentRow(itemCount);

    //If name is blank, make editable immediately
    if(inName.isNull()){
        QModelIndex index = listMap[currList]->model()->index(itemCount,0, QModelIndex());
        listMap[currList]->edit(index);
    }
}

//Add click default action
//Task has no name, edited immediately
void tasklist_notes::addClick(){
    QString tempStr = NULL;
    addItemAction(tempStr, false);
}

//When apply is clicked in tasklist_options, data
//is transferred from option_pane to note_pane and
//is synchronized across both views
void tasklist_notes::applyClicked(QListWidgetItem &passedIn){
    if(currList != NULL && listMap[currList]->count() > 0){
        listMap[currList]->currentItem()->setFont(passedIn.font());
        listMap[currList]->currentItem()->setData(32, passedIn.data(32)); //note
        listMap[currList]->currentItem()->setData(33, passedIn.data(33)); //display note bool
        listMap[currList]->currentItem()->setData(34, passedIn.data(34)); //note date

        //If no note is currently displayed
        if(listMap[currList]->currentItem()->data(35).toBool() == false){
            //If display note has been checked, create subtext
            if((listMap[currList]->currentItem()->data(33).toBool() == true)&&
               (!listMap[currList]->currentItem()->data(32).toString().trimmed().isEmpty())){
                QListWidgetItem *noteSubtext = new QListWidgetItem();
                noteSubtext->setFlags(0);
                noteSubtext->setText(listMap[currList]->currentItem()->data(32).toString());
                noteSubtext->setFont(passedIn.font());
                noteSubtext->setTextColor("black");
                listMap[currList]->currentItem()->setData(35, QVariant(true));
                listMap[currList]->insertItem(listMap[currList]->currentRow()+1, noteSubtext);
                itemCount = listMap[currList]->count();
            }
        }
        else if(listMap[currList]->currentItem()->data(35).toBool() == true){
            //Opposite: note is displayed, display note no longer checked
            if((listMap[currList]->currentItem()->data(33).toBool() == false) ||
                (listMap[currList]->currentItem()->data(32).toString().isEmpty())){
                QListWidgetItem *deleteMe = listMap[currList]->takeItem(listMap[currList]->currentRow()+1);
                delete deleteMe;
                itemCount = listMap[currList]->count();
                listMap[currList]->currentItem()->setData(35, QVariant(false));
            }
            else if((listMap[currList]->currentItem()->data(33).toBool() == true) ||
                (!listMap[currList]->currentItem()->data(32).toString().isEmpty())){
                //Else, we update everything if text has changed
                QListWidgetItem *changeMe = listMap[currList]->takeItem(listMap[currList]->currentRow()+1);
                changeMe->setText(listMap[currList]->currentItem()->data(32).toString());
                changeMe->setFont(passedIn.font());
                listMap[currList]->insertItem(listMap[currList]->currentRow()+1, changeMe);
            }
        }
    }
}

//Emit to tasklist_options if an item selection
//has been changed in QListWidget (syncs data
//from note pane to option pane)
void tasklist_notes::itemSelectChange(){
    QListWidgetItem selectedItem;
    if(deleteClicked == false){
        selectedItem.setFont(listMap[currList]->currentItem()->font());
        selectedItem.setData(32, listMap[currList]->currentItem()->data(32)); //note
        selectedItem.setData(33, listMap[currList]->currentItem()->data(33)); //display note bool
        selectedItem.setData(34, listMap[currList]->currentItem()->data(34)); //note date
        emit itemSelectEmit(selectedItem);
    }
}

//Sets up layout tree for note pane
tasklist_notes::tasklist_notes(QWidget *parent) :
    QWidget(parent)
{
    addButton = new QPushButton("+Add+");
    addButton->setDisabled(true);
    delButton = new QPushButton("-Del-");
    delButton->setDisabled(true);

    listLayout = new QVBoxLayout;
    changeLayout = new QHBoxLayout;

    changeLayout->addWidget(addButton);
    changeLayout->addWidget(delButton);

    newPtr = new my_listwidget();
    listLayout->addLayout(changeLayout);
    listLayout->addWidget(newPtr);

    this->setLayout(listLayout);
    this->setParent(parent);

    currList = NULL;

    connect(addButton, SIGNAL(clicked()), this, SLOT(addClick()));
    connect(delButton, SIGNAL(clicked()), this, SLOT(delClick()));
}
