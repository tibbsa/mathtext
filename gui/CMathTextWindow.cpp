#include "common-includes.h"
#include "CMathTextWindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QProcess>
#include <QDesktopServices>

#include "MathDocument.h"
#include "MathInterpreter.h"
#include "MathSourceFile.h"
#include "LaTeXRenderer.h"
#include "UEBRenderer.h"
#include "MathExceptions.h"
#include "logging.h"

CMathTextWindow::CMathTextWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}

CMathTextWindow::~CMathTextWindow()
{

}

void CMathTextWindow::on_actionFileExit_triggered(void)
{
  close();
}

void CMathTextWindow::on_actionFileConvertToBraille_triggered(void)
{
  QString inputFilename;
  QString outputFilename;
  QString inputFilters = tr("MathText Documents (*.mtx);;Text Files (*.txt);;All Files (*.*)");
  QString outputFilters = tr("Formatted Braille Documents (*.brf);;All Files (*.*)");

  inputFilename = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), inputFilters);

  if (inputFilename.isEmpty())
    return;

  QFileInfo qif(inputFilename);
  QString defaultOutputFilename = qif.path() + QDir::separator() + qif.completeBaseName() + ".BRF";
    
  outputFilename = QFileDialog::getSaveFileName(this, tr("Save Braille File"), defaultOutputFilename, outputFilters);

  if (outputFilename.isEmpty())
    return;

  MathDocument doc;
  if (!loadFile(inputFilename, doc))
    return;

  std::string output;
  UEBRenderer ueb;
  ueb.enableLineWrapping(30);
  output = ueb.renderDocument(doc);

  std::ofstream ofs;
  std::string outputFilenameString = outputFilename.toStdString();
  ofs.open(outputFilenameString.c_str());
  if (!ofs.is_open()) {
    QString errMsg;
    QTextStream ts(&errMsg);
    ts << "An error occurred while opening '" << outputFilename << "' for writing!";
    QMessageBox::critical(this, "File Save Error", errMsg);
    return;
  }

  ofs << output;
  if (!ofs.good()) {
    QString errMsg;
    QTextStream ts(&errMsg);
    ts << "An error occurred while writing to '" << outputFilename << "'!";
    QMessageBox::critical(this, "File Save Error", errMsg);
    return;
  }

  ofs.close();
  QMessageBox::information(this, "Translate to Braille", "Braille translation completed successfully.");
}

void CMathTextWindow::on_actionFileConvertToLaTeX_triggered(void)
{
  QString inputFilename;
  QString outputFilename;
  QString inputFilters = tr("MathText Documents (*.mtx);;Text Files (*.txt);;All Files (*.*)");
  QString outputFilters = tr("LaTeX Documents (*.tex *.latex);;All Files (*.*)");

  inputFilename = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), inputFilters);

  if (inputFilename.isEmpty())
    return;

  QFileInfo qif(inputFilename);
  QString defaultOutputFilename = qif.path() + QDir::separator() + qif.completeBaseName() + ".tex";

  outputFilename = QFileDialog::getSaveFileName(this, tr("Save LaTeX File"), defaultOutputFilename, outputFilters);

  if (outputFilename.isEmpty())
    return;

  MathDocument doc;
  if (!loadFile(inputFilename, doc))
    return;

  std::string output;
  LaTeXRenderer renderer;
  output = renderer.renderDocument(doc);

  std::ofstream ofs;
  std::string outputFilenameString = outputFilename.toStdString();
  ofs.open(outputFilenameString.c_str());
  if (!ofs.is_open()) {
    QString errMsg;
    QTextStream ts(&errMsg);
    ts << "An error occurred while opening '" << outputFilename << "' for writing!";
    QMessageBox::critical(this, "File Save Error", errMsg);
    return;
  }

  ofs << output;
  if (!ofs.good()) {
    QString errMsg;
    QTextStream ts(&errMsg);
    ts << "An error occurred while writing to '" << outputFilename << "'!";
    QMessageBox::critical(this, "File Save Error", errMsg);
    return;
  }

  ofs.close();
  QMessageBox::information(this, "Translate to LaTeX", "LaTeX translation completed successfully.");
}

