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
	//imageFile = DUNE_DATASET_BASE_PATH + "Kumtagh 1/Kumtagh 1 image.jpg";
	//proc.AddImageFile(imageFile);
	imageFile = DUNE_DATASET_BASE_PATH + "Namib 2/Namib 2 image.jpg";
	proc.AddImageFile(imageFile);
	imageFile = DUNE_DATASET_BASE_PATH + "Simpson 1/Simpson 1_image.jpg";
	proc.AddImageFile(imageFile);
	imageFile = DUNE_DATASET_BASE_PATH + "Skeleton Coast 3/Skeleton Coast 3_image.jpg";
	proc.AddImageFile(imageFile);
	imageFile = DUNE_DATASET_BASE_PATH + "White Sands 2/White Sands_2_image.jpg";
	proc.AddImageFile(imageFile);
	imageFile = DUNE_DATASET_BASE_PATH + "WDC 1/WDC_1_image.jpg";
	proc.AddImageFile(imageFile);
	proc.Process();

	return 0;
}