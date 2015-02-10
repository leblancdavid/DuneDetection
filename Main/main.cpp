#define _CRT_SECURE_NO_WARNINGS

#include <iostream>

const std::string DUNE_DATASET_BASE_PATH = "C:/Users/David/Documents/Research/DuneDetect/DuneDataset/";

#include "DatasetProcessor.h"

int main()
{
	std::string imageFile = DUNE_DATASET_BASE_PATH;
	imageFile += "Kalahari 3/Kalahari 3 image.jpg";

	DatasetProcessor proc;
	proc.AddImageFile(imageFile);
	proc.Process();

	return 0;
}