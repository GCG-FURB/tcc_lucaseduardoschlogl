#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <time.h>
#include <sstream>
#include <string>
#include <vector>
#include <assert.h>

using namespace cv;
using namespace std;

void generatePatterns(int numberOfPatterns, int patternWidth, int patternHeight, string filePath) {
	printf("Generating patterns\n");
	int counter = 1;
	for (int i = 1; i < numberOfPatterns + 1; i++) {
		Mat generatedPattern(patternHeight, patternWidth, CV_8UC3, Scalar(255, 255, 255));

		int numberOfStripes = pow(2, i);
		int stripeWidth = patternWidth / pow(2, i);

		for (int j = 0; j < numberOfStripes; j++) {
			int xMargin = j * stripeWidth;
			cv::Rect rect(xMargin * 2, 0, stripeWidth, patternHeight);
			cv::rectangle(generatedPattern, rect, cv::Scalar(0, 0, 0), CV_FILLED);
		}

		std::stringstream stringStreamPattern;
		stringStreamPattern << filePath << "\\" << counter << ".jpg";
		std::string stringPath = stringStreamPattern.str();

	//	printf("Writing: %s", stringPath);
		imwrite(stringPath, generatedPattern);
		counter++;
		Mat invertedPattern = ~generatedPattern;

		std::stringstream stringStreamPatternInverted;
		stringStreamPatternInverted << filePath << "\\" << counter << ".jpg";
		std::string stringPathInverted = stringStreamPatternInverted.str();

	//	printf("Writing: %s", stringPathInverted);
		imwrite(stringPathInverted, invertedPattern);
		counter++;

		//imshow("pattern", generatedPattern);
		//imshow("inverted pattern", invertedPattern);
		//cvWaitKey(0);

	}
	printf("Finished patterns\n");
}

void getRoi() {

	cvStartWindowThread();
	cvNamedWindow("corners", CV_WINDOW_NORMAL);
	cvSetWindowProperty("corners", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

	cvWaitKey(100);

	CvCapture* capture = cvCaptureFromCAM(CV_CAP_ANY);
	if (!capture) {
		fprintf(stderr, "Camera not found \n");
		getchar();
		exit(0);
	}
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 480);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 640);

	Mat projectionTopCornerImg = cvLoadImage("C:\\temp\\topCorner.jpg");
	imshow("corners", projectionTopCornerImg);

	cvWaitKey(4000);
	bool tryAgain = true;
	int count = 0;
	Mat topCornerImg;
	do {
		cvWaitKey(250);

		cvGrabFrame(capture);
		topCornerImg = cvRetrieveFrame(capture);

		Mat topCornerImgGray;
		Point topCornerPoint;

		cvtColor(topCornerImg, topCornerImgGray, COLOR_BGR2GRAY);
		GaussianBlur(topCornerImgGray, topCornerImgGray, Size(9, 9), 2, 2);

		std::vector<Vec3f> circlesTopCorner;

		HoughCircles(topCornerImgGray, circlesTopCorner, CV_HOUGH_GRADIENT, 1, topCornerImgGray.rows / 8, 100, 50, 0, 0);

		if (circlesTopCorner.size() == 1) {
			topCornerPoint = Point(cvRound(circlesTopCorner[0][0]), cvRound(circlesTopCorner[0][1]));
			tryAgain = false;
		}
		else {
			printf("Ocorreu um erro ao calibrar.\n");
		}
		if (tryAgain && count == 10) {
			tryAgain = false;
		}
		count++;

	} while (tryAgain);

	Mat projectionBottomCornerImg = cvLoadImage("C:\\temp\\bottomCorner.jpg");
	imshow("corners", projectionBottomCornerImg);

	cvWaitKey(4000);
	tryAgain = true;
	count = 0;
	Mat bottomCornerImg;

	do {
		cvWaitKey(250);

		cvGrabFrame(capture);
		bottomCornerImg = cvRetrieveFrame(capture);

		Mat bottomCornerImgGray;
		Point bottomCornerPoint;

		cvtColor(bottomCornerImg, bottomCornerImgGray, COLOR_BGR2GRAY);
		GaussianBlur(bottomCornerImgGray, bottomCornerImgGray, Size(9, 9), 2, 2);

		std::vector<Vec3f> circlesBottomCorner;

		HoughCircles(bottomCornerImgGray, circlesBottomCorner, CV_HOUGH_GRADIENT, 1, bottomCornerImgGray.rows / 8, 100, 50, 0, 0);

		if (circlesBottomCorner.size() == 1) {
			bottomCornerPoint = Point(cvRound(circlesBottomCorner[0][0]), cvRound(circlesBottomCorner[0][1]));
			tryAgain = false;
		}
		else {
			printf("Ocorreu um erro ao calibrar.\n");
		}
		if (tryAgain && count == 10) {
			tryAgain = false;
		}
		count++;
	} while (tryAgain);

	cvDestroyWindow("corners");
	//imshow("grabbed", topCornerImg);
	cvWaitKey(0);

}

