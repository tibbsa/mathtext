/**
* @file
*
* @copyright Copyright 2015 Anthony Tibbs
* This project is released under the GNU General Public License.
*/
#ifndef __MTDOGWINDOW_H__
#define __MTDOCWINDOW_H__

#include <QPlainTextEdit>

/**
 * This class represents a single MathText document which has been opened for 
 * editing or processing.
 */
class MTDocWindow : public QPlainTextEdit
{
  Q_OBJECT

public:
  MTDocWindow();

  void newFile(void);
  bool loadFile(const QString &filename);
  bool save(void);
  bool saveAs(void);
  bool saveFile(const QString &filename);

protected:
   
private slots:

private:
  bool isUntitled;
  QString documentFilename;

};

#endif // __MTDOCWINDOW_H__
