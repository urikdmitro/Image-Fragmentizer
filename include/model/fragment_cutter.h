#ifndef MODEL_FRAGMENT_CUTTER_H_
#define MODEL_FRAGMENT_CUTTER_H_

#include "fragment_info.h"
#include "image.h"

class IFragmentCutter
{
public:
    virtual void CutImageToFragment(
        FragmentInfo fragment_info,
        Image& image
    ) const = 0;
};


class FragmentCutter : public IFragmentCutter
{
public:
    virtual void CutImageToFragment(
        FragmentInfo fragment_info,
        Image& image
    ) const override;
};



#endif // MODEL_FRAGMENT_CUTTER_H_
