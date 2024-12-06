kernel void cut_image_to_fragments(
    global uchar* image_data,
    const uint width,
    const uint height,
    const uint channels,
    const uint lower_bound,
    const uint upper_bound,
    const uchar nonfragment_pixel_value,
    global const uchar* channels_to_fragmentize,
    const uint num_channels_to_fragmentize
) {
    int x = get_global_id(0);
    int y = get_global_id(1);

    if (x >= width || y >= height) return; // Ensure we don't go out of bounds

    printf("Processing pixel (%d, %d), channels array size: %d, channels: %d, lower: %d, upper: %d \n", x, y, num_channels_to_fragmentize, channels, lower_bound, upper_bound);

    for (int c = 0; c < channels; c++) {
        uint channel = channels_to_fragmentize[c];

        if (channel >= channels) {
            printf("Invalid channel index: %d\n", channel);
            return;
        }

        int pixel_index = (y * width + x) * channels + channel;
        printf("Pixel index: %d\n", pixel_index);

        uchar pixel_value = image_data[pixel_index];
        if (pixel_value < lower_bound || pixel_value > upper_bound) {
            printf("Out of bounds pixel value: %d at index %d\n", pixel_value, pixel_index);
            image_data[pixel_index] = nonfragment_pixel_value;
        }
    }
}
