#include <iostream>
#include <omp.h>
#include "DisparityMapMaker.h"
#include "opencvReleaseFunctions.h"

DisparityMapMaker<class Type>::DisparityMapMaker(const int& windowSize, const int& dRange)
: windowSize(windowSize), dRange(dRange) {
}

DisparityMapMaker<class Type>::~DisparityMapMaker() {
}

std::shared_ptr<IplImage> DisparityMapMaker<class Type>::getDisparityMapPtr(const IplImage* leftGrayImage, const IplImage* rightGrayImage) {
	IplImage* disparityMap = cvCreateImage(cvGetSize(leftGrayImage), IPL_DEPTH_8U, 1);

	for (int i = windowSize + 1; i < disparityMap->height - windowSize; i++) {
		uchar* ptr = (uchar*)(disparityMap->imageData + i*disparityMap->widthStep);
		for (int j = windowSize + 1; j < disparityMap->width - windowSize - dRange; j++) {
			ptr[j] = (uchar)(getDisparityValue(leftGrayImage, rightGrayImage, j, i) * 255 / dRange);
		}
	}

	 TODO ��ü ���� �� ����� ���� �ӵ� �̽� ������ Ȯ��
	return std::shared_ptr<IplImage>(disparityMap, releaseImageForSharedPtr);
}

template<>
long long int DisparityMapMaker<class Type>::getDisparityValue(const IplImage* leftGrayImage, const IplImage* rightGrayImage, const int& x, const int& y) {
	long long int minSADvalue = 0x7fffffffffffffff;
	long long int disparity = 0;

#pragma omp parallel for schedule(dynamic)
	for (int d = 0; d < dRange; d++) {
		long long int value = estimationType.getValue(leftGrayImage, rightGrayImage, x, y, windowSize, d);
		if (minSADvalue > value) {
			minSADvalue = value;
			disparity = d;
		}
	}

	return disparity;
}