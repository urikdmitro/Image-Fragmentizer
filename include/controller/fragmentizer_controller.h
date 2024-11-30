#ifndef CONTROLLER_FRAGMENTIZER_CONTROLLER_H_
#define CONTROLLER_FRAGMENTIZER_CONTROLLER_H_

#include "model/fragmentizer.h"
#include "model/image.h"
#include "view/texture.h"

class FragmentizerController
{
private:
    Fragmentizer &fragmentizer;

public:
    FragmentizerController(Fragmentizer &fragmentizer);

    void SetNewImage(std::string path);
    Texture GetFragment(int fragments_count, int fragment_number) const;
    Texture GetImage() const;
};

#endif // CONTROLLER_FRAGMENTIZER_CONTROLLER_H_
