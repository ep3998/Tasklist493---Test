#include <QtGui>
#include <QDialog>
#include <QtXml/QDomElement>
#include <QtXml/QDomDocument>
#include <QtXml/QDomNode>
#include <QtXml/QDomText>
#include <QString>
#include <QMessageBox>

#include "tasklist_main.h"
#include "tasklist_lists.h"
#include "tasklist_notes.h"
#include "tasklist_options.h"
#include "tasklist_templates.h"
#include "my_listwidget.h"

//XML
//Add element to node tree
QDomElement TaskList_Main::addElement(QDomDocument &doc, QDomNode &node,
                               const QString &tag, const QString &value){
    QDomElement elt = doc.createElement(tag);
    node.appendChild(elt);
    if (!value.isNull()){
        QDomText txt = doc.createTextNode(value);
        elt.appendChild(txt);
    }

    return elt;
}

//XML
//Parse XML into tasklist form
int TaskList_Main::parseXML(QDomDocument &domTree){
    QDomElement set = domTree.namedItem("listset").toElement();
    QMessageBox msgBox;
    //If tree doesn't exist, stop
    if(set.isNull()){
        msgBox.setText("No <listset> element at top level");
        msgBox.setWindowTitle("Erorr parsing XML");
        msgBox.exec();
        return -1;
    }

    //Iterate through all "list" items
    QDomElement n = set.firstChildElement("list");
    for( ; !n.isNull(); n = n.nextSiblingElement("list")){
        emit createList(n.namedItem("list_title").toElement().text());
        delListAction->setEnabled(true);
        printAction->setEnabled(true);
        printAllAction->setEnabled(true);

        //Iterate through all "task" items part of "list"
        QDomElement o = n.firstChildElement("task");
        for( ; !o.isNull(); o = o.nextSiblingElement("task")){
            my_listwidget *currList = notePane->listMap[notePane->currList];
            QListWidgetItem *currItem;
            QString tempStr;

            //If task is 'main' (not subtext/subnote)
            if(o.attribute("task_type") == "main"){
                //Change task name
                notePane->addItemAction(o.namedItem("task_title").toElement().text(), false);
                currItem = currList->currentItem();

                //Change task check state
                tempStr = o.namedItem("task_check").toElement().text();
                if(tempStr == "unchecked")
                    currItem->setCheckState(Qt::Unchecked);
                else if (tempStr == "checked")
                    currItem->setCheckState(Qt::Checked);
                else if (tempStr == "part_check")
                    currItem->setCheckState(Qt::PartiallyChecked);
                else{
                    msgBox.setText("Unknown check state");
                    msgBox.setWindowTitle("Erorr parsing XML");
                    msgBox.exec();
                    return -1;
                }

                //Change task subnote
                currItem->setData(32, QVariant(
                                      o.namedItem("task_note").toElement().text()));

                //Change if task subnote is displayed
                tempStr = o.namedItem("task_display").toElement().text();
                if(tempStr == "false"){
                    currItem->setData(33, QVariant(false));
                    currItem->setData(35, QVariant(false));
                }
                else if(tempStr == "true"){
                    currItem->setData(33, QVariant(true));
                    currItem->setData(35, QVariant(true));
                }
                else{
                    msgBox.setText("Unknown bool type - display");
                    msgBox.setWindowTitle("Erorr parsing XML");
                    msgBox.exec();
                    return -1;
                }

                //Change the task due date
                tempStr = o.namedItem("task_date").toElement().text();
                qDebug((const char *)tempStr.toAscii().data());
                QDate tempDate;
                int year = tempStr.left(4).toInt();
                int month = tempStr.mid(5, 2).toInt();
                int day = tempStr.right(2).toInt();

                tempDate.setDate(year, month, day);
                if(!tempDate.isValid()){
                    msgBox.setText("Unknown date type");
                    msgBox.setWindowTitle("Erorr parsing XML");
                    msgBox.exec();
                    return -1;
                }
                currItem->setData(34, QVariant(tempDate));

                //Change the task font
                tempStr = o.namedItem("task_font").toElement().text();
                QFont tempFont;
                if(!tempFont.fromString(tempStr)){
                    msgBox.setText("Unknown font");
                    msgBox.setWindowTitle("Erorr parsing XML");
                    msgBox.exec();
                    return -1;
                }
                currItem->setFont(tempFont);
            }
            //Else if it is a subtext/subnote for a 'main'
            else if (o.attribute("task_type") == "sub"){
                //Change note's text
                notePane->addItemAction(o.namedItem("task_title").toElement().text(), true);
                currItem = currList->currentItem();
                currItem->setFlags(0);

                //Change note's font
                tempStr = o.namedItem("task_font").toElement().text();
                QFont tempFont;
                if(!tempFont.fromString(tempStr)){
                    msgBox.setText("Unknown font");
                    msgBox.setWindowTitle("Erorr parsing XML");
                    msgBox.exec();
                    return -1;
                }
                currItem->setFont(tempFont);
            }
            //Else, exit gracefully
            else{
                msgBox.setText("Unknown list type");
                msgBox.setWindowTitle("Erorr parsing XML");
                msgBox.exec();
                return -1;
            }

            //Synchronize the option pane
            optionPane->itemSelectDataIn(*currItem);
        }
    }

    return 0;
}

