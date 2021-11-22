#include "markdown.h"
using namespace std;

QString rm_blank_space(QString &s)
{
    QString result;
    int size = s.length();
    bool str_start = false;
    for(int i=0; i<size; i++)
    {
        if(s[i]==' ' || s[i]=='\t')
        {
            if(!str_start)
                continue;
            else
            {
                if(s[i]=='\t')
                    result += "    ";
                else
                    result += s[i];
            }
        }
        else
        {
            str_start = true;
            result += s[i];
        }
    }
    return result;
}

pair<bool, QString> Markdown::preProcess(QString &s)
{
    if(s.isEmpty())
        return make_pair(false, s);

    bool is_nested = false;
    if(s[0]=='\t')
        is_nested = true;

    return make_pair(is_nested, rm_blank_space(s));
}

bool process_title_sign(QString &str, pair<int, QString> &result)
{
    int title_num = 0;
    for(int i=0; i<str.length(); i++)
    {
        if(str[i] == '#')
            title_num++;
        else if(str[i] == ' ')
        {
            if(title_num>0 && title_num<7)
            {
                result = make_pair(h1+title_num-1, str.mid(i+1));
                return true;
            }
            return false;
         }
         else
             return false;
    }
    return false;
}

bool process_ol_sign(QString &str, pair<int, QString> &result)
{
    for(int i=0; i<str.length(); i++)
    {
        if(str[i] == '.' && i<str.length()-1 && str[i+1] == ' ')
        {
            result = make_pair(ol, str.mid(i+2));
            return true;
        }
        else if(!(str[i]>='0' && str[i]<='9'))
            return false;
    }
    return false;
}

bool process_quote_sign(QString &str, pair<int, QString> &result)
{
    int quote_num = 0;
    for(int i=0; i<str.length(); i++)
    {
        if(str[i] == '>')
            quote_num++;
        else if(str[i] == ' ')
        {
            result = make_pair(quote+quote_num-1, str.mid(i+1));
            return true;
        }
        else
            return false;
    }
    return false;
}

pair<int, QString> Markdown::processLine(QString str)
{
    if(str.isEmpty())
        return make_pair(plainText, "");

    pair<int, QString> result;

    if(str[0]=='#' && process_title_sign(str, result))
        return result;

    if(str.left(3) == "```" || str.left(3) == "~~~" )
        return make_pair(precode, "");

    if(str.left(2) == "* " || str.left(2) == "- " || str.left(2) == "+ ")
        return make_pair(ul, str.mid(2));

    if(str[0]>='0' && str[0]<='9' && process_ol_sign(str, result))
        return result;

    if(str[0] == '>' && process_quote_sign(str, result))
        return result;

    return make_pair(plainText, str);
}

void Markdown::processInsert(node* root, QString str)
{
    bool is_code = false;
    bool is_b = false;
    bool is_em = false;
    root->children.push_back(new node(plainText));
    int len = str.length();
    for(int i=0; i<len; i++)
    {
        if(str[i] == '`')
        {
            if(is_code)
                root->children.push_back(new node(plainText));
            else
                root->children.push_back(new node(code));
            is_code = !is_code;
        }

        else if(!is_code && ((str[i]=='*' && i<len-1 && str[i+1]=='*')
                             ||(str[i]=='_' && i<len-1 && str[i+1]=='_')))
        {
            if(is_b)
                root->children.push_back(new node(plainText));
            else
                root->children.push_back(new node(b));
            is_b = !is_b;
            i++;
        }

        else if(!is_code && (str[i]=='*' || str[i]=='_'))
        {
            if(is_em)
                root->children.push_back(new node(plainText));
            else
                root->children.push_back(new node(em));
            is_em = !is_em;
        }

        else if(!is_code && (str[i]=='!' && i<len-1 && str[i+1]=='['))
        {
            root->children.push_back(new node(img));
            i += 2;
            for(; i<len && str[i]!=']'; i++)
                root->children.back()->text[0] += str[i];
            i += 2;
            for(; i<len && str[i]!=')'; i++)
                root->children.back()->text[1] += str[i];

            root->children.push_back(new node(plainText));
        }

        else if(!is_code && str[i]=='[')
        {
            root->children.push_back(new node(href));
            i += 1;
            for(; i<len && str[i]!=']'; i++)
                root->children.back()->text[0] += str[i];
            i += 2;
            for(; i<len && str[i]!=')'; i++)
                root->children.back()->text[1] += str[i];

            root->children.push_back(new node(plainText));
        }

        else
            root->children.back()->text[0] += str[i];
    }
}

