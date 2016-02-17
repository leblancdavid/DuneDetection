#pragma once
#include <iostream>
#include <fstream>
#include <time.h>
#include <algorithm>

std::ofstream GetTimeStampedFileStream(const std::string &path, const std::string &filename)
{
	time_t rawtime;
	time(&rawtime);
	std::string timestamp = ctime(&rawtime);
	timestamp.erase(timestamp.end() - 1, timestamp.end());
	std::replace(timestamp.begin(), timestamp.end(), ':', '-');
	timestamp.erase(std::remove(timestamp.begin(), timestamp.end(), ' '), timestamp.end());
	//std::string filename = DUNE_DATASET_BASE_PATH + "Results/EdgeBased - " + timestamp + ".txt";
	//std::string prFile = DUNE_DATASET_BASE_PATH + "Results/EdgeBased_PR - " + timestamp + ".txt";
	std::string ofile = path + filename + timestamp + ".txt";

	std::ofstream logfile;
	logfile.open(ofile);

	return logfile;
}