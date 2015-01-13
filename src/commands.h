#ifndef COMMANDS_H
#define COMMANDS_H

#include <QObject>
#include <QUndoCommand>
#include <QImage>

class PaintCommand : public QUndoCommand
{
public:
	PaintCommand(QImage *image,QUndoCommand *parent = 0);
	~PaintCommand();
	void setNewImage(QImage *image);
	void undo();
	void redo();

private:
	QImage* pImage;
	QImage orginImage;
	QImage newImage;
};

class EraserCommand : public QUndoCommand
{
public:
	EraserCommand(QImage *image, QUndoCommand *parent = 0);
	~EraserCommand();
	void setNewImage(QImage *image);
	void undo();
	void redo();

private:
	QImage* pImage;
	QImage orginImage;
	QImage newImage;
};

class FillCommand : public QUndoCommand
{
public:
	FillCommand(QImage *image, QUndoCommand *parent = 0);
	~FillCommand();
	void setNewImage(QImage *image);
	void undo();
	void redo();

private:
	QImage* pImage;
	QImage orginImage;
	QImage newImage;
};

class DodgeCommand : public QUndoCommand
{
public:
	DodgeCommand(QImage *image, QUndoCommand *parent = 0);
	~DodgeCommand();
	void setNewImage(QImage *image);
	void undo();
	void redo();

private:
	QImage* pImage;
	QImage orginImage;
	QImage newImage;
};

class DeepenCommand : public QUndoCommand
{
public:
	DeepenCommand(QImage *image, QUndoCommand *parent = 0);
	~DeepenCommand();
	void setNewImage(QImage *image);
	void undo();
	void redo();

private:
	QImage* pImage;
	QImage orginImage;
	QImage newImage;
};



#endif // COMMANDS_H
