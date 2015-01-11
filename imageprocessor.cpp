#include "imageprocessor.h"

ImageProcessor::ImageProcessor()
{

}

ImageProcessor::~ImageProcessor()
{

}

void ImageProcessor::grayScale(QImage *image)
{
	QColor tmpColor;

	for (int x = 0; x < image->width(); x++){
		for (int y = 0; y < image->height(); y++){
			tmpColor = QColor::fromRgba(image->pixel(x, y));
			int average = (tmpColor.red() + tmpColor.green() + tmpColor.blue()) / 3;
			image->setPixel(x, y, qRgba(average, average, average, tmpColor.alpha()));
		}
	}
}

void ImageProcessor::flipHorizontal(QImage *image)
{
	*image = image->mirrored(true, false);
}

void ImageProcessor::flipVertical(QImage *image)
{
	*image = image->mirrored();
}


void ImageProcessor::sepia(QImage *image)
{
	QColor tmpColor;
	for (int x = 0; x < image->width(); x++){
		for (int y = 0; y < image->height(); y++){
			tmpColor = QColor::fromRgba(image->pixel(x, y));
			int average = (tmpColor.red() + tmpColor.green() + tmpColor.blue()) / 3;
			int sepia = average + 50; //! sets the sepia value to a boosted grey value
			sepia = qBound(0, sepia, 255);
			int blue = average - 50; //! sets the blue value into a darker grey value
			blue = qBound(0, average, 255);
			image->setPixel(x, y, qRgba(sepia, sepia - 20, blue, tmpColor.alpha()));
		}
	}
}

void ImageProcessor::sepiaGreen(QImage *image)
{
	QColor tmpColor;
	for (int x = 0; x < image->width(); x++){
		for (int y = 0; y < image->height(); y++){
			tmpColor = QColor::fromRgba(image->pixel(x, y));
			int average = (tmpColor.red() + tmpColor.green() + tmpColor.blue()) / 3;
			int sepia = average + 50; //! sets the sepia value to a boosted grey value
			sepia = qBound(0, sepia, 255);
			int red = average - 50; //! sets the blue value into a darker grey value
			red = qBound(0, average, 255);
			image->setPixel(x, y, qRgba(red, sepia, sepia - 20, tmpColor.alpha()));
		}
	}
}


void ImageProcessor::sepiaBlue(QImage *image)
{
	QColor tmpColor;
	for (int x = 0; x < image->width(); x++){
		for (int y = 0; y < image->height(); y++){
			tmpColor = QColor::fromRgba(image->pixel(x, y));
			int average = (tmpColor.red() + tmpColor.green() + tmpColor.blue()) / 3;
			int sepia = average + 50; //! sets the sepia value to a boosted grey value
			sepia = qBound(0, sepia, 255);
			int red = average - 50; //! sets the blue value into a darker grey value
			red = qBound(0, average, 255);
			image->setPixel(x, y, qRgba(red, sepia - 20, sepia, tmpColor.alpha()));
		}
	}
}

void ImageProcessor::pixelate(QImage *image)
{
	*image = image->scaled(0.125 *image->width(), 0.125 * image->height(), Qt::KeepAspectRatio, Qt::FastTransformation); //! shrinks the current image by a value of 8 and overwrites it, high resolution data is lost
	*image = image->scaled(8 * image->width(), 8 * image->height(), Qt::KeepAspectRatio, Qt::FastTransformation); //! takes the shrunken image and blows it back up to its original size
}