void CMathTextWindow::on_actionFileConvertToPDF_triggered(void)
{
  QString inputFilename;
  QString outputFilename;
  QString inputFilters = tr("MathText Documents (*.mtx);;Text Files (*.txt);;All Files (*.*)");
  QString outputFilters = tr("PDF Documents (*.pdf);;All Files (*.*)");

  inputFilename = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), inputFilters);

  if (inputFilename.isEmpty())
    return;

  QFileInfo qif(inputFilename);
  QString defaultOutputFilename = qif.path() + QDir::separator() + qif.completeBaseName() + ".pdf";

  outputFilename = QFileDialog::getSaveFileName(this, tr("Save PDF File"), defaultOutputFilename, outputFilters);

  if (outputFilename.isEmpty())
    return;

  MathDocument doc;
  if (!loadFile(inputFilename, doc))
    return;

  std::string output;
  LaTeXRenderer renderer;
  output = renderer.renderDocument(doc);

  std::ofstream ofs;
  std::string intermediaryFilename = outputFilename.toStdString() + ".tex_temp";
  ofs.open(intermediaryFilename.c_str());
  if (!ofs.is_open()) {
    QString errMsg;
    QTextStream ts(&errMsg);
    ts << "An error occurred while opening temporary file '" << QString::fromStdString(intermediaryFilename) << "' for writing!";
    QMessageBox::critical(this, "File Save Error", errMsg);
    return;
  }

  ofs << output;
  if (!ofs.good()) {
    QString errMsg;
    QTextStream ts(&errMsg);
    ts << "An error occurred while writing to temporary file '" << QString::fromStdString(intermediaryFilename) << "'!";
    QMessageBox::critical(this, "File Save Error", errMsg);
    return;
  }

  ofs.close();

  QString program = "C:\\Program Files (x86)\\MiKTeX 2.9\\miktex\\bin\\pdflatex.exe";
  QStringList arguments;

  arguments << "-aux-directory=temp";
  arguments << "-c-style-errors";
  arguments << "-enable-installer";
  arguments << "-interaction=nonstopmode";
  arguments << "-halt-on-error";
  arguments << "-job-name=" + QDir::toNativeSeparators(QFileInfo(outputFilename).completeBaseName());
  arguments << "-output-directory=" + QDir::toNativeSeparators(QFileInfo(outputFilename).absolutePath());
  arguments << QString::fromStdString(intermediaryFilename);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  unsigned generationCount = 0;
  for (unsigned generationCount = 0; generationCount < 3; generationCount++) {
    QProcess pdfLatex;
    pdfLatex.setProcessChannelMode(QProcess::MergedChannels);

    pdfLatex.start(program, arguments);
    if (!pdfLatex.waitForStarted()) {
      QApplication::restoreOverrideCursor();
      QString errMsg;
      QTextStream ts(&errMsg);
      ts << "An error occurred while trying to launch the PDF conversion.";
      QMessageBox::critical(this, "File Save Error", errMsg);
      return;
    }

    while (!pdfLatex.waitForFinished(500)) {
      QApplication::processEvents();
    }

    QApplication::restoreOverrideCursor();

    QByteArray outputData = pdfLatex.readAllStandardOutput();
    QString outputString = QString(outputData);
    ///QMessageBox::information(this, "LaTeX Output", outputString);

    if (outputString.contains("Label(s) may have changed") ||
      outputString.contains("There were undefined references")) {

      // re-run PdfLatex
      continue;
    }

    break;
  }

  if (generationCount == 3) {
    QMessageBox::warning(this, "Save to PDF File", "There may have been some problems generating the PDF file. If the output is not as you expected, please contact the developer for assistance.");
  }

  QFile::remove(QString::fromStdString(intermediaryFilename));
  QUrl pdfUrl = QUrl::fromLocalFile(outputFilename);
  QDesktopServices::openUrl(pdfUrl);
}

bool CMathTextWindow::loadFile(const QString &inputFilename, MathDocument &doc)
{
  try {
    MathSourceFile srcFile;
    MathInterpreter interp(srcFile, doc);

    srcFile.loadFromFile(inputFilename.toStdString());
    std::vector<std::string> renderCommands;
    LaTeXRenderer::getInterpreterCommandList(renderCommands);
    UEBRenderer::getInterpreterCommandList(renderCommands);
    interp.registerCommands(renderCommands);

    try {
      interp.interpret();
    }
    catch (MathInterpreterException &e) {
      QString msg;
      QTextStream ts(&msg);

      ts << "An error was encountered while interpreting your document and the process had to be stopped.";
      if (!interp.haveMessages()) {
        ts << "\n\nNo further information is available. Please contact the developer for assistance.";
      }
      else {
        const std::vector<MathInterpreterMsg> &msgs = interp.getMessages();
        ts << "\n" << msgs.size() << " issues were found:\n\n";
        BOOST_FOREACH(const MathInterpreterMsg m, msgs) {
          ts << "- " << QString(m.getFormattedMessage().c_str()) << "\n";
        }
      }

      QMessageBox::critical(this, "Interpreation Errors", msg);
      return false;
    }

    if (interp.haveMessages()) {
      QString msg;
      QTextStream ts(&msg);

      const std::vector<MathInterpreterMsg> &msgs = interp.getMessages();
      ts << "\n" << msgs.size() << " issues were found:\n";
      BOOST_FOREACH(const MathInterpreterMsg m, msgs) {
        ts << "- " << QString(m.getFormattedMessage().c_str()) << "\n\n";
      }

      QMessageBox::information(this, "Interpreation Notes", msg);
    }
  }
  catch (MathDocumentException &e) {
    std::string const *error = boost::get_error_info<mdx_error_info>(e);
    std::string const *file = boost::get_error_info<mdx_filename_info>(e);
    std::string const *desc = boost::get_error_info<mdx_liberrmsg_info>(e);
    std::string boost_diagnostics = boost::diagnostic_information(e);

    LOG_FATAL << "=======================================" << endl;
    LOG_FATAL << boost::diagnostic_information(e);
    LOG_FATAL << "=======================================" << endl;

    std::string errorMessage;
    if (error)
      errorMessage = *error;
    else
      errorMessage = "An unspecified error occurred";

    if (file)
      errorMessage += " [in file " + *file + "]";

    if (desc)
      errorMessage += " - " + *desc;

    LOG_FATAL << errorMessage;
    LOG_FATAL << "============================================";

    QMessageBox::critical(this, "Translation Error", QString(errorMessage.c_str()));
    return false;
  }

  return true;
}
