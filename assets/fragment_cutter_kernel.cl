kernel void cut_image_to_fragments(
    global uchar* image_data,
    const uint width,
    const uint height,
    const uint channels,
    const uint lower_bound,
    const uint upper_bound,
    const uchar nonfragment_pixel_value,
    const uchar channels_to_fragmentize
) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    int pixel_index = (y * width + x) * channels;

    for (int c = 0; c < channels; c++) {
        if ((channels_to_fragmentize & (1 << c)) == 0)
        {
            continue;
        }

        int channel_index = pixel_index + c;

        uchar pixel_value = image_data[channel_index];

        if (pixel_value < lower_bound || pixel_value > upper_bound)
        {
            image_data[channel_index] = nonfragment_pixel_value;
        }
    }
}