//XML
//Converts the given lists and puts them in an XML tree
QString TaskList_Main::listsToXML(const QListWidget &inList){
    QDomDocument doc;
    QDomProcessingInstruction instr = doc.createProcessingInstruction(
                "xml", "version='1.0' encoding='UTF-8'");
    doc.appendChild(instr);

    //Create list to hold individual task lists
    QDomElement listElement = addElement(doc, doc, "listset");

    //Iteratore through available 'lists'
    for(int i = 0; i < inList.count(); ++i){
        QDomElement list = addElement(doc, listElement, "list");
        addElement(doc, list, "list_title", inList.item(i)->text());

        //Find associated note mapping
        my_listwidget *listPtr = notePane->listMap[inList.item(i)];

        //Iterates through 'tasks' in 'list'
        bool nextIsSub = false;
        for(int j = 0; j < listPtr->count(); ++j){
            QDomElement task = addElement(doc, list, "task");
            QListWidgetItem *taskPtr = listPtr->item(j);

            //If next item is subtext...
            if(nextIsSub){
                nextIsSub = false;
                task.setAttribute("task_type", "sub");
                addElement(doc, task, "task_title", taskPtr->text());
                addElement(doc, task, "task_font", taskPtr->font().toString());
            }
            //Else, we know it's main task
            else{
                task.setAttribute("task_type", "main");
                addElement(doc, task, "task_title", taskPtr->text());
                if(taskPtr->checkState() == Qt::Unchecked)
                    addElement(doc, task, "task_check", "unchecked");
                if(taskPtr->checkState() == Qt::Checked)
                    addElement(doc, task, "task_check", "checked");
                if(taskPtr->checkState() == Qt::PartiallyChecked)
                    addElement(doc, task, "task_check", "part_check");
                addElement(doc, task, "task_note", taskPtr->data(32).toString());
                addElement(doc, task, "task_display", taskPtr->data(33).toString());
                addElement(doc, task, "task_date", taskPtr->data(34).toDate().toString(Qt::ISODate));
                addElement(doc, task, "task_font", taskPtr->font().toString());

                if(taskPtr->data(35).toBool() == true){
                    nextIsSub = true;
                }
            }
        }
    }

    //Return DOM document
    return doc.toString();
}

//Slot for expanding and closing "additional info"
//panel IE the right pane
void TaskList_Main::expClick(){
    if(optionPane->isHidden()){
        optionPane->setHidden(false);
        expButton->setIcon(QIcon(":img/icon/left_arrow.png"));
    }
    else{
        optionPane->setHidden(true);
        optionPane->applyClicked();
        expButton->setIcon(QIcon(":img/icon/right_arrow.png"));
    }
}