void capturedImagesWebcam (int numberOfImages, string patternsPath, string saveImagesPath){

	vector<IplImage *> capturedImages(numberOfImages+2);
	vector<IplImage *> patterns(numberOfImages);

	for (int i = 0; i < numberOfImages; i++) {

		std::stringstream stringStreamPatterns;
		stringStreamPatterns << patternsPath << "\\" << i + 1 << ".jpg";
		std::string stringPath = stringStreamPatterns.str();

		//capturedImages.push_back(cvLoadImage(stringPath.c_str()));

		patterns[i] = cvLoadImage(stringPath.c_str());
		if (!patterns[i]) {
			printf("Nao carregou pattern: %s\n", stringPath);
			return;
		}
	}

	cvStartWindowThread();
	cvNamedWindow("mainWin", CV_WINDOW_NORMAL);
	cvSetWindowProperty("mainWin", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

	cvWaitKey(100);

	CvCapture* capture = cvCaptureFromCAM(CV_CAP_ANY);
	if (!capture) {
		fprintf(stderr, "Camera não encontrada \n");
		getchar();
		exit(0);
	}
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 480);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 640);

	for (int i = 0; i<numberOfImages; i++) {
		cvShowImage("mainWin", patterns[i]);
		cvWaitKey(200);
		cvGrabFrame(capture);
		capturedImages[numberOfImages + 1] = cvRetrieveFrame(capture);

		capturedImages[i] = cvCloneImage(capturedImages[numberOfImages + 1]);

		std::stringstream stringStreamCapture;
		stringStreamCapture << saveImagesPath << "\\" << i + 1 << ".jpg";
		std::string stringPathCaptured = stringStreamCapture.str();

		cvSaveImage(stringPathCaptured.c_str(), capturedImages[numberOfImages + 1]);

	}

	cvReleaseCapture(&capture);

	for (int i = 0; i<numberOfImages; i++) {
		cvShowImage("mainWin", capturedImages[i]);
		cvWaitKey(100);
	}
	cvDestroyWindow("mainWin");

}
vector<IplImage *> getFrames(int numOfImages, string filePath) {
	printf("START Reading Frames\n");

	vector<IplImage*> capImg;

	char s[128];

	for (int i = 0; i < numOfImages; i++) {
		std::stringstream stringStreamFrame;
		stringStreamFrame << filePath << "\\" << i+1 << ".jpg";
		std::string stringPath = stringStreamFrame.str();
		capImg.push_back(cvLoadImage(stringPath.c_str()));
		//capImg[i] = cvLoadImage(stringPath.c_str());

		if (!capImg[i]) {
			printf("Couldn't load img: %s\n", s);
			return capImg;
		}
	}

	cvDestroyWindow("capturedImages");
	printf("Finished reading frames\n");

	return capImg;
}


// Fonte: http://www.dspguru.com/dsp/tricks/gray-code-conversion
int gray2bin(int gray) {
	unsigned int temp = gray ^ (gray >> 8);

	// propaga o error
	if (gray == -1) return -1;

	temp ^= (temp >> 4);
	temp ^= (temp >> 2);
	temp ^= (temp >> 1);
	return temp;
}

int* reconstruct(int white_threshold, int black_threshold, vector<IplImage *> imgsCapturadas) {

	printf("Started reconstruction\n");

	// Inicializa a matriz que irá guardar os valores
	// Image data size in bytes (==image->height*image->widthStep in case of interleaved data)
	// https://docs.opencv.org/3.4.1/d6/d5b/structIplImage.html
	int *matriz = (int*)calloc(imgsCapturadas[0]->imageSize, sizeof(int));

	if (matriz == NULL) {
		printf("Erro ao inicializar matriz de pontos\n");
		return NULL;
	}

	for (int i = 0; i < imgsCapturadas.size(); i += 2) {
		printf("Processing scan patterns %d, %d\n", i, i + 1);
		uchar* data1 = (uchar*)imgsCapturadas[i]->imageData;
		uchar* data0 = (uchar*)imgsCapturadas[i + 1]->imageData;

		for (int y = 0; y<imgsCapturadas[i]->height; y++) {
			for (int x = 0; x<imgsCapturadas[i]->width; x++) {
				int u = y*imgsCapturadas[i]->widthStep + x*imgsCapturadas[i]->nChannels;
				if (matriz[u] == -1) { continue; }
				int gray1 = data1[u] + data1[u + 1] + data1[u + 2];
				int gray0 = data0[u] + data0[u + 1] + data0[u + 2];

				matriz[u] = matriz[u] << 1;
				if (gray1 - gray0 > white_threshold) {
					matriz[u] |= 1;
				} else if (gray1 - gray0 < black_threshold) {
					matriz[u] |= 0;
				} else {
					matriz[u] = -1;
				}
			}
		}
	}

	for (int i = 0; i < imgsCapturadas[0]->imageSize; i++) {
		matriz[i] = gray2bin(matriz[i]);
	}

	printf("Ended reconstruction\n");
	return matriz;
}

