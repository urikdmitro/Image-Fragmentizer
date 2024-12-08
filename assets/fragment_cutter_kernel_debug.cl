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

    if (x == 1 && y == 1)
    {
        printf(
            "width: %d, height: %d, channels: %d, lower_bound: %d,"
            " upper_bound: %d, nonfragment_value: %d\n",
            width,
            height,
            channels,
            lower_bound,
            upper_bound,
            nonfragment_pixel_value
        );
    }

    for (int c = 0; c < channels; c++) {
        if ((channels_to_fragmentize & (1 << c)) == 0) {
            return;
        }

        int pixel_index = (y * width + x) * channels + c;

        uchar pixel_value = image_data[pixel_index];
        if (pixel_value < lower_bound || pixel_value > upper_bound) {
            image_data[pixel_index] = nonfragment_pixel_value;
        }
    }
}
