#include "EdgeDirectionDuneDetector.h"
#include "ConnectedComponentsExtractor.h"

namespace dune
{

#define PI 3.14159

	EdgeDirectionDuneDetector::EdgeDirectionDuneDetector()
	{
		ImageProcess = new EdgeDirectionImageProcessor();
	}

	EdgeDirectionDuneDetector::EdgeDirectionDuneDetector(EdgeDirectionImageProcessor* imgproc, EdgeDirectionDetectorParameters *params)
	{
		ImageProcess = imgproc;
		Parameters = params;
	}

	EdgeDirectionDuneDetector::~EdgeDirectionDuneDetector()
	{
		delete ImageProcess;
		delete Parameters;
	}

	void EdgeDirectionDuneDetector::SetParameters(BaseDetectorParameters *params)
	{
		Parameters = static_cast<EdgeDirectionDetectorParameters*>(params);
		ImageProcess->SetParameters(Parameters->ImageProcessParameters);
	}

	std::vector<DuneSegment> EdgeDirectionDuneDetector::Extract(const cv::Mat &img)
	{
		cv::Mat processedImage;
		ImageProcess->Process(img, processedImage);
		cv::imshow("processedImage", processedImage);
		cv::waitKey(33);

		//std::vector<DuneSegment> duneSegs = GetCannySegments(processedImage);
		std::vector<DuneSegment> duneSegs = GetContourSegments(processedImage.clone());
		FilterByEdgeDirection(processedImage, duneSegs);
		//ShiftSegmentsToGradientPeak(duneSegs, processedImage);
		//ShiftSegmentsToGradientPeak2(duneSegs, processedImage);
		//ShiftSegmentsToIntensityPeak(duneSegs, img);
		return duneSegs;
	}

	std::vector<DuneSegment> EdgeDirectionDuneDetector::GetCannySegments(const cv::Mat &img)
	{
		int k = 3;
		cv::Mat dx, dy, mag(img.rows, img.cols, CV_64F);
		cv::Sobel(img, dx, CV_64F, 1, 0, k);
		cv::Sobel(img, dy, CV_64F, 0, 1, k);

		for (int i = 0; i < img.rows; ++i)
		{
			for (int j = 0; j < img.cols; ++j)
			{
				mag.at<double>(i, j) = std::sqrt(dx.at<double>(i, j)*dx.at<double>(i, j) + dy.at<double>(i, j)*dy.at<double>(i, j));
			}
		}
		
		cv::Scalar mean, stddev;
		cv::meanStdDev(mag, mean, stddev);

		cv::Mat canny;
		cv::Canny(img, canny, mean[0]*2.0, mean[0]*6.0, k);

		std::cout << mean[0] << std::endl;
		cv::imshow("cannyImage", canny);
		cv::waitKey(100);

		imgproc::ConnectedComponentsExtractor cce;
		
		std::vector<std::vector<cv::Point>> connectedComponents = cce.GetCC(canny);

		std::vector<DuneSegment> output;
		for (int i = 0; i < connectedComponents.size(); ++i)
		{
			DuneSegment segment;
			std::vector<DuneSegmentData> segData;
			for (int j = 0; j < connectedComponents[i].size(); ++j)
			{
				segData.push_back(DuneSegmentData(connectedComponents[i][j], 0.0));
			}
			segment.SetSegmentData(segData);
			output.push_back(segment);
		}

		return output;
	}






