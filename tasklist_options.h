#ifndef TASKLIST_OPTIONS_H
#define TASKLIST_OPTIONS_H

#include <QWidget>

class QLabel;
class QTextEdit;
class QDateEdit;
class QCheckBox;
class QFontComboBox;
class QPushButton;
class QListWidgetItem;
class QDate;
class QFont;
class QString;

//Options pane, contains the additional items
//for a given task, including notes and fonts
class tasklist_options : public QWidget
{
    Q_OBJECT
public:
    explicit tasklist_options(QWidget *parent = 0);
    
signals:
    void noteInfoSent(QListWidgetItem &passedOutItem);
    void cancelPane();

public slots:
    void applyClicked();
    void cancelClicked();
    void itemSelectDataIn(QListWidgetItem &passedInItem);

private:
    QLabel *noteLabel;
    QLabel *dueLabel;
    QLabel *fontLabel;
    QTextEdit *noteEdit;
    QDateEdit *dueEdit;
    QCheckBox *displayCheck;
    QFontComboBox *fontBox;
    QPushButton *applyButton;
    QPushButton *cancelButton;

    void paintEvent(QPaintEvent *);
};

#endif // TASKLIST_OPTIONS_H