//New list create wrapper (list has no name)
void TaskList_Main::newListCreate(){
    QString tempStr = NULL;
    emit createList(tempStr);
    delListAction->setEnabled(true);
    printAction->setEnabled(true);
    printAllAction->setEnabled(true);
}

//Generate template modal window
void TaskList_Main::newListClick(){
    tasklist_templates *tmpWidget = new tasklist_templates(this, Qt::Popup | Qt::Dialog);
    tmpWidget->setWindowModality(Qt::WindowModal);
    tmpWidget->show();
}

//Delete list clicked
void TaskList_Main::delListClick(){
    listPane->deleteListClicked = true;
    QListWidgetItem * tempPtr = listPane->mainList->takeItem(
                listPane->mainList->currentRow());
    notePane->listMap[tempPtr]->setParent(0);
    notePane->listMap[tempPtr]->setDisabled(true);
    notePane->listMap.erase(tempPtr);

    delete tempPtr;

    if(listPane->mainList->count() == 0){
        qDebug("nothing left");
        delListAction->setDisabled(true);
        printAction->setDisabled(true);
        printAllAction->setDisabled(true);
        notePane->listChanged(NULL);
    }
    else{
        qDebug("something left");
        notePane->listChanged(listPane->mainList->currentItem());
    }
    listPane->deleteListClicked = false;
}

//Open XML action
void TaskList_Main::openXML(QString &xmlPath){
    if(!xmlPath.isEmpty())
        openClickAction(xmlPath, false);
    else
        newListCreate();
}

//Open click wrapper for button
void TaskList_Main::openClick(){
    QString name = "";
    openClickAction(name, true);
}

//Open XML file clicked
void TaskList_Main::openClickAction(QString inName, bool clearLists){
    if(inName.trimmed().isEmpty())
        inName = QFileDialog::getOpenFileName(this, tr("Open List File"),
                                                  QDir::currentPath(),
                                                  tr("LIST Files (*.xml)"));
    if(inName.isEmpty())
        return;

    //Open file stream
    QFile in(inName);
    if(!in.open(QFile::ReadOnly| QFile::Text)){
        QMessageBox::warning(this, tr("Open List"),
                             tr("Cannot open file %1:\n%2")
                             .arg(inName).arg(in.errorString()));
        return;
    }

    //If not a template, clear listing
    if(clearLists){
        while(listPane->mainList->count() != 0)
            delListClick();
    }

    //Create the XML document to read in
    QDomDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;
    if(doc.setContent(&in, &errorMsg, &errorLine, &errorColumn)){
        if(parseXML(doc) < 0){
            while(listPane->mainList->count() != 0)
                delListClick();
        }
    }
    else{
        errorMsg.append(" line ");
        errorMsg.append(QString::number(errorLine));
        errorMsg.append(" col ");
        errorMsg.append(QString::number(errorColumn));

        QMessageBox msgBox;
        msgBox.setText(errorMsg.toAscii().data());
        msgBox.setWindowTitle("Erorr parsing XML");
        msgBox.exec();
    }

    //Handle whether it came from template rsc. or from
    //actual user file
    qDebug(inName.toAscii().data());
    if(!inName.contains(":/tmp/xml/"))
        myFileName = inName;
    //If task_week item, sync up next dates for tasks
    else if(inName.contains(":/tmp/xml/task_groceries.xml")){
        QDate currDate = QDate::currentDate();
        qDebug(currDate.toString().toAscii().data());

        QListWidgetItem *listPtr = listPane->mainList->currentItem();
        my_listwidget *taskPtr = notePane->listMap[listPtr];

        for(int task = 0; task < taskPtr->count(); ++task){
            taskPtr->item(task)->setData(34, QVariant(currDate));
            optionPane->itemSelectDataIn(*taskPtr->item(task));
        }

    }
    else if(inName.contains(":/tmp/xml/task_week.xml")){
        QDate currDate = QDate::currentDate();
        qDebug(currDate.toString().toAscii().data());
        while(currDate.dayOfWeek() != 1){
            qDebug("add day");
            currDate = currDate.addDays(1);
            qDebug(currDate.toString().toAscii().data());
        }
        for(int day = listPane->mainList->count()-7; day < listPane->mainList->count(); ++day){
            QListWidgetItem *listPtr = listPane->mainList->item(day);
            my_listwidget *dayPtr = notePane->listMap[listPtr];

            QString newTitle = currDate.toString();
            listPtr->setText(newTitle);

            for(int task = 0; task < dayPtr->count(); ++task){
                dayPtr->item(task)->setData(34, QVariant(currDate));
                optionPane->itemSelectDataIn(*dayPtr->item(task));
            }
            currDate = currDate.addDays(1);
        }
    }
}