	std::vector<DuneSegment> EdgeDirectionDuneDetector::GetContourSegments(const cv::Mat &img)
	{
		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(img, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

		std::vector<DuneSegment> output;
		for (size_t i = 0; i < contours.size(); ++i)
		{
			DuneSegment segment;
			std::vector<DuneSegmentData> data;
			for (size_t j = 0; j < contours[i].size(); ++j)
			{
				data.push_back(DuneSegmentData(contours[i][j], 0));
			}
			segment.SetSegmentData(data);
			output.push_back(segment);
		}

		return output;

	}

	void EdgeDirectionDuneDetector::FilterByEdgeDirection(const cv::Mat &input, std::vector<DuneSegment> &segments)
	{
		std::vector<DuneSegment> output;

		cv::Mat dx, dy;
		cv::Sobel(input, dx, CV_64F, 1, 0, 5);
		cv::Sobel(input, dy, CV_64F, 0, 1, 5);

		for (int i = 0; i < segments.size(); ++i)
		{
			std::vector<DuneSegment> filtered = FilterSegmentEdgeDirection(segments[i], dx, dy, Parameters->MinSegmentLength);
			for (int j = 0; j < filtered.size(); ++j)
			{
				output.push_back(filtered[j]);
			}
		}
		
		segments = output;
	}

	std::vector<DuneSegment> EdgeDirectionDuneDetector::FilterSegmentEdgeDirection(const DuneSegment &segment, const cv::Mat dx, const cv::Mat dy, int kSize)
	{
		EdgeDirectionImageProcessor* edgeImgProc = dynamic_cast<EdgeDirectionImageProcessor*>(ImageProcess);
		EdgeDirectionProcParams* imgProcParams = edgeImgProc->GetParameters();
		double domOrientation = 0.0;
		if (edgeImgProc->IsDominantOrientationComputed())
			domOrientation = edgeImgProc->GetDominantOrientation();
		else
			domOrientation = imgProcParams->Orientation;

		//domOrientation += 3.1416;
		cv::Vec2d domVec(std::cos(domOrientation), std::sin(domOrientation));

		std::vector<dune::DuneSegmentData> data = segment.GetSegmentData();
		std::vector<DuneSegment> output;
		std::vector<bool> labels;

		cv::Mat gaussian = cv::getGaussianKernel(kSize, kSize / 5.0);
		if (data.size() < kSize)
		{
			output.push_back(segment);
			return output;
		}

		for (int i = 0; i < data.size(); ++i)
		{
			int count = 0;
			cv::Vec2d r;
			r[0] = 0.0;
			r[1] = 0.0;

			for (int j = 0; j < kSize; ++j)
			{
				int k = i - kSize / 2 + j;
				if (k < 0)
					k = data.size() + k;
				else if (k >= (int)data.size())
					k = j - 1;

				r[0] += dx.at<double>(data[k].Position) * gaussian.at<double>(j, 0);
				r[1] += dy.at<double>(data[k].Position) * gaussian.at<double>(j, 0);
			}

			r /= cv::norm(r);
			double d = r[0] * domVec[0] + r[1] * domVec[1];
			if (d >= -0.4)
				labels.push_back(true);
			else
				labels.push_back(false);

			//double d = std::atan2(r[1], r[0]);
			//double low = fabs(d - domOrientation);
			//double high = fabs(d - (domOrientation + 3.1416));

			//double val = (std::min(low, high) / 3.1416);
			//if (val >= 0.5)
			//	labels.push_back(true);
			//else
			//	labels.push_back(false);

			
			//double low = fabs(d - domOrientation);
			//double high = fabs(d - (domOrientation + 3.1416));

			//double val = (std::min(low, high) / 3.1416);
			//if (val >= 0.5)
			//	labels.push_back(true);
			//else
			//	labels.push_back(false);
		}

		std::vector<dune::DuneSegmentData> segments;


		int start = 0;
		while (labels[start] && start < labels.size() - 1)
			start++;

		int i = start + 1;
		if (i >= labels.size())
			i = 0;

		while (i != start)
		{
			while (!labels[i] && i != start)
			{
				i++;

				if (i >= labels.size())
					i = 0;
			}

			while (labels[i] && i != start)
			{
				segments.push_back(data[i]);
				i++;
				if (i >= labels.size())
					i = 0;
			}
			if (segments.size() > kSize)
			{
				output.push_back(dune::DuneSegment(segments));
			}
			segments.clear();
		}

		//wrap around for the contour
		return output;
	}

	void EdgeDirectionDuneDetector::ShiftSegmentsToIntensityPeak(std::vector<DuneSegment> &segments, const cv::Mat &image)
	{
		EdgeDirectionImageProcessor* edgeImgProc = dynamic_cast<EdgeDirectionImageProcessor*>(ImageProcess);
		EdgeDirectionProcParams* imgProcParams = edgeImgProc->GetParameters();
		double domOrientation = 0.0;
		if (edgeImgProc->IsDominantOrientationComputed())
			domOrientation = edgeImgProc->GetDominantOrientation();
		else
			domOrientation = imgProcParams->Orientation;

		double searchDir = domOrientation - PI;

		for (size_t i = 0; i < segments.size(); ++i)
		{
			ShiftSegmentToOptimalGradientPeak(segments[i], imgProcParams->K, searchDir, image);
			segments[i].GaussianSmooth(imgProcParams->K);
		}
	}

	void EdgeDirectionDuneDetector::ShiftSegmentToOptimalIntensityPeak(DuneSegment &segment, int kSize, double dir, const cv::Mat &image)
	{
		bool found = false;
		double x_incr = std::cos(dir);
		double y_incr = std::sin(dir);

		double optimalIntensity = 0.0;
		int peakK = 0;
		//Find the optimal shift
		for (int k = -1; k < kSize; ++k)
		{
			double totalIntensity = 0.0;
			double count = 0.0;
			int length = segment.GetLength();
			for (int i = 0; i < length; ++i)
			{
				double x = k * x_incr + segment[i].Position.x;
				double y = k * y_incr + segment[i].Position.y;
				if (x < 0 || x >= image.cols ||
					y < 0 || y >= image.rows)
					continue;

				totalIntensity += (double)image.at<uchar>(y, x);
				count++;
			}
			totalIntensity /= count;
			if (totalIntensity > optimalIntensity)
			{
				peakK = k;
				optimalIntensity = totalIntensity;
			}
		}

		//apply the shift
		for (int i = 0; i < segment.GetLength(); ++i)
		{
			segment[i].Position.x = peakK * x_incr + segment[i].Position.x;
			segment[i].Position.y = peakK * y_incr + segment[i].Position.y;

			if (segment[i].Position.x < 0.0)
				segment[i].Position.x = 0.0;
			else if (segment[i].Position.x >= image.cols)
				segment[i].Position.x = image.cols - 1;
			if (segment[i].Position.y < 0.0)
				segment[i].Position.y = 0.0;
			else if (segment[i].Position.y >= image.rows)
				segment[i].Position.y = image.rows - 1;
		}
	}























	void EdgeDirectionDuneDetector::ShiftSegmentsToGradientPeak(std::vector<DuneSegment> &segments, const cv::Mat &domMap)
	{
		EdgeDirectionImageProcessor* edgeImgProc = dynamic_cast<EdgeDirectionImageProcessor*>(ImageProcess);
		EdgeDirectionProcParams* imgProcParams = edgeImgProc->GetParameters();
		double domOrientation = 0.0;
		if (edgeImgProc->IsDominantOrientationComputed())
			domOrientation = edgeImgProc->GetDominantOrientation();
		else
			domOrientation = imgProcParams->Orientation;

		double searchDir = domOrientation - PI;
		double x_incr = std::cos(searchDir);
		double y_incr = std::sin(searchDir);

		for (size_t i = 0; i < segments.size(); ++i)
		{
			std::vector<cv::Vec2d> shiftValues(segments[i].GetLength());

			for (int j = 0; j < segments[i].GetLength(); ++j)
			{
				Get2dShiftMagnitudeAt(segments[i][j].Position.x,
					segments[i][j].Position.y, imgProcParams->K,
					searchDir,
					shiftValues[j],
					domMap);
			}
			SmoothShifts(shiftValues, imgProcParams->K);
			for (int j = 0; j < segments[i].GetLength(); ++j)
			{
				segments[i][j].Position.x += std::ceil(shiftValues[j][0] - 0.5);
				segments[i][j].Position.y += std::ceil(shiftValues[j][1] - 0.5);

				if (segments[i][j].Position.x < 0)
					segments[i][j].Position.x = 0;
				else if (segments[i][j].Position.x >= edgeImgProc->BaseData.Gradient.cols)
					segments[i][j].Position.x = edgeImgProc->BaseData.Gradient.cols - 1;

				if (segments[i][j].Position.y < 0)
					segments[i][j].Position.y = 0;
				else if (segments[i][j].Position.y >= edgeImgProc->BaseData.Gradient.rows)
					segments[i][j].Position.y = edgeImgProc->BaseData.Gradient.rows - 1;
			}

			segments[i].GaussianSmooth(imgProcParams->K*2);
		}
	}

	void EdgeDirectionDuneDetector::ShiftSegmentsToGradientPeak2(std::vector<DuneSegment> &segments, const cv::Mat &domMap)
	{
		EdgeDirectionImageProcessor* edgeImgProc = dynamic_cast<EdgeDirectionImageProcessor*>(ImageProcess);
		EdgeDirectionProcParams* imgProcParams = edgeImgProc->GetParameters();
		double domOrientation = 0.0;
		if (edgeImgProc->IsDominantOrientationComputed())
			domOrientation = edgeImgProc->GetDominantOrientation();
		else
			domOrientation = imgProcParams->Orientation;

		double searchDir = domOrientation - PI;

		for (size_t i = 0; i < segments.size(); ++i)
		{
			ShiftSegmentToOptimalGradientPeak(segments[i], imgProcParams->K*2, searchDir, domMap);
			segments[i].GaussianSmooth(imgProcParams->K);
		}
	}

	void EdgeDirectionDuneDetector::ShiftSegmentToOptimalGradientPeak(DuneSegment &segment, int kSize, double dir, const cv::Mat &domMap)
	{
		EdgeDirectionImageProcessor* edgeImgProc = dynamic_cast<EdgeDirectionImageProcessor*>(ImageProcess);
		bool found = false;
		double x_incr = std::cos(dir);
		double y_incr = std::sin(dir);

		double optimalMag = 0.0;
		int peakK = 0;
		//Find the optimal shift
		for (int k = -1; k < kSize; ++k)
		{
			double totalMag = 0.0;
			double magCount = 0.0;
			int length = segment.GetLength();
			for (int i = 0; i < length; ++i)
			{
				double x = k * x_incr + segment[i].Position.x;
				double y = k * y_incr + segment[i].Position.y;
				if (x < 0 || x >= edgeImgProc->BaseData.Gradient.cols ||
					y < 0 || y >= edgeImgProc->BaseData.Gradient.rows)
					continue;
				
				/*if (domMap.at<uchar>(y, x) > 0){
					magCount++;
					totalMag += edgeImgProc->BaseData.getMagnitudeAt(x, y) / 1000.0;
				}*/
				magCount++;
				totalMag += edgeImgProc->BaseData.getMagnitudeAt(x, y) / 1000.0;
			
			}
			totalMag /= magCount;
			if (totalMag > optimalMag)
			{
				peakK = k;
				optimalMag = totalMag;
			}
		}

		//apply the shift
		for (int i = 0; i < segment.GetLength(); ++i)
		{
			segment[i].Position.x = peakK * x_incr + segment[i].Position.x;
			segment[i].Position.y = peakK * y_incr + segment[i].Position.y;

			if (segment[i].Position.x < 0.0)
				segment[i].Position.x = 0.0;
			else if (segment[i].Position.x >= edgeImgProc->BaseData.Gradient.cols)
				segment[i].Position.x = edgeImgProc->BaseData.Gradient.cols - 1;
			if (segment[i].Position.y < 0.0)
				segment[i].Position.y = 0.0;
			else if (segment[i].Position.y >= edgeImgProc->BaseData.Gradient.rows)
				segment[i].Position.y = edgeImgProc->BaseData.Gradient.rows - 1;
		}
	}

	void EdgeDirectionDuneDetector::SmoothShifts(std::vector<cv::Vec2d> &shift, int kSize)
	{
		if (shift.size() < kSize)
			return;

		std::vector<cv::Vec2d> processed(shift.size());
		cv::Mat gausK = cv::getGaussianKernel(kSize, kSize / 5.0);

		for (int i = 0; i < shift.size(); ++i)
		{
			int count = 0;
			double x = 0.0;
			double y = 0.0;
			for (int j = 0; j < kSize; ++j)
			{
				int k = i - kSize / 2 + j;
				if (k < 0)
					k = shift.size() + k;
				else if (k >= (int)shift.size())
					k = j - 1;

				x += shift[k][0] * gausK.at<double>(j, 0);
				y += shift[k][1] * gausK.at<double>(j, 0);
			}

			processed[i][0] = x;
			processed[i][1] = y;
		}

		shift = processed;
	}

	bool EdgeDirectionDuneDetector::Get2dShiftPeakAt(double x, double y, int distance, double dir, cv::Vec2d &shift)
	{
		EdgeDirectionImageProcessor* edgeImgProc = dynamic_cast<EdgeDirectionImageProcessor*>(ImageProcess);
		bool found = false;
		double x_incr = std::cos(dir);
		double y_incr = std::sin(dir);

		shift = cv::Vec2d(0.0, 0.0);
		std::vector<double> gradVals(distance + 1);
		for (int k = -1; k < distance; ++k)
		{
			double xshift = x + k * x_incr;
			double yshift = y + k * y_incr;

			if (xshift < 0 || xshift >= edgeImgProc->BaseData.Gradient.cols ||
				yshift < 0 || yshift >= edgeImgProc->BaseData.Gradient.rows)
				break;

			gradVals[k+1] = edgeImgProc->BaseData.getMagnitudeAt(xshift, yshift);
		}
		double maxGrad = 0.0;
		for (int k = 1; k < gradVals.size() - 1; ++k)
		{
			//Stop at the first peak
			if (gradVals[k] >= gradVals[k - 1] &&
				gradVals[k] >= gradVals[k + 1])
			{
				shift[0] = (k - 1) * x_incr;
				shift[1] = (k - 1) * y_incr;
				found = true;
				break;
			}
		}

		return found;
	}

	bool EdgeDirectionDuneDetector::Get2dShiftMagnitudeAt(double x, double y, int sizeK, double dir, cv::Vec2d &shift, const cv::Mat &domMap)
	{
		EdgeDirectionImageProcessor* edgeImgProc = dynamic_cast<EdgeDirectionImageProcessor*>(ImageProcess);
		bool found = false;
		double x_incr = std::cos(dir);
		double y_incr = std::sin(dir);

		shift = cv::Vec2d(0.0, 0.0);
		double maxMag = -999999.9;
		for (int k = -1; k < sizeK; ++k)
		{
			double xshift = x + k * x_incr;
			double yshift = y + k * y_incr;

			if (xshift < 0 || xshift >= edgeImgProc->BaseData.Gradient.cols ||
				yshift < 0 || yshift >= edgeImgProc->BaseData.Gradient.rows)
				continue;

			double mag = edgeImgProc->BaseData.getMagnitudeAt(xshift, yshift);

			if (mag > maxMag && domMap.at<uchar>(yshift, xshift) > 0)
			{
				maxMag = mag;
				shift[0] = k * x_incr;
				shift[1] = k * y_incr;
				found = true;
			}
		}

		return found;
	}
}