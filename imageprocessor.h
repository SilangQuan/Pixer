#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H
#include <QImage>
#include <QColor>

class ImageProcessor
{
public:
	ImageProcessor();
	~ImageProcessor();

	static void grayScale(QImage *image);
	static void flipHorizontal(QImage *image);
	static void flipVertical(QImage *image);
	static void sepia(QImage *image);
	static void sepiaGreen(QImage *image);
	static void sepiaBlue(QImage *image);
	static void pixelate(QImage *image);
	
private:
	
};

#endif // IMAGEPROCESSOR_H
