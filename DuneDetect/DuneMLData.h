#pragma once
#include <string>

namespace duneML
{
	class DuneMLData
	{
	public:
		DuneMLData(){}
		DuneMLData(const DuneMLData &cpy)
		{
			ImageFileName = cpy.ImageFileName;
			GroundTruthFileName = cpy.GroundTruthFileName;
			SunOrientation = cpy.SunOrientation;
		}
		~DuneMLData(){}
		DuneMLData(std::string imageFile, std::string groundTruthFile, double orientation)
		{
			SetData(imageFile, groundTruthFile, orientation);
		}
		
		void SetData(std::string imageFile, std::string groundTruthFile, double orientation)
		{
			ImageFileName = imageFile;
			GroundTruthFileName = groundTruthFile;
			SunOrientation = orientation;
		}

		std::string ImageFileName;
		std::string GroundTruthFileName;
		double SunOrientation;

	private:
		


	};
}