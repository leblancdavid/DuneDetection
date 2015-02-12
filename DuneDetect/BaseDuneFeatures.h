#ifndef _BASE_DUNE_FEATURES_H_
#define _BASE_DUNE_FEATURES_H_

namespace dune
{

template<class T>
class BaseDuneFeature
{
public:
	BaseDuneFeature() { }
	~BaseDuneFeature() { }
	BaseDuneFeature(const BaseDuneFeature &cpy)
	{
		Data = cpy.Data;
	}

	std::vector<T> Data;
private:

};

class DuneSegments : public BaseDuneFeature<cv::Point>
{
public:
	DuneSegments() { }
	~DuneSegments() { }
	DuneSegments(const DuneSegments &cpy)
	{
		Data = cpy.Data;
	}
private:
};

}
#endif