void Markdown::Transform2html(node* root)
{
    html_content += front_tag[root->type];

    switch(root->type)
    {
        case href:
            html_content += "<a href=\"";
            html_content += root->text[1];
            html_content += "\">";
            html_content += root->text[0];
            html_content += "</a>";
        break;
        case img:
            html_content +=  "<img src=\"";
            html_content += root->text[1];
            html_content += "\" alt=\"";
            html_content += root->text[0];
            html_content += "\">";
        break;
        default:
            html_content += root->text[0];
    }

    for(unsigned long long i=0; i<root->children.size(); i++)
        Transform2html(root->children[i]);

    html_content += back_tag[root->type];
}

Markdown::Markdown(QString &input)
    :root(new node(plainText))
{
    md_content = input;
    QTextStream in(&md_content);
    bool is_blockcode = false;
    root->children.push_back(new node(para));

    QString line = in.readLine();
    while(!line.isNull())
    {
        QString old_line(line);
        pair<bool, QString> pre_pair = preProcess(line);
        line = pre_pair.second;
        bool is_nested = pre_pair.first;

        if(!is_blockcode && line.isEmpty())
            root->children.push_back(new node(para));

        pair<int, QString> processed_line = processLine(line);

        if(processed_line.first == precode)
        {
            if(!is_blockcode)
                root->children.back()->children.push_back(new node(precode));
            is_blockcode = !is_blockcode;
        }

        else if(is_blockcode)
        {
            root->children.back()->children.back()->text[0] += old_line;
            root->children.back()->children.back()->text[0] += '\n';
        }

        else if(!processed_line.second.isEmpty() && processed_line.first == plainText)
        {
            if(processed_line.second.length()>1 && processed_line.second.mid(line.length()-2)=="  ")
                processed_line.second = processed_line.second.mid(0, processed_line.second.length()-2)+front_tag[br];

            processInsert(root->children.back(), processed_line.second);
        }

        else if(processed_line.first>=h1 && processed_line.first<=h6)
        {
            root->children.push_back(new node(processed_line.first));
            processInsert(root->children.back(), processed_line.second);
            root->children.push_back(new node(para));
        }

        else if(processed_line.first == ul)
        {
            if(is_nested && !root->children.back()->children.empty()
                    && root->children.back()->children.back()->type == ul)
            {
                node *cur_li = root->children.back()->children.back()->children.back();
                cur_li->children.push_back(new node(ul));
                node *cur_ul = cur_li->children.back();
                cur_ul->children.push_back(new node(li));
                processInsert(cur_ul->children.back(), processed_line.second);
            }

            else
            {
                if(root->children.back()->children.empty()
                        || root->children.back()->children.back()->type != ul)
                    root->children.back()->children.push_back(new node(ul));

                node *cur_ul = root->children.back()->children.back();
                cur_ul->children.push_back(new node(li));
                processInsert(cur_ul->children.back(), processed_line.second);
            }
        }

        else if(processed_line.first == ol)
        {
            if(is_nested && !root->children.back()->children.empty()
                    && root->children.back()->children.back()->type == ol)
            {
                node *cur_li = root->children.back()->children.back()->children.back();
                cur_li->children.push_back(new node(ol));
                node *cur_ol = cur_li->children.back();
                cur_ol->children.push_back(new node(li));
                processInsert(cur_ol->children.back(), processed_line.second);
            }

            else
            {
                if(root->children.back()->children.empty()
                        || root->children.back()->children.back()->type != ol)
                    root->children.back()->children.push_back(new node(ol));

                node *cur_ol = root->children.back()->children.back();
                cur_ol->children.push_back(new node(li));
                processInsert(cur_ol->children.back(), processed_line.second);
            }
        }

        else if(processed_line.first >= quote)
        {
            node* cur = root->children.back();
            for(int i=0; i<=processed_line.first-quote; i++)
            {
                cur->children.push_back(new node(quote));
                cur = cur->children.back();
            }
            processInsert(cur, processed_line.second);
        }

        line = in.readLine();
    }
    Transform2html(root);
}


void Markdown::clear(node* root)
{
    if(!root)
        return;
    unsigned long long child_size = root->children.size();
    for(unsigned long long i=0; i<child_size; i++)
        clear(root->children[i]);
    delete root;
}
Markdown::~Markdown()
{
    if(root)
        clear(root);
}
