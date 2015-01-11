#include "commands.h"

//Paint Command
PaintCommand::PaintCommand(QImage *image, QUndoCommand *parent)
: QUndoCommand(parent)
{
	pImage = image;
	orginImage = *image;
}

PaintCommand::~PaintCommand()
{

}

void PaintCommand::undo()
{
	*pImage = orginImage;

	setText(QObject::tr("Paint"));
}

void PaintCommand::setNewImage(QImage *image)
{
	newImage = *image;
}

void PaintCommand::redo()
{
	*pImage = newImage;
	setText(QObject::tr("Paint"));
}

//Eraser Command
EraserCommand::EraserCommand(QImage *image, QUndoCommand *parent)
: QUndoCommand(parent)
{
	pImage = image;
	orginImage = *image;
}

EraserCommand::~EraserCommand()
{

}

void EraserCommand::undo()
{
	*pImage = orginImage;

	setText(QObject::tr("Eraser"));
}

void EraserCommand::setNewImage(QImage *image)
{
	newImage = *image;
}

void EraserCommand::redo()
{
	*pImage = newImage;
	setText(QObject::tr("Eraser"));
}

//Fill Command
FillCommand::FillCommand(QImage *image, QUndoCommand *parent)
: QUndoCommand(parent)
{
	pImage = image;
	orginImage = *image;
}

FillCommand::~FillCommand()
{

}

void FillCommand::undo()
{
	*pImage = orginImage;

	setText(QObject::tr("Fill Selection"));
}

void FillCommand::setNewImage(QImage *image)
{
	newImage = *image;
}

void FillCommand::redo()
{
	*pImage = newImage;
	setText(QObject::tr("Fill Selection"));
}

//Dodge Command
DodgeCommand::DodgeCommand(QImage *image, QUndoCommand *parent)
: QUndoCommand(parent)
{
	pImage = image;
	orginImage = *image;
}

DodgeCommand::~DodgeCommand()
{

}

void DodgeCommand::undo()
{
	*pImage = orginImage;

	setText(QObject::tr("Dodge"));
}

void DodgeCommand::setNewImage(QImage *image)
{
	newImage = *image;
}

void DodgeCommand::redo()
{
	*pImage = newImage;
	setText(QObject::tr("Dodge"));
}

//Deepen Command
DeepenCommand::DeepenCommand(QImage *image, QUndoCommand *parent)
: QUndoCommand(parent)
{
	pImage = image;
	orginImage = *image;
}

DeepenCommand::~DeepenCommand()
{

}

void DeepenCommand::undo()
{
	*pImage = orginImage;

	setText(QObject::tr("Deepen"));
}

void DeepenCommand::setNewImage(QImage *image)
{
	newImage = *image;
}

void DeepenCommand::redo()
{
	*pImage = newImage;
	setText(QObject::tr("Deepen"));
}
