/*
  QPyConsole.h

  QConsole specialization for Python

  (C) 2006, Mondrian Nuessle, Computer Architecture Group, University of Mannheim, Germany

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


  nuessle@uni-mannheim.de

*/

#ifndef QPYCONSOLE_H
#define QPYCONSOLE_H

#include "qconsole.h"

/**An emulated singleton console for Python within a Qt application (based on the QConsole class)
 *@author Mondrian Nuessle
 */

class QPyConsole : public QConsole
{
public:
    //destructor
    ~QPyConsole();

    //get the QPyConsole instance
    static QPyConsole *getInstance(QWidget *parent = NULL,
                                   const QString& welcomeText = "");

    void printHistory();

protected:
    //give suggestions to complete a command (not working...)
    QStringList suggestCommand(const QString &cmd, QString& prefix);

    //private constructor
    QPyConsole(QWidget *parent = NULL,
               const QString& welcomeText = "");

    //execute a validated command
    QString interpretCommand(const QString &command, int *res);

    void setNormalPrompt(bool display) { setPrompt(">>", display); }
    void setMultilinePrompt(bool display) { setPrompt("...", display); }

private:

    //The instance
    static QPyConsole *theInstance;

private:
    // function to check if current command compiled and if not hinted for a multiline statement
    bool py_check_for_unexpected_eof();

    // string holding the current command
    QString command;

    // number of lines associated with current command
    int lines;
};

#endif
