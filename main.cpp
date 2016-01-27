#include <opencv2\opencv.hpp>

using namespace cv;

void main(void)
{
	Mat obrazek = imread("lena.jpg");

	if (obrazek.empty())
		std::cout << "failed to open img.jpg" << std::endl;
	else
		std::cout << "img.jpg loaded OK" << std::endl;

	imshow("okienko", obrazek);
	waitKey();
}