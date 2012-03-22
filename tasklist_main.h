#ifndef TASKLIST_MAIN_H
#define TASKLIST_MAIN_H

#include <QMainWindow>

class tasklist_lists;
class tasklist_options;
class tasklist_notes;
class QPushButton;
class QPixmap;
class QIcon;
class QMenu;
class QDomElement;
class QDomDocument;
class QDomNode;
class QString;
class QListWidget;

//Main overarching QMainWindow class
//Contains items for menus, panes, and connecting
//slots + signal connectors
class TaskList_Main : public QMainWindow
{
    Q_OBJECT
public:
    TaskList_Main(QWidget *parent = 0);
    tasklist_lists *listPane;
    tasklist_notes *notePane;
    tasklist_options *optionPane;

    void openClickAction(QString inName, bool clearLists);
    void newListCreate();

signals:
    void createList(QString listName);

public slots:
    void expClick();
    void newListClick();
    void delListClick();
    void printClick();
    void printAllClick();
    void openClick();
    void saveClick();
    void saveAsClick();

    void openXML(QString &xmlPath);

private:
        QPushButton *expButton;
        QIcon buttonIcon;
        QString myFileName;
        QMenuBar *menuBar;

        QMenu *fileMenu;
        QAction *newListAction;
        QAction *delListAction;
        QAction *openListAction;
        QAction *saveAction;
        QAction *saveAsAction;
        QAction *printAction;
        QAction *printAllAction;
        QAction *quitAction;

        void createMenu();
        QDomElement addElement(QDomDocument &doc, QDomNode &node,
                               const QString &tag, const QString &value = QString:: null);
        int parseXML(QDomDocument &domTree);
        QString listsToXML(const QListWidget &inList);
};

#endif // TASKLIST_MAIN_H