//Save action clicked
void TaskList_Main::saveClick(){
    if(myFileName != ""){
        //Open a file stream if file accessed already
        QFile file(myFileName);
        if(!file.open(QFile::WriteOnly | QFile::Text)){
            QMessageBox::warning(this, tr("Save List"),
                                 tr("Cannot write file %1:\n%2")
                                 .arg(myFileName).arg(file.errorString()));
            return;
        }

        //If file successfully saved, update status
        if(file.write((const char *)listsToXML(*listPane->mainList).toAscii().data())){
            statusBar()->showMessage(tr("File saved"), 2000);
        }
    }
    else
        saveAsClick();  //Get a file name/path
}

//Save as clicked, need to get new file path
void TaskList_Main::saveAsClick(){
    QString outFile = QFileDialog::getSaveFileName(this, tr("Save List File"),
                                                   QDir::currentPath(),
                                                   tr("LIST Files (*.xml)"));
    if(outFile.isEmpty())
        return;

    //Open a file stream
    QFile file(outFile);
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, tr("Save List"),
                             tr("Cannot write file %1:\n%2")
                             .arg(outFile).arg(file.errorString()));
        return;
    }

    //Write to the file stream the XML metadata
    if(file.write((const char *)listsToXML(*listPane->mainList).toAscii().data())){
        statusBar()->showMessage(tr("File saved"), 2000);
        myFileName = outFile;
    }
}

//Print clicked, format current list
//for printing
void TaskList_Main::printClick(){
    QPrinter printer;

    //Create new print dialog (select printer)
    QPrintDialog *pDialog = new QPrintDialog(&printer, this);
    pDialog->setWindowTitle(tr("Print Document"));
    if(pDialog->exec() != QDialog::Accepted)
        return;
    else{
        QTextDocument document;
        QString html = "";
        QListWidgetItem *currList = listPane->mainList->currentItem();
        my_listwidget *currNote = notePane->listMap[currList];

        //Generate HTML driven output
        html += "<font size=\"8\"><b>" + currList->text() + "</b></font><br />";
        for(int i = 0; i < currNote->count(); i++){
            if(currNote->item(i)->flags() != 0)
                html += "<font size=\"5\">" + currNote->item(i)->text() + "</font><br />";
            else
                html += "--<font size=\"4\">" + currNote->item(i)->text() + "</font><br />";
        }

        //Set document's HTML info
        document.setHtml(html);

        //Print the HTML text
        document.print(&printer);
    }
}

//Print clicked, format all lists
//for printing
void TaskList_Main::printAllClick(){
    QPrinter printer;

    //Create new print dialog (select printer)
    QPrintDialog *pDialog = new QPrintDialog(&printer, this);
    pDialog->setWindowTitle(tr("Print Document"));
    if(pDialog->exec() != QDialog::Accepted)
        return;
    else{
        QTextDocument document;
        QString html = "";

        for(int i = 0; i < listPane->mainList->count(); ++i){
            QListWidgetItem *currList = listPane->mainList->item(i);
            my_listwidget *currNote = notePane->listMap[currList];

            //Generate HTML driven output
            html += "<font size=\"7\"><b>" + currList->text() + "</b></font><br />";
            for(int i = 0; i < currNote->count(); i++){
                if(currNote->item(i)->flags() != 0)
                    html += "<font size=\"5\">" + currNote->item(i)->text() + "</font><br />";
                else
                    html += "--<font size=\"4\"><i>" + currNote->item(i)->text() + "</i></font><br />";
            }

            html += "<br />";
        }

        //Set document's HTML info
        document.setHtml(html);

        //Print the HTML text
        document.print(&printer);
    }
}