void generateDisparityMap(vector<IplImage *> imgsCapturadas, int *matriz, string filePath, int proj_width, int cam_width, int zScale) {

	printf("Generating disparity map\n");

	int maxNum = -1;
	int minNum = 10000000;

	int near = 300;
	int far = 530;

	for (int y = 0; y < imgsCapturadas[0]->height; y++) {
		for (int x = 0; x < imgsCapturadas[0]->width; x++) {

			int u = y * imgsCapturadas[0]->widthStep + x * imgsCapturadas[0]->nChannels;

			if (matriz[u] == -1) { continue; }

			float disp = (float)matriz[u] / (proj_width)-(float)x / cam_width;
			if (disp == 0.0) { continue; }
			float z = (float)zScale / (disp);
			if (z >= near && z <= far) {
				maxNum = max(maxNum, (int)z);
				minNum = min(minNum, (int)z);
			}
		}
	}

	Mat disparityMap(imgsCapturadas[0]->height, imgsCapturadas[0]->width, CV_8UC3, Scalar(255, 255, 255));

	for (int y = 0; y < imgsCapturadas[0]->height; y++) {
		for (int x = 0; x < imgsCapturadas[0]->width; x++) {

			int u = y * imgsCapturadas[0]->widthStep + x * imgsCapturadas[0]->nChannels;
			double disp = (float)matriz[u] / (proj_width)-(float)x / cam_width;
			//double normalized = (255.0f * (disp - minNum)) / (double)(maxNum - minNum);
			//printf("%f\n", normalized);
			double normalized = abs(disp * 255);
			disparityMap.at<Vec3b>(Point(x, y)) = Vec3b(normalized, normalized, normalized);
		}
	}
	imshow("disparity map", disparityMap);
	cvWaitKey(0);
	printf("Ended writing disparity map\n");
}


void generateXYZ(vector<IplImage *> imgsCapturadas, int *matriz, string filePath, int proj_width, int cam_width, int zScale) {
	//IplImage* capImg[18 + 1] = { 0 };

	printf("Generating nuvem XYZ\n");

	std::stringstream stringStream;
	stringStream << filePath << "\\nuvem_pontos.xyz";
	std::string stringPath = stringStream.str();

	FILE *f = fopen(stringPath.c_str(), "w");
	if (f == NULL) {
		printf("Erro ao abrir %s\nTentando diretorio local...\n", stringPath);
		FILE *f = fopen("nuvem_pontos.xyz", "w");
		if (f == NULL) {
			printf("Erro ao salvar nuvem de pontos!\n");
			return;
		}
	}

	printf("Salvando a nuvem de pontos...\n");
	for (int y = 0; y < imgsCapturadas[0]->height; y++) {
		for (int x = 0; x < imgsCapturadas[0]->width; x++) {

			int u = y * imgsCapturadas[0]->widthStep + x * imgsCapturadas[0]->nChannels;

			if (matriz[u] == -1) { continue; }

			float disp = (float)matriz[u] / (proj_width)-(float)x / cam_width;
			// 			printf("%f\n",disp);
			if (disp == 0.0) { continue; }
			float z = (float)zScale / (disp);

			fprintf(f, "%d %d %f\n", x, y, z);
		}
	}

	fclose(f);
	printf("Ended writing XYZ\n");
}


