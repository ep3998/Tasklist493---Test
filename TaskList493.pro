HEADERS += \
    tasklist_main.h \
    tasklist_notes.h \
    tasklist_options.h \
    my_listwidget.h \
    tasklist_lists.h \
    tasklist_templates.h \
    templateButtonGroup.h

SOURCES += \
    tasklist_main.cpp \
    main.cpp \
    tasklist_notes.cpp \
    tasklist_options.cpp \
    my_listwidget.cpp \
    tasklist_lists.cpp \
    tasklist_templates.cpp

OTHER_FILES += \
    right_arrow.jpg \
    style.qss \
    task_groceries.xml \
    task_week.xml \
    week_icon.png \
    right_arrow.png \
    left_arrow.png \
    grocery_icon.png \
    blank_icon.png

RESOURCES += \
    images.qrc \
    templates.qrc

QT += xml
