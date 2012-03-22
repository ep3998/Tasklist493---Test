#include "tasklist_main.h"
#include "tasklist_options.h"
#include "tasklist_notes.h"
#include <QtGui>
#include <QFontComboBox>
#include <QListWidgetItem>
#include <QDate>

QListWidgetItem currItem;

//Repaint event for syncing information
void tasklist_options::paintEvent(QPaintEvent *){
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

//Sends data to note pane when "apply"
//button is clicked
void tasklist_options::applyClicked(){
    bool displayBool = displayCheck->isChecked();

    QString noteInformation = noteEdit->toPlainText();
    QDate noteDate;
    noteDate.setDate(dueEdit->date().year(),
                     dueEdit->date().month(),
                     dueEdit->date().day());
    QFont noteFont = fontBox->currentFont();
    QListWidgetItem itemChanges;
    itemChanges.setFont(noteFont);
    itemChanges.setData(32, QVariant(noteInformation));
    itemChanges.setData(33, QVariant(displayBool));
    itemChanges.setData(34, QVariant(noteDate));

    emit noteInfoSent(itemChanges);
}

//Closes pane if cancel is clicked
void tasklist_options::cancelClicked(){
    fontBox->setCurrentFont(currItem.font());
    noteEdit->setText(currItem.data(32).toString());
    displayCheck->setChecked(currItem.data(33).toBool());
    QDate inDate = currItem.data(34).toDate();
    dueEdit->setDate(inDate);

    emit cancelPane();
}

//Updates option pane's information if selection
//on note pane is changed
void tasklist_options::itemSelectDataIn(QListWidgetItem &passedInItem){
    fontBox->setCurrentFont(passedInItem.font());
    noteEdit->setText(passedInItem.data(32).toString());
    displayCheck->setChecked(passedInItem.data(33).toBool());
    QDate inDate = passedInItem.data(34).toDate();
    dueEdit->setDate(inDate);

    currItem = passedInItem;
}

//Creates layout tree for option pane
tasklist_options::tasklist_options(QWidget *parent) :
    QWidget(parent)
{
    noteLabel = new QLabel("Notes:");
    noteEdit = new QTextEdit();

    dueLabel = new QLabel("Due Date:");
    dueEdit = new QDateEdit();

    fontLabel = new QLabel("Font:");
    fontBox = new QFontComboBox();

    displayCheck = new QCheckBox("Display note in list?");
    applyButton = new QPushButton("Apply");
    cancelButton = new QPushButton("Cancel");

    QVBoxLayout *rightMain = new QVBoxLayout;
    QHBoxLayout *dateLayout = new QHBoxLayout;
    QHBoxLayout *fontLayout = new QHBoxLayout;
    QHBoxLayout *butLayout = new QHBoxLayout;
    rightMain->addWidget(noteLabel);
    rightMain->addWidget(noteEdit);
    rightMain->addWidget(displayCheck);

    dateLayout->addWidget(dueLabel);
    dateLayout->addWidget(dueEdit);
    dateLayout->setAlignment(Qt::AlignLeft);

    fontLayout->addWidget(fontLabel);
    fontLayout->addWidget(fontBox);
    fontLayout->setAlignment(Qt::AlignLeft);

    butLayout->addWidget(applyButton);
    butLayout->addWidget(cancelButton);

    rightMain->addLayout(dateLayout);
    rightMain->addLayout(fontLayout);
    rightMain->addLayout(butLayout);

    QDate currDate = QDate::currentDate();
    dueEdit->setCalendarPopup(true);
    dueEdit->setDate(currDate);
    //dueEdit->setMinimumDate(currDate);
    //removed for dates from previous lists being "past"

    this->setLayout(rightMain);
    this->setParent(parent);
    connect(applyButton, SIGNAL(clicked()), this, SLOT(applyClicked()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelClicked()));
}