void generatePLY(vector<IplImage *> imgsCapturadas, int *matriz, string filePath, int proj_width, int cam_width, int zScale) {

	printf("Generating nuvem PLY\n");

	std::stringstream stringStream;
	stringStream << filePath << "\\nuvem_pontos.ply";
	std::string stringPath = stringStream.str();

	FILE *f = fopen(stringPath.c_str(), "w");
	if (f == NULL) {
		printf("Erro ao abrir %s\nTentando diretorio local...\n", stringPath);
		FILE *f = fopen("nuvem_pontos.ply", "w");
		if (f == NULL) {
			printf("Erro ao salvar nuvem de pontos!\n");
			return;
		}
	}

	std::stringstream headerSS;
	std::stringstream dataSS;
	int qtyOfPoints = 0;

	printf("Salvando a nuvem de pontos...\n");
	for (int y = 0; y < imgsCapturadas[0]->height; y++) {
		for (int x = 0; x < imgsCapturadas[0]->width; x++) {

			int u = y * imgsCapturadas[0]->widthStep + x * imgsCapturadas[0]->nChannels;

			if (matriz[u] == -1) { continue; }

			float disp = (float)matriz[u] / (proj_width)-(float)x / cam_width;
			// 			printf("%f\n",disp);
			if (disp == 0.0) { continue; }
			float z = (float)zScale / (disp);
			dataSS << x << " " << y << " " << z << endl;
			qtyOfPoints++;
		}
	}

	headerSS << "ply\n" <<
		"format ascii 1.0\n" <<
		"element vertex " << qtyOfPoints-1 << std::endl <<
		"property float x\n" <<
		"property float y\n" <<
		"property float z\n" <<
		"end_header\n";

	fprintf(f, "%s", headerSS.str().c_str());
	fprintf(f, "%s", dataSS.str().c_str());

	fclose(f);
	printf("Ended writing ply\n");
}

int main() {

	printf("Starting Library Menu: \n");


	int menuOption = 0;
	do {

		printf("\n\n1 - Generate Patterns\n");
		printf("2 - Get Images from camera\n");
		printf("3 - Define ROI\n");
		printf("4 - Reconstruct - Export XYZ\n");
		printf("5 - Reconstruct - Export Disparity\n");
		printf("6 - Reconstruct - Export PLY\n");
		printf("7 - Exit\n\n");

		cin >> menuOption;

		int numberOfPatterns = 0;
		//reconstruct
		int white_treshold = 0;
		int black_treshold = 0;
		int *matriz;
		//export
		int proj_width = 0;
		int cam_width = 0;
		int zScale = 0;

		vector<IplImage*> capturedImages;

		switch (menuOption) {
		case 1:
			//Generate Patterns
			printf("Choose the number of patterns [1-10]: \n");
			cin >> numberOfPatterns;
			if (numberOfPatterns >= 1 && numberOfPatterns <= 10) {
				generatePatterns(numberOfPatterns, 640, 480, "C:\\temp\\patterns");
			}
			else {
				printf("Invalid number of patterns\n");
			}
			break;
		case 2:
			//GET images from webcam
			capturedImagesWebcam(18, "c:\\temp\\patterns", "c:\\temp\\webcam");
			break;
		case 3:
				//Define ROI
				getRoi();
				break;
		case 4:
			//Export XYZ

			printf("Projector Width:\n");
			cin >> proj_width;

			printf("Camera Width:\n");
			cin >> cam_width;

			printf("Z Scale:\n");
			cin >> zScale;

			printf("White treshold:\n");
			cin >> white_treshold;

			printf("Black treshold:\n");
			cin >> black_treshold;

			capturedImages = getFrames(18, "c:\\temp\\fotos");
			matriz = reconstruct(white_treshold, black_treshold, capturedImages);
			generateXYZ(capturedImages, matriz, "C:\\temp", proj_width, cam_width, zScale);
			break;
		case 5:
			//Export Disparity

			printf("Projector Width:\n");
			cin >> proj_width;

			printf("Camera Width:\n");
			cin >> cam_width;

			printf("Z Scale:\n");
			cin >> zScale;

			printf("White treshold:\n");
			cin >> white_treshold;

			printf("Black treshold:\n");
			cin >> black_treshold;

			capturedImages = getFrames(18, "c:\\temp\\fotos");
			matriz = reconstruct(white_treshold, black_treshold, capturedImages);
			generateDisparityMap(capturedImages, matriz, "C:\\temp", proj_width, cam_width, zScale);
			break;
		case 6:
			//Export PLY

			printf("Projector Width:\n");
			cin >> proj_width;

			printf("Camera Width:\n");
			cin >> cam_width;

			printf("Z Scale:\n");
			cin >> zScale;

			printf("White treshold:\n");
			cin >> white_treshold;

			printf("Black treshold:\n");
			cin >> black_treshold;

			capturedImages = getFrames(18, "c:\\temp\\fotos");
			matriz = reconstruct(white_treshold, black_treshold, capturedImages);
			generatePLY(capturedImages, matriz, "C:\\temp", proj_width, cam_width, zScale);
			break;
		case 7:
			//Exit
			return 0;
		}

	} while (menuOption != 7);

	return 0;
}
