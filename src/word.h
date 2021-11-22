#ifndef WORD_H
#define WORD_H
#include "JlCompress.h"
#include <QString>
#include <QFile>
#include <QMessageBox>

class Word
{
private:
    QString html_content;
    QString word_content;
public:
    Word(QString input);
    ~Word()
    {
        QDir dir("tmp");
        dir.removeRecursively();
    }
    inline QString get_html(){return html_content;}
};

#endif // WORD_H
