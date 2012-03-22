#include "my_listwidget.h"
#include <QDebug>
#include <QtGui>

int prevRow = -1;           //For custom drag/drop
bool firstDrag = false;     //For custom drag/drop

//Default constructor for a list widget
my_listwidget::my_listwidget(QWidget *parent) : QListWidget(parent)
{
    QListWidget::QListWidget();
}

//Move event for my_listwidget
//Captures previous row being moved
void my_listwidget::dragMoveEvent(QDragMoveEvent *event){
    if(firstDrag == false){
        prevRow = currentRow();
        firstDrag=true;
    }
    QListWidget::dragMoveEvent(event);
}

//Custom drop event
//Moves based on characteristics of items, whether
//they're subnotes or actual tasks
void my_listwidget::dropEvent(QDropEvent *event){
    //If item is a subnote
    if(currentItem()->data(35).toBool() == true){
        QListWidget::dropEvent(event);

        QListWidgetItem *prevNote;
        if(currentRow() < prevRow)
            prevNote = takeItem(prevRow+1);
        else
            prevNote = takeItem(prevRow);
        insertItem(currentRow()+1, prevNote);
    }
    //Else if it's a main note
    else{
        QListWidget::dropEvent(event);

        //Swapping items
        if((count()>1) &&
                (currentRow() != count()-1) &&
                (item(currentRow()+1)->flags() == 0)){
            QListWidgetItem *prevNote = takeItem(currentRow());
            insertItem(prevRow, prevNote);
            qDebug("Can't insert between note-sub");
        }
    }

    //Clean-up/reset drag-drop
    firstDrag = false;
    prevRow = -1;
}