//Create the menu bar
void TaskList_Main::createMenu(){
    menuBar = new QMenuBar;
    fileMenu = new QMenu(tr("File"), this);

    newListAction = fileMenu->addAction(tr("New List..."));
    newListAction->setShortcuts(QKeySequence::New);

    delListAction = fileMenu->addAction(tr("Delete Current List..."));
    delListAction->setShortcuts(QKeySequence::Delete);
    delListAction->setDisabled(true);

    openListAction = fileMenu->addAction(tr("Open List..."));
    openListAction->setShortcuts(QKeySequence::Open);

    saveAction = fileMenu->addAction(tr("Save Lists..."));
    saveAction->setShortcuts(QKeySequence::Save);
    saveAsAction = fileMenu->addAction(tr("Save Lists As..."));
    saveAsAction->setShortcuts(QKeySequence::SaveAs);

    printAction = fileMenu->addAction(tr("Print..."));
    printAction->setShortcuts(QKeySequence::Print);
    printAction->setDisabled(true);
    printAllAction = fileMenu->addAction(tr("Print all..."));
    printAllAction->setDisabled(true);

    quitAction = fileMenu->addAction(tr("Exit"));
    quitAction->setShortcuts(QKeySequence::Quit);

    //Show menu
    menuBar->addMenu(fileMenu);

    //Connect actions to associated slots
    connect(newListAction, SIGNAL(triggered()), this, SLOT(newListClick()));
    connect(delListAction, SIGNAL(triggered()), this, SLOT(delListClick()));
    connect(quitAction, SIGNAL(triggered()),this,SLOT(close()));
    connect(printAction, SIGNAL(triggered()), this, SLOT(printClick()));
    connect(printAllAction, SIGNAL(triggered()), this, SLOT(printAllClick()));
    connect(openListAction, SIGNAL(triggered()), this, SLOT(openClick()));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveClick()));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAsClick()));
}

//Sets up layout that combines left and right panes
TaskList_Main::TaskList_Main(QWidget *parent) :
    QMainWindow(parent)
{
    listPane = new tasklist_lists(this);
    listPane->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    notePane = new tasklist_notes(this);
    notePane->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    optionPane = new tasklist_options(this);
    optionPane->setHidden(true);
    expButton = new QPushButton("");

    expButton->setAutoFillBackground(true);
    expButton->setIcon(QIcon(":img/icon/right_arrow.png"));
    expButton->setIconSize(QSize(15,300));
    expButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);

    QHBoxLayout *paneLayout = new QHBoxLayout;
    paneLayout->addWidget(listPane);
    paneLayout->addWidget(notePane);
    paneLayout->addWidget(expButton);
    paneLayout->addWidget(optionPane);

    //Set the central widget (work-around)
    QWidget *central = new QWidget();
    central->setLayout(paneLayout);
    this->setCentralWidget(central);
    createMenu();
    setMenuBar(menuBar);

    //Connect signals between different panes
    connect(expButton, SIGNAL(clicked()), this, SLOT(expClick()));
    connect(optionPane, SIGNAL(noteInfoSent(QListWidgetItem&)), notePane, SLOT(applyClicked(QListWidgetItem&)));
    connect(notePane, SIGNAL(itemSelectEmit(QListWidgetItem&)), optionPane, SLOT(itemSelectDataIn(QListWidgetItem&)));
    connect(optionPane, SIGNAL(cancelPane()), this, SLOT(expClick()));
    connect(this, SIGNAL(createList(QString)), listPane, SLOT(newList(const QString &)));
    connect(listPane, SIGNAL(listSelectEmit(QListWidgetItem*)), notePane, SLOT(listChanged(QListWidgetItem*)));
}
