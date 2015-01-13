#ifndef NEWFILEDLG_H
#define NEWFILEDLG_H

#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QDebug>
#include <QDialogButtonBox>
#include "canvas.h"

class NewFileDlg : public QDialog
{
	Q_OBJECT

public:
	NewFileDlg(QWidget *parent = 0);
	~NewFileDlg();

private:
	QDialogButtonBox *buttonBox;
	QComboBox *presetBox;
	QComboBox *colormodeBox;
	QLineEdit *widthEdit;
	QLineEdit *heightEdit;

public slots:
	virtual void accept();

private slots:
void presetBoxChanged(int index);

signals:
	void configConfirmed(int width, int height, COLORMODE m);

};

#endif // NEWFILEDLG_H
