#include "base.h"
#include "matrix.h"
#include "feature_window.h"

int32_t window_shift_get(struct frame_option_t frame_opt)
{
    return frame_opt.sample_rate * 0.001 * frame_opt.frame_shift_ms;
}

int32_t window_size_get(struct frame_option_t frame_opt)
{
    return frame_opt.sample_rate * 0.001 * frame_opt.frame_length_ms;
}

int32_t round_to_power_of_two_near(int32_t n)
{
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n + 1;
}

int32_t padding_window_size(struct frame_option_t frame_opt)
{
    return frame_opt.round_to_power_of_two ?  
            round_to_power_of_two_near(window_size_get(frame_opt)):window_size_get(frame_opt);
}

int64_t first_sample_frame(int32_t frame, struct frame_option_t frame_opt)
{
    int64_t frame_shift = window_shift_get(frame_opt);
    if(frame_opt.snip_edges)
    {
        return frame * frame_shift;
    }
    else
    {
        int64_t midpoint_of_frame = frame_shift * frame + frame_shift / 2;
        int64_t beginning_of_frame = midpoint_of_frame - window_size_get(frame_opt) / 2;
        return beginning_of_frame;
    }
}

int32_t window_num_frames(int64_t num_samples, struct frame_option_t frame_opt, bool flush)
{
    int64_t frame_shift = window_shift_get(frame_opt);
    int64_t frame_length = window_size_get(frame_opt);

    if(frame_opt.snip_edges)
    {
        /*
         *
         */
        if(num_samples < frame_length)
            return 0;
        else
            return (1 + ((num_samples - frame_length) / frame_shift));
    }
    else
    {
        /*
         *
         *
         */
        int32_t num_frames = (num_samples + (frame_shift / 2)) / frame_shift;

        if(flush)
            return num_frames;

        int64_t end_sample_of_last_frame = first_sample_frame(num_frames - 1, frame_opt) 
                                                                + frame_length;
        
        while(num_frames > 0 && end_sample_of_last_frame > num_samples)
        {
            num_frames --;
            end_sample_of_last_frame -= frame_shift;
        }
        return num_frames;
    }
}

void frame_option_init(struct frame_option_t *frame_opt)
{
    if(frame_opt)
    {
        frame_opt->sample_rate = 16000;
        frame_opt->frame_shift_ms = 10.0;
        frame_opt->frame_length_ms = 25.0;
        frame_opt->dither = 1.0;;
        frame_opt->preemph_coeff = 0.97;
        frame_opt->remove_dc_offset = true;
        strcpy(frame_opt->window_type, "povey");
        frame_opt->round_to_power_of_two = true;
        frame_opt->blackman_coeff = 0.42;
        frame_opt->snip_edges = true;
        frame_opt->allow_downsample = false;
        frame_opt->allow_upsample = false;
        frame_opt->max_feature_vectors = -1;
    }
}

void window_compute(int64_t sample_offset, matrix_t *wave, int32_t f, 
                struct frame_option_t frame_opt, matrix_t *window, float *log_energy_pre_window)
{
    

}
