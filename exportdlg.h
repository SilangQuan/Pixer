#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIntValidator>
#include <QLineEdit>
#include <QFileDialog>
#include "thumbnailcanvas.h"
#include "layermanager.h"

class ExportDlg : public QDialog
{
	Q_OBJECT

public:
	ExportDlg(QWidget *parent = 0);
	~ExportDlg();

private:
	void setPreviewSize(int w, int h);

	QLabel *originalSizeLabel;
	QLabel *exportSizeLabel;
	QLabel *widthLabel;
	QLineEdit *widthEdit;
	QLineEdit *heightEdit;
	QLabel *heightLabel;
	QDialogButtonBox *buttonBox;
	ThumbnailCanvas* canvas;
	QSize imageSize;
	QPixmap previewImage;

private slots:
	void widthEditChangedSlot(const QString & text);
	void heightEditChangedSlot(const QString & text);
	void accept();

signals:
	void exportImage(QString &filepath, int width, int height);
};

#endif // EXPORTDIALOG_H
