#include "word.h"

Word::Word(QString input)
{
    JlCompress::extractDir(input, "tmp");
    QFile word_file("tmp/word/document.xml");
    if(!word_file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(nullptr, QObject::tr("预览Word"), QObject::tr("无法预览该文件！ "));
        return;
    }

    word_content = word_file.readAll();
    for(qsizetype i=0; i<word_content.length(); i++)
    {
        if(word_content.mid(i, 6) == "</w:p>")
        {
            html_content += "</w:p>";
            html_content += "<br />";
            i += 5;
        }
        else
            html_content += word_content[i];
    }
    word_file.close();
}
