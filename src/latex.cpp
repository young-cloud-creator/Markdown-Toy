#include "latex.h"

QString preProcess(QString &s)
{
    QString result;
    int size = s.length();
    for(int i=0; i<size; i++)
    {
        if(s[i]=='\t')
            result += "    ";
        else
            result += s[i];
    }
    return result;
}

Latex::Latex(QString &input)
    :tex_content(preProcess(input))
{
    bool is_start = false;
    bool is_in_para = false;
    int len = tex_content.length();

    for(int i=0; i<len; i++)
    {
        if(tex_content[i] == '%')
        {
            bool has_line = false;
            if(i==0 || tex_content[i-1]=='\n')
                has_line = true;
            i++;
            for(;i<len;i++)
            {
                if(tex_content[i]=='\n')
                {
                    if(has_line)
                    {
                        if(is_in_para)
                            html_content += "</p>";
                        html_content += "<p>";
                        is_in_para = true;
                    }
                    else
                        i--;
                    break;
                }
            }
        }
        else if(tex_content.mid(i, 15) == "\\begin{comment}")
        {
            int com_line = 0;
            if(i==0 || tex_content[i-1]=='\n')
                com_line++;
            i+=15;
            for(;i<len;i++)
            {
                if(tex_content[i] == '\n')
                    com_line++;
                else if(tex_content.mid(i,13) =="\\end{comment}")
                {
                    i += 12;
                    break;
                }
            }
            if(i+1<len && tex_content[i+1]=='\n')
            {
                com_line++;
                i++;
            }
            if(com_line>2)
            {
                if(is_in_para)
                    html_content += "</p>";
                html_content += "<p>";
                is_in_para = true;
            }
        }
        else if(tex_content.mid(i, 2)=="\\\\")
        {
            html_content += "<br />";
            i++;
        }
        else if(tex_content.mid(i, 9)=="\\newline "||tex_content.mid(i, 9)=="\\newline\n")
        {
            html_content += "<br />";
            i += 8;
        }
        else if(tex_content.mid(i, 5)=="\\par "||tex_content.mid(i, 5)=="\\par\n")
        {
            if(is_in_para)
                html_content += "</p>";
            html_content += "<p>";
            is_in_para = true;
            i += 4;
        }
        else if(tex_content.mid(i, 2)=="\n\n")
        {
            if(is_in_para)
                html_content += "</p>";
            html_content += "<p>";
            is_in_para = true;
            i++;
        }
        else if(tex_content[i]=='\n')
            continue;
        else if(!is_start)
        {
            if(tex_content.mid(i, 14) == "\\documentclass")
            {
                i += 14;
                for(; i<len; i++)
                {
                    if(tex_content[i]=='}')
                        break;
                }
            }
            else if(tex_content.mid(i, 11) == "\\usepackage")
            {
                i += 11;
                for(; i<len; i++)
                {
                    if(tex_content[i]=='}')
                        break;
                }
            }
            else if(tex_content.mid(i,16) == "\\begin{document}")
            {
                is_start = true;
                i += 15;
            }
            else if(tex_content.mid(i, 7) == "\\title{")
            {
                i += 7;
                html_content += "<p><center><b><font size=\"10\">";
                for(; i<len; i++)
                {
                    if(tex_content[i]=='}')
                        break;
                    else
                        html_content += tex_content[i];
                }
                html_content += "</font></b></center></p>";
            }
            else if(tex_content.mid(i, 8) == "\\author{")
            {
                i += 8;
                html_content += "<p><center><font size=\"5\">";
                for(; i<len; i++)
                {
                    if(tex_content[i]=='}')
                        break;
                    else
                        html_content += tex_content[i];
                }
                html_content += "</font></center></p>";
            }
            else if(tex_content.mid(i, 6) == "\\date{")
            {
                i += 6;
                html_content += "<p><center><b><font size=\"3\">";
                for(; i<len; i++)
                {
                    if(tex_content[i]=='}')
                        break;
                    else
                        html_content += tex_content[i];
                }
                html_content += "</font></b></center></p>";
            }
        }
        else
        {
            if(tex_content.mid(i, 14) == "\\end{document}")
            {
                if(is_in_para)
                    html_content += "</p>";
                return;
            }
            else if(tex_content.mid(i, 8) == "\\textbf{")
            {
                i += 8;
                html_content += "<b>";
                for(; i<len; i++)
                {
                    if(tex_content[i]=='}')
                         break;
                     else
                         html_content += tex_content[i];
                }
                html_content += "</b>";
            }
            else if(tex_content.mid(i, 6) == "\\emph{")
            {
                i += 6;
                html_content += "<em>";
                for(; i<len; i++)
                {
                    if(tex_content[i]=='}')
                         break;
                     else
                         html_content += tex_content[i];
                }
                html_content += "</em>";
            }
            else if(tex_content.mid(i, 9) == "\\section{")
            {
                i+=9;
                if(is_in_para)
                    html_content += "</p>";
                is_in_para = false;
                html_content += "<p><center><b><font size=\"6\">";
                for(; i<len; i++)
                {
                    if(tex_content[i]=='}')
                        break;
                    else
                        html_content += tex_content[i];
                }
                html_content += "</font></b></center></p>";
            }
            else if(tex_content.mid(i, 12) == "\\subsection{")
            {
                i+=12;
                if(is_in_para)
                    html_content += "</p>";
                is_in_para = false;
                html_content += "<p><b><font size=\"5\">";
                for(; i<len; i++)
                {
                    if(tex_content[i]=='}')
                        break;
                    else
                        html_content += tex_content[i];
                }
                html_content += "</font></b></p>";
            }
            else if(tex_content.mid(i, 15) == "\\subsubsection{")
            {
                i+=15;
                if(is_in_para)
                    html_content += "</p>";
                is_in_para = false;
                html_content += "<p><b><font size=\"4\">";
                for(; i<len; i++)
                {
                    if(tex_content[i]=='}')
                        break;
                    else
                        html_content += tex_content[i];
                }
                html_content += "</font></b></p>";
            }
            else if(tex_content.mid(i, 11) == "\\paragraph{")
            {
                i+=11;
                if(is_in_para)
                    html_content += "</p>";
                is_in_para = true;
                html_content += "<p>";
                for(; i<len; i++)
                {
                    if(tex_content[i]=='}')
                        break;
                    else
                        html_content += tex_content[i];
                }
            }
            else if(tex_content.mid(i, 14) == "\\subparagraph{")
            {
                i+=14;
                if(is_in_para)
                    html_content += "</p>";
                is_in_para = true;
                html_content += "<p>";
                html_content += "&nbsp;&nbsp;&nbsp;&nbsp;";
                for(; i<len; i++)
                {
                    if(tex_content[i]=='}')
                        break;
                    else
                        html_content += tex_content[i];
                }
            }
            else if(tex_content.mid(i, 13) == "\\begin{quote}")
            {
                i+=13;
                html_content += "<blockquote>";
                for(;i<len;i++)
                {
                    if(tex_content.mid(i,11) =="\\end{quote}")
                    {
                        i += 10;
                        break;
                    }
                    if(tex_content[i]=='\n')
                        continue;
                    else if(tex_content[i]==' ')
                        html_content += "&nbsp;";
                    else html_content += tex_content[i];
                }
                html_content += "</blockquote>";
            }
            else
                html_content += tex_content[i];
        }
    }
}
